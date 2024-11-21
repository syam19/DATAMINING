#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

struct Point {
    double x;
    double y;
};

struct Cluster {
    vector<Point> points;
    double centroidX;
    double centroidY;

    void calculateCentroid() {
        double totalX = 0, totalY = 0;
        for (const auto& point : points) {
            totalX += point.x;
            totalY += point.y;
        }
        centroidX = totalX / points.size();
        centroidY = totalY / points.size();
    }
};

class Chameleon {
public:
    Chameleon(int minPoints, int maxPoints, double mergeThreshold)
        : minPoints(minPoints), maxPoints(maxPoints), mergeThreshold(mergeThreshold) {}

    void fit(vector<Point>& data) {
        // Step 1: Initial clustering using K-Means
        initialClusters = initialClustering(data);
        
        // Step 2: Merge clusters based on interconnectivity and closeness
        mergeClusters();
    }

    const vector<Cluster>& getClusters() const {
        return finalClusters;
    }

private:
    int minPoints;
    int maxPoints;
    double mergeThreshold;
    vector<Cluster> initialClusters;
    vector<Cluster> finalClusters;

    vector<Cluster> initialClustering(vector<Point>& data) {
        // Placeholder for a basic clustering algorithm (e.g., K-Means)
        // For simplicity, we'll just create one cluster
        Cluster cluster;
        for (const auto& point : data) {
            cluster.points.push_back(point);
        }
        cluster.calculateCentroid();
        return {cluster}; // Return single cluster for now
    }

    double euclideanDistance(const Point& p1, const Point& p2) {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    }

    void mergeClusters() {
        finalClusters = initialClusters;

        while (true) {
            bool merged = false;
            for (size_t i = 0; i < finalClusters.size(); ++i) {
                for (size_t j = i + 1; j < finalClusters.size(); ++j) {
                    if (shouldMerge(finalClusters[i], finalClusters[j])) {
                        merge(finalClusters[i], finalClusters[j]);
                        finalClusters.erase(finalClusters.begin() + j);
                        merged = true;
                        break; // Restart loop as sizes have changed
                    }
                }
                if (merged) break;
            }
            if (!merged) break;
        }
    }

    bool shouldMerge(const Cluster& c1, const Cluster& c2) {
        double dist = euclideanDistance(Point{c1.centroidX, c1.centroidY}, 
                                         Point{c2.centroidX, c2.centroidY});
        return dist <= mergeThreshold;
    }

    void merge(Cluster& c1, Cluster& c2) {
        c1.points.insert(c1.points.end(), c2.points.begin(), c2.points.end());
        c1.calculateCentroid(); // Recalculate centroid
    }
};

void loadData(const string& filename, vector<Point>& data) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        Point point;
        if (ss >> point.x >> point.y) {
            data.push_back(point);
        }
    }
}

int main() {
    vector<Point> data;
    loadData("sample.txt", data);

    Chameleon chameleon(3, 10, 2.0); // minPoints, maxPoints, mergeThreshold
    chameleon.fit(data);

    const vector<Cluster>& clusters = chameleon.getClusters();
    cout << "Clusters:" << endl;
    for (const auto& cluster : clusters) {
        cout << "Cluster with centroid at (" << cluster.centroidX << ", " << cluster.centroidY << ") with points:" << endl;
        for (const auto& point : cluster.points) {
            cout << "(" << point.x << ", " << point.y << ")" << endl;
        }
    }

    return 0;
}
