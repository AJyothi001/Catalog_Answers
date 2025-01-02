#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;

// Function to decode a string in a given base to a long long integer
long long decodeValueInBase(const string& encodedValue, int base) {
    return stoll(encodedValue, nullptr, base);
}

// Function to perform Gaussian elimination on a matrix
vector<double> solveLinearSystem(vector<vector<double>> matrix, vector<double> constants) {
    int numRows = matrix.size();
    for (int pivotRow = 0; pivotRow < numRows; ++pivotRow) {
        // Normalize the pivot row
        double pivotElement = matrix[pivotRow][pivotRow];
        for (int col = 0; col < numRows; ++col) {
            matrix[pivotRow][col] /= pivotElement;
        }
        constants[pivotRow] /= pivotElement;

        // Eliminate the current column in other rows
        for (int row = 0; row < numRows; ++row) {
            if (row != pivotRow) {
                double factor = matrix[row][pivotRow];
                for (int col = 0; col < numRows; ++col) {
                    matrix[row][col] -= factor * matrix[pivotRow][col];
                }
                constants[row] -= factor * constants[pivotRow];
            }
        }
    }
    return constants; // The constants vector now contains the solution
}

void processJSONTestCase(const char* filename) {
    // Open the JSON file containing the encoded polynomial data
    FILE* filePointer = fopen(filename, "r");
    if (!filePointer) {
        cerr << "Error: Unable to open " << filename << endl;
        return;
    }

    // Read and parse the JSON file
    char buffer[65536];
    FileReadStream inputStream(filePointer, buffer, sizeof(buffer));
    Document jsonDocument;
    jsonDocument.ParseStream(inputStream);
    fclose(filePointer);

    // Extract the number of points and the polynomial degree
    const Value& keysData = jsonDocument["keys"];
    int numPoints = keysData["n"].GetInt();
    int degreeOfPolynomial = keysData["k"].GetInt();

    // Initialize the coefficient matrix and constants vector
    vector<vector<double>> matrix(numPoints, vector<double>(degreeOfPolynomial, 0));
    vector<double> constants(numPoints, 0);

    // Populate the matrix and vector with decoded values
    for (int i = 1; i <= numPoints; ++i) {
        string index = to_string(i);
        int xCoordinate = i;
        const Value& rootData = jsonDocument[index.c_str()];
        int base = rootData["base"].GetInt();
        string encodedValue = rootData["value"].GetString();
        long long decodedYValue = decodeValueInBase(encodedValue, base);
        constants[i - 1] = decodedYValue;
        for (int j = 0; j < degreeOfPolynomial; ++j) {
            matrix[i - 1][j] = pow(xCoordinate, degreeOfPolynomial - j - 1);
        }
    }

    // Solve the system using Gaussian elimination
    vector<double> solution = solveLinearSystem(matrix, constants);

    // Output the constant term, which is the last element in the solution vector
    cout << "The secret (constant term c) for " << filename << " is: " << solution.back() << endl;
}

int main() {
    // Process each test case
    processJSONTestCase("testcase1.json");
    processJSONTestCase("testcase2.json");

    return 0;
}
