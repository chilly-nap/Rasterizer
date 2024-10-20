#pragma once
#include "Setup.h"

// Function to setup SDL and initialize video
bool SetupWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
    printf("SDL initialized successfully.\n");
    return true;
}

bool SetupExampleModel()
{
	texture.read_tga_file("Model/head.tga");
	texture.flip_vertically();

	normalMap.read_tga_file("Model/head_nm.tga");
	normalMap.flip_vertically();

	specularMap.read_tga_file("Model/head_spec.tga");
	specularMap.flip_vertically();

	return true;
}

// Function to setup the SDL window and renderer
bool SetupAll(SDL_WindowFlags& window_flags, SDL_Window** ui_window, SDL_Renderer** windowRenderer, ImGuiIO& io, const char* title, int width, int height)
{
    window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    // Create SDL window
    *ui_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, window_flags);
	model_window = SDL_CreateWindow("Model", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);
    if (*ui_window == nullptr)  // Fixed the condition to check the dereferenced pointer
    {
        printf("Error: SDL_CreateWindow() failed: %s\n", SDL_GetError());
        return false;
    }
    printf("Window created successfully: %s\n", title);

	if (model_window == nullptr)  // Fixed the condition to check the dereferenced pointer
	{
		printf("Error: SDL_CreateWindow() failed: %s\n", SDL_GetError());
		SDL_DestroyWindow(*ui_window);  // Cleanup window if model window creation fails
		return false;
	}
	printf("Model window created successfully.\n");

    // Create SDL renderer
    *windowRenderer = SDL_CreateRenderer(*ui_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (*windowRenderer == nullptr)  // Fixed the condition to check the dereferenced pointer
    {
        printf("Error: SDL_CreateRenderer() failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(*ui_window);  // Cleanup window if renderer creation fails
        return false;
    }
    printf("Renderer created successfully.\n");

	renderer = SDL_CreateRenderer(model_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)  // Fixed the condition to check the dereferenced pointer
	{
		printf("Error: SDL_CreateRenderer() failed: %s\n", SDL_GetError());
		SDL_DestroyWindow(model_window);  // Cleanup window if renderer creation fails
		return false;
	}

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplSDL2_InitForSDLRenderer(*ui_window, *windowRenderer))
    {
        printf("Error: ImGui_ImplSDL2_InitForSDLRenderer() failed\n");
        return false;
    }

    if (!ImGui_ImplSDLRenderer2_Init(*windowRenderer))
    {
        printf("Error: ImGui_ImplSDLRenderer2_Init() failed\n");
        return false;
    }

    printf("ImGui initialized successfully.\n");
    return true;
}

// Function to render the model window
void renderWindow(SDL_Window* ui_window, SDL_Renderer* windowRenderer, ImGuiIO& io, Model& model)
{
    bool show_demo_window = true;
    bool done = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    shaderProgram shader;

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(model_window))
                done = true;
        }

        // Skip rendering if the window is minimized
        if (SDL_GetWindowFlags(ui_window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start a new ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        {
			ImGui::Begin("Config:");
            ImGui::Text("Camera Config: %.3f %.3f %.3f", Camera.x, Camera.y, Camera.z);

            ImGui::SliderFloat("Camera X", &cameraX, -10.0f, 10.0f);
            ImGui::SliderFloat("Camera Y", &cameraY, -10.0f, 10.0f);
            ImGui::SliderFloat("Camera Z", &cameraZ, 0.5f, 10.0f);

            ImGui::Text("Background Config:");
            ImGui::ColorEdit3("clear color", (float*)&clear_color);

            ImGui::Text("Light Config: %.3f %.3f %.3f", lightDirection.x, lightDirection.y, lightDirection.z);
            ImGui::SliderFloat("light up", &upLight, -10.0f, 10.0f);
            ImGui::SliderFloat("light down", &downLight, -10.0f, 10.0f);
            ImGui::SliderFloat("light left", &leftLight, -10.0f, 10.0f);

			ImGui::Text("Wireframe Config:");
			ImGui::Checkbox("Wireframe", &drawWireframe);
            
            ImGui::End();
        }

		lightDirection = { upLight, downLight, leftLight };
		Camera = { cameraX, cameraY, cameraZ };

        ImGui::Render();

        SDL_RenderSetScale(windowRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(windowRenderer, (clear_color.x * 255), (clear_color.y * 255), (clear_color.z * 255), (clear_color.w * 255));
        SDL_RenderClear(windowRenderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), windowRenderer);
        SDL_RenderPresent(windowRenderer);

        SDL_SetRenderDrawColor(renderer, (clear_color.x * 255), (clear_color.y * 255), (clear_color.z * 255), (clear_color.w * 255));
        SDL_RenderClear(renderer);
        if (!drawWireframe)
        {
            renderModel(model, shader);
            SDL_RenderPresent(renderer);
        }
        else
        {
			renderWireframe(model);
			SDL_RenderPresent(renderer);
        }
        
    }
}

// Cleanup function to free resources
void Cleanup(SDL_Window* ui_window, SDL_Renderer* windowRenderer)
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        printf("Renderer destroyed.\n");
    }
    if (model_window)
    {
        SDL_DestroyWindow(model_window);
        printf("Window destroyed.\n");
    }
    if (windowRenderer)
	{
		SDL_DestroyRenderer(windowRenderer);
		printf("Window renderer destroyed.\n");
	}
	if (ui_window)
	{
		SDL_DestroyWindow(ui_window);
		printf("UI window destroyed.\n");
	}

    SDL_Quit();
    printf("SDL cleaned up successfully.\n");
}