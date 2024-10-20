#pragma once

#include "Matrix.h"
#include "Structs.h"
#include "World.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class shaderProgram
{
public:
	Matrix uniform_M;

	Matrix uniform_MIT;

	Matrix uniform_Transform;

	TexCoord uvCoord[3];

	shaderProgram();

	// Transform the vertex from model space to screen space
	virtual Vertex vertexShader(Vertex& vertex, Vertex& vertexNormal, TexCoord& uv, int ith);

	// Fragment shader
	virtual bool fragmentShader(Vertex &bary, SDL_Color& color);

	virtual Vertex transformNormal(Vertex& normal, Matrix& transform);

	virtual Vertex transformDirection(Vertex& direction, Matrix& transform);
};
