#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>

using namespace std;

// Function to generate candidate itemsets of size k
set<set<int>> generate_candidates(const set<set<int>> &frequent_itemsets, int k) {
    set<set<int>> candidates;
    for (auto it1 = frequent_itemsets.begin(); it1 != frequent_itemsets.end(); ++it1) {
        for (auto it2 = next(it1); it2 != frequent_itemsets.end(); ++it2) {
            set<int> candidate;
            auto first1 = it1->begin(), first2 = it2->begin();
            while (first1 != it1->end() && first2 != it2->end() && candidate.size() < k - 1) {
                if (*first1 == *first2) {
                    candidate.insert(*first1);
                    ++first1;
                    ++first2;
                } else if (*first1 < *first2) {
                    candidate.insert(*first1++);
                } else {
                    candidate.insert(*first2++);
                }
            }
            while (first1 != it1->end() && candidate.size() < k - 1)
                candidate.insert(*first1++);
            while (first2 != it2->end() && candidate.size() < k - 1)
                candidate.insert(*first2++);
            if (candidate.size() == k - 1) {
                set<int> union_set = *it1;
                union_set.insert(it2->begin(), it2->end());
                if (union_set.size() == k)
                    candidates.insert(union_set);
            }
        }
    }
    return candidates;
}

// Function to count the support of candidates in the transaction database
map<set<int>, int> count_support(const set<set<int>> &candidates, const vector<set<int>> &transactions) {
    map<set<int>, int> support_count;
    for (const auto &transaction : transactions) {
        for (const auto &candidate : candidates) {
            if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                support_count[candidate]++;
            }
        }
    }
    return support_count;
}

// Function to filter itemsets based on minimum support threshold
set<set<int>> filter_itemsets(const map<set<int>, int> &support_count, int min_support) {
    set<set<int>> frequent_itemsets;
    for (const auto &item : support_count) {
        if (item.second >= min_support) {
            frequent_itemsets.insert(item.first);
        }
    }
    return frequent_itemsets;
}

// Function to perform the Transaction Reduction Apriori algorithm
void apriori(const vector<set<int>> &transactions, int min_support, ofstream &output_file) {
    set<set<int>> frequent_itemsets;
    map<int, int> item_support;

    // Count support for individual items (L1)
    for (const auto &transaction : transactions) {
        for (int item : transaction) {
            item_support[{item}]++;
        }
    }

    // Filter frequent itemsets of size 1
    set<set<int>> L1;
    for (const auto &item : item_support) {
        if (item.second >= min_support) {
            L1.insert({item.first});
        }
    }

    // Apriori iterations
    int k = 2;
    set<set<int>> prev_frequent_itemsets = L1;
    while (!prev_frequent_itemsets.empty()) {
        set<set<int>> candidates = generate_candidates(prev_frequent_itemsets, k);
        map<set<int>, int> support_count = count_support(candidates, transactions);
        set<set<int>> frequent_itemsets = filter_itemsets(support_count, min_support);

        if (!frequent_itemsets.empty()) {
            for (const auto &itemset : frequent_itemsets) {
                for (int item : itemset) {
                    output_file << item << " ";
                }
                output_file << "\n";
            }
        }

        prev_frequent_itemsets = frequent_itemsets;
        k++;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: ./transaction_reduction <min_support> <input_file> <output_file>\n";
        return 1;
    }

    int min_support = stoi(argv[1]);
    string input_file = argv[2];
    string output_file = argv[3];

    ifstream infile(input_file);
    ofstream outfile(output_file);

    if (!infile.is_open() || !outfile.is_open()) {
        cerr << "Error opening file.\n";
        return 1;
    }

    vector<set<int>> transactions;
    string line;
    
    // Read transactions from input file
    while (getline(infile, line)) {
        istringstream iss(line);
        set<int> transaction;
        int item;
        while (iss >> item) {
            transaction.insert(item);
        }
        transactions.push_back(transaction);
    }

    // Run the Apriori algorithm
    apriori(transactions, min_support, outfile);

    infile.close();
    outfile.close();

    return 0;
}

