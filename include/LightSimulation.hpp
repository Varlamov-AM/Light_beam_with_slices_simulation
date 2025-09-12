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
        static constexpr int n_beams = 41;
        LightSource(SourceType type = SourceType::PlanePolarized, double intensity = 1.0, double polarization_angle = 0.0);
        std::vector<std::array<LightBeam, n_beams>> generate_beams() const;

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
        static constexpr int lambda_arr_len = LightSource::n_beams;
    
        std::vector<double> apply_xyz_filter(std::vector<double> intensity);

        static constexpr double lambda_arr[lambda_arr_len] = 
            {
            380, 390, 400, 410, 420, 430, 440, 450, 460, 470,
            480, 490, 500, 510, 520, 530, 540, 550, 560, 570,
            580, 590, 600, 610, 620, 630, 640, 650, 660, 670,
            680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 
            780
            };

        static constexpr double x_filter[lambda_arr_len] = 
            {
            0.0014, 0.0042, 0.0143, 0.0435, 0.1344,
            0.2839, 0.3483, 0.3362, 0.2908, 0.1954,
            0.0956, 0.0320, 0.0049, 0.0093, 0.0633,
            0.1655, 0.2904, 0.4334, 0.5945, 0.7621,
            0.9163, 1.0263, 1.0622, 1.0026, 0.8544,
            0.6424, 0.4479, 0.2835, 0.1649, 0.0874,
            0.0468, 0.0227, 0.0114, 0.0058, 0.0029,
            0.0014, 0.0007, 0.0003, 0.0002, 0.0001,
            0.
            };

        static constexpr double y_filter[lambda_arr_len] = 
            {
            0.0000, 0.0001, 0.0004, 0.0012, 0.0040,
            0.0116, 0.0230, 0.0380, 0.0600, 0.0910,
            0.1390, 0.2080, 0.3230, 0.5030, 0.7100,
            0.8620, 0.9540, 0.9950, 0.9950, 0.9520, 
            0.8700, 0.7570, 0.6310, 0.5030, 0.3810,
            0.2650, 0.1750, 0.1070, 0.0610, 0.0320,
            0.0170, 0.0082, 0.0041, 0.0021, 0.0010,
            0.0005, 0.0003, 0.0001, 0.0001, 0.0000,
            0.
            };

        static constexpr double z_filter[lambda_arr_len] = 
            {
            0.0065, 0.0201, 0.0679, 0.2074, 0.6456,
            1.3856, 1.7471, 1.7721, 1.6692, 1.2876,
            0.8130, 0.4652, 0.2720, 0.1582, 0.0782,
            0.0422, 0.0203, 0.0087, 0.0039, 0.0021,
            0.0017, 0.0011, 0.0008, 0.0003, 0.0002,
            0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
            0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
            0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
            0.
            };

        std::vector<std::vector<double>> xyz_to_rgb_matrix = {
            std::vector<double>{3.2406, -1.5372, -0.4986},
            std::vector<double>{-0.9689, 1.8758, 0.0415},
            std::vector<double>{0.0557, -0.2040, 1.0570},
        };

        std::vector<double> light_intensity;

    private:
        World& world_;
        LightSource& light_source_;
        JonesVector apply_nicol_prism(const JonesVector& input) const;
    };

} // namespace Project

#endif // LIGHT_SIMULATION_HPP  
