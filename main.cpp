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
#include "assignment2.h"
#include <ctime>

void
makeSpiralScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(1.0f, 1.0f, 1.0f));
    g_camera->setEye(Vector3(-5, 2, 3));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(1000);
    g_scene->addLight(light);

    // create a spiral of spheres
    Material* mat = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
    const int maxI = 200;
    const float a = 0.15f;
    for (int i = 1; i < maxI; ++i)
    {
        float t = i/float(maxI);
        float theta = 4*PI*t;
        float r = a*theta;
        float x = r*cos(theta);
        float y = r*sin(theta);
        float z = 2*(2*PI*a - r);
        Sphere * sphere = new Sphere;
        sphere->setCenter(Vector3(x,y,z));
        sphere->setRadius(r/10);
        sphere->setMaterial(mat);
        g_scene->addObject(sphere);
    }

	g_image->resize(512, 512);
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}

void
makeSphereScene()
{
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(512, 512);

	//Set up camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-5, 8, 19));
	g_camera->setLookAt(Vector3(-.5, -1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-2, 2, 4));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(1000);
	g_scene->addLight(light);

	PointLight *light2 = new PointLight;
	light2->setPosition(Vector3(-2, 2, -4));
	light2->setColor(Vector3(1, 1, 1));
	light2->setWattage(500);
	g_scene->addLight(light2);

	PointLight *light3 = new PointLight;
	light3->setPosition(Vector3(2, 2, -4));
	light3->setColor(Vector3(1, 1, 1));
	light3->setWattage(500);
	g_scene->addLight(light3);

	Material* mat = new Lambert(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f), Vector3(0.0f, 0.0f, 0.0f), 1, 0, 0);
	Material* matSphere = new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f), 0, 0.0f, 0.0f);
	Material* matSolidSphere = new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0.0f), 0, 0, 0);
	
	TriangleMesh * sphere = new TriangleMesh;
	sphere->load("models/sphere.obj");

	// create all the triangles in the sphere mesh and add to the scene
	for (int i = 0; i < sphere->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(sphere);
		t->setMaterial(matSphere);
		g_scene->addObject(t);
	}
	

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, -1, 10));
	floor->setV2(Vector3(10, -1, -10));
	floor->setV3(Vector3(-10, -1, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	
	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(mat);
	g_scene->addObject(t);

	g_image->resize(512, 512);
	

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void
makeOldTeapotScene()
{
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);
	g_scene->setSamples(10);

	g_scene->setBackground(Vector3(0.0f, 0.0f, 0.2f));

	// set up the camera
	g_camera->setBGColor(g_scene->bg());
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-2, 2, 4));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(500);
	g_scene->addLight(light);

	PointLight *light2 = new PointLight;
	light2->setPosition(Vector3(-2, 2, -4));
	light2->setColor(Vector3(1, 1, 1));
	light2->setWattage(500);
	g_scene->addLight(light2);

	PointLight *light3 = new PointLight;
	light3->setPosition(Vector3(2, 2, -4));
	light3->setColor(Vector3(1, 1, 1));
	light3->setWattage(500);
	g_scene->addLight(light3);

	Material* mat = new Lambert(Vector3(1.0f));

	TriangleMesh * teapot = new TriangleMesh;
	teapot->load("models/teapot.obj");

	Material* matFloor = new Lambert(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f), Vector3(0.0f, 0.0f, 0.0f), 1, 0, 0);
	Material* matTeapot = new Lambert(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f), 0, 1.0f, 0.0f);
	Material* matSolidSphere = new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0.0f), 0, 0, 0);

	/*
	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(-1, 1, -3));
	sphere->setRadius(1);
	sphere->setMaterial(matSolidSphere);
	g_scene->addObject(sphere);
	*/

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < teapot->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(teapot);
		t->setMaterial(matTeapot);
		g_scene->addObject(t);
	}

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(matFloor);
	g_scene->addObject(t);

	g_image->resize(512, 512);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}



