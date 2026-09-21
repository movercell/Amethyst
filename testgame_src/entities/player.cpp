#include "player.h"

void Entity_Player::Initialize() {
    PlayerCamera = world->GetRWorld()->MakeCamera(vec2(800 * 2, 600 * 2), "maincamera");
}
void Entity_Player::Update() {
    PlayerCamera->SetPosition(position);
    PlayerCamera->SetAngles(vec3(pitch, yaw, 0.0f));
}

void Entity_PlayerStart::Initialize() {
    if (!(*world)[0]) {
        auto playerenthandler = world->MakeEntity("player", std::nullopt, 0);

        Entity_Player* playerent = reinterpret_cast<Entity_Player*>(playerenthandler->GetEntityPtr());

        playerent->position = position;
        playerent->pitch = std::asin(2 * (rotation.w * rotation.y - rotation.x * rotation.z)) * 180.0f/M_PI;
        playerent->yaw = std::atan2(2 * (rotation.w * rotation.z + rotation.x * rotation.y), rotation.w * rotation.w + rotation.x * rotation.x - rotation.y * rotation.y - rotation.z * rotation.z) * 180.0f/M_PI;

        playerenthandler->InitEntity();
    }

    handler->Remove();
}