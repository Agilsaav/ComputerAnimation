#include "Particle.h"

#include <iostream>


Particle::Particle()
{
}

Particle::Particle(const float& x, const float& y, const float& z) :
m_previousPosition(0, 0, 0), m_velocity(0, 0, 0), m_force(0, 0, 0), m_bouncing(1), m_lifetime(50), m_fixed(false)
{
	m_currentPosition.x = x;
	m_currentPosition.y = y;
	m_currentPosition.z = z;

}

/*
Particle::Particle(glm::vec3 pos, glm::vec3 vel, float bouncing, bool fixed, int lifetime, glm::vec3 force) :
m_currentPosition(pos), m_previousPosition(pos), m_force(force), m_velocity(vel), m_bouncing(bouncing), m_lifetime(lifetime), m_fixed(fixed)
{
}
*/

Particle::~Particle()
{
}

//setters
void Particle::setPosition(const float& x, const float& y, const float& z)
{
	glm::vec3 pos(x,y,z);
	m_currentPosition =  pos;
}
void Particle::setPosition(glm::vec3 pos)
{
	m_currentPosition = pos;
}

void Particle::setPreviousPosition(const float& x, const float& y, const float& z)
{
	glm::vec3 pos(x, y, z);
	m_previousPosition = pos;
}

void Particle::setPreviousPosition(glm::vec3 pos)
{
	m_previousPosition = pos;
}

void Particle::setForce(const float& x, const float& y, const float& z)
{
	glm::vec3 force(x, y, z);
	m_force = force;
}

void Particle::setForce(glm::vec3 force)
{
	m_force = force;
}

void Particle::setMass(const float& partMass)
{
	m_mass = partMass;
}

void Particle::setDensity(const float& den)
{
	m_density = den;
}

void Particle::setPreasure(const float& pre)
{
	m_preasure = pre;
}

void Particle::addForce(const float& x, const float& y, const float& z)
{
	glm::vec3 force(x,y,z);
	m_force += force;
}

void Particle::computeParticleMass(const float& fluidDen, const float& Vol, const int& numPart)
{
	m_mass = (float)((fluidDen * Vol) / numPart);
}

void Particle::computeParticleDensity (const float& h, const float& fluidDen)
{
	m_density = 0.0f;
	float selfWeight = 315.0f / (64.0f * (float)PI * pow(h, 6));

	////Itself:
	//m_density += m_mass * selfWeight;

	//for (unsigned int i = 0; i < m_nn.size(); i++)
	//{
	//	float r = glm::distance(m_currentPosition, m_nn[i]->getCurrentPosition());
	//	float weight = (315.0f / (64.0f * (float)PI * pow(h, 9))) * pow((pow(h, 2) - pow(r, 2)), 3);
	//	m_density += weight * m_nn[i]->getMass();
	//}

	m_density += fluidDen;

	for (unsigned int i = 0; i < m_nn.size(); i++)
	{
		float r = glm::distance(m_currentPosition, m_nn[i]->getCurrentPosition());
		float weight = (315.0f / (64.0f * (float)PI * pow(h, 9))) * pow((pow(h, 2) - pow(r, 2)), 3);
		m_density += weight * m_nn[i]->getMass();
	}


}

void Particle::computePreasure(const float& fluidDen, const float& Vol, const float& k)
{
	//m_preasure = pow(soundSpeed,2) * (m_density - fluidDen);
	m_preasure =(float)(k / Vol) - (float)(k * fluidDen);
}

void Particle::computeNN(std::vector<Particle>& particles, const float& dist, unsigned int index)
{
	m_nn.clear();

	for (unsigned int j = 0; j < particles.size(); j++)
	{
		if (j != index)
		{
			float distance = glm::distance(m_currentPosition, particles[j].getCurrentPosition());			
			if (distance < dist) {
				Particle* part = &particles[j];
				m_nn.push_back(part);
			}
		}
	}
}

void Particle::addForce(glm::vec3 force)
{
	m_force += force;
}

void Particle::setVelocity(const float& x, const float& y, const float& z)
{
	glm::vec3 vel(x,y,z);
	m_velocity = vel;
}

void Particle::setVelocity(glm::vec3 vel)
{
	m_velocity = vel;
}

void Particle::setBouncing(float bouncing)
{
	m_bouncing = bouncing;
}

void Particle::setLifetime(float lifetime)
{
	m_lifetime = lifetime;
}

void Particle::setFixed(bool fixed)
{
	m_fixed = fixed;
}

//getters
glm::vec3 Particle::getCurrentPosition()
{
	return m_currentPosition;
}

glm::vec3 Particle::getPreviousPosition()
{
	return m_previousPosition;
}

glm::vec3 Particle::getForce()
{
	return m_force;
}

glm::vec3 Particle::getVelocity()
{
	return m_velocity;
}

std::vector<Particle*> Particle::getNeighbours()
{
	return m_nn;
}

float Particle::getMass()
{
	return m_mass;
}

float Particle::getDensity()
{
	return m_density;
}

float Particle::getPreasure()
{
	return m_preasure;
}

float Particle::getBouncing()
{
	return m_bouncing;
}

float Particle::getLifetime()
{
	return m_lifetime;
}

bool Particle::isFixed()
{
	return m_fixed;
}

void Particle::updateParticle(const float& dt, UpdateMethod method)
{
		switch (method)
		{
		case UpdateMethod::EulerOrig:
		{
			m_previousPosition = m_currentPosition;
			m_currentPosition += m_velocity*dt;
			m_velocity += m_force*dt;
		}
			break;
		case UpdateMethod::EulerSemi:
		{
			m_previousPosition = m_currentPosition;
			m_velocity += m_force * dt;
			m_currentPosition += m_velocity * dt;								 
		}
			break;
		case UpdateMethod::Verlet:
		{			
			glm::vec3 currpos = m_currentPosition;			
			m_currentPosition += 0.99f * (m_currentPosition - m_previousPosition) + (float)pow(dt, 2) * m_force;
			m_velocity = (m_currentPosition - currpos)/dt;		
			m_previousPosition = currpos;
		}
			break;
		}
	return;
}
