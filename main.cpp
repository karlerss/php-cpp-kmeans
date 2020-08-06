#include <phpcpp.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <climits>

using namespace std;

class Point{

private:
    int pointId, clusterId;
    int dimensions;
    vector<double> values;

public:
    Point(int id, double x, double y){
        dimensions = 2;
        pointId = id;
        values.push_back(x);
        values.push_back(y);
        clusterId = 0; //Initially not assigned to any cluster
    }

    int getDimensions(){
        return dimensions;
    }

    int getCluster(){
        return clusterId;
    }

    int getID(){
        return pointId;
    }

    void setCluster(int val){
        clusterId = val;
    }

    double getVal(int pos){
        return values[pos];
    }
};



class Cluster{

private:
    int clusterId;
    vector<double> centroid;
    vector<Point> points;

public:
    Cluster(int clusterId, Point centroid){
        this->clusterId = clusterId;
        for(int i=0; i<centroid.getDimensions(); i++){
            this->centroid.push_back(centroid.getVal(i));
        }
        this->addPoint(centroid);
    }

    void addPoint(Point p){
        p.setCluster(this->clusterId);
        points.push_back(p);
    }

    bool removePoint(int pointId){
        int size = points.size();

        for(int i = 0; i < size; i++)
        {
            if(points[i].getID() == pointId)
            {
                points.erase(points.begin() + i);
                return true;
            }
        }
        return false;
    }

    int getId(){
        return clusterId;
    }

    Point getPoint(int pos){
        return points[pos];
    }

    int getSize(){
        return points.size();
    }

    double getCentroidByPos(int pos) {
        return centroid[pos];
    }

    void setCentroidByPos(int pos, double val){
        this->centroid[pos] = val;
    }
};

class KMeans{
private:
    int K, iters, dimensions, total_points;
    vector<Cluster> clusters;

    int getNearestClusterId(Point point){
        double sum = 0.0, min_dist;
        int NearestClusterId;

        for(int i = 0; i < dimensions; i++)
        {
            sum += pow(clusters[0].getCentroidByPos(i) - point.getVal(i), 2.0);
        }

        min_dist = sqrt(sum);
        NearestClusterId = clusters[0].getId();

        for(int i = 1; i < K; i++)
        {
            double dist;
            sum = 0.0;

            for(int j = 0; j < dimensions; j++)
            {
                sum += pow(clusters[i].getCentroidByPos(j) - point.getVal(j), 2.0);
            }

            dist = sqrt(sum);

            if(dist < min_dist)
            {
                min_dist = dist;
                NearestClusterId = clusters[i].getId();
            }
        }

        return NearestClusterId;
    }

public:
    KMeans(int K, int iterations){
        this->K = K;
        this->iters = iterations;
    }

    vector<Cluster> run(vector<Point>& all_points){

        total_points = all_points.size();
        dimensions = all_points[0].getDimensions();


        //Initializing Clusters
        vector<int> used_pointIds;

        for(int i=1; i<=K; i++)
        {
            while(true)
            {
                int index = rand() % total_points;

                if(find(used_pointIds.begin(), used_pointIds.end(), index) == used_pointIds.end())
                {
                    used_pointIds.push_back(index);
                    all_points[index].setCluster(i);
                    Cluster cluster(i, all_points[index]);
                    clusters.push_back(cluster);
                    break;
                }
            }
        }
        //cout<<"Clusters initialized = "<<clusters.size()<<endl<<endl;


        //cout<<"Running K-Means Clustering.."<<endl;

        int iter = 1;
        while(true)
        {
            //cout<<"Iter - "<<iter<<"/"<<iters<<endl;
            bool done = true;

            // Add all points to their nearest cluster
            for(int i = 0; i < total_points; i++)
            {
                int currentClusterId = all_points[i].getCluster();
                int nearestClusterId = getNearestClusterId(all_points[i]);

                if(currentClusterId != nearestClusterId)
                {
                    if(currentClusterId != 0){
                        for(int j=0; j<K; j++){
                            if(clusters[j].getId() == currentClusterId){
                                clusters[j].removePoint(all_points[i].getID());
                            }
                        }
                    }

                    for(int j=0; j<K; j++){
                        if(clusters[j].getId() == nearestClusterId){
                            clusters[j].addPoint(all_points[i]);
                        }
                    }
                    all_points[i].setCluster(nearestClusterId);
                    done = false;
                }
            }

            // Recalculating the center of each cluster
            for(int i = 0; i < K; i++)
            {
                int ClusterSize = clusters[i].getSize();

                for(int j = 0; j < dimensions; j++)
                {
                    double sum = 0.0;
                    if(ClusterSize > 0)
                    {
                        for(int p = 0; p < ClusterSize; p++)
                            sum += clusters[i].getPoint(p).getVal(j);
                        clusters[i].setCentroidByPos(j, sum / ClusterSize);
                    }
                }
            }

            if(done || iter >= iters)
            {
                //cout << "Clustering completed in iteration : " <<iter<<endl<<endl;
                break;
            }
            iter++;
        }

        /*
        //Print pointIds in each cluster
        for(int i=0; i<K; i++){
            cout<<"Points in cluster "<<clusters[i].getId()<<" : ";
            for(int j=0; j<clusters[i].getSize(); j++){
                cout<<clusters[i].getPoint(j).getID()<<" ";
            }
            cout<<endl<<endl;
        }
        cout<<"========================"<<endl<<endl;
        */

        return clusters;
    }
};


Php::Value cpp_kmeans(Php::Parameters &params)
{
    int K = params[0];
    Php::Array results = params[1];

    vector<Point> all_points;

    for (auto &res : results){
        Point point(res.first, res.second[0], res.second[1]);
        all_points.push_back(point);
    }

    int iters = 100;

    KMeans kmeans(K, iters);
    vector<Cluster> clusters = kmeans.run(all_points);

    Php::Array php_clusters;

    for(int i=0; i<K; i++){
        Php::Array this_cluster;
        for(int j=0; j<clusters[i].getSize(); j++){
            Php::Array the_point({clusters[i].getPoint(j).getVal(0), clusters[i].getPoint(j).getVal(1)});
            this_cluster[clusters[i].getPoint(j).getID()] = the_point;
        }
        php_clusters[i] = this_cluster;
    }

    return php_clusters;
}

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("kmeans", "1.0");

        extension.add<cpp_kmeans>("cpp_kmeans", {
            Php::ByVal("k", Php::Type::Numeric),
            Php::ByVal("results", Php::Type::Array)
        });

        return extension;
    }
}
