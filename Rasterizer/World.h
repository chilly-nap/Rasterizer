#pragma once

#include "Matrix.h"
#include "Structs.h"
#include "TGAImage.h"

// Global variables

// Light intensity values
extern float upLight;
extern float downLight;
extern float leftLight;

// Camera position
extern float cameraX;
extern float cameraY;
extern float cameraZ;

// Light direction, camera position, target position, and up vector
extern Vertex lightDirection;
extern Vertex Camera;
extern Vertex Target;
extern Vertex Up;

// Matrices for view, projection, and viewport transformations
extern Matrix viewMatrix;
extern Matrix projection;
extern Matrix viewportMatrix;

// Window dimensions
extern int m_width;
extern int m_height;
extern bool drawWireframe;

// Texture, normal map, and specular map images
extern TGAImage texture;
extern TGAImage normalMap;
extern TGAImage specularMap;

// SDL renderer and window
extern SDL_Renderer* renderer;
extern SDL_Window* model_window;
extern SDL_WindowFlags window_flags;