import numpy as np
import matplotlib.pyplot as plt

# Read vectors from file
with open("vectors.txt", "r") as file:
    lines = file.readlines()
    hash_powers = np.array(list(map(float, lines[0].split())))
    speeds = np.array(list(map(int, lines[1].split())))
    fractions = np.array(list(map(float, lines[2].split())))

# Plotting
plt.figure(figsize=(8, 6))

# Plot points or circles for each peer
n = len(hash_powers)
for i in range(n):
    color = 'red' if speeds[i] == 1 else 'blue'  # Color based on speed
    plt.scatter(i + 0.5, fractions[i], c=color, s=100, alpha=0.7)  # Adjust x-position to add padding

plt.xticks(np.arange(n) + 0.5, range(n))  # Set x-ticks with padding
plt.xlabel('Peer ID')
plt.ylabel('Fraction of Blocks')
plt.title('Peer Fractions of Blocks with Speeds')
plt.grid(True)
fast_legend = plt.Line2D([0], [0], marker='o', color='w', markerfacecolor='red', markersize=10, label='Fast')
slow_legend = plt.Line2D([0], [0], marker='o', color='w', markerfacecolor='blue', markersize=10, label='Slow')
plt.legend(handles=[fast_legend, slow_legend], loc='upper left')
plt.tight_layout()
plt.savefig("stats.png")
