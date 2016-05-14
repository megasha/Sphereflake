#ifndef CSE168_BBOX_H_INCLUDED
#define CSE168_BBOX_H_INCLUDED

#include "Object.h"

class BBox : public Object
{
public:
	BBox(Vector3 cMin, Vector3 cMax, Objects o);
	BBox(Vector3 cMin, Vector3 cMax, Object *o);
	~BBox();
	Vector3 getMin() { return min; }
	Vector3 getMax() { return max; }
	//virtual bool isLeaf() { return false; }
	virtual void renderGL();
	bool intersect(HitInfo& result, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX);
	bool bvhIntersect(HitInfo& result, const Ray& ray,
		float tMin = 0.0f, float tMax = MIRO_TMAX) const;

private:
	Vector3 min;
	Vector3 max;
	Objects complex_objects;
};

#endif