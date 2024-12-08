#include <iostream>
#include <set>
#include <string>
#include <vector>
using namespace std;

class AlienDictionary {
    #define ALPHABET_SIZE 26
    // Note: each character maps to (int)c - 'a'.
    struct node {
        int8_t val;
        node* next;
        node(int v) : val(v), next(nullptr) {}
    };
    // node -- for the graph edges [char_val is greater than node_pos]
    struct Node {
        int8_t visited;
        // 0 (white, unvisited),
        // 1 (parent, for cycle detection),
        // 2 (visited but not parent)
        node* next;
        Node() : visited(0), next(nullptr) {}
    };
    // Node -- for the graph.
    struct List {
        int val;
        List* next;
        List(int v) : val(v), next(nullptr) {}
    };
    // List -- for the topological sort.
    typedef Node* Ptr;
    typedef node* ptr;
    void destroy(Ptr t) {
        if (t->next != nullptr) {
            destroy(t->next);
        }
        delete t;
    }
    void destroy(ptr t) {
        if (t->next != nullptr) {
            destroy(t->next);
        }
        delete t;
    }
    void destroy(List* t) {
        if (t->next != nullptr) {
            destroy(t->next);
        }
        delete t;
    }

    // MAIN CLASS VARIABLES
    // abcd...z only. But in integer indices. 0 -- 25
    Ptr* alphabets; 
    bool* listed;

    pair<List*, List*> dfs(int i) {
        List* start = new List(i);
        List* end = start;
        alphabets[i]->visited = 1;
        ptr temp = alphabets[i]->next;

        while (temp != nullptr) {
            if (alphabets[temp->val]->visited == 1) {
                throw(0); 
            } 
            // cycle detected.
            if (alphabets[temp->val]->visited == 0) {
                List* store = start->next;
                auto iter = dfs(temp->val);
                start->next = iter.first;
                iter.second->next = store;
                if (end == start) {
                    end = iter.second;
                }
            }
            temp = temp->next;
        }
        alphabets[i]->visited = 2;
        return {start, end};
    }

 public:
    // O(sum of string lengths + 26^2) time; O(26^2) space.
    string alienOrder(vector<string>& words) {
        int n = words.size();
        if (n == 1) {
            class set<char> temp(words[0].begin(), words[0].end());
            return string(temp.begin(), temp.end());
        }
        alphabets = new Ptr[ALPHABET_SIZE];
        listed = new bool[ALPHABET_SIZE];
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            alphabets[i] = new Node;
            listed[i] = 0;
        }
        for (int j = 0; j < words[0].length(); j++) {
            listed[(int)words[0][j] - 'a'] = 1;
        }
        // Populate all orders in graph. Then do the figuring out. 
        // This takes O(sum of chars) time.
        for (int i = 1; i < n; i++) {
            for (int j = 0; j < words[i].length(); j++) {
                listed[(int)words[i][j] - 'a'] = 1;
            }
            int j = 0;
            while (j < words[i - 1].length()) {
                if (j == words[i].length()) {
                    for (int k = 0; k < ALPHABET_SIZE; k++) {
                        destroy(alphabets[k]);
                    }
                    delete[] alphabets;
                    return ""; 
                    // No lexicographic order has later word a prefix
                    // of former.
                }
                int x = (int)words[i - 1][j] - 'a';
                int y = (int)words[i][j] - 'a';
                if (x != y) {
                    ptr temp = alphabets[x]->next;
                    alphabets[x]->next = new node(y);
                    alphabets[x]->next->next = temp;
                    break;
                }
                j++;
            }
        }

        List* start = new List(-1);
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (listed[i] == 1 && alphabets[i]->visited == 0) {
                try {
                    List* store = start->next;
                    auto iter = dfs(i);
                    start->next = iter.first;
                    iter.second->next = store;
                } 
                catch (int myNum) {
                    for (int j = 0; j < ALPHABET_SIZE; j++) {
                        destroy(alphabets[j]);
                    }
                    delete[] alphabets;
                    destroy(start);
                    return "";
                }
            }
        }

        string result;
        List* end = start->next;
        while (end != nullptr) {
            result += (char)(end->val + 'a');
            end = end->next;
        }
        destroy(start);
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            destroy(alphabets[i]);
        }
        delete[] alphabets;
        return result;
    }
};
