#ifndef CSE168_TRIANGLE_H_INCLUDED
#define CSE168_TRIANGLE_H_INCLUDED

#include "Object.h"

/*
    The Triangle class stores a pointer to a mesh and an index into its
    triangle array. The mesh stores all data needed by this Triangle.
*/
class Triangle : public Object
{
public:
    Triangle(TriangleMesh * m = 0, unsigned int i = 0);
    virtual ~Triangle();

    void setIndex(unsigned int i) {m_index = i;}
    void setMesh(TriangleMesh* m) {m_mesh = m;}

	 
	virtual Vector3 getMin();
	virtual Vector3 getMax();
	virtual void setMin() { min = getMin(); }
	virtual void setMax() { max = getMax(); }

	virtual Vector3 getCenter();

    virtual void renderGL();
	virtual bool intersect(HitInfo& result, const Ray& ray, unsigned int &bCount, unsigned int &tCount,
                           float tMin = 0.0f, float tMax = MIRO_TMAX);
    
protected:
	Vector3 min, max;
    TriangleMesh* m_mesh;
    unsigned int m_index;
};

#endif // CSE168_TRIANGLE_H_INCLUDED
