import numpy as np
from scipy.spatial import Voronoi, cKDTree
import scipy

# Configuration
GRID_SIZE = 640  # Size of the pixel grid
NUM_DOMAINS = 200  # Approximate number of domains

# Material types
materials = ['transparent', 'translucent', 'uniaxial']

# Set random seed for reproducibility
# np.random.seed(42)

def generate_voronoi_materials():
    # Generate random seed points for Voronoi tessellation
    points = np.random.uniform(0, GRID_SIZE, (NUM_DOMAINS, 2))  # [x, y] coordinates

    # Create Voronoi diagram
    vor = Voronoi(points)

    # Use cKDTree for efficient nearest-neighbor lookup
    tree = cKDTree(points)

    # Create pixel grid
    x, y = np.meshgrid(np.arange(GRID_SIZE), np.arange(GRID_SIZE))
    pixel_coords = np.vstack((x.ravel(), y.ravel())).T

    # Assign each pixel to the nearest seed point (domain)
    _, domain_indices = tree.query(pixel_coords, k=1)
    domain_map = domain_indices.reshape(GRID_SIZE, GRID_SIZE)

    # Generate random properties for each domain
    domain_properties = []
    for _ in range(NUM_DOMAINS):
        material = np.random.choice(materials)
        if material == 'transparent':
            param1 = np.random.uniform(1.3, 1.4)  # Refractive index
            param2 = 0.0
            phi = 0.0
            theta = 0.0
        elif material == 'translucent':
            param1 = np.random.uniform(0.00001, 0.000015)  # Absorption coefficient
            param2 = 0.0
            phi = 0.0
            theta = 0.0
        else:  # uniaxial
            param1 = np.random.uniform(1.5, 1.52)  # n_o
            param2 = np.random.uniform(1.6, 1.65)  # n_e
            phi = np.random.uniform(0, 6.2831)  # 0 to 2π
            theta = np.random.uniform(0, 3.1415)  # 0 to π
        thickness = np.random.uniform(0.999, 1.001) * 30000  # Thickness in arbitrary units
        domain_properties.append((material, param1, param2, thickness, phi, theta))

    # Write pixel properties to file
    with open('materials.txt', 'w') as f:
        for i in range(GRID_SIZE):
            for j in range(GRID_SIZE):
                domain_idx = domain_map[i, j]
                material, param1, param2, thickness, phi, theta = domain_properties[domain_idx]
                f.write(f"{material} {param1} {param2} {thickness} {phi} {theta}\n")

if __name__ == "__main__":
    generate_voronoi_materials()
    print("Voronoi domain-based material map generated and saved to materials.txt")