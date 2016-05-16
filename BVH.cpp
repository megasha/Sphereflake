#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "BBox.h"
#include <algorithm>
#include <vector>
#include <queue>
#include <ctime>

void
BVH::build(Objects * objs)
{
	std::cout << "\nInitializing BVH" << std::endl;
	std::clock_t start;
	double duration;

	start = std::clock();

	Objects original = *objs;
	m_objects = objs;

	std::vector<BBox> BBoxes;

	//Put bounding box over all primatives
	for (size_t i = 0; i < objs->size(); ++i) {
		BBox aBox((*objs)[i]->getMin(), (*objs)[i]->getMax(), (*objs)[i]);
		(*objs)[i]->setMin(); //for debug purposes
		(*objs)[i]->setMax(); //for debug purposes
		BBoxes.push_back(aBox);
	}
	
	for (int i = 0; i < BBoxes.size(); i++)
	{
		//m_objects->push_back(BBoxes[i]);
	}

	//Calculate main box
	Vector3 min, max;
	min = BBoxes[0].getMin();
	max = BBoxes[0].getMax();

	Vector3 tempMin;
	Vector3 tempMax;
	for (int i = 0; i < BBoxes.size(); i++)
	{
		tempMin = BBoxes[i].getMin();
		tempMax = BBoxes[i].getMax();

		min.x = std::min(tempMin.x,min.x);
		min.y = std::min(tempMin.y,min.y);
		min.z = std::min(tempMin.z,min.z);

		max.x = std::max(tempMax.x,max.x);
		max.y = std::max(tempMax.y,max.y);
		max.z = std::max(tempMax.z,max.z);
	}

	//mainBox = new BBox(min, max, BBoxes);
	mainBox = new BBox(min, max, original);
	m_objects->push_back(mainBox);

	//Split mainbox
	std::queue<BBox*> splitQueue;
	mainBox->split(m_objects,splitQueue, 0);
	
	while (!splitQueue.empty()) {
		BBox *currBox = splitQueue.front();
		std::cout << "Num children: " << currBox->getNumChildren() << std::endl;
		currBox->split(m_objects, splitQueue, 0);
		splitQueue.pop();
	}

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	std::cout << "BVH Complete" << std::endl;
	std::cout << "BVH Duration: " << duration << " seconds\n" << std::endl;
	
	
}

//Return bool and minHit
bool
BVH::intersect(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
    // Here you would need to traverse the BVH to perform ray-intersection
    // acceleration. For now we just intersect every object.

	
    bool hit = false;
    HitInfo tempMinHit;
    minHit.t = MIRO_TMAX;

	if (mainBox->bvhIntersect(tempMinHit, ray, tMin, tMax)) {
		hit = true;
		if (tempMinHit.t < minHit.t)
			minHit = tempMinHit;
	}

	/*
	if (mainBox->intersect(tempMinHit, ray, tMin, tMax)){
		if (mainBox->isLeaf()){
			hit = mainBox->bvhIntersect(tempMinHit, ray, tempMinHit.t, tMax);
			minHit = tempMinHit;
		}
		else {
			mainBox->leftBox->bvhIntersect(tempMinHit, ray, tMin, tMax);

		}
	}
	*/
    
    return hit;
}
