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

Vector3
Triangle::getMin()
{
	Vector3 min;

	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	min.x = v0.x;
	min.y = v0.y;
	min.z = v0.z;

	if (min.x > v1.x) min.x = v1.x;
	if (min.x > v2.x) min.x = v2.x;

	if (min.y > v1.y) min.y = v1.y;
	if (min.y > v2.y) min.y = v2.y;

	if (min.z > v1.z) min.z = v1.z;
	if (min.z > v2.z) min.z = v2.z;

	return min;
}

Vector3
Triangle::getMax()
{
	Vector3 max;

	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	max.x = v0.x;
	max.y = v0.y;
	max.z = v0.z;

	if (max.x < v1.x) max.x = v1.x;
	if (max.x < v2.x) max.x = v2.x;

	if (max.y < v1.y) max.y = v1.y;
	if (max.y < v2.y) max.y = v2.y;

	if (max.z < v1.z) max.z = v1.z;
	if (max.z < v2.z) max.z = v2.z;

	return max;
}

Vector3
Triangle::getCenter() {
	Vector3 ret;
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	ret = (v0 + v1 + v2) / 3;
	return ret;
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
Triangle::intersect(HitInfo& result, const Ray& r, unsigned int &bCount, unsigned int &tCount, float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

	ti3 = m_mesh->nIndices()[m_index];
	const Vector3 & n0 = m_mesh->normals()[ti3.x];
	const Vector3 & n1 = m_mesh->normals()[ti3.y];
	const Vector3 & n2 = m_mesh->normals()[ti3.z];

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
	if (t < tMin || t > tMax) return false;

	//Compute Alpha
	Vector3 temp = cross(b, AC);
	float alpha = dot(-r.d, temp);
	alpha /= detA;

	//Compute Beta
	temp = cross(AB, b);
	float beta = dot(-r.d, temp);
	beta /= detA;

	if (alpha < 0 || beta < 0 || alpha + beta > 1) return false;

	//Compute interpolated normal
	normal = (1 - alpha - beta)*n0 + alpha*n1 + beta*n2;

	result.t = t;
	result.N = normal.normalize();
	result.P = r.o + result.t*r.d;
	result.material = this->m_material;

	tCount++;

    return true;
}
