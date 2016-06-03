#include "BBox.h"
#include "Ray.h"
#include <algorithm>
#include <limits>
#include <queue>

BBox::BBox(Vector3 center, float length) {
	leftBox = nullptr;
	rightBox = nullptr;
	float halfLength = length / 2.0f;
	min = Vector3(center.x - halfLength, center.y+2.0f , center.z - halfLength);
	max = Vector3(center.x + halfLength, center.y+3.0f, center.z + halfLength);
}

BBox::BBox(Vector3 cMin, Vector3 cMax, Objects o) :
min(cMin), max(cMax), complex_objects(o), leaf(true)
{
	leftBox = nullptr;
	rightBox = nullptr;
}

BBox::BBox(Vector3 cMin, Vector3 cMax, Object *o) :
min(cMin), max(cMax), leaf(true)
{
	leftBox = nullptr;
	rightBox = nullptr;
	complex_objects.push_back(o);
}


BBox::~BBox()
{
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
BBox::intersect(HitInfo& result, const Ray& r, unsigned int &bCount, unsigned int &tCount, float tMin, float tMax) {
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
		result.material = m_material;
		//Add Material??
		bCount++;
		return true;
	}

	return false;
}

float
BBox::getCost(unsigned int &lTriangles, unsigned int &rTriangles, Vector3 &lMin, Vector3 &lMax, Vector3 &rMin, Vector3 &rMax){
	float ret = 0;

	Vector3 lRange = lMax - lMin;
	lRange = Vector3(abs(lRange[0]), abs(lRange[1]), abs(lRange[2]));

	Vector3 rRange = rMax - rMin;
	rRange = Vector3(abs(rRange[0]), abs(rRange[1]), abs(rRange[2]));

	float saLeft = 2 * ((lRange[0] * lRange[1]) + (lRange[1] * lRange[2]) + (lRange[0] * lRange[2]));
	float saRight = 2 * ((rRange[0] * rRange[1]) + (rRange[1] * rRange[2]) + (rRange[0] * rRange[2]));

	ret = (saLeft * lTriangles) + (saRight * rTriangles);
	return ret;
}

