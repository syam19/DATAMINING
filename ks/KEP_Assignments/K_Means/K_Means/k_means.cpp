#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <chrono>

using namespace std;

// Function to read data from a CSV file into a vector of vectors
vector<vector<double>> read_file(const string& filename) {
    vector<vector<double>> data;
    ifstream file(filename);
    string line;
    
    // Skip the header line
    getline(file, line);
    
    while (getline(file, line)) {
        vector<double> row;
        stringstream line_stream(line);
        string cell;
        while (getline(line_stream, cell, ',')) {
            row.push_back(stod(cell));
        }
        data.push_back(row);
    }
    return data;
}

// Function to calculate Euclidean distance between two points
double euclidean_distance(const vector<double>& a, const vector<double>& b) {
    double distance = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        distance += pow(a[i] - b[i], 2);
    }
    return sqrt(distance);
}

// Function to initialize centroids using K-means++ method
vector<vector<double>> initialize_centroids(const vector<vector<double>>& data, int k) {
    vector<vector<double>> centroids;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, data.size() - 1);

    // Choose the first centroid randomly
    centroids.push_back(data[dis(gen)]);

    for (int i = 1; i < k; ++i) {
        vector<double> distances(data.size(), numeric_limits<double>::max());
        
        // Calculate distances from points to nearest centroid
        for (size_t j = 0; j < data.size(); ++j) {
            for (const auto& centroid : centroids) {
                double dist = euclidean_distance(data[j], centroid);
                distances[j] = min(distances[j], dist);
            }
        }

        // Choose the next centroid with probability proportional to distance
        discrete_distribution<> d(distances.begin(), distances.end());
        centroids.push_back(data[d(gen)]);
    }

    return centroids;
}

// Function to assign each point to the nearest centroid
vector<int> assign_clusters(const vector<vector<double>>& data, const vector<vector<double>>& centroids) {
    vector<int> assignments(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        double min_dist = numeric_limits<double>::max();
        int cluster_idx = 0;
        for (size_t j = 0; j < centroids.size(); ++j) {
            double dist = euclidean_distance(data[i], centroids[j]);
            if (dist < min_dist) {
                min_dist = dist;
                cluster_idx = j;
            }
        }
        assignments[i] = cluster_idx;
    }
    return assignments;
}

// Function to update centroids by calculating the mean of assigned points
vector<vector<double>> update_centroids(const vector<vector<double>>& data, const vector<int>& assignments, int k) {
    vector<vector<double>> new_centroids(k, vector<double>(data[0].size(), 0.0));
    vector<int> counts(k, 0);

    for (size_t i = 0; i < data.size(); ++i) {
        int cluster_idx = assignments[i];
        for (size_t j = 0; j < data[i].size(); ++j) {
            new_centroids[cluster_idx][j] += data[i][j];
        }
        counts[cluster_idx]++;
    }

    for (int i = 0; i < k; ++i) {
        if (counts[i] > 0) {
            for (size_t j = 0; j < data[0].size(); ++j) {
                new_centroids[i][j] /= counts[i];
            }
        } else {
            // If a cluster is empty, reinitialize it to a random point
            new_centroids[i] = data[rand() % data.size()];
        }
    }
    return new_centroids;
}

// Function to check convergence
bool is_converged(const vector<vector<double>>& centroids, const vector<vector<double>>& new_centroids, double tolerance = 1e-4) {
    for (size_t i = 0; i < centroids.size(); ++i) {
        if (euclidean_distance(centroids[i], new_centroids[i]) > tolerance) {
            return false;
        }
    }
    return true;
}

// K-means Clustering Algorithm
void k_means(const vector<vector<double>>& data, int k, const string& output_filename, int max_iterations = 100) {
    ofstream output(output_filename);
    if (!output.is_open()) {
        cerr << "Error: Could not open output file." << endl;
        return;
    }

    vector<vector<double>> centroids = initialize_centroids(data, k);
    vector<int> assignments;

    for (int iter = 0; iter < max_iterations; ++iter) {
        assignments = assign_clusters(data, centroids);
        vector<vector<double>> new_centroids = update_centroids(data, assignments, k);

        if (is_converged(centroids, new_centroids)) {
            output << "Converged after " << iter + 1 << " iterations." << endl;
            break;
        }
        centroids = new_centroids;
    }

    // Write the clusters to the output file
    vector<int> cluster_sizes(k, 0);
    for (int cluster : assignments) {
        cluster_sizes[cluster]++;
    }
    
    for (int i = 0; i < k; ++i) {
        output << "Cluster " << i + 1 << " size: " << cluster_sizes[i] << endl;
        output << "Centroid: (";
        for (size_t j = 0; j < centroids[i].size(); ++j) {
            output << centroids[i][j];
            if (j != centroids[i].size() - 1) output << ", ";
        }
        output << ")" << endl << endl;
    }

    output.close();
}

int main() {
    string filename = "dataset.txt";  // Use dataset.txt as input file
    vector<vector<double>> data = read_file(filename);
    int k = 3; // Number of clusters

    // Seed the random number generator
    srand(static_cast<unsigned>(chrono::system_clock::now().time_since_epoch().count()));

    string output_filename = "output.txt";  // Output file for cluster results
    k_means(data, k, output_filename);

    cout << "K-means clustering completed. Results saved to " << output_filename << "." << endl;
    return 0;
}

