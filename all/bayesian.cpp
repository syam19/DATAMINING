#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace std;

class NaiveBayes {
private:
    unordered_map<string, double> priorProbabilities;
    unordered_map<string, unordered_map<string, double>> likelihoods;
    vector<string> classes;
    int totalDocuments = 0;

public:
    void fit(const vector<vector<string>>& data, const vector<string>& labels) {
        unordered_map<string, int> classCounts;
        unordered_map<string, unordered_map<string, int>> featureCounts;

        // Count occurrences of each class and feature
        for (size_t i = 0; i < labels.size(); ++i) {
            classCounts[labels[i]]++;
            totalDocuments++;

            for (const auto& word : data[i]) {
                featureCounts[labels[i]][word]++;
            }
        }

        // Calculate prior probabilities
        for (const auto& className : classCounts) {
            priorProbabilities[className.first] = static_cast<double>(className.second) / totalDocuments;
            classes.push_back(className.first);
        }

        // Calculate likelihoods
        for (const auto& className : classes) {
            for (const auto& feature : featureCounts[className]) {
                likelihoods[className][feature.first] = static_cast<double>(feature.second) / classCounts[className];
            }
        }
    }

    string predict(const vector<string>& instance) {
        double maxProbability = -1.0;
        string bestClass;

        for (const auto& className : classes) {
            double probability = priorProbabilities[className];

            for (const auto& feature : instance) {
                double likelihood = likelihoods[className][feature];
                // Using Laplace smoothing
                probability *= (likelihood > 0 ? likelihood : 1e-6);
            }

            if (probability > maxProbability) {
                maxProbability = probability;
                bestClass = className;
            }
        }

        return bestClass;
    }
};

vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string token;
    while (ss >> token) {
        // Simple normalization: convert to lowercase and remove punctuation
        transform(token.begin(), token.end(), token.begin(), ::tolower);
        token.erase(remove_if(token.begin(), token.end(), ::ispunct), token.end());
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    // Example dataset
    vector<string> emails = {
        "Free money now",
        "Hello, how are you?",
        "Win a million dollars",
        "Important update on your account",
        "Hi, can we meet tomorrow?",
        "Exclusive offer just for you",
        "Meeting at noon",
        "Congratulations, you've won!",
        "See you at the conference",
        "Claim your prize now"
    };

    vector<string> labels = {
        "spam", "not spam", "spam", "not spam", "not spam", 
        "spam", "not spam", "spam", "not spam", "spam"
    };

    NaiveBayes nb;
    vector<vector<string>> tokenizedData;

    // Tokenize the emails
    for (const auto& email : emails) {
        tokenizedData.push_back(tokenize(email));
    }

    nb.fit(tokenizedData, labels);

    // Predict a new email
    string newEmail = "Win big money now!";
    vector<string> newInstance = tokenize(newEmail);
    string predictedClass = nb.predict(newInstance);

    cout << "Predicted class: " << predictedClass << endl;

    return 0;
}
