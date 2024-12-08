#include <array>
#include <iostream>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

template <size_t Net, size_t N, typename... Args>
struct tuple_leaf;

template <size_t Net, size_t N, typename T, typename... Args> requires (N > 0) && (N <= sizeof...(Args) + 1)
struct tuple_leaf<Net, N, T, Args...> : public tuple_leaf<Net, N - 1, Args...> {
protected:
    T __value;
public:
    virtual ~tuple_leaf() = default;
    tuple_leaf() : tuple_leaf<Net, N - 1, Args...>(), __value() {}
    tuple_leaf(T __arg, Args... __args) : tuple_leaf<Net, N - 1, Args...>(__args...), __value(__arg) {}
    tuple_leaf(const tuple_leaf&) = default;
    tuple_leaf(tuple_leaf&&) = default;
    tuple_leaf& operator = (const tuple_leaf&) = default;
    tuple_leaf& operator = (tuple_leaf&&) = default;
    template <size_t Q> auto& __get() {
        if constexpr (Q == Net - N) {
            return __value;
        } else {
            return tuple_leaf<Net, N - 1, Args...>::template __get<Q>();
        }
    }
};

template <size_t Net, typename... Args> requires (sizeof...(Args) == 0)
struct tuple_leaf<Net, 0, Args...> {
    virtual ~tuple_leaf() = default;
    tuple_leaf() = default;
    tuple_leaf(const tuple_leaf&) = default;
    tuple_leaf(tuple_leaf&&) = default;
    tuple_leaf& operator = (const tuple_leaf&) = default;
    tuple_leaf& operator = (tuple_leaf&&) = default;
};

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
    inline int8_t count(avail_t& X) {
        int8_t c = 0; 
        for (int8_t i = 0; i < 9; i++) c += X[i]; 
        return c;
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

    size_t get_size(void) {
        return size;
    }

    size_t get_dim(int i) {
        return dims[i];
    }

    size_t get_stride(int i) {
        return strides[i];
    }

    T& operator()(std::span<size_t> indices) {
        size_t index = 0;
        for (int i = 0; i < sizeof...(Dims); i++) {
            index += indices[i] * strides[i];
        }
        return data[index];
    }

    T& operator()(size_t indices[sizeof...(Dims)]) {
        size_t index = 0;
        for (int i = 0; i < sizeof...(Dims); i++) {
            index += indices[i] * strides[i];
        }
        return data[index];
    }

protected:
    std::unique_ptr<T[]> data;
    size_t size;
    size_t dims[sizeof...(Dims)];
    size_t strides[sizeof...(Dims)];
};

template <typename... Args>
struct tuple : public tuple_leaf<sizeof...(Args), sizeof...(Args), Args...> {
    virtual ~tuple() = default;
    tuple() = default;
    tuple(Args... args) : tuple_leaf<sizeof...(Args), sizeof...(Args), Args...>(args...) {}
    tuple(const tuple&) = default;
    tuple(tuple&&) = default;
    tuple& operator = (const tuple& oth) = default;
    tuple& operator = (tuple&& oth) = default;
    template <size_t Q> auto& __get_val() {
        static_assert(Q < sizeof...(Args), "Tuple index out of range");
        return tuple_leaf<sizeof...(Args), sizeof...(Args), Args...>::template __get<Q>();
    }
};

int main(void) {
    std::cout << "Hello, World" << std::endl;
    return 0;
}
