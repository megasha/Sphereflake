#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

Lambert::Lambert(const Vector3 & kd, const Vector3 & ks, const Vector3 & ka) :
    m_kd(kd), m_ks(ks), m_ka(ka)
{

}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

	if (ray.rayNum > 2) return L;
    
    const Vector3 viewDir = ray.d; // d is a unit vector
    
    const Lights *lightlist = scene.lights();
    
    // loop over all of the lights
    Lights::const_iterator lightIter;
    for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
    {
        PointLight* pLight = *lightIter;
    
        Vector3 l = pLight->position() - hit.P;
        
        // the inverse-squared falloff
        float falloff = l.length2();
        
        // normalize the light direction
        l /= sqrt(falloff);
		
        // get the diffuse component
        float nDotL = dot(hit.N, l);
        Vector3 result = pLight->color();
        result *= m_kd;

		float irradiance = nDotL / falloff * pLight->wattage() / (4*PI);


		//Compute phong highlight
		Vector3 reflection = -2 * dot(l, hit.N) * hit.N + l;
		float pHighlight = std::max(0.0f, dot(reflection,viewDir));
		pHighlight = std::max(0.0f,pow(pHighlight, 50) / dot(hit.N, l));

		//Calculate shadows
		Ray shadow_ray;
		shadow_ray.o = hit.P;
		shadow_ray.d = l;
		HitInfo shadow_hit;
		if (scene.trace(shadow_hit, shadow_ray, 0.0001, sqrt(falloff))){}
		else {
			L += std::max(0.0f, irradiance) * result;
			L += m_ks * pHighlight*std::max(0.0f, irradiance) * (Vector3(1.0f) - result);
		}

    }

	//Specular reflection
	Vector3 reflection = -2 * dot(viewDir, hit.N) * hit.N + viewDir;
	Ray ray2;
	ray2.o = hit.P;
	ray2.d = reflection.normalize();
	ray2.rayNum = ray.rayNum+1;
	HitInfo stage2;
	if (scene.trace(stage2, ray2,0.001f, MIRO_TMAX)){
		L+= m_ks * stage2.material->shade(ray2, stage2, scene);
	}

	//Specular refraction
	//1.00029/1.60
	
	/*
	float n1 = 1.0029f;
	float n2 = 1.5f;
	float n = n1 / n2;

	float nDotV = dot(viewDir, hit.N);

	if (nDotV <= 0) {
		n = n2 / n1;
		nDotV = -nDotV;
	}

	float sinT2 = n * n * (1.0 - (nDotV*nDotV));
	
	Vector3 refract;
	//refract = n * viewDir - (n + sqrt(1.0 - sinT2)) * hit.N;
	//Vector3 refract = -n*(viewDir - nDotV*hit.N) - (sqrt(1 - sinT2)*hit.N);

	if (dot(viewDir, hit.N) >0)
		refract = -n*(viewDir - nDotV*hit.N) - (sqrt(1 - sinT2)*hit.N);
	else
		refract = -n*(viewDir - nDotV*-hit.N) - (sqrt(1 - sinT2)*-hit.N);


	Ray ray3;
	ray3.o = hit.P;
	ray3.d = refract.normalize();
	ray3.rayNum = ray.rayNum + 1;
	HitInfo stage3;
	if (scene.trace(stage3, ray3, 0.0001f, MIRO_TMAX) && sinT2 <= 1.0){
		L += m_ks * stage3.material->shade(ray3, stage3, scene);
	}
	else L += m_ks *Vector3(0.0f,0.0f,0.2f);
	*/
    
    // add the ambient component
    L += m_ka;
    
    return L;
}
