import numpy as np
from PIL import Image

GRID_SIZE = 640

# Read image data
image_data = np.loadtxt('image_data.txt').reshape(GRID_SIZE, GRID_SIZE, 3)

image_data = (image_data * 255).astype(np.uint8)

# Create and save image
image = Image.fromarray(image_data, 'RGB')
image.save('output_image.png')
print("Image saved as output_image.png")
