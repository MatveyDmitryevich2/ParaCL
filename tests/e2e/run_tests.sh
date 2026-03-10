#!/bin/bash

PCL="./build/ParaCL"
TESTS_DIR="./tests/e2e"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
TOTAL=0
PASSED=0

run_success_test() {
    local test_dir=$1
    local test_name=$(basename "$test_dir")
    local program_file="$test_dir/program.pcl"
    local input_file="$test_dir/input.txt"
    local expected_file="$test_dir/expected.txt"

    echo -n "  Testing $test_name... "

    if [ -f "$input_file" ]; then
        $PCL "$program_file" < "$input_file" > "$test_dir/output.tmp" 2>&1
    else
        $PCL "$program_file" > "$test_dir/output.tmp" 2>&1
    fi

    if diff -w -B "$expected_file" "$test_dir/output.tmp" > "$test_dir/diff.tmp"; then
        echo -e "${GREEN}PASSED${NC}"
        rm -f "$test_dir/output.tmp" "$test_dir/diff.tmp"
        return 0
    else
        echo -e "${RED}FAILED${NC}"
        cat "$test_dir/diff.tmp"
        return 1
    fi
}
run_error_test() {
    local test_dir=$1
    local test_name=$(basename "$test_dir")
    local program_file="$test_dir/program.pcl"
    local expected_file="$test_dir/expected.txt"

    echo -n "  Testing $test_name... "

    output=$($PCL "$program_file" 2>&1)
    first_line=$(echo "$output" | head -n1)


    expected_type=$(cat "$expected_file" | head -n1 | sed 's/://')

    if [[ "$first_line" == "$expected_type:"* ]]; then
        echo -e "${GREEN}PASSED${NC}"
        return 0
    else
        echo -e "${RED}FAILED${NC}"
        echo "    Expected type: '$expected_type'"
        echo "    Got: '$first_line'"
        echo "    Full output:"
        echo "$output" | sed 's/^/    /'
        return 1
    fi
}

find "$TESTS_DIR" -name "*.tmp" -delete
find "$TESTS_DIR" -name "*.diff" -delete


echo "Running successful tests..."
for test_dir in "$TESTS_DIR/success"/*/; do
    if [ -d "$test_dir" ]; then
        TOTAL=$((TOTAL + 1))
        if run_success_test "$test_dir"; then
            PASSED=$((PASSED + 1))
        fi
    fi
done

echo -e "\nRunning error tests..."
for test_dir in "$TESTS_DIR/errors"/*/; do
    if [ -d "$test_dir" ]; then
        TOTAL=$((TOTAL + 1))
        if run_error_test "$test_dir"; then
            PASSED=$((PASSED + 1))
        fi
    fi
done

echo -e "\n${GREEN}${PASSED}${NC}/${TOTAL} tests passed"

if [ $PASSED -eq $TOTAL ]; then
    exit 0
else
    exit 1
fi
