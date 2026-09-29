# Repository guide for AI assistants

This repository uses C++17, CERN ROOT, and Python for data analysis.
Keep it general: do not assume a specific experiment, detector, run-number scheme, storage layout, or host environment.

## Start here

- Read `README.md` for commands and the current directory layout.
- Read `docs/README.md` for design notes and project conventions.
- Inspect the files related to the request before editing them; do not scan or rewrite unrelated areas.

## Working rules

- Make the smallest change that fully solves the requested problem.
- Follow nearby code style and preserve compatibility unless a deliberate migration is requested.
- Do not introduce dependencies, directories, or abstractions before they are needed.
- Do not hard-code user names, host names, absolute paths, dataset locations, or exact tool versions.
- Keep generated files, large data, ROOT outputs, and credentials out of Git.

## Physics and ROOT safety

- Never change cuts, calibration values, units, coordinate conventions, or reconstruction behavior silently.
- State the expected effect of any change that can alter physics results.
- Preserve existing TTree, branch, histogram, and ROOT object names unless a migration is part of the task.
- Validate input files, trees, and required branches before an event loop.
- Keep provenance needed to reproduce results, such as the Git revision, ROOT version, inputs, and important options.

## Code and verification

- C++ uses C++17. Prefer RAII, values, standard-library facilities, and explicit ownership.
- Add C++ programs explicitly with `add_analysis_executable(...)`; do not glob sources into targets.
- Python scripts stay under `scripts/` until shared code genuinely warrants a package.
- Build C++ changes with `./build.sh`. Run the smallest relevant example or script when practical.
- Do not treat generated files under `.build*/`, `data/`, or `results/` as source files.

## Git commits

- Read-only commands such as `git status`, `git diff`, and `git log` may be used for inspection.
- Do not run `git add`, create a commit, or push unless the user explicitly requests that action.
- Present the proposed changes for review before staging or committing when practical.
- Use the repository user's configured Git identity as the sole author.
- Do not add AI identities or attribution such as `Co-authored-by`, `Made with`, or generated-by trailers.
- Do not change the user's Git name or email configuration.

## GitHub issues

- Proactively suggest an issue when a bug, deferred task, open question, or design decision should be tracked beyond the current work.
- Include a concise proposed title, context, acceptance criteria, and relevant files or evidence.
- A suggestion is not authorization: do not create, edit, label, assign, comment on, or close an issue until the user explicitly approves that action.
- After approval, keep the issue focused and avoid including credentials, private paths, or unnecessary environment-specific details.

## Documentation

- Keep `README.md` short and task-oriented.
- Put durable explanations in `docs/` and add them to `docs/README.md`.
- Organize documentation by its concrete analysis topic from the beginning; do not impose generic categories that do not match the project.
- Give each topic directory its own `README.md` entry point and link it from `docs/README.md`.
- Record assumptions, units, input/output schemas, and validation methods for analysis logic.
- Date statements that describe temporary status or time-dependent results.
- Treat this file as the environment-independent source for AI guidance.
- Keep tool-specific settings such as `.cursor/` local and derive them from this file when needed.
