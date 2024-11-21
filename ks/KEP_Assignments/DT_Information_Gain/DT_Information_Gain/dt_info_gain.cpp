#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

struct DataPoint {
    vector<string> features;
    string label;
};

// Function to split a string by a delimiter
vector<string> split(const string &line, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(line);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to calculate entropy
double entropy(const vector<string> &labels) {
    map<string, int> label_count;
    for (const string &label : labels) {
        label_count[label]++;
    }
    double entropy_value = 0.0;
    for (const auto &[label, count] : label_count) {
        double probability = (double)count / labels.size();
        entropy_value -= probability * log2(probability);
    }
    return entropy_value;
}

// Function to calculate information gain
double information_gain(const vector<DataPoint> &data, int feature_idx, const vector<string> &unique_labels) {
    double total_entropy = entropy(unique_labels);
    map<string, vector<string>> subsets;
    for (const DataPoint &dp : data) {
        subsets[dp.features[feature_idx]].push_back(dp.label);
    }
    double subset_entropy = 0.0;
    for (const auto &[value, subset_labels] : subsets) {
        subset_entropy += (double)subset_labels.size() / unique_labels.size() * entropy(subset_labels);
    }
    return total_entropy - subset_entropy;
}

// Function to find the best feature to split on
int best_split_feature(const vector<DataPoint> &data, const vector<string> &unique_labels) {
    int num_features = data[0].features.size();
    double best_gain = -1;
    int best_feature_idx = -1;
    for (int i = 0; i < num_features; i++) {
        double gain = information_gain(data, i, unique_labels);
        if (gain > best_gain) {
            best_gain = gain;
            best_feature_idx = i;
        }
    }
    return best_feature_idx;
}

// Function to recursively build the decision tree
void decision_tree(ofstream &output, const vector<DataPoint> &data, const vector<string> &features, int depth) {
    vector<string> unique_labels;
    for (const DataPoint &dp : data) {
        unique_labels.push_back(dp.label);
    }

    // Base case: if all labels are the same, output the decision
    if (unique(unique_labels.begin(), unique_labels.end()) == unique_labels.end()) {
        output << string(depth, '-') << " Decision: " << unique_labels[0] << endl;
        return;
    }

    // Find the best feature to split on
    int best_feature = best_split_feature(data, unique_labels);
    output << string(depth, '-') << " Split on feature: " << features[best_feature] << endl;

    // Split the data by the selected feature
    map<string, vector<DataPoint>> subsets;
    for (const DataPoint &dp : data) {
        subsets[dp.features[best_feature]].push_back(dp);
    }

    // Recursively build the tree for each subset
    for (const auto &[value, subset] : subsets) {
        output << string(depth + 1, '-') << " Value: " << value << endl;
        decision_tree(output, subset, features, depth + 2);
    }
}

int main() {
    ifstream input("dataset.txt");
    ofstream output("output.txt");

    if (!input.is_open()) {
        cerr << "Error: Could not open dataset.txt" << endl;
        return 1;
    }

    if (!output.is_open()) {
        cerr << "Error: Could not create output.txt" << endl;
        return 1;
    }

    string line;
    vector<DataPoint> data;
    vector<string> features;

    // Read the header (features)
    if (getline(input, line)) {
        features = split(line, ',');
    }

    // Read the data rows
    while (getline(input, line)) {
        vector<string> tokens = split(line, ',');
        
        // Ensure the row has the correct number of columns
        if (tokens.size() != features.size()) {
            cerr << "Error: Invalid data row (incorrect number of columns)" << endl;
            continue;  // Skip invalid rows
        }

        // Store the data points
        DataPoint dp;
        dp.features.assign(tokens.begin(), tokens.end() - 1);
        dp.label = tokens.back();
        data.push_back(dp);
    }

    if (data.empty()) {
        cerr << "Error: No valid data found in the dataset" << endl;
        return 1;
    }

    decision_tree(output, data, features, 0);

    input.close();
    output.close();

    cout << "Decision tree generated in output.txt" << endl;
    return 0;
}

