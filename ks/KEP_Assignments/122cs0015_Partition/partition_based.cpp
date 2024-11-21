#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <tuple>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <random>
using namespace std;

// Helper functions
long double round(long double value, int pos) {
    long double temp = value * pow(10, pos);
    temp = floor(temp + 0.5);
    temp *= pow(10, -pos);
    return temp;
}

// K-Means Clustering
class KMeans {
private:
    vector<vector<int>> data;
    int k;
    vector<vector<int>> centroids;
    vector<int> assignments;
    
    vector<vector<int>> initializeCentroids() {
        vector<vector<int>> centroids(k, vector<int>(data[0].size()));
        set<int> chosenIndices;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, data.size() - 1);
        
        int index;
        for (int i = 0; i < k; ++i) {
            do {
                index = dis(gen);
            } while (chosenIndices.find(index) != chosenIndices.end());
            chosenIndices.insert(index);
            centroids[i] = data[index];
        }
        return centroids;
    }
    
    void assignClusters() {
        for (int i = 0; i < data.size(); ++i) {
            int bestCluster = 0;
            double bestDist = numeric_limits<double>::max();
            for (int j = 0; j < k; ++j) {
                double dist = 0;
                for (int l = 0; l < data[i].size(); ++l) {
                    dist += pow(data[i][l] - centroids[j][l], 2);
                }
                if (dist < bestDist) {
                    bestDist = dist;
                    bestCluster = j;
                }
            }
            assignments[i] = bestCluster;
        }
    }
    
    void updateCentroids() {
        vector<vector<int>> newCentroids(k, vector<int>(data[0].size(), 0));
        vector<int> counts(k, 0);
        
        for (int i = 0; i < data.size(); ++i) {
            int cluster = assignments[i];
            for (int j = 0; j < data[i].size(); ++j) {
                newCentroids[cluster][j] += data[i][j];
            }
            counts[cluster]++;
        }
        
        for (int i = 0; i < k; ++i) {
            if (counts[i] > 0) {
                for (int j = 0; j < centroids[i].size(); ++j) {
                    newCentroids[i][j] /= counts[i];
                }
            }
        }
        
        centroids = newCentroids;
    }
    
public:
    KMeans(const vector<vector<int>>& data, int k) : data(data), k(k), assignments(data.size()) {
        centroids = initializeCentroids();
    }
    
    void run(int maxIterations = 100) {
        for (int i = 0; i < maxIterations; ++i) {
            assignClusters();
            updateCentroids();
        }
    }
    
    void printCentroids(ofstream& out_file) const {
        out_file << "Final centroids:\n";
        for (const auto& centroid : centroids) {
            for (const auto& value : centroid) {
                out_file << value << ' ';
            }
            out_file << '\n';
        }
    }
};

// Partition-Based Frequent Itemset Mining
class PartitionBasedMining {
private:
    vector<vector<int>> transactions;
    long double minSupport;
    int numPartitions;
    vector<vector<int>> allFrequentItemsets;
    
    vector<vector<int>> findFrequentItemsets(const vector<vector<int>>& partition) {
        vector<vector<int>> frequentItemsets;
        set<vector<int>> itemsetSet;
        
        // Generate all itemsets (single items, pairs, etc.)
        set<int> items;
        for (const auto& trans : partition) {
            for (int item : trans) {
                items.insert(item);
            }
        }
        
        vector<int> itemVec(items.begin(), items.end());
        
        // Check single items
        for (int item : itemVec) {
            vector<int> itemset = {item};
            long double support = getSupport(partition, itemset);
            if (support >= minSupport) {
                frequentItemsets.push_back(itemset);
                itemsetSet.insert(itemset);
            }
        }
        
        // Check pairs, triplets, etc.
        for (size_t r = 2; r <= itemVec.size(); ++r) {
            vector<vector<int>> combinations;
            generateCombinations(itemVec, r, combinations);
            
            for (const auto& combo : combinations) {
                long double support = getSupport(partition, combo);
                if (support >= minSupport) {
                    frequentItemsets.push_back(combo);
                    itemsetSet.insert(combo);
                }
            }
        }
        
        return frequentItemsets;
    }
    
