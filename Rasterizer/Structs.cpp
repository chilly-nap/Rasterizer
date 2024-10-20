#include "Structs.h"

Vertex computeBarycentricCoord(const Vertex& A, const Vertex& B, const Vertex& C, const Vertex& P) {
    float denom = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);

    // If denom is zero, the triangle is degenerate
    if (denom == 0.0f) {
        return { -1.0f, -1.0f, -1.0f };  // Return invalid coordinates for degenerate triangle
    }

    // Calculate lambda1 and lambda2 using the same denominator
    float lambda1 = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denom;
    float lambda2 = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denom;

    // Calculate lambda3 (since lambda1 + lambda2 + lambda3 = 1)
    return { lambda1, lambda2, 1.0f - lambda1 - lambda2 };
}

TexCoord interpolateTexCoordinates(const TexCoord uvCoord[3], const Vertex& bary) {
    // Directly calculate and return the interpolated texture coordinates
    return {
        bary.x * uvCoord[0].u + bary.y * uvCoord[1].u + bary.z * uvCoord[2].u,
        bary.x * uvCoord[0].v + bary.y * uvCoord[1].v + bary.z * uvCoord[2].v
    };
}

Vertex crossProduct(const Vertex& v1, const Vertex& v2) {
    Vertex result;

    // Calculate the cross product
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;

    return result;
}

float dotProduct(const Vertex& v1, const Vertex& v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

void normalizeVertex(Vertex& v) {
    // Calculate the length of the vector
    float length = v.x * v.x + v.y * v.y + v.z * v.z;

    // Normalize the vector
    if (length > 0) {
		length = sqrt(length);
        v.x /= length;
        v.y /= length;
        v.z /= length;
    }
}

SDL_Color convertTGAColorToSDLColor(const TGAColor& tgaColor) {
    SDL_Color sdlColor;

    // Assign the color components from TGAColor to SDL_Color
    sdlColor.r = tgaColor.r;
    sdlColor.g = tgaColor.g;
    sdlColor.b = tgaColor.b;
    sdlColor.a = tgaColor.a; // Use alpha component from TGAColor

    return sdlColor;
}