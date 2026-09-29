cmake_minimum_required(VERSION 3.22)

project(MyAnalysis VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ANALYSIS_BUILD_TESTS "Build unit tests" ON)
option(ANALYSIS_ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

find_package(ROOT 6.24 REQUIRED COMPONENTS Core RIO Tree Hist Graf Gpad ROOTDataFrame)

execute_process(
    COMMAND git rev-parse --short=12 HEAD
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE ANALYSIS_GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT ANALYSIS_GIT_COMMIT)
    set(ANALYSIS_GIT_COMMIT "unknown")
endif()
execute_process(
    COMMAND git status --porcelain --untracked-files=no
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE ANALYSIS_GIT_STATUS
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(ANALYSIS_GIT_STATUS)
    set(ANALYSIS_GIT_DIRTY "true")
else()
    set(ANALYSIS_GIT_DIRTY "false")
endif()

add_library(analysis_options INTERFACE)
target_compile_features(analysis_options INTERFACE cxx_std_17)
target_compile_definitions(analysis_options INTERFACE
    ANALYSIS_VERSION="${PROJECT_VERSION}"
    ANALYSIS_GIT_COMMIT="${ANALYSIS_GIT_COMMIT}"
    ANALYSIS_GIT_DIRTY="${ANALYSIS_GIT_DIRTY}"
)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(analysis_options INTERFACE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
    )
    if(ANALYSIS_ENABLE_SANITIZERS)
        target_compile_options(analysis_options INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer
        )
        target_link_options(analysis_options INTERFACE
            -fsanitize=address,undefined
        )
    endif()
endif()

add_library(analysis_core STATIC
    src/RunPaths.cpp
)
target_include_directories(analysis_core PUBLIC src)
target_link_libraries(analysis_core PUBLIC analysis_options)

function(add_analysis_executable target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE
        analysis_core
        ROOT::Core
        ROOT::RIO
        ROOT::Tree
        ROOT::Hist
        ROOT::Graf
        ROOT::Gpad
        ROOT::ROOTDataFrame
    )
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endfunction()

# Add new programs explicitly here. This keeps accidental/unfinished sources
# from silently becoming build targets.
add_analysis_executable(example_analysis src/example_analysis.cpp)
add_analysis_executable(rdf_analysis src/rdf_analysis.cpp)

if(ANALYSIS_BUILD_TESTS)
    include(CTest)
    add_executable(test_run_paths tests/test_run_paths.cpp)
    target_link_libraries(test_run_paths PRIVATE analysis_core analysis_options)
    add_test(NAME run_paths COMMAND test_run_paths)
endif()
