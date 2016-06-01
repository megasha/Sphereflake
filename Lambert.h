#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"

class Lambert : public Material
{
public:
	Lambert(const Vector3 & kd = Vector3(1),
		    const Vector3 & ks = Vector3(0),
            const Vector3 & ka = Vector3(0),
			const bool & texture = 0,
			const float & reflec = 0,
			const float & refrac = 0);
    virtual ~Lambert();

	const Vector3 & kd() const { return m_kd; }
	const Vector3 & ks() const {return m_ks;}
    const Vector3 & ka() const {return m_ka;}
	const bool & texture() const { return m_texture; }
	const float & reflec() const { return m_reflec; }
	const float & refrac() const { return m_refrac; }

    void setKd(const Vector3 & kd) { m_kd = kd;}
	void setKs(const Vector3 & ks) { m_ks = ks; }
    void setKa(const Vector3 & ka) { m_ka = ka;}
	void setTexture(const bool &t) { m_texture = t; }
	void setReflec(const float &r) { m_reflec = r; }
	void setRefrac(const float &r) { m_refrac = r; }

	virtual Vector3 getKd() { return m_kd; }
	virtual float getRefrac() { return m_refrac; }
    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          Scene& scene) const;
protected:
	Vector3 m_ks;
    Vector3 m_kd;
    Vector3 m_ka;
	bool m_texture;
	float m_reflec;
	float m_refrac;
};

#endif // CSE168_LAMBERT_H_INCLUDED
