#pragma once

#include "engine/entities/Entity.h"
#include "engine/graphics/ModelInstance.h"


template<typename T>
struct DynamicPropHandler : public BaseEntityHandler<T> {
    using BaseEntityHandler<T>::BaseEntityHandler;
    using Base = BaseEntityHandler<T>;

    static void PropertyInit() {
        Base::AddProperty("model", &T::model);

        Base::PropertyInit();
    }
};

struct DynamicProp : public BaseEntity {
    std::string model;
    std::unique_ptr<ModelInstance> modelinstance;

    void Init();
    void Update();
};