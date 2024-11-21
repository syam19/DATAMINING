#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

// Definition of a P-tree node
struct PTree {
    int rootCount; 
    vector<PTree*> children;

    PTree(int count) : rootCount(count) {}

    PTree* AND(PTree* other) {
        if (!other) return nullptr;
        PTree* result = new PTree(min(this->rootCount, other->rootCount));
        for (size_t i = 0; i < this->children.size(); i++) {
            if (this->children[i] && other->children[i]) {
                result->children.push_back(this->children[i]->AND(other->children[i]));
            } else {
                result->children.push_back(nullptr);
            }
        }
        return result;
    }
};

vector<PTree*> PreprocessData(const vector<vector<int>>& data) {
    vector<PTree*> pTrees;

    for (const auto& band : data) {
        for (int bit = 0; bit < 8; ++bit) {
            int count = 0;
            for (int value : band) {
                if ((value >> bit) & 1) count++;
            }
            PTree* pTree = new PTree(count);
            pTrees.push_back(pTree);
            cout << "PTree created for bit " << bit << " with rootCount " << count << endl; // Debug output
        }
    }

    return pTrees;
}

vector<set<int>> GenerateInitialItemsets(const vector<PTree*>& pTrees) {
    vector<set<int>> itemsets;
    for (int i = 0; i < pTrees.size(); ++i) {
        if (pTrees[i]->rootCount > 0) {
            itemsets.push_back({i});
        }
    }
    return itemsets;
}

int CalculateSupport(const set<int>& itemset, const vector<PTree*>& pTrees) {
    PTree* currentPTree = pTrees[*itemset.begin()];
    for (auto it = next(itemset.begin()); it != itemset.end(); ++it) {
        currentPTree = currentPTree->AND(pTrees[*it]);
    }
    return currentPTree ? currentPTree->rootCount : 0;
}

vector<set<int>> GenerateNextLevelItemsets(const vector<set<int>>& frequentItemsets) {
    vector<set<int>> candidateItemsets;
    for (size_t i = 0; i < frequentItemsets.size(); ++i) {
        for (size_t j = i + 1; j < frequentItemsets.size(); ++j) {
            set<int> candidate = frequentItemsets[i];
            candidate.insert(frequentItemsets[j].begin(), frequentItemsets[j].end());
            if (candidate.size() == frequentItemsets[i].size() + 1) {
                candidateItemsets.push_back(candidate);
            }
        }
    }
    return candidateItemsets;
}

vector<set<int>> GenerateFrequentItemsets(const vector<PTree*>& pTrees, int minSupport) {
    vector<set<int>> frequentItemsets;
    vector<set<int>> candidateItemsets = GenerateInitialItemsets(pTrees);

    while (!candidateItemsets.empty()) {
        vector<set<int>> newFrequentItemsets;
        for (const auto& itemset : candidateItemsets) {
            int support = CalculateSupport(itemset, pTrees);
            cout << "Calculating support for itemset: { ";
            for (int item : itemset) {
                cout << item << " ";
            }
            cout << "} -> Support: " << support << endl; // Debug output
            if (support >= minSupport) {
                frequentItemsets.push_back(itemset);
                newFrequentItemsets.push_back(itemset);
            }
        }
        candidateItemsets = GenerateNextLevelItemsets(newFrequentItemsets);
    }

    return frequentItemsets;
}

vector<pair<set<int>, set<int>>> PARMAlgorithm(const vector<vector<int>>& data, int minSupport, double minConfidence) {
    vector<PTree*> pTrees = PreprocessData(data);
    vector<set<int>> frequentItemsets = GenerateFrequentItemsets(pTrees, minSupport);

    cout << "Frequent itemsets found: " << frequentItemsets.size() << endl; // Debug output
    for (const auto& itemset : frequentItemsets) {
        cout << "Frequent itemset: { ";
        for (int item : itemset) {
            cout << item << " ";
        }
        cout << "}" << endl; // Debug output
    }

    return {};
}

int main() {
    vector<vector<int>> data = {
        {210, 150, 200, 130},
        {120, 100, 80, 140},
        {50, 80, 90, 60},
        {200, 170, 220, 180}
    };

    int minSupport = 2;
    double minConfidence = 0.7;

    PARMAlgorithm(data, minSupport, minConfidence);

    return 0;
}