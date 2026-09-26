#include "BaseEngineUI.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "engine/Console.h"

ConsoleVariable<bool> DemoWindowOpen = {"ui_showdemo", false, false, "Shows the Dear ImGUI demo window."};
ConsoleVariable<bool> ConsoleWindowOpen = {"ui_showconsole", false, false, "Shows the console."};

void Engine::DrawEngineUI() {
    DrawConsole();

    if (DemoWindowOpen) {
        ImGui::ShowDemoWindow(&DemoWindowOpen.GetValue());
    }
}


// From Console.cpp
std::map<std::string_view, World*>& GetWorldMap();

inline constexpr float RunButtonSize = 30.0f;

void Engine::DrawConsole() {
    auto& WorldMap = GetWorldMap();
    if (!ConsoleWindowOpen || WorldMap.size() == 0) return;
    
    if (!ImGui::Begin("Amethyst engine console", &ConsoleWindowOpen.GetValue())) {
        ImGui::End();
        return;
    }

    // Text area.
    float ReservedSpace = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("TextArea", ImVec2(0, -ReservedSpace), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_AlwaysVerticalScrollbar)) {


    }
    ImGui::EndChild();


    // Input area.


    ImGui::AlignTextToFramePadding();

    ImGui::Text("In:");
    ImGui::SameLine();
    static int In = std::numeric_limits<int>::max();
    if (In >= WorldMap.size()) { // In case the world stops to exist and now we are out of bounds on the map.
        In = 0;
    }
    std::string InPreview;
    {
        auto InVal = WorldMap.begin();
        for (int i = 0; i < In; i++) ++InVal;
        InPreview = InVal->second->GetName();
    }
    ImGui::SetNextItemWidth(60.0f);
    if (ImGui::BeginCombo("##AmethystConsoleInputIn", InPreview.c_str())) {
        int i = 0;
        for (const auto& world : WorldMap) {
            const std::string& Name = world.second->GetName();
            if (ImGui::Selectable(Name.c_str(), In == i)) {
                In = i;
                InPreview = Name;
            }
            i++;
        }

        ImGui::EndCombo();
    }
    ImGui::SameLine();


    static std::string As = "0";
    ImGui::Text("As:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(20.0f);
    ImGui::InputText("##AmethystConsoleInputAs", &As, ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();

    bool ShouldDoCommand = false;

    static std::string Do = "";
    ImGui::Text("Do:");
    ImGui::SameLine();
    const float DoWidth = ImGui::GetContentRegionAvail().x - RunButtonSize - ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetNextItemWidth(DoWidth);
    ShouldDoCommand = ImGui::InputText("##AmethystConsoleInputDo", &Do, ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ShouldDoCommand) ImGui::SetKeyboardFocusHere(-1); // To retain keyboard focus.

    if (ImGui::Button("Go!", ImVec2(RunButtonSize, 0.0f))) ShouldDoCommand = true;

    if (ShouldDoCommand) {
        auto InVal = WorldMap.begin();
        for (int i = 0; i < In; i++) ++InVal;
        
        int AsVal;
        std::from_chars(As.data(), As.data() + As.size(), AsVal);

        Engine::ExecuteConsoleCommand(InVal->second, AsVal, Do);
        Do.clear();
    }

    ImGui::End();
}