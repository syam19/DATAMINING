#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <numeric> // For inner_product

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
public:
    NeuralNetwork(int input_nodes, int hidden_nodes, int output_nodes);
    void train(const vector<vector<double>>& inputs, const vector<vector<double>>& targets, int epochs, double learning_rate);
    vector<double> predict(const vector<double>& input);

private:
    int input_nodes;
    int hidden_nodes;
    int output_nodes;
    vector<double> hidden_weights;
    vector<double> output_weights;
    vector<double> hidden_bias;
    vector<double> output_bias;
};

NeuralNetwork::NeuralNetwork(int input_nodes, int hidden_nodes, int output_nodes) 
    : input_nodes(input_nodes), hidden_nodes(hidden_nodes), output_nodes(output_nodes) {
    
    srand(static_cast<unsigned>(time(0)));
    hidden_weights.resize(input_nodes * hidden_nodes);
    output_weights.resize(hidden_nodes * output_nodes);
    hidden_bias.resize(hidden_nodes);
    output_bias.resize(output_nodes);
    
    // Initialize weights and biases randomly
    for (int i = 0; i < hidden_weights.size(); ++i) {
        hidden_weights[i] = static_cast<double>(rand()) / RAND_MAX - 0.5;
    }
    for (int i = 0; i < output_weights.size(); ++i) {
        output_weights[i] = static_cast<double>(rand()) / RAND_MAX - 0.5;
    }
    for (int i = 0; i < hidden_bias.size(); ++i) {
        hidden_bias[i] = static_cast<double>(rand()) / RAND_MAX - 0.5;
    }
    for (int i = 0; i < output_bias.size(); ++i) {
        output_bias[i] = static_cast<double>(rand()) / RAND_MAX - 0.5;
    }
}

void NeuralNetwork::train(const vector<vector<double>>& inputs, const vector<vector<double>>& targets, int epochs, double learning_rate) {
    for (int epoch = 0; epoch < epochs; ++epoch) {
        for (size_t i = 0; i < inputs.size(); ++i) {
            // Forward propagation
            vector<double> input = inputs[i];

            // Hidden layer
            vector<double> hidden(hidden_nodes);
            for (int j = 0; j < hidden_nodes; ++j) {
                hidden[j] = sigmoid(std::inner_product(input.begin(), input.end(), 
                    hidden_weights.begin() + j * input_nodes, hidden_bias[j]));
            }

            // Output layer
            vector<double> output(output_nodes);
            for (int j = 0; j < output_nodes; ++j) {
                output[j] = sigmoid(std::inner_product(hidden.begin(), hidden.end(), 
                    output_weights.begin() + j * hidden_nodes, output_bias[j]));
            }

            // Backpropagation
            vector<double> output_errors(output_nodes);
            for (int j = 0; j < output_nodes; ++j) {
                output_errors[j] = targets[i][j] - output[j];
            }

            vector<double> hidden_errors(hidden_nodes);
            for (int j = 0; j < hidden_nodes; ++j) {
                hidden_errors[j] = std::inner_product(output_errors.begin(), output_errors.end(),
                    output_weights.begin() + j * output_nodes, 0.0) * sigmoid_derivative(hidden[j]);
            }

            // Update output weights and biases
            for (int j = 0; j < output_nodes; ++j) {
                for (int k = 0; k < hidden_nodes; ++k) {
                    output_weights[j * hidden_nodes + k] += learning_rate * output_errors[j] * hidden[k];
                }
                output_bias[j] += learning_rate * output_errors[j];
            }

            // Update hidden weights and biases
            for (int j = 0; j < hidden_nodes; ++j) {
                for (int k = 0; k < input_nodes; ++k) {
                    hidden_weights[j * input_nodes + k] += learning_rate * hidden_errors[j] * input[k];
                }
                hidden_bias[j] += learning_rate * hidden_errors[j];
            }
        }
    }
}

vector<double> NeuralNetwork::predict(const vector<double>& input) {
    // Hidden layer
    vector<double> hidden(hidden_nodes);
    for (int j = 0; j < hidden_nodes; ++j) {
        hidden[j] = sigmoid(std::inner_product(input.begin(), input.end(), 
            hidden_weights.begin() + j * input_nodes, hidden_bias[j]));
    }

    // Output layer
    vector<double> output(output_nodes);
    for (int j = 0; j < output_nodes; ++j) {
        output[j] = sigmoid(std::inner_product(hidden.begin(), hidden.end(), 
            output_weights.begin() + j * hidden_nodes, output_bias[j]));
    }
    
    return output;
}

void load_data(const string& filename, vector<vector<double>>& inputs, vector<vector<double>>& targets) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        vector<double> input(2);
        vector<double> target(1);
        ss >> input[0] >> input[1] >> target[0];
        inputs.push_back(input);
        targets.push_back(target);
    }
}

int main() {
    NeuralNetwork nn(2, 2, 1); // 2 input nodes, 2 hidden nodes, 1 output node

    vector<vector<double>> inputs;
    vector<vector<double>> targets;

    load_data("sample_data.txt", inputs, targets);

    nn.train(inputs, targets, 10000, 0.1);

    // Testing the network
    for (const auto& input : inputs) {
        vector<double> output = nn.predict(input);
        cout << "Input: [" << input[0] << ", " << input[1] << "] => Output: " << output[0] << endl;
    }

    return 0;
}
