#include "BasicEntities.h"


void DynamicProp::Init() {
    modelinstance = world->GetRWorld()->MakeModelInstance(model);
}

void DynamicProp::Update() {
    modelinstance->SetMatrix(mat4());
}