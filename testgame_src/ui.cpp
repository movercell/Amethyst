#include "ui.h"
#include "main.h"
#include "engine/Console.h"

MainMenu_t MainMenu;

ImFont* MainMenuButtonFont;
ImFont* MainMenuButtonBoldFont;

ConsoleVariable<float> player_speedVariable = {"player_speed", 100.0f, false, "Player camera speed", 0.0f, 1024.0f};

static MainMenuType GetCurrentMenuType() {
    auto PlayerEntityHandler = (*world)[0];
	if (PlayerEntityHandler) {
        if (PlayerEntityHandler->GetClassname() == "player") return MainMenuType::InGame;
        if (PlayerEntityHandler->GetClassname() == "player_mainmenu") return MainMenuType::Main;
    }
    return MainMenuType::None;
}

ConsoleCommand menu_pauseCommand("menu_pause", []ConsoleCommandLambda {
	if (GetCurrentMenuType() == MainMenuType::InGame) {
		if (CurrentGameState == GameState::Normal) {
			CurrentGameState = GameState::Paused;
		} else if (CurrentGameState == GameState::Paused) {
			CurrentGameState = GameState::Normal;
		}
	}
});
ConsoleCommand menu_quitCommand("menu_quit", []ConsoleCommandLambda {
	Engine::QueueShutdown();
});

void MainMenuButton::Do(float Width, float Height, MainMenuType CurrentMenuType) {
    if (exclusivity != MainMenuType::None && exclusivity != CurrentMenuType)
        return;

	vec4 TextColor = (vec4(1.0f, 1.0f, 1.0f) * (1.0f - activatedamount)) + (MainMenu.ButtonActiveColor * (activatedamount));
	TextColor.w = 1.0f;

	ImGui::PushStyleColor(ImGuiCol_Text, std::bit_cast<ImVec4>(TextColor));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(Width * MainMenu.ButtonStylishBarWidthRatio * selectedamount + Width * MainMenu.ButtonTextOffsetRatio, ImGui::GetStyle().FramePadding.y));
	if (isHovered) ImGui::PushFont(MainMenuButtonBoldFont, Height * MainMenu.ButtonFontSizeRatio);

	Height += Height * MainMenu.ButtonSelectedSizeIncrease * selectedamount;
	bool isPressed = ImGui::Button(text.c_str(), ImVec2(Width, Height));

	if (isHovered) ImGui::PopFont();
	ImGui::PopStyleColor(1);
	ImGui::PopStyleVar(1);

	isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);
	isActive = ImGui::IsItemActive();

	selectedamount = std::clamp(selectedamount + deltaTime * MainMenu.ButtonStylishBarHoveredAppearanceSpeed * (isHovered ? 1 : -1), 0.0f, 1.0f);
	activatedamount = std::clamp(activatedamount + deltaTime * MainMenu.ButtonActivatedTextColorAppearanceSpeed * (isActive ? 1 : -1), 0.0f, 1.0f);

	ImDrawList* DrawList = ImGui::GetWindowDrawList();
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 size = ImGui::GetItemRectSize();
	size.x *= MainMenu.ButtonStylishBarWidthRatio;
	ImVec2 max = ImVec2(min.x + (size.x * selectedamount), min.y + size.y);
	vec4 StylishBarColor = MainMenu.ButtonActiveColor * selectedamount + MainMenu.ButtonNotActiveColor * (1.0 - selectedamount);
	DrawList->AddRectFilled(min, max, std::bit_cast<ImColor>(StylishBarColor));

    if (isPressed) {
		// TODO: Play a sound
        Engine::ExecuteConsoleCommand(world.get(), 0, concommand);
    }
}

