#include "analysis/RunPaths.hpp"

#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {

std::filesystem::path env_or_default(
    const char* name,
    const std::filesystem::path& fallback
) {
    const char* value = std::getenv(name);
    return (value != nullptr && value[0] != '\0')
        ? std::filesystem::path(value)
        : fallback;
}

}  // namespace

namespace analysis {

ProjectPaths ProjectPaths::from_environment(
    const std::filesystem::path& project_root
) {
    return {
        env_or_default("ANALYSIS_DATA_DIR", project_root / "data"),
        env_or_default("ANALYSIS_OUTPUT_DIR", project_root / "results"),
        env_or_default("ANALYSIS_PARAM_DIR", project_root / "param"),
        env_or_default("ANALYSIS_SCRATCH_DIR", project_root / "scratch")
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