void
makeBunnyScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(128, 128);

	g_scene->setBackground(Vector3(0.0f, 0.0f, 0.2f));
    
    // set up the camera
    g_camera->setBGColor(g_scene->bg());
    g_camera->setEye(Vector3(-2, 3, 5));
    g_camera->setLookAt(Vector3(-.5, 1, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
	light->setPosition(Vector3(-2, 2, 4));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(500);
    g_scene->addLight(light);

	PointLight *light2 = new PointLight;
	light2->setPosition(Vector3(-2, 2, -4));
	light2->setColor(Vector3(1, 1, 1));
	light2->setWattage(500);
	g_scene->addLight(light2);

	PointLight *light3 = new PointLight;
	light3->setPosition(Vector3(2, 2, -4));
	light3->setColor(Vector3(1, 1, 1));
	light3->setWattage(500);
	g_scene->addLight(light3);

	Material* matFloor = new Lambert(Vector3(0.0f, 0.0f, 0.8f), Vector3(0.0f), Vector3(0.0f, 0.0f, 0.0f), 0, 0, 0);
	Material* matBunny = new Lambert(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f), 0, 0.8f, 0.0f);
	Material* matSolidSphere = new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0.0f), 0, 0, 0);

	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(-1, 1, -2));
	sphere->setRadius(1);
	sphere->setMaterial(matSolidSphere);
	g_scene->addObject(sphere);

    TriangleMesh * bunny = new TriangleMesh;
    bunny->load("models/bunny.obj");
    
    // create all the triangles in the bunny mesh and add to the scene
    for (int i = 0; i < bunny->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(bunny);
        t->setMaterial(matBunny); 
        g_scene->addObject(t);
    }
    
    // create the floor triangle
    TriangleMesh * floor = new TriangleMesh;
    floor->createSingleTriangle();
    floor->setV1(Vector3(  0, 0,  10));
    floor->setV2(Vector3( 10, 0, -10));
    floor->setV3(Vector3(-10, 0, -10));
    floor->setN1(Vector3(0, 1, 0));
    floor->setN2(Vector3(0, 1, 0));
    floor->setN3(Vector3(0, 1, 0));
    
    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(floor);
    t->setMaterial(matFloor); 
    g_scene->addObject(t);
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}

void
makeTestScene()
{
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(512, 512);

	//Set background color
	g_scene->setBackground(Vector3(0.2f, 0.2f, 0.2f));

	//Set up camera
	g_camera->setBGColor(g_scene->bg());
	g_camera->setEye(Vector3(-5, 8, 19));
	g_camera->setLookAt(Vector3(-.5, -1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-2, 2, 4));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(500);
	g_scene->addLight(light);

	PointLight *light2 = new PointLight;
	light2->setPosition(Vector3(-2, 2, -4));
	light2->setColor(Vector3(1, 1, 1));
	light2->setWattage(500);
	g_scene->addLight(light2);

	PointLight *light3 = new PointLight;
	light3->setPosition(Vector3(2, 2, -4));
	light3->setColor(Vector3(1, 1, 1));
	light3->setWattage(500);
	g_scene->addLight(light3);

	Material* mat = new Lambert(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f), Vector3(0.0f, 0.0f, 0.0f),1,0,0);
	Material* matSphere = new Lambert(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f),Vector3(0.0f),0,0.0f,0.8f);
	Material* matSolidSphere = new Lambert(Vector3(1.0f, 0.0f, 0.0f), Vector3(0.1f, 0.1f, 0.1f), Vector3(0.0f), 0, 0, 0);

	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(0, 1, 0));
	sphere->setRadius(1);
	sphere->setMaterial(matSphere);
	g_scene->addObject(sphere);

	Sphere * sphere2 = new Sphere;
	sphere2->setCenter(Vector3(0, 1,-3)); 
	sphere2->setRadius(1);
	sphere2->setMaterial(matSolidSphere);
	g_scene->addObject(sphere2);



	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));


	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(mat);
	g_scene->addObject(t);

	g_image->resize(512, 512);


	// let objects do pre-calculations if needed
	g_scene->preCalc();
}



int
main(int argc, char*argv[])
{
	srand(time(0));

    // create a scene
    //makeSpiralScene();
	//makeBunnyScene();
	//makeOldTeapotScene();
	//makeSphereScene();
	//makeTestScene();

	//Assignment 2
	//makeTeapotScene();
	//makeBunny1Scene();
	//makeBunny20Scene();
	//makeSponzaScene();
	makeCornellScene();


    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

