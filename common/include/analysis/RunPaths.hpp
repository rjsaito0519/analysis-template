#ifndef ANALYSIS_RUN_PATHS_HPP
#define ANALYSIS_RUN_PATHS_HPP

#include <filesystem>
#include <string>

namespace analysis {

struct ProjectPaths {
    std::filesystem::path data;
    std::filesystem::path output;
    std::filesystem::path param;
    std::filesystem::path scratch;

    static ProjectPaths from_environment(const std::filesystem::path& project_root);
};

std::string run_tag(int run_number);
std::filesystem::path run_output_dir(
    const std::filesystem::path& base_output,
    int run_number
);

}  // namespace analysis

#endif

