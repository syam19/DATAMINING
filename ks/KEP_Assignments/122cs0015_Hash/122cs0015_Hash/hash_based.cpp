#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <sstream>

using namespace std;

class InputReader {
private:
    ifstream fin;
    vector<vector<int>> transactions;
public:
    InputReader(string filename) {
        fin.open(filename);
        if (!fin) {
            cerr << "Input file could not be opened\n";
            exit(0);
        }
        parse();
    }

    void parse() {
        string str;
        while (getline(fin, str)) {
            vector<int> arr;
            stringstream ss(str);
            int num;
            while (ss >> num) {
                arr.push_back(num);
            }
            transactions.push_back(arr);
        }
    }

    vector<vector<int>> getTransactions() {
        return transactions;
    }
};

class DirectHashingAndPruning {
private:
    vector<vector<int>> transactions;
    int minSupportCount;
    unordered_map<string, int> itemsetCounts;

    // Hash function to convert a vector to a string
    string vectorToString(const vector<int>& vec) {
        stringstream ss;
        for (int num : vec) {
            ss << num << ",";
        }
        string str = ss.str();
        if (!str.empty()) str.pop_back(); // Remove trailing comma
        return str;
    }

    // Generate candidate itemsets of length k
    vector<vector<int>> generateCandidates(const vector<vector<int>>& itemsets, int k) {
        vector<vector<int>> candidates;
        for (size_t i = 0; i < itemsets.size(); ++i) {
            for (size_t j = i + 1; j < itemsets.size(); ++j) {
                vector<int> candidate(itemsets[i]);
                candidate.insert(candidate.end(), itemsets[j].begin(), itemsets[j].end());
                sort(candidate.begin(), candidate.end());
                candidate.erase(unique(candidate.begin(), candidate.end()), candidate.end());

                if (candidate.size() == k) {
                    candidates.push_back(candidate);
                }
            }
        }
        return candidates;
    }

    // Count itemsets in transactions
    void countItemsets(const vector<vector<int>>& candidates) {
        unordered_map<string, int> localItemsetCounts;
        for (const auto& transaction : transactions) {
            for (const auto& candidate : candidates) {
                if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                    localItemsetCounts[vectorToString(candidate)]++;
                }
            }
        }

        for (const auto& pair : localItemsetCounts) {
            itemsetCounts[pair.first] += pair.second;
        }
    }

    // Generate frequent itemsets from counts
    vector<vector<int>> generateFrequentItemsets() {
        vector<vector<int>> frequentItemsets;
        for (const auto& pair : itemsetCounts) {
            if (pair.second >= minSupportCount) {
                vector<int> itemset;
                stringstream ss(pair.first);
                string item;
                while (getline(ss, item, ',')) {
                    itemset.push_back(stoi(item));
                }
                frequentItemsets.push_back(itemset);
            }
        }
        return frequentItemsets;
    }

public:
    DirectHashingAndPruning(vector<vector<int>> _transactions, int _minSupportCount)
        : transactions(_transactions), minSupportCount(_minSupportCount) {}

    void process() {
        vector<vector<int>> itemsets;

        // Initialize with single-item itemsets
        unordered_map<string, int> singleItemsetCounts;
        for (const auto& transaction : transactions) {
            for (int item : transaction) {
                vector<int> itemset = { item };
                string key = vectorToString(itemset);
                singleItemsetCounts[key]++;
            }
        }

        // Filter single itemsets by minSupportCount
        for (const auto& pair : singleItemsetCounts) {
            if (pair.second >= minSupportCount) {
                vector<int> itemset = { stoi(pair.first) };
                itemsets.push_back(itemset);
                itemsetCounts[pair.first] = pair.second;
            }
        }

        // Generate larger itemsets
        int k = 2;
        while (true) {
            vector<vector<int>> candidates = generateCandidates(itemsets, k);
            if (candidates.empty()) break;

            countItemsets(candidates);
            vector<vector<int>> newFrequentItemsets = generateFrequentItemsets();

            if (newFrequentItemsets.empty()) break;
            itemsets = newFrequentItemsets;
            k++;
        }
    }

    vector<vector<int>> getAllFrequentItemsets() {
        return generateFrequentItemsets();
    }
};

int main() {
    string inputFileName = "hash_input.txt";
    string outputFileName = "hash_output.txt";
    int minSupportCount = 3;  // Minimum support count

    InputReader inputReader(inputFileName);
    vector<vector<int>> transactions = inputReader.getTransactions();

    DirectHashingAndPruning dhp(transactions, minSupportCount);
    dhp.process();

    ofstream out_file(outputFileName);
    if (!out_file) {
        cerr << "Output file could not be opened\n";
        return 1;
    }

    out_file << "Frequent itemsets:\n";
    for (const auto& itemset : dhp.getAllFrequentItemsets()) {
        out_file << "{ ";
        for (const auto& item : itemset) {
            out_file << item << ' ';
        }
        out_file << "}\n";
    }

    out_file.close();

    return 0;
}
