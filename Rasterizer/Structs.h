#pragma once

#include "TGAImage.h"
#include <SDL.h>

// Vertex structure
struct Vertex {
    float x; // X coordinate
    float y; // Y coordinate
    float z; // Z coordinate

    Vertex operator-(const Vertex& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }
};

// Texture coordinate structure
struct TexCoord {
    float u; // U coordinate
    float v; // V coordinate
};

// Structure representing a triangular face
struct Face {
    int vertexIndex[3];     // Indices of the vertices that make up the face
    int texCoordIndex[3];   // Indices of the texture coordinates
};

// Function declarations
Vertex computeBarycentricCoord(const Vertex& A, const Vertex& B, const Vertex& C, const Vertex& P);

TexCoord interpolateTexCoordinates(const TexCoord uvCoord[3], const Vertex& bary);

// Cross product of two vectors
Vertex crossProduct(const Vertex& v1, const Vertex& v2);

float dotProduct(const Vertex& v1, const Vertex& v2);

// Normalize the given vertex
void normalizeVertex(Vertex& v);

// Convert TGA color to SDL_Color
SDL_Color convertTGAColorToSDLColor(const TGAColor& tgaColor);