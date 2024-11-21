#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>  // For file operations

using namespace std;

typedef set<int> Itemset;
typedef vector<Itemset> ItemsetList;
typedef map<Itemset, int> ItemsetCountMap;

struct Transaction {
    int custID;
    int transID;
    Itemset items;
};

// Function to generate random transactions
vector<Transaction> generateRandomTransactions(int numTransactions, int maxItems, int maxItemID) {
    vector<Transaction> transactions;
    srand(time(0));  // Initialize random seed

    for (int i = 0; i < numTransactions; ++i) {
        Transaction transaction;
        transaction.custID = rand() % 1000 + 1;  // Random customer ID between 1 and 1000
        transaction.transID = i + 1;  // Transaction ID

        int numItems = rand() % maxItems + 1;  // Random number of items (between 1 and maxItems)
        for (int j = 0; j < numItems; ++j) {
            int item = rand() % maxItemID + 1;  // Random item ID between 1 and maxItemID
            transaction.items.insert(item);  // Insert random item into the transaction
        }

        transactions.push_back(transaction);
    }

    return transactions;
}

// Save transactions to a file
void saveTransactionsToFile(const vector<Transaction>& transactions, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const Transaction& transaction : transactions) {
            file << "Customer ID: " << transaction.custID << ", Transaction ID: " << transaction.transID << ", Items: {";
            for (int item : transaction.items) {
                file << item << " ";
            }
            file << "}" << endl;
        }
        file.close();
        cout << "Transactions saved to " << filename << endl;
    } else {
        cout << "Unable to open file to save transactions!" << endl;
    }
}

// Generate candidate itemsets from the previous level's frequent itemsets
ItemsetList generateCandidates(const ItemsetList& prevFrequentItemsets) {
    ItemsetList candidates;
    int size = prevFrequentItemsets.size();

    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            Itemset candidate = prevFrequentItemsets[i];
            candidate.insert(prevFrequentItemsets[j].begin(), prevFrequentItemsets[j].end());

            if (candidate.size() == prevFrequentItemsets[0].size() + 1) {
                candidates.push_back(candidate);
            }
        }
    }

    return candidates;
}

// Prune candidates by removing those with infrequent subsets
void pruneCandidates(ItemsetList& candidates, const ItemsetCountMap& frequentItemsets) {
    candidates.erase(
        remove_if(candidates.begin(), candidates.end(), [&](const Itemset& candidate) {
            for (int item : candidate) {
                Itemset subset = candidate;
                subset.erase(item);

                if (frequentItemsets.find(subset) == frequentItemsets.end()) {
                    return true;
                }
            }
            return false;
        }),
        candidates.end()
    );
}

// Count support for each candidate itemset
ItemsetCountMap countItemsets(const ItemsetList& candidates, const vector<Transaction>& transactions) {
    ItemsetCountMap itemsetCountMap;

    for (const Transaction& transaction : transactions) {
        for (const Itemset& candidate : candidates) {
            if (includes(transaction.items.begin(), transaction.items.end(), candidate.begin(), candidate.end())) {
                itemsetCountMap[candidate]++;
            }
        }
    }

    return itemsetCountMap;
}

// Filter itemsets by minimum support
ItemsetList filterFrequentItemsets(const ItemsetCountMap& itemsetCountMap, int minSupport) {
    ItemsetList frequentItemsets;

    for (const auto& pair : itemsetCountMap) {
        if (pair.second >= minSupport) {
            frequentItemsets.push_back(pair.first);
        }
    }

    return frequentItemsets;
}

