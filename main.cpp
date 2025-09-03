#include <iostream>
#include <fstream>
#include "LightSimulation.hpp"

int main(int argc, char* argv[]) {
    // Initialize world and light source
    Project::World world;
    Project::LightSource light_source(Project::LightSource::SourceType::PlanePolarized, 1.0, 0.0);

    // Load material data from file
    std::ifstream mat_file("materials.txt");
    if (!mat_file.is_open()) {
        std::cerr << "Failed to open materials.txt\n";
        return 1;
    }

    for (int i = 0; i < Project::GRID_SIZE; ++i) {
        for (int j = 0; j < Project::GRID_SIZE; ++j) {
            std::string type;
            double param1, param2, thickness, phi, theta;
            mat_file >> type >> param1 >> param2 >> thickness >> phi >> theta;
            
            std::unique_ptr<Project::PhysicalVolume> phys_vol;
            if (type == "transparent") {
                phys_vol = std::make_unique<Project::TransparentVolume>(param1);
            } else if (type == "translucent") {
                phys_vol = std::make_unique<Project::TranslucentVolume>(param1);
            } else if (type == "uniaxial") {
                phys_vol = std::make_unique<Project::UniaxialCrystalVolume>(param1, param2, phi, theta);
            } else {
                std::cerr << "Unknown material type: " << type << "\n";
                continue;
            }

            auto geom_vol = std::make_unique<Project::PixelVolume>(thickness);
            auto log_vol = std::make_unique<Project::LogicalVolume>(std::move(phys_vol), 
                                                                   std::move(geom_vol), 
                                                                   type + "_pixel_" + std::to_string(i) + "_" + std::to_string(j));
            world.add_pixel_volume(i, j, std::move(log_vol));
        }
    }
    mat_file.close();

    // Simulate
    Project::PhysicalEngine phys_engine(world, light_source);
    auto image = phys_engine.simulate();

    // Save output to file
    std::ofstream out_file("image_data.txt");
    for (const auto& row : image) {
        for (const auto& pixel : row) {
            out_file << pixel[0] << " " << pixel[1] << " " << pixel[2] << "\n";
        }
    }
    out_file.close();

    std::cout << "Simulation complete. Image data saved to image_data.txt\n";
    return 0;
}
