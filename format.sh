set -euo pipefail

# colors and fonts for console custom output
CONSOLE_COLOR_GREEN='\x1b[32m'
CONSOLE_COLOR_WHITE='\e[0;37m' 
CONSOLE_COLOR_YELLOW='\e[0;33m'
CONSOLE_COLOR_RED='\x1b[31m'
CONSOLE_BIND_FONT='\e[1m'
RESET_CONSOLE_OUTPUT_SETTINGS='\e[0m'

function custom_echo
{
    local color=$1 font=$2 msg=$3
    echo -e "${color}${font}$msg${RESET_CONSOLE_OUTPUT_SETTINGS}"
}

function custom_echo_err
{
    local color=$1 font=$2 msg=$3
    echo -e "${color}${font}${msg}${RESET_CONSOLE_OUTPUT_SETTINGS}" >&2
}

function check_that_exists
{
    local programm=$1
    if ! command -v "${programm}" >/dev/null 2>&1; then
        custom_echo_err "${CONSOLE_COLOR_RED}" "${CONSOLE_BIND_FONT}" "'${programm}' - not found"
        echo "This script requires '${programm}' to be installed"
        exit 1
    fi
}

check_that_exists clang-format
check_that_exists find
check_that_exists xargs

find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.cppm" \) -print0 | xargs -0 clang-format -i

custom_echo "${CONSOLE_COLOR_GREEN}" "${CONSOLE_BIND_FONT}" "Formatting completed successfully"
