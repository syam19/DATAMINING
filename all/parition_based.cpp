#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <iterator>

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

class Apriori {
private:
    vector<vector<int>> transactions; // Transaction database
    int minSupport; // Minimum support threshold
    int numPartitions; // Number of partitions

    // Function to generate candidate itemsets of size k
    set<vector<int>> generateCandidates(const set<vector<int>>& prevItemsets, int k) {
        set<vector<int>> candidates;

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

    // Function to count support of itemsets in a given partition
    unordered_map<vector<int>, int, VectorHash, VectorEqual> countSupport(const vector<vector<int>>& partition, const set<vector<int>>& candidates) {
        unordered_map<vector<int>, int, VectorHash, VectorEqual> supportCount;

        for (const auto& transaction : partition) {
            for (const auto& candidate : candidates) {
                // Check if candidate is included in the transaction
                if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                    supportCount[candidate]++;
                }
            }
        }

        return supportCount;
    }

public:
    Apriori(const vector<vector<int>>& trans, int minSup, int partitions)
        : transactions(trans), minSupport(minSup), numPartitions(partitions) {}

    void run() {
        set<vector<int>> frequentItemsets;

        // Generate initial itemsets (single items)
        set<vector<int>> candidates;
        for (const auto& transaction : transactions) {
            for (int item : transaction) {
                candidates.insert({item});
            }
        }

        // Process each partition
        for (int i = 0; i < numPartitions; ++i) {
            int start = i * (transactions.size() / numPartitions);
            int end = (i == numPartitions - 1) ? transactions.size() : (i + 1) * (transactions.size() / numPartitions);
            vector<vector<int>> partition(transactions.begin() + start, transactions.begin() + end);

            // Count support for candidates in the current partition
            auto supportCount = countSupport(partition, candidates);
            set<vector<int>> currentFrequentItemsets;

            // Filter candidates to find frequent itemsets in the partition
            for (const auto& pair : supportCount) {
                if (pair.second >= minSupport) {
                    currentFrequentItemsets.insert(pair.first);
                }
            }

            // Merge frequent itemsets from this partition with global frequent itemsets
            frequentItemsets.insert(currentFrequentItemsets.begin(), currentFrequentItemsets.end());
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
    int numPartitions = 2; // Number of partitions

    Apriori apriori(transactions, minSupport, numPartitions);
    apriori.run();

    return 0;
}
