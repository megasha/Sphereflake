#include "BBox.h"
#include "Ray.h"
#include <algorithm>

BBox::BBox(Vector3 cMin, Vector3 cMax, Objects o) :
min(cMin), max(cMax), complex_objects(o)
{
	leaf = false;
}

BBox::BBox(Vector3 cMin, Vector3 cMax, Object *o, bool b) :
min(cMin), max(cMax), leaf(b)
{
	complex_objects.push_back(o);
}


BBox::~BBox()
{
	leaf = false;
}

void 
BBox::renderGL()
{
	//Front
	glBegin(GL_POLYGON);
	glVertex3f(max.x, max.y, max.z);
	glVertex3f(min.x, max.y, max.z);
	glVertex3f(min.x, min.y, max.z);
	glVertex3f(max.x, min.y, max.z);
	glEnd();

	//Back
	glBegin(GL_POLYGON);
	glVertex3f(min.x, max.y, min.z);
	glVertex3f(max.x, max.y, min.z);
	glVertex3f(max.x, min.y, min.z);
	glVertex3f(min.x, min.y, min.z);
	glEnd();

	//Left
	glBegin(GL_POLYGON);
	glVertex3f(min.x, max.y, max.z);
	glVertex3f(min.x, max.y, min.z);
	glVertex3f(min.x, min.y, min.z);
	glVertex3f(min.x, min.y, max.z);
	glEnd();

	//Right
	glBegin(GL_POLYGON);
	glVertex3f(max.x, max.y, min.z);
	glVertex3f(max.x, max.y, max.z);
	glVertex3f(max.x, min.y, max.z);
	glVertex3f(max.x, min.y, min.z);
	glEnd();

	//Top
	glBegin(GL_POLYGON);
	glVertex3f(max.x, max.y, min.z);
	glVertex3f(min.x, max.y, min.z);
	glVertex3f(min.x, max.y, max.z);
	glVertex3f(max.x, max.y, max.z);
	glEnd();

	//Bottom
	glBegin(GL_POLYGON);
	glVertex3f(max.x, min.y, max.z);
	glVertex3f(min.x, min.y, max.z);
	glVertex3f(min.x, min.y, min.z);
	glVertex3f(max.x, min.y, min.z);
	glEnd();


}

bool 
BBox::intersect(HitInfo& result, const Ray& r, float tMin, float tMax) {
	float xMin, yMin, zMin, xMax, yMax, zMax;
	float currMin, currMax;

	xMin = (min.x - r.o.x) / r.d.x;
	yMin = (min.y - r.o.y) / r.d.y;
	zMin = (min.z - r.o.z) / r.d.z;

	xMax = (max.x - r.o.x) / r.d.x;
	yMax = (max.y - r.o.y) / r.d.y;
	zMax = (max.z - r.o.z) / r.d.z;

	currMin = std::max(std::max(std::min(xMin, xMax), std::min(yMin, yMax)), std::min(zMin, zMax));
	currMax = std::min(std::min(std::max(xMin, xMax), std::max(yMin, yMax)), std::max(zMin, zMax));

	if (currMin <= currMax) {
		result.t = currMin;
		result.P = r.o + result.t*r.d;

		//Add Material??
		return true;
	}

	return false;
}

/*
Vector3 
BBox::getCenter(){
	return (max - min) / 2.0f; 
}
*/

bool
BBox::bvhIntersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	bool hit = false;
	HitInfo tempMinHit;
	minHit.t = MIRO_TMAX;

	for (size_t i = 0; i < complex_objects.size(); ++i)
	{
		if (complex_objects[i]->intersect(tempMinHit, ray, tMin, tMax))
		{
			hit = true;
			if (tempMinHit.t < minHit.t)
				minHit = tempMinHit;
		}
	}

	return hit;
}

