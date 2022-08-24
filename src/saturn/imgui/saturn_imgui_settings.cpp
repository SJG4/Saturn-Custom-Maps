#include "saturn_imgui_settings.h"

#include <string>
#include <iostream>

#include "saturn/libs/imgui/imgui.h"
#include "saturn/libs/imgui/imgui_internal.h"
#include "saturn/libs/imgui/imgui_impl_sdl.h"
#include "saturn/libs/imgui/imgui_impl_opengl3.h"
#include "saturn/saturn.h"
#include "saturn_imgui.h"
#include <SDL2/SDL.h>

extern "C" {
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "pc/cheats.h"
#include "pc/controller/controller_api.h"
#include "pc/controller/controller_sdl.h"
#include "pc/controller/controller_keyboard.h"
}

using namespace std;

// Variables

int windowWidth, windowHeight;
bool waitingForKeyPress;

const char* translate_bind_to_name(int bind) {
    static char name[11] = { 0 };
    sprintf(name, "%04X", bind);

    if (bind == VK_INVALID) { return ""; }

    // gamepad
    if (bind >= VK_BASE_SDL_GAMEPAD) {
        int gamepad_button = (bind - VK_BASE_SDL_GAMEPAD);
        switch (gamepad_button) {
            case 0: return "[A]";
            case 1: return "[B]";
            case 2: return "[X]";
            case 3: return "[Y]";
            case 4: return "[Back]";
            case 5: return "[Guide]";
            case 6: return "[Start]";
            case 7: return "[L Stick]";
            case 8: return "[R Stick]";
            case 9: return "[L Bump]";
            case 10: return "[R Bump]";
            case 11: return "[DPAD Up]";
            case 12: return "[DPAD Down]";
            case 13: return "[DPAD Left]";
            case 14: return "[DPAD Right]";
            case 0x1A: return "[L Trig]";
            case 0x1B: return "[R Trig]";
            default: return name;
        }
    }

    // keyboard
    if (bind >= 512) { return name; }

    SDL_Scancode sc = bind_to_sdl_scancode[bind];
    if (sc == 0) { return name; }

    const char* sname = SDL_GetScancodeName(sc);
    if (strlen(sname) <= 9) { return sname; }

    char* space = strchr(sname, ' ');
    if (space == NULL) { return sname; }

    snprintf(name, 10, "%c%s", sname[0], (space + 1));
    
    return name;
}

int waitingIndex = -1;

void SaturnKeyBind(const char* title, unsigned int configKey[], const char* id, int buttonIndex) {
    for (int i = 0; i < MAX_BINDS; i++) {
        unsigned int key = configKey[i];

        // A different ID for each button
        string buttonName = (string)translate_bind_to_name(key) + (string)"###btn" + to_string(buttonIndex + i);

        // Only run this code for the button we're currently rebinding
        if(buttonIndex + i == waitingIndex) {
            buttonName = (string)"...###btn" + to_string(buttonIndex + i);

            // Configure key
            u32 rawKey = controller_get_raw_key();
            if (rawKey != VK_INVALID && rawKey != 0x1101 && rawKey != -1) {
                configKey[i] = rawKey;

                // Escape is our "clear"
                if (rawKey == 1) configKey[i] = 0xffff;

                controller_reconfigure();
                configfile_save(configfile_name());

                // Clear data
                rawKey = -1;
                waitingIndex = -1;
                key = -1;
            }

            // Set a tooltip so the user knows they are rebinding
            ImGui::SetTooltip("Press any key or gamepad button. ESC to clear.");
            
            accept_text_input = true;
        }

        // buttonIndex is a unique ordered number for each button
        if (ImGui::Button(buttonName.c_str(), ImVec2(75, 20))) {
            waitingIndex = buttonIndex + i;
            accept_text_input = false;
        }

        ImGui::SameLine();
    }

    ImGui::Text(title);
}

void ssettings_imgui_init() {
    windowWidth = configWindow.w;
    windowHeight = configWindow.h;
}

