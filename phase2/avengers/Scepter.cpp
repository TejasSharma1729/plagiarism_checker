#include <array>
#include <iostream>
#include <vector>
using namespace std;

namespace sudoku_solver {
    typedef array<array<array<int8_t, 9>, 9>, 9> options_t;
    typedef array<array<int8_t, 9>, 9> board_t;
    typedef array<int8_t, 9> avail_t;
    struct config_t {
        options_t options;
        board_t board;
    };
    struct mincount_t {
        int8_t count;
        int8_t x;
        int8_t y;
    };
    inline int8_t count(avail_t &X);
    inline avail_t at(board_t &board, int8_t x, int8_t y);
    inline void set_config(config_t &config);
    inline void set_square(config_t &config, int8_t x, int8_t y, int8_t val);
    config_t recurse_solve(mincount_t mincount, config_t main);
    inline int8_t set_single_count(config_t &config);
    inline int8_t filled(config_t &config);
    inline mincount_t min_count(config_t &config);
};

inline int8_t sudoku_solver::count(avail_t& X) {
    int8_t c = 0; 
    for (int8_t i = 0; i < 9; i++) c += X[i]; 
    return c;
}

inline sudoku_solver::avail_t sudoku_solver::at(board_t& board, int8_t x, int8_t y) {
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

inline void sudoku_solver::set_config(config_t& config) {
    for (int8_t i = 0; i < 9; i++)
        for (int8_t j = 0; j < 9; j++)
            config.options[i][j] = at(config.board, i, j);
}

inline void sudoku_solver::set_square(config_t& config, int8_t x, int8_t y, int8_t val) {
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

sudoku_solver::config_t sudoku_solver::recurse_solve(mincount_t mincount, config_t main) {
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

inline int8_t sudoku_solver::set_single_count(config_t& config) {
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

inline int8_t sudoku_solver::filled(config_t& config) {
    int8_t result = 1;
    for (int8_t i = 0; i < 9; i++) for (int8_t j = 0; j < 9; j++) {
        result = min(result, config.board[i][j]);
    }
    return result;
}

inline sudoku_solver::mincount_t sudoku_solver::min_count(config_t& config) {
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

void solve_sudoku(vector<vector<char>>& board) {
    using namespace sudoku_solver;
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
