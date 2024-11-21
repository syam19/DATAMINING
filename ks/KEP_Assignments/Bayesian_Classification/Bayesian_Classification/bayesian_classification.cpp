#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

// Structure to hold a data point (features and label)
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

// Function to calculate prior probabilities of classes (yes and no)
map<string, double> calculate_prior(const vector<DataPoint> &data) {
    map<string, int> class_count;
    for (const auto &dp : data) {
        class_count[dp.label]++;
    }

    map<string, double> prior;
    int total = data.size();
    for (const auto &[class_label, count] : class_count) {
        prior[class_label] = (double)count / total;
    }

    return prior;
}

// Function to calculate likelihood (P(feature|class))
map<string, map<string, map<string, double>>> calculate_likelihood(const vector<DataPoint> &data, const vector<string> &features) {
    map<string, map<string, map<string, int>>> feature_class_count;
    map<string, int> class_count;

    for (const auto &dp : data) {
        class_count[dp.label]++;
        for (size_t i = 0; i < dp.features.size(); ++i) {
            feature_class_count[features[i]][dp.features[i]][dp.label]++;
        }
    }

    map<string, map<string, map<string, double>>> likelihood;
    for (const auto &[feature, feature_value_map] : feature_class_count) {
        for (const auto &[feature_value, class_map] : feature_value_map) {
            for (const auto &[class_label, count] : class_map) {
                likelihood[feature][feature_value][class_label] = (double)count / class_count[class_label];
            }
        }
    }

    return likelihood;
}

// Function to classify a new data point using Bayesian classification
string classify(const DataPoint &dp, const map<string, double> &prior, const map<string, map<string, map<string, double>>> &likelihood, const vector<string> &features) {
    map<string, double> posterior;
    
    for (const auto &[class_label, prior_prob] : prior) {
        posterior[class_label] = prior_prob;  // Start with prior probability
        
        // Multiply by likelihood for each feature
        for (size_t i = 0; i < dp.features.size(); ++i) {
            string feature_value = dp.features[i];
            string feature_name = features[i];
            if (likelihood.count(feature_name) && likelihood.at(feature_name).count(feature_value) && likelihood.at(feature_name).at(feature_value).count(class_label)) {
                posterior[class_label] *= likelihood.at(feature_name).at(feature_value).at(class_label);
            } else {
                posterior[class_label] *= 0;  // Handle unseen feature values
            }
        }
    }

    // Find the class with the highest posterior probability
    string best_class;
    double best_prob = -1;
    for (const auto &[class_label, prob] : posterior) {
        if (prob > best_prob) {
            best_prob = prob;
            best_class = class_label;
        }
    }

    return best_class;
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

    // Calculate prior probabilities and likelihoods
    map<string, double> prior = calculate_prior(data);
    map<string, map<string, map<string, double>>> likelihood = calculate_likelihood(data, features);

    // Classify the data points (output the original data along with the predicted label)
    output << "age,income,student,credit_rating,predicted_buys_computer" << endl;
    for (const auto &dp : data) {
        string predicted_class = classify(dp, prior, likelihood, features);
        for (const auto &feature : dp.features) {
            output << feature << ",";
        }
        output << predicted_class << endl;
    }

    input.close();
    output.close();

    cout << "Classification results generated in output.txt" << endl;
    return 0;
}

