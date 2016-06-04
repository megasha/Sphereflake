#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include <ctime>
#include "Sphere.h"

Scene * g_scene = 0;

Scene::Scene() {
	numRays = 0;
	bCount = 0;
	tCount = 0;
	samples = 1;
}

void
Scene::openGL(Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam->drawGL();

    // draw objects
    for (size_t i = 0; i < m_objects.size(); ++i)
        m_objects[i]->renderGL();

    glutSwapBuffers();
}

void
Scene::preCalc()
{

	std::cout << "Rendering " << m_objects.size() << " objects" << std::endl;
    Objects::iterator it;
    for (it = m_objects.begin(); it != m_objects.end(); it++)
    {
        Object* pObject = *it;
        pObject->preCalc();
    }
    Lights::iterator lit;
    for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
    {
        PointLight* pLight = *lit;
        pLight->preCalc();
    }
	m_bvh.build(&m_objects);

	photonMap = new Photon_map(10000000);
	causticsMap = new Photon_map(10000000);

	std::clock_t start;
	double duration;
	start = std::clock();

	std::cout << "Setting global photon map" << std::endl;
	setPhotonMap(photonMap);
	std::cout << "Setting caustic photon map" << std::endl;
	setCausticsMap(causticsMap);
	std::cout << "End setup" << std::endl;

	std::cout << "\nPhotonMap Duration: " << duration << " seconds" << std::endl;
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;


   

}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
	

    Ray ray;
    HitInfo hitInfo;
    Vector3 shadeResult;
    
    // loop over all pixels in the image

	std::clock_t start;
	double duration;
	start = std::clock();
	bool hit = false;
	unsigned int sampleHits = 0;

	//Setup photon map

    for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0; i < img->width(); ++i)
        {
			for (int k = 0; k < samples; k++) {

				ray = cam->eyeRay(i, j, img->width(), img->height());
				if (trace(hitInfo, ray))
				{
					hit = true;
					sampleHits++;
					shadeResult += hitInfo.material->shade(ray, hitInfo, *this);	

					//Add indirect lighting
					
					float irrad[3] = { 0, 0, 0 };
					float hitPoint[3] = { hitInfo.P.x, hitInfo.P.y, hitInfo.P.z };
					float hitNormal[3] = { hitInfo.N.x, hitInfo.N.y, hitInfo.N.z };
					photonMap->irradiance_estimate(irrad, hitPoint, hitNormal, 0.5f, 1000);
					Vector3 irradVec(irrad[0], irrad[1], irrad[2]);

					if (hitInfo.material->getRefrac() <= 0.0)
						shadeResult += hitInfo.material->getKd()* irradVec;
					
					//Add caustics
					float irradCaus[3] = { 0, 0, 0 };
					causticsMap->irradiance_estimate(irradCaus, hitPoint, hitNormal, 0.5f, 1000);
					Vector3 irradCausVec(irradCaus[0], irradCaus[1], irradCaus[2]);
					shadeResult += irradCausVec;


				}
			}
			if (hit) {
				shadeResult /= (float) sampleHits;
				img->setPixel(i, j, shadeResult);
				hit = false;
				shadeResult = Vector3(0.0f);
				sampleHits = 0;
			}		
        }
        img->drawScanline(j);
        glFinish();
        //printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
        fflush(stdout);
    }
    
   // printf("Rendering Progress: 100.000%\n");
    //debug("done Raytracing!\n");

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	std::cout << "Total Number of Rays: " << numRays << std::endl;
	std::cout << "Total Number of Ray Box Intersections: " << bCount << std::endl;
	std::cout << "Total Number of Ray Triangle Intersections: " << tCount << std::endl;
	std::cout << "\n~~~~~~~~~~~~ END OF TASK 3 STATS ~~~~~~~~~~~~~~\n" << std::endl;

	std::cout << "\nRendering Duration: " << duration << " seconds" << std::endl;


}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax)
{
	numRays++;
    return m_bvh.intersect(minHit, ray, bCount, tCount, tMin, tMax);
}

