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

struct CFNode {
    vector<Point> points;
    double centroidX;
    double centroidY;
    int count;

    CFNode() : centroidX(0), centroidY(0), count(0) {}
};

class BIRCH {
public:
    BIRCH(double threshold) : threshold(threshold) {}

    void fit(const vector<Point>& data) {
        for (const auto& point : data) {
            insert(point);
        }
        cluster();
    }

    const vector<CFNode>& get_clusters() const {
        return clusters;
    }

private:
    double threshold;              // Clustering threshold
    vector<CFNode> clusters;       // List of clusters

    void insert(const Point& point) {
        for (auto& cluster : clusters) {
            if (is_within_threshold(cluster, point)) {
                cluster.points.push_back(point);
                update_centroid(cluster);
                return;
            }
        }
        // Create a new cluster if none fits
        CFNode newCluster;
        newCluster.points.push_back(point);
        newCluster.centroidX = point.x;
        newCluster.centroidY = point.y;
        newCluster.count = 1;
        clusters.push_back(newCluster);
    }

    bool is_within_threshold(const CFNode& cluster, const Point& point) {
        double dist = euclidean_distance(Point{cluster.centroidX, cluster.centroidY}, point);
        return dist <= threshold;
    }

    void update_centroid(CFNode& cluster) {
        double totalX = 0;
        double totalY = 0;
        cluster.count++;
        for (const auto& p : cluster.points) {
            totalX += p.x;
            totalY += p.y;
        }
        cluster.centroidX = totalX / cluster.count;
        cluster.centroidY = totalY / cluster.count;
    }

    double euclidean_distance(const Point& p1, const Point& p2) {
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    }

    void cluster() {
        // Further clustering logic can be added here
        // For simplicity, we are just summarizing clusters
    }
};

void load_data(const string& filename, vector<Point>& data) {
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
    load_data("sample_data.txt", data);

    BIRCH birch(1.0); // Set a threshold for clustering
    birch.fit(data);

    const vector<CFNode>& clusters = birch.get_clusters();
    cout << "Clusters:" << endl;
    for (const auto& cluster : clusters) {
        cout << "Cluster with centroid at (" << cluster.centroidX << ", " << cluster.centroidY << ") with points:" << endl;
        for (const auto& point : cluster.points) {
            cout << "(" << point.x << ", " << point.y << ")" << endl;
        }
    }

    return 0;
}
