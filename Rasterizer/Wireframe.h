#pragma once

#include "Matrix.h"
#include "readObj.h"  // Include the readObj header to read vertices and faces
#include "TGAImage.h"
#include "World.h"
#include <algorithm>
#include <cstdlib> // for rand() and RAND_MAX
#include <ctime>   // for time()
#include <SDL.h>
#include <vector>

class Wireframe {
public:
    // Constructor to load the model and set up rendering
    Wireframe(const std::string& objFile, SDL_Renderer* renderer, int screenWidth, int screenHeight);

	// Destructor to free memory
	~Wireframe();

	void printNormals();

    // Method to render the wireframe
    void drawLine(int x0, int y0, int x1, int y1);

    void renderWireframe();

    //void renderTriangle(const Vertex& A, const Vertex& B, const Vertex& C, const TexCoord& texA, const TexCoord& texB, const TexCoord& texC, TGAImage& texture);
    void renderTriangle (TGAImage& texture, const Vertex& A, const Vertex& B, const Vertex& C,
        const TexCoord& texA, const TexCoord& texB, const TexCoord& texC,
        float intensityA, float intensityB, float intensityC);

    void renderMesh(const Vertex& lightDirection, TGAImage& texture, Matrix& projectionMatrix, Matrix& viewModelMatrix);

private:
    std::vector<Vertex> vertices;  // Store vertices from the OBJ file
    std::vector<Face> faces;       // Store faces from the OBJ file
	std::vector<TexCoord> texCord; // Store texture coordinates from the OBJ file
	std::vector<Vertex> vertexNormals; // Store vertex normals from the OBJ file
    SDL_Renderer* renderer;        // SDL renderer for drawing lines
    int width, height;             // Screen dimensions
	int* zBuffer;                  // Z-buffer for hidden surface removal
};
