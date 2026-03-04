
PCL_INTERP="../../build/ParaCL"

if [ ! -f "$PCL_INTERP" ]; then
    echo "ERROR: Interpreter not found at $PCL_INTERP"
    echo "   Current directory: $(pwd)"
    echo "   Contents of ../../build/:"
    ls -la ../../build/
    exit 1
fi

echo "Found interpreter: $PCL_INTERP"
echo "==================================="


TOTAL=0
PASSED=0
FAILED=0


TEMP_OUT=$(mktemp)
TEMP_EXPECTED=$(mktemp)
TEMP_OUT_CLEAN=$(mktemp)
TEMP_EXPECTED_CLEAN=$(mktemp)

for test_dir in */ ; do
    test_dir=${test_dir%/}

    if [ ! -d "$test_dir" ]; then
        continue
    fi

    program_file="$test_dir/program.pcl"
    if [ ! -f "$program_file" ]; then
        continue
    fi

    echo -n "Test: $test_dir "

    expected_file="$test_dir/expected.txt"
    if [ ! -f "$expected_file" ]; then
        echo "FAILED (no expected.txt)"
        continue
    fi

    input_file="$test_dir/input.txt"

    if [ -f "$input_file" ]; then
        "$PCL_INTERP" "$program_file" < "$input_file" > "$TEMP_OUT" 2>&1
    else
        "$PCL_INTERP" "$program_file" > "$TEMP_OUT" 2>&1
    fi

    tr -d '\r' < "$TEMP_OUT" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' | grep -v '^$' > "$TEMP_OUT_CLEAN"
    tr -d '\r' < "$expected_file" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' | grep -v '^$' > "$TEMP_EXPECTED_CLEAN"


    if diff -q "$TEMP_EXPECTED_CLEAN" "$TEMP_OUT_CLEAN" > /dev/null; then
        echo "PASSED"
        PASSED=$((PASSED + 1))
    else
        echo "FAILED"
        echo "   Expected (cleaned):"
        cat "$TEMP_EXPECTED_CLEAN" | sed 's/^/     /'
        echo "   Got (cleaned):"
        cat "$TEMP_OUT_CLEAN" | sed 's/^/     /'

        echo "   Differences:"
        diff -u "$TEMP_EXPECTED_CLEAN" "$TEMP_OUT_CLEAN" | sed 's/^/     /'

        FAILED=$((FAILED + 1))
    fi

    TOTAL=$((TOTAL + 1))
done

rm -f "$TEMP_OUT" "$TEMP_EXPECTED" "$TEMP_OUT_CLEAN" "$TEMP_EXPECTED_CLEAN"

echo "==================================="
echo "RESULTS:"
echo "   Total tests: $TOTAL"
echo "   Passed: $PASSED"
echo "   Failed: $FAILED"
echo "==================================="

if [ $FAILED -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed"
    exit 1
fi
