#pragma once

#include "Structs.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

// Class to handle reading of OBJ files
class ObjReader {
public:
    // Method to read vertices from the OBJ file
    static std::vector<Vertex> readVertices(const std::string& filename);

    // Method to read faces from the OBJ file
    static std::vector<Face> readFaces(const std::string& filename);

    // Method to read texture coordinates from the OBJ file
    static std::vector<TexCoord> readTexCoords(const std::string& filename);

    // Method to read vertex normals from the OBJ file
    static std::vector<Vertex> readVertexNormals(const std::string& filename);

private:
    // Helper method to parse a vertex line
    static Vertex parseVertex(const std::string& line);

    // Helper method to parse a face line
    static Face parseFace(const std::string& line);

    // Helper method to parse a texture coordinate line
    static TexCoord parseTexCoord(const std::string& line);

    // Helper method to parse a vertex normal line (vn)
    static Vertex parseVertexNormal(const std::string& line);
};
