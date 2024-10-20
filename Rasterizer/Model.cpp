#include "Model.h"

// Constructor: Initializes the Wireframe object and loads data from the OBJ file
Model::Model(const std::string& objFile, SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), width(screenWidth), height(screenHeight) {

    // Load vertices, faces, and texture coordinates from the OBJ file
    vertices = ObjReader::readVertices(objFile);
    faces = ObjReader::readFaces(objFile);
    texCord = ObjReader::readTexCoords(objFile);
    vertexNormals = ObjReader::readVertexNormals(objFile);

    // Print loading information
    std::cout << "Loaded OBJ file: " << objFile << "\n";
    std::cout << "Number of vertices: " << vertices.size() << "\n";
    std::cout << "Number of faces: " << faces.size() << "\n";
    std::cout << "Number of texture coordinates: " << texCord.size() << "\n";
    std::cout << "Number of vertex normals: " << vertexNormals.size() << "\n";

    // Initialize Z-buffer
    zBuffer = new int[width * height];
    std::fill(zBuffer, zBuffer + (width * height), std::numeric_limits<int>::min());
    std::cout << "Z-buffer initialized with size: " << width * height << "\n";
}

// Destructor: Frees allocated memory
Model::~Model() {
    delete[] zBuffer;
    std::cout << "Wireframe object destroyed, z-buffer memory freed.\n";
}

// Return a const reference to avoid copying the vector
const std::vector<Face>& Model::getFaces() const {
    return faces;
}

// Return a const reference to avoid copying the vector
const std::vector<Vertex>& Model::getVertices() const {
    return vertices;
}

// Return a const reference to avoid copying the vector
const std::vector<TexCoord>& Model::getTexCoords() const {
    return texCord;
}

// Return a const reference to avoid copying the vector
const std::vector<Vertex>& Model::getVertexNormals() const {
    return vertexNormals;
}

// Return a pointer to the zBuffer
int* Model::getZBuffer() const {
    return zBuffer;
}
