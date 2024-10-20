#pragma once
#include "ReadObj.h"
#include "Structs.h"
#include <iostream>
#include <vector>

class Matrix {
public:
    float m[4][4];  // 4x4 matrix represented as a 2D array

    // Constructor for a 4x4 matrix initialized to zero
    Matrix();

    // Static method to create an identity matrix of a given size (typically 4x4 for 3D graphics)
    static Matrix identity(int size);

    // Overloaded multiplication operator to perform matrix multiplication
    Matrix operator*(const Matrix& other) const;

    // Overloaded indexing operator to access matrix elements
    float* operator[](int index);

    // Function to calculate the determinant of the 4x4 matrix
    float determinant() const;

    // Helper function to calculate the determinant of a 3x3 matrix
    float determinant3x3(const std::vector<std::vector<float>>& mat) const;

    // Function to calculate the inverse of the matrix
    Matrix inverse() const;

    // Function to calculate the transpose of the matrix (swap rows and columns)
    Matrix transpose() const;

    // Function to calculate the inverse-transpose of a matrix (often used in normal transformations)
    Matrix invertTranspose() const;
};

extern float depth; // Global variable to store the depth of the scene

// Function to calculate the magnitude (length) of a vertex (3D point or vector)
float magnitude(const Vertex& v);

// Function to convert a Vertex (3D point or vector) into a 4x1 matrix for transformations
Matrix vertexToMatrix(const Vertex& vertex);

// Function to create a viewport matrix for transforming normalized coordinates into screen coordinates
Matrix viewport(int x, int y, int w, int h);

// Function to create a projection matrix with a given coefficient (for perspective projection)
Matrix projectionMatrix(float coeff);

// Function to create a "look at" matrix to define the view from a camera position looking at a target with a specified up vector
Matrix lookAt(Vertex& camera, Vertex& target, Vertex& up);
