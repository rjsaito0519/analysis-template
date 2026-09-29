#include "analysis/RunPaths.hpp"

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDF/HistoModels.hxx>
#include <TFile.h>
#include <TNamed.h>
#include <TROOT.h>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
    try {
        if (argc != 6) {
            std::cerr << "Usage: " << argv[0]
                      << " RUN_NUMBER INPUT.root TREE BRANCH OUTPUT.root\n";
            return 2;
        }

        const int run_number = std::stoi(argv[1]);
        const std::string input_file = argv[2];
        const std::string tree_name = argv[3];
        const std::string branch_name = argv[4];
        const std::filesystem::path output_file = argv[5];
        if (run_number < 0) {
            throw std::invalid_argument("run number must be non-negative");
        }
        if (!std::filesystem::is_regular_file(input_file)) {
            throw std::runtime_error("input file does not exist: " + input_file);
        }

        // Enable only after confirming that all called analysis code is thread-safe.
        // ROOT::EnableImplicitMT();
        ROOT::RDataFrame dataframe(tree_name, input_file);
        auto histogram = dataframe.Histo1D(
            {"h_value", "Selected branch;value;events", 200, -100.0, 100.0},
            branch_name
        );
        auto entries = dataframe.Count();

        if (!output_file.parent_path().empty()) {
            std::filesystem::create_directories(output_file.parent_path());
        }
        TFile output(output_file.string().c_str(), "RECREATE");
        if (output.IsZombie()) {
            throw std::runtime_error("cannot create output file: " + output_file.string());
        }
        histogram->Write();  // Triggers one lazy event loop for all booked results.
        TNamed run("run", analysis::run_tag(run_number).c_str());
        TNamed source("source_file", input_file.c_str());
        TNamed source_tree("source_tree", tree_name.c_str());
        TNamed source_branch("source_branch", branch_name.c_str());
        TNamed analysis_version("analysis_version", ANALYSIS_VERSION);
        TNamed root_version("root_version", gROOT->GetVersion());
        run.Write();
        source.Write();
        source_tree.Write();
        source_branch.Write();
        analysis_version.Write();
        root_version.Write();
        output.Close();

        std::cout << "Processed " << *entries << " entries into " << output_file << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
