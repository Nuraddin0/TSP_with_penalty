import time

import matplotlib.pyplot as plt

# Read the input file
filename = "test-input-4.txt"
cities = []

with open(filename, "r") as file:
    for line in file:
        parts = line.strip().split()
        if len(parts) == 3:
            city_id, x, y = parts
            cities.append((int(city_id), int(x), int(y)))

# Extract the coordinates
x_coords = [city[1] for city in cities]
y_coords = [city[2] for city in cities]

# Plotting
plt.figure(figsize=(40, 32))
plt.scatter(x_coords, y_coords, color='blue', marker='o')
plt.text(x, y, str(city_id), fontsize=8, ha='right', va='bottom')

path=[]
fileOut = "output-4.txt"
with open(fileOut, "r") as file2:
    for line in file2:
        id = line.strip()
        path.append(int(id))

print(path)

path_x_coords = []
path_y_coords = []

for city1 in path:
    for city2 in cities:
        if city2[0] == city1:
            path_x_coords.append(city2[1])
            path_y_coords.append(city2[2])
            break

print(path_x_coords)
print(path_y_coords)


dx = [path_x_coords[i+1] - path_x_coords[i] for i in range(len(path_x_coords)-1)]
dy = [path_y_coords[i+1] - path_y_coords[i] for i in range(len(path_y_coords)-1)]

plt.quiver(path_x_coords[:-1], path_y_coords[:-1], dx, dy, angles='xy', scale_units='xy',
                scale=1, color='orange', width=0.003, headwidth=3, headlength=5, alpha=1)

plt.scatter(path_x_coords, path_y_coords, color='red', marker='x')
plt.scatter(path_x_coords[0], path_y_coords[0], color='lime', marker='*', s=400, edgecolors='black', label='Start Node')


'''
path2=[]
fileOut2 = "best_tour_2.txt"
with open(fileOut2, "r") as file2:
    for line in file2:
        id = line.strip()
        path2.append(int(id))

print(path2)

path_x_coords2 = []
path_y_coords2 = []

for city1 in path2:
    for city2 in cities:
        if city2[0] == city1:
            path_x_coords2.append(city2[1])
            path_y_coords2.append(city2[2])
            break

print(path_x_coords2)
print(path_y_coords2)


dx2 = [path_x_coords2[i+1] - path_x_coords2[i] for i in range(len(path_x_coords2)-1)]
dy2 = [path_y_coords2[i+1] - path_y_coords2[i] for i in range(len(path_y_coords2)-1)]

plt.quiver(path_x_coords2[:-1], path_y_coords2[:-1], dx2, dy2, angles='xy', scale_units='xy',
           scale=1, color='green', width=0.003, headwidth=3, headlength=5, alpha=1)

plt.scatter(path_x_coords2, path_y_coords2, color='pink', marker='o')
'''


plt.title("City Coordinates")
plt.xlabel("X Coordinate")
plt.ylabel("Y Coordinate")
plt.grid(True)
plt.show()

