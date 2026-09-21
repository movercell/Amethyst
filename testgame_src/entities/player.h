#pragma once

#include "engine/entities/Entity.h"

struct EntityClassname("player") Entity_Player : public BaseEntity {
    ADFSerialize float pitch = 0.0f;
    ADFSerialize float yaw = 0.0f;
    Engine::Reference<Camera> PlayerCamera;

    void Initialize();
    void Update();
};

struct EntityClassname("info_player_start") Entity_PlayerStart : public BaseEntity {
    void Initialize();
};