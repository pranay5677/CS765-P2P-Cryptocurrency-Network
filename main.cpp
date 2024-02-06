#include<bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <random>
#include"headers.h"
using namespace std;
class TypeEvent;

class TypeEvent{
    public:
    int type_of_event; 
    // 0-scheduling-trasaction
    // 1-send-receive-trasaction
    // 2 -generate block
    // 3-receive block
    int receiver; // if type_of_trasaction=1 or receive block
    double time;
    Transaction  transaction;
    Block block;
    TypeEvent(int type_of_event,int receiver,double time){
        this->type_of_event=type_of_event;
        this->receiver=receiver;
        this->time=time;
    }
    TypeEvent(int type_of_event,int receiver,double time,Transaction transaction ){
        this->type_of_event=type_of_event;
        this->receiver=receiver;
        this->time=time;
        this->transaction=transaction;
    }
    TypeEvent(int type_of_event,int receiver,double time,Block block){
        this->type_of_event=type_of_event;
        this->receiver=receiver;
        this->time=time;
        this->block=block;
    }

    bool operator>(const TypeEvent& other) const {
        return time > other.time;
    }

};

class Event{
public:
    vector<Peer>peers;
    int total_nodes;
    vector<vector<int>>adjacency;
    vector<vector<vector<double>>>latencies;
    int trasaction_count;
    int block_count;
    priority_queue<TypeEvent, vector<TypeEvent>, greater<TypeEvent>> event_queue;

