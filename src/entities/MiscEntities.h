#pragma once

#include "engine/entities/Entity.h"
namespace EngineEntities {

    struct EntityClass("point_rotating") PointRotating : public BaseEntity {
        ADFSerialize vec3 rotationamount;

        void Update();
    };
}