#include "BasicEntities.h"

namespace EngineEntities {

    void DynamicProp::Initialize() {
        BaseEntity::Initialize();

        modelinstance = world->GetRWorld()->MakeModelInstance(model);
    }

    void DynamicProp::Update() {
        BaseEntity::Update();

        modelinstance->SetMatrix(TransformationMatrix);
    }

    void LightSpot::Initialize() {
        BaseEntity::Initialize();

        light = world->GetRWorld()->MakeSpotLight(resolution, innerfalloffangle, outerfalloffangle, color, near, far);
        light->SetPosition(position);
        light->SetRotation(rotation);
        light->SetIntensity(intensity);
        light->SetFalloff(constantfalloff, linearfalloff, quadraticfalloff);
    }

    void LightSpot::Update() {
        BaseEntity::Update();

        if (isconstantlyupdating) {
            light->SetPosition(position);
            light->SetRotation(rotation);
        }
    }
}