#pragma once
#include "Matrix.h"

float depth = 255.0f; // Global variable to store the depth of the scene

// Constructor for a 4x4 matrix initialized to zero
Matrix::Matrix()
{
    std::memset(m, 0, sizeof(m));
}

// Static method to create an identity matrix
Matrix Matrix::identity(int size) {
    Matrix mat;
    for (int i = 0; i < size; ++i) {
        mat.m[i][i] = 1;
    }
    return mat;
}

// Overload operator* for matrix multiplication
Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result; // Resultant matrix

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }

    return result; // Return the resultant matrix
}

float* Matrix::operator[](int index) {
    return m[index];
}

// Helper function to calculate the determinant of a 3x3 matrix
float Matrix::determinant3x3(const std::vector<std::vector<float>>& mat) const {
    return mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]) -
        mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]) +
        mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
}

// Function to calculate the determinant of a 4x4 matrix
float Matrix::determinant() const {
    float det = 0.0f;

    // We will use cofactor expansion along the first row (index 0)
    for (int col = 0; col < 4; ++col) {
        // Create a 3x3 submatrix by excluding the first row and the current column
        std::vector<std::vector<float>> submatrix(3, std::vector<float>(3));
        for (int i = 1; i < 4; ++i) {
            int sub_col = 0;
            for (int j = 0; j < 4; ++j) {
                if (j == col) continue; // Skip the current column
                submatrix[i - 1][sub_col] = m[i][j];
                sub_col++;
            }
        }

        // Calculate the cofactor for the current element
        float cofactor = ((col % 2 == 0) ? 1 : -1) * m[0][col] * determinant3x3(submatrix);

        // Add to the determinant
        det += cofactor;
    }

    return det;
}

// Function to calculate the inverse of a 4x4 matrix
Matrix Matrix::inverse() const {
    Matrix result; // This will store the inverse matrix
    float det = determinant();
    if (det == 0) {
        throw std::runtime_error("Matrix is singular, cannot invert.");
    }

    // Compute the cofactor matrix (adjugate)
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            // Create a 3x3 submatrix by excluding the current row and column
            std::vector<std::vector<float>> submatrix(3, std::vector<float>(3));
            int sub_row = 0;
            for (int i = 0; i < 4; ++i) {
                if (i == row) continue; // Skip the current row
                int sub_col = 0;
                for (int j = 0; j < 4; ++j) {
                    if (j == col) continue; // Skip the current column
                    submatrix[sub_row][sub_col] = m[i][j];
                    sub_col++;
                }
                sub_row++;
            }

            // Calculate the cofactor, including sign
            float cofactor = ((row + col) % 2 == 0 ? 1 : -1) * determinant3x3(submatrix);

            // Assign the cofactor to the result matrix in transposed position
            result[col][row] = cofactor / det; // Note the transpose here by assigning to [col][row]
        }
    }

    return result;
}

// Function to calculate the transpose of a matrix
Matrix Matrix::transpose() const {
    Matrix result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i][j] = m[j][i];
        }
    }
    return result;
}

// Function to calculate the invert transpose of the matrix
Matrix Matrix::invertTranspose() const {
    // First, calculate the inverse of the matrix
    Matrix invMatrix = inverse();
    // Then, transpose the inverse matrix
    return invMatrix.transpose();
}

// Function to compute the magnitude (Euclidean norm) of a Vertex
float magnitude(const Vertex& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Function to transform a Vertex into a 4x1 Matrix
Matrix vertexToMatrix(const Vertex& vertex) {
    Matrix mat; // Create a 4x1 matrix

    // Assign the vertex coordinates to the first three rows
    mat[0][0] = vertex.x; // x-coordinate
    mat[1][0] = vertex.y; // y-coordinate
    mat[2][0] = vertex.z; // z-coordinate
    mat[3][0] = 1.0f;     // Homogeneous coordinate

    return mat; // Return the resulting matrix
}

// Function to create a viewport matrix
Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4); // Create a 4x4 identity matrix

    // Set the translation components for the viewport
    m[0][3] = x + w / 2.f; // Translate in x
    m[1][3] = y + h / 2.f; // Translate in y
    m[2][3] = depth / 2.f; // Translate in z

    // Set the scaling factors for the viewport
    m[0][0] = w / 2.f; // Scale in x
    m[1][1] = h / 2.f; // Scale in y
    m[2][2] = depth / 2.f; // Scale in z (if applicable)

    return m; // Return the resulting viewport matrix
}

Matrix projectionMatrix(float coeff) {
    Matrix m = Matrix::identity(4); // Create a 4x4 identity matrix

    m[3][2] = coeff; // Set the projection factor
    return m; // Return the resulting viewport matrix
}

Matrix lookAt(Vertex& camera, Vertex& target, Vertex& up) {
    // Calculate the forward vector
    Vertex forward = { camera.x - target.x, camera.y - target.y, camera.z - target.z };
    normalizeVertex(forward);

    // Calculate the right vector
    Vertex right = crossProduct(up, forward);
    normalizeVertex(right);

    // Calculate the up vector
    Vertex newUp = crossProduct(forward, right);
    normalizeVertex(newUp);

    // Set the view matrix
    Matrix Minv = Matrix::identity(4);
    Matrix Tr = Matrix::identity(4);

    Minv[0][0] = right.x;
    Minv[0][1] = right.y;
    Minv[0][2] = right.z;

    Minv[1][0] = newUp.x;
    Minv[1][1] = newUp.y;
    Minv[1][2] = newUp.z;

    Minv[2][0] = forward.x;
    Minv[2][1] = forward.y;
    Minv[2][2] = forward.z;

    Tr[0][3] = -camera.x;
    Tr[1][3] = -camera.y;
    Tr[2][3] = -camera.z;

    Matrix viewMatrix = Minv * Tr;
    return viewMatrix;
}