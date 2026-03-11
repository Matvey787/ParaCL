import sys
import subprocess

def color_text(text, color_code):
    return f"\033[{color_code}m{text}\033[0m"

def main():
    if len(sys.argv) != 4:
        print("Error: need 3 arguments")
        print("Usage: python tester.py <executable> <input_file> <expected_output_file>")
        return 1

    executable, input_file, expected_file = sys.argv[1:4]
    
    try:
        with open(expected_file, 'r') as f:
            expected_numbers = f.read().strip().split()
        
        with open(input_file, 'r') as f:
            result = subprocess.run(
                [executable], 
                stdin=f, 
                capture_output=True, 
                text=True,
                timeout=30
            )
        
        actual_numbers = result.stdout.strip().split()
        
        if actual_numbers == expected_numbers:
            print(color_text("✓ Test passed! Numbers match exactly", "32"))
            return 0
        else:
            print(color_text("✗ Test failed - numbers don't match", "31"))
            print("=" * 90)
            
            max_len = max(len(actual_numbers), len(expected_numbers))
            correct_answers_was_stopped = False
            incorrect_answers_was_stopped = False
            for i in range(max_len):
                expected_num = expected_numbers[i] if i < len(expected_numbers) else None
                actual_num = actual_numbers[i] if i < len(actual_numbers) else None
                
                if expected_num == actual_num:
                    incorrect_answers_was_stopped = True
                    if correct_answers_was_stopped:
                        print()
                        correct_answers_was_stopped = False
                        
                    print(f"{color_text(expected_num, '32')} ", end='')

                else:
                    correct_answers_was_stopped = True
                    if incorrect_answers_was_stopped:
                        print()
                        incorrect_answers_was_stopped = False
                    if actual_num is None:
                        print(f"{i+1:06d}: {color_text('[MISSING]  ' + expected_num, '31')}", end='')
                    elif expected_num is None:
                        print(f"{i+1:06d}: {color_text('[EXTRA]    ' + actual_num, '33')}", end='')
                    else:
                        print(f"{i+1:06d}: {color_text('[MISMATCH] ' + expected_num, '31')} {color_text('> ' + actual_num, '33')}", end='')
            print()
            return 1
            
    except FileNotFoundError as e:
        print(color_text(f"Error: File not found - {e}", "31"))
        return 1
    except subprocess.TimeoutExpired:
        print(color_text("Error: Program execution timeout", "31"))
        return 1
    except Exception as e:
        print(color_text(f"Error: {e}", "31"))
        return 1

if __name__ == "__main__":
    sys.exit(main())