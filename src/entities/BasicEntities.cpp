#include "BasicEntities.h"


void DynamicProp::Initialize() {
    modelinstance = world->GetRWorld()->MakeModelInstance(model);

    BaseEntity::Initialize();
}

void DynamicProp::Update() {
    modelinstance->SetMatrix(MakeTransformationMatrix());

    BaseEntity::Update();
}