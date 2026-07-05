#pragma once

#include "engine/entities/Entity.h"

template<typename T>
struct PointRotatingHandler : public BaseEntityHandler<T> {
    using BaseEntityHandler<T>::BaseEntityHandler;
    using Base = BaseEntityHandler<T>;
    using Parent = BaseEntityHandler<T>;

    static void PropertyInit() {
        Base::AddProperty("rotationamount", &T::rotationamount);

        Parent::PropertyInit();
    }
};

struct PointRotating : public BaseEntity {
    vec3 rotationamount;

    void Update();
};
