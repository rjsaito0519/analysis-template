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
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0]
                      << " INPUT.root TREE BRANCH OUTPUT.root\n";
            return 2;
        }

        const std::string input_file = argv[1];
        const std::string tree_name = argv[2];
        const std::string branch_name = argv[3];
        const std::filesystem::path output_file = argv[4];
        if (!std::filesystem::is_regular_file(input_file)) {
            throw std::runtime_error("input file does not exist: " + input_file);
        }

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
        histogram->Write();
        TNamed source("source_file", input_file.c_str());
        TNamed source_tree("source_tree", tree_name.c_str());
        TNamed source_branch("source_branch", branch_name.c_str());
        TNamed analysis_version("analysis_version", ANALYSIS_VERSION);
        TNamed git_commit("git_commit", ANALYSIS_GIT_COMMIT);
        TNamed git_dirty("git_dirty", ANALYSIS_GIT_DIRTY);
        TNamed root_version("root_version", gROOT->GetVersion());
        source.Write();
        source_tree.Write();
        source_branch.Write();
        analysis_version.Write();
        git_commit.Write();
        git_dirty.Write();
        root_version.Write();
        output.Close();

        std::cout << "Processed " << *entries << " entries into " << output_file << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}

