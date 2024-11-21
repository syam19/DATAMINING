#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

class NeuralNetwork {
private:
    vector<vector<double>> weightsInputHidden;
    vector<double> weightsHiddenOutput;
    double learningRate;
    int hiddenSize;

    // Activation function (Sigmoid)
    double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }

    // Derivative of the sigmoid function
    double sigmoidDerivative(double x) {
        return x * (1.0 - x);
    }

public:
    NeuralNetwork(int inputSize, int hiddenSize, double lr) 
        : hiddenSize(hiddenSize), learningRate(lr) {
        srand(static_cast<unsigned int>(time(0)));

        // Initialize weights between input and hidden layer
        weightsInputHidden.resize(inputSize, vector<double>(hiddenSize));
        for (auto& row : weightsInputHidden) {
            for (auto& weight : row) {
                weight = ((double)rand() / RAND_MAX) * 2 - 1; // Random weights between -1 and 1
            }
        }

        // Initialize weights between hidden layer and output layer
        weightsHiddenOutput.resize(hiddenSize);
        for (auto& weight : weightsHiddenOutput) {
            weight = ((double)rand() / RAND_MAX) * 2 - 1; // Random weights between -1 and 1
        }
    }

    // Forward pass
    double predict(const vector<double>& input) {
        // Hidden layer activation
        vector<double> hidden(hiddenSize, 0.0);
        for (int j = 0; j < hiddenSize; j++) {
            for (int i = 0; i < input.size(); i++) {
                hidden[j] += input[i] * weightsInputHidden[i][j];
            }
            hidden[j] = sigmoid(hidden[j]);
        }

        // Output layer activation
        double output = 0.0;
        for (int j = 0; j < hiddenSize; j++) {
            output += hidden[j] * weightsHiddenOutput[j];
        }
        return sigmoid(output);
    }

    // Train the network
    void train(const vector<vector<double>>& inputs, const vector<double>& targets, int epochs) {
        for (int epoch = 0; epoch < epochs; epoch++) {
            for (size_t i = 0; i < inputs.size(); i++) {
                // Forward pass
                vector<double> hidden(hiddenSize, 0.0);
                for (int j = 0; j < hiddenSize; j++) {
                    for (int k = 0; k < inputs[i].size(); k++) {
                        hidden[j] += inputs[i][k] * weightsInputHidden[k][j];
                    }
                    hidden[j] = sigmoid(hidden[j]);
                }

                double output = 0.0;
                for (int j = 0; j < hiddenSize; j++) {
                    output += hidden[j] * weightsHiddenOutput[j];
                }
                output = sigmoid(output);

                // Calculate error
                double error = targets[i] - output;

                // Backpropagation
                // Calculate gradients for hidden-output weights
                double outputDelta = error * sigmoidDerivative(output);
                for (int j = 0; j < hiddenSize; j++) {
                    weightsHiddenOutput[j] += learningRate * outputDelta * hidden[j];
                }

                // Calculate gradients for input-hidden weights
                vector<double> hiddenDelta(hiddenSize, 0.0);
                for (int j = 0; j < hiddenSize; j++) {
                    hiddenDelta[j] = outputDelta * weightsHiddenOutput[j] * sigmoidDerivative(hidden[j]);
                }

                for (int j = 0; j < hiddenSize; j++) {
                    for (int k = 0; k < inputs[i].size(); k++) {
                        weightsInputHidden[k][j] += learningRate * hiddenDelta[j] * inputs[i][k];
                    }
                }
            }
        }
    }
};

int main() {
    // XOR problem data
    vector<vector<double>> inputs = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };

    vector<double> targets = {0, 1, 1, 0}; // XOR output

    NeuralNetwork nn(2, 2, 0.1); // 2 input nodes, 2 hidden nodes
    nn.train(inputs, targets, 10000); // Train the network

    // Test predictions
    cout << "Testing Predictions:" << endl;
    for (const auto& input : inputs) {
        double output = nn.predict(input);
        cout << "Input: " << input[0] << ", " << input[1] 
             << " => Predicted Output: " << round(output) 
             << " (Raw Output: " << output << ")" << endl;
    }

    return 0;
}
