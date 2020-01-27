#include "Physics.h"

#include <iostream>

Physics::Physics() {};
Physics::~Physics() {};

void Physics::GetPlaneCollisions(Particle& p, std::vector<Plane>& planes, std::vector<float>& distBefore, std::vector<float>& distNew, float& frictionCoef, int& solverUsed)
{
	for (unsigned int i = 0; i < planes.size(); i++)
	{
		distBefore[i] = distNew[i];
		distNew[i] = planes[i].distPoint2Plane(p.getCurrentPosition());
		if (distNew[i] * distBefore[i] < 0.0f)
		{
			glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(planes[i].normal, p.getCurrentPosition()) + planes[i].dconst) * planes[i].normal;
			glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(planes[i].normal, p.getVelocity()) * planes[i].normal;
			glm::vec3 normVelocity = glm::dot(planes[i].normal, p.getVelocity()) * planes[i].normal;
			glm::vec3 tgVelocity = p.getVelocity() - normVelocity;
			p.setPosition(newPosition);
			p.setVelocity(newVelocity - frictionCoef * tgVelocity);
			distNew[i] = -distNew[i];

			if (solverUsed == 2)
			{
				glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(planes[i].normal, p.getPreviousPosition()) + planes[i].dconst) * planes[i].normal;
				p.setPreviousPosition(newPrevPos);
			}
		}
	}
}

void Physics::GetTriangleCollision(Particle& p, Triangle& tri, float& distBefore, float& distNew, float& frictionCoef, int& solverUsed)
{
	Plane plane(tri.vertex1, tri.vertex2, tri.vertex3);

	distBefore = distNew;
	distNew = plane.distPoint2Plane(p.getCurrentPosition());
	if (distNew * distBefore < 0.0f) //Plane colision
	{
		if (tri.isInside(p.getCurrentPosition())) //Inside Triangle
		{
			glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getCurrentPosition()) + plane.dconst) * plane.normal;
			glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(plane.normal, p.getVelocity()) * plane.normal;
			glm::vec3 normVelocity = glm::dot(plane.normal, p.getVelocity()) * plane.normal;
			glm::vec3 tgVelocity = p.getVelocity() - normVelocity;
			p.setPosition(newPosition);
			p.setVelocity(newVelocity - frictionCoef * tgVelocity);
			distNew = -distNew;

			if (solverUsed == 2)
			{
				glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getPreviousPosition()) + plane.dconst) * plane.normal;
				p.setPreviousPosition(newPrevPos);
			}
		}
	}
}

void Physics::GetParticleCollisions(std::vector<Particle>& particles, Particle& p, int particleIndex, float& radiusCollision, float dt)
{
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		if (i != particleIndex)
		{
			float distance = glm::distance(p.getCurrentPosition(), particles[i].getCurrentPosition());
			if (distance <= 2.0f*(radiusCollision))
			{
				//std::cout << "Hei coll between Part!" << std::endl;
				//p.setPosition(p.getCurrentPosition() - dt*p.getVelocity());
				//particles[i].setPosition(particles[i].getCurrentPosition() - dt*particles[i].getVelocity());
				glm::vec3 norm = (p.getCurrentPosition() - particles[i].getCurrentPosition()) / distance;

				float distToMove = 2.0f * radiusCollision - distance;
				p.setPosition(p.getCurrentPosition() - norm*(distToMove / 2.0f));
				particles[i].setPosition(particles[i].getCurrentPosition() - norm*(distToMove / 2.0f));

				glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(norm, p.getVelocity()) * norm;
				p.setVelocity(newVelocity);
	
				//particles[i].setVelocity(-particles[i].getVelocity());
			}
		}
	}
}

void Physics::GetSpehereCollision(Particle& p, Sphere& sphere, float& frictionCoef, int& solverUsed)
{
	if (sphere.isInside(p.getCurrentPosition())) //Inside sphere
	{

		//Compute Plane
		glm::vec3 pointP = sphere.computePointP(p.getPreviousPosition(), p.getCurrentPosition());
		glm::vec3 normP(pointP.x - sphere.center.x, pointP.y - sphere.center.y, pointP.z - sphere.center.z);
		Plane plane(pointP, normP);

		//Plane Collision:

		glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getCurrentPosition()) + plane.dconst) * plane.normal;
		glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(plane.normal, p.getVelocity()) * plane.normal;
		p.setPosition(newPosition);

		glm::vec3 vel = 2.0f * 4.0f * glm::normalize(newVelocity);
		p.setVelocity(vel);
		//p.setVelocity(newVelocity);

		if (solverUsed == 2)
		{
			glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getPreviousPosition()) + plane.dconst) * plane.normal;
			p.setPreviousPosition(newPrevPos);
		}
	}
}

