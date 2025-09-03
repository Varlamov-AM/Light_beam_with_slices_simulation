#include "LightSimulation.hpp"
#include <cmath>
#include <iostream>

namespace Project {

    // LightSource implementation
    LightSource::LightSource(SourceType type, double intensity, double polarization_angle)
        : type_(type), intensity_(intensity), polarization_angle_(polarization_angle) {}

    std::vector<std::array<LightBeam, 3>> LightSource::generate_beams() const {
        std::vector<std::array<LightBeam, 3>> beams(GRID_SIZE * GRID_SIZE);
        JonesVector pol{std::cos(polarization_angle_ * M_PI / 180.0), 
                        std::sin(polarization_angle_ * M_PI / 180.0)};
        for (size_t i = 0; i < GRID_SIZE * GRID_SIZE; ++i) {
            beams[i] = {
                LightBeam(intensity_, 700.0, pol), // Red
                LightBeam(intensity_, 545.0, pol), // Green
                LightBeam(intensity_, 435.0, pol)  // Blue
            };
        }
        return beams;
    }

    // LightBeam implementation
    LightBeam::LightBeam(double intensity, double wavelength, const JonesVector& polarization)
        : intensity_(intensity), wavelength_(wavelength), polarization_(polarization) {}

    void LightBeam::update_intensity(double new_intensity) {
        intensity_ = new_intensity;
    }

    void LightBeam::update_polarization(const JonesVector& new_polarization) {
        polarization_ = new_polarization;
    }

    void TransparentVolume::apply_optical_effect(LightBeam& beam, double thickness) const {
        // Simple refraction effect: no intensity change, polarization unchanged
        // Could add phase shift based on refractive_index_ and thickness if needed
    }

    // TranslucentVolume implementation
    void TranslucentVolume::apply_optical_effect(LightBeam& beam, double thickness) const {
        // Bouguer-Lambert-Beer law: I = I_0 * exp(-alpha * d)
        double intensity = beam.get_intensity() * std::exp(-absorption_coeff_ * thickness);
        beam.update_intensity(intensity);
    }

    // UniaxialCrystalVolume implementation
    UniaxialCrystalVolume::UniaxialCrystalVolume(double n_o, double n_e, double axis_phi, double axis_theta)
        : n_o_(n_o), n_e_(n_e), axis_phi_(axis_phi), axis_theta_(axis_theta) {}

    void UniaxialCrystalVolume::apply_optical_effect(LightBeam& beam, double thickness) const {
        // Simplified birefringence: introduce phase difference
        double k = 2.0 * M_PI / beam.get_wavelength();
        double n_e_theta = sqrt(
            (cos(axis_theta_) * cos(axis_theta_)) / (n_o_ * n_o_) + 
            (sin(axis_theta_) * sin(axis_theta_)) / (n_e_ * n_e_) 
        );
        double delta_n = n_e_theta - n_o_;
        double phase_diff = k * delta_n * thickness;
        
        JonesVector start_pol = beam.get_polarization();
        JonesVector final_pol = beam.get_polarization();

        // JonesMatrix = 
        // (A  B)
        // (B* C)

        std::complex<double> A = std::complex<double>(cos(phase_diff/2), -1 * sin(phase_diff/2) * cos(2 * axis_phi_));
        std::complex<double> B = std::complex<double>(0, -1 * sin(phase_diff/2) * sin(2 * axis_phi_));
        std::complex<double> C = std::complex<double>(cos(phase_diff/2),      sin(phase_diff/2) * cos(2 * axis_phi_));

        final_pol[0] = A * start_pol[0] + B * start_pol[1];
        final_pol[1] = std::conj(B) * start_pol[0] + C * start_pol[1];

        beam.update_polarization(final_pol);
    }


    // LogicalVolume implementation
    LogicalVolume::LogicalVolume(std::unique_ptr<PhysicalVolume> phys_vol, 
                                 std::unique_ptr<GeometryVolume> geom_vol, 
                                 std::string name)
        : phys_vol_(std::move(phys_vol)), geom_vol_(std::move(geom_vol)), name_(std::move(name)) {
        if (!phys_vol_ || !geom_vol_) {
            throw std::invalid_argument("PhysicalVolume and GeometryVolume must not be null");
        }
    }

    void LogicalVolume::apply_effect(LightBeam& beam) const {
        phys_vol_->apply_optical_effect(beam, geom_vol_->get_thickness());
    }

    // World implementation
    World::World() : volumes_(GRID_SIZE) {
        for (auto& inner : volumes_) {
            inner.resize(GRID_SIZE);  // Resize each inner vector
        }
    }

    void World::add_pixel_volume(int i, int j, std::unique_ptr<LogicalVolume> volume) {
        if (i < 0 || i >= GRID_SIZE || j < 0 || j >= GRID_SIZE) {
            throw std::out_of_range("Pixel indices out of bounds");
        }
        volumes_[i][j] = std::move(volume);
    }

    // PhysicalEngine implementation
    PhysicalEngine::PhysicalEngine(World& world, LightSource& light_source)
        : world_(world), light_source_(light_source) {}

    std::vector<std::vector<std::array<double, 3>>> PhysicalEngine::simulate() {
        auto beams = light_source_.generate_beams();
        std::vector<std::vector<std::array<double, 3>>> image(GRID_SIZE, 
            std::vector<std::array<double, 3>>(GRID_SIZE));
        
        size_t beam_idx = 0;
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                auto& volume = world_.get_volumes()[i][j];
                if (!volume) continue;

                for (int c = 0; c < 3; ++c) {
                    volume->apply_effect(beams[beam_idx][c]);
                    auto pol = apply_nicol_prism(beams[beam_idx][c].get_polarization());
                    image[i][j][c] = beams[beam_idx][c].get_intensity() * 
                                     (std::norm(pol[0]) + std::norm(pol[1]));
                }
                ++beam_idx;
            }
        }
        return image;
    }

    JonesVector PhysicalEngine::apply_nicol_prism(const JonesVector& input) const {
        // Nicol prism at 90 degrees to initial polarization (assumed 0 degrees)
        JonesVector pol{input[0], input[1]}; // Passes all polarization
        // JonesVector pol{0.0, input[1]}; // Passes only y-component
        // JonesVector pol{input[0], 0.0}; // Passes only x-component
        return pol; 
    }

} 