#include "World.h"

int m_width = 800;
int m_height = 800;
bool drawWireframe = false;

float upLight = 1.0f;
float downLight = 1.0f;
float leftLight = 1.0f;

float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 1.0f;

Vertex lightDirection = { upLight, downLight, leftLight };
Vertex Camera = { cameraX, cameraY, cameraZ };
Vertex Target = { 0.0f, 0.0f, 0.0f };
Vertex Up = { 0.0f, 1.0f, 0.0f };

Matrix viewMatrix = lookAt(Camera, Target, Up);

Matrix projection = projectionMatrix(-1.0f / magnitude(Camera - Target));

Matrix viewportMatrix = viewport(m_width / 8, m_height / 8, m_width * 3 / 4, m_height * 3 / 4);

TGAImage texture;
TGAImage normalMap;
TGAImage specularMap;

SDL_Renderer* renderer;
SDL_Window* model_window;
SDL_WindowFlags window_flags;