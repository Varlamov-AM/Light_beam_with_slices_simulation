# Light Beam with Slices Simulation

This project simulates a light beam interacting with material slices, generating a material properties file and producing a final image based on the simulation output. The simulation uses a combination of C++ (compiled via CMake) and Python scripts for material generation and image processing.

## Prerequisites

Before running the simulation, ensure you have the following installed:

- **CMake**: For configuring and building the C++ project.
- **C++ Compiler**: Compatible with your system (e.g., `g++`, `clang`, or `MSVC`).
- **Python 3.6+**: For running the provided Python scripts.
- **Python Libraries**: Install the required Python dependencies listed in `requirements.txt`:
  - `numpy`: For numerical computations.
  - `scipy`: For Voronoi tessellation and spatial algorithms.
  - `Pillow`: For image processing and output.
- **System Dependencies** (if needed for Pillow on Linux):
  ```bash
  sudo apt-get install libjpeg-dev zlib1g-dev  # For Ubuntu/Debian
  ```

## Installation

1. **Set up a Python virtual environment** (recommended to avoid conflicts):
   ```bash
   python3 -m venv venv
   source venv/bin/activate  # On macOS/Linux
   venv\Scripts\activate     # On Windows
   ```

2. **Install Python dependencies**:
   ```bash
   pip3 install -r requirements.txt
   ```

## Running the Simulation

Follow these steps to execute the simulation:

1. **Build the C++ project**:
   - Create a build directory:
     ```bash
     mkdir build && cd build
     ```
   - Configure the CMake project:
     ```bash
     cmake ..
     ```
   - Compile the project:
     ```bash
     cmake --build .
     ```

2. **Generate material properties**:
   - Run the material generation script to create `materials.txt`:
     ```bash
     python3 ../scripts/generate_materials.py
     ```
   - Alternatively, use `scripts/generate_materials_domain.py` for a Voronoi-based material map:
     ```bash
     python3 ../scripts/generate_materials_domain.py
     ```

3. **Run the simulation**:
   - Execute the compiled simulation binary (`LightSimulation` in the `build` directory) with the `materials.txt` file as input. Alternatively, use the provided batch script:
     ```bash
     bash ../scripts/run_simulation_batch.sh
     ```

4. **Generate the final image**:
   - Process the simulation output (`image_data.txt`) to create an image:
     ```bash
     python3 ../scripts/process_image.py
     ```
   - The output image will be saved as `output_image.png` in the `build` directory.

## Project Structure

The project is organized as follows:

```
.
├── build
│   ├── image_data.txt
│   ├── LightSimulation
│   ├── materials.txt
│   ├── output_image.png
├── CMakeLists.txt
├── documentation
│   ├── images
│   ├── physical_model_description.pdf
├── include
│   └── LightSimulation.hpp
├── main.cpp
├── README.md
├── requirements.txt
├── scripts
│   ├── generate_materials_domain.py
│   ├── generate_materials.py
│   ├── process_image.py
│   └── run_simulation_batch.sh
└── src
    └── LightSimulation.cpp
```

### Key Files and Directories
- `CMakeLists.txt`: Configures the C++ project build.
- `scripts/`: Contains Python scripts for material generation and image processing, plus a bash script for batch simulation.
- `build/`: Directory for build artifacts, including the `LightSimulation` binary and output files like `materials.txt` and `output_image.png`.
- `include/`: Contains header files for the C++ simulation.
- `src/`: Contains source files for the C++ simulation.
- `documentation/`: Contains documentation files (see Documentation section).

## Documentation

The project includes detailed documentation in the `documentation/` directory:
- **`physical_model_description.pdf`**: A PDF document describing the physical model used in the simulation, including details about the light beam interaction with material slices, material properties, and the underlying algorithms. Refer to this file for a comprehensive understanding of the simulation's theoretical basis.
- The `images/` subdirectory may contain figures or diagrams referenced in the documentation.

## Notes

- Ensure the `GRID_SIZE` variable in the Python scripts matches the simulation's expected input!
- The `generate_materials.py` script uses a random seed for reproducibility, while `generate_materials_domain.py` can be modified to include a seed for consistent results.
- If you encounter issues with Pillow installation on Linux, ensure system dependencies are installed (see Prerequisites).

## Troubleshooting

- **Pillow installation fails**: Install system dependencies for `libjpeg` and `zlib` (see Prerequisites).
- **CMake errors**: Verify that CMake and a compatible C++ compiler are installed.
- **Python script errors**: Ensure the virtual environment is activated and all dependencies are installed.
- **Simulation errors**: Check that `materials.txt` is correctly generated in the `build` directory before running `LightSimulation`.