#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <algorithm>
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

// Function to find all points within the epsilon neighborhood of a point
vector<int> regionQuery(const vector<Point> &points, int pointIndex, double epsilon) {
    vector<int> neighbors;
    for (int i = 0; i < points.size(); ++i) {
        if (euclideanDistance(points[pointIndex], points[i]) <= epsilon) {
            neighbors.push_back(i);
        }
    }
    return neighbors;
}

// Function to expand the cluster starting from a given point
void expandCluster(vector<Point> &points, int pointIndex, int clusterID, double epsilon, int minPts) {
    vector<int> seeds = regionQuery(points, pointIndex, epsilon);

    if (seeds.size() < minPts) {
        points[pointIndex].clusterID = 0;  // Mark as noise (clusterID = 0)
        return;
    }

    // Assign the cluster ID to the seed points
    for (int seed : seeds) {
        points[seed].clusterID = clusterID;
    }

    // Remove the core point itself from the list
    seeds.erase(remove(seeds.begin(), seeds.end(), pointIndex), seeds.end());

    while (!seeds.empty()) {
        int currentPoint = seeds.back();
        seeds.pop_back();

        vector<int> result = regionQuery(points, currentPoint, epsilon);
        if (result.size() >= minPts) {
            for (int i : result) {
                if (points[i].clusterID == -1 || points[i].clusterID == 0) {
                    if (points[i].clusterID == -1) {
                        seeds.push_back(i);
                    }
                    points[i].clusterID = clusterID;
                }
            }
        }
    }
}

// DBSCAN clustering algorithm
void dbscan(vector<Point> &points, double epsilon, int minPts) {
    int clusterID = 1;  // Start cluster ID from 1 (0 is reserved for noise)

    for (int i = 0; i < points.size(); ++i) {
        if (points[i].clusterID == -1) {  // If point is not yet visited
            expandCluster(points, i, clusterID, epsilon, minPts);
            if (points[i].clusterID == clusterID) {
                clusterID++;  // Increment cluster ID if a new cluster was formed
            }
        }
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

// Function to write the clusters to output file
void writeClustersToFile(const vector<Point> &points, const string &filename) {
    ofstream output(filename);
    if (!output.is_open()) {
        cerr << "Error: Could not create " << filename << endl;
        return;
    }

    set<int> uniqueClusters;
    for (const auto &point : points) {
        uniqueClusters.insert(point.clusterID);
    }

    for (int clusterID : uniqueClusters) {
        if (clusterID == 0) {
            output << "Noise: ";
        } else {
            output << "Cluster " << clusterID << ": ";
        }

        for (const auto &point : points) {
            if (point.clusterID == clusterID) {
                output << "[";
                for (size_t i = 0; i < point.coordinates.size(); ++i) {
                    output << point.coordinates[i];
                    if (i < point.coordinates.size() - 1) output << ", ";
                }
                output << "] ";
            }
        }
        output << endl;
    }

    output.close();
}

int main() {
    // Read dataset from file
    vector<Point> data = readDataset("dataset.txt");

    double epsilon = 3.0;  // Maximum distance between two points to be considered neighbors
    int minPts = 2;        // Minimum number of points required to form a dense region

    // Perform DBSCAN clustering
    dbscan(data, epsilon, minPts);

    // Write the results to output file
    writeClustersToFile(data, "output.txt");

    cout << "Clustering completed. Results saved to output.txt." << endl;

    return 0;
}

