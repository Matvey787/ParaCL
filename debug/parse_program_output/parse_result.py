#!/usr/bin/python3

import sys
import subprocess

class Colors:
    RED       = '\033[91m'
    GREEN     = '\033[92m'
    YELLOW    = '\033[93m'
    BLUE      = '\033[94m'
    MAGENTA   = '\033[95m'
    CYAN      = '\033[96m'
    WHITE     = '\033[97m'
    BOLD      = '\033[1m'
    UNDERLINE = '\033[4m'
    RESET     = '\033[0m'

def color_print(color, arg, **kwargs):
    print(f"{color}{arg}{Colors.RESET}", **kwargs)

def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]}.py <run_traingles_exe> <test.dat> <test.ans>")
        sys.exit(1)

    executable, test_dat, ans_dat = sys.argv[1:4]
    n_triangles = 0

    try:
        with open(test_dat, 'r') as f:
            result = subprocess.run(
                [executable],
                stdin=f,
                capture_output=True,
                text=True
            )

        if result.returncode != 0:
            color_print(Colors.RED, f"Error: Program failed with exit code {result.returncode}")
            print(f"stderr: {result.stderr}")
            print(f"stdin: ${result.stdout}")
            sys.exit(1)

        program_stdout = result.stdout
        program_stderr = result.stderr

    except Exception as e:
        color_print(Colors.RED, f"Error running program: {e}")
        sys.exit(1)

    try:
        with open(ans_dat, 'r') as f:
            content = f.read().strip()

        for token in content.split():
            try:
                num = int(token)
                if num >= 0:
                    correct_good_triangles.append(num)
            except ValueError:
                continue

    except Exception as e:
        print(f"Error reading answer file: {e}")
        sys.exit(1)

        print(content)
        print(program_stdout)
        print(program_stderr)


if __name__ == "__main__":
    sys.exit(main())