// Generate association rules from frequent itemsets
void generateRules(const ItemsetList& frequentItemsets, const ItemsetCountMap& itemsetCountMap, int totalTransactions, double minConf) {
    for (const Itemset& itemset : frequentItemsets) {
        for (int item : itemset) {
            Itemset antecedent = itemset;
            antecedent.erase(item);

            if (!antecedent.empty()) {
                int antecedentSupport = itemsetCountMap.at(antecedent);
                int itemsetSupport = itemsetCountMap.at(itemset);

                double confidence = static_cast<double>(itemsetSupport) / antecedentSupport;

                if (confidence >= minConf) {
                    cout << "{ ";
                    for (int antecItem : antecedent) {
                        cout << antecItem << " ";
                    }
                    cout << "} => { " << item << " } (Conf: " << confidence << ")" << endl;
                }
            }
        }
    }
}

// Reduce transactions by removing items that are not in any frequent itemset
vector<Transaction> reduceTransactions(const vector<Transaction>& transactions, const ItemsetList& frequentItemsets) {
    vector<Transaction> reducedTransactions;

    for (const Transaction& transaction : transactions) {
        Transaction reducedTransaction = transaction;
        Itemset reducedItems;

        for (const Itemset& itemset : frequentItemsets) {
            if (includes(transaction.items.begin(), transaction.items.end(), itemset.begin(), itemset.end())) {
                reducedItems.insert(itemset.begin(), itemset.end());
            }
        }

        if (!reducedItems.empty()) {
            reducedTransaction.items = reducedItems;
            reducedTransactions.push_back(reducedTransaction);
        }
    }

    return reducedTransactions;
}

int main() {
    int numTransactions, maxItems, maxItemID;
    double minSupPercentage, minConf;
    string filename;

    // Take input one by one from the user
    cout << "Enter the number of transactions: ";
    cin >> numTransactions;
    cout << "Enter the maximum number of items per transaction: ";
    cin >> maxItems;
    cout << "Enter the maximum item ID: ";
    cin >> maxItemID;
    cout << "Enter the minimum support percentage: ";
    cin >> minSupPercentage;
    cout << "Enter the minimum confidence: ";
    cin >> minConf;
    cout << "Enter the filename to save transactions: ";
    cin >> filename;

    // Generate random transactions
    vector<Transaction> transactions = generateRandomTransactions(numTransactions, maxItems, maxItemID);
    int totalTransactions = transactions.size();
    int minSupport = static_cast<int>(minSupPercentage * totalTransactions);

    // Save transactions to file
    saveTransactionsToFile(transactions, filename);

    // Start measuring time
    clock_t startTime = clock();

    ItemsetList frequentItemsets;
    ItemsetCountMap itemsetCountMap;

    // Generate 1-itemset candidates
    ItemsetList candidates;
    for (const Transaction& transaction : transactions) {
        for (int item : transaction.items) {
            Itemset singleItemSet = { item };
            candidates.push_back(singleItemSet);
        }
    }

    // Remove duplicates from 1-itemset candidates
    sort(candidates.begin(), candidates.end());
    candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end());

    int level = 1;
    while (!candidates.empty()) {
        // Count support for each candidate
        ItemsetCountMap candidateCountMap = countItemsets(candidates, transactions);

        // Filter candidates by minimum support
        frequentItemsets = filterFrequentItemsets(candidateCountMap, minSupport);

        // Store the frequent itemsets with their counts
        itemsetCountMap.insert(candidateCountMap.begin(), candidateCountMap.end());

        cout << "Level " << level << " - Candidates: " << candidates.size() << ", Frequent Itemsets: " << frequentItemsets.size() << endl;

        // Reduce transactions after first iteration
        if (level == 1) {
            transactions = reduceTransactions(transactions, frequentItemsets);
        }

        // Generate next level candidates
        candidates = generateCandidates(frequentItemsets);

        // Prune candidates that have infrequent subsets
        pruneCandidates(candidates, itemsetCountMap);

        level++;
    }

    // Generate association rules
    generateRules(frequentItemsets, itemsetCountMap, totalTransactions, minConf);

    // Stop measuring time and calculate the elapsed time
    clock_t endTime = clock();
    double timeTaken = double(endTime - startTime) / CLOCKS_PER_SEC;
    cout << "Execution Time: " << timeTaken << " seconds" << endl;

    return 0;
}