void Physics::GetSphereSphereCollision(Particle& p, Particle& p2, Sphere& sphere, float& frictionCoef, int& solverUsed)
{
	if (sphere.isInside(p.getCurrentPosition())) //Inside sphere
	{

		//Compute Plane
		glm::vec3 pointP = sphere.computePointP(p.getPreviousPosition(), p.getCurrentPosition());
		glm::vec3 normP(pointP.x - sphere.center.x, pointP.y - sphere.center.y, pointP.z - sphere.center.z);
		Plane plane(pointP, normP);

		//Plane Collision:

		glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getCurrentPosition()) + plane.dconst) * plane.normal;
		glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(plane.normal, p.getVelocity()) * plane.normal;
		p.setPosition(newPosition);
		p.setVelocity(newVelocity);
		//p.setVelocity(4.0f*glm::normalize(newVelocity));

		//glm::vec3 norm2 = -plane.normal;

		//glm::vec3 newPosition2 = p2.getCurrentPosition() - (1.0f + p2.getBouncing()) * (glm::dot(norm2, p2.getCurrentPosition()) + plane.dconst) * norm2;
		//glm::vec3 newVelocity2 = p2.getVelocity() - (1.0f + p2.getBouncing()) * glm::dot(norm2, p2.getVelocity()) * norm2;
		//p.setPosition(newPosition2);
		//p.setVelocity(newVelocity2);

		if (solverUsed == 2)
		{
			glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getPreviousPosition()) + plane.dconst) * plane.normal;
			p.setPreviousPosition(newPrevPos);
		}
	}
}

void Physics::GetRectangleCircleColision(Particle& p, GRectangle& rectangle, const float& radius, float& frictionCoef, int& solverUsed)
{
	float testX = p.getCurrentPosition().x;
	float testY = p.getCurrentPosition().z;

	float dist1, dist2, sign1 , sign2;
	glm::vec3 norm;

	if (p.getCurrentPosition().x < rectangle.pos.x)
	{
		testX = rectangle.pos.x;
		norm = glm::vec3(-1.0f, 0.0f, 0.0f);
		dist1 = abs(rectangle.pos.x - p.getPreviousPosition().x);
		sign1 = -1.0f;
	}
	else if (p.getCurrentPosition().x > rectangle.pos.x + rectangle.width)
	{
		testX = rectangle.pos.x + rectangle.width;
		norm = glm::vec3(1.0f, 0.0f, 0.0f);
		dist1 = abs(rectangle.pos.x + rectangle.width - p.getPreviousPosition().x);
		sign1 = 1.0f;
	}

	if (p.getCurrentPosition().z < rectangle.pos.y)
	{
		testY = rectangle.pos.y;
		norm = glm::vec3(0.0f, 0.0f, -1.0f);
		dist2 = abs(rectangle.pos.y - p.getPreviousPosition().z);
		sign2 = -1.0f;
	}
	else if (p.getCurrentPosition().z > rectangle.pos.y + rectangle.height)
	{
		testY = rectangle.pos.y + rectangle.height;
		norm = glm::vec3(0.0f, 0.0f, 1.0f);
		dist2 = abs(rectangle.pos.y + rectangle.height - p.getPreviousPosition().z);
		sign2 = 1.0f;
	}

	float distX = p.getCurrentPosition().x - testX;
	float distY = p.getCurrentPosition().z - testY;
	float dist = sqrt(pow(distX, 2) + pow(distY, 2));

	bool inside;
	if (dist <= radius) inside = true;
	else inside = false;

	if (inside)
	{
		if (dist1 < dist2) norm = sign1 * glm::vec3(1.0f, 0.0f, 0.0f);
		else norm = sign2 * glm::vec3(0.0f, 0.0f, 1.0f);

		//glm::vec3 newPosition = p.getCurrentPosition() + dist * glm::normalize(p.getVelocity());
		//p.setPosition(newPosition);
		//std::cout << "hit " <<  norm.x << " " << norm.y << " " << norm.z << std::endl;
		//p.setVelocity(-p.getVelocity());
		//glm::vec3 newPosition = p.getCurrentPosition() - (1.0f + p.getBouncing()) * (glm::dot(norm, p.getCurrentPosition())) * norm;

		//glm::vec3 newPosition = p.getCurrentPosition() + dist * (glm::normalize(p.getVelocity()));
		//p.setPosition(newPosition);
		glm::vec3 newVelocity = p.getVelocity() - (1.0f + p.getBouncing()) * glm::dot(norm, p.getVelocity()) * norm;
		glm::vec3 vel = 2.0f * 3.0f * glm::normalize(newVelocity);
		p.setVelocity(vel);
		//p.setVelocity(newVelocity);
		glm::vec3 newPosition = p.getCurrentPosition() + dist * (newVelocity * 0.01f);
		p.setPosition(newPosition);
		

		if (solverUsed == 2)
		{
			glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(norm, p.getPreviousPosition())) * norm;
			p.setPreviousPosition(newPrevPos);
		}
	}
}

