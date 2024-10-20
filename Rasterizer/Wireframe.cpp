#pragma once

#include "Wireframe.h"

// Constructor: Initializes the Wireframe object and loads data from the OBJ file
Wireframe::Wireframe(const std::string& objFile, SDL_Renderer* renderer, int screenWidth, int screenHeight)
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
Wireframe::~Wireframe() {
    delete[] zBuffer;
    std::cout << "Wireframe object destroyed, z-buffer memory freed.\n";
}

// Print the normals of the vertices
void Wireframe::printNormals() {
	for (size_t i = 0; i < vertexNormals.size(); ++i) {
		std::cout << "Normal " << i << ": (" << vertexNormals[i].x << ", " << vertexNormals[i].y << ", " << vertexNormals[i].z << ")\n";
	}
}

// Render a triangle with texture and shading
void Wireframe::renderTriangle(TGAImage& texture, const Vertex& A, const Vertex& B, const Vertex& C,
    const TexCoord& texA, const TexCoord& texB, const TexCoord& texC,
    float intensityA, float intensityB, float intensityC) {
    // Find the bounding box of the triangle
    int minX = static_cast<int>(std::min({ A.x, B.x, C.x }));
    int maxX = static_cast<int>(std::max({ A.x, B.x, C.x }));
    int minY = static_cast<int>(std::min({ A.y, B.y, C.y }));
    int maxY = static_cast<int>(std::max({ A.y, B.y, C.y }));

    // Iterate through each pixel in the bounding box
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            Vertex P = { static_cast<float>(x), static_cast<float>(y), 0 };

            // Compute the barycentric coordinates of the pixel relative to the triangle
            BarycentricCoords bary = computeBarycentricCoords(A, B, C, P);
            TexCoord texP = interpolateTexCoordBarycentric(A, B, C, texA, texB, texC, P);

            int texX = static_cast<int>(texP.u * texture.get_width());
            int texY = static_cast<int>(texP.v * texture.get_height());

            intensityA = std::clamp(intensityA, 0.0f, 1.0f);
            intensityB = std::clamp(intensityB, 0.0f, 1.0f);
            intensityC = std::clamp(intensityC, 0.0f, 1.0f);

			float intensity = intensityA * bary.lambda1 + intensityB * bary.lambda2 + intensityC * bary.lambda3;

            TGAColor color = texture.get(texX, texY);
            SDL_Color sdlColor = convertTGAColorToSDLColor(color);
            sdlColor.r *= intensity;
            sdlColor.g *= intensity;
            sdlColor.b *= intensity;
            sdlColor.a = 255;

            // Check if the pixel is inside the triangle
            if (bary.lambda1 >= 0 && bary.lambda2 >= 0 && bary.lambda3 >= 0) {
                float z = bary.lambda1 * A.z + bary.lambda2 * B.z + bary.lambda3 * C.z;
                int index = y * width + x;

                // Perform depth test and render the pixel
                if (z > zBuffer[index]) {
                    zBuffer[index] = z;
                    SDL_SetRenderDrawColor(renderer, sdlColor.r, sdlColor.g, sdlColor.b, sdlColor.a);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }
}

// Render the entire mesh using the light direction and texture
void Wireframe::renderMesh(const Vertex& lightDirection, TGAImage& texture, Matrix& projection, Matrix& viewModelMatrix) {

    for (const Face& face : faces) {
        // Get vertex and texture coordinate indices
        int v0Index = face.vertexIndex[0];
        int v1Index = face.vertexIndex[1];
        int v2Index = face.vertexIndex[2];

        Vertex v0 = vertices[v0Index];
        Vertex v1 = vertices[v1Index];
        Vertex v2 = vertices[v2Index];

        int t0Index = face.texCoordIndex[0];
        int t1Index = face.texCoordIndex[1];
        int t2Index = face.texCoordIndex[2];

        TexCoord texA = texCord[t0Index];
        TexCoord texB = texCord[t1Index];
        TexCoord texC = texCord[t2Index];

		// Calculate the normal of the face
		Vertex norm1 = vertexNormals[v0Index];
		Vertex norm2 = vertexNormals[v1Index];
		Vertex norm3 = vertexNormals[v2Index];

        normalizeVertex(norm1);
        normalizeVertex(norm2);
        normalizeVertex(norm3);

        Matrix mat1 = viewModelMatrix.multiplyMatrix(vertexToMatrix(v0));
        Matrix mat2 = viewModelMatrix.multiplyMatrix(vertexToMatrix(v1));
        Matrix mat3 = viewModelMatrix.multiplyMatrix(vertexToMatrix(v2));

        mat1 = projection.multiplyMatrix(mat1);
        mat2 = projection.multiplyMatrix(mat2);
        mat3 = projection.multiplyMatrix(mat3);

        // Transform vertices
        mat1 = viewportMatrix.multiplyMatrix(mat1);
        mat2 = viewportMatrix.multiplyMatrix(mat2);
        mat3 = viewportMatrix.multiplyMatrix(mat3);

        // Convert to screen coordinates
        Vertex v0_screen = { mat1[0][0] / mat1[3][0], height - (mat1[1][0] / mat1[3][0]), mat1[2][0] / mat1[3][0] }; // Invert Y
        Vertex v1_screen = { mat2[0][0] / mat2[3][0], height - (mat2[1][0] / mat2[3][0]), mat2[2][0] / mat2[3][0] }; // Invert Y
        Vertex v2_screen = { mat3[0][0] / mat3[3][0], height - (mat3[1][0] / mat3[3][0]), mat3[2][0] / mat3[3][0] }; // Invert Y

        // Calculate intensity of illumination
        float intensityA = norm1.x * lightDirection.x + norm1.y * lightDirection.y + norm1.z * lightDirection.z;
		float intensityB = norm2.x * lightDirection.x + norm2.y * lightDirection.y + norm2.z * lightDirection.z;
		float intensityC = norm3.x * lightDirection.x + norm3.y * lightDirection.y + norm3.z * lightDirection.z;

        // Render the triangle if it is visible
        renderTriangle(texture, v0_screen, v1_screen, v2_screen, texA, texB, texC, intensityA, intensityB, intensityC);
    }
}

// Render the wireframe representation of the mesh
void Wireframe::renderWireframe() {
    for (const Face& face : faces) {
        for (size_t i = 0; i < 3; ++i) { // We only need 3 vertices for a triangle
            int v0Index = face.vertexIndex[i];
            int v1Index = face.vertexIndex[(i + 1) % 3];  // Wrap to create closed triangle

            Vertex v0 = vertices[v0Index];
            Vertex v1 = vertices[v1Index];

            // Transform 3D coordinates to 2D screen coordinates
            int x0 = static_cast<int>(((v0.x + 1) * width) / 2);
            int y0 = static_cast<int>(((1 - v0.y) * height) / 2);
            int x1 = static_cast<int>(((v1.x + 1) * width) / 2);
            int y1 = static_cast<int>(((1 - v1.y) * height) / 2);

            // Draw a line between the two vertices
            drawLine(x0, y0, x1, y1);
        }
    }
}

// Draw a line using SDL
void Wireframe::drawLine(int x0, int y0, int x1, int y1) {
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
}
