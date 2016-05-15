#ifndef CSE168_BBOX_H_INCLUDED
#define CSE168_BBOX_H_INCLUDED

#include "Object.h"

class BBox : public Object
{
public:
	BBox(Vector3 cMin, Vector3 cMax, Objects o);
	BBox(Vector3 cMin, Vector3 cMax, Object *o, bool b);
	~BBox();
	Vector3 getMin() { return min; }
	Vector3 getMax() { return max; }
	virtual Vector3 getCenter() { return ((max - min) / 2.0f) + min; }
	virtual void renderGL();
	bool isLeaf() { return leaf; }
	bool intersect(HitInfo& result, const Ray& ray, float tMin = 0.0f, float tMax = MIRO_TMAX);
	bool bvhIntersect(HitInfo& result, const Ray& ray,
		float tMin = 0.0f, float tMax = MIRO_TMAX) const;
	void split(Objects *gl_objects, unsigned int);

private:
	bool leaf;
	Vector3 min;
	Vector3 max;
	Objects complex_objects;
};

#endif