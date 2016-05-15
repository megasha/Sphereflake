#include "BVH.h"
#include "Ray.h"
#include "Console.h"
#include "BBox.h"
#include <vector>

void
BVH::build(Objects * objs)
{
	Objects original = *objs;
	m_objects = objs;

	Objects BBoxes;

	//Put bounding box over all primatives
	for (size_t i = 0; i < objs->size(); ++i) {
		BBox *aBox = new BBox((*objs)[i]->getMin(), (*objs)[i]->getMax(), (*objs)[i], true);
		BBoxes.push_back(aBox);
	}
	
	for (int i = 0; i < BBoxes.size(); i++)
	{
		//m_objects->push_back(BBoxes[i]);
	}

	//Calculate main box
	Vector3 min, max;
	min = BBoxes[0]->getMin();
	max = BBoxes[0]->getMax();

	Vector3 tempMin;
	Vector3 tempMax;
	for (int i = 0; i < BBoxes.size(); i++)
	{
		tempMin = BBoxes[i]->getMin();
		tempMax = BBoxes[i]->getMax();

		if (min.x > tempMin.x) min.x = tempMin.x;
		if (min.y > tempMin.y) min.y = tempMin.y;
		if (min.z > tempMin.z) min.z = tempMin.z;

		if (max.x < tempMax.x) max.x = tempMax.x;
		if (max.y < tempMax.y) max.y = tempMax.y;
		if (max.z < tempMax.z) max.z = tempMax.z;
	}

	mainBox = new BBox(min, max, BBoxes);
	//mainBox = new BBox(min, max, original);
	m_objects->push_back(mainBox);

	//Split mainbox
	mainBox->split(m_objects,0);

	
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

	if (mainBox->intersect(tempMinHit, ray, tMin, tMax)){
		if (mainBox->isLeaf()){
			hit = mainBox->bvhIntersect(tempMinHit, ray, tempMinHit.t, tMax);
			minHit = tempMinHit;
		}
		else {

		}
	}
    
    return hit;
}
