#include "ui.h"
#include "main.h"

MainMenu_t MainMenu;

ImFont* MainMenuButtonFont;

void MainMenuButton::Do(float MainMenuWidth, MainMenuType CurrentMenuType) {
    if (exclusivity != MainMenuType::None && exclusivity != CurrentMenuType)
        return;

	vec4 TextColor = (vec4(1.0f, 1.0f, 1.0f) * (1.0f - activatedamount)) + (std::bit_cast<vec4>(MainMenuButtonActiveColor) * (activatedamount));
	TextColor.w = 1.0f;

	ImGui::PushStyleColor(ImGuiCol_Text, std::bit_cast<ImVec4>(TextColor));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(MainMenuWidth * MainMenuButtonStylishBarWidthRatio * selectedamount + MainMenuWidth * MainMenuButtonTextOffsetRatio, ImGui::GetStyle().FramePadding.y));
	float buttonwidth = MainMenuWidth;
	float buttonheight = buttonwidth * MainMenuButtonHeightRatio;
	buttonheight += buttonheight * MainMenuButtonSelectedSizeIncrease * selectedamount;
	bool isPressed = ImGui::Button(text.c_str(), ImVec2(buttonwidth, buttonheight));
	ImGui::PopStyleColor(1);
	ImGui::PopStyleVar(1);

	bool isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);
	bool isActive = ImGui::IsItemActive();

	selectedamount = std::clamp(selectedamount + deltaTime * MainMenuButtonStylishBarHoveredAppearanceSpeed * (isHovered ? 1 : -1), 0.0f, 1.0f);
	activatedamount = std::clamp(activatedamount + deltaTime * MainMenuButtonActivatedTextColorAppearanceSpeed * (isActive ? 1 : -1), 0.0f, 1.0f);

	ImDrawList* DrawList = ImGui::GetWindowDrawList();
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 size = ImGui::GetItemRectSize();
	size.x *= MainMenuButtonStylishBarWidthRatio;
	ImVec2 max = ImVec2(min.x + (size.x * selectedamount), min.y + size.y);
	vec4 FancyBarColor = std::bit_cast<vec4>(MainMenuButtonActiveColor) * selectedamount + std::bit_cast<vec4>(MainMenuButtonNotActiveColor) * (1.0 - selectedamount);
	DrawList->AddRectFilled(min, max, std::bit_cast<ImColor>(FancyBarColor));

    if (isPressed) {
		Engine::Print(text);
		// TODO: Play a sound
        // TODO: Do the console command.
    }
}

void MainMenu_t::Do() {
    MainMenuType CurrentMenuType;
    auto PlayerEntityHandler = (*world)[0];
	if (PlayerEntityHandler) {
        if (PlayerEntityHandler->GetClassname() == "player") CurrentMenuType = MainMenuType::Pause;
        if (PlayerEntityHandler->GetClassname() == "player_mainmenu") CurrentMenuType = MainMenuType::Main;
    } else {
        CurrentMenuType = MainMenuType::None;
    }

    if (CurrentMenuType == MainMenuType::Pause)
        if (CurrentGameState != GameState::Paused)
            return;

    ImGui::SetNextWindowPos(ImVec2(window->GetWidth() * MainMenuXOffsetRatio, window->GetHeight() * MainMenuYOffsetRatio));
    float MainMenuWidth = window->GetWidth() * MainMenuWidthRatio;
    ImGui::SetNextWindowSize(ImVec2(-1.0f, -1.0f));
    ImGui::Begin("Main menu buttons", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

	ImGui::PushFont(MainMenuButtonFont);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
	ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

    for (auto& button : Buttons) {
            button.Do(MainMenuWidth, CurrentMenuType);
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


	// Pause logic.
	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
		if (CurrentGameState == GameState::Normal) {
			CurrentGameState = GameState::Paused;
		} else if (CurrentGameState == GameState::Paused) {
			CurrentGameState = GameState::Normal;
		}
	}

	// Main/pause menu
	MainMenu.Do();
	
	// In-game.
	static bool shouldUseCamera = false;

	if (ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
		shouldUseCamera = !shouldUseCamera;
	}
	static char previsUsingCamera = 2; // So that it's not equal by default.
	char isUsingCamera = shouldUseCamera && CurrentGameState == GameState::Normal;
	if (isUsingCamera != previsUsingCamera) {
		previsUsingCamera = isUsingCamera;
		window->SetEatCursor(isUsingCamera);
	}

	auto PlayerEntityHandler = (*world)[0];
	if (!PlayerEntityHandler || !(PlayerEntityHandler->GetClassname() == "player" || PlayerEntityHandler->GetClassname() == "player_mainmenu")) return;
	Entity_Player* PlayerEntity = reinterpret_cast<Entity_Player*>(PlayerEntityHandler->GetEntityPtr());

	Engine::Reference<Camera> camera = PlayerEntity->PlayerCamera;

	// Always calculate the mouse offset because the controls logic CAN get skipped.
	static vec2 lastmouse = vec2(0, 0);
	vec2 currmouse = std::bit_cast<vec2>(ImGui::GetMousePos());
	if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
		lastmouse = currmouse;
	vec2 mouseoffset = currmouse - lastmouse;
	lastmouse = currmouse;

	mouseoffset.x *= 7.5f * deltaTime;
	mouseoffset.y *= 7.5f * deltaTime;

	// Camera controls.
	float velocity = 100.0f * deltaTime;
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
	}

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

	//Draws the camera output
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

	if (ImGui::IsKeyPressed(ImGuiKey_F4)) {
		Engine::QueueShutdown();
	}
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

	MainMenuButtonFont = renderer->LoadFont("resources/fonts/Arimo.ttf", window->GetWidth() * MainMenuWidthRatio * MainMenuButtonHeightRatio * MainMenuButtonFontSizeRatio);
    MainMenuLayout.Deserialize(MainMenu);
}