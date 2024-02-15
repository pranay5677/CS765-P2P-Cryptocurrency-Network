#include <bits/stdc++.h>
#include "headers.h"
using namespace std;

ofstream logfile("logfile.txt");
random_device rd;
mt19937 gen(rd());

class TypeEvent;
class Event;

class TypeEvent{
public:
    int type_of_event; 
        // 0 - a transaction is scheduled from sender to reciever at time
        // 1 - a transaction is received at receiver, analyse it and send it to his other connected peers
        // 2 - a block is scheduled to be mined at time if it meets required conditions
        // 3 - a block is received at receiver, analyse it and send it to his other connected peers
    int sender;
    int receiver; // valid for type_of_event = 1 or 3
    double time;

    Transaction transaction;
    Block block;
    
    TypeEvent(int type_of_event,int receiver,double time){
        this->type_of_event=type_of_event;
        this->receiver=receiver;
        this->time=time;
    }
    TypeEvent(int type_of_event,int receiver,double time,Transaction transaction){
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
    int total_nodes;
    double T_tx;
    double z0;
    double z1;
    double I;    
    int transaction_count;
    int block_count;

    vector<Peer> peers;
    vector<vector<int>> adjacency;
    vector<vector<vector<double>>> latencies;
    priority_queue<TypeEvent, vector<TypeEvent>, greater<TypeEvent>> event_queue;

    Event(int total_nodes,double T_tx,double z0,double z1,double I){
        this->total_nodes=total_nodes;
        this->T_tx=T_tx;
        this->z0=z0;
        this->z1=z1;
        this->I=I;
        transaction_count=1;
        block_count=1;

        latencies=vector<vector<vector<double>>>(total_nodes,vector<vector<double>>(total_nodes, vector<double>(3, 0.0)));
        
        read_adjacency_matrix("adjacency_list.txt");
        peers_intialize();
    }
    void read_adjacency_matrix(string filename) {
        ifstream file(filename);
        vector<vector<int>> tempAdjacency;
        string line;

        while(getline(file, line)){
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
        adjacency.resize(numNodes, vector<int>(numNodes, 0));
        for(size_t i = 0; i < numNodes; ++i){
            for(size_t j = 0; j < tempAdjacency[i].size(); ++j){
                adjacency[i][j] = tempAdjacency[i][j];
            }
        }       
    }
    void peers_intialize(){
        peers = vector<Peer>(total_nodes);
        int slow_peers=int((z0*total_nodes)/100);
        int low_cpu_peers=int((z1*total_nodes)/100);

        for(int i=0;i<total_nodes;i++){
            peers[i]=Peer(i,10,1,0);

            for(int j=0;j<total_nodes;j++){
                if(adjacency[i][j]) peers[i].neighbours.push_back(j);
            }
            peers[i].tree=new Tree();
            event_queue.push(generate_transaction(i,0));
            event_queue.push(generate_block(i,0));
        }
        
        vector<int> list_all_nodes;
        for(int i=0;i<total_nodes;i++) list_all_nodes.push_back(i);
        shuffle(list_all_nodes.begin(),list_all_nodes.end(),gen);
        for(int i=0;i<slow_peers;i++){
            peers[list_all_nodes[i]].speed=0;
        }
        shuffle(list_all_nodes.begin(),list_all_nodes.end(),gen);
        for(int i=0;i<low_cpu_peers;i++){
            peers[list_all_nodes[i]].hash_power=1;
        }

        int sum=low_cpu_peers+10*(total_nodes-low_cpu_peers);
        for(int i=0;i<total_nodes;i++){
            peers[i].hash_power/=sum;
        }

        intialize_latencies();
        return;
    }
    void intialize_latencies(){
        for(int i=0;i<total_nodes;i++){
            for(int j=0;j<total_nodes;j++){
                uniform_real_distribution<double> distribution1(10, 500);    // p_ij in ms
                latencies[i][j][0]=distribution1(gen);

                if(peers[i].speed && peers[j].speed) latencies[i][j][1]=100; // c_ij in Mbps
                else latencies[i][j][1]=5;

                latencies[i][j][2]=latencies[i][j][1]/96;                    // inverse of mean of d_ij in ms_inverse
            }
        }
    }
    TypeEvent generate_transaction(int sender, double current_time){
        uniform_int_distribution<int> distribution1(0,total_nodes-2);
        int receiver = distribution1(gen);
        if(receiver==sender) receiver=total_nodes-1;

        exponential_distribution<double> distribution2(1/T_tx);
        int sending_time = distribution2(gen)+current_time;

        Transaction t(transaction_count,-1,sender,receiver);
        transaction_count++;

        TypeEvent E(0,-1,sending_time,t);
        return E;
       
    }
    TypeEvent generate_block(int sender,double t_k){
        vector<int> all_tns_ids;
        vector<int> used_tns_ids;

        for(auto tns:peers[sender].all_transactions) all_tns_ids.push_back(tns.transaction_id);

        vector<int> chain = peers[sender].tree->findLongestPath();
        for(auto i : chain){
            if(i==-1) continue;

            TreeNode *t = peers[sender].tree->findbyvalue(i);
            for(auto j:t->block->tns){
                used_tns_ids.push_back(j.transaction_id);
            }
        }

        vector<int> difference;
        sort(all_tns_ids.begin(), all_tns_ids.end());
        sort(used_tns_ids.begin(), used_tns_ids.end());        
        set_difference(all_tns_ids.begin(), all_tns_ids.end(),used_tns_ids.begin(), used_tns_ids.end(),back_inserter(difference));
        
        if(difference.size()==0){
            Block b(block_count,sender,vector<Transaction>(),chain[chain.size()-1]);
            block_count++;

            exponential_distribution<double> distribution2(peers[sender].hash_power/I);
            double T_k = distribution2(gen);
            TypeEvent E(2,-1,t_k+T_k,b);
            return E;
        }
        else{
            uniform_int_distribution<int> distribution1(0,difference.size());
            int no_of_tns = distribution1(gen);

            shuffle(difference.begin(),difference.end(),gen);
            vector<int> selected_tns_ids(difference.begin(),difference.begin()+no_of_tns);
            vector<Transaction> selected_tns;

            for(auto id:selected_tns_ids){
                for(auto t: peers[sender].all_transactions){
                    if(id==t.transaction_id) selected_tns.push_back(t);
                }
            }
            Block b(block_count,sender,selected_tns,chain[chain.size()-1]);
            block_count++;

            exponential_distribution<double> distribution2(peers[sender].hash_power/I);
            double T_k=distribution2(gen);
            TypeEvent E(2,-1,t_k+T_k,b);
            return E;
        }
    }
    bool receive_forward_transaction(int person,TypeEvent E){
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
    bool receive_forward_block(int person,TypeEvent E){
        bool seen=false;
        int id=E.block.block_id;
        if(peers[person].tree->findbyvalue(id)!=nullptr) seen=true;

        for(auto b: peers[person].pending_blocks){
            if(b.block_id==id){
                seen=true;
                break;
            }
        }
        if(!seen){
            if(peers[person].tree->findbyvalue(E.block.prev_block_id) == nullptr){
                peers[person].pending_blocks.push_back(E.block);
            }
            else{
                vector<int> chain1=peers[person].tree->findLongestPath();
                
                peers[person].tree->insert(E.block.prev_block_id,id,&E.block,E.time);
                queue<int> parent_ids;
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

                vector<Block> updated_pending_blocks;
                for(auto i : peers[person].pending_blocks){
                    if(peers[person].tree->findbyvalue(i.block_id) == nullptr){
                        updated_pending_blocks.push_back(i);
                    }
                }
                peers[person].pending_blocks = updated_pending_blocks;

                vector<int> chain2=peers[person].tree->findLongestPath();
                if(chain2.size()>chain1.size()){
                    event_queue.push(generate_block(person,E.time));
                }
            }
        }        
        return seen;
    }
    void run(){
        logfile<<"Time\tType\tId\tSender\tReceiver"<<endl;

        while(!event_queue.empty()){            
            TypeEvent present_event = event_queue.top();
            event_queue.pop();

            if(present_event.time > 200) break;
            
            if(present_event.type_of_event==0){
                int person = present_event.transaction.sender;

                // cout<<present_event.time<<"\t"<<present_event.transaction.transaction_id<<"\t0\t"<<person<<"\tall"<<endl;

                int available_balance = peers[person].balance;
                uniform_real_distribution<double> distribution1(0,available_balance);
                present_event.transaction.money=distribution1(gen);

                peers[person].balance-=present_event.transaction.money;
                peers[present_event.transaction.receiver].balance+=present_event.transaction.money;

                if(present_event.transaction.money == 0){
                    break;
                }

                peers[person].all_transactions.push_back(present_event.transaction);
                for(auto neighbour:peers[person].neighbours){
                    TypeEvent t=present_event;
                    t.type_of_event=1;
                    t.sender=person;
                    t.receiver=neighbour;

                    exponential_distribution<double> distribution2(latencies[person][neighbour][2]);
                    double rd_var_dij = distribution2(gen);
                    t.time += (latencies[person][neighbour][0]+(8/latencies[person][neighbour][1])+rd_var_dij)*0.001;
                    event_queue.push(t);
                }
                event_queue.push(generate_transaction(person,present_event.time));

            }
            if(present_event.type_of_event==1){
                int person=present_event.receiver;
                
                logfile<<present_event.time<<"\t"<<"Transaction\t"<<present_event.transaction.transaction_id<<"\t"<<present_event.sender<<"\t"<<person<<endl;

                if(!receive_forward_transaction(person,present_event)){
                    for(auto neighbour:peers[person].neighbours){
                        if(neighbour == present_event.sender){
                            continue;
                        }
                        TypeEvent t=present_event;
                        t.receiver=neighbour;

                        exponential_distribution<double> distribution2(latencies[person][neighbour][2]);
                        double rd_var_dij = distribution2(gen);
                        t.time += (latencies[person][neighbour][0]+(8/latencies[person][neighbour][1])+rd_var_dij)*0.001;
                        t.sender=person;
                        event_queue.push(t);
                    }
                }
            }
            if(present_event.type_of_event==2){
                int person=present_event.block.miner;

                // cout<<present_event.time<<"\t"<<"Block\t"<<present_event.block.block_id<<"\t"<<present_event.block.prev_block_id<<"\t2\t"<<person<<"\tall"<<endl;

                vector<int> longest_chain = peers[person].tree->findLongestPath();
                if(longest_chain[longest_chain.size()-1] == present_event.block.prev_block_id){
                    int size_tns = present_event.block.tns.size();

                    for(auto neighbour:peers[person].neighbours){
                        TypeEvent t=present_event;
                        t.type_of_event=3;
                        t.sender=person;
                        t.receiver=neighbour;

                        exponential_distribution<double> distribution2(latencies[person][neighbour][2]);
                        double rd_var_dij = distribution2(gen);
                        t.time += (latencies[person][neighbour][0]+size_tns*(8/latencies[person][neighbour][1])+rd_var_dij)*0.001;
                        event_queue.push(t);
                    }
                    peers[person].tree->insert(present_event.block.prev_block_id,present_event.block.block_id,&present_event.block,present_event.time);
                    peers[person].balance+=50;
                    event_queue.push(generate_block(person,present_event.time));
                }                
            }
            if(present_event.type_of_event==3){
                int person=present_event.receiver;

                logfile<<present_event.time<<"\tBlock\t"<<present_event.block.block_id<<"\t"<<present_event.sender<<"\t"<<person<<endl;

                int size_tns=present_event.block.tns.size();
                if(!receive_forward_block(present_event.receiver,present_event)){
                    for(auto neighbour:peers[person].neighbours){
                        TypeEvent t=present_event;
                        t.type_of_event=3;
                        t.sender = person;
                        t.receiver=neighbour;

                        exponential_distribution<double> distribution2(latencies[person][neighbour][2]);
                        double rd_var_dij = distribution2(gen);
                        t.time += (latencies[person][neighbour][0]+size_tns*(8/latencies[person][neighbour][1])+rd_var_dij)*0.001;
                        event_queue.push(t);
                    }
                }
            }         
        }
    }
};