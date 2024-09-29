#!/bin/bash
# This line tells the system to use bash to interpret the script.

# Enable exit on error
set -e

# Set variables
EXE="./fileclient comp117-02 0 0 SRC"  # Your executable
DIR1="SRC"         # First directory to compare
DIR2="TARGET"         # Second directory to compare
OUT="out1.txt"    # Output file from your program
EXPECTED="out2.txt"  # Expected output for comparison
sorted_out="sorted_out1.txt"
sorted_expected="sorted_out2.txt"


if [ -f "$OUT" ]; then
    rm "$OUT"  # Remove the file
fi

if [ -f "$EXPECTED" ]; then
    rm "$EXPECTED"  # Remove the file
fi
if [ -f "$sorted_out" ]; then
    rm "$sorted_out"  # Remove the file
fi
if [ -f "$sorted_expected" ]; then
    rm "$sorted_expected"  # Remove the file
fi
# Step 1: Run the executable and save its output
$EXE > $OUT

# Step 2: Compare files in the two directories and generate expected output
for file in "$DIR1"/*; do
    basefile=$(basename "$file")
    if [ -e "$DIR2/$basefile" ]; then
        if ! diff -q "$DIR1/$basefile" "$DIR2/$basefile" > /dev/null; then
            echo "$basefile 0" >> $EXPECTED
        else
            echo "$basefile 1" >> $EXPECTED
        fi
    else
        echo "File $basefile exists in $DIR1 but not in $DIR2"
    fi
done

# Step 3: Sort both output files and compare them
echo "Sorting and comparing outputs..."

sort $OUT > $sorted_out
sort $EXPECTED > $sorted_expected

# Step 4: Diff the sorted outputs
if diff -q $sorted_out $sorted_expected > /dev/null; then
    echo "Outputs match!"
else
    echo "Outputs differ!"
    echo "Differences:"
    diff $sorted_out $sorted_expected
fi

rm $OUT $EXPECTED 
