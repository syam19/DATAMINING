#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>

using namespace std;

// Clustering Feature (CF) structure
struct CF {
    int N;               // Number of data points in the cluster
    vector<double> LS;   // Linear sum of the data points
    double SS;           // Squared sum of the data points

    CF(int dimensions) {
        N = 0;
        LS.resize(dimensions, 0.0);
        SS = 0.0;
    }

    // Update the CF by adding a new point
    void addPoint(const vector<double>& point) {
        N++;
        for (size_t i = 0; i < point.size(); ++i) {
            LS[i] += point[i];
        }
        SS += innerProduct(point, point);
    }

    // Compute the centroid of the points in the cluster
    vector<double> centroid() const {
        vector<double> result(LS.size());
        for (size_t i = 0; i < LS.size(); ++i) {
            result[i] = LS[i] / N;
        }
        return result;
    }

    // Compute the radius (cluster compactness)
    double radius() const {
        double sumSqCentroid = innerProduct(centroid(), centroid());
        return sqrt((SS / N) - sumSqCentroid);
    }

    // Helper function to calculate inner product (dot product)
    double innerProduct(const vector<double>& a, const vector<double>& b) const {
        double result = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    }
};

// CF Node (internal node of CF tree)
struct CFNode {
    vector<CF> clusters;
    double threshold;  // Distance threshold to determine if points are added to existing clusters

    CFNode(double t) : threshold(t) {}

    // Add a point to the CF tree
    void addPoint(const vector<double>& point) {
        int closestCluster = -1;
        double closestDistance = numeric_limits<double>::max();

        // Find the closest cluster to the point
        for (size_t i = 0; i < clusters.size(); ++i) {
            double dist = euclideanDistance(clusters[i].centroid(), point);
            if (dist < closestDistance) {
                closestDistance = dist;
                closestCluster = i;
            }
        }

        // If the closest cluster is within the threshold, add the point to it
        if (closestDistance <= threshold) {
            clusters[closestCluster].addPoint(point);
        } else {
            // Otherwise, create a new cluster
            CF newCluster(point.size());
            newCluster.addPoint(point);
            clusters.push_back(newCluster);
        }
    }

    // Helper function to calculate Euclidean distance
    double euclideanDistance(const vector<double>& a, const vector<double>& b) const {
        double dist = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            dist += pow(a[i] - b[i], 2);
        }
        return sqrt(dist);
    }

    // Display the clusters and write to output file
    void displayClusters(ofstream &output) const {
        output << "Clusters:\n";
        for (size_t i = 0; i < clusters.size(); ++i) {
            vector<double> centroid = clusters[i].centroid();
            output << "Cluster " << i + 1 << " Centroid: [";
            for (size_t j = 0; j < centroid.size(); ++j) {
                output << centroid[j];
                if (j < centroid.size() - 1) output << ", ";
            }
            output << "], Radius: " << clusters[i].radius() << "\n";
        }
    }
};

// BIRCH algorithm
void BIRCH(const vector<vector<double>>& data, double threshold, ofstream &output) {
    CFNode cfNode(threshold);

    // Insert points into the CF tree
    for (const auto& point : data) {
        cfNode.addPoint(point);
    }

    // Display the clusters
    cfNode.displayClusters(output);
}

// Function to read dataset from file
vector<vector<double>> readDataset(const string& filename) {
    ifstream input(filename);
    vector<vector<double>> data;
    string line;

    while (getline(input, line)) {
        stringstream ss(line);
        string value;
        vector<double> point;
        
        while (getline(ss, value, ',')) {
            point.push_back(stod(value));
        }
        data.push_back(point);
    }

    input.close();
    return data;
}

int main() {
    // Read dataset from file
    vector<vector<double>> data = readDataset("dataset.txt");

    // Set a threshold for the cluster radius (tuning parameter)
    double threshold = 2.0;

    // Open output file
    ofstream output("output.txt");
    if (!output.is_open()) {
        cerr << "Error: Could not create output.txt" << endl;
        return 1;
    }

    // Run the BIRCH algorithm
    BIRCH(data, threshold, output);

    output.close();
    cout << "Clustering completed. Results saved to output.txt." << endl;

    return 0;
}

