#include "analysis/RunPaths.hpp"

#include <TFile.h>
#include <TParameter.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " RUN_NUMBER CALIBRATION_VALUE\n";
            return 2;
        }

        const int run_number = std::stoi(argv[1]);
        const double value = std::stod(argv[2]);
        const auto paths = analysis::ProjectPaths::in_project(
            std::filesystem::current_path()
        );
        const auto output_file = analysis::run_output_dir(paths.output, run_number)
            / "example_calibration.root";
        std::filesystem::create_directories(output_file.parent_path());

        TFile output(output_file.string().c_str(), "RECREATE");
        if (output.IsZombie()) {
            throw std::runtime_error("cannot create output file");
        }
        TParameter<double> calibration("calibration_value", value);
        calibration.Write();
        output.Close();

        std::cout << "Wrote calibration value " << value << " to "
                  << output_file << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