void
Scene::setPhotonMap(Photon_map* photonMap){
	std::queue<PhotonTraceUnit> PhotonQueue;


	int numPhotons = 0;
	int emitedPhotons = 0;
	int maxPhotons = 50000;
	
	Vector3 photonDir;
	
	while (emitedPhotons < maxPhotons) {
		do {
			photonDir.x = 2.0f * ((float)rand() / (RAND_MAX)) - 1.0f;
			photonDir.y = 2.0f * ((float)rand() / (RAND_MAX)) - 1.0f;
			photonDir.z = 2.0f * ((float)rand() / (RAND_MAX)) - 1.0f;
		} while ((photonDir.x*photonDir.x + photonDir.y*photonDir.y + photonDir.z*photonDir.z) > 1.0f);

		Ray photonRay;
		HitInfo hitInfo;
		photonRay.o = m_lights[0]->position();
		photonRay.d = photonDir;

		if (trace(hitInfo, photonRay)) {
			float photonPow = m_lights[0]->wattage();

			float power[3] = { photonPow, photonPow, photonPow };
			float pos[3] = { hitInfo.P.x, hitInfo.P.y, hitInfo.P.z };
			float dir[3] = { photonDir.x, photonDir.y, photonDir.z };
			//photonMap->store(power, pos, dir);
			//numPhotons++;
			emitedPhotons++;

			Vector3 currkd = hitInfo.material->getKd();
			currkd *=photonPow;


			PhotonTraceUnit temp;
			temp.pos = hitInfo.P;
			temp.norm = hitInfo.N;
			temp.dir = photonDir;
			temp.pow = currkd;
			temp.depth = 0;
			PhotonQueue.push(temp);

			//tracePhoton(hitInfo.P,hitInfo.N,photonDir,currkd, 0, numPhotons);

			/****FOR DEBUGGING PHOTONMAP**/
			/*
			Sphere *photonSphere = new Sphere();
			photonSphere->setCenter(Vector3(pos[0], pos[1], pos[2]));
			photonSphere->setRadius(0.05f);
			photonDebug.push_back(photonSphere);
			m_objects.push_back(photonSphere);
			*/
			
			
		}


	}

	while (!PhotonQueue.empty()) {
		PhotonTraceUnit temp = PhotonQueue.front();
		tracePhoton(temp.pos, temp.norm, temp.dir, temp.pow, PhotonQueue, temp.depth,numPhotons);
		PhotonQueue.pop();
	}

	photonMap->scale_photon_power(1.0f/numPhotons);
	photonMap->balance();
	

}


void
Scene::setCausticsMap(Photon_map* photonMap){
	std::queue<CausticTraceUnit> CausticQueue;

	int numPhotons = 0;
	int initialHits = 0;
	int maxPhotons = 25000;

	Vector3 photonDir;

	while (initialHits < maxPhotons) {
		do {
			photonDir.x = 2.0f * ((float)rand() / (RAND_MAX)) - 1.0f;
			photonDir.y = 2.0f * ((float)rand() / (RAND_MAX)) - 1.0f;
			photonDir.z = 2.0f * ((float)rand() / (RAND_MAX)) - 1.0f;
		} while ((photonDir.x*photonDir.x + photonDir.y*photonDir.y + photonDir.z*photonDir.z) > 1.0f);

		Ray photonRay;
		HitInfo hitInfo;
		photonRay.o = m_lights[0]->position();
		photonRay.d = photonDir;

		if (trace(hitInfo, photonRay)) {
			float photonPow = m_lights[0]->wattage();

			float power[3] = { photonPow, photonPow, photonPow };
			float pos[3] = { hitInfo.P.x, hitInfo.P.y, hitInfo.P.z };
			float dir[3] = { photonDir.x, photonDir.y, photonDir.z };
			//photonMap->store(power, pos, dir);
			//numPhotons++;

			Vector3 currkd = photonPow * hitInfo.material->getRefrac();
			if (hitInfo.material->getRefrac() > 0.0f) {
				//traceCausticPhoton(hitInfo.P, hitInfo.N, photonDir, currkd, 0, numPhotons);
				CausticTraceUnit temp;
				temp.pos = hitInfo.P;
				temp.norm = hitInfo.N;
				temp.dir = photonDir;
				temp.pow = currkd;
				temp.dist = 0;
				temp.oldHit = hitInfo;
				temp.depth = 0;
				CausticQueue.push(temp);
				initialHits++;
			}

			/****FOR DEBUGGING PHOTONMAP**/
			/*
			Sphere *photonSphere = new Sphere();
			photonSphere->setCenter(Vector3(pos[0], pos[1], pos[2]));
			photonSphere->setRadius(0.05f);
			photonDebug.push_back(photonSphere);
			m_objects.push_back(photonSphere);
			*/


		}
	}

	std::cout << "FINISHED EMITTING STUFF" << std::endl;

	while (!CausticQueue.empty()) {
		CausticTraceUnit temp = CausticQueue.front();
		traceCausticPhoton(temp.pos, temp.norm, temp.dir, temp.pow, CausticQueue, numPhotons, temp.dist, temp.oldHit, temp.depth);
		CausticQueue.pop();
	}

	photonMap->scale_photon_power(1.0f / maxPhotons);
	photonMap->balance();


}



