#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

// Structure to represent a point in the dataset
struct Point {
    vector<double> coordinates;
    int clusterID;

    Point(vector<double> coords) : coordinates(coords), clusterID(-1) {}
};

// Function to calculate Euclidean distance between two points
double euclideanDistance(const Point &a, const Point &b) {
    double dist = 0.0;
    for (size_t i = 0; i < a.coordinates.size(); ++i) {
        dist += pow(a.coordinates[i] - b.coordinates[i], 2);
    }
    return sqrt(dist);
}

// Function to find the nearest cluster for a given point
int findNearestCluster(const Point &point, const vector<Point> &centroids) {
    int nearestCluster = -1;
    double minDist = numeric_limits<double>::max();

    for (size_t i = 0; i < centroids.size(); ++i) {
        double dist = euclideanDistance(point, centroids[i]);
        if (dist < minDist) {
            minDist = dist;
            nearestCluster = i;
        }
    }
    return nearestCluster;
}

// Function to update centroids based on the assigned points
void updateCentroids(const vector<Point> &points, vector<Point> &centroids, int k) {
    vector<int> clusterSizes(k, 0);
    vector<vector<double>> newCoords(k, vector<double>(points[0].coordinates.size(), 0.0));

    // Sum the coordinates of all points in each cluster
    for (const auto &point : points) {
        int clusterID = point.clusterID;
        clusterSizes[clusterID]++;
        for (size_t i = 0; i < point.coordinates.size(); ++i) {
            newCoords[clusterID][i] += point.coordinates[i];
        }
    }

    // Calculate new centroids
    for (int i = 0; i < k; ++i) {
        if (clusterSizes[i] > 0) {
            for (size_t j = 0; j < centroids[i].coordinates.size(); ++j) {
                centroids[i].coordinates[j] = newCoords[i][j] / clusterSizes[i];
            }
        }
    }
}

// CHAMELEON-inspired clustering algorithm (simplified version)
void chameleonClustering(vector<Point> &points, int k, ofstream &output) {
    // Step 1: Initialize centroids randomly from the dataset
    vector<Point> centroids;
    for (int i = 0; i < k; ++i) {
        centroids.push_back(points[rand() % points.size()]);
    }

    bool changed = true;
    while (changed) {
        changed = false;

        // Step 2: Assign each point to the nearest centroid
        for (auto &point : points) {
            int newClusterID = findNearestCluster(point, centroids);
            if (newClusterID != point.clusterID) {
                point.clusterID = newClusterID;
                changed = true;
            }
        }

        // Step 3: Update centroids based on the new cluster assignments
        updateCentroids(points, centroids, k);
    }

    // Display the final clusters to output file
    output << "Clusters formed:\n";
    for (int i = 0; i < k; ++i) {
        output << "Cluster " << i + 1 << ": ";
        for (const auto &point : points) {
            if (point.clusterID == i) {
                output << "[";
                for (size_t j = 0; j < point.coordinates.size(); ++j) {
                    output << point.coordinates[j];
                    if (j < point.coordinates.size() - 1) output << ", ";
                }
                output << "] ";
            }
        }
        output << endl;
    }
}

// Function to read dataset from file
vector<Point> readDataset(const string &filename) {
    ifstream input(filename);
    vector<Point> points;
    string line;

    while (getline(input, line)) {
        stringstream ss(line);
        string value;
        vector<double> coords;
        
        while (getline(ss, value, ',')) {
            coords.push_back(stod(value));
        }
        points.push_back(Point(coords));
    }

    input.close();
    return points;
}

int main() {
    srand(time(0));  // Initialize random seed

    // Read dataset from file
    vector<Point> data = readDataset("dataset.txt");

    int k = 3;  // Number of clusters

    // Open output file
    ofstream output("output.txt");
    if (!output.is_open()) {
        cerr << "Error: Could not create output.txt" << endl;
        return 1;
    }

    // Perform clustering
    chameleonClustering(data, k, output);

    output.close();
    cout << "Clustering completed. Results saved to output.txt." << endl;

    return 0;
}

