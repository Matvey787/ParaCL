#!/bin/bash
set -euo pipefail

# Console colors
readonly CONSOLE_COLOR_GREEN='\x1b[32m'
readonly CONSOLE_COLOR_WHITE='\e[0;37m'
readonly CONSOLE_COLOR_YELLOW='\e[0;33m'
readonly CONSOLE_COLOR_RED='\x1b[31m'
readonly CONSOLE_BIND_FONT='\e[1m'
readonly RESET_CONSOLE_OUTPUT_SETTINGS='\e[0m'

# Global variables
declare BUILD_TYPE="Debug"
declare SANITIZE_ENABLED=false
declare TESTING_ENABLED=false
declare LOGGER_ENABLED=false
declare GRAPHVIZ_ENABLED=false
declare CONAN_ENABLED=false
declare CLEAN_BUILD=false
declare PARALLEL_JOBS=
declare VERBOSE=false
declare SHOW_HELP=false
declare CMAKE_EXTRA_ARGS=()

# Function definitions
function custom_echo {
    local color=$1 font=$2 msg=$3
    echo -e "${color}${font}${msg}${RESET_CONSOLE_OUTPUT_SETTINGS}"
}

function custom_echo_err {
    local color=$1 font=$2 msg=$3
    echo -e "${color}${font}${msg}${RESET_CONSOLE_OUTPUT_SETTINGS}" >&2
}

function skip_line_in_console {
    echo
}

function check_that_exists {
    local program=$1
    if ! command -v "${program}" >/dev/null 2>&1; then
        custom_echo_err "${CONSOLE_COLOR_RED}" "${CONSOLE_BIND_FONT}" "'${program}' not found."
        custom_echo_err "" "" "This script requires '${program}' to be installed."
        return 1
    fi
    return 0
}

function display_help {
    cat << EOF
Usage: $(basename "$0") [OPTIONS]

Build script for C++ project with Conan and CMake.

Options:
  -b, --build-type TYPE     Set build type (Debug, Release, RelWithDebInfo, MinSizeRel)
                            Default: Debug
  -s, --sanitize            Enable sanitizers (Address, Undefined)
  -t, --testing             Enable testing (BUILD_TESTING=ON)
  -l, --logger              Enable logger (LOGGER=ON)
  -g, --graphviz            Enable GraphViz documentation (GRAPHVIZ=ON)
  -c, --conan               Run Conan dependency installation
  -C, --clean               Clean build directory before building
  -j, --jobs N              Number of parallel jobs (default: all cores)
  -v, --verbose             Show detailed commands output
  -h, --help                Display this help message
  -- CMAKE_ARGS             Additional CMake arguments (must be last)

Examples:
  $(basename "$0") -b Release -s -t -c
  $(basename "$0") --build-type Debug --sanitize --jobs 4
  $(basename "$0") --clean --verbose -- -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

EOF
}

