#include "MiscEntities.h"


void PointRotating::Update() {
    angles += rotationamount * deltaTime;

    if (angles.x > 360.0f) angles.x = 0.0f;
    if (angles.y > 360.0f) angles.y = 0.0f;
    if (angles.z > 360.0f) angles.z = 0.0f;

    if (angles.x < 0.0f) angles.x = 360.0f;
    if (angles.y < 0.0f) angles.y = 360.0f;
    if (angles.z < 0.0f) angles.z = 360.0f;

    rotation = quat(angles);
    BaseEntity::Update();
}