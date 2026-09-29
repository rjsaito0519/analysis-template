#include "RunPaths.hpp"

#include <filesystem>
#include <stdexcept>

int main() {
    if (analysis::run_tag(0) != "run00000") return 1;
    if (analysis::run_tag(42) != "run00042") return 1;
    if (analysis::run_tag(123456) != "run123456") return 1;
    if (analysis::run_output_dir(std::filesystem::path("results"), 7)
        != std::filesystem::path("results") / "run00007") return 1;

    bool rejected_negative = false;
    try {
        static_cast<void>(analysis::run_tag(-1));
    } catch (const std::invalid_argument&) {
        rejected_negative = true;
    }
    return rejected_negative ? 0 : 1;
}
