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

// Function to preprocess data and build P-trees
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

// Generate initial 1-itemsets from P-trees
vector<set<int>> GenerateInitialItemsets(const vector<PTree*>& pTrees) {
    vector<set<int>> itemsets;
    for (int i = 0; i < pTrees.size(); ++i) {
        if (pTrees[i]->rootCount > 0) {
            itemsets.push_back({i});
        }
    }
    return itemsets;
}

// Calculate support of an itemset using P-trees
int CalculateSupport(const set<int>& itemset, const vector<PTree*>& pTrees) {
    PTree* currentPTree = pTrees[*itemset.begin()];
    for (auto it = next(itemset.begin()); it != itemset.end(); ++it) {
        currentPTree = currentPTree->AND(pTrees[*it]);
    }
    return currentPTree ? currentPTree->rootCount : 0;
}

// Generate next level itemsets (Apriori principle)
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

// Generate frequent itemsets based on the minimum support
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

        // Generate new candidate itemsets based on the new frequent itemsets
        candidateItemsets = GenerateNextLevelItemsets(newFrequentItemsets);

        // Break the loop if no new frequent itemsets were found
        if (newFrequentItemsets.empty()) break;
    }

    return frequentItemsets;
}

// Calculate confidence for a rule: "If subset then remaining"
double CalculateConfidence(const set<int>& subset, const set<int>& remaining, const map<set<int>, int>& supportMap) {
    int supportSubset = supportMap.at(subset);
    int supportBoth = supportMap.at(subset); // This should actually refer to the support of subset and remaining combined
    return (supportSubset > 0) ? (double)supportBoth / supportSubset : 0.0;
}

// Generate association rules from frequent itemsets
vector<pair<set<int>, set<int>>> GenerateAssociationRules(const vector<set<int>>& frequentItemsets, double minConfidence, const map<set<int>, int>& supportMap) {
    vector<pair<set<int>, set<int>>> associationRules;

    for (const auto& itemset : frequentItemsets) {
        // Generate rules by splitting the itemset into a condition and a result
        for (auto it = itemset.begin(); it != itemset.end(); ++it) {
            set<int> subset = { *it };  // Each item as a subset
            set<int> remaining(itemset.begin(), itemset.end());
            remaining.erase(*it);  // Remaining items
            
            double confidence = CalculateConfidence(subset, remaining, supportMap);
            if (confidence >= minConfidence) {
                associationRules.push_back({subset, remaining});  // Add rule if confidence is high enough
            }
        }
    }

    return associationRules;
}

// Main PARM algorithm function
vector<pair<set<int>, set<int>>> PARMAlgorithm(const vector<vector<int>>& data, int minSupport, double minConfidence) {
    // Step 1: Preprocess the data to build P-trees
    vector<PTree*> pTrees = PreprocessData(data);

    // Step 2: Generate frequent itemsets
    vector<set<int>> frequentItemsets = GenerateFrequentItemsets(pTrees, minSupport);

    // Step 3: Map itemsets to their support values
    map<set<int>, int> supportMap;
    for (const auto& itemset : frequentItemsets) {
        supportMap[itemset] = CalculateSupport(itemset, pTrees);
    }

    // Step 4: Generate association rules based on minimum confidence
    vector<pair<set<int>, set<int>>> associationRules = GenerateAssociationRules(frequentItemsets, minConfidence, supportMap);

    return associationRules;
}

int main() {
    // Example spatial data (pixel values), simplified as integers
    vector<vector<int>> data = {
        {210, 150, 200, 130},  // Red values for 4 pixels
        {120, 100, 80, 140},   // Green values for 4 pixels
        {50, 80, 90, 60},      // Blue values for 4 pixels
        {200, 170, 220, 180}   // Yield values for 4 pixels
    };

    int minSupport = 4;  // Example minimum support threshold
    double minConfidence = 0.7;  // Example minimum confidence threshold

    // Run PARM algorithm
    vector<pair<set<int>, set<int>>> rules = PARMAlgorithm(data, minSupport, minConfidence);

    // Output the generated association rules
    cout << "Generated Association Rules:" << endl;
    for (const auto& rule : rules) {
        cout << "If ";
        for (int item : rule.first) {
            cout << "Item " << item << " ";
        }
        cout << "then ";
        for (int item : rule.second) {
            cout << "Item " << item << " ";
        }
        cout << endl;
    }

    return 0;
}
