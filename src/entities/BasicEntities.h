#pragma once

#include "engine/entities/Entity.h"
#include "engine/graphics/ModelInstance.h"
#include "engine/graphics/Light.h"


template<typename T>
struct BasicVisiblePropHandler : public BaseEntityHandler<T> {
    using BaseEntityHandler<T>::BaseEntityHandler;
    using Base = BaseEntityHandler<T>;
    using Parent = BaseEntityHandler<T>;

    static void PropertyInit() {
        Base::AddProperty("model", &T::model);

        Parent::PropertyInit();
    }
};

struct DynamicProp : public BaseEntity {
    std::string model;
    std::unique_ptr<ModelInstance> modelinstance;

    void Initialize();
    void Update();
};

template<typename T>
struct LightSpotHandler : public BaseEntityHandler<T> {
    using BaseEntityHandler<T>::BaseEntityHandler;
    using Base = BaseEntityHandler<T>;
    using Parent = BaseEntityHandler<T>;

    static void PropertyInit() {
        Base::AddProperty("near", &T::near);
        Base::AddProperty("far", &T::far);
        Base::AddProperty("resolution", &T::resolution);
        Base::AddProperty("constantfalloff", &T::constantfalloff);
        Base::AddProperty("linearfalloff", &T::linearfalloff);
        Base::AddProperty("quadraticfalloff", &T::quadraticfalloff);
        Base::AddProperty("intensity", &T::intensity);
        Base::AddProperty("color", &T::color);

        Base::AddProperty("innerfalloffangle", &T::innerfalloffangle);
        Base::AddProperty("outerfalloffangle", &T::outerfalloffangle);

        Base::AddProperty("isconstantlyupdating", &T::isconstantlyupdating);

        Parent::PropertyInit();
    }
};

struct LightSpot : public BaseEntity {
    float near = 1.0f;
    float far = 256.0f;
    vec2 resolution;
    float constantfalloff = 0.0f;
    float linearfalloff = 0.0f;
    float quadraticfalloff = 1.0f;
    float intensity = 0.0f;
    vec3 color;

    float innerfalloffangle = 0.0f;
    float outerfalloffangle = 0.0f;

    bool isconstantlyupdating = false;

    Engine::Reference<Light> light;

    void Initialize();
    void Update();
};