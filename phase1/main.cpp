#include "match_submissions.hpp"
#include "../tokenizer.hpp"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <cmath>
#include <cassert>

// You should NOT modify ANYTHING in this file.
extern std::array<int, 5> match_submissions(std::vector<int> &submission1, 
        std::vector<int> &submission2);

namespace testcase {
    const std::string directory = "fellowship/";
    const std::vector<std::pair<std::string, std::string>> file_pairs = {
        {"saruman.cpp", "gollum.cpp"},
        {"saruman.cpp", "smeagol.cpp"},
        {"saruman.cpp", "frodo.cpp"},
        {"saruman.cpp", "aragorn.cpp"},
        {"saruman.cpp", "gandalf.cpp"},
        {"gollum.cpp", "smeagol.cpp"},
        {"gollum.cpp", "frodo.cpp"},
        {"gollum.cpp", "aragorn.cpp"},
        {"gollum.cpp", "gandalf.cpp"},
        {"smeagol.cpp", "frodo.cpp"},
        {"smeagol.cpp", "aragorn.cpp"},
        {"smeagol.cpp", "gandalf.cpp"},
        {"frodo.cpp", "aragorn.cpp"},
        {"frodo.cpp", "gandalf.cpp"},
        {"aragorn.cpp", "gandalf.cpp"}
    };
    const std::vector<std::array<int, 5>> correct = {
        {0, 50, 30, 320, 670},
        {1, 1640, 390, 560, 1340},
        {2, 200, 80, 1630, 1240},
        {0, 30, 0, 0, 0},
        {1, 1690, 1500, 140, 0},
        {1, 890, 600, 0, 180},
        {0, 0, 40, 770, 1070},
        {0, 0, 30, 740, 40},
        {0, 50, 40, 770, 1560},
        {2, 170, 80, 0, 1240},
        {0, 0, 40, 2440, 60},
        {1, 1470, 390, 1340, 420},
        {2, 100, 110, 1010, 0},
        {2, 120, 120, 1000, 1490},
        {1, 270, 270, 0, 1500}
        // 2 for first flag = borderline, both results are OK.
    };
    double* total_score;
    std::array<int, 5>* results;
    double are_equal_flag(int flag1, int flag2);
    double are_within_thirty(int len1, int len2);
    double compare_lengths(int len1, int len2);
    void handle_sigchld(int sig);
    void print_and_update_results(int index);
    void execute_testcase(std::vector<int> submission_one, 
            std::vector<int> submission_two);
    void testcase_wrapper(int index);
};

double testcase::are_equal_flag(int flag1, int flag2) {
    if (flag2 * flag2 != flag2) {
        return 1.0;
    }
    return flag1 == flag2 ? 1.0 : 0.0;
}

double testcase::are_within_thirty(int len1, int len2) {
    return std::max(len1, len2) - std::min(len1, len2) <= 30 ? 1.0 : 0.0;
}

double testcase::compare_lengths(int len1, int len2) {
    if (testcase::are_within_thirty(len1, len2)) {
        return 1.0;
    } else if (len1 == 0 || len2 == 0) {
        return 0.0;
    }
    double __ratio = std::max<double>(len1, len2) / std::min<double>(len1, len2);
    if (__ratio <= 1.1) {
        return 1.0;
    }
    return std::exp(-2 * (__ratio - 1.1));
}

void testcase::handle_sigchld(int sig) {
    int status;
    waitpid(-1, &status, WNOHANG);
    if (WIFSIGNALED(status)) {
        if (WTERMSIG(status) == SIGALRM) {
            exit(2);
        } else {
            exit(1);
        }
    }
    if (WIFEXITED(status)) {
        exit(0);
    }
    exit(3);
}

void testcase::print_and_update_results(int index) {
    printf("Results: %4d %4d %4d %4d %4d\n", testcase::results[0][0], 
            testcase::results[0][1], testcase::results[0][2], 
            testcase::results[0][3], testcase::results[0][4]);
    printf("Expected: %3d %4d %4d %4d %4d\n", testcase::correct[index][0],
            testcase::correct[index][1], testcase::correct[index][2],
            testcase::correct[index][3], testcase::correct[index][4]);
    *testcase::total_score += 0.1 * testcase::are_equal_flag(
            testcase::results[0][0], testcase::correct[index][0]);
    *testcase::total_score += 0.1 * testcase::compare_lengths(
            testcase::results[0][1], testcase::correct[index][1]);
}

void testcase::execute_testcase(std::vector<int> submission_one, 
        std::vector<int> submission_two) {
    signal(SIGCHLD, testcase::handle_sigchld);
    int pid = fork();
    if (pid == 0) {
        *results = match_submissions(submission_one, submission_two);
        exit(0);
    }
    usleep(15 * 1'000'000);
    kill(pid, SIGALRM);
    usleep(100000);
    exit(2);
}

void testcase::testcase_wrapper(int index) {
    std::cout << "Comparing " << testcase::file_pairs[index].first << 
            " and " << testcase::file_pairs[index].second << std::endl;
    int pid = 0;
    if ((pid = fork()) == 0) {
        tokenizer_t parser_one(testcase::directory + 
                testcase::file_pairs[index].first);
        tokenizer_t parser_two(testcase::directory + 
                testcase::file_pairs[index].second);
        std::vector<int> submission_one = parser_one.get_tokens();
        std::vector<int> submission_two = parser_two.get_tokens();
        testcase::execute_testcase(submission_one, submission_two);
        exit(0);
    }
    int status;
    waitpid(pid, &status, 0);
    switch (WEXITSTATUS(status)) {
        case 0:
            testcase::print_and_update_results(index);
            break;
        case 1:
            std::cout << "Runtime error" << std::endl;
            break;
        case 2:
            std::cout << "Time limit exceeded" << std::endl;
            break;
        case 3:
            std::cout << "Unknown error" << std::endl;
            break;
    }
}

int main(void) {
    testcase::total_score = static_cast<double*>(mmap(NULL, sizeof(double), 
                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    *testcase::total_score = 0.0;
    testcase::results = static_cast<std::array<int, 5>*>(mmap(NULL, 
                sizeof(std::array<int, 5>), PROT_READ | PROT_WRITE, 
                MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    *testcase::results = {0, 0, 0, 0, 0};
    for (int i = 0; i < 15; i++) {
        testcase::testcase_wrapper(i);
    }
    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Total score: " << *testcase::total_score << std::endl;
    munmap(static_cast<void*>(testcase::total_score), sizeof(double));
    munmap(static_cast<void*>(testcase::results), sizeof(std::array<int, 5>));
    return 0;
}
