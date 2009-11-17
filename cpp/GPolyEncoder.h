#ifndef GPOLYENCODER_H_
#define GPOLYENCODER_H_

#include <vector>
#include <string>
#include <cmath>

class GPolyEncoder {

private:
    int numLevels;
    int zoomFactor;
    double threshold;
    bool forceEndpoints;
    double *zoomLevelBreaks;
    
    void _buildZoomLevelBreaks();
    double distance(std::pair<double,double>& p0, std::pair<double,double>& p1, std::pair<double,double>& p2);
    inline int floor1e5(double coordinate) { return (int)floor(coordinate * 1e5); }
    std::string encodeSignedNumber(int num);
    std::string encodeNumber(int num);
    int computeLevel(double absMaxDist);
    std::auto_ptr<std::pair<std::string, std::string> > encode(std::vector<std::pair<double,double> >& points, const double dists[], double absMaxDist);
    
public:
    GPolyEncoder(int numLevels=18, int zoomFactor=2, double threshold=0.00001, bool forceEndpoints=true);
    ~GPolyEncoder();

    std::auto_ptr<std::pair<std::string, std::string> > dpEncode(std::vector<std::pair<double, double> >& points);
    int getNumLevels() { return numLevels; }
    int getZoomFactor() { return zoomFactor; }
};

#endif /*GPOLYENCODER_H_*/
