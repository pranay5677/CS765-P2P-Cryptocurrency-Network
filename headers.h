#include<bits/stdc++.h>
using namespace std;

class Transaction;
class Block;
class TreeNode;
class Tree;
class Peer;

class Peer{
public:
    int id;
    double hash_power;
    int speed;
    vector<int>neighbours;
    int balance;
    vector<Transaction>all_transactions;
    vector<Block>pending_blocks;
    Tree *tree;
    Peer() {}
    Peer(int id,double hash_power,int speed,int balance){
        this->speed=speed;
        this->id=id;
        this->hash_power=hash_power;
        this->balance=balance;

    }

};
class Transaction{
public:
    int transaction_id;
    int money;
    int sender;
    int receiver;
    double sending_time;
    double receiving_time;
    Transaction () {}
    Transaction(int transaction_id,int money,int sender,int receiver,double sending_time,double receiving_time){
        this->transaction_id=transaction_id;
        this->sender=sender;
        this->money=money;
        this->receiver=receiver;
        this->sending_time=sending_time;
        this->receiving_time=receiving_time;
    }
};
class Block{
public:
    int block_id;
    vector<Transaction>tns;
    double time;
    int miner;
    Block() {}
    Block(int block_id,double time,int miner){
        this->block_id=block_id;
        this->time=time;
        this->miner=miner;
    }
};
class TreeNode {
public:
    int value;
    double time;
    Block* block;
    std::vector<TreeNode*> children;
    TreeNode() {}
    TreeNode(int val, Block* blk, double t) : value(val), block(blk), time(t) {}

    ~TreeNode() {
        for (TreeNode* child : children) {
            delete child;
        }
    }

    // Function to add a child to the node
    void addChild(TreeNode* child) {
        children.push_back(child);
    }
};

class Tree {
public:
    TreeNode* root;

    Tree()  {
        root = new TreeNode(-1,nullptr,0);
    }

    ~Tree() {
        delete root;
    }


    void insert(int parentValue, int childValue, Block* block, int time) {
        if (root == nullptr) {
            // If the tree is empty, create a root node
            root = new TreeNode(parentValue, nullptr, 0); // Root node has no Block and time
        }

        TreeNode* parentNode = findNode(root, parentValue);

        if (parentNode != nullptr) {
            // If the parent node is found, insert the child
            TreeNode* childNode = new TreeNode(childValue, block, time);
            parentNode->addChild(childNode);
        } else {
            std::cout << "Parent node not found. Cannot insert child." << std::endl;
        }
    }

    // Function to find the longest path from root to leaf
    std::vector<int> findLongestPath() {
        std::vector<int> currentPath;
        return findLongestPathHelper(root, currentPath);
    }

    // Helper function to find a node with a specific value
    TreeNode* findNode(TreeNode* current, int value) {
        if (current == nullptr) {
            return nullptr;
        }

        if (current->value == value) {
            return current;
        }

        for (TreeNode* child : current->children) {
            TreeNode* result = findNode(child, value);
            if (result != nullptr) {
                return result;
            }
        }

        return nullptr; // Node not found
    }

    // Helper function to find the longest path recursively
    std::vector<int> findLongestPathHelper(const TreeNode* node, const std::vector<int>& currentPath) {
        if (node == nullptr) {
            return currentPath;
        }

        std::vector<int> newPath = currentPath;
        newPath.push_back(node->value);

        if (node->children.empty()) {
            // If it's a leaf node, return the current path
            return newPath;
        }

        std::vector<int> longestPath;

        for (const TreeNode* child : node->children) {
            std::vector<int> childPath = findLongestPathHelper(child, newPath);
            if (childPath.size() > longestPath.size()) {
                // Choose the path with greater length or, in case of equal length, the path with less time
                longestPath = childPath;
            }
        }

        return longestPath;
    }
     TreeNode* operator[](int value) {
        return findNode(root, value);
    }

};

// int main() {
//     Tree myTree;

//     // Insert nodes
//     myTree.insert(1, 2, nullptr, 0);
//     myTree.insert(1, 3, nullptr, 0);
//     myTree.insert(2, 4, nullptr, 5);
//     myTree.insert(4, 7, nullptr, 5);
//     myTree.insert(2, 5, nullptr, 3);
//     myTree.insert(3, 6, nullptr, 2);

//     std::vector<int> longestPath = myTree.findLongestPath();

//     std::cout << "Longest path from root to leaf: ";
//     for (int value : longestPath) {
//         std::cout << value << " ";
//     }
//     std::cout << std::endl;

//     return 0;
// }