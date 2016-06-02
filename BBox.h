#ifndef CSE168_BBOX_H_INCLUDED
#define CSE168_BBOX_H_INCLUDED

#include "Object.h"
#include <queue>

class BBox : public Object
{
public:
	BBox(Vector3 center, float length);
	BBox(Vector3 cMin, Vector3 cMax, Objects o);
	BBox(Vector3 cMin, Vector3 cMax, Object *o);
	~BBox();
	Vector3 getMin() { return min; }
	Vector3 getMax() { return max; }
	Objects getObjects() { return complex_objects; }
	virtual Vector3 getCenter() { return ((max - min) / 2.0f) + min; }
	virtual void renderGL();
	bool isLeaf() { return leaf; }
	virtual bool intersect(HitInfo& result, const Ray& ray, unsigned int &bCount, unsigned int &tCount, float tMin = 0.0f, float tMax = MIRO_TMAX);
	int getNumChildren() { return complex_objects.size(); }
	bool bvhIntersect(HitInfo& result, const Ray& ray, unsigned int &bCount, unsigned int &tCount,
		float tMin = 0.0f, float tMax = MIRO_TMAX);
	void split(Objects *gl_objects, std::queue<BBox*> &splitQueue, unsigned int);
	BBox *leftBox, *rightBox;

protected:
	Objects complex_objects;
	bool leaf;

private:
	
	Vector3 min;
	Vector3 max;
	void getBBox(Objects o, Vector3 &min, Vector3 &max);
	float getCost(unsigned int &lTriangles, unsigned int &rTriangles, Vector3 &lMin, Vector3 &lMax, Vector3 &rMin, Vector3 &rMax);
};

#endif