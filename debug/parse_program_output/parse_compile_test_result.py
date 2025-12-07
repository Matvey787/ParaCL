#!/usr/bin/python3

import sys
import subprocess
import os

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
        print(f"Usage: {sys.argv[0]} <paracl_exe> <test>.cl <answer>.ans")
        return 1

    paracl_executable, test_input, test_answer = sys.argv[1:4]

    if not os.path.exists(paracl_executable):
        color_print(Colors.RED, f"Error: Compiler executable '{paracl_executable}' not found")
        return 1
        
    if not os.path.exists(test_input):
        color_print(Colors.RED, f"Error: Test file '{test_input}' not found")
        return 1
        
    if not os.path.exists(test_answer):
        color_print(Colors.RED, f"Error: Answer file '{test_answer}' not found")
        return 1

    try:
        with open(test_answer, 'r') as f:
            answer_content = f.read().strip()
    except Exception as e:
        color_print(Colors.RED, f"Error reading answer file: {e}")
        return 1

    expect_compilation_death = False
    expected_compile_exit_code = 0

    if answer_content.startswith("DEATH_WITH:"):
        expect_compilation_death = True
        parts = answer_content.split(':', 2)
        if len(parts) < 2:
            color_print(Colors.RED, "Error: Bad format in .ans file. Expected 'DEATH_WITH:exit_code'")
            return 1
        try:
            expected_compile_exit_code = int(parts[1].strip())
        except ValueError:
            color_print(Colors.RED, f"Error: Invalid exit code in .ans file: {parts[1]}")
            return 1
    else:
        expect_compilation_death = False
        expected_compile_exit_code = 0

    compile_result = subprocess.run(
        [paracl_executable, "--compile", test_input, "test.o"],
        capture_output=True,
        text=True
    )
    
    if expect_compilation_death:
        if compile_result.returncode == expected_compile_exit_code:
            color_print(Colors.GREEN, "TEST PASSED - Expected compilation death occurred")
            return 0
        else:
            color_print(Colors.RED, "TEST FAILED - Expected compilation death but compilation succeeded")
            color_print(Colors.GREEN, f"[ expected exit code ]: {expected_compile_exit_code}")
            color_print(Colors.RED,   f"[ compiler exit code ]: {compile_result.returncode}")
            if compile_result.stdout:
                print(f"Compiler stdout: {compile_result.stdout}")
            if compile_result.stderr:
                print(f"Compiler stderr: {compile_result.stderr}")
            return 1

    if compile_result.returncode != 0:
        color_print(Colors.RED, f"Compilation failed with exit code {compile_result.returncode}")
        print(f"Compiler stdout: {compile_result.stdout}")
        print(f"Compiler stderr: {compile_result.stderr}")
        return 1

    if not os.path.exists("a.out"):
        color_print(Colors.RED, "Error: Compiled executable 'a.out' not found")
        return 1

    result = subprocess.run(
        ["./a.out"],
        capture_output=True,
        text=True
    )

    if os.path.exists("a.out"):
        os.remove("a.out")

    program_stdout = result.stdout
    program_stderr = result.stderr

    if program_stderr:
        color_print(Colors.YELLOW, "Program stderr:")
        print(program_stderr)

    expected_numbers = extract_numbers(answer_content)        
    program_output = extract_numbers(program_stdout)

    try:
        assert expected_numbers == program_output, f"Expected {expected_numbers}, got {program_output}"
        assert result.returncode == 0, f"Program exited with non-zero code: {result.returncode}"
        
        color_print(Colors.GREEN, "TEST PASSED - All assertions passed")
        return 0
        
    except AssertionError as e:
        color_print(Colors.RED, f"TEST FAILED - Assertion error: {e}")
        
        color_print(Colors.WHITE, "\n" + "="*50)
        color_print(Colors.WHITE, "DETAILED OUTPUT ANALYSIS")
        color_print(Colors.WHITE, "="*50)

        errors = 0
        good_program_out = 0
        len_out = len(program_output)
        len_ans = len(expected_numbers)

        max_len = max(len_out, len_ans)
        for i in range(max_len):
            if i < len_out and i < len_ans:
                if program_output[i] == expected_numbers[i]:
                    good_program_out += 1
                    color_print(Colors.WHITE, f"[{i}]")
                    color_print(Colors.GREEN, f"✓ program: {program_output[i]}, expected: {expected_numbers[i]}")
                else:
                    errors += 1
                    color_print(Colors.WHITE, f"[{i}]")
                    color_print(Colors.RED, f"✗ program: {program_output[i]}, expected: {expected_numbers[i]}")
            elif i < len_out:
                errors += 1
                color_print(Colors.WHITE, f"[{i}]")
                color_print(Colors.RED, f"✗ program: {program_output[i]}, expected: NONE")
            else:
                errors += 1
                color_print(Colors.WHITE, f"[{i}]")
                color_print(Colors.RED, f"✗ program: NONE, expected: {expected_numbers[i]}")

        color_print(Colors.WHITE, "\n" + "="*50)
        color_print(Colors.WHITE, "STATISTICS")
        color_print(Colors.WHITE, "="*50)

        color_print(Colors.CYAN, f"Expected numbers: {expected_numbers}")
        color_print(Colors.CYAN, f"Program output:   {program_output}")

        color_print(Colors.WHITE, f"\nTotal expected numbers: {len_ans}")
        color_print(Colors.WHITE, f"Total output numbers:   {len_out}")
        color_print(Colors.WHITE, f"Correct matches:        {good_program_out}")
        color_print(Colors.WHITE, f"Errors:                 {errors}")

        if len_ans > 0:
            accuracy = (good_program_out / len_ans) * 100
            error_rate = (errors / max(len_ans, 1)) * 100
        else:
            accuracy = 100.0 if len_out == 0 else 0.0
            error_rate = 0.0 if len_out == 0 else 100.0

        color_print(Colors.GREEN, f"Accuracy: {accuracy:.1f}%")
        color_print(Colors.RED,   f"Error rate: {error_rate:.1f}%")

        return 1

if __name__ == "__main__":
    sys.exit(main())
