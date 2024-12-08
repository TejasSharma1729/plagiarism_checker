#include <iostream>
#include <string>
#include <vector>
using namespace std;

class SmallestRectangleEnclosingBlackPixels {
    bool isZeroRow(vector<vector<char>>& image, int i, int n) {
        for (int j = 0; j < n; j++) if (image[i][j] != '0') return false;
        return true;
    }
    bool isZeroCol(vector<vector<char>>& image, int j, int xL, int xR) {
        for (int i = xL; i <= xR; i++) if (image[i][j] != '0') return false;
        return true;
    }

public:
    int minArea(vector<vector<char>>& image, int x, int y) {
        // Binary Search. BFS did not work.
        int m = image.size();
        int n = image[0].size();
        int i = -1;
        int j = x;
        int k = (i+j)/2;
        int xL, xR, yL, yR;

        while (j-i > 1) {
            if (isZeroRow(image, k, n)) {i = k; k = (i+j)/2;}
            else {j = k; k = (i+j)/2;}
        }
        xL = j;
        i = x;
        j = m;
        k = (i+j)/2;
        while (j-i > 1) {
            if (isZeroRow(image, k, n)) {j = k; k = (i+j)/2;}
            else {i = k; k = (i+j)/2;}
        }
        xR = i;
        i = -1;
        j = y;
        k = (i+j)/2;
        while (j-i > 1) {
            if (isZeroCol(image, k, xL, xR)) {i = k; k = (i+j)/2;}
            else {j = k; k = (i+j)/2;}
        }
        yL = j;
        i = y;
        j = n;
        k = (i+j)/2;
        while (j-i > 1) {
            if (isZeroCol(image, k, xL, xR)) {j = k; k = (i+j)/2;}
            else {i = k; k = (i+j)/2;}
        }
        yR = i;
        return (xR - xL + 1)*(yR - yL + 1);
    }
};
