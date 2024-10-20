#pragma once
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Model.h"
#include "Renderer.h"
#include "Setup.h"
#include "shaderProgram.h"
#include "TGAImage.h"
#include "Wireframe.h"
#include "World.h"
#include <SDL.h>
#include <stdio.h>

// Main code  
int main(int, char**)  
{     
   SDL_Window* ui_window;
   ImGuiIO io;
   SDL_Renderer* windowRenderer;

   SetupWindow();
   SetupExampleModel();
   SetupAll(window_flags, &ui_window, &windowRenderer, io, "Config", 1000, 1000);

   Model head("Model/head.obj", renderer, 1000, 1000);

   renderWindow(ui_window, windowRenderer, io, head);
  
   Cleanup(ui_window, windowRenderer);
 
   return 0;  
}
