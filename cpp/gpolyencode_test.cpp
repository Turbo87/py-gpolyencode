#include "GPolyEncoder.h"

#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

string replace_slashes(string s) {
    // if we're printing JSON-compatible output
    // we need to encode \ properly...
    string::size_type pos = 0;
    while ((pos=s.find("\\",pos)) != string::npos) {
        s.replace(pos, 2, "\\\\");
        pos += 2;
    }
    return s;
}

int main(int argc, const char* argv[]) {
    vector<pair<double, double> > trk;

    trk.push_back(pair<double, double>(8.94328,52.29834));
    trk.push_back(pair<double, double>(8.93614,52.29767));
    trk.push_back(pair<double, double>(8.93301,52.29322));
    trk.push_back(pair<double, double>(8.93036,52.28938));
    trk.push_back(pair<double, double>(8.97475,52.27014));

    GPolyEncoder *pe = new GPolyEncoder();
    
    auto_ptr<pair<string, string> > enc;
    enc = pe->dpEncode(trk);
    cout << '"' << replace_slashes(enc->first) << '"' << endl << '"' << enc->second << '"' << endl;
    
    enc = pe->dpEncode(trk);
    cout << '"' << replace_slashes(enc->first) << '"' << endl << '"' << enc->second << '"' << endl;

    trk.clear();
    
    string line;
    ifstream f_data("gpolyencode_test_geom.txt");
    getline(f_data, line);
    int nCoords = atoi(line.c_str());
    cout << "Loading " << nCoords << " coordinates..." << endl;
    
    for (int i=0; i<nCoords; i++) {
        string c;
        getline(f_data, c, ',');
        double x = atof(c.c_str());
        getline(f_data, c, '\n');
        double y = atof(c.c_str());
        trk.push_back(pair<double, double>(x, y));
    }
    
    clock_t t0, t1;
    t0 = clock();
    enc = pe->dpEncode(trk);
    t1 = clock();
    cout << ((double) (t1 - t0)) / CLOCKS_PER_SEC << "s" << endl;

    if (argc >= 2 && string(argv[1]) == "-v") {
        cout << '"' << replace_slashes(enc->first) << '"' << endl << '"' << enc->second << '"' << endl;
    } else {
        cout << "(use -v to print it)" << endl;
    }
    
    delete pe;
    return 0;
}
