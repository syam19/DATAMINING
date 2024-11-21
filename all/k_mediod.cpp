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

class KMedoids {
public:
    KMedoids(int k, int max_iters) : k(k), max_iters(max_iters) {
        srand(static_cast<unsigned>(time(0)));
    }

    void fit(const vector<Point>& data) {
        int n = data.size();
        initialize_medoids(data);
        
        vector<int> labels(n);
        for (int iter = 0; iter < max_iters; ++iter) {
            assign_clusters(data, labels);
            update_medoids(data, labels);
        }
    }

    const vector<Point>& get_medoids() const {
        return medoids;
    }

private:
    int k;                      // Number of clusters
    int max_iters;             // Maximum number of iterations
    vector<Point> medoids;     // Cluster medoids

    void initialize_medoids(const vector<Point>& data) {
        medoids.clear();
        for (int i = 0; i < k; ++i) {
            int index = rand() % data.size();
            medoids.push_back(data[index]);
        }
    }

    void assign_clusters(const vector<Point>& data, vector<int>& labels) {
        for (size_t i = 0; i < data.size(); ++i) {
            double min_dist = numeric_limits<double>::max();
            for (int j = 0; j < k; ++j) {
                double dist = euclidean_distance(data[i], medoids[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    labels[i] = j;
                }
            }
        }
    }

    void update_medoids(const vector<Point>& data, const vector<int>& labels) {
        vector<Point> new_medoids(medoids);
        
        for (int j = 0; j < k; ++j) {
            double min_cost = numeric_limits<double>::max();
            Point best_medoid = medoids[j];
            
            for (size_t i = 0; i < data.size(); ++i) {
                if (labels[i] == j) {
                    double cost = 0.0;
                    for (size_t m = 0; m < data.size(); ++m) {
                        if (labels[m] == j) {
                            cost += euclidean_distance(data[i], data[m]);
                        }
                    }
                    if (cost < min_cost) {
                        min_cost = cost;
                        best_medoid = data[i];
                    }
                }
            }
            new_medoids[j] = best_medoid;
        }
        medoids = new_medoids;
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

    KMedoids kmedoids(2, 100); // 2 clusters, 100 max iterations
    kmedoids.fit(data);

    const vector<Point>& medoids = kmedoids.get_medoids();
    cout << "Medoids:" << endl;
    for (const auto& medoid : medoids) {
        cout << "(" << medoid.x << ", " << medoid.y << ")" << endl;
    }

    return 0;
}
