#include<bits/stdc++.h>
#include <cstdlib> // For rand() function
#include <ctime>   // For seeding srand() with current time
#include <fstream>
using namespace std;
int NODE=4;
int MIN_DEGREE=2;
int MAX_DEGREE=3;
void writeAdjacencyToFile(vector<vector<int>>& adjacency) {
    std::ofstream outFile("adjacency_list.txt");

    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    for (size_t i = 0; i < adjacency.size(); ++i) {
        for (size_t j = 0; j < adjacency[i].size(); ++j) {
            outFile << adjacency[i][j] << " ";
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Adjacency list written to 'adjacency_list.txt'." << std::endl;
}

void traverse(int u, bool visited[],vector<vector<int>>&graph) {
   visited[u] = true; //mark v as visited
   for(int v = 0; v<NODE; v++) {
      if(graph[u][v]) {
         if(!visited[v])
            traverse(v, visited,graph);
      }
   }
}
bool isConnected(vector<vector<int>>&graph) {
   bool *vis = new bool[NODE];
   //for all vertex u as start point, check whether all nodes are visible or not
   for(int u; u < NODE; u++) {
      for(int i = 0; i<NODE; i++)
         vis[i] = false; //initialize as no node is visited
         traverse(u,vis,graph);
      for(int i = 0; i<NODE; i++) {
         if(!vis[i]) //if there is a node, not visited by traversal, graph is not connected
            return false;
      }
   }
   return true;
}
int main(int argc, char*argv[]){
    NODE=atoi(argv[1]);
    srand(static_cast<unsigned int>(time(0)));
    while(true){
        vector<vector<int>>adjacency_list(NODE,vector<int>(NODE,0));
        vector<int>degree(NODE,0);
        for(int i=0;i<NODE;i++){
            if(degree[i]<MIN_DEGREE){
                int a=MIN_DEGREE-degree[i];
                int b=MAX_DEGREE-degree[i];
                int randomNum = rand() % (b-a+1) +a ;
                vector<int>select;
                for(int j=0;j<NODE;j++){
                    if(degree[j]<6 && adjacency_list[i][j]==0 && i!=j) select.push_back(j);
                }
                if(randomNum>select.size()) randomNum=select.size()-1;
                for (int k1 = select.size() - 1; k1 > 0; --k1) {
                    int k2 = rand() % (select.size());
                    int temp=select[k1];
                    select[k1]=select[k2];
                    select[k2]=temp;
                }
                vector<int>new_points(select.begin(),select.begin()+randomNum);
                for(int j=0;j<new_points.size();j++){
                    adjacency_list[i][new_points[j]]=1;
                    adjacency_list[new_points[j]][i]=1;
                    degree[i]++;
                    degree[new_points[j]]++;
                }
            }

        }
        if(isConnected(adjacency_list)){
            writeAdjacencyToFile(adjacency_list);
            break;
        }
    }
}