#include "analysis/RunPaths.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace analysis {

ProjectPaths ProjectPaths::in_project(
    const std::filesystem::path& project_root
) {
    return {
        project_root / "data",
        project_root / "results",
        project_root / "param",
        project_root / "scratch"
    };
}

std::string run_tag(const int run_number) {
    if (run_number < 0) {
        throw std::invalid_argument("run number must be non-negative");
    }

    std::ostringstream stream;
    stream << "run" << std::setfill('0') << std::setw(5) << run_number;
    return stream.str();
}

std::filesystem::path run_output_dir(
    const std::filesystem::path& base_output,
    const int run_number
) {
    return base_output / run_tag(run_number);
}

}  // namespace analysis
