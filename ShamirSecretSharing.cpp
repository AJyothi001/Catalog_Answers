#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;
long long decodeValueInBase(const string& encodedValue, int base) {
    return stoll(encodedValue, nullptr, base);
}
vector<double> solveLinearSystem(vector<vector<double>> matrix, vector<double> constants) {
    int numRows = matrix.size();
    for (int pivotRow = 0; pivotRow < numRows; ++pivotRow) {
        double pivotElement = matrix[pivotRow][pivotRow];
        for (int col = 0; col < numRows; ++col) {
            matrix[pivotRow][col] /= pivotElement;
        }
        constants[pivotRow] /= pivotElement;
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
    return constants; 
}

void processJSONTestCase(const char* filename) {
    FILE* filePointer = fopen(filename, "r");
    if (!filePointer) {
        cerr << "Error: Unable to open " << filename << endl;
        return;
    }

    char buffer[65536];
    FileReadStream inputStream(filePointer, buffer, sizeof(buffer));
    Document jsonDocument;
    jsonDocument.ParseStream(inputStream);
    fclose(filePointer);
    const Value& keysData = jsonDocument["keys"];
    int numPoints = keysData["n"].GetInt();
    int degreeOfPolynomial = keysData["k"].GetInt();
    vector<vector<double>> matrix(numPoints, vector<double>(degreeOfPolynomial, 0));
    vector<double> constants(numPoints, 0);
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
    vector<double> solution = solveLinearSystem(matrix, constants);
    cout << "The secret (constant term c) for " << filename << " is: " << solution.back() << endl;
}

int main() {
    processJSONTestCase("testcase1.json");
    processJSONTestCase("testcase2.json");

    return 0;
}
