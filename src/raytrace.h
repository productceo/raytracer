#pragma once

#include "common/Application.h"

class RayTrace : public Application
{
public:
    virtual std::shared_ptr<class Camera> CreateCamera() const override;
    virtual std::shared_ptr<class Scene> CreateScene() const override;
    virtual std::shared_ptr<class ColorSampler> CreateSampler() const override;
    virtual std::shared_ptr<class Renderer> CreateRenderer(std::shared_ptr<class Scene> scene, std::shared_ptr<class ColorSampler> sampler) const override;
    virtual int GetSamplesPerPixel() const override;
    virtual bool NotifyNewPixelSample(glm::vec3 inputSampleColor, int sampleIndex) override;
    virtual int GetMaxReflectionBounces() const override;
    virtual int GetMaxRefractionBounces() const override;
    virtual glm::vec2 GetImageOutputResolution() const override;
private:
    int bg_indices[2] = {1082, 1083};
    int black_indices[7] = {21, 22, 23, 24, 30, 32, 1085};
    int floor_indices[1] = {19};
    int glass_indices[14] = {0, 2, 4, 6, 8, 10, 11, 12, 1072, 1073, 1074, 1075, 1076, 1077};
    int gloss_indices[2] = {13, 1050};
    int marble_indices[2] = {1055, 1057};
    int matte_indices[16] = {17, 18, 20, 25, 26, 27, 28, 29, 1046, 1049, 1051, 1058, 1066, 1078, 1080, 1084};
    int metal_indices[2] = {33, 36};
};
