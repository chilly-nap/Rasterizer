#pragma once

#include "ShaderProgram.h"

// Constructor: Initializes the shaderProgram object and sets initial intensity and UV coordinates to zero
shaderProgram::shaderProgram()
{
	// Initialize UV coordinates for the three vertices to (0, 0)
	uvCoord[0] = { 0, 0 };
	uvCoord[1] = { 0, 0 };
	uvCoord[2] = { 0, 0 };

	uniform_M = projection * viewMatrix;
	uniform_MIT = (uniform_M).invertTranspose();
	uniform_Transform = viewportMatrix * uniform_M;
}

// Vertex shader function: Computes the screen coordinates and light intensity for a vertex
Vertex shaderProgram::vertexShader(Vertex& vertex, Vertex& vertexNormal, TexCoord& uv, int ith)
{
	// Store the UV coordinates for the current vertex
	uvCoord[ith] = uv;

	// Transform the vertex into screen coordinates using the combined transformation matrix
	Matrix mat1 = uniform_Transform * (vertexToMatrix(vertex));

	// Convert the homogeneous coordinates to screen coordinates (inverting Y)
	Vertex screenCoord = {
		mat1[0][0] / mat1[3][0],      // X coordinate
		m_height - (mat1[1][0] / mat1[3][0]), // Y coordinate (inverted)
		mat1[2][0] / mat1[3][0]       // Z coordinate
	};

	return screenCoord; // Return the computed screen coordinates
}

// Optimized Fragment shader function
bool shaderProgram::fragmentShader(Vertex& bary, SDL_Color& color)
{
    // Interpolate UV coordinates based on barycentric coordinates
    TexCoord uv = interpolateTexCoordinates(uvCoord, bary);

    const float scaleFactor = 0.007843f; // 1/255
    TGAColor colorNormal = normalMap.get(uv.u * normalMap.get_width(), uv.v * normalMap.get_height());

    // Precompute scaled values for color components
    float red = static_cast<float>(colorNormal.r) * scaleFactor;
    float green = static_cast<float>(colorNormal.g) * scaleFactor;
    float blue = static_cast<float>(colorNormal.b) * scaleFactor;

    // Directly compute the normal without repeating calculations
    Vertex normal = {
        red - 1.0f,   // X component (normalized)
        green - 1.0f, // Y component (normalized)
        blue - 1.0f   // Z component (normalized)
    };

    // Transform and normalize the normal vector in world space
    normal = transformNormal(normal, uniform_MIT);
    normalizeVertex(normal);

    // Transform and normalize light direction in world space
    Vertex lightDir = transformDirection(lightDirection, uniform_M);
    normalizeVertex(lightDir);

    // Compute the dot product between the normal and the light direction
    float dotNL = std::max(0.0f, dotProduct(normal, lightDir)); // Merged dotNL and diff

    // Reflection vector calculation
    Vertex reflectedDir = {
        normal.x * 2.0f * dotNL - lightDir.x,
        normal.y * 2.0f * dotNL - lightDir.y,
        normal.z * 2.0f * dotNL - lightDir.z
    };
    normalizeVertex(reflectedDir);

    // Retrieve specular intensity from the specular map
    float specularIntensity = specularMap.get(uv.u * specularMap.get_width(), uv.v * specularMap.get_height()).b;
    float spec = pow(std::max(reflectedDir.z, 0.0f), specularIntensity);

    // Retrieve texture color
    TGAColor colorTex = texture.get(uv.u * texture.get_width(), uv.v * texture.get_height());

    // Compute final color using texture color, diffuse, and specular intensity
    float intensity = dotNL + 0.6f * spec; // Merged diff and spec factor
    color.r = std::min(255.0f, 5.0f + colorTex.r * intensity);
    color.g = std::min(255.0f, 5.0f + colorTex.g * intensity);
    color.b = std::min(255.0f, 5.0f + colorTex.b * intensity);

    return false; // No pixel discard
}

Vertex shaderProgram::transformNormal(Vertex& normal, Matrix& transform)
{
	return {
		transform[0][0] * normal.x + transform[0][1] * normal.y + transform[0][2] * normal.z,
		transform[1][0] * normal.x + transform[1][1] * normal.y + transform[1][2] * normal.z,
		transform[2][0] * normal.x + transform[2][1] * normal.y + transform[2][2] * normal.z
	};
}

Vertex shaderProgram::transformDirection(Vertex& direction, Matrix& transform)
{
	// Direct matrix multiplication without creating an unnecessary copy
	return {
		transform[0][0] * direction.x + transform[0][1] * direction.y + transform[0][2] * direction.z,
		transform[1][0] * direction.x + transform[1][1] * direction.y + transform[1][2] * direction.z,
		transform[2][0] * direction.x + transform[2][1] * direction.y + transform[2][2] * direction.z
	};
}
