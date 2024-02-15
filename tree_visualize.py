import networkx as nx
import pygraphviz as pgv

def generate_tree(edges):
    G = nx.Graph()
    for edge in edges:
        src, dest = map(int, edge.split('-'))
        G.add_edge(src, dest)
    return G

def visualize_tree(G, filename):
    node_count = len(G.nodes)
    node_size = max(3000 / node_count, 100)  # Scale the node size based on the number of nodes

    A = nx.nx_agraph.to_agraph(G)
    A.graph_attr.update(dpi='300', rankdir='LR')  # Set direction to left-to-right
    A.node_attr.update(style='filled', shape='circle', width=str(node_size), height=str(node_size), fixedsize='true', fillcolor='lightblue')
    for node in A.nodes():
        node.attr['fontsize'] = '10'  # Set font size for node labels

    A.draw(filename, format='png', prog='dot')

def main():
    with open("edge_trees.txt", "r") as file:
        tree_edges = [line.strip() for line in file.readlines()]

    current_tree_edges = []
    tree_count = 0
    for edge in tree_edges:
        if edge:  # Non-empty line
            current_tree_edges.append(edge)
        else:  # Empty line indicates the end of current tree
            if current_tree_edges:
                tree_count += 1
                G = generate_tree(current_tree_edges)
                visualize_tree(G, f"tree_{tree_count}.png")
                current_tree_edges = []

    # Generate tree for the last set of edges if not empty
    if current_tree_edges:
        tree_count += 1
        G = generate_tree(current_tree_edges)
        visualize_tree(G, f"tree_{tree_count}.png")

if __name__ == "__main__":
    main()
