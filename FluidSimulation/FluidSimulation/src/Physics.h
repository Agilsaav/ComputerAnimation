#include <vector>
#include "Particle.h"
#include "Geometry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>

//const double PI = 3.14159265358979323846;

class Physics
{
public:
	Physics();
	~Physics();

	//Collisions
	void GetPlaneCollisions(Particle& p, std::vector<Plane>& planes, std::vector<float>& distBefore, std::vector<float>& distNew, float& frictionCoef, int& solverUsed);
	void GetTriangleCollision(Particle& p, Triangle& tri, float& distBefore, float& distNew, float& frictionCoef, int& solverUsed);
	void GetParticleCollisions(std::vector<Particle>& particles, Particle& p, int particleIndex, float& radiusCollision, float dt);
	void GetSpehereCollision(Particle& p, Sphere& sphere, float& frictionCoef, int& solverUsed);


	//Forces
	void ComputeForces1D(std::vector<Particle>& particles, float elasticCoef, float dampingCoef, float stringLength);
	void ComputeForces2D(std::vector<Particle>& particles, int rows, int columns, float elasticCoef, float dampingCoef, float stringLength);
	void ComputeParticleForce(std::vector<Particle>& particles, std::vector<glm::vec3>& particlesForces, int i, int j, float elasticCoef, float dampingCoef, float stringLength);
	void ComputeFluidForce(Particle& p1, std::vector<Particle>& particles, const float& visc, const float& h, const int& index);

private:
	glm::vec3 ComputeDampingForce(Particle& p1, Particle& p2, float dampingCoef);
	glm::vec3 ComputeElasticForce(Particle& p1, Particle& p2, float elasticCoef, float stringLength);
	glm::vec3 ComputePressureForce(Particle& p1,Particle* p2, const float& h);
	glm::vec3 ComputeViscosityForce(Particle& p1, Particle* p2, const float& visc, const float& h);
	glm::vec3 ComputeCurvatureSurface(Particle& p1, Particle* p2, const float& r, const float& h);
	float ComputeSmoothColor(Particle& p1, Particle* p2, const float& r, const float& h);

};