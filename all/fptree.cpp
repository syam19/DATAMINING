#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

class FPTreeNode {
public:
    int item;  // Item value
    int count; // Count of the item
    FPTreeNode* parent; // Pointer to parent node
    unordered_map<int, FPTreeNode*> children; // Children nodes

    FPTreeNode(int item, FPTreeNode* parent = nullptr) 
        : item(item), count(0), parent(parent) {}
};

class FPTree {
private:
    FPTreeNode* root; // Root of the FP-tree
    unordered_map<int, int> frequency; // Item frequency
    unordered_map<int, FPTreeNode*> headerTable; // Header table for linking nodes

    void minePatterns(FPTreeNode* node, vector<int>& prefix, vector<vector<int>>& patterns, int minSupport) {
        if (!node) return; // Safety check

        for (auto& pair : node->children) {
            int item = pair.first;
            FPTreeNode* child = pair.second;

            // Extend the prefix path
            vector<int> newPrefix = prefix;
            newPrefix.push_back(item);

            // Record the pattern if its count meets the minimum support
            if (child->count >= minSupport) {
                patterns.push_back(newPrefix);
            }

            // Recursively mine from the child
            minePatterns(child, newPrefix, patterns, minSupport);
        }
    }

public:
    FPTree() : root(new FPTreeNode(-1)) {}

    void insert(const vector<int>& transaction) {
        FPTreeNode* currentNode = root;

        for (int item : transaction) {
            frequency[item]++; // Update frequency count
            if (currentNode->children.find(item) == currentNode->children.end()) {
                currentNode->children[item] = new FPTreeNode(item, currentNode);
            }
            currentNode = currentNode->children[item];
            currentNode->count++;
        }
    }

    void buildHeaderTable(int minSupport) {
        for (auto& pair : frequency) {
            if (pair.second >= minSupport) {
                headerTable[pair.first] = nullptr; // Initialize header table
            }
        }
    }

    vector<vector<int>> mine(int minSupport) {
        vector<vector<int>> patterns;
        vector<int> prefix;

        // Sort header table items by their frequency
        vector<pair<int, int>> sortedItems(frequency.begin(), frequency.end());
        sort(sortedItems.begin(), sortedItems.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
            return a.second > b.second; // Sort by frequency
        });

        for (auto& pair : sortedItems) {
            int item = pair.first;
            if (headerTable.find(item) != headerTable.end()) {
                cout << "Mining patterns for item: " << item << endl; // Debug output
                // Start mining from this item
                if (root->children.find(item) != root->children.end()) {
                    minePatterns(root->children[item], prefix, patterns, minSupport);
                } else {
                    cout << "Item not found in children: " << item << endl; // Debug output
                }
            }
        }

        return patterns;
    }
};

// Function to perform FP-Growth
vector<vector<int>> fpgrowth(const vector<vector<int>>& transactions, int minSupport) {
    FPTree tree;

    // Insert transactions into the FP-tree
    for (const auto& transaction : transactions) {
        tree.insert(transaction);
    }

    // Build the header table
    tree.buildHeaderTable(minSupport);

    // Mine frequent patterns
    return tree.mine(minSupport);
}

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

    // Execute FP-Growth
    vector<vector<int>> frequentPatterns = fpgrowth(transactions, minSupport);

    // Output frequent patterns
    cout << "Frequent Patterns:\n";
    for (const auto& pattern : frequentPatterns) {
        cout << "{ ";
        for (int item : pattern) {
            cout << item << " ";
        }
        cout << "}\n";
    }

    return 0;
}
