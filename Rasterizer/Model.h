#pragma once

#include "Matrix.h"
#include "readObj.h"  // Include the readObj header to read vertices and faces
#include <string>
#include <vector>

class Model {
public:
    // Constructor to load the model and set up rendering
    Model(const std::string& objFile, SDL_Renderer* renderer, int screenWidth, int screenHeight);

    // Destructor to free memory
    ~Model();

    const std::vector<Face>& getFaces() const;

	const std::vector<Vertex>& getVertices() const;

	const std::vector<TexCoord>& getTexCoords() const;

	const std::vector<Vertex>& getVertexNormals() const;

	int* getZBuffer() const;

private:
    std::vector<Vertex> vertices;  // Store vertices from the OBJ file
    std::vector<Face> faces;       // Store faces from the OBJ file
    std::vector<TexCoord> texCord; // Store texture coordinates from the OBJ file
    std::vector<Vertex> vertexNormals; // Store vertex normals from the OBJ file
    SDL_Renderer* renderer;        // SDL renderer for drawing lines
    int width, height;             // Screen dimensions
    int* zBuffer;                  // Z-buffer for hidden surface removal
};
