#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"


Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}


void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& r,float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	Vector3 AB = v1 - v0;
	Vector3 AC = v2 - v0;

	Vector3 b = r.o - v0;

	Vector3 normal = cross(AB, AC);

	float detA = dot(-r.d, normal);

	//Check
	if (detA == 0) return false;

	//Compute T
	float t = dot(b, normal);
	t /= detA;

	//Check
	if (t <= tMin || t >= tMax) return false;

	//Compute Alpha
	Vector3 temp = cross(b, AC);
	float alpha = dot(-r.d, temp);
	alpha /= detA;

	//Compute Beta
	temp = cross(AB, b);
	float beta = dot(-r.d, temp);
	beta /= detA;

	if (alpha < 0 || beta < 0 || alpha + beta > 1) return false;

	result.t = t;
	result.N = normal.normalize();
	result.P = r.o + result.t*r.d;
	result.material = this->m_material;

    return true;
}
