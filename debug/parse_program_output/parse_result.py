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
    """Извлекает все целые числа из текста"""
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
        print(f"Usage: {sys.argv[0]} <program_exe> <test>.pcl <answer>.ans")
        sys.exit(1)

    executable, test_input, test_answer = sys.argv[1:4]

    try:
        with open(test_input, 'r') as f:
            result = subprocess.run(
                [executable],
                stdin=f,
                capture_output=True,
                text=True
            )

        if result.returncode != 0:
            color_print(Colors.RED, f"Error: Program failed with exit code {result.returncode}")
            print(f"stderr: {result.stderr}")
            print(f"stdout: {result.stdout}")
            sys.exit(1)

        program_stdout = result.stdout
        program_stderr = result.stderr

    except Exception as e:
        color_print(Colors.RED, f"Error running program: {e}")
        sys.exit(1)

    try:
        with open(test_answer, 'r') as f:
            answer_content = f.read().strip()
        
        expected_numbers = extract_numbers(answer_content)        
        program_output = extract_numbers(program_stdout)

    except Exception as e:
        color_print(Colors.RED, f"Error reading files: {e}")
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
            color_print(Colors.RED, f"position {i}: program output: {program_output[i]}\nexpected: {expected_numbers[i]}", end='\n\n')
        else:
            good_program_out += 1
            color_print(Colors.GREEN, f"position {i}: program output: {program_output[i]}\nexpected: {expected_numbers[i]}", end='\n\n')

    if len_out < len_ans:
        for i in range(len_out, len_ans):
            errors += 1
            color_print(Colors.RED, f"position {i}: program output: NONE\nexpected: {expected_numbers[i]}", end='\n\n')

    if len_out > len_ans:
        for i in range(len_ans, len_out):
            errors += 1
            color_print(Colors.RED, f"position {i}: program output: {program_output[i]}\nexpected: NONE", end='\n\n')

    color_print(Colors.WHITE, "INFO:", end='\n\n')

    print(f"{Colors.CYAN}Expected answer: {expected_numbers}{Colors.RESET}")
    print(f"{Colors.CYAN}Program output:  {program_output}{Colors.RESET}")

    print(f"{Colors.CYAN}Program output:{Colors.RESET}")
    print(program_stdout)

    color_print(Colors.WHITE, f"[ total numbers expected ]: {len_ans}")
    color_print(Colors.WHITE, f"[ total numbers received ]: {len_out}") 
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