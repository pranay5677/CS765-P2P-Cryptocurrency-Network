from graphviz import Digraph

class TreeNode:
    def __init__(self, value, block, time):
        self.value = value
        self.block = block
        self.time = time
        self.children = []

    def add_child(self, child):
        self.children.append(child)

class Tree:
    def __init__(self):
        self.root = None

    def insert(self, parent_value, child_value, block, time):
        if self.root is None:
            self.root = TreeNode(parent_value, None, 0)

        parent_node = self.find_node(self.root, parent_value)

        if parent_node is not None:
            child_node = TreeNode(child_value, block, time)
            parent_node.add_child(child_node)
        else:
            print("Parent node not found. Cannot insert child.")

    def find_node(self, current, value):
        if current is None:
            return None

        if current.value == value:
            return current

        for child in current.children:
            result = self.find_node(child, value)
            if result is not None:
                return result

        return None

    def find_longest_path(self):
        current_path = []
        return self.find_longest_path_helper(self.root, current_path)

    def find_longest_path_helper(self, node, current_path):
        if node is None:
            return current_path

        new_path = current_path + [node.value]

        if not node.children:
            return new_path

        longest_path = []

        for child in node.children:
            child_path = self.find_longest_path_helper(child, new_path)
            if len(child_path) > len(longest_path):
                longest_path = child_path

        return longest_path

    def print_tree(self):
        self.print_tree_helper(self.root, 0)

    def print_tree_helper(self, node, indent):
        if node is not None:
            print(" " * (indent * 4) + f"Value: {node.value} | Time: {node.time}")

            for child in node.children:
                self.print_tree_helper(child, indent + 1)

def visualize_tree(tree):
    dot = Digraph(comment="Tree Visualization")
    visualize_tree_helper(dot, tree.root)
    dot.render('tree', format='png', cleanup=True)

def visualize_tree_helper(dot, node):
    if node is not None:
        dot.node(str(node.value), label=f"Value: {node.value}\nTime: {node.time}")

        for child in node.children:
            dot.edge(str(node.value), str(child.value))
            visualize_tree_helper(dot, child)

def read_tree_from_file(file_path):
    tree = Tree()

    with open(file_path, 'r') as file:
        for line in file:
            values = line.strip().split('|')

            # Check if the line has enough elements
            if len(values) >= 3:
                parent_value = int(values[0].split(':')[-1].strip())
                child_value = int(values[1].split(':')[-1].strip())
                block = None  # Add code to extract Block information if needed
                time = int(values[2].split(':')[-1].strip())

                tree.insert(parent_value, child_value, block, time)
            else:
                print(f"Ignoring line: {line.strip()} - Not enough elements")

    return tree

if __name__ == "__main__":
    # Replace 'tree.txt' with the actual path to your text file containing the tree structure
    tree_file_path = 'tree.txt'
    my_tree = read_tree_from_file(tree_file_path)

    print("Tree Structure:")
    my_tree.print_tree()

    visualize_tree(my_tree)
    print("Tree visualization created as 'tree.png'")
