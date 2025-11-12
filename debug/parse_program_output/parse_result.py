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

def extract_numbers(text):
    numbers = []
    for token in text.split():
        try:
            num = int(token)
            numbers.append(num)
        except ValueError:
            continue
    return numbers

def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <paracl_exe> <test>.pcl <answer>.ans")
        return 1

    executable, test_input, test_answer = sys.argv[1:4]

    expect_death = False
    exit_code = 0

    try:
        with open(test_answer, 'r') as f:
            answer_content = f.read()

        if answer_content.partition(':')[0] == "DEATH_WITH":
            expect_death = True
            if len(answer_content.partition(':')) <= 2:
                color_print(Colors.RED, "bad format of ans file")
                return 1

            exit_code = int(answer_content.partition(':')[2])

        else:
            answer_content.strip()

    except Exception as e:
        color_print(Colors.RED, f"Error reading file: {e}")
        sys.exit(1)

    try:
        with open(test_input, 'r') as f:
            result = subprocess.run(
                [executable],
                stdin=f,
                capture_output=True,
                text=True
            )


        if expect_death:
            if result.returncode == exit_code:
                color_print(Colors.GREEN, "TEST PASSED")
                return 0

            color_print(Colors.WHITE, "Expect DEATH", end = '\n\n')
            color_print(Colors.GREEN, f"[ expected exit code ]: {exit_code}")
            color_print(Colors.RED,   f"[ program  exit code ]: {result.returncode}", end = '\n\n')
            color_print(Colors.RED, "\n\nTEST FAILED")
            return 1

            color_print(Colors.RED, f"Error: Program failed with exit code {result.returncode}")
            print(f"stderr: {result.stderr}")
            print(f"stdout: {result.stdout}")

            color_print(Colors.RED, "\n\nTEST FAILED")

            return 1

        program_stdout = result.stdout
        program_stderr = result.stderr

        expected_numbers = extract_numbers(answer_content)        
        program_output = extract_numbers(program_stdout)

    except Exception as e:
        color_print(Colors.RED, f"Error reading file: {e}")
        sys.exit(1)

    if program_stderr:
        print(f"{Colors.YELLOW}Program stderr:{Colors.RESET}")
        print(program_stderr)

    if expected_numbers == program_output:
        color_print(Colors.GREEN, "TEST PASSED")
        return 0

    print()

    errors = 0
    good_program_out = 0
    len_out = len(program_output)
    len_ans = len(expected_numbers)

    for i in range(min(len_out, len_ans)):
        if program_output[i] != expected_numbers[i]:
            errors += 1
            color_print(Colors.WHITE, f"[{i}]")
            color_print(Colors.RED, f"program output: {program_output[i]}\nexpected value: {expected_numbers[i]}", end='\n\n')
        else:
            good_program_out += 1
            color_print(Colors.WHITE, f"[{i}]")
            color_print(Colors.GREEN, f"program output: {program_output[i]}\nexpected value: {expected_numbers[i]}", end='\n\n')

    if len_out < len_ans:
        for i in range(len_out, len_ans):
            errors += 1
            color_print(Colors.WHITE, f"[{i}]")
            color_print(Colors.RED, f"program output: NONE\nexpected value: {expected_numbers[i]}", end='\n\n')

    if len_out > len_ans:
        for i in range(len_ans, len_out):
            errors += 1
            color_print(Colors.WHITE, f"[{i}]")
            color_print(Colors.RED, f"program output: {program_output[i]}\nexpected value: NONE", end='\n\n')

    color_print(Colors.WHITE, "\nINFO:", end='\n\n')

    color_print(Colors.CYAN, f"Expected answer: {expected_numbers}")
    color_print(Colors.CYAN, f"Program output:  {program_output}", end = '\n\n')

    color_print(Colors.WHITE, f"[ total numbers expected ]: {len_ans}")
    color_print(Colors.WHITE, f"[ total numbers received ]: {len_out}", end = '\n\n') 

    color_print(Colors.WHITE, f"[ errors / total ]: {errors}/{len_ans}", end='\n\n')

    if len_ans > 0:
        percent_of_accuracy = (good_program_out / len_ans) * 100
        percent_of_errors = 100 - percent_of_accuracy
    elif len_ans == 0 and len_out == 0:
        percent_of_accuracy = 100
        percent_of_errors = 0
    else:
        percent_of_accuracy = 0
        percent_of_errors = 100

    color_print(Colors.GREEN, f"[ percentage of accuracy ]: {round(percent_of_accuracy, 2)}%")
    color_print(Colors.RED,   f"[ percentage of errors   ]: {round(percent_of_errors, 2)}%")

    color_print(Colors.RED, "\n\nTEST FAILED")
    return 1

if __name__ == "__main__":
    sys.exit(main())
