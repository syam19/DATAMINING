#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>

using namespace std;

struct Point {
    double x;
    double y;
    bool visited = false;
    int cluster_id = -1; // -1 indicates noise
};

class DBSCAN {
public:
    DBSCAN(double epsilon, int minPts)
        : epsilon(epsilon), minPts(minPts), clusterId(0) {}

    void fit(vector<Point>& data) {
        for (size_t i = 0; i < data.size(); ++i) {
            if (!data[i].visited) {
                data[i].visited = true;
                vector<int> neighbors = regionQuery(data, i);

                if (neighbors.size() < minPts) {
                    data[i].cluster_id = -1; // Mark as noise
                } else {
                    clusterId++;
                    expandCluster(data, i, neighbors);
                }
            }
        }
    }

    void printClusters(const vector<Point>& data) {
        for (const auto& point : data) {
            cout << "(" << point.x << ", " << point.y << ") -> Cluster ID: " << point.cluster_id << endl;
        }
    }

private:
    double epsilon;
    int minPts;
    int clusterId;

    vector<int> regionQuery(const vector<Point>& data, int pointIdx) {
        vector<int> neighbors;
        for (size_t i = 0; i < data.size(); ++i) {
            if (i != pointIdx && distance(data[pointIdx], data[i]) <= epsilon) {
                neighbors.push_back(i);
            }
        }
        return neighbors;
    }

    double distance(const Point& p1, const Point& p2) {
        return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    }

    void expandCluster(vector<Point>& data, int pointIdx, vector<int>& neighbors) { // Changed to non-const
        data[pointIdx].cluster_id = clusterId;

        for (size_t i = 0; i < neighbors.size(); ++i) {
            int neighborIdx = neighbors[i];

            if (!data[neighborIdx].visited) {
                data[neighborIdx].visited = true;
                vector<int> newNeighbors = regionQuery(data, neighborIdx);

                if (newNeighbors.size() >= minPts) {
                    neighbors.insert(neighbors.end(), newNeighbors.begin(), newNeighbors.end());
                }
            }

            if (data[neighborIdx].cluster_id == -1) {
                data[neighborIdx].cluster_id = clusterId; // Change noise to border point
            }
        }
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
    loadData("sample_data1.txt", data);

    DBSCAN dbscan(1.0, 2); 
    dbscan.fit(data);
    dbscan.printClusters(data);

    return 0;
}
