#pragma once

#include "engine/entities/Entity.h"
#include "engine/graphics/ModelInstance.h"


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