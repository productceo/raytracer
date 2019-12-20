#include "common/Rendering/Renderer/Photon/PhotonMappingRenderer.h"
#include "common/Scene/Scene.h"
#include "common/Sampling/ColorSampler.h"
#include "common/Scene/Lights/Light.h"
#include "common/Scene/Geometry/Primitives/Primitive.h"
#include "common/Scene/Geometry/Mesh/MeshObject.h"
#include "common/Rendering/Material/Material.h"
#include "common/Intersection/IntersectionState.h"
#include "common/Scene/SceneObject.h"
#include "common/Scene/Geometry/Mesh/MeshObject.h"
#include "common/Rendering/Material/Material.h"
#include "glm/gtx/component_wise.hpp"

#define VISUALIZE_PHOTON_MAPPING 1

PhotonMappingRenderer::PhotonMappingRenderer(std::shared_ptr<class Scene> scene, std::shared_ptr<class ColorSampler> sampler):
    BackwardRenderer(scene, sampler), 
    diffusePhotonNumber(1000000),
    maxPhotonBounces(1000)
{
    srand(static_cast<unsigned int>(time(NULL)));
}

void PhotonMappingRenderer::InitializeRenderer()
{
    // Generate Photon Maps
    GenericPhotonMapGeneration(diffuseMap, diffusePhotonNumber);
    diffuseMap.optimise();
}

void PhotonMappingRenderer::GenericPhotonMapGeneration(PhotonKdtree& photonMap, int totalPhotons)
{
    float totalLightIntensity = 0.f;
    size_t totalLights = storedScene->GetTotalLights();
    for (size_t i = 0; i < totalLights; ++i) {
        const Light* currentLight = storedScene->GetLightObject(i);
        if (!currentLight) {
            continue;
        }
        totalLightIntensity += glm::length(currentLight->GetLightColor());
    }

    // Shoot photons -- number of photons for light is proportional to the light's intensity relative to the total light intensity of the scene.
    for (size_t i = 0; i < totalLights; ++i) {
        const Light* currentLight = storedScene->GetLightObject(i);
        if (!currentLight) {
            continue;
        }

        const float proportion = glm::length(currentLight->GetLightColor()) / totalLightIntensity;
        const int totalPhotonsForLight = static_cast<const int>(proportion * totalPhotons);
        const glm::vec3 photonIntensity = currentLight->GetLightColor() / static_cast<float>(totalPhotonsForLight);
        for (int j = 0; j < totalPhotonsForLight; ++j) {
            Ray photonRay;
            std::vector<char> path;
            path.push_back('L');
            currentLight->GenerateRandomPhotonRay(photonRay);
            TracePhoton(photonMap, &photonRay, photonIntensity, path, 1.f, maxPhotonBounces);
        }
    }
}

void PhotonMappingRenderer::TracePhoton(PhotonKdtree& photonMap, Ray* photonRay, glm::vec3 lightIntensity, std::vector<char>& path, float currentIOR, int remainingBounces)
{
    /*
     * Assignment 8 TODO: Trace a photon into the scene and make it bounce.
     *    
     *    How to insert a 'Photon' struct into the photon map.
     *        Photon myPhoton;
     *        ... set photon properties ...
     *        photonMap.insert(myPhoton);
     */

    assert(photonRay);
    if (remainingBounces <= 0) return;
    IntersectionState state(0, 0);
    state.currentIOR = currentIOR;
    
    bool hit = storedScene->Trace(photonRay, &state);
    if(!hit) return;
    const glm::vec3 intersectionPoint = state.intersectionRay.GetRayPosition(state.intersectionT);

    if(path.size() > 1){
        Photon myPhoton;
        myPhoton.intensity = lightIntensity;
        myPhoton.toLightRay.SetRayPosition(glm::vec3(photonRay->GetPosition()));
//        glm::vec3 dir = photonRay->GetRayDirection();
//        myPhoton.toLightRay.SetRayDirection(-1.0f * dir);
        myPhoton.toLightRay.SetMaxT(photonRay->GetMaxT());
        myPhoton.position = intersectionPoint;
        photonMap.insert(myPhoton);
    }
    
    const MeshObject* hitMeshObject = state.intersectedPrimitive->GetParentMeshObject();
    const Material* hitMaterial = hitMeshObject->GetMaterial();
    glm::vec3 color = hitMaterial->GetBaseDiffuseReflection();

    float pr;
    if(color.x > color.y) pr = color.x;
    else pr = color.y;
    if(color.z > pr) pr = color.z;

    float n = (((float) rand()) / (float) RAND_MAX);
    if(n < pr){
        float u1 = ((float) rand()) / (float) RAND_MAX;
        float u2 = ((float) rand()) / (float) RAND_MAX;
        float r = sqrt(u1), theta = 2 * PI * u2;
        glm::vec3 dray = glm::normalize(glm::vec3(r * cos(theta), r * sin(theta), sqrt(1 - u1)));
    
        glm::vec3 n = state.ComputeNormal();
        glm::vec3 t;
        if(abs(abs(glm::dot(n, glm::vec3(1, 0, 0))) - 1) < 0.0001) t = glm::cross(n, glm::vec3(0, 1, 0));
        else t = glm::cross(n, glm::vec3(1, 0, 0));
        glm::vec3 b = glm::cross(n, t);
        glm::mat3 trans;
        trans[0] = t;
        trans[1] = b;
        trans[2] = n;
        
        dray = glm::transpose(trans) * dray;
        path.push_back('L');
        glm::vec3 diffuse = dray * trans;
        Ray newRay;
        newRay = Ray(intersectionPoint + normalize(diffuse) * 0.0001f, diffuse);
//        photonRay->SetRayDirection(dray);
//        photonRay->SetPosition(state.intersectionRay.GetRayPosition(state.intersectionT));
        TracePhoton(photonMap, &newRay, lightIntensity, path, currentIOR, remainingBounces - 1);
    }

}

glm::vec3 PhotonMappingRenderer::ComputeSampleColor(const struct IntersectionState& intersection, const class Ray& fromCameraRay) const
{
    glm::vec3 finalRenderColor = BackwardRenderer::ComputeSampleColor(intersection, fromCameraRay);
#if VISUALIZE_PHOTON_MAPPING
    Photon intersectionVirtualPhoton;
    intersectionVirtualPhoton.position = intersection.intersectionRay.GetRayPosition(intersection.intersectionT);

    std::vector<Photon> foundPhotons;
    diffuseMap.find_within_range(intersectionVirtualPhoton, 0.003f, std::back_inserter(foundPhotons));
    if (!foundPhotons.empty()) {
        finalRenderColor += glm::vec3(1.f, 0.f, 0.f);
    }
#endif
    return finalRenderColor;
}

void PhotonMappingRenderer::SetNumberOfDiffusePhotons(int diffuse)
{
    diffusePhotonNumber = diffuse;
}
