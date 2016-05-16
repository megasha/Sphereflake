#ifndef CSE168_BVH_H_INCLUDED
#define CSE168_BVH_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "BBox.h"

class BVH
{
public:
    void build(Objects * objs);

	bool intersect(HitInfo& result, const Ray& ray, unsigned int &bCount, unsigned int &tICount,
                   float tMin = 0.0f, float tMax = MIRO_TMAX) const;

protected:
    Objects * m_objects;
	BBox *mainBox;
};

#endif // CSE168_BVH_H_INCLUDED