void ssettings_imgui_update() {
    ImGui::Checkbox("Limit FPS", &limit_fps);
    imgui_bundled_tooltip("(F4) Helpful for speeding up slow in-game events. Works like Project64.");

    ImGui::SetNextItemOpen(true, ImGuiCond_FirstUseEver);
    if (ImGui::CollapsingHeader("Graphics")) {
        if (ImGui::Checkbox("Fullscreen", &configWindow.fullscreen))
            configWindow.settings_changed = true;
        if (!configWindow.fullscreen) {
            ImGui::Text("Resolution");
            static int screen_res[2] = { windowWidth, windowHeight };
            ImGui::InputInt2("###screen_resolution", screen_res); ImGui::SameLine();
            if (ImGui::Button("Set###screen_apply")) {
                windowWidth = screen_res[0];
                windowHeight = screen_res[1];
                if (windowWidth < 640 || windowHeight < 480) {
                    windowWidth = screen_res[0] = 640;
                    windowHeight = screen_res[1] = 480;
                }
                SDL_SetWindowSize(window, windowWidth, windowHeight);
                SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                configWindow.w = windowWidth;
                configWindow.h = windowHeight;
            }
        }

        ImGui::Dummy(ImVec2(0, 5));

        if (ImGui::Checkbox("VSync", &configWindow.vsync))
            configWindow.settings_changed = true;
        imgui_bundled_tooltip("Enable/disable this if your game speed is too fast or slow.");

        ImGui::Text("Texture Filtering");
        const char* texture_filters[] = { "Nearest", "Linear", "Three-point" };
        ImGui::Combo("###texture_filters", (int*)&configFiltering, texture_filters, IM_ARRAYSIZE(texture_filters));

        if (ImGui::Checkbox("Anti-aliasing", &configWindow.enable_antialias))
            configWindow.settings_changed = true;
        imgui_bundled_tooltip("EXPERIMENTAL. Enables/disables anti-aliasing with OpenGL.");

        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Checkbox("Disable near-clipping", &configEditorNearClipping);
        imgui_bundled_tooltip("Enable when some close to the camera starts clipping through. Disable if the level fog goes nuts.");
    }
    if (ImGui::CollapsingHeader("Audio")) {
        ImGui::Text("Volume");
        ImGui::SliderInt("Master", (int*)&configMasterVolume, 0, MAX_VOLUME);
        ImGui::SliderInt("SFX", (int*)&configSfxVolume, 0, MAX_VOLUME);
        ImGui::SliderInt("Music", (int*)&configMusicVolume, 0, MAX_VOLUME);
        ImGui::SliderInt("Fanfares", (int*)&configEnvVolume, 0, MAX_VOLUME);
        ImGui::Checkbox("Enable Voices", &configVoicesEnabled);
    }
    if (ImGui::CollapsingHeader("Controls")) {
        if (ImGui::BeginTabBar("###controls_tabbar", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Game")) {
                ImGui::Text("Movement");
                SaturnKeyBind("Up", configKeyStickUp, "bUp", 3*1);
                SaturnKeyBind("Down", configKeyStickDown, "bDown", 3*2);
                SaturnKeyBind("Left", configKeyStickLeft, "bLeft", 3*3);
                SaturnKeyBind("Right", configKeyStickRight, "bRight", 3*4);
                SaturnKeyBind("A", configKeyA, "bA", 3*5);
                SaturnKeyBind("B", configKeyB, "bB", 3*6);
                SaturnKeyBind("Z", configKeyZ, "bZ", 3*7);
                ImGui::Text("Camera");
                SaturnKeyBind("C Up", configKeyCUp, "bCUp", 3*8);
                SaturnKeyBind("C Down", configKeyCDown, "bCDown", 3*9);
                SaturnKeyBind("C Left", configKeyCLeft, "bCLeft", 3*10);
                SaturnKeyBind("C Right", configKeyCRight, "bCRight", 3*11);
                SaturnKeyBind("L", configKeyL, "bL", 3*12);
                SaturnKeyBind("R", configKeyR, "bR", 3*13);
                ImGui::Text("Other");
                SaturnKeyBind("Start", configKeyStart, "bStart", 0);
                ImGui::SliderInt("Rumble###rumble_strength", (int*)&configRumbleStrength, 0, 50);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Editor")) {
                ImGui::Text("Machinima Camera");
                SaturnKeyBind("Freeze", configKeyFreeze, "bFreeze", 3*14);
                const char* mCameraSettings[] = { "Keyboard", "Keyboard/Gamepad (Old)", "Mouse (Experimental)" };
                ImGui::Combo("Mode###machinima_camera_mode", (int*)&configMCameraMode, mCameraSettings, IM_ARRAYSIZE(mCameraSettings));
                if (configMCameraMode == 2) {
                    ImGui::SameLine(); imgui_bundled_help_marker("Move Camera -> LShift + Mouse Buttons");
                } else if (configMCameraMode == 1) {
                    ImGui::SameLine(); imgui_bundled_help_marker("Pan Camera -> R + C-Buttons\nRaise/Lower Camera -> L + C-Buttons\nRotate Camera -> L + Crouch + C-Buttons");
                } else if (configMCameraMode == 0) {
                    ImGui::SameLine(); imgui_bundled_help_marker("Move Camera -> Y/G/H/J\nRaise/Lower Camera -> T/U\nRotate Camera -> R + Y/G/H/J");
                }
                ImGui::Text("Animation");
                SaturnKeyBind("Play", configKeyPlayAnim, "bPlayA", 3*24);
                SaturnKeyBind("Pause", configKeyPauseAnim, "bPauseA", 3*25);
                ImGui::Text("Other");
                SaturnKeyBind("Hide Menu", configKeyShowMenu, "bShowMenu", 3*27);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    if (ImGui::CollapsingHeader("Editor")) {
#ifdef DISCORDRPC
        ImGui::Checkbox("Discord Activity Status", &configDiscordRPC);
        imgui_bundled_tooltip("Enables/disables Discord Rich Presence. Requires restart.");
        ImGui::Dummy(ImVec2(0, 5));
#endif
        const char* mThemeSettings[] = { "Legacy", "Moon Edition", "Half-Life" };
        ImGui::Text("Theme");
        ImGui::Combo("###theme", (int*)&configEditorTheme, mThemeSettings, IM_ARRAYSIZE(mThemeSettings));
        ImGui::SameLine(); imgui_bundled_help_marker("Changes the UI theme. Requires restart.");
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Checkbox("Auto-apply CC color editor", &configEditorFastApply);
        imgui_bundled_tooltip("If enabled, color codes will automatically apply in the CC editor. May cause lag on low-end machines.");
        //ImGui::Checkbox("Auto-enable CometSPARK support", &configEditorAutoSpark);
        //imgui_bundled_tooltip("If enabled, CometSPARK support will automatically turn on when a SPARK GS code is loaded.");
    }
    if (ImGui::CollapsingHeader("Gameplay")) {
        ImGui::Checkbox("Infinite Health", &Cheats.GodMode);
        ImGui::Checkbox("Moon Jump", &Cheats.MoonJump);
        imgui_bundled_tooltip("Just like '07! Hold L to in the air to moon jump.");
        ImGui::Checkbox("Exit Anywhere", &Cheats.ExitAnywhere);
        imgui_bundled_tooltip("Allows the level to be exited from any state.");
        ImGui::Checkbox("Skip Intro", &configSkipIntro);
    }
}