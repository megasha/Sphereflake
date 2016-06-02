#include <math.h>
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include "PointLight.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "Lambert.h"
#include "MiroWindow.h"
#include "assignment1.h"
#include "assignment2.h"
#include <ctime>

namespace
{
	inline Matrix4x4 translate(float x, float y, float z);
	inline Matrix4x4 scale(float x, float y, float z);
	inline Matrix4x4 rotate(float angle, float x, float y, float z);
} // namespace


void myScene(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(512, 512);
	g_scene->setSamples(10);


	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(2.75f, 2.75f, 5.5f));
	g_camera->setLookAt(Vector3(2.75f, 2.75f, -1));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(55);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(2.75f, 5.45f, -2.75f));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(75);
	g_scene->addLight(light);

	TriangleMesh * cornell = new TriangleMesh;
	cornell->load("models/my_box.obj");

	Material* white = new Lambert(Vector3(1.0f));
	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	Material* green = new Lambert(Vector3(0.0f, 1.0f, 0.0f));

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < cornell->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(cornell);
		t->setMaterial(white);
		if (i == 2 || i == 3) t->setMaterial(red);
		if (i == 0 || i == 1) t->setMaterial(green);
		g_scene->addObject(t);

	}

	Material* matSphere = new Lambert(Vector3(0.5f,0.0f,0.0f), Vector3(1.0f,1.0f,1.0f), Vector3(0.0f), 0, 0, 0.5f);
	Material* matSphere1 = new Lambert(Vector3(0.0f,0.0f,0.5f), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.5f);



	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(1.5, 1, -3.5));
	sphere->setRadius(1);
	sphere->setMaterial(matSphere);
	g_scene->addObject(sphere);

	Sphere * sphere1 = new Sphere;
	sphere1->setCenter(Vector3(4.0, 1, -1.5));
	sphere1->setRadius(1);
	sphere1->setMaterial(matSphere1);
	g_scene->addObject(sphere1);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void branch(Vector3 pos, float radius, Scene* g_scene, Material* matSphere, Matrix4x4 currMat, int depth) {
	if (depth > 1) return;

	float newRad = radius / 4;
	float offset = 30.0f;

	Matrix4x4 xform;

	float rand0 = ((float)rand() / (RAND_MAX));
	float rand1 = ((float)rand() / (RAND_MAX));
	float rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere0 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);

	//Add sphere0
	xform = currMat;
	xform *= rotate(offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere0 = new Sphere;
	sphere0->setCenter(xform*pos);
	sphere0->setRadius(newRad);
	sphere0->setMaterial(matSphere0);
	g_scene->addObject(sphere0);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform, depth + 1);

	rand0 = ((float)rand() / (RAND_MAX));
	rand1 = ((float)rand() / (RAND_MAX));
	rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere1 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);

	//Add sphere1
	xform = currMat;
	xform *= rotate(90.0f, 0, 1.0f, 0.0f);
	xform *= rotate(offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere1 = new Sphere;
	sphere1->setCenter(xform*pos);
	sphere1->setRadius(newRad);
	sphere1->setMaterial(matSphere1);
	g_scene->addObject(sphere1);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform,depth+1);

	rand0 = ((float)rand() / (RAND_MAX));
	rand1 = ((float)rand() / (RAND_MAX));
	rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere2 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);


	//Add sphere2
	xform = currMat;
	xform *= rotate(180.0f, 0, 1.0f, 0.0f);
	xform *= rotate(offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere2 = new Sphere;
	sphere2->setCenter(xform*pos);
	sphere2->setRadius(newRad);
	sphere2->setMaterial(matSphere2);
	g_scene->addObject(sphere2);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform, depth + 1);

	rand0 = ((float)rand() / (RAND_MAX));
	rand1 = ((float)rand() / (RAND_MAX));
	rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere3 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);

	//Add sphere3
	xform = currMat;
	xform *= rotate(270.0f, 0, 1.0f, 0.0f);
	xform *= rotate(offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere3 = new Sphere;
	sphere3->setCenter(xform*pos);
	sphere3->setRadius(newRad);
	sphere3->setMaterial(matSphere3);
	g_scene->addObject(sphere3);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform, depth + 1);

	rand0 = ((float)rand() / (RAND_MAX));
	rand1 = ((float)rand() / (RAND_MAX));
	rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere4 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);
	

	//Add sphere4
	xform = currMat;
	xform *= rotate(0.0f, 0, 1.0f, 0.0f);
	xform *= rotate(-offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere4 = new Sphere;
	sphere4->setCenter(xform*pos);
	sphere4->setRadius(newRad);
	sphere4->setMaterial(matSphere4);
	g_scene->addObject(sphere4);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform, depth + 1);

	rand0 = ((float)rand() / (RAND_MAX));
	rand1 = ((float)rand() / (RAND_MAX));
	rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere5 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);

	//Add sphere5
	xform = currMat;
	xform *= rotate(90.0f, 0, 1.0f, 0.0f);
	xform *= rotate(-offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere5 = new Sphere;
	sphere5->setCenter(xform*pos);
	sphere5->setRadius(newRad);
	sphere5->setMaterial(matSphere5);
	g_scene->addObject(sphere5);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform, depth + 1);

	rand0 = ((float)rand() / (RAND_MAX));
	rand1 = ((float)rand() / (RAND_MAX));
	rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere6 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);
	
	//Add sphere6
	xform = currMat;
	xform *= rotate(180.0f, 0, 1.0f, 0.0f);
	xform *= rotate(-offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere6 = new Sphere;
	sphere6->setCenter(xform*pos);
	sphere6->setRadius(newRad);
	sphere6->setMaterial(matSphere6);
	g_scene->addObject(sphere6);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform, depth + 1);

	rand0 = ((float)rand() / (RAND_MAX));
	rand1 = ((float)rand() / (RAND_MAX));
	rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere7 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);

	//Add sphere7
	xform = currMat;
	xform *= rotate(270.0f, 0, 1.0f, 0.0f);
	xform *= rotate(-offset, 0, 0, 1.0);
	xform *= translate(radius + newRad, 0.0f, 0.0f);
	Sphere * sphere7 = new Sphere;
	sphere7->setCenter(xform*pos);
	sphere7->setRadius(newRad);
	sphere7->setMaterial(matSphere7);
	g_scene->addObject(sphere7);
	xform *= rotate(90.0f, 0, 0, 1.0f);
	branch(pos, newRad, g_scene, matSphere, xform, depth + 1);

	

	
}

