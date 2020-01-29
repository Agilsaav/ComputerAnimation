#pragma once
#include "Geometry.h"
#include <iostream>

//****************************************************
// Plane
//****************************************************

Plane::Plane(const glm::vec3& point, const glm::vec3& normalVect){
	normal = glm::normalize(normalVect);
	dconst = -glm::dot(point, normal);
};

Plane::Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2){
	glm::vec3 v1 = point1 - point0;
	glm::vec3 v2 = point2 - point0;
	normal = glm::normalize(glm::cross(v1, v2));
	dconst = -glm::dot(point0, normal);
};

void Plane::setPosition(const glm::vec3& newPos){
	dconst = -glm::dot(newPos, normal);
};

bool Plane::isInside(const glm::vec3& point){
	float dist;
	dist = glm::dot(point, normal) + dconst;
	if (dist > 1.e-7)
		return false;
	else
		return true;
};

float Plane::distPoint2Plane(const glm::vec3& point){
	float dist;
	return dist = glm::dot(point, normal) + dconst;
};

glm::vec3 Plane::closestPointInPlane(const glm::vec3& point){
	glm::vec3 closestP;
	float r = (-dconst - glm::dot(point, normal));
	return closestP = point + r*normal;
};

bool Plane::intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall){
	if (distPoint2Plane(point1)*distPoint2Plane(point2) > 0)	return false;
	float r = (-dconst - glm::dot(point1, normal)) / glm::dot((point2 - point1), normal);
	pTall = (1 - r)*point1 + r*point2;
	return true;
};


//****************************************************
// Triangle
//****************************************************

Triangle::Triangle(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2)
{
	vertex1 = point0;
	vertex2 = point1;
	vertex3 = point2;
};

void Triangle::setPosition(const glm::vec3& newPos)
{

};

bool Triangle::isInside(const glm::vec3& point)
{
	float Area = calculateArea(point, vertex2, vertex3) + calculateArea(vertex1, point, vertex3)
		+ calculateArea(vertex1, vertex2, point) - calculateArea(vertex1, vertex2, vertex3);

	if(Area <= 0.2f) return true;
	else return false;
};

bool Triangle::intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall)
{
	return true;
};


float Triangle::calculateArea(const glm::vec3& point1, const glm::vec3& point2, const glm::vec3& point3)
{
	glm::vec3 crossProd = glm::cross((point2 - point1), (point3 - point1));
	float area = 0.5f * glm::length(crossProd);

	return area;
}

//****************************************************
// Sphere
//****************************************************
Sphere::Sphere(const glm::vec3& point, const float& radious)
{
	center = point;
	radi = radious;
}

void Sphere::setPosition(const glm::vec3& newPos)
{
	center = newPos;
}

bool Sphere::isInside(const glm::vec3& point)
{
	float dist = distanceToSpehere(point);

	if (dist <= radi) return true;
	else return false;
}

float Sphere::distanceToSpehere(const glm::vec3& point)
{
	float dist = sqrt(pow(point.x - center.x, 2) + pow(point.y - center.y, 2) + pow(point.z - center.z, 2));

	return dist;
}

glm::vec3 Sphere::computePointP(const glm::vec3& previousPos, const glm::vec3& nextPos)
{
	glm::vec3 P;
	glm::vec3 v = nextPos - previousPos;
	float alpha = glm::dot(v, v);
	float beta = glm::dot(2.0f*v, previousPos - center);
	float gamma = glm::dot(center, center) + glm::dot(previousPos, previousPos)
		- 2.0f * glm::dot(previousPos, center) - pow(radi, 2);

	float lambda = (1.0 / 2.0 * alpha) * (-beta + sqrt(pow(beta, 2) - (4.0f * alpha * gamma)));
	if(lambda > 1.0f || lambda < 0.0f )
		lambda = (1.0 / 2.0 * alpha) * (-beta - sqrt(pow(beta, 2) - (4.0f * alpha * gamma)));

	//std::cout << lambda << std::endl;
	P = previousPos + lambda * v;

	return P;
}