void
Scene::tracePhoton(Vector3 pos, Vector3 norm, Vector3 dir, Vector3 pow, std::queue<PhotonTraceUnit> &PhotonQueue, int depth, int &numPhotons) {
	//if (depth == 8) return;

	/***Indirrect Diffuse Lighting**/
	/*
	float theta, phi;
	Vector3 Nx, Nz;
	Vector3 randV;
	int temp0, temp1, temp2;
	float rand0 = ((float)rand() / (RAND_MAX));
	float rand1 = ((float)rand() / (RAND_MAX));

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
	Nx = cross(norm, randV);
	Nz = cross(Nx, norm);

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
	randomRay = randomRay.x * Nx + randomRay.y * norm + randomRay.z * Nz;
	*/

	Vector3 reflectRay;
	reflectRay = -2.0*dot(dir, norm)*norm + dir;

	Ray sampleRay;
	sampleRay.o = pos;
	sampleRay.d = reflectRay.normalize();
	HitInfo hitRand;
	if (trace(hitRand, sampleRay, 0.001f, MIRO_TMAX)) {
		Vector3 currkd = hitRand.material->getKd();

		float power[3] = { pow.x, pow.y, pow.z };
		float pos[3] = { hitRand.P.x, hitRand.P.y, hitRand.P.z };
		float dir[3] = { reflectRay.x, reflectRay.y, reflectRay.z };
		photonMap->store(power, pos, dir);

		currkd *= pow;
		numPhotons++;

		Vector3 kd = hitRand.material->getKd();
		float diffuseComp = (kd.x + kd.y + kd.z) / 3.0f;
		float diffRoulette = ((float)rand() / (RAND_MAX));

		/*FOR DEBUGGING PURPOSES*/
		/*
		Sphere *photonSphere = new Sphere();
		photonSphere->setCenter(Vector3(pos[0], pos[1], pos[2]));
		photonSphere->setRadius(0.05f);
		photonDebug.push_back(photonSphere);
		m_objects.push_back(photonSphere);
		*/

		if (diffRoulette < diffuseComp) {
			//tracePhoton(hitRand.P, hitRand.N,reflectRay, currkd, depth + 1, numPhotons);
			PhotonTraceUnit temp;
			temp.pos = hitRand.P;
			temp.norm = hitRand.N;
			temp.dir = reflectRay;
			temp.pow = currkd;
			temp.depth = depth + 1;
			PhotonQueue.push(temp);
		}
	}
}

