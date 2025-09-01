#ifndef LIGHT_SIMULATION_HPP
#define LIGHT_SIMULATION_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <array>
#include <complex>

namespace Project {

    constexpr int GRID_SIZE = 640;
    using JonesVector = std::array<std::complex<double>, 2>;

    class LightBeam;

    class LightSource {
    public:
        enum class SourceType { PlanePolarized };
        LightSource(SourceType type = SourceType::PlanePolarized, double intensity = 1.0, double polarization_angle = 0.0);
        std::vector<std::array<LightBeam, 3>> generate_beams() const;

    private:
        SourceType type_;
        double intensity_;
        double polarization_angle_;
    };


    class LightBeam {
    public:
        LightBeam(double intensity, double wavelength, const JonesVector& polarization);
        LightBeam() : intensity_(1.0), wavelength_(0.0), polarization_{{0.0, 0.0}} {}
        // LightBeam(double intensity, double wavelength, const JonesVector& polarization)
        //     : intensity_(intensity), wavelength_(wavelength), polarization_(polarization) {}
        void update_intensity(double new_intensity);
        void update_polarization(const JonesVector& new_polarization);
        double get_intensity() const { return intensity_; }
        const JonesVector& get_polarization() const { return polarization_; }
        double get_wavelength() const { return wavelength_; }

    private:
        double intensity_;
        double wavelength_;
        JonesVector polarization_;
    };

    class PhysicalVolume {
    public:
        virtual ~PhysicalVolume() = default;
        virtual void apply_optical_effect(LightBeam& beam, double thickness) const = 0;
    };

    class TransparentVolume : public PhysicalVolume {
    public:
        TransparentVolume(double refractive_index) : refractive_index_(refractive_index) {}
        void apply_optical_effect(LightBeam& beam, double thickness) const override;

    private:
        double refractive_index_;
    };

    class TranslucentVolume : public PhysicalVolume {
    public:
        TranslucentVolume(double absorption_coeff) : absorption_coeff_(absorption_coeff) {}
        void apply_optical_effect(LightBeam& beam, double thickness) const override;

    private:
        double absorption_coeff_;
    };

    class UniaxialCrystalVolume : public PhysicalVolume {
    public:
        UniaxialCrystalVolume(double n_o, double n_e, double axis_phi, double axis_theta);
        void apply_optical_effect(LightBeam& beam, double thickness) const override;

    private:
        double n_o_, n_e_; // Ordinary and extraordinary refractive indices
        double axis_theta_, axis_phi_; // Orientation of optical axis in spherical coordinates
    };

    class GeometryVolume {
    public:
        virtual ~GeometryVolume() = default;
        virtual double get_thickness() const = 0;
    };

    class PixelVolume : public GeometryVolume {
    public:
        PixelVolume(double thickness) : thickness_(thickness) {}
        double get_thickness() const override { return thickness_; }

    private:
        double thickness_;
    };

    class LogicalVolume {
    public:
        LogicalVolume(std::unique_ptr<PhysicalVolume> phys_vol, std::unique_ptr<GeometryVolume> geom_vol, 
                      std::string name);
        void apply_effect(LightBeam& beam) const;
        const std::string& get_name() const { return name_; }

    private:
        std::unique_ptr<PhysicalVolume> phys_vol_;
        std::unique_ptr<GeometryVolume> geom_vol_;
        std::string name_;
    };

    class World {
    private:
        std::vector<std::vector<std::unique_ptr<LogicalVolume>>> volumes_;
    public:
        World();  // Constructor
        void add_pixel_volume(int i, int j, std::unique_ptr<LogicalVolume> volume);
        const std::vector<std::vector<std::unique_ptr<LogicalVolume>>>& get_volumes() const { return volumes_; }
    };

    class PhysicalEngine {
    public:
        PhysicalEngine(World& world, LightSource& light_source);
        std::vector<std::vector<std::array<double, 3>>> simulate();

    private:
        World& world_;
        LightSource& light_source_;
        JonesVector apply_nicol_prism(const JonesVector& input) const;
    };

} // namespace Project

#endif // LIGHT_SIMULATION_HPP  