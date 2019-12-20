#include "common/Scene/Lights/Point/PointLight.h"


void PointLight::ComputeSampleRays(std::vector<Ray>& output, glm::vec3 origin, glm::vec3 normal) const
{
    origin += normal * LARGE_EPSILON;
    const glm::vec3 lightPosition = glm::vec3(GetPosition());
    const glm::vec3 rayDirection = glm::normalize(lightPosition - origin);
    const float distanceToOrigin = glm::distance(origin, lightPosition);
    output.emplace_back(origin, rayDirection, distanceToOrigin);
}

float PointLight::ComputeLightAttenuation(glm::vec3 origin) const
{
    return 1.f;
}

void PointLight::GenerateRandomPhotonRay(Ray& ray) const
{
    float x;
    float y;
    float z;
    do {
        x = (double) rand() / RAND_MAX * 2 - 1;
        y = (double) rand() / RAND_MAX * 2 - 1;
        z = (double) rand() / RAND_MAX * 2 - 1;
    } while (x * x + y * y + z * z > 1);

    ray.SetRayPosition(glm::vec3(this->position));
    ray.SetRayDirection(glm::normalize(glm::vec3(x, y, z)));
}