void Physics::ComputeObsAvoidance(std::vector<Particle>& particles, Particle& p1, std::vector<GRectangle> obstacles, const int index, const float& radius, const float& radiusObs, const float& maxSeeAhead, const float& maxAvoidForce)
{
	float maxVel = 4.0f;
	glm::vec3 ahead = p1.getCurrentPosition() + glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity())/maxVel);
	glm::vec3 ahead2 = p1.getCurrentPosition() + 0.5f * glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity()) / maxVel);

	//std::cout << glm::distance(ahead, p1.getCurrentPosition())<< std::endl;
	//std::cout << glm::distance(ahead2, p1.getCurrentPosition()) << std::endl;

	glm::vec3 mostThreateningVec = glm::vec3(0.0f);
	float mostThreateningDist = std::numeric_limits<float>::max();
	bool avoidance = false;

	//Avoidance Particles!
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		if (i == index) continue;

		float dist_ahead = glm::distance(ahead, particles[i].getCurrentPosition());
		float dist_ahead2 = glm::distance(ahead2, particles[i].getCurrentPosition());

		if (dist_ahead <= (2.0f * radius + 0.05f) || dist_ahead2 <=( 2.0f * radius + 0.05f))
		{
			if (!avoidance) avoidance = true;

			if (glm::distance(p1.getCurrentPosition(), particles[i].getCurrentPosition()) < mostThreateningDist)
			{
				mostThreateningDist = glm::distance(p1.getCurrentPosition(), particles[i].getCurrentPosition());
				mostThreateningVec = particles[i].getCurrentPosition();
			}
		}
	}

	if (avoidance)
	{
		//std::cout << "Avoidance!" << std::endl;
		glm::vec3 avoidanceVec = glm::vec3(0.0f);
		avoidanceVec.x = ahead.x - mostThreateningVec.x;
		avoidanceVec.z = ahead.z - mostThreateningVec.z;

		avoidanceVec = maxAvoidForce * glm::normalize(avoidanceVec) * (1.0f / glm::distance(p1.getCurrentPosition(), mostThreateningVec));
		//std::cout << avoidanceVec.x << " " << avoidanceVec.z << std::endl;

		p1.setVelocity(p1.getVelocity() + avoidanceVec);
	}


	//Avoidance Walls
	ahead = p1.getCurrentPosition() + glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity()) / maxVel);
	ahead2 = p1.getCurrentPosition() + 0.5f * glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity()) / maxVel);

	mostThreateningVec = glm::vec3(0.0f);
	mostThreateningDist = std::numeric_limits<float>::max();
	avoidance = false;

	for (unsigned int i = 0; i < obstacles.size(); i++)
	{
		if (glm::dot(glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f) - p1.getCurrentPosition(), p1.getVelocity()) > 0.05f)
		{
			float dist_ahead = glm::distance(ahead, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
			float dist_ahead2 = glm::distance(ahead2, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));

			if (dist_ahead <= (radiusObs + radius - 0.05f) || dist_ahead2 <= (radiusObs + radius - 0.05f))
			{
				if (!avoidance) avoidance = true;

				if (glm::distance(p1.getCurrentPosition(), glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f)) < mostThreateningDist)
				{
					mostThreateningVec = glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f);
					mostThreateningDist = glm::distance(p1.getCurrentPosition(), glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
				}
			}
		}
	}

	if (avoidance)
	{
		glm::vec3 avoidanceVec = glm::vec3(0.0f);
		avoidanceVec.x = ahead.x - mostThreateningVec.x;
		avoidanceVec.z = ahead.z - mostThreateningVec.z;

		avoidanceVec = maxAvoidForce * glm::normalize(avoidanceVec)* (1.0f / glm::distance(p1.getCurrentPosition(), mostThreateningVec));

		p1.setVelocity(p1.getVelocity() + avoidanceVec);
	}
}

