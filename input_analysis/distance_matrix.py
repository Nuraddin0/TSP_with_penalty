import math

# Your provided distance function
def distance(a, b):
    dx = a[0] - b[0]
    dy = a[1] - b[1]
    return round(math.sqrt(dx * dx + dy * dy))

# Read city coordinates from file
filename = "example-input-2.txt"
points = []

with open(filename, "r") as file:
    for line in file:
        parts = line.strip().split()
        if len(parts) == 3:
            _, x, y = parts
            points.append((int(x), int(y)))

n = len(points)
distance_matrix = [[0] * n for _ in range(n)]

# Compute only the upper triangle
for i in range(n):
    for j in range(i + 1, n):
        d = distance(points[i], points[j])
        distance_matrix[i][j] = d
        distance_matrix[j][i] = d  # Symmetric

# Pretty print the matrix
print("\nDistance matrix (rounded Euclidean, symmetric):\n")
header = "     " + " ".join([f"{j:5}" for j in range(n)])
print(header)
print("-" * len(header))

for i, row in enumerate(distance_matrix):
    row_str = " ".join([f"{d:5}" for d in row])
    print(f"{i:3} | {row_str}")
