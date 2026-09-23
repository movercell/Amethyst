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



struct EntityClassname("player_mainmenu") Entity_Player_MainMenu : public BaseEntity {
    ADFSerialize int stub = 0;
    Engine::Reference<Camera> PlayerCamera;

    void Initialize();
};

struct EntityClassname("info_player_start_mainmenu") Entity_PlayerStart_MainMenu : public Entity_Player {
    void Initialize();
    void Update() {};
};