void Physics::ComputeObsAvoidanceUnaligned(std::vector<Particle>& particles, Particle& p1, std::vector<GRectangle> obstacles, const int index, const float& radius, const float& radiusObs, const float& maxSeeAhead, const float& maxAvoidForce)
{
	float maxVel = 4.0f;
	glm::vec3 ahead = p1.getCurrentPosition() + glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity()) / maxVel);
	glm::vec3 ahead2 = p1.getCurrentPosition() + 0.5f * glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity()) / maxVel);

	glm::vec3 ahead_2 = p1.getCurrentPosition() + 0.5f * glm::cross(glm::normalize(p1.getVelocity()), glm::vec3(0.0f, 1.0f, 0.0f)) * (glm::length(p1.getVelocity()) / maxVel);
	glm::vec3 ahead_22 = p1.getCurrentPosition() + 0.5f * 0.5f * glm::cross(glm::normalize(p1.getVelocity()), glm::vec3(0.0f, 1.0f, 0.0f)) * (glm::length(p1.getVelocity()) / maxVel);

	glm::vec3 ahead_3 = p1.getCurrentPosition() + 0.5f * glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(p1.getVelocity())) * (glm::length(p1.getVelocity()) / maxVel);
	glm::vec3 ahead_32 = p1.getCurrentPosition() + 0.5f * 0.5f * glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(p1.getVelocity())) * (glm::length(p1.getVelocity()) / maxVel);

	glm::vec3 mostThreateningVec = glm::vec3(0.0f);
	float mostThreateningDist = std::numeric_limits<float>::max();
	bool avoidance = false;

	//Avoidance Particles!
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		if (i == index) continue;

		float dist_ahead = glm::distance(ahead, particles[i].getCurrentPosition());
		float dist_ahead2 = glm::distance(ahead2, particles[i].getCurrentPosition());
		float dist_ahead_2 = glm::distance(ahead_2, particles[i].getCurrentPosition());
		float dist_ahead_22 = glm::distance(ahead_22, particles[i].getCurrentPosition());
		float dist_ahead_3 = glm::distance(ahead_3, particles[i].getCurrentPosition());
		float dist_ahead_32 = glm::distance(ahead_32, particles[i].getCurrentPosition());

		if (dist_ahead <= 2.0f * radius || dist_ahead2 <= 2.0f * radius ||
			dist_ahead_2 <= 2.0f * radius || dist_ahead_22 <= 2.0f * radius	||
			dist_ahead_3 <= 2.0f * radius || dist_ahead_32 <= 2.0f * radius)
		{
			if (!avoidance) avoidance = true;

			if (glm::distance(p1.getCurrentPosition(), particles[i].getCurrentPosition()) < mostThreateningDist)
			{
				mostThreateningDist = glm::distance(p1.getCurrentPosition(), particles[i].getCurrentPosition());
				mostThreateningVec = particles[i].getCurrentPosition();
			}
		}
	}

	if (avoidance)
	{
		glm::vec3 avoidanceVec = glm::vec3(0.0f);
		avoidanceVec.x = ahead.x - mostThreateningVec.x;
		avoidanceVec.z = ahead.z - mostThreateningVec.z;

		avoidanceVec = maxAvoidForce * glm::normalize(avoidanceVec) * (1.0f / glm::distance(p1.getCurrentPosition(), mostThreateningVec));

		p1.setVelocity(p1.getVelocity() + avoidanceVec);
	}


	//Avoidance Walls
	ahead = p1.getCurrentPosition() + glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity()) / maxVel);
	ahead2 = p1.getCurrentPosition() + 0.5f * glm::normalize(p1.getVelocity()) * (glm::length(p1.getVelocity()) / maxVel);

	ahead_2 = p1.getCurrentPosition() + glm::cross(glm::normalize(p1.getVelocity()), glm::vec3(0.0f, 1.0f, 0.0f)) * (glm::length(p1.getVelocity()) / maxVel);
	ahead_22 = p1.getCurrentPosition() + 0.5f * glm::cross(glm::normalize(p1.getVelocity()), glm::vec3(0.0f, 1.0f, 0.0f)) * (glm::length(p1.getVelocity()) / maxVel);

	ahead_3 = p1.getCurrentPosition() + glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(p1.getVelocity())) * (glm::length(p1.getVelocity()) / maxVel);
	ahead_32 = p1.getCurrentPosition() + 0.5f * glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(p1.getVelocity())) * (glm::length(p1.getVelocity()) / maxVel);

	mostThreateningVec = glm::vec3(0.0f);
	mostThreateningDist = std::numeric_limits<float>::max();
	avoidance = false;

	for (unsigned int i = 0; i < obstacles.size(); i++)
	{
		if (glm::dot(glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f) - p1.getCurrentPosition(), p1.getVelocity()) > 0.1f)
		{
			float dist_ahead = glm::distance(ahead, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
			float dist_ahead2 = glm::distance(ahead2, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
			float dist_ahead_2 = glm::distance(ahead_2, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
			float dist_ahead_22 = glm::distance(ahead_22, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
			float dist_ahead_3 = glm::distance(ahead_3, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
			float dist_ahead_32 = glm::distance(ahead_32, glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));

			if (dist_ahead <= (radiusObs + radius - 0.05f) || dist_ahead2 <= (radiusObs + radius - 0.05f) ||
				dist_ahead_2 <= (radiusObs + radius - 0.05f) || dist_ahead_22 <= (radiusObs + radius - 0.05f) ||
				dist_ahead_3 <= (radiusObs + radius - 0.05f) || dist_ahead_32 <= (radiusObs + radius - 0.05f))
			{
				if (!avoidance) avoidance = true;

				if (glm::distance(p1.getCurrentPosition(), glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f)) < mostThreateningDist)
				{
					mostThreateningVec = glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f);
					mostThreateningDist = glm::distance(p1.getCurrentPosition(), glm::vec3(obstacles[i].pos.x + 0.5f, 0.0f, obstacles[i].pos.y + 0.5f));
				}
			}
		}
	}

	if (avoidance)
	{
		glm::vec3 avoidanceVec = glm::vec3(0.0f);
		avoidanceVec.x = ahead.x - mostThreateningVec.x;
		avoidanceVec.z = ahead.z - mostThreateningVec.z;

		avoidanceVec = maxAvoidForce * glm::normalize(avoidanceVec) * (1.0f / glm::distance(p1.getCurrentPosition(), mostThreateningVec));

		p1.setVelocity(p1.getVelocity() + avoidanceVec);
	}
}



