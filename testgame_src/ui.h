#pragma once

#include <functional>
#include "imgui.h"
#include "engine/graphics/Renderer.h"
#include "engine/filesystem/ADF.h"

constexpr float MainMenuXOffsetRatio = 0.05f;
constexpr float MainMenuYOffsetRatio = 0.4f;
constexpr float MainMenuWidthRatio = 0.35f;
constexpr float MainMenuButtonHeightRatio = 0.15f;
constexpr float MainMenuButtonStylishBarWidthRatio = 0.05f;
constexpr float MainMenuButtonStylishBarHoveredAppearanceSpeed = 5.0f;
constexpr float MainMenuButtonSelectedSizeIncrease = 0.2f;
constexpr float MainMenuButtonTextOffsetRatio = 0.04f;
constexpr float MainMenuButtonFontSizeRatio = 0.6f;
constexpr ImColor MainMenuButtonActiveColor = ImColor(153, 102, 204, 255);
constexpr ImColor MainMenuButtonNotActiveColor = ImColor(53, 2, 104, 255);
constexpr float MainMenuButtonActivatedTextColorAppearanceSpeed = 10.0f;

extern std::function<void(Renderer*, Window*)> mainuifunction;

enum MainMenuType {
    None = 0,
    Main = 1,
    Pause = 2
};

struct MainMenuButton {
    ADFSerialize std::string text;
    ADFSerialize std::string concommand;
    ADFSerialize int exclusivity = MainMenuType::None;

    bool isHovered = false;
    bool isActive = false;
    float selectedamount = 0.0f;
    float activatedamount = 0.0f;

    void Do(float MainMenuWidth, MainMenuType CurrentMenuType);
};

struct MainMenu_t {
    ADFSerialize std::vector<MainMenuButton> Buttons;
    
    void Do();
};

void UIInit();