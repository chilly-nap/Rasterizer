#pragma once
#include "Renderer.h"

// Function to render a 3D model using a texture and a shader program
void renderModel(Model& model, shaderProgram& shader)
{
	// Normalize the light direction vector
	normalizeVertex(lightDirection);
    viewMatrix = lookAt(Camera, Target, Up);
    projection = projectionMatrix(-1.0f / magnitude(Camera - Target));
	shader.uniform_M = projection * viewMatrix;
	shader.uniform_MIT = (shader.uniform_M).invertTranspose();
	shader.uniform_Transform = viewportMatrix * shader.uniform_M;

	// Retrieve model data: faces, vertices, vertex normals, texture coordinates, and z-buffer
	std::vector<Face> faces = model.getFaces();
	std::vector<Vertex> vertices = model.getVertices();
	std::vector<Vertex> vertexNormals = model.getVertexNormals();
	std::vector<TexCoord> texCords = model.getTexCoords();
	int* zbuffer = model.getZBuffer();

	Vertex screenCoord[3];

	// Iterate over each face of the model
	for (const Face& face : faces)
	{
		// Process each vertex in the face (assuming triangular faces)
		for (int i = 0; i < 3; ++i)
		{
			// Retrieve vertex, its normal, and texture coordinate
			Vertex vertex = vertices[face.vertexIndex[i]];
			Vertex vertexNormal = vertexNormals[face.vertexIndex[i]];
			TexCoord uv = texCords[face.texCoordIndex[i]];

			// Normalize the vertex normal
			normalizeVertex(vertexNormal);

			// Compute screen coordinates using the vertex shader
			screenCoord[i] = shader.vertexShader(vertex, vertexNormal, uv, i);
		}

		// Render the triangle formed by the three screen coordinates
		renderTriangle(screenCoord, shader, zbuffer);
	}
	
	// Clear the z-buffer for the next frame
	std::fill(zbuffer, zbuffer + m_width * m_height, std::numeric_limits<int>::min());
}

// Function to render a triangle on the screen
void renderTriangle(Vertex screenCoord[3], shaderProgram& shader, int* zbuffer)
{
    // Determine the bounding box of the triangle in screen space
    int minX = std::min({ screenCoord[0].x, screenCoord[1].x, screenCoord[2].x });
    int maxX = std::max({ screenCoord[0].x, screenCoord[1].x, screenCoord[2].x });
    int minY = std::min({ screenCoord[0].y, screenCoord[1].y, screenCoord[2].y });
    int maxY = std::max({ screenCoord[0].y, screenCoord[1].y, screenCoord[2].y });

    // Precompute values for barycentric coordinates
    float denom = (screenCoord[1].y - screenCoord[2].y) * (screenCoord[0].x - screenCoord[2].x) +
        (screenCoord[2].x - screenCoord[1].x) * (screenCoord[0].y - screenCoord[2].y);
    float invDenom = 1.0f / denom;

    // Barycentric coordinate gradients
    float dx_baryX = (screenCoord[1].y - screenCoord[2].y) * invDenom;
    float dy_baryX = (screenCoord[2].x - screenCoord[1].x) * invDenom;
    float dx_baryY = (screenCoord[2].y - screenCoord[0].y) * invDenom;
    float dy_baryY = (screenCoord[0].x - screenCoord[2].x) * invDenom;

    // Precompute the barycentric coordinates at the top-left of the bounding box
    float startX = minX + 0.5f;
    float startY = minY + 0.5f;

    float baryX_start = ((screenCoord[1].y - screenCoord[2].y) * (startX - screenCoord[2].x) +
        (screenCoord[2].x - screenCoord[1].x) * (startY - screenCoord[2].y)) * invDenom;

    float baryY_start = ((screenCoord[2].y - screenCoord[0].y) * (startX - screenCoord[2].x) +
        (screenCoord[0].x - screenCoord[2].x) * (startY - screenCoord[2].y)) * invDenom;

    // Initialize the color to white
    SDL_Color color = { 255, 255, 255, 255 };

    // Precompute z-values for the triangle's vertices
    float z0 = screenCoord[0].z;
    float z1 = screenCoord[1].z;
    float z2 = screenCoord[2].z;

    // Iterate through each pixel in the bounding box
    for (int y = minY; y <= maxY; ++y) {
        float baryX = baryX_start;
        float baryY = baryY_start;

        for (int x = minX; x <= maxX; ++x)
        {
            // Compute the remaining barycentric coordinate (baryZ)
            float baryZ = 1.0f - baryX - baryY;

            // Check if the point is inside the triangle
            if (baryX >= 0 && baryY >= 0 && baryZ >= 0)
            {
                // Compute the interpolated z-value for depth testing
                float z = baryX * z0 + baryY * z1 + baryZ * z2;
                int index = y * m_width + x;

                // Perform depth test to see if the pixel should be drawn
                if (z > zbuffer[index])
                {
                    Vertex bary = { baryX, baryY, baryZ };
                    // Execute the fragment shader to compute the pixel color
                    if (!shader.fragmentShader(bary, color))
                    {
                        // Update the z-buffer and set the pixel color
                        zbuffer[index] = z;
                        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }
            }

            // Increment the barycentric coordinates in x direction
            baryX += dx_baryX;
            baryY += dx_baryY;
        }

        // Increment the starting barycentric coordinates for the next row (y direction)
        baryX_start += dy_baryX;
        baryY_start += dy_baryY;
    }
}

// Function to render a wireframe of a 3D model
void renderWireframe(Model& model)
{	
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	std::vector<Face> faces = model.getFaces();
	std::vector<Vertex> vertices = model.getVertices();
	for (const Face& face : faces) {
		for (size_t i = 0; i < 3; ++i) { // We only need 3 vertices for a triangle
			int v0Index = face.vertexIndex[i];
			int v1Index = face.vertexIndex[(i + 1) % 3];  // Wrap to create closed triangle

			Vertex v0 = vertices[v0Index];
			Vertex v1 = vertices[v1Index];

			// Transform 3D coordinates to 2D screen coordinates
			int x0 = (((v0.x + 1) * m_width) / 2);
			int y0 = (((1 - v0.y) * m_height) / 2);
			int x1 = (((v1.x + 1) * m_width) / 2);
			int y1 = (((1 - v1.y) * m_height) / 2);

			// Draw a line between the two vertices
			renderLine(x0, y0, x1, y1);
		}
	}
}

// Function to render a line on the screen
void renderLine(int x0, int y0, int x1, int y1)
{
	SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
}