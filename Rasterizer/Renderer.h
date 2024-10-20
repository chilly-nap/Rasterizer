#pragma once

#include "Model.h"
#include "ShaderProgram.h"
#include "Structs.h"
#include <SDL.h>

// Render a model using the given shader
void renderModel(Model& model, shaderProgram& shader);

// Render a triangle using the given shader
void renderTriangle(Vertex screenCoord[3], shaderProgram& shader, int * zbuffer);

// Render a wireframe of a model
void renderWireframe(Model& model);

// Render a wireframe of a model with a specific color
void renderLine(int x0, int y0, int x1, int y1);