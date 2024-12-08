#include <array>
#include <cmath>
#include <iostream>
#include <span>
#include <vector>
using namespace std;
#define eps 1.0e-10
// Time Complexity: O(log(n) * q + n), Space Complexity: O(n + q)
// Here, int n = no. of vertices in polygon; int q = no. of queries. (1 per query)

template <typename T, size_t... Dims>
class ndarray {
public:
    ndarray(size_t __size, size_t __dims[sizeof...(Dims)]) : size(__size) {
        for (int i = 0; i < sizeof...(Dims); i++) {
            dims[i] = __dims[i];
        }
        strides[sizeof...(Dims) - 1] = 1;
        for (int i = sizeof...(Dims) - 2; i >= 0; i--) {
            strides[i] = strides[i + 1] * dims[i + 1];
        }
        data = std::make_unique<T[]>(size);
    }

    T& operator()(size_t indices[sizeof...(Dims)]) {
        size_t index = 0;
        for (int i = 0; i < sizeof...(Dims); i++) {
            index += indices[i] * strides[i];
        }
        return data[index];
    }

    T& operator()(std::span<size_t> indices) {
        size_t index = 0;
        for (int i = 0; i < sizeof...(Dims); i++) {
            index += indices[i] * strides[i];
        }
        return data[index];
    }

    size_t get_size(void) {
        return size;
    }

    size_t get_dim(int i) {
        return dims[i];
    }

    size_t get_stride(int i) {
        return strides[i];
    }
protected:
    std::unique_ptr<T[]> data;
    size_t size;
    size_t dims[sizeof...(Dims)];
    size_t strides[sizeof...(Dims)];
};

bool isZero(double x) {
    return (x < eps && x > -eps);
} 

bool isEqual(const double& x, const double& y) {
    return (x - y <= x * eps && y - x <= x * eps);
}
// Self-explanatory (can't equate floating points!)

double areaTriangle(double x1, double y1, double x2, double y2, double x3, double y3) {
    double sum = 0;
    sum += x1 * (y2 - y3);
    sum += x2 * (y3 - y1);
    sum += x3 * (y1 - y2);
    return sum; 
    // Not really the area (actually +-2 times),
     // but kind of checks the way we rotate to get x1 x2 x3 clockwise or anticlockwise.
}

// This function checks whether the vertex (x, y) is 
// inside the triangle pt0, pt1, pt2 or on or outside. 
// Also what to do next if it is on or outside?
int inTriangle(vector<array<double, 2>> vertices, double x, double y, int m) {
    // m varies from 1 to n - 2 strictly inclusive.
    double x1 = vertices[m][0], x2 = vertices[m+1][0];
    double y1 = vertices[m][1], y2 = vertices[m+1][1];
    double x0 = vertices[0][0], y0 = vertices[0][1];
    int n = vertices.size();

    if (isEqual(x, x0) && isEqual(y, y0)) 
        return 0;
    // Nomenclature:
    // point 0 [pt0] is the first vertex (x0 y0) of polygon from array,  or 0th position. 
    // point 1 [pt1] is vertex at mth position, 
    // point 2 [pt2] at (m+1)th position.

    double area = areaTriangle (x1, y1, x2, y2, x0, y0);
    double pointArea0 = area*areaTriangle (x1, y1, x2, y2, x, y);
    double pointArea1 = area*areaTriangle (x2, y2, x0, y0, x, y);
    double pointArea2 = area*areaTriangle (x0, y0, x1, y1, x, y);
    // The whole point of these: pointArea0 > 0 means point (x, y) is on 
    // SAME side to line (pt1, pt2) as pt0.
    // < 0 means OPPOSITE side, same points.
    // == 0 means point (x, y),
    // point 1, point 2 are COLLINEAR.
    // Come up with similar definitions for pointArea1 and pointArea2 yourself. 
    // These kindof identify whether given point is 
    // inside, on or outside triangle(pt0, pt1, pt2)
    // Nomenclature (for returning) 0 ==> CONFIRMED point on the polygon. 
    // 1 ==> CONFIRMED point outside the polygon. -1 ==> CONFIRMED point inside polygon.
    // 2/3 ==> INCONCLUSIVE cases.

    if (pointArea0 < 0) 
        return 1; 
    // Think of a drawing part of the polygon and look at this triangle. 
    // Case: (x, y) lies beyond the side opposite to pt0 and 
    // CANNOT lie on or inside polygon.

    else if (isZero(pointArea0)) { 
        // So (x, y) is collinear to pt1 and pt2. Then it is on or outside.
        double RATIO;
        (isEqual(x1, x2)) ? RATIO = (y - y1)/(y2 - y1) : RATIO = (x - x1)/(x2 - x1); 
        // Ratio similar to m1 / (m1 + m2) for point on line, 
        // at which it divides line segment between pt1 and pt2
        
        if (RATIO >= 0 && RATIO <= 1) { 
            // Is (x, y) in the line SEGMENT? 
            // (Endpoints are accepted. Those are the vertices)
            return 0; 
            // If so, then point is on polygon.
        } 
        else return 1; 
        // So point is outside the line segment and polygon. 
        // Case: square (+-1, +-1), point (10000, 1)
    } 

    // OK. After this we know pointArea1 > 0 necessarily. Now what?
    else if (isZero(pointArea2) && m == 1 && pointArea1 >= 0) 
        return 0; 
    // So it lies on segment (pt0, pt1) and it is the first edge; 
    // it can be treated as 'on the polygon'.
    else if (isZero(pointArea1) && m == n - 2 && pointArea2 >= 0) 
        return 0;
    // Same rule, just for segment (pt0, pt2) and now the last edge;
    // NOTE: pointArea1 >= 0 (firstcase), 
    // pointArea2 >= 0 (secondcase) 
    // and pointArea0 > 0 (both cases) 
    // This (three rules) ensures that point lies on line SEGMENT not just on line.

    else if (pointArea1 >= 0 && pointArea2 >= 0) {
        return -1; 
        // Just a point inside the triangle. (or On it and not on an edge).
    }
    else if (pointArea2 < 0) { 
        // Think of the meaning of this.
        if (m == 1) {
            return 1; 
            // Certainly outside. 
            // Think of multiple triangle and rotation/angles about pt0.
        } 
        else {
            if (pointArea1 < 0) 
                return 1; 
            // Extend BOTH line segments from pt0 backward.
            // A point inside polygon CANNOT lie on the opposite side.
            else return 2;
            // Could be outside, middle or inside. Inconclusive.
            // Go check the left triangles for if inside or on, 
            // it is surely on those. 
            // (back to main algorithm).
        }
    }
    else if (pointArea1 < 0) {
        if (m == n - 2) {
            return 1;
        }
        else {
            if (pointArea2 < 0) return 1; 
            // If you think about it a bit, it is an exhaustive case check.
            else return 3; 
            // Very similar to the last block, 
            // just asked to check triangles on the right (bakc to main algorithm).
        }
    } 
    return 1;
    // The function won't really go to this case. Just to not give warning...
}

