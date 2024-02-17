#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <random>
#include "event.h"
using namespace std;

ofstream logfile("logfile.txt");

int main(int argc, char* argv[]){
    
    if(!logfile.is_open()){cout<<"error"<<endl; return 1;}
    unordered_map<string, string> args;

    // Parsing command-line arguments
    for(int i = 1; i < argc; ++i){
        string arg = argv[i];

        if(arg.substr(0, 2) == "--"){
            // Remove leading "--"
            arg = arg.substr(2);

            // Split argument into key and value
            size_t equalPos = arg.find('=');
            string key = arg.substr(0, equalPos);
            string value = (equalPos != string::npos) ? arg.substr(equalPos + 1) : "";

            // Store key-value pair in the map
            args[key] = value;
        }
    }

    // Command line arguments == simulator parameters:
    // n    total_nodes
    // z0   percent of slow peers
    // z1   percent of low CPU
    // T_tx Mean of time between transactions
    // p_ij light of speed delay
   
    // Retrieve values from the parsed arguments
    int n       = stoi(args["n"]);
    int z0      = stoi(args["z0"]);
    int z1      = stoi(args["z1"]);
    double T_tx = stod(args["T_tx"]);
    double I    = stod(args["I"]);

    cout << "n: " << n << "\n";
    cout << "z0: " << z0 << "\n";
    cout << "z1: " << z1 << "\n";
    cout << "T_tx: " << T_tx << "\n";
    cout << "I: " << I << "\n";

    srand(static_cast<unsigned int>(time(0)));

    Event e(n,T_tx,z0,z1,I,logfile);
    e.run();

    ofstream outputFile("edge_trees.txt", ios::app);
   
    for(int i = 0; i < n; ++i){
        streambuf* original = cout.rdbuf(outputFile.rdbuf());
        e.peers[i].tree->printAllEdges();
        cout<<"\n\n";
        cout.rdbuf(original);
    }

    vector<double> hash_powers(n);
    vector<int> speeds(n);
    vector<double> fractions(n, 0);

    vector<int> chain = e.peers[0].tree->findLongestPath();
    int count = 0;

    for(auto i: chain){
        if(i==-1) continue;

        TreeNode *node = e.peers[0].tree->findbyvalue(i);
        fractions[node->block->miner] += 1;
        count++;
    }
    for(int i=0; i<n; i++){
        speeds[i] = e.peers[i].speed;
        hash_powers[i] = e.peers[i].hash_power;
        fractions[i] /= count;
    }

    ofstream vec("vectors.txt");
    if(vec.is_open()) {
        for(double power : hash_powers) {
            vec << power << " ";
        }
        vec << endl;

        for(int speed : speeds) {
            vec << speed << " ";
        }
        vec << endl;

        for(double fraction : fractions) {
            vec << fraction << " ";
        }
        vec << endl;
    }
    else{
        cerr << "Unable to open file for writing!" << endl;
        return 1;
    }
    sort(hash_powers.begin(),hash_powers.end());
    double a=0,b=0,c=0,d=0;
    for(int i=0; i<n; i++){
        if(e.peers[i].speed && e.peers[i].hash_power==hash_powers[hash_powers.size()-1]){
            a+=fractions[i];
        }
        if(!e.peers[i].speed && e.peers[i].hash_power==hash_powers[hash_powers.size()-1]){
            b+=fractions[i];
            
        }if(e.peers[i].speed && e.peers[i].hash_power!=hash_powers[hash_powers.size()-1]){
            c+=fractions[i];
            
        }if(!e.peers[i].speed && e.peers[i].hash_power!=hash_powers[hash_powers.size()-1]){
            d+=fractions[i];
            
        }
    }
    // cout<<"Length of longest chain: "<<e.peers[0].tree->findLongestPath().size()<<endl;
    // cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<endl;

    vec.close();
    logfile.close();
    outputFile.close();   
}
