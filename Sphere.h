#ifndef CSE168_SPHERE_H_INCLUDED
#define CSE168_SPHERE_H_INCLUDED

#include "Vector3.h"
#include "Object.h"

class Sphere : public Object
{
public:
    Sphere();
    virtual ~Sphere();

    void setCenter(const Vector3& v)    {m_center = v;}
    void setRadius(const float f)       {m_radius = f;}

    const Vector3& center() const       {return m_center;}
    float radius() const                {return m_radius;}

	virtual Vector3 getMin();
	virtual Vector3 getMax();
	virtual void setMin() { min = getMin(); }
	virtual void setMax() { max = getMax(); }

	virtual Vector3 getCenter() { return m_center; }

    virtual void renderGL();
	virtual bool intersect(HitInfo& result, const Ray& ray, unsigned int &bCount, unsigned int &tCount,
                           float tMin = 0.0f, float tMax = MIRO_TMAX);

protected:
	Vector3 min, max;
    Vector3 m_center;
    float m_radius;
};

#endif // CSE168_SPHERE_H_INCLUDED
