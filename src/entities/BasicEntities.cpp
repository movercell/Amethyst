#include "BasicEntities.h"


void DynamicProp::Initialize() {
    BaseEntity::Initialize();

    modelinstance = world->GetRWorld()->MakeModelInstance(model);
}

void DynamicProp::Update() {
    BaseEntity::Update();
    
    modelinstance->SetMatrix(TransformationMatrix);
}