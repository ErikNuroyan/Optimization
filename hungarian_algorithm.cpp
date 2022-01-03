#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <climits>

using namespace std;

void crossRow(bool* crossed, int rowIndex, int nDims) {
    for (int i = 0; i < nDims; ++i) {
        crossed[rowIndex * nDims + i] = false;
    }
}

void crossColumn(bool* crossed, int columnIndex, int nDims) {
    for (int i = 0; i < nDims; ++i) {
        crossed[i * nDims + columnIndex] = false;
    }
}

vector<pair<int, int>> markZeros(bool* crossed, int nDims, bool* markedRows) {
    bool done = false;
    vector<pair<int, int>> markedZeros;
    while (!done) {
        int minRow = INT_MAX;
        int minCol = INT_MAX;
        int minZeroCount = INT_MAX;
        for (int i = 0; i < nDims; ++i) {
            int sum = 0; 
            int zeroIndex = INT_MIN;
            for (int j = 0; j < nDims; ++j) {
                sum += crossed[i * nDims + j];
                if (zeroIndex == INT_MIN && crossed[i * nDims + j] == true) {
                    zeroIndex = j;
                }
            }
            
            if (sum > 0 && sum < minZeroCount) {
                minRow = i;
                minCol = zeroIndex;
                minZeroCount = sum;
            }
        }
        
        markedRows[minRow] = true;
        markedZeros.emplace_back(minRow, minCol);   
        crossRow(crossed, minRow, nDims);
        crossColumn(crossed, minCol, nDims);
        
        done = true;
        for (int i = 0; i < nDims; ++i) {
            for (int j = 0; j < nDims; ++j) {
                if (crossed[i * nDims + j] == true) {
                    done = false;
                }
            }
        }
    }

    return markedZeros;
}


std::vector<int> hungarianAlgorithm(int* matrix, int nDims) {
    //Per row minimum subtraction
    for (int i = 0; i < nDims; ++i) {
        int minIndex = std::distance(matrix + i * nDims,
                                     std::min_element(matrix + i * nDims, matrix + (i + 1) * nDims));
        int minValue = matrix[i * nDims + minIndex];
        for (int j = 0; j < nDims; ++j) {
            matrix[i * nDims + j] -= minValue;
        }
    }
    
    //Per column minimum subtraction
    for (int j = 0; j < nDims; ++j) {
        int minValue = INT_MAX;
        for (int i = 0; i < nDims; ++i) {
            minValue = std::min(minValue, matrix[i * nDims + j]);
        }
        
        for (int i = 0; i < nDims; ++i) {
            matrix[i * nDims + j] -= minValue;
        }
    }
    
    while (true) {
        int* intersectionCount = static_cast<int*>(std::calloc(1, nDims * nDims * sizeof(int)));
        bool* crossed = static_cast<bool*>(std::calloc(1, nDims * nDims));
        
        for (int i = 0; i < nDims; ++i) {
            for (int j = 0; j < nDims; ++j) {
                if (matrix[i * nDims + j] == 0 ) {
                    crossed[i * nDims + j] = true;
                }
            }
        }
        
        bool* markedRows = static_cast<bool*>(std::calloc(1, nDims));
        std::vector<std::pair<int, int>> markedZeros = markZeros(crossed, nDims, markedRows);
        bool checkSwitch = true;
        bool* markedColumns = static_cast<bool*>(std::calloc(1, nDims));
        while (checkSwitch) {
            for (int i = 0; i < nDims; ++i) {
                checkSwitch = false;
                if (markedRows[i]) {
                    continue;
                }
                
                for (int j = 0; j < nDims; ++j) {
                    if (matrix[i * nDims + j] == 0 && !markedColumns[j]) {
                        markedColumns[j] = true;
                        checkSwitch = true;
                    }
                }
            }
            
            for (auto markCoord : markedZeros) {
                if (markedRows[markCoord.first] && markedColumns[markCoord.second]) {
                    markedRows[markCoord.first] = false;
                    checkSwitch = true;
                }
            }
        }
        
        int sum = 0;
        for (int i = 0; i < nDims; ++i) {
            if (markedRows[i]) {
                ++sum;
            }
            if (markedColumns[i]) {
                ++sum;
            }
        }
        
        if (sum == nDims) {
            //The optimal solution is found
            free(intersectionCount);
            free(crossed);
            free(markedRows);
            free(markedColumns);
            
            std::cout << "Optimal solution is found\n";
            std::vector<int> optimalSolution(nDims);
            for (auto p : markedZeros) {
                optimalSolution[p.first] = p.second; 
            }
            
            return optimalSolution;
        }

        int minValue = INT_MAX;
        for (int i = 0; i < nDims; ++i) {
            for (int j = 0; j < nDims; ++j) {
                if (!markedRows[i] && !markedColumns[j]) {
                    minValue = std::min(matrix[i * nDims + j], minValue);               
                }
            }
        }
        
        for (int i = 0; i < nDims; ++i) {
            for (int j = 0; j < nDims; ++j) {
                if (!markedRows[i] && !markedColumns[j]) {
                    matrix[i * nDims + j] -= minValue;               
                }
                else if (markedRows[i] && markedColumns[j]) {
                    matrix[i * nDims + j] += minValue;
                }
            }
        }
        
        free(intersectionCount);
        free(crossed);
        free(markedRows);
        free(markedColumns);
    }
    
}

int main()
{
    int nDims = 5;
    
    // int matrix[] = {
    //     9, 11, 14, 11, 7,
    //     6, 15, 13, 13, 10,
    //     12, 13, 6, 8, 8,
    //     11, 9, 10, 12, 9,
    //     7, 12, 14, 10, 14
    // };
    int matrix[] = {
        85,	75,	65,	125, 75,
        90,	78,	66,	132, 78,
        75,	66,	57,	114, 69,
        80,	72,	60,	120, 72,
        76, 64,	56,	112, 68
    };
    
    std::vector<int> optimalSolution = hungarianAlgorithm(matrix, nDims);
    for (auto i: optimalSolution) {
        std::cout << i << std::endl;
    }
        
}
