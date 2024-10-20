#pragma once

#include "readObj.h"

// Function to read vertices from an OBJ file
std::vector<Vertex> ObjReader::readVertices(const std::string& filename) {
    std::vector<Vertex> vertices;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("v ", 0) == 0) { // Check if the line starts with "v "
            vertices.push_back(parseVertex(line));
        }
    }

    file.close();
    std::cout << "vertices read\n";
    return vertices;
}

// Helper function to parse a vertex line
Vertex ObjReader::parseVertex(const std::string& line) {
    std::istringstream iss(line.substr(2)); // Skip the "v "
    Vertex vertex;

    if (!(iss >> vertex.x >> vertex.y >> vertex.z)) {
        throw std::runtime_error("Error parsing vertex: " + line);
    }

    return vertex;
}

// Function to read faces from an OBJ file
std::vector<Face> ObjReader::readFaces(const std::string& filename) {
    std::vector<Face> faces;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("f ", 0) == 0) { // Check if the line starts with "f "
            faces.push_back(parseFace(line));
        }
    }

    file.close();
    std::cout << "faces read\n";
    return faces;
}

// Helper function to parse a face line
Face ObjReader::parseFace(const std::string& line) {
    std::istringstream iss(line.substr(2)); // Skip the "f "
    Face face;
    std::string vertexData;

    for (int i = 0; i < 3; ++i) { // Expect exactly 3 vertex indices for each face
        if (!(iss >> vertexData)) {
            throw std::runtime_error("Error parsing face: " + line);
        }

        // Split the vertex data to extract vertex and texture coordinate indices
        size_t pos1 = vertexData.find('/');
        if (pos1 != std::string::npos) {
            face.vertexIndex[i] = std::stoi(vertexData.substr(0, pos1)) - 1; // Vertex index (1-based index in OBJ)

            // Check for texture coordinate index
            size_t pos2 = vertexData.find('/', pos1 + 1);
            if (pos2 != std::string::npos) {
                face.texCoordIndex[i] = std::stoi(vertexData.substr(pos1 + 1, pos2 - pos1 - 1)) - 1; // Texture coord index
            }
            else {
                face.texCoordIndex[i] = -1; // No texture coordinate
            }
        }
        else {
            face.vertexIndex[i] = std::stoi(vertexData) - 1; // Only vertex index
            face.texCoordIndex[i] = -1; // No texture coordinate
        }
    }

    return face;
}

// Function to read texture coordinates from an OBJ file
std::vector<TexCoord> ObjReader::readTexCoords(const std::string& filename) {
    std::cout << "reading texture coordinates\n";
    std::vector<TexCoord> texCoords;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("vt ", 0) == 0) { // Check if the line starts with "vt "
            texCoords.push_back(parseTexCoord(line));
        }
    }

    file.close();
    std::cout << "texture coordinates read\n";
    return texCoords;
}

// Helper function to parse a texture coordinate line
TexCoord ObjReader::parseTexCoord(const std::string& line) {
    std::istringstream iss(line.substr(3)); // Skip the "vt "
    TexCoord texCoord;

    if (!(iss >> texCoord.u >> texCoord.v)) {
        throw std::runtime_error("Error parsing texture coordinate: " + line);
    }

    return texCoord;
}

// Function to read vertex normals from an OBJ file
std::vector<Vertex> ObjReader::readVertexNormals(const std::string& filename) {
    std::vector<Vertex> vertexNormals;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return vertexNormals;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Look for lines that start with "vn" for vertex normals
        if (line.substr(0, 2) == "vn") {
            vertexNormals.push_back(parseVertexNormal(line));
        }
    }

    file.close();
    return vertexNormals;
}

// Helper function to parse a vertex normal line
Vertex ObjReader::parseVertexNormal(const std::string& line) {
    Vertex normal;
    std::istringstream iss(line.substr(2)); // Skip the "vn" part

    // Parse the three floats for the normal
    iss >> normal.x >> normal.y >> normal.z;

    return normal;
}