glm::vec3 Physics::ComputeElasticForce(Particle& p1, Particle& p2, float elasticCoef, float stringLength)
{
	glm::vec3 posDif = p2.getCurrentPosition() - p1.getCurrentPosition();
	glm::vec3 dir = posDif / glm::length(posDif);
	glm::vec3 elasticForce = elasticCoef * (glm::length(posDif) - stringLength) * dir;

	//std::cout << "elastic" << elasticForce.x << " " << elasticForce.y << " " << elasticForce.z << " " << std::endl;

	return elasticForce;
}

glm::vec3 Physics::ComputeDampingForce(Particle& p1, Particle& p2, float dampingCoef)
{
	glm::vec3 posDif = p2.getCurrentPosition() - p1.getCurrentPosition();
	glm::vec3 vecDif = p2.getVelocity() - p1.getVelocity();
	glm::vec3 dir = posDif / glm::length(posDif);
	glm::vec3 dampingForce = dampingCoef * glm::dot(vecDif, dir) * dir;

	//std::cout << "damping" << dampingForce.x << " " << dampingForce.y << " " << dampingForce.z << " " << std::endl;

	return dampingForce;
}


void Physics::ComputeForces1D(std::vector<Particle>& particles, float elasticCoef, float dampingCoef, float stringLength)
{
	std::vector<glm::vec3> particlesForces;
	//particlesForces.resize(particles.size() * sizeof(float));

	for (unsigned int i = 0; i < particles.size(); i++)
	{
		particlesForces.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	for (unsigned int i = 0; i < particles.size(); i++)
	{
		if (i != 0) particlesForces[i] += glm::vec3(0.0, -9.8f, 0.0f);

		if (i < particles.size() - 1)
		{
			glm::vec3 elasticForce = ComputeElasticForce(particles[i], particles[i + 1], elasticCoef, stringLength);
			glm::vec3 dampingForce = ComputeDampingForce(particles[i], particles[i + 1], dampingCoef);

			particlesForces[i] += (elasticForce + dampingForce);
			particlesForces[i + 1] -= (elasticForce + dampingForce);
		}
		particles[i].setForce(particlesForces[i]);
	}

	particles[0].setForce(0.0f, 0.0f, 0.0f);
}

void Physics::ComputeForces2D(std::vector<Particle>& particles, int rows, int columns, float elasticCoef, float dampingCoef, float stringLength)
{
	std::vector<glm::vec3> particlesForces;
	//particlesForces.resize(particles.size() * sizeof(float));

	for (unsigned int i = 0; i < particles.size(); i++)
	{
		particlesForces.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	float stringDiagDist = sqrt(2 * pow(stringLength, 2));


	for (unsigned int i = 0; i < rows; i++)
	{
		for (unsigned int j = 0; j < columns; j++)
		{
			int numParticle = (columns * i) + j;
			int numParticlePlus_i = (columns * (i + 1)) + j;
			int numParticleMinus_i = (columns * (i - 1)) + j;
			int numParticlePlus_j = (columns * i) + (j + 1);
			int numParticleMinus_j = (columns * i) + (j - 1);
			int numParticlePlus_2i = (columns * (i + 2)) + j;
			int numParticleMinus_2i = (columns * (i - 2)) + j;
			int numParticlePlus_2j = (columns * i) + (j + 2);
			int numParticleMinus_2j = (columns * i) + (j - 2);
			int numParticleDiag1 = (columns * (i + 1)) + (j - 1);
			int numParticleDiag2 = (columns * (i - 1)) + (j + 1);
			int numParticleDiag3 = (columns * (i + 1)) + (j + 1);
			int numParticleDiag4 = (columns * (i - 1)) + (j - 1);

			//Gravity
			if (i != 0) particlesForces[numParticle] += glm::vec3(0.0, -9.8f / 2.0f, 0.0f);

			//Streach
			if (i < rows - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_i, elasticCoef, dampingCoef, stringLength);

			if (i > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_i, elasticCoef, dampingCoef, stringLength);

			if (j < columns - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_j, elasticCoef, dampingCoef, stringLength);

			if (j > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_j, elasticCoef, dampingCoef, stringLength);

			//Shear
			if (i < rows - 1 && j > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag1, elasticCoef, dampingCoef, stringDiagDist);

			if (i > 0 && j < columns - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag2, elasticCoef, dampingCoef, stringDiagDist);

			if (i < rows - 1 && j < columns - 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag3, elasticCoef, dampingCoef, stringDiagDist);

			if (i > 0 && j > 0)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleDiag4, elasticCoef, dampingCoef, stringDiagDist);

			//Bend
			if (i < rows - 2)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_2i, elasticCoef, dampingCoef, 2.0f * stringLength);

			if (i > 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_2i, elasticCoef, dampingCoef, 2.0f * stringLength);

			if (j < columns - 2)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticlePlus_2j, elasticCoef, dampingCoef, 2.0f * stringLength);

			if (j > 1)
				ComputeParticleForce(particles, particlesForces, numParticle, numParticleMinus_2j, elasticCoef, dampingCoef, 2.0f * stringLength);

		}

	}

	for (unsigned int i = 0; i < rows; i++)
	{
		for (unsigned int j = 0; j < columns; j++)
		{
			int numParticle = (columns * i) + j;
			if (i != 0) particles[numParticle].setForce(particlesForces[numParticle]);
			else particles[numParticle].setForce(0.0, 0.0, 0.0);
		}
	}
}

void Physics::ComputeParticleForce(std::vector<Particle>& particles, std::vector<glm::vec3>& particlesForces, int i, int j, float elasticCoef, float dampingCoef, float stringLength)
{
	glm::vec3 elasticForce = ComputeElasticForce(particles[i], particles[j], elasticCoef, stringLength);
	glm::vec3 dampingForce = ComputeDampingForce(particles[i], particles[j], dampingCoef);

	particlesForces[i] += (elasticForce + dampingForce);
	particlesForces[j] -= (elasticForce + dampingForce);
}