// The main function. 
// Check whether the point is inside or 'on polygon' traingle wise
// (join each line segment to the point pt0 to get n - 2 triangles).
int posPoint(vector<array<double, 2>> vertices, double x, double y) { 
    // NOTE: vertices is array of length 2n not n. 
    // x and y coordinates for each point.
    int n = vertices.size();
    int left = 1;
    int right = n - 1;
    int mid = (left + right) / 2; 
    // Like binary search.

    while (right > left) { 
        // This will execute inTriangle only once in case of triangle.
        // There m = 1 = n-2 ofc.
        int midTriangle = inTriangle (vertices, x, y, mid);

        if (midTriangle == 0) 
            return 0;
        else if (midTriangle == 1) 
            return 1;
        else if (midTriangle == -1) 
            return -1; 
        // Certain cases. Exit fn.

        else if (midTriangle == 2) {
            right = mid;
            mid = (left + right)/2;
        } 
        else if (midTriangle == 3) {
            left = mid;
            mid = (left + right)/2;
        } 
        // Compare with binary search of integers how we check triangles.
    } 
    return 1;
}

class SudokuSolver {
    typedef std::array<std::array<std::array<int8_t, 9>, 9>, 9> options_t;
    typedef std::array<std::array<int8_t, 9>, 9> board_t;
    typedef std::array<int8_t, 9> avail_t;
    struct config_t {
        options_t options;
        board_t board;
    };
    struct mincount_t {
        int8_t count;
        int8_t x;
        int8_t y;
    };
    inline int8_t count(avail_t& X) {
        int8_t c = 0; 
        for (int8_t i = 0; i < 9; i++) c += X[i]; 
        return c;
    }
    inline avail_t at(board_t& board, int8_t x, int8_t y) {
        if (x >= 9 || y >= 9 || x < 0 || y < 0) abort();
        avail_t A = {1, 1, 1, 1, 1, 1, 1, 1, 1};
        if (board[x][y]) 
            for (int8_t k = 0; k < 9; k++) 
                if (k != board[x][y]-1) A[k] = 0;
        for (int8_t i = 0; i < 9; i++) 
            if (i != y && board[x][i])
                A[board[x][i] - 1] = 0;
        for (int8_t j = 0; j < 9; j++)
            if (j != x && board[j][y])
                A[board[j][y] - 1] = 0;
        for (int8_t j = x/3 * 3; j < (x+3)/3 * 3; j++)
            for (int8_t i = y/3 * 3; i < (y+3)/3 * 3; i++)
                if (j != x && i != y && board[j][i])
                    A[board[j][i] - 1] = 0;
        return A;
    }
    inline int8_t set_single_count(config_t& config) {
        int8_t result = 0;
        for (int8_t i = 0; i < 81; i++) {
            if (!config.board[i/9][i%9] && count(config.options[i/9][i%9]) == 1) {
                for (int8_t j = 0; j < 9; j++) if (config.options[i/9][i%9][j]) {
                    set_square(config, i/9, i%9, j+1);
                }
                result++;
            }
        }
        return result;
    }
    inline void set_square(config_t& config, int8_t x, int8_t y, int8_t val) {
        if (x >= 9 || y >= 9 || x < 0 || y < 0 || val < 0 || val > 9) abort();
        config.board[x][y] = val;
        for (int8_t i = 0; i < 9; i++)
            if (i != val-1)
                config.options[x][y][i] = 0;
        for (int8_t i = 0; i < 9; i++)
            if (i != y) config.options[x][i][val - 1] = 0;
        for (int8_t j = 0; j < 9; j++)
            if (j != x) config.options[j][y][val - 1] = 0;
        for (int8_t j = x/3 * 3; j < (x+3)/3 * 3; j++)
            for (int8_t i = y/3 * 3; i < (y+3)/3 * 3; i++)
                if (j != x && i != y) config.options[j][i][val - 1] = 0;
    }
    inline void set_config(config_t& config) {
        for (int8_t i = 0; i < 9; i++)
            for (int8_t j = 0; j < 9; j++)
                config.options[i][j] = at(config.board, i, j);
    }
    config_t recurse_solve(mincount_t mincount, config_t main) {
        config_t temp = main;
        avail_t options = main.options[mincount.x][mincount.y];
        for (int8_t i = 0; i < 9; i++) if (options[i]) {
            main = temp;
            set_square(main, mincount.x, mincount.y, i+1);
            while (set_single_count(main));
            mincount_t res;
            if ((res = min_count(main)).count != 1) 
            {
                if (res.count == 0) continue;
                if (res.count > 9) {
                    return main;
                }
                config_t result = recurse_solve(res, main);
                if (!min_count(result).count || !filled(result)) {
                    continue;
                }
                return result;
            }
            else {
                return main;
            }
        }
        return temp;
    }
    inline mincount_t min_count(config_t& config) {
        mincount_t min_count = {10, -1, -1};
        for (int8_t i = 0; i < 9; i++) for (int8_t j = 0; j < 9; j++) {
            if (config.board[i][j]) continue;
            int getcount;
            if ((getcount = count(config.options[i][j])) < min_count.count) {
                min_count = (mincount_t){count(config.options[i][j]), i, j};
            }
        }
        return min_count;
    }
    inline int8_t filled(config_t& config) {
        int8_t result = 1;
        for (int8_t i = 0; i < 9; i++) for (int8_t j = 0; j < 9; j++) {
            result = std::min(result, config.board[i][j]);
        }
        return result;
    }
public:
    void solveSudoku(std::vector<std::vector<char>>& board) {
        config_t main;
        for (int8_t i = 0; i < 9; i++) for (int8_t j = 0; j < 9; j++) {
            main.board[i][j] = ((board[i][j] == '.') ? 0 : board[i][j] - '0');
        }
        set_config(main);
        mincount_t res;
        while (set_single_count(main));
        if ((res = min_count(main)).count == 0) return; // no solutions
        if (res.count < 10) main = recurse_solve(res, main);
        for (int8_t i = 0; i < 9; i++) for (int8_t j = 0; j < 9; j++) {
            board[i][j] = main.board[i][j] + '0';
        }
    }
};