void
Scene::traceCausticPhoton(Vector3 pos, Vector3 norm, Vector3 dir, Vector3 pow, std::queue<CausticTraceUnit> &CausticQueue, int &numPhotons, float dist, HitInfo oldHit, int depth) {
	if (depth > 100000) {
		std::cout << "EXITING CAUSTIC PHOTON" << std::endl;
		return;
	}
	float n1 = 1.00029f;
	float n2 = 1.6f;
	float n = n1 / n2;

	float nDotV = dot(-dir, norm);

	if (nDotV <= 0) {
		n = n2 / n1;
		nDotV = dot(-dir, -norm);
	}

	float sinT2 = n * n * (1.0f - (nDotV*nDotV));

	Vector3 refract;
	if (dot(-dir, norm) > 0)
		refract = -n*(-dir - nDotV*norm) - (sqrt(1.0f - sinT2)*norm);
	else
		refract = -n*(-dir - nDotV*-norm) - (sqrt(1.0f - sinT2)*-norm);

	Vector3 reflection = -2 * dot(dir, norm) * norm + dir;

	Ray ray2;
	Ray ray3;
	ray3.o = pos;
	ray3.d = refract.normalize();

	HitInfo stage2;
	HitInfo stage3;
	if (sinT2 > 1.0) {
		if (nDotV > 0)
			reflection = -2.0f * nDotV * norm + dir;
		else
			reflection = -2.0f * nDotV * -norm + dir;

		ray2.o = pos;
		ray2.d = reflection.normalize();
		if (trace(stage2, ray2, 0.0001f, MIRO_TMAX)){
			if (stage2.material->getRefrac() <= 0.0f) {
				Vector3 mat = Vector3(1.0f) - oldHit.material->getGlass();
				mat *= -dist;
				mat = Vector3(expf(mat.x), expf(mat.y), expf(mat.z));
				float hitPoint[3] = { stage2.P.x, stage2.P.y, stage2.P.z };
				float direction[3] = { ray2.d.x, ray2.d.y, ray2.d.z };
				float power[3] = { mat.x*pow.x, mat.y*pow.y, mat.z*pow.z };
				causticsMap->store(power, hitPoint, direction);
				numPhotons++;

				/*
				Sphere *photonSphere = new Sphere();
				photonSphere->setCenter(stage2.P);
				photonSphere->setRadius(0.05f);
				photonDebug.push_back(photonSphere);
				m_objects.push_back(photonSphere);
				*/
			}
			else {
				//traceCausticPhoton(stage2.P, stage2.N, ray2.d, pow, 0, numPhotons);
				CausticTraceUnit temp;
				temp.pos = stage2.P;
				temp.norm = stage2.N;
				temp.dir = ray2.d;
				temp.pow = pow*stage2.material->getRefrac();
				temp.dist = (pos - stage2.P).length();
				temp.oldHit = stage2;
				temp.depth = depth+1;

				if (stage2.material != oldHit.material) {
					temp.pow *= Vector3(1.0f) - oldHit.material->getGlass();
				}
				CausticQueue.push(temp);
			}
		}
	}
	else if (trace(stage3, ray3, 0.0001f, MIRO_TMAX)){
		if (stage3.material->getRefrac() <= 0.0f) {
			Vector3 mat = Vector3(1.0f) - oldHit.material->getGlass();
			mat *= -dist;
			mat = Vector3(expf(mat.x), expf(mat.y), expf(mat.z));

			float hitPoint[3] = { stage3.P.x, stage3.P.y, stage3.P.z };
			float power[3] = { mat.x*pow.x, mat.y*pow.y, mat.z*pow.z };
			float direction[3] = { ray3.d.x, ray3.d.y, ray3.d.z };
			causticsMap->store(power, hitPoint, direction);
			numPhotons++;

			/*
			Sphere *photonSphere = new Sphere();
			photonSphere->setCenter(stage3.P);
			photonSphere->setRadius(0.05f);
			photonDebug.push_back(photonSphere);
			m_objects.push_back(photonSphere);
			*/
			
		}
		else {
			//traceCausticPhoton(stage3.P, stage3.N, ray3.d, pow, 0, numPhotons);
			CausticTraceUnit temp;
			temp.pos = stage3.P;
			temp.norm = stage3.N;
			temp.dir = ray3.d;
			temp.pow = pow*stage3.material->getRefrac();
			temp.dist = (pos - stage3.P).length();
			temp.oldHit = stage3;
			temp.depth = depth + 1;

			if (stage3.material != oldHit.material) {
				temp.pow *= Vector3(1.0f) - oldHit.material->getKd();
			}
			CausticQueue.push(temp);
		}

	}
		/*FOR DEBUGGING PURPOSES*/
		/*
		Sphere *photonSphere = new Sphere();
		photonSphere->setCenter(Vector3(pos[0], pos[1], pos[2]));
		photonSphere->setRadius(0.05f);
		photonDebug.push_back(photonSphere);
		m_objects.push_back(photonSphere);
		*/
}