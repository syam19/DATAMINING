#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <iterator>
#include <functional> // For std::hash

using namespace std;

// Custom hash function for vector<int>
struct VectorHash {
    size_t operator()(const vector<int>& vec) const {
        size_t hash = 0;
        for (int num : vec) {
            hash ^= std::hash<int>()(num) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

// Custom equality function for vector<int>
struct VectorEqual {
    bool operator()(const vector<int>& lhs, const vector<int>& rhs) const {
        return lhs == rhs;
    }
};

class HashBasedApriori {
private:
    vector<vector<int>> transactions; // Transaction database
    int minSupport; // Minimum support threshold

    // Function to generate candidate itemsets of size k
    set<vector<int>> generateCandidates(const set<vector<int>>& prevItemsets, int k) {
        set<vector<int>> candidates;

        // Create candidates by merging previous itemsets
        for (auto it1 = prevItemsets.begin(); it1 != prevItemsets.end(); ++it1) {
            for (auto it2 = next(it1); it2 != prevItemsets.end(); ++it2) {
                vector<int> candidate;
                set_union(it1->begin(), it1->end(), it2->begin(), it2->end(), back_inserter(candidate));
                
                if (candidate.size() == k) {
                    candidates.insert(candidate);
                }
            }
        }

        return candidates;
    }

    // Function to count support of itemsets using a hash table
    unordered_map<vector<int>, int, VectorHash, VectorEqual> countSupport(const set<vector<int>>& candidates) {
        unordered_map<vector<int>, int, VectorHash, VectorEqual> supportCount;

        for (const auto& transaction : transactions) {
            for (const auto& candidate : candidates) {
                if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                    supportCount[candidate]++;
                }
            }
        }

        return supportCount;
    }

public:
    HashBasedApriori(const vector<vector<int>>& trans, int minSup)
        : transactions(trans), minSupport(minSup) {}

    void run() {
        set<vector<int>> frequentItemsets;
        int k = 1;

        // Generate initial itemsets (single items)
        set<vector<int>> candidates;
        for (const auto& transaction : transactions) {
            for (int item : transaction) {
                candidates.insert({item});
            }
        }

        // Repeat until no more frequent itemsets are found
        while (!candidates.empty()) {
            auto supportCount = countSupport(candidates);
            set<vector<int>> currentFrequentItemsets;

            for (const auto& pair : supportCount) {
                if (pair.second >= minSupport) {
                    currentFrequentItemsets.insert(pair.first);
                }
            }

            if (currentFrequentItemsets.empty()) break;

            frequentItemsets.insert(currentFrequentItemsets.begin(), currentFrequentItemsets.end());
            candidates = generateCandidates(currentFrequentItemsets, ++k);
        }

        // Output frequent itemsets
        cout << "Frequent Itemsets:\n";
        for (const auto& itemset : frequentItemsets) {
            cout << "{ ";
            for (int item : itemset) {
                cout << item << " ";
            }
            cout << "}\n";
        }
    }
};

int main() {
    // Example transaction database
    vector<vector<int>> transactions = {
        {1, 2, 3},
        {2, 3, 4},
        {1, 2, 4},
        {1, 2},
        {2, 3}
    };
    
    int minSupport = 2; // Minimum support threshold

    HashBasedApriori apriori(transactions, minSupport);
    apriori.run();

    return 0;
}
