#pragma once

#include <functional>
#include "imgui.h"
#include "engine/graphics/Renderer.h"

constexpr float MainMenuXOffsetRatio = 0.05f;
constexpr float MainMenuYOffsetRatio = 0.5f;
constexpr float MainMenuWidthRatio = 0.35f;
constexpr float MainMenuButtonHeightRatio = 0.15f;
constexpr float MainMenuButtonStylishBarWidthRatio = 0.05f;
constexpr ImColor MainMenuButtonStylishBarHoveredColor = ImColor(153, 102, 204, 255);
constexpr ImColor MainMenuButtonStylishBarNotHoveredColor = ImColor(53, 2, 104, 255);
constexpr float MainMenuButtonTextOffset = 0.02f;

extern std::function<void(Renderer*, Window*)> mainuifunction;