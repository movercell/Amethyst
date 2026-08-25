#pragma once

#include "engine/entities/Entity.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/graphics/Light.h"

namespace EngineEntities {

    struct EntityClassname("info_target") Target : public BaseEntity {};

    struct EntityClassname("prop_dynamic") DynamicProp : public BaseEntity {
        ADFSerialize std::string model;
        std::unique_ptr<ModelInstance> modelinstance;

        void Initialize();
        void Update();
    };

    struct EntityClassname("light_spot") LightSpot : public BaseEntity {
        Engine::Reference<Light> light;

        ADFSerialize float near = 1.0f;
        ADFSerialize float far = 256.0f;
        ADFSerialize vec2 resolution;
        ADFSerialize float constantfalloff = 0.0f;
        ADFSerialize float linearfalloff = 0.0f;
        ADFSerialize float quadraticfalloff = 1.0f;
        ADFSerialize float intensity = 0.0f;
        ADFSerialize vec3 color;

        ADFSerialize float innerfalloffangle = 0.0f;
        ADFSerialize float outerfalloffangle = 0.0f;

        ADFSerialize bool isconstantlyupdating = false;

        void Initialize();
        void Update();
    };
}