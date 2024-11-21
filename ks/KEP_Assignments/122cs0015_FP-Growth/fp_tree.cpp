#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <memory>

using namespace std;

// Node structure for the FP-Tree
struct FPTreeNode {
    int item;
    int count;
    unordered_map<int, FPTreeNode*> children;
    FPTreeNode* parent;

    FPTreeNode(int item, FPTreeNode* parent = nullptr) : item(item), count(1), parent(parent) {}
};

// Class for FP-Tree Algorithm
class FPTree {
private:
    FPTreeNode* root;
    unordered_map<int, vector<FPTreeNode*>> headerTable;
    vector<vector<int>> transactions;
    int minSupportCount;

    // Parse input file
    void parseInputFile(const string& filename) {
        ifstream fin(filename);
        if (!fin) {
            cerr << "Input file could not be opened\n";
            exit(1);
        }

        string str;
        while (getline(fin, str)) {
            vector<int> transaction;
            stringstream ss(str);
            int num;
            while (ss >> num) {
                transaction.push_back(num);
            }
            transactions.push_back(transaction);
        }
        fin.close();

        // Debug: Print transactions
        cout << "Parsed Transactions:" << endl;
        for (const auto& transaction : transactions) {
            for (int item : transaction) {
                cout << item << " ";
            }
            cout << endl;
        }
    }

    // Insert a transaction into the FP-Tree
    void insertTransaction(const vector<int>& transaction) {
        FPTreeNode* currentNode = root;
        for (int item : transaction) {
            if (currentNode->children.find(item) == currentNode->children.end()) {
                FPTreeNode* newNode = new FPTreeNode(item, currentNode);
                currentNode->children[item] = newNode;
                headerTable[item].push_back(newNode);
            } else {
                currentNode->children[item]->count++;
            }
            currentNode = currentNode->children[item];
        }
    }

    // Build the FP-Tree
    void buildTree() {
        unordered_map<int, int> itemFrequency;
        // Count frequency of each item
        for (const auto& transaction : transactions) {
            for (int item : transaction) {
                itemFrequency[item]++;
            }
        }

        // Debug: Print item frequencies
        cout << "Item Frequencies:" << endl;
        for (const auto& pair : itemFrequency) {
            cout << "Item: " << pair.first << " Count: " << pair.second << endl;
        }

        // Remove items that don't meet the minimum support count
        vector<vector<int>> filteredTransactions;
        for (auto& transaction : transactions) {
            vector<int> filtered;
            for (int item : transaction) {
                if (itemFrequency[item] >= minSupportCount) {
                    filtered.push_back(item);
                }
            }
            if (!filtered.empty()) {
                sort(filtered.begin(), filtered.end(), [&](int a, int b) {
                    return itemFrequency[a] > itemFrequency[b]; // Sort by frequency
                });
                filteredTransactions.push_back(filtered);
            }
        }
        transactions = move(filteredTransactions);

        // Insert filtered transactions into the FP-Tree
        for (const auto& transaction : transactions) {
            insertTransaction(transaction);
        }

        // Debug: Print tree structure
        cout << "FP-Tree Built" << endl;
    }

    // Mine the FP-Tree recursively
    void mineTree(FPTreeNode* subtreeRoot, vector<int> prefix, vector<vector<int>>& frequentItemsets) {
        vector<int> items;
        for (const auto& entry : headerTable) {
            items.push_back(entry.first);
        }
        sort(items.begin(), items.end(), greater<int>());

        for (int item : items) {
            vector<int> newPrefix = prefix;
            newPrefix.push_back(item);
            frequentItemsets.push_back(newPrefix);

            vector<vector<int>> conditionalPatternBase;
            for (FPTreeNode* node : headerTable[item]) {
                vector<int> path;
                FPTreeNode* current = node->parent;
                while (current != root) {
                    path.push_back(current->item);
                    current = current->parent;
                }
                reverse(path.begin(), path.end());
                for (int i = 0; i < node->count; ++i) {
                    conditionalPatternBase.push_back(path);
                }
            }

            // Debug: Print conditional pattern base
            cout << "Conditional Pattern Base for item " << item << ":" << endl;
            for (const auto& path : conditionalPatternBase) {
                for (int pItem : path) {
                    cout << pItem << " ";
                }
                cout << endl;
            }

            if (!conditionalPatternBase.empty()) {
                FPTree subtree(conditionalPatternBase, minSupportCount);
                subtree.mineTree(subtree.root, newPrefix, frequentItemsets);
            }
        }
    }

    // Build a conditional FP-Tree for mining
    void buildConditionalTree(const vector<vector<int>>& conditionalPatternBase, int minSupportCount) {
        root = new FPTreeNode(-1);
        headerTable.clear();
        transactions = conditionalPatternBase;
        this->minSupportCount = minSupportCount;
        buildTree();
    }

    // Deallocate memory used by the FP-Tree
    void deallocateTree(FPTreeNode* node) {
        for (auto& child : node->children) {
            deallocateTree(child.second);
        }
        delete node;
    }

public:
    // Constructor for main FP-Tree
    FPTree(const string& filename, int minSupportCount) : minSupportCount(minSupportCount) {
        root = new FPTreeNode(-1);
        parseInputFile(filename);
        buildTree();
    }

    // Constructor for conditional FP-Tree
    FPTree(const vector<vector<int>>& conditionalPatternBase, int minSupportCount) : minSupportCount(minSupportCount) {
        root = new FPTreeNode(-1);
        buildConditionalTree(conditionalPatternBase, minSupportCount);
    }

    ~FPTree() {
        deallocateTree(root);
    }

    // Get all frequent itemsets using the FP-Growth algorithm
    vector<vector<int>> getFrequentItemsets() {
        vector<vector<int>> frequentItemsets;
        mineTree(root, {}, frequentItemsets);
        return frequentItemsets;
    }
};

int main() {
    string inputFileName = "fp_tree_input.txt";
    string outputFileName = "fp_tree_output.txt";
    int minSupportCount = 2;  // Minimum support count

    FPTree fpTree(inputFileName, minSupportCount);
    vector<vector<int>> frequentItemsets = fpTree.getFrequentItemsets();

    // Debug: Print found frequent itemsets
    cout << "Frequent Itemsets Found:" << endl;
    for (const auto& itemset : frequentItemsets) {
        for (int item : itemset) {
            cout << item << " ";
        }
        cout << endl;
    }

    ofstream out_file(outputFileName);
    if (!out_file) {
        cerr << "Output file could not be opened\n";
        return 1;
    }

    out_file << "Frequent itemsets:\n";
    for (const auto& itemset : frequentItemsets) {
        out_file << "{ ";
        for (const auto& item : itemset) {
            out_file << item << ' ';
        }
        out_file << "}\n";
    }

    out_file.close();
    return 0;
}
