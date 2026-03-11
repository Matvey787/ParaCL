# Tester

A framework for conveniently connecting tests (CTest and Google Test) to CMake projects.  
Created by Matvey Galicin (https://github.com/Matvey787/)  

This tool provides a ready-to-use test configuration that connects to your repository's main CMake via add_subdirectory(path/to/tester). The framework configures two types of testing: CTest for end-to-end tests and Google Test for unit testing. The main goal is to avoid cluttering the main CMakeLists.txt, eliminate the need to remember gtest/ctest implementation details, and delegate all this logic to this "tester".  

***CTEST_PROGRAM_EXEC***  
This variable specifies the path to the executable program that will be tested. The framework will run this program with various input data and compare the output against expected values.  
`Mandatory variable if CTEST is enabled.`

***CTEST_INPUTS_DIR***  
The directory containing test input files. The framework scans this directory for files with the extension specified in `CTEST_INPUT_PATTERN`.  
`Mandatory variable if CTEST is enabled.`

***CTEST_ANSWERS_DIR***  
The directory containing test answer files. The framework scans this directory for files with the extension specified in `CTEST_INPUT_PATTERN`.  
`Mandatory variable if CTEST is enabled.`  

***CTEST_INPUT_PATTERN***  
The extension (without the dot) of files considered as test input data. The framework automatically finds all files with this extension in the `CTEST_INPUTS_DIR` directory and creates a separate test for each one.  
`Mandatory variable if CTEST is enabled.`  

***CTEST_ANSWER_PATTERN***  
The extension (without the dot) of files considered as test answer data. The framework automatically finds all files with this extension in the `CTEST_ANSWERS_DIR` directory and creates a separate test for each one.  
`Mandatory variable if CTEST is enabled.`  

***CTEST_TIMEOUT***  
The maximum test execution time in seconds. If a test runs longer than the specified time, it's considered failed (timeout). Default value is 30 seconds.  
`Mandatory variable if CTEST is enabled.`  

***GTEST_DIR***  
This specifies the path to the folder containing all the gtest files required for this tester.  
`Mandatory variable if GTEST is enabled.`  

***GTEST_OUTPUT_EXEC_FILE_NAME***  
The name of the executable file that will run the gtests.  
`Mandatory variable if GTEST is enabled.`  

***GTEST_ADDITIONAL_LIBS_NAMES***  
The tester is built on testing user libraries, which are the very libraries used in gtest files. Without specifying them, nothing will be compiled. These are the very libraries that you mainly use in cmake, where you add the tester as a subdirectory.  
`Mandatory variable if GTEST is enabled.`  

***LLVMCOV***  
Can be TRUE/FALSE. Flag that activates the creation of information about the percentage coverage of tests.  
`Mandatory variable if GTEST is enabled.`  

***GTEST_ADDITIONAL_LIBS_NAMES_ENABLE_LLVMCOV***  
Specifies which libraries from the GTEST_ADDITIONAL_LIBS_NAMES list should have coverage information collected. If set to 0, coverage information is collected for all libraries in the list. If a list of indices or names is specified, coverage is collected only for the specified libraries.  
`Mandatory variable if GTEST is enabled.`  

***CTESTER_SCRIPT***  
Path to the Python script that processes CTest tests. This script is responsible for running the program, comparing output with reference files, and generating test results.  
`Do not change without a special reason.`  

***GTEST_FETCH_URL***  
URL for downloading the Google Test library. The framework automatically downloads and integrates Google Test of the specified version, eliminating the need for prior system installation. `If the library is in the system, it will not be installed!`  
`Do not change without a special reason.`  

***TESTER_RELATIVE_PATH***  
Automatically calculated relative path from the project root directory to the tester's current location. Used for correctly displaying paths in messages.  
`Do not change without a special reason.`  

***INFO_USER_FONT_COLOR_MESSAGE*** - text color for informational messages  
`Do not change without a special reason.`  

***INFO_USER_BACK_COLOR_MESSAGE*** - background color for informational messages  
`Do not change without a special reason.`  

***STATUS_COLOR_MESSAGE*** - color for status messages  
`Do not change without a special reason.`  

***WARNING_COLOR_MESSAGE*** - color for warnings  
`Do not change without a special reason.`  

***FATAL_ERROR_COLOR_MESSAGE*** - color for critical errors  
`Do not change without a special reason.`  


```bash
# Generate building system with ctests
cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++ -G Ninja -DCTEST=ON

# Generate building system with gtests
cmake -S . -B build -DCMAKE_CXX_COMPILER=clang++ -G Ninja -DGTEST=ON

# Building gtests without coverage
cmake --build build/

# Building gtests with coverage
cmake --build build/ --target coverage

```
