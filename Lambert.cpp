#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>
#include "Worley.h"
#include "Perlin.h"
#include <random>

std::default_random_engine gen;
std::uniform_real_distribution<float> dist(0, 1);

Lambert::Lambert(const Vector3 & kd, const Vector3 & ks, const Vector3 & ka,
	const bool & texture, const float & reflec, const float & refrac, const Vector3 & glass) :
	m_kd(kd), m_ks(ks), m_ka(ka), m_texture(texture), m_reflec(reflec), m_refrac(refrac), m_glass(glass)
{
}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, Scene& scene) const
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

	if (ray.rayNum > 8) return L;
    
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

		float irradiance = nDotL / falloff * pLight->wattage() / (2.0f*PI);



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
			if (m_refrac <= 0.0f)
				L += std::max(0.0f, irradiance) * result;

			L += m_ks * pHighlight*std::max(0.0f, irradiance);
		}

	
    }

	/***Indirrect Diffuse Lighting**/
	/*
	float theta, phi;
	Vector3 Nx, Nz;
	Vector3 randV;
	int temp0, temp1, temp2;
	float rand0 = dist(gen);
	float rand1 = dist(gen);

	//Create random vector
	temp0 = rand() % 2;
	temp1 = rand() % 2;
	temp2 = rand() % 2;
	randV.x = (float)rand();
	randV.y = (float)rand();
	randV.z = (float)rand();
	if (temp0) randV.x = -randV.x;
	if (temp1) randV.y = -randV.y;
	if (temp2) randV.z = -randV.z;
	randV.normalize();

	//Create coordinate axis where hit normal is y axis
	Nx = cross(hit.N, randV);
	Nz = cross(Nx, hit.N);

	//Get random spherical coordinates
	theta = sqrtf(rand0);
	theta = asinf(theta);
	phi = 2.0f * PI * (rand1);
	

	//Convert random spherical coordinates to local cartesian coordinates
	Vector3 randomRay;	
	randomRay.x = cosf(phi) * sinf(theta);
	randomRay.y = sinf(phi) * sinf(phi);
	randomRay.z = cosf(theta);
	
	//Convert to world coordinates
	randomRay = randomRay.x * Nx + randomRay.y * hit.N + randomRay.z * Nz;
	
	Ray sampleRay;
	sampleRay.o = hit.P;
	sampleRay.d = randomRay.normalize();
	sampleRay.rayNum = ray.rayNum + 1;
	HitInfo hitRand;
	if (scene.trace(hitRand, sampleRay, 0.001f, MIRO_TMAX)) {
		L += (m_kd * std::max(0.0f,dot(hit.N, sampleRay.d)) * hitRand.material->shade(sampleRay, hitRand, scene));
	}
	*/

	/***Specular Reflection**/
	if (m_reflec > 0) {
		Vector3 reflection = -2 * dot(viewDir, hit.N) * hit.N + viewDir;
		Ray ray2;
		ray2.o = hit.P;
		ray2.d = reflection.normalize();
		ray2.rayNum = ray.rayNum + 1;
		HitInfo stage2;
		if (scene.trace(stage2, ray2, 0.001f, MIRO_TMAX)){
			L += m_reflec * stage2.material->shade(ray2, stage2, scene);
		}
		else
			L += m_reflec * scene.bg();
	}
	
	
	
	/***Specular Refraction**/
	if (m_refrac > 0) {
		float n1 = 1.00029f;
		float n2 = 1.6f;
		float n = n1 / n2;

		float nDotV = dot(-viewDir, hit.N);

		if (nDotV <= 0) {
			n = n2 / n1;
			nDotV = dot(-viewDir, -hit.N);
		}

		float sinT2 = n * n * (1.0f - (nDotV*nDotV));

		Vector3 refract;
		if (dot(-viewDir, hit.N) > 0)
			refract = -n*(-viewDir - nDotV*hit.N) - (sqrt(1.0f - sinT2)*hit.N);
		else
			refract = -n*(-viewDir - nDotV*-hit.N) - (sqrt(1.0f - sinT2)*-hit.N);

		Vector3 reflection = -2 * dot(viewDir, hit.N) * hit.N + viewDir;
		Ray ray2;
		Ray ray3;
		ray3.o = hit.P;
		ray3.d = refract.normalize();
		ray3.rayNum = ray.rayNum + 1;
		HitInfo stage2;
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
				
				if (hit.material == stage2.material) {
					Vector3 absorbance = stage2.material->getGlass()* (stage2.P - ray2.o).length();
					absorbance = Vector3(expf(absorbance.x), expf(absorbance.y), expf(absorbance.z));
					L += stage2.material->getGlass() * absorbance;
				}


				//Add indirect lighting to refraction
				float irrad[3] = { 0, 0, 0 };
				float hitPoint[3] = { stage2.P.x, stage2.P.y, stage2.P.z };
				float hitNormal[3] = { stage2.N.x, stage2.N.y, stage2.N.z };
				scene.photonMap->irradiance_estimate(irrad, hitPoint, hitNormal, 0.5f, 1000);
				L += stage2.material->getKd()*Vector3(irrad[0], irrad[1], irrad[2]);

				//Add indirect beer?
				if (hit.material == stage2.material) {
					L += stage2.material->getGlass()*Vector3(irrad[0], irrad[1], irrad[2]);
				}
			}
		}
		else if (scene.trace(stage3, ray3, 0.0001f, MIRO_TMAX)){
			L += m_refrac * stage3.material->shade(ray3, stage3, scene);

			if (hit.material == stage3.material) {
				Vector3 absorbance = stage3.material->getGlass() * (stage3.P-ray3.o).length();
				absorbance = Vector3(expf(absorbance.x), expf(absorbance.y), expf(absorbance.z));
				L += stage3.material->getGlass() * absorbance;
			}

			//Add indirect lighting to refraction
			float irrad[3] = { 0, 0, 0 };
			float hitPoint[3] = { stage3.P.x, stage3.P.y, stage3.P.z };
			float hitNormal[3] = { stage3.N.x, stage3.N.y, stage3.N.z };
			scene.photonMap->irradiance_estimate(irrad, hitPoint, hitNormal, 0.5f, 1000);
			L += stage3.material->getKd()*Vector3(irrad[0], irrad[1], irrad[2]);

			//Add indirect beer?
			if (hit.material == stage3.material) {
				L += stage3.material->getGlass()*Vector3(irrad[0], irrad[1], irrad[2]);
			}
		}
		else L += m_refrac *scene.bg();
	}
	
    // Ambient component
    L += m_ka;
    
    return L;
}
