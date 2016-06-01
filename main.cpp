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
	makeCornellScene();


    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

