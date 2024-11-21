#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>

using namespace std;

struct Point {
    double x;
    double y;
};

class KMeans {
public:
    KMeans(int k, int max_iters) : k(k), max_iters(max_iters) {
        srand(static_cast<unsigned>(time(0)));
    }

    void fit(const vector<Point>& data) {
        int n = data.size();
        initialize_centroids(data);
        
        vector<int> labels(n);
        for (int iter = 0; iter < max_iters; ++iter) {
            assign_clusters(data, labels);
            update_centroids(data, labels);
        }
    }

    const vector<Point>& get_centroids() const {
        return centroids;
    }

private:
    int k;                      // Number of clusters
    int max_iters;             // Maximum number of iterations
    vector<Point> centroids;   // Cluster centroids

    void initialize_centroids(const vector<Point>& data) {
        centroids.clear();
        for (int i = 0; i < k; ++i) {
            int index = rand() % data.size();
            centroids.push_back(data[index]);
        }
    }

    void assign_clusters(const vector<Point>& data, vector<int>& labels) {
        for (size_t i = 0; i < data.size(); ++i) {
            double min_dist = numeric_limits<double>::max();
            for (int j = 0; j < k; ++j) {
                double dist = euclidean_distance(data[i], centroids[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    labels[i] = j;
                }
            }
        }
    }

    void update_centroids(const vector<Point>& data, const vector<int>& labels) {
        vector<Point> new_centroids(k, {0.0, 0.0});
        vector<int> counts(k, 0);

        for (size_t i = 0; i < data.size(); ++i) {
            new_centroids[labels[i]].x += data[i].x;
            new_centroids[labels[i]].y += data[i].y;
            counts[labels[i]]++;
        }

        for (int j = 0; j < k; ++j) {
            if (counts[j] != 0) {
                new_centroids[j].x /= counts[j];
                new_centroids[j].y /= counts[j];
            }
        }

        centroids = new_centroids;
    }

    double euclidean_distance(const Point& p1, const Point& p2) {
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
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
    load_data("sample.txt", data);

    KMeans kmeans(2, 100); // 2 clusters, 100 max iterations
    kmeans.fit(data);

    const vector<Point>& centroids = kmeans.get_centroids();
    cout << "Centroids:" << endl;
    for (const auto& centroid : centroids) {
        cout << "(" << centroid.x << ", " << centroid.y << ")" << endl;
    }

    return 0;
}
