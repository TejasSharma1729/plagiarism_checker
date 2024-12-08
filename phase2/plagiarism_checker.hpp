#include "structures.hpp"
// -----------------------------------------------------------------------------
#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <span>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
// You are free to add any STL includes above this comment, below the --line--.
// DO NOT add "using namespace std;" or include any other files/libraries.
// Also DO NOT add the include "bits/stdc++.h"

// OPTIONAL: Add your helper functions and classes here
struct suffix_node_t {
    std::unordered_map<int, std::unique_ptr<suffix_node_t>> children;
    std::unordered_set<long> present_in;
};

class plagiarism_checker_t {
    // You should NOT modify the public interface of this class.
public:
    plagiarism_checker_t(void);
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> 
                            __submissions);
    ~plagiarism_checker_t(void);
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    // TODO: Add members and function signatures here
    std::thread main_thread;
    std::queue<std::pair<double, std::shared_ptr<submission_t>>> 
        yet_added_submissions;
    std::mutex yet_added_mutex;
    std::atomic<bool> is_running = true;
    inline double get_time_now(void);
    void main_loop(void);

    std::shared_ptr<suffix_node_t> root;
    void insert_to_suffix_tree(std::shared_ptr<submission_t> submission);
    std::queue<std::pair<double, std::shared_ptr<submission_t>>> 
        waitlist_submissions;
    inline bool short_pattern_match(std::span<int> pattern, 
            int &num_pattern_matches, long __sub_id,
            std::map<long, int> &submission_matches);
    inline bool long_pattern_match(std::span<int> pattern);
    void check_submission(std::shared_ptr<submission_t> __submission);
    // End TODO
};