void MainMenu_t::Do(MainMenuType CurrentMenuType) {
    // Pause logic.
	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && CurrentMenuType == MainMenuType::InGame) {
		Engine::ExecuteConsoleCommand(world.get(), 0, "menu_pause");
	}

	// Fully in the gameplay, no need to draw the main menu.
    if (CurrentMenuType == MainMenuType::InGame)
        if (CurrentGameState != GameState::Paused)
            return;

    ImGui::SetNextWindowPos(ImVec2(window->GetWidth() * MainMenu.XOffsetRatio, window->GetHeight() * MainMenu.YOffsetRatio));
    float MainMenuWidth = window->GetWidth() * MainMenu.WidthRatio;
	float MainMenuButtonHeight = std::min(MainMenuWidth * MainMenu.ButtonHeightRatio, window->GetHeight() * MainMenu.ButtonHeightCapRatio);

    ImGui::SetNextWindowSize(ImVec2(-1.0f, -1.0f));
    ImGui::Begin("Main menu buttons", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

	ImGui::PushFont(MainMenuButtonFont, MainMenuButtonHeight * MainMenu.ButtonFontSizeRatio); // TODO: use a style?
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, MainMenu.ButtonSelectedBackgroundAlpha));
	ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

    for (auto& button : Buttons) {
            button.Do(MainMenuWidth, MainMenuButtonHeight, CurrentMenuType);
    }

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(4);
	ImGui::PopFont();

    ImGui::End();
	
}

