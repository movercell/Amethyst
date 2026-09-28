#include <mutex>
#include "engine/Console.h"
#include "engine/StringUtils.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"


// To avoid initialization order fiasco when creating a console command in the engine itself.
// (Yes, those leaks are intentional.(Because at least one object that needs one of these would be created prior to them and make a std::at_exit screw up if they were actual static objects.))
static inline auto& GetCommandMap() {
    static auto& commands = *(new std::map<std::string_view, ConsoleCommand*>);
    return commands;
}
static inline auto& GetConsoleVariablePreservationLambdasVector() {
    static auto& preservationlambdas = *(new std::vector<std::function<std::string()>>);
    return preservationlambdas;
}
static inline auto& GetWorldMap() {
    static auto& worlds = *(new std::map<std::string_view, World*>);
    return worlds;
}


void Engine::Internal::RegisterConsoleCommand(std::string_view Name, ConsoleCommand* Command) {
    if (GetWorldMap().contains(Name)) {
        Engine::Error("Attempted to create a console command of the same name as another!");
    }
    
    GetCommandMap().emplace(Name, Command);
}
void Engine::Internal::RegisterConsoleVariablePreservation(std::function<std::string()> Function) {
    GetConsoleVariablePreservationLambdasVector().emplace_back(Function);
}
std::map<std::string_view, World*>::iterator Engine::Internal::RegisterWorldForConsole(std::string_view Name, World* world) {
    if (GetWorldMap().contains(Name)) {
        Engine::Error("Attempted to create a world of the same name as another!");
    }

    return GetWorldMap().emplace(Name, world).first;
}
void Engine::Internal::UnregisterWorldForConsole(std::map<std::string_view, World*>::iterator Iterator) {
    GetWorldMap().erase(Iterator);
}



#define PUSH_PARAM \
                if (CurrentContent.size() > 0) { \
                    Params.emplace_back(CurrentContent.begin(), CurrentContent.end()); \
                    CurrentContent.clear(); \
                }

static auto CommandParseDo(std::string Do) {
    std::vector<std::vector<std::string>> Commands;
    int i = 0;
    while (i < Do.length()) {
        std::vector<std::string> Params;
        std::inplace_vector<char, 1024> CurrentContent;

        while (i < Do.length()) {
            // Quation mark.
            if (Do[i] == '\"') {
                i++;
                while (i < Do.length() && Do[i] != '\"') {
                    // Escape.(Yes this is just copied.)
                    if (Do[i] == '\\') {
                        i++;
                        // Failsafe when last char.
                        if (i == Do.size())
                            break;
                        CurrentContent.push_back(Engine::CharacterEscapeResult(Do[i]));
                        i++;
                        continue;
                    }

                    CurrentContent.push_back(Do[i]);
                    i++;
                }
                PUSH_PARAM;
                i++;
            }

            // Semicolon.
            if (Do[i] == ';') {
                PUSH_PARAM;
                i++;
                break;
            }
            // Escape.
            if (Do[i] == '\\') {
                i++;
                // Failsafe when last char.
                if (i == Do.size())
                    break;
                CurrentContent.push_back(Engine::CharacterEscapeResult(Do[i]));
                i++;
                continue;
            }
            // Whitespace.
            if (!std::isgraph(Do[i])) {
                PUSH_PARAM;
                i++;
                continue;
            }
            
            // Normal letter.
            CurrentContent.push_back(Do[i]);
            i++;
        }
        PUSH_PARAM;

        Commands.emplace_back(std::move(Params));
    }

    return Commands;
}

void Engine::ExecuteConsoleCommand(World* InWorld, int AsEntityFromSlot, std::string Do) {
    if (InWorld == nullptr) {
        Engine::Warning("Ran command in a world passed in as nullptr!");
        return;
    }
    if (!(*InWorld)[AsEntityFromSlot]) {
        Engine::Warning("Ran command as an invalid entity!");
        return;
    }

    auto Commands = CommandParseDo(Do);
    auto& CommandMap = GetCommandMap();

    for (auto& Params : Commands) {
        auto Command = CommandMap.find(Params[0]);

        if (Command != CommandMap.end()) {
            Command->second->operator()(InWorld, AsEntityFromSlot, Params);
        } else {
            Engine::Print(std::format("Unknown console command: {}", Params[0]));
        }
    }
}

// Default engine console commands.
ConsoleCommand helpCommand("help", []ConsoleCommandLambda {
    if (Do.size() != 2) {
        Engine::Print("Usage: help [command]");
        return;
    }

    auto& CommandMap = GetCommandMap();
    auto Command = CommandMap.find(Do[1]);
    if (Command != CommandMap.end()) {
         Engine::Print(std::string(Command->second->GetHelpString()));
    } else {
        Engine::Print(std::format("{} is not a valid console command!", Do[1]));
    }
}, "Returns the help string of a command.");




// Console UI.


ConsoleVariable<bool> ConsoleWindowOpen("engine_ui_showconsole", false, false, "Shows the console.");

inline constexpr float RunButtonSize = 30.0f;
inline constexpr float InInputSize = 80.0f;
inline constexpr float AsInputSize = 30.0f;

inline constexpr int ConsoleTextBufferSize = 8192;

std::mutex ConsoleTextBufferMutex;

