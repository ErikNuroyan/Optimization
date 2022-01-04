#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <climits>

using namespace std;

void crossRow(std::vector<bool>& crossed, int rowIndex, int nDims) {
    for (int i = 0; i < nDims; ++i) {
        crossed[rowIndex * nDims + i] = false;
    }
}

void crossColumn(std::vector<bool>& crossed, int columnIndex, int nDims) {
    for (int i = 0; i < nDims; ++i) {
        crossed[i * nDims + columnIndex] = false;
    }
}

vector<pair<int, int>> markZeros(std::vector<bool>& crossed, std::vector<bool>& markedRows, int nDims) {
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

std::vector<int> hungarianAlgorithm(std::vector<int>& matrix, int nDims) {
    //Per row minimum subtraction
    for (int i = 0; i < nDims; ++i) {
        int minIndex = std::distance(matrix.begin() + i * nDims,
                                     std::min_element(matrix.begin() + i * nDims, matrix.begin() + (i + 1) * nDims));
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
        std::vector<bool> crossed(nDims * nDims, false);
        for (int i = 0; i < nDims; ++i) {
            for (int j = 0; j < nDims; ++j) {
                if (matrix[i * nDims + j] == 0) {
                    crossed[i * nDims + j] = true;
                }
            }
        }
        
        std::vector<bool> markedRows(nDims, false);
        std::vector<std::pair<int, int>> markedZeros = markZeros(crossed, markedRows, nDims);
        bool checkSwitch = true;
        std::vector<bool> markedColumns(nDims, false);
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
        
        auto predicate = [](int i) {return i != 0;};
        int sum = std::count_if(markedRows.begin(), markedRows.end(), predicate);
        sum += std::count_if(markedColumns.begin(), markedColumns.end(), predicate);
        
        if (sum == nDims) {
            //Optimal solution is found
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
    }
}

int main()
{
    int nDims = 5;
    
    // int matrix[] = {
    //     9, 11, 14, 11, 7 ,
    //     6, 15, 13, 13, 10,
    //     12, 13, 6, 8,  8 ,
    //     11, 9, 10, 12, 9 ,
    //     7, 12, 14, 10, 14
    // };
    vector<int> matrix = {
        85,	75,	65,	125, 75,
        90,	78,	66,	132, 78,
        75,	66,	57,	114, 69,
        80,	72,	60,	120, 72,
        76, 64,	56,	112, 68
    };
    
    std::vector<int> optimalSolution = hungarianAlgorithm(matrix, nDims);
    for (auto i: optimalSolution) {
        std::cout << i << " ";
    }
}