function parse_arguments {
    # Define short and long options
    local short_opts="b:stlgcCj:vh"
    local long_opts="build-type:,sanitize,testing,logger,graphviz,conan,clean,jobs:,verbose,help"
    
    # Parse options
    local parsed_opts
    if ! parsed_opts=$(getopt -o "$short_opts" --long "$long_opts" -n "$(basename "$0")" -- "$@"); then
        exit 1
    fi
    
    eval set -- "$parsed_opts"
    
    # Process options
    while true; do
        case "$1" in
            -b|--build-type)
                BUILD_TYPE="$2"
                case "$BUILD_TYPE" in
                    Debug|Release|RelWithDebInfo|MinSizeRel)
                        ;; # Valid build type
                    *)
                        custom_echo_err "${CONSOLE_COLOR_RED}" "${CONSOLE_BIND_FONT}" \
                            "Invalid build type: $BUILD_TYPE"
                        custom_echo_err "" "" "Valid types: Debug, Release, RelWithDebInfo, MinSizeRel"
                        exit 1
                        ;;
                esac
                shift 2
                ;;
            -s|--sanitize)
                SANITIZE_ENABLED=true
                shift
                ;;
            -t|--testing)
                TESTING_ENABLED=true
                shift
                ;;
            -l|--logger)
                LOGGER_ENABLED=true
                shift
                ;;
            -g|--graphviz)
                GRAPHVIZ_ENABLED=true
                shift
                ;;
            -c|--conan)
                CONAN_ENABLED=true
                shift
                ;;
            -C|--clean)
                CLEAN_BUILD=true
                shift
                ;;
            -j|--jobs)
                if [[ "$2" =~ ^[0-9]+$ ]]; then
                    PARALLEL_JOBS="$2"
                else
                    custom_echo_err "${CONSOLE_COLOR_RED}" "${CONSOLE_BIND_FONT}" \
                        "Invalid number of jobs: $2"
                    exit 1
                fi
                shift 2
                ;;
            -v|--verbose)
                VERBOSE=true
                shift
                ;;
            -h|--help)
                SHOW_HELP=true
                shift
                ;;
            --)
                shift
                CMAKE_EXTRA_ARGS=("$@")
                break
                ;;
            *)
                custom_echo_err "${CONSOLE_COLOR_RED}" "${CONSOLE_BIND_FONT}" \
                    "Internal error while parsing options"
                exit 1
                ;;
        esac
    done
}

function check_dependencies {
    local deps=("cmake" "flex" "bison" "clang++" "ninja")
    local missing_deps=()
    
    custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" "Checking dependencies..."
    
    for dep in "${deps[@]}"; do
        if check_that_exists "$dep"; then
            custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" \
                "$(printf "%-7s" "${dep}") was found"
        else
            missing_deps+=("$dep")
        fi
    done
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        custom_echo_err "${CONSOLE_COLOR_RED}" "${CONSOLE_BIND_FONT}" \
            "Missing dependencies: ${missing_deps[*]}"
        exit 1
    fi
    
    custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" \
        "All dependencies are found, continue to work..."
    skip_line_in_console
}

function clean_build_directory {
    local build_dir="$1"
    
    if [ -d "${build_dir}" ] && [ -n "$(ls -A "${build_dir}" 2>/dev/null)" ]; then
        custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" \
            "Cleaning directory: ${build_dir}/"
        
        local rm_command="rm -rf \"${build_dir}/\""
        if [ "$VERBOSE" = true ]; then
            custom_echo "${CONSOLE_COLOR_WHITE}" "" "$rm_command"
            rm -rf "${build_dir}/"
        else
            rm -rf "${build_dir}/" 2>/dev/null
        fi
        
        skip_line_in_console
    fi
}

function run_conan_install {
    local build_dir="$1"
    
    if [ "$CONAN_ENABLED" = true ]; then
        custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" \
            "Installing dependencies with conan..."
        
        local conan_command="conan install . --build=missing -of \"${build_dir}\""
        
        if [ "$VERBOSE" = true ]; then
            custom_echo "${CONSOLE_COLOR_WHITE}" "" "$conan_command"
            eval "$conan_command"
        else
            eval "$conan_command" >/dev/null 2>&1
        fi
    else
        custom_echo "${CONSOLE_COLOR_YELLOW}" "" "Skipping Conan installation (use -c to enable)"
    fi
}