std::function<void(Renderer*, Window*)> mainuifunction = [](Renderer* renderer, Window* window) {

	// Loading screen stuff.
	static float CurrentLoadingPopupAlpha = 0.0f;
	static const char* CurrentLoadingPopupText = "";
	bool IsLoading = CurrentGameState == GameState::Loading;
	if (IsLoading)
		CurrentLoadingPopupAlpha = 1.0f;
	if (CurrentLoadingPopupAlpha > 0.0f) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, CurrentLoadingPopupAlpha);
		// They are separately named so that their sizes are fit to their text.(If ImGuiWindowFlags_AlwaysAutoResize is set then there's still a frame of wrong size.)
		ImGui::Begin(CurrentLoadingPopupText, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		ImGui::TextUnformatted(CurrentLoadingPopupText);
		ImGui::End();
		ImGui::PopStyleVar();
		CurrentLoadingPopupAlpha -= deltaTime * 1.5f;
	}
	if (IsLoading)
		return; // No need to draw the rest of the UI while the game is loading as it'll look very broken.


	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// Main/pause menu
	MainMenuType CurrentMenuType = GetCurrentMenuType();
	MainMenu.Do(CurrentMenuType);
	
	// In-game.
	static bool shouldUseCamera = false;

	if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
		shouldUseCamera = !shouldUseCamera;
	}
	static char previsUsingCamera = 2; // So that it's not equal by default.
	char isUsingCamera = shouldUseCamera && CurrentGameState == GameState::Normal && CurrentMenuType == InGame;
	if (isUsingCamera != previsUsingCamera) {
		previsUsingCamera = isUsingCamera;
		window->SetEatCursor(isUsingCamera);
	}

	// Safety check
	if (CurrentMenuType == MainMenuType::None) return;

	auto PlayerEntityHandler = (*world)[0];
	Entity_Player* PlayerEntity;
	Engine::Reference<Camera> camera;
	if (CurrentMenuType == MainMenuType::InGame) {
		PlayerEntity = reinterpret_cast<Entity_Player*>(PlayerEntityHandler->GetEntityPtr());
		camera = PlayerEntity->PlayerCamera;
	} else if (CurrentMenuType == MainMenuType::Main) {
		camera = reinterpret_cast<Entity_Player_MainMenu*>(PlayerEntityHandler->GetEntityPtr())->PlayerCamera;
	}

	// Draw the camera output.
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("main", NULL, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
                 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_NoBackground);
		ImVec2 CameraSize = ImGui::GetContentRegionAvail();
		ImGui::Image(camera->GetTexture(), CameraSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	ImGui::PopStyleVar(3);

	// Always calculate the mouse offset because the controls logic CAN get skipped.
	static vec2 lastmouse = vec2(0, 0);
	vec2 currmouse = std::bit_cast<vec2>(ImGui::GetMousePos());
	if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
		lastmouse = currmouse;
	vec2 mouseoffset = currmouse - lastmouse;
	lastmouse = currmouse;

	mouseoffset.x *= 7.5f * deltaTime;
	mouseoffset.y *= 7.5f * deltaTime;

	if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
		Engine::QueueShutdown();
	}
	// No camera controls and no rest of the UI in main menu.
	if (CurrentMenuType == MainMenuType::Main) return;

	// Camera controls.
	float velocity = player_speedVariable * deltaTime;
	vec3 direction;
	if (isUsingCamera && window->IsWindowInFocus()) {
    	    if (ImGui::IsKeyDown(ImGuiKey_W))
    	        direction += camera->GetFront();
    	    if (ImGui::IsKeyDown(ImGuiKey_S))
    	        direction -= camera->GetFront();
    	    if (ImGui::IsKeyDown(ImGuiKey_A))
    	        direction += camera->GetLeft();
    	    if (ImGui::IsKeyDown(ImGuiKey_D))
    	        direction -= camera->GetLeft();
    	    if (ImGui::IsKeyDown(ImGuiKey_Space))
    	        direction += vec3(0, 0, 1);
    	    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    	        direction -= vec3(0, 0, 1);
		direction = direction.norm();
		PlayerEntity->position += direction * velocity;

    	PlayerEntity->yaw   -= mouseoffset.x;
    	PlayerEntity->pitch += mouseoffset.y;

        if (PlayerEntity->pitch > 89.0f)
            PlayerEntity->pitch = 89.0f;
        if (PlayerEntity->pitch < -89.0f)
            PlayerEntity->pitch = -89.0f;

		// These are failsafes.(They can get triggered if the player does alt+tab in fullscreen with camera controls active.)
		if (PlayerEntity->yaw > 2000.0f)    PlayerEntity->yaw = 0.0f;
		if (PlayerEntity->yaw < -2000.0f)   PlayerEntity->yaw = 0.0f;

		// To avoid floating point imprecision at longer playtimes
		while (PlayerEntity->yaw > 360.0f)  PlayerEntity->yaw -= 360.0f;
		while (PlayerEntity->yaw < -360.0f) PlayerEntity->yaw += 360.0f;
	}

	ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::Begin("Hello from ui function");
		ImGui::Text("Delta		 : %f", deltaTime);
		ImGui::Text("Player pitch: %f", PlayerEntity->pitch);
		ImGui::Text("Player yaw  : %f", PlayerEntity->yaw);
		ImGui::Text("Player X	 : %f", PlayerEntity->position.x);
		ImGui::Text("Player Y	 : %f", PlayerEntity->position.y);
		ImGui::Text("Player Z	 : %f", PlayerEntity->position.z);
		if (ImGui::Button("Quicksave(F5)") || ImGui::IsKeyPressed(ImGuiKey_F5, false)) {
			CurrentLoadingPopupText = "Saved!";
			CurrentLoadingPopupAlpha = 1.0f;
			world->Save().ToFile("saves/quick.adf", true);
		}
		if (ImGui::Button("Quickload(F6)") || ImGui::IsKeyPressed(ImGuiKey_F6, false)) {
			CurrentLoadingPopupText = "Loading...";
			QueuedLoad = ADFEntry::FromFile("saves/quick.adf");
		}

	ImGui::End();
};


void UIInit() {
    ADFEntry MainMenuLayout = ADFEntry::FromFile("resources/MainMenuLayout.adf")["MainMenuLayout"];
    MainMenuLayout.Deserialize(MainMenu);

	ImFontConfig fontconfig;
	fontconfig.OversampleH = 1;
	fontconfig.OversampleV = 1;
	MainMenuButtonFont = renderer->LoadFont(MainMenu.MainMenuButtonFontPath, &fontconfig);
	MainMenuButtonBoldFont = renderer->LoadFont(MainMenu.MainMenuButtonFontBoldPath, &fontconfig);
}