    long double getSupport(const vector<vector<int>>& partition, const vector<int>& itemset) {
        int count = 0;
        for (const auto& trans : partition) {
            if (includes(trans.begin(), trans.end(), itemset.begin(), itemset.end())) {
                ++count;
            }
        }
        return static_cast<long double>(count) / partition.size() * 100.0;
    }
    
    void generateCombinations(const vector<int>& items, size_t r, vector<vector<int>>& combinations) {
        vector<int> indices(r);
        iota(indices.begin(), indices.end(), 0);
        
        while (true) {
            vector<int> combo;
            for (size_t i = 0; i < r; ++i) {
                combo.push_back(items[indices[i]]);
            }
            combinations.push_back(combo);
            
            size_t i = r;
            while (i-- > 0) {
                if (indices[i] != i + items.size() - r) {
                    break;
                }
            }
            
            if (i == static_cast<size_t>(-1)) break;
            
            ++indices[i];
            for (size_t j = i + 1; j < r; ++j) {
                indices[j] = indices[j - 1] + 1;
            }
        }
    }
    
public:
    PartitionBasedMining(const vector<vector<int>>& transactions, long double minSupport, int numPartitions)
        : transactions(transactions), minSupport(minSupport), numPartitions(numPartitions) {}
    
    void process() {
        int partitionSize = transactions.size() / numPartitions;
        vector<vector<int>> partitionFrequentItemsets;
        
        for (int i = 0; i < numPartitions; ++i) {
            vector<vector<int>> partition;
            if (i == numPartitions - 1) {
                partition.assign(transactions.begin() + i * partitionSize, transactions.end());
            } else {
                partition.assign(transactions.begin() + i * partitionSize, transactions.begin() + (i + 1) * partitionSize);
            }
            
            vector<vector<int>> frequentItemsets = findFrequentItemsets(partition);
            partitionFrequentItemsets.insert(partitionFrequentItemsets.end(), frequentItemsets.begin(), frequentItemsets.end());
        }
        
        // Merge frequent itemsets from all partitions
        set<vector<int>> mergedItemsets(partitionFrequentItemsets.begin(), partitionFrequentItemsets.end());
        allFrequentItemsets.assign(mergedItemsets.begin(), mergedItemsets.end());
    }
    
    vector<vector<int>> getAllFrequentItemsets() const {
        return allFrequentItemsets;
    }
};

// Input Reader
class InputReader {
private:
    ifstream fin;
    vector<vector<int>> transactions;
    
public:
    InputReader(const string& filename) : fin(filename) {
        if (!fin) {
            cerr << "Input file could not be opened\n";
            exit(0);
        }
        parse();
    }
    
    void parse() {
        string line;
        while (getline(fin, line)) {
            vector<int> arr;
            size_t start = 0;
            size_t end = line.find(' ');
            while (end != string::npos) {
                arr.push_back(stoi(line.substr(start, end - start)));
                start = end + 1;
                end = line.find(' ', start);
            }
            arr.push_back(stoi(line.substr(start, end)));
            transactions.push_back(arr);
        }
    }
    
    vector<vector<int>> getTransactions() const {
        return transactions;
    }
};

// Main Function
int main() {
    // Load transactions
    InputReader inputReader("partition_input.txt");
    vector<vector<int>> transactions = inputReader.getTransactions();
    
    // Open output file
    ofstream out_file("partition_output.txt");
    if (!out_file) {
        cerr << "Output file could not be opened\n";
        return 1;
    }
    
    // Perform k-means clustering
    KMeans kmeans(transactions, 3); // Example k = 3
    kmeans.run();
    kmeans.printCentroids(out_file);
    
    // Perform partition-based frequent itemset mining
    PartitionBasedMining pbMining(transactions, 20.0, 4); // Example minSupport = 20%, 4 partitions
    pbMining.process();
    
    out_file << "Frequent itemsets:\n";
    for (const auto& itemset : pbMining.getAllFrequentItemsets()) {
        out_file << "{ ";
        for (const auto& item : itemset) {
            out_file << item << ' ';
        }
        out_file << "}\n";
    }
    
    out_file.close();
    
    return 0;
}
