#ifndef CSE168_SCENE_H_INCLUDED
#define CSE168_SCENE_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "PointLight.h"
#include "BVH.h"
#include "photonmap.h"

class Camera;
class Image;

class Scene
{
public:
	Scene();
    void addObject(Object* pObj)        {m_objects.push_back(pObj);}
    const Objects* objects() const      {return &m_objects;}

    void addLight(PointLight* pObj)     {m_lights.push_back(pObj);}
    const Lights* lights() const        {return &m_lights;}

    void preCalc();
    void openGL(Camera *cam);
	void setSamples(int s) { samples = s; }

	void setBackground(const Vector3 & b) { m_bg = b; }
	const Vector3 & bg() const { return m_bg; }

    void raytraceImage(Camera *cam, Image *img);
    bool trace(HitInfo& minHit, const Ray& ray,
               float tMin = 0.0f, float tMax = MIRO_TMAX);
	void incNumRays() { numRays++; }
	void setPhotonMap(Photon_map*);
	void setCausticsMap(Photon_map*);
	void tracePhoton(Vector3 pos, Vector3 norm, Vector3 pow, int depth, int &numPhotons);
	void traceCausticPhoton(Vector3 pos, Vector3 norm, Vector3 dir, Vector3 pow, int depth, int &numPhotons);

	Photon_map *photonMap;
	Photon_map *causticsMap;


protected:
    Objects m_objects;
	Objects photonDebug;
    BVH m_bvh;
    Lights m_lights;
	Vector3 m_bg;
	int samples;
	unsigned int numRays;
	unsigned int bCount;
	unsigned int tCount;
	
};

extern Scene * g_scene;

#endif // CSE168_SCENE_H_INCLUDED