// Fill the two buffers with spaces.
static constinit auto ConsoleTextBuffer = []() constexpr {
    std::array<char, ConsoleTextBufferSize> out;
    for (char& character : out) character = '\n';
    return out;
}();
static constinit auto ConsoleDrawTextBuffer = []() constexpr {
    std::array<char, ConsoleTextBufferSize + 1> out;
    for (char& character : out) character = '\n';
    out[ConsoleTextBufferSize] = '\0'; // This one needs a null terminator.
    return out;
}();

static bool dirty = false;
static int cursor = 0;
static bool shouldscrolltobottom = true;

static inline void PrintSingleCharacter(char Character) {
    if (cursor == ConsoleTextBufferSize) cursor = 0;

    *(ConsoleTextBuffer.begin() + cursor) = Character;
    cursor++;
} 

void Engine::Print(const std::string& text) {
    std::unique_lock<std::mutex> lock(ConsoleTextBufferMutex);

    PrintSingleCharacter('\n');

    if (cursor + text.size() < ConsoleTextBufferSize) {
        std::copy(text.begin(), text.end(), ConsoleTextBuffer.begin() + cursor);
        cursor += text.size();
    } else {
        for (char Character : text) PrintSingleCharacter(Character);
    }

    dirty = true;
}

static inline void UpdateDrawTextBuffer() {
    if (cursor == 0) {
        std::copy(ConsoleTextBuffer.begin(), ConsoleTextBuffer.end(), ConsoleDrawTextBuffer.begin());
        dirty = false;
        return;
    }

    // Before the cursor.
    std::copy(ConsoleTextBuffer.begin(), ConsoleTextBuffer.begin() + cursor, ConsoleDrawTextBuffer.end() - cursor - 1);
    // After the cursor.
    std::copy(ConsoleTextBuffer.begin() + cursor, ConsoleTextBuffer.end(), ConsoleDrawTextBuffer.begin());
    dirty = false;
}

void Engine::Internal::DrawConsole() {
    if (!ConsoleWindowOpen) return;
    
    if (!ImGui::Begin("Amethyst Engine Console", &ConsoleWindowOpen.GetValue())) {
        ImGui::End();
        return;
    }

    {
        std::unique_lock<std::mutex> lock(ConsoleTextBufferMutex);
        if (dirty) UpdateDrawTextBuffer();

        float ReservedHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap | ImGuiInputTextFlags_EnterReturnsTrue;
        ImGui::InputTextMultiline("##AmethystConsoleOutput", &ConsoleDrawTextBuffer.at(0), ConsoleDrawTextBuffer.size(), ImVec2(ImGui::GetContentRegionAvail().x, -ReservedHeight), flags);


        if (shouldscrolltobottom) {
            if (ImGui::BeginChild("##AmethystConsoleOutput")) {
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();

            shouldscrolltobottom = false;
        }
    }

    // Input area.

    ImGui::AlignTextToFramePadding();

    auto& WorldMap = GetWorldMap();
    if (WorldMap.size() == 0) {
        ImGui::Text("No worlds present!");

        ImGui::End();
        return;
    }

    static int InInput = 0;
    ImGui::Text("In:");
    ImGui::SameLine();
    if (InInput >= WorldMap.size()) { // In case the world stops to exist and now we are out of bounds on the map.
        InInput = 0;
    }
    std::string InPreview;
    {
        auto InVal = WorldMap.begin();
        for (int i = 0; i < InInput; i++) ++InVal;
        InPreview = InVal->second->GetName();
    }
    ImGui::SetNextItemWidth(InInputSize);
    if (ImGui::BeginCombo("##AmethystConsoleInputIn", InPreview.c_str())) {
        int i = 0;
        for (const auto& world : WorldMap) {
            const std::string& Name = world.second->GetName();
            if (ImGui::Selectable(Name.c_str(), InInput == i)) {
                InInput = i;
                InPreview = Name;
            }
            i++;
        }

        ImGui::EndCombo();
    }
    ImGui::SameLine();


    static std::string AsInput = "0";
    ImGui::Text("As:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(AsInputSize);
    ImGui::InputText("##AmethystConsoleInputAs", &AsInput, ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();

    bool ShouldDoCommand = false;

    static std::string Do = "";
    ImGui::Text("Do:");
    ImGui::SameLine();
    const float DoWidth = ImGui::GetContentRegionAvail().x - RunButtonSize - ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetNextItemWidth(DoWidth);
    ShouldDoCommand = ImGui::InputText("##AmethystConsoleInputDo", &Do, ImGuiInputTextFlags_EnterReturnsTrue);
    if (ShouldDoCommand) ImGui::SetKeyboardFocusHere(-1);
    ImGui::SameLine();
    if (ShouldDoCommand) ImGui::SetKeyboardFocusHere(-1); // To retain keyboard focus.

    if (ImGui::Button("Go!", ImVec2(RunButtonSize, 0.0f))) ShouldDoCommand = true;

    if (ShouldDoCommand) {
        auto In = WorldMap.begin();
        for (int i = 0; i < InInput; i++) ++In;
        
        int As;
        std::from_chars(AsInput.data(), AsInput.data() + AsInput.size(), As);

        Engine::Print("] " + Do);
        Engine::ExecuteConsoleCommand(In->second, As, Do);
        Do.clear();
        shouldscrolltobottom = true;
    }

    ImGui::End();
}