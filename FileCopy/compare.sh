#!/bin/bash

# Define the source and target directories
SRC="SRC"
TARGET="TARGET"

# Loop through each file in the SRC directory
for file in "$SRC"/*; do
    filename=$(basename "$file")
    tmp_file="$TARGET/${filename}"
    
    # Check if the corresponding .TMP file exists
    if [ -f "$tmp_file" ]; then
        echo "Comparing SRC/$filename with TARGET/${filename}..."
        # Perform the diff comparison
        diff "$file" "$tmp_file"
    else
        echo "No corresponding .TMP file for $filename in $TARGET"
    fi
done

echo "Comparison complete."
