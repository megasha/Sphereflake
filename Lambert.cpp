#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>
#include "Worley.h"
#include "Perlin.h"

Lambert::Lambert(const Vector3 & kd, const Vector3 & ks, const Vector3 & ka,
	const bool & texture, const float & reflec, const float & refrac) :
	m_kd(kd), m_ks(ks), m_ka(ka), m_texture(texture), m_reflec(reflec), m_refrac(refrac)
{
}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	Vector3 pattern = Vector3(1.0f);
	Vector3 patternRand = Vector3(1.0f);
	Vector3 color = m_kd;

	//Stone texture
	if (m_texture) {
		float at[3] = { hit.P[0], hit.P[1], hit.P[2] };
		long maxOrder = 7;
		float *F = new float[maxOrder];
		float(*delta)[3] = new float[maxOrder][3];
		unsigned long *ID = new unsigned long[maxOrder];

		WorleyNoise::noise3D(at, maxOrder, F, delta, ID);
		pattern = Vector3(F[2] - F[1]);

		float x1 = abs(PerlinNoise::noise(hit.P[0], hit.P[1], hit.P[2]));
		float y1 = abs(PerlinNoise::noise(hit.P[1], hit.P[0], hit.P[2]));
		float z1 = abs(PerlinNoise::noise(hit.P[2], hit.P[0], hit.P[1]));

		float x = abs(PerlinNoise::noise(pattern.x, pattern.y, pattern.z));
		float y = abs(PerlinNoise::noise(pattern.y, pattern.x, pattern.z));
		float z = abs(PerlinNoise::noise(pattern.z, pattern.y, pattern.x));

		patternRand = Vector3(x, y, z);

		color = Vector3(x1, y1, z1);
	}


    Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

	if (ray.rayNum > 4) return L;
    
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
        result *= pattern*patternRand*color;

		float irradiance = nDotL / falloff * pLight->wattage() / (0.5*PI);


		//Compute phong highlight
		Vector3 reflection = -2 * dot(l, hit.N) * hit.N + l;
		reflection.normalize();
		float pHighlight = std::max(0.0f, dot(reflection,viewDir));
		pHighlight = std::max(0.0f,pow(pHighlight, 50) / dot(hit.N, l));

		//Calculate shadows
		Ray shadow_ray;
		shadow_ray.o = hit.P;
		shadow_ray.d = l;
		HitInfo shadow_hit;
		if (scene.trace(shadow_hit, shadow_ray, 0.001f, sqrt(falloff))){}
		else {
			L += std::max(0.0f, irradiance) * result;
			L += m_ks * pHighlight*std::max(0.0f, irradiance);
		}

    }

	//Specular reflection
	Vector3 reflection = -2 * dot(viewDir, hit.N) * hit.N + viewDir;
	Ray ray2;
	ray2.o = hit.P;
	ray2.d = reflection.normalize();
	ray2.rayNum = ray.rayNum+1;
	HitInfo stage2;
	if (scene.trace(stage2, ray2, 0.001f, MIRO_TMAX)){
		L += m_reflec * stage2.material->shade(ray2, stage2, scene);
	}
	else
		L += m_reflec * scene.bg();
	
	
	//Specular refraction
	float n1 = 1.00029f;
	float n2 = 1.6f;
	float n = n1 / n2;

	float nDotV = dot(-viewDir, hit.N);

	if (nDotV <= 0) {
		n = n2 / n1;
		nDotV = dot(-viewDir,-hit.N);
	}

	float sinT2 = n * n * (1.0f - (nDotV*nDotV));
	
	Vector3 refract;
	if (dot(-viewDir, hit.N) > 0)
		refract = -n*(-viewDir - nDotV*hit.N) - (sqrt(1.0f - sinT2)*hit.N);
	else
		refract = -n*(-viewDir - nDotV*-hit.N) - (sqrt(1.0f - sinT2)*-hit.N);


	Ray ray3;
	ray3.o = hit.P;
	ray3.d = refract.normalize();
	ray3.rayNum = ray.rayNum + 1;
	HitInfo stage3;
	if (sinT2 > 1.0) {
		if (nDotV > 0)
			reflection = -2.0f * nDotV * hit.N + viewDir;
		else
			reflection = -2.0f * nDotV * -hit.N + viewDir;

		ray2.o = hit.P;
		ray2.d = reflection.normalize();
		ray2.rayNum = ray.rayNum + 1;
		if (scene.trace(stage2, ray2, 0.0001f, MIRO_TMAX)){
			L += m_refrac * stage2.material->shade(ray2, stage2, scene);
		}
	}
	else if (scene.trace(stage3, ray3, 0.0001f, MIRO_TMAX)){
		L += m_refrac * stage3.material->shade(ray3, stage3, scene);
	}
	else L += m_refrac *scene.bg();
	
    
    // Ambient component
    L += m_ka;
    
    return L;
}
