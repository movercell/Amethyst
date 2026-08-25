#include "MiscEntities.h"

namespace EngineEntities {

    void PointRotating::Update() {
        rotation *= quat(rotationamount * deltaTime);
        rotation.Norm();

        BaseEntity::Update();
    }
}