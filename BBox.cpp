#include "BBox.h"
#include "Ray.h"
#include <algorithm>

BBox::BBox(Vector3 cMin, Vector3 cMax, Objects o) :
min(cMin), max(cMax), complex_objects(o)
{

}

BBox::BBox(Vector3 cMin, Vector3 cMax, Object *o) :
min(cMin), max(cMax)
{
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