void BBox::split(Objects *gl_objects, std::queue<BBox*> &splitQueue, unsigned int recurse) {
	unsigned int numBins = 16;
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
		if ((tempCenter[0] - min[0]) < 0) binIndex = 0;
		else if (binIndex > numBins) binIndex = numBins - 1;


		binX[binIndex].push_back(complex_objects[i]);
	}

	//Bin by Y component
	for (size_t i = 0; i < complex_objects.size(); ++i)
	{
		tempCenter = complex_objects[i]->getCenter();

		binIndex = (tempCenter[1] - min[1]) / (range[1] / numBins);

		if (binIndex == numBins) binIndex--;
		if ((tempCenter[1] - min[1]) < 0) binIndex = 0;
		else if (binIndex > numBins) binIndex = numBins - 1;

		binY[binIndex].push_back(complex_objects[i]);
	}

	//Bin by Z component
	for (size_t i = 0; i < complex_objects.size(); ++i)
	{
		tempCenter = complex_objects[i]->getCenter();

		binIndex = (tempCenter[2] - min[2]) / (range[2] / numBins);

		if (binIndex == numBins) binIndex--;
		if ((tempCenter[2] - min[2]) < 0) binIndex = 0;
		else if (binIndex > numBins) binIndex = numBins - 1;


		binZ[binIndex].push_back(complex_objects[i]);
	}

	//Compute cost of X
	Objects leftObjects;
	Objects rightObjects;
	unsigned int leftTriangles = 0;
	unsigned int rightTriangles = 0;
	unsigned int splitX = 0;
	float binSizeX = range[0] / numBins;
	float tempCostX = 0;

	Vector3 leftMin, leftMax, rightMin, rightMax;

	//X: Initial cost setup
	costX = std::numeric_limits<float>::max();
	splitX = 0;
	rightTriangles = 0;

	//X: Recursive cost setup
	leftObjects.clear();
	rightObjects.clear();
	for (int i = 0; i < numBins-1; i++) {

		leftTriangles += binX[i].size();
		for (int j = 0; j < binX[i].size(); j++)
			leftObjects.push_back(binX[i][j]);

		for (int j = i+1; j < numBins; j++) {
			rightTriangles += binX[j].size();
			for (int k = 0; k < binX[j].size(); k++)
				rightObjects.push_back(binX[j][k]);
		}

		getBBox(leftObjects, leftMin, leftMax);
		getBBox(rightObjects, rightMin, rightMax);
		tempCostX = getCost(leftTriangles, rightTriangles, leftMin, leftMax, rightMin, rightMax);

		if (tempCostX < costX && leftObjects.size() !=0 && rightObjects.size() !=0) {
			costX = tempCostX;
			splitX = i;
		}

		rightTriangles = 0;
		rightObjects.clear();
	}

	leftObjects.clear();
	rightObjects.clear();
	leftTriangles = 0;
	rightTriangles = 0;
	unsigned int splitY = 0;
	float binSizeY = range[1] / numBins;
	float tempCostY = 0;
	leftMin = Vector3(0.0f);
	leftMax = Vector3(0.0f);
	rightMin = Vector3(0.0f);
	rightMax = Vector3(0.0f);

	//Y: Initial cost setup
	costY = std::numeric_limits<float>::max();
	splitY = 0;
	rightTriangles = 0;

	//Y: Recursive cost setup
	leftObjects.clear();
	rightObjects.clear();
	for (int i = 0; i < numBins - 1; i++) {

		leftTriangles += binY[i].size();
		for (int j = 0; j < binY[i].size(); j++)
			leftObjects.push_back(binY[i][j]);

		for (int j = i + 1; j < numBins; j++) {
			rightTriangles += binY[j].size();
			for (int k = 0; k < binY[j].size(); k++)
				rightObjects.push_back(binY[j][k]);
		}

		getBBox(leftObjects, leftMin, leftMax);
		getBBox(rightObjects, rightMin, rightMax);
		tempCostY = getCost(leftTriangles, rightTriangles, leftMin, leftMax, rightMin, rightMax);

		if (tempCostY < costY && leftObjects.size() != 0 && rightObjects.size() != 0) {
			costY = tempCostY;
			splitY = i;
		}

		rightTriangles = 0;
		rightObjects.clear();
	}

	leftObjects.clear();
	rightObjects.clear();
	leftTriangles = 0;
	rightTriangles = 0;
	unsigned int splitZ = 0;
	float binSizeZ = range[2] / numBins;
	float tempCostZ = 0;
	leftMin = Vector3(0.0f);
	leftMax = Vector3(0.0f);
	rightMin = Vector3(0.0f);
	rightMax = Vector3(0.0f);

	//Z: Initial cost setup
	costZ = std::numeric_limits<float>::max();
	splitZ = 0;
	rightTriangles = 0;

	//Z: Recursive cost setup
	leftObjects.clear();
	rightObjects.clear();
	for (int i = 0; i < numBins - 1; i++) {

		leftTriangles += binZ[i].size();
		for (int j = 0; j < binZ[i].size(); j++)
			leftObjects.push_back(binZ[i][j]);

		for (int j = i + 1; j < numBins; j++) {
			rightTriangles += binZ[j].size();
			for (int k = 0; k < binZ[j].size(); k++)
				rightObjects.push_back(binZ[j][k]);
		}

		getBBox(leftObjects, leftMin, leftMax);
		getBBox(rightObjects, rightMin, rightMax);
		tempCostZ = getCost(leftTriangles, rightTriangles, leftMin, leftMax, rightMin, rightMax);

		if (tempCostZ < costZ && leftObjects.size() != 0 && rightObjects.size() != 0) {
			costZ = tempCostZ;
			splitZ = i;
		}

		rightTriangles = 0;
		rightObjects.clear();
	}

	//Decide which axis contains the least cost
	float cost = costX;
	unsigned int axisCost = 0;

	if (costY < cost) {
		cost = costY;
		axisCost = 1;
	}

	if (costZ < cost) {
		cost = costZ;
		axisCost = 2;
	}

	//Populate left and right leaf boxes
	Objects leftChildren, rightChildren;
	if (axisCost == 0) {
		for (int i = 0; i < splitX+1; i++) {
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
		for (int i = 0; i < splitY+1; i++) {
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
		for (int i = 0; i < splitZ+1; i++) {
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

	Vector3 minLeft, minRight, maxLeft, maxRight;
	Vector3 tempMin, tempMax;
	
	//Get min and max of two child boxes
	getBBox(leftChildren, minLeft, maxLeft);
	getBBox(rightChildren, minRight, maxRight);

	//Create two child boxes
	complex_objects.clear();
	leftBox = new BBox(minLeft, maxLeft, leftChildren);
	rightBox = new BBox(minRight, maxRight, rightChildren);
	
	//Add child box to openGL preview
	//gl_objects->push_back(leftBox);
	//gl_objects->push_back(rightBox);
	
	//This box is no longer a leaf since it has children
	leaf = false;

	//Check size of children, if they have 8 triangles, try to split them
	if (leftChildren.size() > 8 && !rightChildren.empty()) {
		splitQueue.push(leftBox);
	}
	if (rightChildren.size() > 8 && !rightChildren.empty()) {
		splitQueue.push(rightBox);
	}
	
}

void
BBox::getBBox(Objects o, Vector3 &min, Vector3 &max) {
	if (o.empty()) return;
	min = o[0]->getMin();
	max = o[0]->getMax();

	Vector3 tempMin;
	Vector3 tempMax;
	for (int i = 0; i < o.size(); i++) {
		tempMin = o[i]->getMin();
		tempMax = o[i]->getMax();

		min.x = std::min(tempMin.x,min.x);
		min.y = std::min(tempMin.y,min.y);
		min.z = std::min(tempMin.z, min.z);

		max.x = std::max(tempMax.x,max.x);
		max.y = std::max(tempMax.y,max.y);	
		max.z = std::max(tempMax.z,max.z);
	}
}

bool
BBox::bvhIntersect(HitInfo& minHit, const Ray& ray, unsigned int &bCount, unsigned int &tCount, float tMin, float tMax)
{
	bool hit = false;
	HitInfo tempMinHit;
	minHit.t = MIRO_TMAX;

	//See if the ray intersects this box
	if (intersect(tempMinHit, ray, bCount, tCount, tMin, tMax)) {

		//If it is a leaf, traverse triangles and see if there is a hit
		if (leaf) {
			for (int i = 0; i < complex_objects.size(); ++i)
				if (complex_objects[i]->intersect(tempMinHit, ray, bCount, tCount, tMin, tMax)) {
					hit = true;
					if (tempMinHit.t < minHit.t)
						minHit = tempMinHit;
				}
		}

		//Else, traverse childre
		else {
			bool left = false;
			bool right = false;
			HitInfo tempMinHitLeft;
			HitInfo tempMinHitRight;

			left = leftBox->bvhIntersect(tempMinHitLeft, ray, bCount, tCount, tMin, tMax);
			right = rightBox->bvhIntersect(tempMinHitRight, ray, bCount, tCount, tMin, tMax);

			//If both children have triangle intersections, return the one with least distance
			if (left  && right) {
				hit = true;
				if (tempMinHitLeft.t < minHit.t)
					minHit = tempMinHitLeft;
				if (tempMinHitRight.t < minHit.t)
					minHit = tempMinHitRight;
			}

			//If left child has intersection, return hit
			else if (left) {
				hit = true;
				if (tempMinHitLeft.t < minHit.t)
					minHit = tempMinHitLeft;
			}

			//If right child has intersection, return hit
			else if (right) {
				hit = true;
				if (tempMinHitRight.t < minHit.t)
					minHit = tempMinHitRight;
			}
		}
	}
	return hit;
}