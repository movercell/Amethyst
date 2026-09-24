#pragma once

#include <functional>
#include "imgui.h"
#include "engine/graphics/Renderer.h"
#include "engine/filesystem/ADF.h"

extern std::function<void(Renderer*, Window*)> mainuifunction;

enum MainMenuType {
    None = 0,
    Main = 1,
    InGame = 2
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
    std::vector<MainMenuButton> Buttons;

    float XOffsetRatio = 0.05f;
    float YOffsetRatio = 0.4f;
    float WidthRatio = 0.35f;
    float ButtonHeightRatio = 0.15f;
    float ButtonStylishBarWidthRatio = 0.05f;
    float ButtonStylishBarHoveredAppearanceSpeed = 5.0f;
    float ButtonSelectedSizeIncrease = 0.2f;
    float ButtonSelectedBackgroundAlpha = 0.2f;
    float ButtonTextOffsetRatio = 0.04f;
    float ButtonFontSizeRatio = 0.6f;
    vec4  ButtonActiveColor = vec4(153, 102, 204, 255) / 255;
    vec4  ButtonNotActiveColor = vec4(53, 2, 104, 255) / 255;
    float ButtonActivatedTextColorAppearanceSpeed = 14.0f;

    std::string MainMenuButtonFontPath;
    std::string MainMenuButtonFontBoldPath;
    
    void Do(MainMenuType CurrentMenuType);
};

void UIInit();