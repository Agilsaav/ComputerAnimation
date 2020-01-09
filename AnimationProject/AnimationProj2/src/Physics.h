#include <vector>
#include "Particle.h"
#include "Geometry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Physics
{
public:
	Physics();
	~Physics();

	void GetPlaneCollisions(Particle& p, std::vector<Plane>& planes, std::vector<float>& distBefore, std::vector<float>& distNew, float& frictionCoef, int& solverUsed);
	void GetTriangleCollision(Particle& p, Triangle& tri, float& distBefore, float& distNew, float& frictionCoef, int& solverUsed);
	void GetParticleCollisions(std::vector<Particle>& particles, Particle& p, int particleIndex, float& radiusCollision, float dt);
	void GetSpehereCollision(Particle& p, Sphere& sphere, float& frictionCoef, int& solverUsed);
	void GetSphereSphereCollision(Particle& p, Particle& p2, Sphere& sphere, float& frictionCoef, int& solverUsed);
	void GetRectangleCircleColision(Particle& p, GRectangle& rectangle, const float& radius, float& frictionCoef, int& solverUsed);

	void ComputeObsAvoidance(std::vector<Particle>& particles, Particle& p1, std::vector<GRectangle> obstacles, const int index, const float& radius, const float& radiusObs, const float& maxSeeAhead, const float& maxAvoidForce);
	void ComputeObsAvoidanceUnaligned(std::vector<Particle>& particles, Particle& p1, std::vector<GRectangle> obstacles, const int index, const float& radius, const float& radiusObs, const float& maxSeeAhead, const float& maxAvoidForce);

	void ComputeForces1D(std::vector<Particle>& particles, float elasticCoef, float dampingCoef, float stringLength);
	void ComputeForces2D(std::vector<Particle>& particles, int rows, int columns, float elasticCoef, float dampingCoef, float stringLength);
	glm::vec3 ComputeDampingForce(Particle& p1, Particle& p2, float dampingCoef);
	glm::vec3 ComputeElasticForce(Particle& p1, Particle& p2, float elasticCoef, float stringLength);
	void ComputeParticleForce(std::vector<Particle>& particles, std::vector<glm::vec3>& particlesForces, int i, int j, float elasticCoef, float dampingCoef, float stringLength);

private:
};