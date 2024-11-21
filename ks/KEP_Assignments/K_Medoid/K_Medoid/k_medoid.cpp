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

// Function to initialize medoids using the k-medoids++ method
vector<int> initialize_medoids(const vector<vector<double>>& data, int k) {
    vector<int> medoids;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, data.size() - 1);

    // Choose the first medoid randomly
    medoids.push_back(dis(gen));

    for (int i = 1; i < k; ++i) {
        vector<double> distances(data.size(), numeric_limits<double>::max());
        
        // Calculate distances from points to nearest medoid
        for (size_t j = 0; j < data.size(); ++j) {
            for (int medoid : medoids) {
                double dist = euclidean_distance(data[j], data[medoid]);
                distances[j] = min(distances[j], dist);
            }
        }

        // Choose the next medoid with probability proportional to distance
        discrete_distribution<> d(distances.begin(), distances.end());
        medoids.push_back(d(gen));
    }

    return medoids;
}

// Function to assign each point to the nearest medoid
vector<int> assign_clusters(const vector<vector<double>>& data, const vector<int>& medoids) {
    vector<int> assignments(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        double min_dist = numeric_limits<double>::max();
        int cluster_idx = 0;
        for (size_t j = 0; j < medoids.size(); ++j) {
            double dist = euclidean_distance(data[i], data[medoids[j]]);
            if (dist < min_dist) {
                min_dist = dist;
                cluster_idx = j;
            }
        }
        assignments[i] = cluster_idx;
    }
    return assignments;
}

// Function to update medoids by finding the point with minimal total distance
vector<int> update_medoids(const vector<vector<double>>& data, const vector<int>& assignments, int k) {
    vector<int> new_medoids(k);
    for (int i = 0; i < k; ++i) {
        double min_total_distance = numeric_limits<double>::max();
        int best_medoid = -1;
        vector<int> cluster_points;
        
        // Collect points in the current cluster
        for (size_t j = 0; j < data.size(); ++j) {
            if (assignments[j] == i) {
                cluster_points.push_back(j);
            }
        }
        
        // Find the point with minimal total distance to other points in the cluster
        for (int point : cluster_points) {
            double total_distance = 0.0;
            for (int other_point : cluster_points) {
                total_distance += euclidean_distance(data[point], data[other_point]);
            }
            if (total_distance < min_total_distance) {
                min_total_distance = total_distance;
                best_medoid = point;
            }
        }
        
        new_medoids[i] = best_medoid;
    }
    return new_medoids;
}

// Function to check convergence
bool is_converged(const vector<int>& medoids, const vector<int>& new_medoids) {
    return medoids == new_medoids;
}

// K-medoids Clustering Algorithm
void k_medoids(const vector<vector<double>>& data, int k, const string& output_filename, int max_iterations = 100) {
    ofstream output(output_filename);
    if (!output.is_open()) {
        cerr << "Error: Could not open output file." << endl;
        return;
    }

    vector<int> medoids = initialize_medoids(data, k);
    vector<int> assignments;

    for (int iter = 0; iter < max_iterations; ++iter) {
        assignments = assign_clusters(data, medoids);
        vector<int> new_medoids = update_medoids(data, assignments, k);

        if (is_converged(medoids, new_medoids)) {
            output << "Converged after " << iter + 1 << " iterations." << endl;
            break;
        }
        medoids = new_medoids;
    }

    // Write the clusters to the output file
    vector<int> cluster_sizes(k, 0);
    for (int cluster : assignments) {
        cluster_sizes[cluster]++;
    }
    
    for (int i = 0; i < k; ++i) {
        output << "Cluster " << i + 1 << " size: " << cluster_sizes[i] << endl;
        output << "Medoid: (";
        for (size_t j = 0; j < data[medoids[i]].size(); ++j) {
            output << data[medoids[i]][j];
            if (j != data[medoids[i]].size() - 1) output << ", ";
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
    k_medoids(data, k, output_filename);

    cout << "K-medoids clustering completed. Results saved to " << output_filename << "." << endl;
    return 0;
}