function generate_build_system {
    local source_dir="$1" build_dir="$2"
    
    custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" "Generating build system..."
    
    # Build CMake command
    local cmake_command="cmake -G Ninja -S \"${source_dir}\" -B \"${build_dir}\""
    cmake_command+=" -DCMAKE_C_COMPILER=clang"
    cmake_command+=" -DCMAKE_CXX_COMPILER=clang++"
    cmake_command+=" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
    
    # Add optional features
    [ "$SANITIZE_ENABLED" = true ] && cmake_command+=" -DSanitize=ON"
    [ "$TESTING_ENABLED" = true ] && cmake_command+=" -DBUILD_TESTING=ON"
    [ "$LOGGER_ENABLED" = true ] && cmake_command+=" -DLOGGER=ON"
    [ "$GRAPHVIZ_ENABLED" = true ] && cmake_command+=" -DGRAPHVIZ=ON"
    
    # Add extra CMake arguments
    if [ ${#CMAKE_EXTRA_ARGS[@]} -gt 0 ]; then
        for arg in "${CMAKE_EXTRA_ARGS[@]}"; do
            cmake_command+=" $arg"
        done
    fi
    
    # Add Conan toolchain if enabled
    if [ "$CONAN_ENABLED" = true ]; then
        cmake_command+=" -DCMAKE_TOOLCHAIN_FILE=\"${build_dir}/conan_toolchain.cmake\""
    fi
    
    # Execute CMake command
    if [ "$VERBOSE" = true ]; then
        custom_echo "${CONSOLE_COLOR_WHITE}" "" "$cmake_command"
        eval "$cmake_command"
    else
        eval "$cmake_command" 2>&1 | grep -v "^--" || true
    fi
    
    custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" \
        "Generating build system completed successfully."
    skip_line_in_console
}

function build_project {
    local build_dir="$1"
    
    custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" "Building project..."
    
    # Determine number of parallel jobs
    local jobs_option=""
    if [ -n "$PARALLEL_JOBS" ]; then
        jobs_option="--parallel $PARALLEL_JOBS"
    else
        jobs_option="--parallel $(nproc)"
    fi
    
    local build_command="cmake --build \"${build_dir}\" $jobs_option"
    
    if [ "$VERBOSE" = true ]; then
        custom_echo "${CONSOLE_COLOR_WHITE}" "" "$build_command"
        eval "$build_command"
    else
        eval "$build_command" 2>&1 | tail -20
    fi
    
    custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" \
        "Building project completed successfully."
    skip_line_in_console
}

function display_final_message {
    local build_dir="$1"
    
    custom_echo "${CONSOLE_COLOR_WHITE}" "${CONSOLE_BIND_FONT}" "Build summary:"
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  Build type:       ${BUILD_TYPE}"
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  Sanitizers:       ${SANITIZE_ENABLED}"
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  Testing:          ${TESTING_ENABLED}"
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  Logger:           ${LOGGER_ENABLED}"
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  GraphViz:         ${GRAPHVIZ_ENABLED}"
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  Conan:            ${CONAN_ENABLED}"
    skip_line_in_console

    custom_echo "${CONSOLE_COLOR_WHITE}" "${CONSOLE_BIND_FONT}" "To work with the project, execute:"
    
    local relative_build_dir="$(realpath --relative-to=. "$build_dir" 2>/dev/null || echo "$build_dir")"
    if [ "$relative_build_dir" = "." ]; then
        relative_build_dir="build"
    fi
    
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  cd \"${relative_build_dir}/\""
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  # Run tests: ctest -V"
    custom_echo "${CONSOLE_COLOR_WHITE}" "" "  # Run specific test: ./tests/your_test"
}

function main {
    parse_arguments "$@"
    
    if [ "$SHOW_HELP" = true ]; then
        display_help
        exit 0
    fi
    
    clear
    
    # Set up directories
    local project_dir
    project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    local source_dir="$project_dir"
    local build_dir="$project_dir/build"
    
    # Clean build directory if requested
    if [ "$CLEAN_BUILD" = true ]; then
        clean_build_directory "$build_dir"
    fi
    
    check_dependencies
    run_conan_install "$build_dir"
    generate_build_system "$source_dir" "$build_dir"
    build_project "$build_dir"
    mkdir -p "${build_dir}/paracl_build"
    display_final_message "$build_dir"
}

# Run main function with all arguments
main "$@"
