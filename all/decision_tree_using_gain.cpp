#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>

using namespace std;

// Function to calculate entropy
double calculateEntropy(const vector<int>& labels) {
    unordered_map<int, int> count;
    for (int label : labels) {
        count[label]++;
    }

    double entropy = 0.0;
    int total = labels.size();
    for (const auto& [key, value] : count) {
        double p = static_cast<double>(value) / total;
        entropy -= p * log2(p);
    }
    return entropy;
}

// Function to calculate information gain
double calculateGain(const vector<int>& labels, const vector<int>& feature, int featureValue) {
    vector<int> subsetLabels;
    for (size_t i = 0; i < labels.size(); i++) {
        if (feature[i] == featureValue) {
            subsetLabels.push_back(labels[i]);
        }
    }

    double gain = calculateEntropy(labels);
    if (subsetLabels.empty()) return gain;

    gain -= static_cast<double>(subsetLabels.size()) / labels.size() * calculateEntropy(subsetLabels);
    return gain;
}

// Function to calculate intrinsic value
double calculateIntrinsicValue(const vector<int>& feature) {
    unordered_map<int, int> count;
    for (int value : feature) {
        count[value]++;
    }

    double intrinsicValue = 0.0;
    int total = feature.size();
    for (const auto& [key, value] : count) {
        double p = static_cast<double>(value) / total;
        intrinsicValue -= p * log2(p);
    }
    return intrinsicValue;
}

// Function to calculate gain ratio
double calculateGainRatio(const vector<int>& labels, const vector<int>& feature, int featureValue) {
    double gain = calculateGain(labels, feature, featureValue);
    double intrinsicValue = calculateIntrinsicValue(feature);
    return intrinsicValue == 0 ? 0 : gain / intrinsicValue;
}

// Function to build the decision tree (simplified)
void buildDecisionTree(const vector<vector<int>>& data, const vector<int>& labels, int depth = 0) {
    // Base case: all labels are the same or no features left
    if (all_of(labels.begin(), labels.end(), [&](int l) { return l == labels[0]; })) {
        cout << "Leaf node at depth " << depth << " with label: " << labels[0] << endl;
        return;
    }

    double bestGainRatio = -1;
    int bestFeatureIndex = -1;
    unordered_map<int, vector<int>> featureValues;

    // Iterate over all features
    for (size_t featureIndex = 0; featureIndex < data[0].size(); featureIndex++) {
        // Store all unique values for the feature
        for (const auto& row : data) {
            featureValues[row[featureIndex]].push_back(row[featureIndex]);
        }

        // Calculate Gain Ratio for each unique value
        for (const auto& [featureValue, _] : featureValues) {
            double gainRatio = calculateGainRatio(labels, featureValues[featureValue], featureValue);
            if (gainRatio > bestGainRatio) {
                bestGainRatio = gainRatio;
                bestFeatureIndex = featureIndex;
            }
        }
        featureValues.clear(); // Clear for the next feature
    }

    if (bestFeatureIndex == -1) {
        cout << "Leaf node at depth " << depth << " with label: " << labels[0] << endl;
        return;
    }

    cout << "Splitting on feature " << bestFeatureIndex << " at depth " << depth << endl;

    // Further split and build subtrees here...
}

int main() {
    // Sample dataset (features and labels)
    vector<vector<int>> data = {
        {0, 1},
        {1, 1},
        {1, 0},
        {0, 0}
    };

    vector<int> labels = {0, 1, 1, 0}; // Corresponding labels

    buildDecisionTree(data, labels);

    return 0;
}
