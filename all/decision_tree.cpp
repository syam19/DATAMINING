#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;

struct Node {
    int featureIndex; // Index of the feature to split on
    double threshold;  // Threshold value for the split
    int label;        // Class label (if it's a leaf node)
    Node* left;      // Left child
    Node* right;     // Right child

    Node() : featureIndex(-1), threshold(0), label(-1), left(nullptr), right(nullptr) {}
};

class DecisionTree {
public:
    DecisionTree(int minSamplesSplit) : minSamplesSplit(minSamplesSplit) {}

    void fit(const vector<vector<double>>& X, const vector<int>& y) {
        root = buildTree(X, y);
    }

    int predict(const vector<double>& sample) {
        return predict(sample, root);
    }

private:
    Node* root;
    int minSamplesSplit;

    Node* buildTree(const vector<vector<double>>& X, const vector<int>& y) {
        if (y.empty()) return nullptr;

        // If all labels are the same, return a leaf node
        if (all_of(y.begin(), y.end(), [&](int label) { return label == y[0]; })) {
            Node* leaf = new Node();
            leaf->label = y[0];
            return leaf;
        }

        // If we can't split further, return a leaf node with the majority label
        if (X.size() < minSamplesSplit) {
            Node* leaf = new Node();
            leaf->label = majorityClass(y);
            return leaf;
        }

        // Find the best split
        int bestFeature;
        double bestThreshold;
        double bestGain = -1;
        vector<int> bestLeftLabels, bestRightLabels;

        for (size_t featureIndex = 0; featureIndex < X[0].size(); ++featureIndex) {
            vector<double> thresholds = getUniqueThresholds(X, featureIndex);
            for (double threshold : thresholds) {
                vector<int> leftLabels, rightLabels;
                for (size_t i = 0; i < X.size(); ++i) {
                    if (X[i][featureIndex] <= threshold) {
                        leftLabels.push_back(y[i]);
                    } else {
                        rightLabels.push_back(y[i]);
                    }
                }
                double gain = informationGain(y, leftLabels, rightLabels);
                if (gain > bestGain) {
                    bestGain = gain;
                    bestFeature = featureIndex;
                    bestThreshold = threshold;
                    bestLeftLabels = leftLabels;
                    bestRightLabels = rightLabels;
                }
            }
        }

        // Create a node and recursively build the tree
        Node* node = new Node();
        node->featureIndex = bestFeature;
        node->threshold = bestThreshold;

        vector<vector<double>> leftX, rightX;
        for (size_t i = 0; i < X.size(); ++i) {
            if (X[i][bestFeature] <= bestThreshold) {
                leftX.push_back(X[i]);
            } else {
                rightX.push_back(X[i]);
            }
        }

        node->left = buildTree(leftX, bestLeftLabels);
        node->right = buildTree(rightX, bestRightLabels);
        return node;
    }

    double entropy(const vector<int>& labels) {
        unordered_map<int, int> counts;
        for (int label : labels) {
            counts[label]++;
        }
        double entropy = 0.0;
        for (const auto& pair : counts) {
            double p = static_cast<double>(pair.second) / labels.size();
            entropy -= p * log2(p);
        }
        return entropy;
    }

    double informationGain(const vector<int>& parent, const vector<int>& left, const vector<int>& right) {
        double parentEntropy = entropy(parent);
        double weightedChildEntropy = (static_cast<double>(left.size()) / parent.size()) * entropy(left) +
                                       (static_cast<double>(right.size()) / parent.size()) * entropy(right);
        return parentEntropy - weightedChildEntropy;
    }

    vector<double> getUniqueThresholds(const vector<vector<double>>& X, size_t featureIndex) {
        vector<double> thresholds;
        for (const auto& row : X) {
            thresholds.push_back(row[featureIndex]);
        }
        sort(thresholds.begin(), thresholds.end());
        thresholds.erase(unique(thresholds.begin(), thresholds.end()), thresholds.end());
        return thresholds;
    }

    int majorityClass(const vector<int>& labels) {
        unordered_map<int, int> counts;
        for (int label : labels) {
            counts[label]++;
        }
        return max_element(counts.begin(), counts.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
        })->first;
    }

    int predict(const vector<double>& sample, Node* node) {
        if (!node) return -1; // Handle null case
        if (node->label != -1) return node->label; // Leaf node
        if (sample[node->featureIndex] <= node->threshold) {
            return predict(sample, node->left);
        } else {
            return predict(sample, node->right);
        }
    }
};

int main() {
    // Sample dataset
    vector<vector<double>> X = {
        {2.0, 3.0},
        {1.0, 1.0},
        {3.0, 3.0},
        {4.0, 1.0},
        {5.0, 5.0}
    };
    
    vector<int> y = {0, 0, 1, 1, 1}; // Class labels

    DecisionTree tree(2); // Minimum samples to split
    tree.fit(X, y);

    // Predictions
    vector<vector<double>> samples = {
        {2.5, 2.5},
        {3.5, 3.5}
    };

    for (const auto& sample : samples) {
        cout << "Predicted class for sample { ";
        for (double feature : sample) {
            cout << feature << " ";
        }
        cout << "} is " << tree.predict(sample) << endl;
    }

    return 0;
}
