import networkx as nx
import matplotlib.pyplot as plt
import pygraphviz as pgv

def generate_tree(edges):
    G = nx.Graph()
    for edge in edges:
        src, dest = map(int, edge.split('-'))
        G.add_edge(src, dest)
    return G

def visualize_tree(G, filename):
    plt.figure(figsize=(10, 6))
    pos = nx.nx_agraph.graphviz_layout(G, prog="dot")
    nx.draw_networkx_nodes(G, pos, node_size=3000, node_color="skyblue", edgecolors="black", linewidths=1)
    nx.draw_networkx_labels(G, pos, font_size=10)
    nx.draw_networkx_edges(G, pos, width=1.0, alpha=0.5)
    plt.axis('off')
    plt.savefig(filename)
    plt.close()

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