void
sphereFlake() {
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(512, 512);
	g_scene->setSamples(10);


	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(0.0f, 0.0f, 8.25f));
	g_camera->setLookAt(Vector3(0.0f, 0.0f, -1));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(55);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(0.0f, 2.7f, 0.0f));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(75);
	g_scene->addLight(light);

	Matrix4x4 trans;
	trans = translate(-2.75f, -2.75f, 2.75f);
	TriangleMesh * cornell = new TriangleMesh;
	cornell->load("models/my_box.obj",trans);

	Material* white = new Lambert(Vector3(1.0f));
	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	Material* green = new Lambert(Vector3(0.0f, 1.0f, 0.0f));

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < cornell->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(cornell);
		t->setMaterial(white);
		//if (i == 2 || i == 3) t->setMaterial(red);
		//if (i == 0 || i == 1) t->setMaterial(green);
		g_scene->addObject(t);

	}

	Material* matSphere = new Lambert(Vector3(0.0f,0.0f,0.0f),Vector3(1.0f),Vector3(0.0f,0.0f,0.0f),0,0,1.0f);

	float rand0 = ((float)rand() / (RAND_MAX));
	float rand1 = ((float)rand() / (RAND_MAX));
	float rand2 = ((float)rand() / (RAND_MAX));
	Material* matSphere1 = new Lambert(Vector3(rand0, rand1, rand2), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0, 0.8f);


	Vector3 position(Vector3(0.0f));
	float radius = 1.25f;
	Sphere * sphere = new Sphere;
	sphere->setCenter(position);
	sphere->setRadius(radius);
	sphere->setMaterial(matSphere1);
	g_scene->addObject(sphere);

	Matrix4x4 xform;
	xform.setIdentity();

	branch(position, radius,g_scene,matSphere1, xform,0);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

int
main(int argc, char*argv[])
{
	srand(time(0));

    //Assignment1
    //makeSpiralScene();
	//makeBunnyScene();
	//makeOldTeapotScene();
	//makeSphereScene();
	//makeTestScene();

	//Assignment2
	//makeTeapotScene();
	//makeBunny1Scene();
	//makeBunny20Scene();
	//makeSponzaScene();
	//makeCornellScene();

	//myScene();
	sphereFlake();


    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

namespace
{


	inline Matrix4x4
		translate(float x, float y, float z)
	{
		Matrix4x4 m;
		m.setColumn4(Vector4(x, y, z, 1));
		return m;
	}


	inline Matrix4x4
		scale(float x, float y, float z)
	{
		Matrix4x4 m;
		m.m11 = x;
		m.m22 = y;
		m.m33 = z;
		return m;
	}

	// angle is in degrees
	inline Matrix4x4
		rotate(float angle, float x, float y, float z)
	{
		float rad = angle*(PI / 180.);

		float x2 = x*x;
		float y2 = y*y;
		float z2 = z*z;
		float c = cos(rad);
		float cinv = 1 - c;
		float s = sin(rad);
		float xy = x*y;
		float xz = x*z;
		float yz = y*z;
		float xs = x*s;
		float ys = y*s;
		float zs = z*s;
		float xzcinv = xz*cinv;
		float xycinv = xy*cinv;
		float yzcinv = yz*cinv;

		Matrix4x4 m;
		m.set(x2 + c*(1 - x2), xy*cinv + zs, xzcinv - ys, 0,
			xycinv - zs, y2 + c*(1 - y2), yzcinv + xs, 0,
			xzcinv + ys, yzcinv - xs, z2 + c*(1 - z2), 0,
			0, 0, 0, 1);
		return m;
	}

} // namespace