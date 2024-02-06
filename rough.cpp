#include<bits/stdc++.h>
using namespace std;
class Block;
class TreeNode {
public:
    int value;
    double time;
    Block *block;
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
};
int main() {
    // Example usage of the Tree class
    Tree myTree;
    myTree.insert(-1, 1, nullptr, 0);
    myTree.insert(1, 2, nullptr, 1);
    myTree.insert(1, 3, nullptr, 2);

    // Print the tree
    myTree.printTree();

    return 0;
}