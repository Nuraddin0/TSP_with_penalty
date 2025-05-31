import matplotlib.pyplot as plt

# Read the input file
filename = "example-input-1.txt"
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
plt.figure(figsize=(20, 16))
plt.scatter(x_coords, y_coords, color='blue', marker='o')

# Label each point with its city ID
#for city_id, x, y in cities:
 #   plt.text(x, y, str(city_id), fontsize=8, ha='right', va='bottom')

plt.title("City Coordinates")
plt.xlabel("X Coordinate")
plt.ylabel("Y Coordinate")
plt.grid(True)
plt.show()
