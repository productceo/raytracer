#include "src/raytrace.h"
#include "common/core.h"


std::shared_ptr<Camera> RayTrace::CreateCamera() const
{
    const glm::vec2 resolution = GetImageOutputResolution();
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(resolution.x / resolution.y, 36.0f);
    camera->SetPosition(glm::vec3(-5.0f, 4.5f, 10.0f)); // LocationX, LocationZ, -LocationY
    camera->Rotate(glm::vec3(1.f, 0.f, 0.f), -13.0f * PI / 180.f); // RotationX - 90
    camera->Rotate(glm::vec3(0.f, 1.f, 0.f), -25.0f * PI / 180.f); // RotationZ
    camera->Rotate(glm::vec3(0.f, 0.f, 1.f), 0.0f * PI / 180.f); // -RotationY
    return camera;
}


// 0 -- Naive.
// 1 -- BVH.
// 2 -- Grid.
#define ACCELERATION_TYPE 2


std::shared_ptr<Scene> RayTrace::CreateScene() const
{
    std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

    // Material
    std::shared_ptr<BlinnPhongMaterial> sceneMaterial = std::make_shared<BlinnPhongMaterial>();
    sceneMaterial->SetDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));
    sceneMaterial->SetSpecular(glm::vec3(0.0f, 0.0f, 0.0f), 10.f);
    sceneMaterial->SetReflectivity(0.0f);

    // Objects
    std::vector<std::shared_ptr<aiMaterial>> loadedMaterials;
    std::vector<std::shared_ptr<MeshObject>> meshObjects = MeshLoader::LoadMesh("junwon/junwon.obj", &loadedMaterials);
    for (size_t i = 0; i < meshObjects.size(); ++i) {
        std::shared_ptr<Material> materialCopy = sceneMaterial->Clone();
        materialCopy->LoadMaterialFromAssimp(loadedMaterials[i]);
        if (std::find(std::begin(bg_indices), std::end(bg_indices), i) != std::end(bg_indices)) {
            materialCopy->SetReflectivity(0.0f);
            materialCopy->SetTransmittance(0.0f);
        } 
        else if (std::find(std::begin(glass_indices), std::end(glass_indices), i) != std::end(glass_indices)) {
            materialCopy->SetReflectivity(0.01f);
            materialCopy->SetTransmittance(0.99f);
        }
        else if (std::find(std::begin(marble_indices), std::end(marble_indices), i) != std::end(marble_indices)) {
            materialCopy->SetReflectivity(0.40f);
        }
        else if (std::find(std::begin(floor_indices), std::end(floor_indices), i) != std::end(floor_indices)) {
            materialCopy->SetReflectivity(0.25f);
        }
        else if (std::find(std::begin(gloss_indices), std::end(gloss_indices), i) != std::end(gloss_indices)) {
            materialCopy->SetReflectivity(0.20f);
        } 
        else if (std::find(std::begin(metal_indices), std::end(metal_indices), i) != std::end(metal_indices)) {
            materialCopy->SetReflectivity(0.20f);
        } 
        else if (std::find(std::begin(black_indices), std::end(black_indices), i) != std::end(black_indices)) {
            materialCopy->SetReflectivity(0.10f);
        } 
        else if (std::find(std::begin(matte_indices), std::end(matte_indices), i) != std::end(matte_indices)) {
            materialCopy->SetReflectivity(0.05f);
            materialCopy->SetTransmittance(0.75f);
        } 
        // else { materialCopy = sceneMaterial->Clone(); } // For Development
        meshObjects[i]->SetMaterial(materialCopy);

        std::shared_ptr<SceneObject> meshSceneObject = std::make_shared<SceneObject>();
        meshSceneObject->AddMeshObject(meshObjects[i]);

        meshSceneObject->CreateAccelerationData(AccelerationTypes::BVH);
        meshSceneObject->ConfigureAccelerationStructure([](AccelerationStructure* genericAccelerator) {
            BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
            accelerator->SetMaximumChildren(2);
            accelerator->SetNodesOnLeaves(2);
        });

        meshSceneObject->ConfigureChildMeshAccelerationStructure([](AccelerationStructure* genericAccelerator) {
            BVHAcceleration* accelerator = dynamic_cast<BVHAcceleration*>(genericAccelerator);
            accelerator->SetMaximumChildren(2);
            accelerator->SetNodesOnLeaves(2);
        });
        newScene->AddSceneObject(meshSceneObject);
    }

    // Lights
    // Light: Point light outside for background
    std::shared_ptr<Light> pointLight1 = std::make_shared<PointLight>();
    pointLight1->SetPosition(glm::vec3(5.86936f, -10.62969f, 10.2964f)); // LocationX, LocationZ, -LocationY
    pointLight1->SetLightColor(glm::vec3(1.f, 0.584f, 0.262f));
    newScene->AddLight(pointLight1);

    // // Light: Point light outside for background reflection
    std::shared_ptr<Light> pointLight2 = std::make_shared<PointLight>();
    pointLight2->SetPosition(glm::vec3(5.18871f, -0.772113f, -0.976246f)); // LocationX, LocationZ, -LocationY
    pointLight2->SetLightColor(glm::vec3(1.f, 1.f, 1.f));
    newScene->AddLight(pointLight2);

    // Light: Area light from the ceiling
    std::shared_ptr<Light> areaLight = std::make_shared<AreaLight>(glm::vec2(10.f, 10.f));
    areaLight->SetPosition(glm::vec3(-1.10181f, 7.2176f, 5.49423f)); // LocationX, LocationZ, -LocationY
    areaLight->SetLightColor(glm::vec3(1.f, 0.735f, 0.604f));
    newScene->AddLight(areaLight);

    // // Light: Spot lights above the TV
    std::shared_ptr<Light> spotLight1 = std::make_shared<SpotLight>(0.35f, 0.25f);
    spotLight1->SetPosition(glm::vec3(-2.427f, 5.05283f, -0.1378f)); // LocationX, LocationZ, -LocationY
    spotLight1->SetLightColor(glm::vec3(1.f, 0.584f, 0.262f));
    spotLight1->Rotate(glm::vec3(1.f, 0.f, 0.f), -90.0f * PI / 180.f); // RotationX - 90
    newScene->AddLight(spotLight1);

    std::shared_ptr<Light> spotLight2 = std::make_shared<SpotLight>(0.35f, 0.25f);
    spotLight2->SetPosition(glm::vec3(0.6454f, 5.05283f, -0.1378f)); // LocationX, LocationZ, -LocationY
    spotLight2->SetLightColor(glm::vec3(1.f, 0.584f, 0.262f));
    spotLight2->Rotate(glm::vec3(1.f, 0.f, 0.f), -90.0f * PI / 180.f); // RotationX - 90
    newScene->AddLight(spotLight2);

    // Light: Directional light from the Sun
    std::shared_ptr<Light> sunLight = std::make_shared<AreaLight>(glm::vec2(1.0f, 1.0f));
    sunLight->SetPosition(glm::vec3(15.0f, 7.0f, 12.0f)); // LocationX, LocationZ, -LocationY
    sunLight->SetLightColor(glm::vec3(1.0f, 0.5f, 0.3f));
    sunLight->Rotate(glm::vec3(1.f, 0.f, 0.f), -130.0f * PI / 180.f); // RotationX - 90
    sunLight->Rotate(glm::vec3(0.f, 1.f, 0.f), -90.0f * PI / 180.f); // RotationZ
    sunLight->Rotate(glm::vec3(0.f, 0.f, 1.f), -75.0f * PI / 180.f); // -RotationY
    newScene->AddLight(sunLight);


