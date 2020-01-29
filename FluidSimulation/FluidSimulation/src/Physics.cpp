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
			if (distance <= (radiusCollision + 0.01f))
			{
				//std::cout << "Hei coll between Part!" << std::endl;
				//p.setPosition(p.getCurrentPosition() - dt*p.getVelocity());
				//particles[i].setPosition(particles[i].getCurrentPosition() - dt*particles[i].getVelocity());

				p.setVelocity(-p.getVelocity());
				particles[i].setVelocity(-particles[i].getVelocity());
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
		p.setVelocity(newVelocity);

		if (solverUsed == 2)
		{
			glm::vec3 newPrevPos = p.getPreviousPosition() - (1.0f + p.getBouncing()) * (glm::dot(plane.normal, p.getPreviousPosition()) + plane.dconst) * plane.normal;
			p.setPreviousPosition(newPrevPos);
		}
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

glm::vec3 Physics::ComputePressureForce(Particle& p1, Particle* p2, const float& h)
{
	//float preassureForce = - p2->getMass() * (( p1.getPreasure()/ (pow(p1.getDensity(), 2))) + (p2->getPreasure() / (pow(p2->getDensity(), 2))));
	
	float preassureForce = (-1.0f)*((p1.getPreasure() + p2->getPreasure()) / 2.0f) * (p2->getMass() / p2->getDensity());

	float r = glm::distance(p1.getCurrentPosition(), p2->getCurrentPosition());
	glm::vec3 distVec = (p1.getCurrentPosition() - p2->getCurrentPosition());
	distVec = glm::normalize(distVec);
	glm::vec3 smoothingKer = glm::vec3(0.0f, 0.0f, 0.0f);
	if (r >= 0.0f && r <= h)
		smoothingKer = distVec *(float)((-1)*(45.f / (PI * pow(h, 6)))*pow(h - r, 2));
		//smoothingKer = (-45.0f / ((float)PI * pow(h, 6))) * pow((h - r), 2)*distVec; //kernel thesis
		//smoothingKer = (-945.0f / (32.0f * (float)PI * pow(h, 9))) * pow((pow(h, 2) - pow(r, 2)),2) * (p2->getCurrentPosition() - p1.getCurrentPosition());

	glm::vec3 pressForceTot = preassureForce * smoothingKer;

	return pressForceTot;
}

glm::vec3 Physics::ComputeViscosityForce(Particle& p1, Particle* p2, const float& visc, const float& h)
{
	 //glm::vec3 viscForce = visc * p2->getMass() * ((p2->getVelocity() - p1.getVelocity()) / (p1.getDensity() * p2->getDensity()));

	glm::vec3 viscForce = visc * (p2->getVelocity() - p1.getVelocity()) * (p2->getMass() / p2->getDensity());

	float r = glm::distance(p1.getCurrentPosition(), p2->getCurrentPosition());
	float smoothingKer = 0.0f;
	if (r >= 0.0f && r <= h)
		smoothingKer =(float)(45.0f / ((float)PI * pow(h, 6))) * (h - r); //kernel thesis
		//smoothingKer = (945.0f / (8.0f * PI * pow(h,9))) * (pow(h, 2) - pow(r, 2)) * (pow(r, 2) - (3.0f / 4.0f) * (pow(h, 2) - pow(r, 2)));

	glm::vec3 viscForceTot = viscForce * smoothingKer;

	return viscForceTot;
}

glm::vec3 Physics::ComputeCurvatureSurface(Particle& p1, Particle* p2, const float& r, const float& h)
{
	glm::vec3 distVec = (p1.getCurrentPosition() - p2->getCurrentPosition());
	distVec = glm::normalize(distVec);
	glm::vec3 smoothingKer = glm::vec3(0.0f, 0.0f, 0.0f);
	if (r >= 0.0f && r <= h)
	{
		smoothingKer = distVec * (float)((-1) * (45.f / (PI * pow(h, 6))) * pow(h - r, 2));
	}
	glm::vec3 norm = (p2->getMass() / p2->getDensity()) * smoothingKer;

	return norm;
}

float Physics::ComputeSmoothColor(Particle& p1, Particle* p2, const float& r, const float& h)
{
	float smoothingKer = 0.0f;
	if (r >= 0.0f && r <= h)
		smoothingKer = (float)(45.0f / ((float)PI * pow(h, 6))) * (h - r);

	float colorField = (p2->getMass() / p2->getDensity()) * smoothingKer;

	return colorField;
}

void Physics::ComputeFluidForce(Particle& p1, std::vector<Particle>& particles, const float& visc, const float& h, const int& index)
{
	glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pressForce = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 viscForce = glm::vec3(0.0f, 0.0f, 0.0f);

	float colorField = 0.0f;
	glm::vec3 normSurf = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<Particle*> nn = p1.getNeighbours();

	for (unsigned int i = 0; i < nn.size(); i++)
	{
		pressForce += ComputePressureForce(p1, nn[i], h);
		viscForce += ComputeViscosityForce(p1, nn[i], visc, h);

		//float r = glm::distance(p1.getCurrentPosition(), nn[i]->getCurrentPosition());
		//colorField += ComputeSmoothColor(p1, nn[i], r, h);
		//normSurf += ComputeCurvatureSurface(p1, nn[i], r, h);
	}

	//float supTensConst = 0.0728f;
	//if (glm::length(normSurf) >= 6.0f)
	//{
	//	glm::vec3 supTension = supTensConst * colorField * glm::normalize(normSurf);
	//	force += supTension;
	//}
		

	force += pressForce;
	force += viscForce;
	force += p1.getDensity()*glm::vec3(0.0f, -9.8f, 0.0f); //Thesis
	//force += glm::vec3(0.0f, -9.8f, 0.0f);

	//std::cout << force.x << " " << force.y << " " << force.z << std::endl;

	//force *= p1.getMass();

	p1.setForce(force/p1.getDensity());
}



