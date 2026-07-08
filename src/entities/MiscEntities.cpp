#include "MiscEntities.h"


void PointRotating::Update() {
    rotation *= quat(rotationamount * deltaTime);
    rotation.Norm();

    BaseEntity::Update();
}