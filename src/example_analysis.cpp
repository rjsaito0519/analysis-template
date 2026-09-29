#include "RunPaths.hpp"

#include <TFile.h>
#include <TH1D.h>
#include <TNamed.h>
#include <TRandom3.h>
#include <TROOT.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    int run_number = -1;
    long long entries = 10000;
    std::filesystem::path output_file;
};

void print_usage(const char* program) {
    std::cerr << "Usage: " << program
              << " RUN_NUMBER [--entries N] [--output FILE]\n";
}

Options parse_options(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        throw std::invalid_argument("run number is required");
    }

    Options options;
    options.run_number = std::stoi(argv[1]);

    for (int i = 2; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument == "--entries" && i + 1 < argc) {
            options.entries = std::stoll(argv[++i]);
        } else if (argument == "--output" && i + 1 < argc) {
            options.output_file = argv[++i];
        } else {
            print_usage(argv[0]);
            throw std::invalid_argument("unknown or incomplete argument: " + argument);
        }
    }

    if (options.run_number < 0 || options.entries <= 0) {
        throw std::invalid_argument("RUN_NUMBER must be >= 0 and entries must be > 0");
    }
    return options;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_options(argc, argv);
        gROOT->SetBatch(kTRUE);
        TH1::AddDirectory(kFALSE);  // Make histogram ownership explicit.

        const auto project_root = std::filesystem::current_path();
        const auto paths = analysis::ProjectPaths::in_project(project_root);
        const auto output_file = options.output_file.empty()
            ? analysis::run_output_dir(paths.output, options.run_number)
                / "example_analysis.root"
            : options.output_file;

        if (!output_file.parent_path().empty()) {
            std::filesystem::create_directories(output_file.parent_path());
        }

        TFile output(output_file.string().c_str(), "RECREATE");
        if (output.IsZombie()) {
            throw std::runtime_error("cannot create output file: " + output_file.string());
        }

        TH1D histogram("h_value", "Example distribution;value;events", 120, -6.0, 6.0);
        TRandom3 random(static_cast<unsigned int>(options.run_number));
        for (long long entry = 0; entry < options.entries; ++entry) {
            histogram.Fill(random.Gaus(0.0, 1.0));
        }

        const std::string run = analysis::run_tag(options.run_number);
        TNamed run_metadata("run", run.c_str());
        TNamed analysis_version("analysis_version", ANALYSIS_VERSION);
        TNamed git_commit("git_commit", ANALYSIS_GIT_COMMIT);
        TNamed git_dirty("git_dirty", ANALYSIS_GIT_DIRTY);
        TNamed root_version("root_version", gROOT->GetVersion());
        TNamed description("description", "Replace the random loop with your event loop");
        output.cd();
        histogram.Write();
        run_metadata.Write();
        analysis_version.Write();
        git_commit.Write();
        git_dirty.Write();
        root_version.Write();
        description.Write();
        output.Close();

        std::cout << "Wrote " << options.entries << " entries to "
                  << output_file << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
