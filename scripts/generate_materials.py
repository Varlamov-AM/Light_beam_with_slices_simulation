import numpy as np

GRID_SIZE = 640

# Generate random material data
np.random.seed(42)
materials = ['transparent', 'translucent', 'uniaxial']
with open('materials.txt', 'w') as f:
    for i in range(GRID_SIZE):
        for j in range(GRID_SIZE):
            material = np.random.choice(materials)
            if material == 'transparent':
                param1 = np.random.uniform(1.3, 1.7)  # Refractive index
                param2 = 0.0
            elif material == 'translucent':
                param1 = np.random.uniform(0.01, 0.1)  # Absorption coefficient
                param2 = 0.0
            else:  # uniaxial
                param1 = np.random.uniform(1.5, 1.6)  # n_o
                param2 = np.random.uniform(1.6, 1.7)  # n_e
                phi    = np.random.uniform(0, 6.2831)  
                theta  = np.random.uniform(0, 3.1415)
            thickness  = np.random.uniform(0.9, 1.1) * 30000   # Thickness in arbitrary units
            f.write(f"{material} {param1} {param2} {thickness} {phi} {theta}\n")