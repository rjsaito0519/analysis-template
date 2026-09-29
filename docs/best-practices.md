# Design notes and practical rules

## Responsibilities

- C++ performs the event loop, detector-specific algorithms, and CPU-heavy fitting.
- ROOT stores columnar event data, histograms, and compact provenance metadata.
- Python orchestrates runs, performs lightweight inspection, plotting, tabulation, and batch submission.
- `main()` parses arguments and assembles a pipeline; reusable physics logic belongs in a library.

This boundary avoids maintaining the same physics selection independently in C++ and Python. If an algorithm must be available from both, expose one implementation through ROOT/PyROOT or a small binding rather than copying it.

## C++ and CMake

- Express build requirements on targets (`target_compile_features`, `target_link_libraries`) instead of global compiler flags.
- Register executables explicitly. Do not glob every experimental `.cpp` into a build target.
- Prefer values, RAII, `std::filesystem`, and smart pointers. Avoid owning raw pointers.
- Treat compiler warnings as feedback. Turn warnings into errors in CI only after third-party headers are isolated.
- Run sanitizer builds on small samples during development; optimized production jobs remain separate.
- Keep build files under `.build*`; never mix generated files with source.

## ROOT

- Prefer `ROOT::RDataFrame` for new columnar analyses. Book all actions before reading their results so ROOT can execute them in one event loop.
- Enable implicit multithreading only after verifying that custom callbacks and external libraries are thread-safe.
- Make object lifetime explicit. ROOT 6 directory ownership can otherwise delete objects at surprising times; use stack objects or smart pointers and detach histograms when needed.
- Check every input and output `TFile`, tree, and required branch before the event loop. Fail with the run number and path in the message.
- Store analysis version, ROOT version, inputs, tree name, and essential cuts alongside results.
- Keep plotting separate from event processing. Batch jobs should not depend on a display server.

## Python

- Importable code lives under `python/src/my_analysis`; one-off entry points are thin wrappers.
- Put dependencies and tool configuration in `pyproject.toml`. Install editable during development with `pip install -e '.[dev]'`.
- Call subprocesses with an argument list and `shell=False` (the default), not a shell command string.
- Use `pathlib.Path`, context managers for ROOT/uproot files, type hints, and small pure functions.
- Use uproot/awkward for portable inspection and plotting when PyROOT behavior is not required; use PyROOT/RDataFrame when sharing ROOT-native transformations matters.

## Reproducibility and data policy

- Never commit raw data, generated ROOT files, credentials, or machine-specific absolute paths.
- Version small calibration/configuration inputs. Record the exact file or content hash used by a job.
- Keep one immutable output directory per run and analysis revision for production. Do not silently overwrite approved results.
- A production manifest should include Git commit, dirty-tree flag, command line, environment name, timestamps, input checksums or dataset IDs, and event counts.
- Test pure calculations without ROOT where possible; add a tiny synthetic ROOT fixture for integration tests.

