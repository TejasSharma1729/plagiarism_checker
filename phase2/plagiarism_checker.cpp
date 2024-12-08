#include "plagiarism_checker.hpp"
// You should NOT add ANY other includes to this file.
// Do NOT add "using namespace std;".

// TODO: Implement the methods of the plagiarism_checker_t class
plagiarism_checker_t::plagiarism_checker_t(void) {
    main_thread = std::thread(&plagiarism_checker_t::main_loop, this);
}

plagiarism_checker_t::plagiarism_checker_t(
        std::vector<std::shared_ptr<submission_t>> __submissions) {
    this->root = std::make_shared<suffix_node_t>();
    for (auto submission : __submissions) {
        this->insert_to_suffix_tree(submission);
    }
    main_thread = std::thread(&plagiarism_checker_t::main_loop, this);
}

plagiarism_checker_t::~plagiarism_checker_t(void) {
    this->is_running = false;
    if (main_thread.joinable()) {
        main_thread.join();
    }
}

void plagiarism_checker_t::add_submission(
        std::shared_ptr<submission_t> __submission) {
    std::lock_guard<std::mutex> lock(this->yet_added_mutex);
    this->yet_added_submissions.push(
            std::make_pair(this->get_time_now(), __submission));
}

inline double plagiarism_checker_t::get_time_now(void) {
    std::chrono::time_point __now = std::chrono::high_resolution_clock::now();
    long __now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            __now.time_since_epoch()).count();
    return 1.0e-9 * __now_ns;
}

void plagiarism_checker_t::main_loop(void) {
    double time_now = 0.0;
    std::pair<double, std::shared_ptr<submission_t>> submission, previous;
    while (this->is_running) {
        std::queue<std::pair<double, std::shared_ptr<submission_t>>> temp_queue;
        while (true) {
            std::lock_guard<std::mutex> lock(this->yet_added_mutex);
            if (this->yet_added_submissions.empty())
                break;
            temp_queue.push(this->yet_added_submissions.front());
            this->yet_added_submissions.pop();
        }
        
        while (!temp_queue.empty()) {
            submission = temp_queue.front();
            temp_queue.pop();
            this->waitlist_submissions.push(submission);
            time_now = submission.first;

            while (time_now - this->waitlist_submissions.front().first >= 1.0) {
                previous = this->waitlist_submissions.front();
                this->waitlist_submissions.pop();
                this->check_submission(previous.second);
            }
            this->insert_to_suffix_tree(submission.second);
        }
    }

    while (!this->yet_added_submissions.empty()) {
        submission = this->yet_added_submissions.front();
        this->yet_added_submissions.pop();
        this->waitlist_submissions.push(submission);
        time_now = submission.first;

        if (time_now - this->waitlist_submissions.front().first >= 1.0) {
            previous = this->waitlist_submissions.front();
            this->waitlist_submissions.pop();
            this->check_submission(previous.second);
        }
        this->insert_to_suffix_tree(submission.second);
    }

    while (!this->waitlist_submissions.empty()) {
        submission = this->waitlist_submissions.front();
        this->waitlist_submissions.pop();
        this->check_submission(submission.second);
    }
}

void plagiarism_checker_t::insert_to_suffix_tree(
        std::shared_ptr<submission_t> submission) {
    tokenizer_t tokenizer(submission->codefile);
    std::vector<int> tokens = tokenizer.get_tokens();
    int num_tokens = tokens.size();
    for (int i = 0; i < num_tokens; i++) {
        suffix_node_t* current_node = this->root.get();
        for (int j = i; j < std::min(i + 70, num_tokens); j++) {
            if (current_node->children.find(tokens[j]) == 
                    current_node->children.end()) {
                current_node->children[tokens[j]] = 
                    std::make_unique<suffix_node_t>();
            }
            current_node = current_node->children[tokens[j]].get();
            if (j == i+14 || j == i+69) {
                current_node->present_in.insert(submission->id);
            }
        }
    }
}

inline bool plagiarism_checker_t::short_pattern_match(std::span<int> pattern,
        int &num_pattern_matches, long __sub_id,
        std::map<long, int> &submission_matches) {
    suffix_node_t* current_node = this->root.get();
    bool match = true;
    for (int j = 0; j < 15; j++) {
        if (current_node->children.find(pattern[j]) == 
                current_node->children.end()) {
            match = false;
            break;
        }
        current_node = current_node->children[pattern[j]].get();
    }
    if (match) {
        if (current_node->present_in.size() <= 1) {
            return false;
        }
        num_pattern_matches++;
        for (auto submission_id : current_node->present_in) {
            if (submission_id == __sub_id) {
                continue;
            }
            submission_matches[submission_id]++;
        }
    }
    return (match && current_node->present_in.size() > 1);
}

inline bool plagiarism_checker_t::long_pattern_match(std::span<int> pattern) {
    suffix_node_t* current_node = this->root.get();
    bool match = true;
    for (int j = 0; j < 70; j++) {
        if (current_node->children.find(pattern[j]) == 
                current_node->children.end()) {
            match = false;
            break;
        }
        current_node = current_node->children[pattern[j]].get();
    }
    return (match && current_node->present_in.size() > 1);
}

void plagiarism_checker_t::check_submission(
        std::shared_ptr<submission_t> __submission) {
    bool flag_student = false;
    tokenizer_t tokenizer(__submission->codefile);
    std::vector<int> __tokens = tokenizer.get_tokens();
    std::span<int> tokens = std::span(__tokens);
    int num_tokens = tokens.size();
    int num_pattern_matches = 0;

    std::map<long, int> submission_matches;
    for (int i = 0; i < num_tokens - 15; i++) {
        if (this->short_pattern_match(tokens.subspan(i, 15),
                num_pattern_matches, __submission->id, submission_matches)) {
            i += 14;
        }
    }
    int max_common_patterns_submisssion_wise = 0;
    if (num_pattern_matches >= 20) {
        flag_student = true;
    }
    for (auto itr = submission_matches.begin(); 
            itr != submission_matches.end(); itr++) {
        if (itr->first == __submission->id) {
            continue;
        }
        if (itr->second > max_common_patterns_submisssion_wise) {
            max_common_patterns_submisssion_wise = itr->second;
        }
    }
    if (max_common_patterns_submisssion_wise >= 10) {
        flag_student = true;
    }
    for (int i = 0; !flag_student && i < num_tokens - 70; i += 10) {
        if (this->long_pattern_match(tokens.subspan(i, 70))) {
            flag_student = true;
            break;
        }
    }
    if (flag_student) {
        __submission->student->flag_student(__submission);
        __submission->professor->flag_professor(__submission);
    }
}
// End TODO
