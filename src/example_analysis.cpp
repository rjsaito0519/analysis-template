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
    long long entries = 10000;
    std::filesystem::path output_file = "results/example_analysis.root";
};

Options parse_options(int argc, char** argv) {
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument == "--entries" && i + 1 < argc) {
            options.entries = std::stoll(argv[++i]);
        } else if (argument == "--output" && i + 1 < argc) {
            options.output_file = argv[++i];
        } else {
            throw std::invalid_argument(
                "usage: example_analysis [--entries N] [--output FILE]"
            );
        }
    }
    if (options.entries <= 0) {
        throw std::invalid_argument("entries must be positive");
    }
    return options;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_options(argc, argv);
        gROOT->SetBatch(kTRUE);
        TH1::AddDirectory(kFALSE);

        if (!options.output_file.parent_path().empty()) {
            std::filesystem::create_directories(options.output_file.parent_path());
        }
        TFile output(options.output_file.string().c_str(), "RECREATE");
        if (output.IsZombie()) {
            throw std::runtime_error(
                "cannot create output file: " + options.output_file.string()
            );
        }

        TH1D histogram("h_value", "Example distribution;value;events", 120, -6.0, 6.0);
        TRandom3 random(12345);
        for (long long entry = 0; entry < options.entries; ++entry) {
            histogram.Fill(random.Gaus(0.0, 1.0));
        }

        TNamed analysis_version("analysis_version", ANALYSIS_VERSION);
        TNamed git_commit("git_commit", ANALYSIS_GIT_COMMIT);
        TNamed git_dirty("git_dirty", ANALYSIS_GIT_DIRTY);
        TNamed root_version("root_version", gROOT->GetVersion());
        output.cd();
        histogram.Write();
        analysis_version.Write();
        git_commit.Write();
        git_dirty.Write();
        root_version.Write();
        output.Close();

        std::cout << "Wrote " << options.entries << " entries to "
                  << options.output_file << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}