int main() {
    cout << endl << "Enter number of vertices of convex polygon: ";
    int n; cin >> n; cout << endl;
    vector<array<double, 2>> points(n);
    for (int i = 0; i < n; i++) {
        cout << 
            "Enter x and y coordinates [space separated floating point numbers] of vertex "
            << i+1 << ": ";
        // YOU HAVE TO input meaningful coordinates that form a convex polygon 
        // IN THE SAME ORDER of atleast three vertices.
        // Otherwise expect random outputs.
        cin >> points[i][0] >> points[i][1];
    }
    cout << endl;

    while (true) {
        double x, y;
        cout << "Enter x and y coordinates of point to check: "; 
        cin >> x >> y;
        int posn = posPoint(points, x, y);

        if (posn == -1) {
            cout << "Point (" << x << ", " << y << ") lies inside the polygon." << endl;
        } 
        else if (posn == 0) {
            cout << "Point (" << x << ", " << y << ") lies on the polygon." << endl;
        } 
        else if (posn == 1) {
            cout << "Point (" << x << ", " << y << ") lies outside the polygon." << endl;
        } 
        int chk = 0;
        cout << "Enter 1 to check another vertex, 0 to exit: "; cin >> chk;
        if (chk) {
            cout << endl;
            continue;
        } 
        else {
            cout << endl << "Thank you!" << endl << endl;
            break;
        }
    }
}
