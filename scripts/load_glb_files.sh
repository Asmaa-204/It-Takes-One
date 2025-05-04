#!/bin/zsh

# Check for required arguments
if [ "$#" -ne 3 ]; then
    echo "Usage: ./file input_directory output_directory config_file"
    exit 1
fi

# Assign arguments to variables
input_dir="$1"
output_dir="$2"
config_file="$3"

# Step 1: Convert GLB to OBJ using Blender
blender --background --python ./scripts/glb_to_obj_converter.py -- "$input_dir" "$output_dir" "$config_file"

# Step 2: Remove all .mtl files in the output directory
find "$output_dir" -type f -name "*.mtl" -delete
echo "✅ Removed all .mtl files."

# Step 3: Clean OBJ files in the output directory
find "$output_dir" -type f -name "*.obj" | while read -r obj_file; do
    sed -i '/^mtllib /d' "$obj_file"
    sed -i '/^usemtl /d' "$obj_file"
    echo "✂️  Cleaned $obj_file"
done
