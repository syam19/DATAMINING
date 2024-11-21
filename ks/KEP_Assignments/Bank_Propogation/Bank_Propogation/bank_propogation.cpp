#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;

// Sigmoid activation function
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Derivative of the sigmoid function
double sigmoid_derivative(double x) {
    return x * (1 - x);
}

class NeuralNetwork {
private:
    vector<vector<double>> weights_input_hidden;
    vector<double> weights_hidden_output;
    double learning_rate;

public:
    NeuralNetwork(int input_size, int hidden_size) {
        // Initialize weights with random values
        weights_input_hidden.resize(input_size, vector<double>(hidden_size));
        for (auto &row : weights_input_hidden)
            for (auto &val : row)
                val = (double)rand() / RAND_MAX; // Random weight initialization

        weights_hidden_output.resize(hidden_size);
        for (auto &val : weights_hidden_output)
            val = (double)rand() / RAND_MAX; // Random weight initialization

        learning_rate = 0.1; // Set learning rate
    }

    // Forward pass
    double predict(const vector<double> &input) {
        vector<double> hidden(hidden_size, 0);
        for (int i = 0; i < hidden_size; ++i) {
            hidden[i] = 0;
            for (int j = 0; j < input_size; ++j) {
                hidden[i] += input[j] * weights_input_hidden[j][i];
            }
            hidden[i] = sigmoid(hidden[i]); // Activation function
        }

        double output = 0;
        for (int i = 0; i < hidden_size; ++i) {
            output += hidden[i] * weights_hidden_output[i];
        }
        return sigmoid(output); // Final output
    }

    // Backpropagation
    void train(const vector<vector<double>> &input_data, const vector<double> &output_data, int epochs) {
        for (int epoch = 0; epoch < epochs; ++epoch) {
            for (size_t i = 0; i < input_data.size(); ++i) {
                // Forward pass
                vector<double> input = input_data[i];
                double target = output_data[i];

                // Calculate hidden layer output
                vector<double> hidden(hidden_size, 0);
                for (int j = 0; j < hidden_size; ++j) {
                    hidden[j] = 0;
                    for (int k = 0; k < input_size; ++k) {
                        hidden[j] += input[k] * weights_input_hidden[k][j];
                    }
                    hidden[j] = sigmoid(hidden[j]);
                }

                // Calculate final output
                double output = 0;
                for (int j = 0; j < hidden_size; ++j) {
                    output += hidden[j] * weights_hidden_output[j];
                }
                output = sigmoid(output);

                // Backpropagation
                double output_error = target - output;
                for (int j = 0; j < hidden_size; ++j) {
                    weights_hidden_output[j] += learning_rate * output_error * sigmoid_derivative(output) * hidden[j];
                }

                for (int j = 0; j < hidden_size; ++j) {
                    for (int k = 0; k < input_size; ++k) {
                        double hidden_error = output_error * weights_hidden_output[j] * sigmoid_derivative(hidden[j]);
                        weights_input_hidden[k][j] += learning_rate * hidden_error * input[k];
                    }
                }
            }
        }
    }

    // Function to output weights to a file
    void output_weights(ofstream &output) {
        output << "Weights from input to hidden layer:\n";
        for (const auto &row : weights_input_hidden) {
            for (const auto &val : row) {
                output << val << " ";
            }
            output << "\n";
        }
        output << "Weights from hidden to output layer:\n";
        for (const auto &val : weights_hidden_output) {
            output << val << " ";
        }
        output << "\n";
    }

private:
    int input_size = 2; // Number of input features (age and income)
    int hidden_size = 2; // Number of hidden neurons
};

int main() {
    // Read dataset from file
    ifstream input("dataset.txt");
    if (!input.is_open()) {
        cerr << "Error: Could not open dataset.txt" << endl;
        return 1;
    }

    string line;
    vector<vector<double>> input_data;
    vector<double> output_data;

    // Skip the header
    getline(input, line);

    // Read the data
    while (getline(input, line)) {
        stringstream ss(line);
        string age_str, income_str, buy_str;

        getline(ss, age_str, ',');
        getline(ss, income_str, ',');
        getline(ss, buy_str, ',');

        double age = stod(age_str); // Convert age to double
        double income = stod(income_str); // Convert income to double
        double buy = stod(buy_str); // Convert buy to double

        input_data.push_back({age, income});
        output_data.push_back(buy);
    }

    input.close();

    NeuralNetwork nn(2, 2); // 2 input features (age, income), 2 hidden neurons
    nn.train(input_data, output_data, 10000); // Train the neural network

    // Write the weights to output.txt
    ofstream output("output.txt");
    if (!output.is_open()) {
        cerr << "Error: Could not create output.txt" << endl;
        return 1;
    }

    nn.output_weights(output);
    output.close();

    cout << "Neural network training completed. Weights saved to output.txt" << endl;
    return 0;
}

