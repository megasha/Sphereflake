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

	Material* matSphere = new Lambert(Vector3(0.0f), Vector3(1.0f,1.0f,1.0f), Vector3(0.0f), 0, 0, 1.0f);


	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(1.5, 1, -3.5));
	sphere->setRadius(1);
	sphere->setMaterial(matSphere);
	g_scene->addObject(sphere);

	Sphere * sphere1 = new Sphere;
	sphere1->setCenter(Vector3(4.0, 1, -1.5));
	sphere1->setRadius(1);
	sphere1->setMaterial(matSphere);
	g_scene->addObject(sphere1);

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

	myScene();


    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