void BBox::split(Objects *gl_objects) {
	unsigned int numBins = 10;
	float costX, costY, costZ;

	Vector3 range;
	range = max - min;
	range = Vector3(abs(range[0]), abs(range[1]), abs(range[2]));

	std::vector<Objects> binX;
	std::vector<Objects> binY;
	std::vector<Objects> binZ;
	binX.resize(numBins);
	binY.resize(numBins);
	binZ.resize(numBins);

	if (complex_objects.empty()) return;

	//Bin by X component
	unsigned int binIndex = 0;
	Vector3 tempCenter;
	for (size_t i = 0; i < complex_objects.size(); ++i)
	{
		tempCenter = complex_objects[i]->getCenter();

		binIndex = (tempCenter[0] - min[0]) / (range[0] / numBins);

		if (binIndex == numBins) binIndex--;

		binX[binIndex].push_back(complex_objects[i]);
	}

	//Bin by Y component
	for (size_t i = 0; i < complex_objects.size(); ++i)
	{
		tempCenter = complex_objects[i]->getCenter();

		binIndex = (tempCenter[1] - min[1]) / (range[1] / numBins);

		if (binIndex == numBins) binIndex--;

		binY[binIndex].push_back(complex_objects[i]);
	}

	//Bin by Z component
	for (size_t i = 0; i < complex_objects.size(); ++i)
	{
		tempCenter = complex_objects[i]->getCenter();

		binIndex = (tempCenter[2] - min[2]) / (range[2] / numBins);

		if (binIndex == numBins) binIndex--;

		binZ[binIndex].push_back(complex_objects[i]);
	}

	//Compute cost of X
	unsigned int leftTriangles = 0;
	unsigned int rightTriangles = 0;
	unsigned int splitX = 0;
	float binSizeX = range[0] / numBins;
	float tempCostX = 0;


	//X: Initial cost setup
	leftTriangles = binX[0].size();
	for (int i = 1; i < numBins; i++)
		rightTriangles += binX[i].size();

	costX = (leftTriangles * binSizeX) + (rightTriangles * binSizeX*(numBins - 1));
	splitX = 0;
	rightTriangles = 0;

	//X: Recursive cost setup
	for (int i = 1; i < numBins-1; i++) {

		leftTriangles += binX[i].size();

		for (int j = i+1; j < numBins; j++) {
			rightTriangles += binX[j].size();
		}

		tempCostX = (leftTriangles * binSizeX*(i + 1)) + (rightTriangles * binSizeX*(numBins - i - 1));

		if (tempCostX < costX) {
			costX = tempCostX;
			splitX = i;
		}

		rightTriangles = 0;
	}

	leftTriangles = 0;
	rightTriangles = 0;
	unsigned int splitY = 0;
	float binSizeY = range[1] / numBins;
	float tempCostY = 0;

	//Y: Initial cost setup
	leftTriangles = binY[0].size();
	for (int i = 1; i < numBins; i++)
		rightTriangles += binY[i].size();

	costY = (leftTriangles * binSizeY) + (rightTriangles* binSizeY *(numBins - 1));
	splitY = 0;
	rightTriangles = 0;

	//Y: Recrusive cost setup
	for (int i = 1; i < numBins - 1; i++) {
		leftTriangles += binY[i].size();

		for (int j = i + 1; j < numBins; j++) {
			rightTriangles += binY[j].size();
		}

		tempCostY = (leftTriangles * binSizeY*(i + 1)) + (rightTriangles * binSizeY*(numBins - i - 1));

		if (tempCostY < costY) {
			costY = tempCostY;
			splitY = i;
		}
		rightTriangles = 0;
	}

	leftTriangles = 0;
	rightTriangles = 0;
	unsigned int splitZ = 0;
	float binSizeZ = range[2] / numBins;
	float tempCostZ = 0;

	//Z: Initial cost setup
	leftTriangles = binZ[0].size();
	for (int i = 1; i < numBins; i++)
		rightTriangles += binZ[i].size();

	costZ = (leftTriangles * binSizeZ) + (rightTriangles* binSizeZ *(numBins - 1));
	splitZ = 0;
	rightTriangles = 0;

	//Z: Recrusive cost setup
	for (int i = 1; i < numBins - 1; i++) {
		leftTriangles += binZ[i].size();

		for (int j = i + 1; j < numBins; j++) {
			rightTriangles += binZ[j].size();
		}

		tempCostZ = (leftTriangles * binSizeZ*(i + 1)) + (rightTriangles * binSizeZ*(numBins - i - 1));

		if (tempCostZ < costZ) {
			costZ = tempCostZ;
			splitZ = i;
		}
		rightTriangles = 0;
	}

	float cost = costX;
	unsigned int axisCost = 0;

	if (costY < cost) {
		cost = costY;
		axisCost = 1;
	}
	else if (costZ < cost) {
		cost = costZ;
		axisCost = 2;
	}

	unsigned int leftMin = numBins + 1;
	unsigned int leftMax = numBins + 1;
	unsigned int rightMin = numBins + 1;
	unsigned int rightMax = numBins + 1;

	if (axisCost == 0) {
		for (int i = 0; i < splitX; i++)
			if (!binX[i].empty()) {
				leftMin = i; 
				break;
			}

		for (int i = splitX; i >= 0; i--) 
			if (!binX[i].empty()) {
				leftMax = i; 
				break;
			}

		for (int i = splitX + 1; i < numBins; i++)
			if (!binX[i].empty()) {
				rightMin = i;
				break;
			}

		for (int i = numBins - 1; i >= splitX + 1;i--)
			if (!binX[i].empty()) {
				rightMax = i;
				break;
			}
	}

	else if (axisCost == 1) {
		for (int i = 0; i < splitY; i++)
			if (!binY[i].empty()) {
				leftMin = i;
				break;
			}

		for (int i = splitY; i >= 0; i--)
			if (!binY[i].empty()) {
				leftMax = i;
				break;
			}

		for (int i = splitY + 1; i < numBins; i++)
			if (!binY[i].empty()) {
				rightMin = i;
				break;
			}

		for (int i = numBins - 1; i >= splitY + 1; i--)
			if (!binY[i].empty()) {
				rightMax = i;
				break;
			}
	}

	else {
		for (int i = 0; i < splitZ; i++)
			if (!binZ[i].empty()) {
				leftMin = i;
				break;
			}

		for (int i = splitZ; i >= 0; i--)
			if (!binZ[i].empty()) {
				leftMax = i;
				break;
			}

		for (int i = splitZ + 1; i < numBins; i++)
			if (!binZ[i].empty()) {
				rightMin = i;
				break;
			}

		for (int i = numBins - 1; i >= splitZ + 1; i--)
			if (!binZ[i].empty()) {
				rightMax = i;
				break;
			}
	}

	//THIS PART IS FUCKING HARD

	Vector3 minLeft, minRight, maxLeft, maxRight;

	if (axisCost == 0) {
		minLeft = binX[leftMin][0]->getMin();
		maxLeft = binX[leftMax][0]->getMax();
		minRight = binX[rightMin][0]->getMin();
		maxRight = binX[rightMax][0]->getMax();

		for (int i = 0; i < binX[leftMin].size(); i++) {
			if (minLeft.x > binX[leftMin][i]->getMin().x)  minLeft.x = binX[leftMin][i]->getMin().x;
			if (minLeft.y > binX[leftMin][i]->getMin().y)  minLeft.y = binX[leftMin][i]->getMin().y;
			if (minLeft.z > binX[leftMin][i]->getMin().z)  minLeft.z = binX[leftMin][i]->getMin().z;
		}
		for (int i = 0; i < binX[leftMax].size(); i++) {
			if (maxLeft.x < binX[leftMax][i]->getMax().x)  maxLeft.x = binX[leftMax][i]->getMax().x;
			if (maxLeft.y < binX[leftMax][i]->getMax().y)  maxLeft.y = binX[leftMax][i]->getMax().y;
			if (maxLeft.z < binX[leftMax][i]->getMax().z)  maxLeft.z = binX[leftMax][i]->getMax().z;
		}

		for (int i = 0; i < binX[rightMin].size(); i++) {
			if (minRight.x > binX[rightMin][i]->getMin().x)  minRight.x = binX[rightMin][i]->getMin().x;
			if (minRight.y > binX[rightMin][i]->getMin().y)  minRight.y = binX[rightMin][i]->getMin().y;
			if (minRight.z > binX[rightMin][i]->getMin().z)  minRight.z = binX[rightMin][i]->getMin().z;
		}
		for (int i = 0; i < binX[rightMax].size(); i++) {
			if (maxRight.x < binX[rightMax][i]->getMax().x)  maxRight.x = binX[rightMax][i]->getMax().x;
			if (maxRight.y < binX[rightMax][i]->getMax().y)  maxRight.y = binX[rightMax][i]->getMax().y;
			if (maxRight.z < binX[rightMax][i]->getMax().z)  maxRight.z = binX[rightMax][i]->getMax().z;
		}
	}

	else if (axisCost == 1) {
		minLeft = binY[leftMin][0]->getMin();
		maxLeft = binY[leftMax][0]->getMax();
		minRight = binY[rightMin][0]->getMin();
		maxRight = binY[rightMax][0]->getMax();

		for (int i = 0; i < binY[leftMin].size(); i++) {
			if (minLeft.x > binY[leftMin][i]->getMin().x)  minLeft.x = binY[leftMin][i]->getMin().x;
			if (minLeft.y > binY[leftMin][i]->getMin().y)  minLeft.y = binY[leftMin][i]->getMin().y;
			if (minLeft.z > binY[leftMin][i]->getMin().z)  minLeft.z = binY[leftMin][i]->getMin().z;
		}
		for (int i = 0; i < binY[leftMax].size(); i++) {
			if (maxLeft.x < binY[leftMax][i]->getMax().x)  maxLeft.x = binY[leftMax][i]->getMax().x;
			if (maxLeft.y < binY[leftMax][i]->getMax().y)  maxLeft.y = binY[leftMax][i]->getMax().y;
			if (maxLeft.z < binY[leftMax][i]->getMax().z)  maxLeft.z = binY[leftMax][i]->getMax().z;
		}

		for (int i = 0; i < binY[rightMin].size(); i++) {
			if (minRight.x > binY[rightMin][i]->getMin().x)  minRight.x = binY[rightMin][i]->getMin().x;
			if (minRight.y > binY[rightMin][i]->getMin().y)  minRight.y = binY[rightMin][i]->getMin().y;
			if (minRight.z > binY[rightMin][i]->getMin().z)  minRight.z = binY[rightMin][i]->getMin().z;
		}
		for (int i = 0; i < binY[rightMax].size(); i++) {
			if (maxRight.x < binY[rightMax][i]->getMax().x)  maxRight.x = binY[rightMax][i]->getMax().x;
			if (maxRight.y < binY[rightMax][i]->getMax().y)  maxRight.y = binY[rightMax][i]->getMax().y;
			if (maxRight.z < binY[rightMax][i]->getMax().z)  maxRight.z = binY[rightMax][i]->getMax().z;
		}
	}

	else {
		minLeft = binZ[leftMin][0]->getMin();
		maxLeft = binZ[leftMax][0]->getMax();
		minRight = binZ[rightMin][0]->getMin();
		maxRight = binZ[rightMax][0]->getMax();

		for (int i = 0; i < binZ[leftMin].size(); i++) {
			if (minLeft.x > binZ[leftMin][i]->getMin().x)  minLeft.x = binZ[leftMin][i]->getMin().x;
			if (minLeft.y > binZ[leftMin][i]->getMin().y)  minLeft.y = binZ[leftMin][i]->getMin().y;
			if (minLeft.z > binZ[leftMin][i]->getMin().z)  minLeft.z = binZ[leftMin][i]->getMin().z;
		}
		for (int i = 0; i < binZ[leftMax].size(); i++) {
			if (maxLeft.x < binZ[leftMax][i]->getMax().x)  maxLeft.x = binZ[leftMax][i]->getMax().x;
			if (maxLeft.y < binZ[leftMax][i]->getMax().y)  maxLeft.y = binZ[leftMax][i]->getMax().y;
			if (maxLeft.z < binZ[leftMax][i]->getMax().z)  maxLeft.z = binZ[leftMax][i]->getMax().z;
		}

		for (int i = 0; i < binZ[rightMin].size(); i++) {
			if (minRight.x > binZ[rightMin][i]->getMin().x)  minRight.x = binZ[rightMin][i]->getMin().x;
			if (minRight.y > binZ[rightMin][i]->getMin().y)  minRight.y = binZ[rightMin][i]->getMin().y;
			if (minRight.z > binZ[rightMin][i]->getMin().z)  minRight.z = binZ[rightMin][i]->getMin().z;
		}
		for (int i = 0; i < binZ[rightMax].size(); i++) {
			if (maxRight.x < binZ[rightMax][i]->getMax().x)  maxRight.x = binZ[rightMax][i]->getMax().x;
			if (maxRight.y < binZ[rightMax][i]->getMax().y)  maxRight.y = binZ[rightMax][i]->getMax().y;
			if (maxRight.z < binZ[rightMax][i]->getMax().z)  maxRight.z = binZ[rightMax][i]->getMax().z;
		}
	}

	//Populate left and right leaf boxes
	Objects leftChildren, rightChildren;
	if (axisCost == 0) {
		for (int i = 0; i < splitX; i++) {
			for (int j = 0; j < binX[i].size(); j++) {
				leftChildren.push_back(binX[i][j]);
			}
		}
		for (int i = splitX+1; i < numBins; i++) {
			for (int j = 0; j < binX[i].size(); j++) {
				rightChildren.push_back(binX[i][j]);
			}
		}
	}
	else if (axisCost == 1) {
		for (int i = 0; i < splitY; i++) {
			for (int j = 0; j < binY[i].size(); j++) {
				leftChildren.push_back(binY[i][j]);
			}
		}
		for (int i = splitY + 1; i < numBins; i++) {
			for (int j = 0; j < binY[i].size(); j++) {
				rightChildren.push_back(binY[i][j]);
			}
		}
	}
	else {
		for (int i = 0; i < splitZ; i++) {
			for (int j = 0; j < binZ[i].size(); j++) {
				leftChildren.push_back(binZ[i][j]);
			}
		}
		for (int i = splitZ + 1; i < numBins; i++) {
			for (int j = 0; j < binZ[i].size(); j++) {
				rightChildren.push_back(binZ[i][j]);
			}
		}
	}

	//Create two child boxes
	BBox *leftBox = new BBox(minLeft, maxLeft, leftChildren);
	BBox *rightBox = new BBox(minRight, maxRight, rightChildren);

	gl_objects->push_back(leftBox);
	gl_objects->push_back(rightBox);

}