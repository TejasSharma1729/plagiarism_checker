#include <iostream>
#include <limits>
#include <string>
#include <vector>
using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <name> <age> <height>" << endl;
        return 1;
    }
    int n1 = stoi(argv[1]);
    int n2 = stoi(argv[2]);
    int n3 = stoi(argv[3]);
    vector<vector<vector<long double>>> DP(n1 + 1, vector<vector<long double>>(
                n2 + 1, vector<long double>(n3 + 1, 0.0L)));
    DP[0][0][0] = 1.0L;
    for (int i = 0; i <= n1; i++) {
        for (int j = 0; j <= n2; j++) {
            DP[i][j][0] = 1.0L;
            if (i == 0 || j == 0) 
                for (int k = 0; k < n3; k++) 
                    DP[i][j][k] = 1.0L;
            else 
                for (int k = 1; k <= n3; k++) 
                    DP[i][j][k] = DP[i-1][j][k] + DP[i][j-1][k] + DP[i][j][k-1];
        }
    }
    cout.precision(numeric_limits<long double>::digits10);
    cout << DP[n1][n2][n3] << endl;
    return 0;
}