#if ACCELERATION_TYPE == 0
    newScene->GenerateAccelerationData(AccelerationTypes::NONE);
#elif ACCELERATION_TYPE == 1
    newScene->GenerateAccelerationData(AccelerationTypes::BVH);
#else
    UniformGridAcceleration* accelerator = dynamic_cast<UniformGridAcceleration*>(newScene->GenerateAccelerationData(AccelerationTypes::UNIFORM_GRID));
    assert(accelerator);
    accelerator->SetSuggestedGridSize(glm::ivec3(10, 10, 10));
#endif    

    return newScene;

}
std::shared_ptr<ColorSampler> RayTrace::CreateSampler() const
{
    std::shared_ptr<JitterColorSampler> jitter = std::make_shared<JitterColorSampler>();
    jitter->SetGridSize(glm::ivec3(1, 1, 1));
    return jitter;
}

std::shared_ptr<class Renderer> RayTrace::CreateRenderer(std::shared_ptr<Scene> scene, std::shared_ptr<ColorSampler> sampler) const
{
    return std::make_shared<BackwardRenderer>(scene, sampler);
}

int RayTrace::GetSamplesPerPixel() const
{
    return 16;
}

bool RayTrace::NotifyNewPixelSample(glm::vec3 inputSampleColor, int sampleIndex)
{
    return true;
}

int RayTrace::GetMaxReflectionBounces() const
{
    return 2;
}

int RayTrace::GetMaxRefractionBounces() const
{
    return 4;
}

glm::vec2 RayTrace::GetImageOutputResolution() const
{
    return glm::vec2(1920.f, 1080.f);
}
