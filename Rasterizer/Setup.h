#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Model.h"
#include "Renderer.h"
#include "Wireframe.h"
#include "World.h"
#include <SDL.h>
#include <stdio.h>

// Setup SDL and create window
bool SetupWindow();

// Draw an exmaple model
bool SetupExampleModel();

// Setup All SDL and ImGui
bool SetupAll(SDL_WindowFlags& window_flags, SDL_Window** ui_window, SDL_Renderer** windowRenderer, ImGuiIO& io,const char* title, int width, int height);

// Render Window
void renderWindow(SDL_Window* ui_window, SDL_Renderer* windowRenderer, ImGuiIO& io, Model& model);

// Cleanup
void Cleanup(SDL_Window* ui_window, SDL_Renderer* windowRenderer);