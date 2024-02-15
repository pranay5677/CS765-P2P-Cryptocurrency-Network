#include <bits/stdc++.h>
using namespace std;

class Transaction;
class Block;
class TreeNode;
class Tree;
class Peer;

class Peer{
public:
    int id;
    int balance;
    int speed;         // fast or slow
    double hash_power; // fast_cpu or slow_cpu
    
    vector<int> neighbours;    
    vector<Transaction> all_transactions;
    vector<Block> pending_blocks;

    Tree *tree;

    Peer(){}
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

    Transaction(){}
    Transaction(int transaction_id,int money,int sender,int receiver){
        this->transaction_id=transaction_id;
        this->sender=sender;
        this->money=money;
        this->receiver=receiver;
    }
};

class Block{
public:
    int block_id;
    int prev_block_id;
    int miner;

    vector<Transaction> tns;
    
    Block(){}
    Block(int block_id,int miner,vector<Transaction>tns,int prev_block_id){
        this->block_id=block_id;
        this->miner=miner;
        this->tns=tns;
        this->prev_block_id=prev_block_id;
    }
};

class TreeNode{
public:
    int value;
    double time;
    Block *block;
    std::vector<TreeNode*> children;
    TreeNode(){}
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

class Tree{
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
    TreeNode* findbyvalue(int value) {
        return findNode(root, value);
    }
    void printTree() const {
        printTreeHelper(root, 0);
    }
    void printTreeHelper(const TreeNode* node, int indent) const {
        if (node != nullptr) {
            // Print the current node's value and other information
            std::cout << std::setw(indent * 4) << "";  // Adjust indentation
            std::cout << "Value: " << node->value;
            if (node->block != nullptr) {
                std::cout << " | Block: " /* Print Block information */;
            }
            std::cout << " | Time: " << node->time << std::endl;

            // Recursively print children
            for (const TreeNode* child : node->children) {
                printTreeHelper(child, indent + 1);
            }
        }
    }
    void printAllEdges() const {
        printAllEdgesHelper(root);
    }

    void printAllEdgesHelper(const TreeNode* node) const {
        if (node != nullptr) {
            for (const TreeNode* child : node->children) {
                // Print edge in the format "a-b"
                if(node->value!=-1) std::cout << node->value << "-" << child->value << std::endl;
                else std::cout << "0" << "-" << child->value << std::endl;
                
                // Recursively print edges for child's subtree
                printAllEdgesHelper(child);
            }
        }
    }

};