    Event(int total_nodes){
        this->total_nodes=total_nodes;
        latencies=vector<vector<vector<double>>>(total_nodes,vector<vector<double>>(total_nodes, vector<double>(3, 0.0)));
        trasaction_count=1;
        block_count=1;
        read_adjacency_matrix(adjacency,"adjacency_list.txt");
        peers_intialize();
    }
    void read_adjacency_matrix(std::vector<std::vector<int>>& adjacency, string filename) {
        ifstream file(filename);
        vector<vector<int>> tempAdjacency;
        string line;
        while (getline(file, line)) {
            vector<int> row;
            istringstream iss(line);
            int value;
            while (iss >> value) {
                row.push_back(value);
            }
            tempAdjacency.push_back(row);
        }
        file.close();
        size_t numNodes = tempAdjacency.size();
        adjacency.resize(numNodes, std::vector<int>(numNodes, 0));
        for (size_t i = 0; i < numNodes; ++i) {
            for (size_t j = 0; j < tempAdjacency[i].size(); ++j) {
                adjacency[i][j] = tempAdjacency[i][j];
            }
        }
        
    }
    void peers_intialize(){
        peers=vector<Peer>(total_nodes);
        for(int i=0;i<total_nodes;i++){
            double t=0;
            peers[i]=Peer(i,t,1,100);
            for(int j=0;j<total_nodes;j++){
                if(adjacency[i][j]) peers[i].neighbours.push_back(j);
            }
            peers[i].tree=new Tree();
            // event_queue.push(generate_trasaction(i));
            event_queue.push(generate_block(i,0));
        }
        intialize_latencies();
        return ;
    }
    void intialize_latencies(){
        random_device rd;
        mt19937 gen(rd());
        for(int i=0;i<total_nodes;i++){
            for(int j=0;j<total_nodes;j++){
                uniform_real_distribution<double> distribution1(10, 500);
                latencies[i][j][0]=distribution1(gen);

                if(peers[i].speed && peers[j].speed) latencies[i][j][1]=100;
                else latencies[i][j][1]=5;

                exponential_distribution<double> distribution2(96 /latencies[i][j][1]);
                latencies[i][j][2]=distribution2(gen);

            }
        }
    }
    TypeEvent generate_trasaction(int sender){
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> distribution1(0,total_nodes-2);
        int receiver=distribution1(gen);
        if(receiver==sender) receiver=total_nodes-1;
        exponential_distribution<double> distribution2(0.01);
        int sending_time=distribution2(gen);
        Transaction t(trasaction_count,-1,sender,receiver,sending_time,-1);
        trasaction_count++;
        TypeEvent E(0,-1,sending_time,t);
        return E;
       
    }
    TypeEvent generate_block(int sender,double t_k){
        random_device rd;
        mt19937 gen(rd());
        vector<int>all_tns_ids;
        vector<int>used_tns_ids;

        for(auto tns:peers[sender].all_transactions) all_tns_ids.push_back(tns.transaction_id);

        vector<int>chain=peers[sender].tree->findLongestPath();
        for(auto i : chain){
            if(i==-1) continue;
            TreeNode * t= peers[sender].tree->findbyvalue(i);
            for(auto j:t->block->tns){
                used_tns_ids.push_back(j.transaction_id);
            }
        }
        sort(all_tns_ids.begin(), all_tns_ids.end());
        sort(used_tns_ids.begin(), used_tns_ids.end());
        vector<int> difference;
        set_difference(all_tns_ids.begin(), all_tns_ids.end(),
                        used_tns_ids.begin(), used_tns_ids.end(),
                        std::back_inserter(difference));
        
        if(difference.size()==0){
            Block b(block_count,sender,vector<Transaction>(),chain[chain.size()-1]);
            block_count++;
            exponential_distribution<double> distribution2(0.01);
            double T_k=distribution2(gen);
            TypeEvent E(2,-1,t_k+T_k,b);
            return E;

        }
        else{
            uniform_real_distribution<double> distribution1(0,difference.size());
            int no_of_elements=distribution1(gen);
            shuffle(difference.begin(),difference.end(),gen);
            vector<int>selected_tns_ids(difference.begin(),difference.begin()+no_of_elements);
            vector<Transaction>selected_tns;
            for(auto id:selected_tns_ids){
                for(auto t: peers[sender].all_transactions){
                    if(id==t.transaction_id) selected_tns.push_back(t);
                }
            }
            Block b(block_count,sender,selected_tns,chain[chain.size()-1]);
            block_count++;
            exponential_distribution<double> distribution2(0.01);
            double T_k=distribution2(gen);
            TypeEvent E(2,-1,t_k+T_k,b);
            return E;
        }


    }
    bool receive_forward_trasaction(int person,TypeEvent E){
        bool seen=false;
        Transaction received_tn=E.transaction;
        for(auto t:peers[person].all_transactions){
            if(t.transaction_id==received_tn.transaction_id){
                seen =true;
                break;
            }
        }
        if(!seen){
            peers[person].all_transactions.push_back(received_tn);

        }
        return seen;
    }
    bool recive_forward_block(int person,TypeEvent E){
        bool seen=false;
        int id=E.block.block_id;
        if(peers[person].tree->findbyvalue(id)!=nullptr) seen =true;
        for(auto b: peers[person].pending_blocks){
            if(b.block_id==id){
                seen=true;
                break;
            }
        }
        if(!seen){
            if(peers[person].tree->findbyvalue(E.block.prev_block_id)==nullptr){
                peers[person].pending_blocks.push_back(E.block);
            }
            else{
                vector<int>chain1=peers[person].tree->findLongestPath();
                
                peers[person].tree->insert(E.block.prev_block_id,id,&E.block,E.time);
                queue<int>parent_ids;
                parent_ids.push(id);
                while(!parent_ids.empty()){
                    int parent=parent_ids.front();
                    parent_ids.pop();
                    for(auto b:peers[person].pending_blocks){
                        if(b.prev_block_id==parent){
                            peers[person].tree->insert(parent,b.block_id,&b,E.time);
                            parent_ids.push(b.block_id);
                        }
                    }
                }

                vector<int>chain2=peers[person].tree->findLongestPath();
                if(chain2.size()>chain1.size()){
                    event_queue.push(generate_block(person,E.time));
                }


            }
        }
        
        return seen;

    }
    void run(){
        random_device rd;
        mt19937 gen(rd());
        // cout<<"time\tTransaction id\ttype\tsender\treceiver"<<endl;
        cout<<"time\tblock id\tprev_block_id\ttype\tsender\treceiver"<<endl;

        while(!event_queue.empty()){
            
            TypeEvent present_event= event_queue.top();

            if(present_event.time > 400) break;

            event_queue.pop();

            if(present_event.type_of_event==0){
                int person=present_event.transaction.sender;
                // cout<<present_event.time<<"\t"<<present_event.transaction.transaction_id<<"\t0\t"<<person<<"\tall"<<endl;

                mt19937 gen(rd());
                int available_balance=peers[person].balance;
                uniform_real_distribution<double> distribution1(0,available_balance);
                present_event.transaction.money=distribution1(gen);

                peers[person].balance-=present_event.transaction.money;
                peers[present_event.transaction.receiver].balance+=present_event.transaction.money;

                peers[person].all_transactions.push_back(present_event.transaction);
                for(auto neighbour:peers[person].neighbours){
                    TypeEvent t=present_event;
                    t.type_of_event=1;
                    t.receiver=neighbour;
                    t.time=present_event.time+(latencies[person][neighbour][0]+(8/latencies[person][neighbour][1])+latencies[person][neighbour][2])*0.001;
                    event_queue.push(t);
                }
                // event_queue.push(generate_trasaction(person));

            }
            if(present_event.type_of_event==1){
                int person=present_event.receiver;
                // cout<<present_event.time<<"\t"<<present_event.transaction.transaction_id<<"\t1\t"<<"-1"<<"\t"<<person<<endl;
                if(!receive_forward_trasaction(person,present_event)){
                    for(auto neighbour:peers[person].neighbours){
                        TypeEvent t=present_event;
                        t.type_of_event=1;
                        t.receiver=neighbour;
                        t.time=present_event.time+(latencies[person][neighbour][0]+(8/latencies[person][neighbour][1])+latencies[person][neighbour][2])*0.001;
                        event_queue.push(t);
                    }
                }
            }
            if(present_event.type_of_event==2){
                int person=present_event.block.miner;
                // cout<<present_event.time<<"\t"<<present_event.block.block_id<<"\t"<<present_event.block.prev_block_id<<"\t2\t"<<person<<"\tall"<<endl;
                vector<int>longest_chain=peers[person].tree->findLongestPath();
                if(longest_chain[longest_chain.size()-1]==present_event.block.prev_block_id){
                    int size_tns=present_event.block.tns.size();
                    for(auto neighbour:peers[person].neighbours){
                        TypeEvent t=present_event;
                        t.type_of_event=3;
                        t.receiver=neighbour;
                        t.time=present_event.time+(latencies[person][neighbour][0]+size_tns*(8/latencies[person][neighbour][1])+latencies[person][neighbour][2])*0.001;
                        event_queue.push(t);
                    }
                    peers[person].tree->insert(present_event.block.prev_block_id,present_event.block.block_id,&present_event.block,
                            present_event.time);
                    peers[person].balance+=50;
                    event_queue.push(generate_block(person,present_event.time));
                }
                
            }
            if(present_event.type_of_event==3){
                int person=present_event.receiver;
                // cout<<present_event.time<<"\t"<<present_event.block.block_id<<"\t"<<present_event.block.prev_block_id<<"\t3\t"<<"-1\t"<<person<<endl;
                int size_tns=present_event.block.tns.size();
                if(!recive_forward_block(present_event.receiver,present_event)){
                    for(auto neighbour:peers[person].neighbours){
                        TypeEvent t=present_event;
                        t.type_of_event=3;
                        t.receiver=neighbour;
                        t.time=present_event.time+(latencies[person][neighbour][0]+size_tns*(8/latencies[person][neighbour][1])+latencies[person][neighbour][2])*0.001;
                        event_queue.push(t);
                    }
                }
            }
         
        }
        for(int i=0;i<total_nodes;i++){
            peers[i].tree->printTree();
            cout<<peers[i].balance<<endl;
        }

    }
};

int main(){
    srand(static_cast<unsigned int>(time(0)));
    exponential_distribution<double> exponentialDist(1.0 / 5);
    Event e(4);
    e.run();
}
