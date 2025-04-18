import os
import sys
import json
import bpy


def convert_glb_to_obj(input_file, output_file):
    """Convert a GLB file to OBJ format.

    Args:
        input_file (str): Path to the input GLB file.
        output_file (str): Path to the output OBJ file.
    """
    # Clear existing data
    bpy.ops.wm.read_factory_settings(use_empty=True)

    # Import the GLB file
    bpy.ops.import_scene.gltf(filepath=input_file)

    # Get all objects from the scene
    all_objects = [obj for obj in bpy.context.scene.objects]

    # Create a new collection
    collection = bpy.data.collections.new("ExportCollection")
    bpy.context.scene.collection.children.link(collection)

    # Link objects to the collection
    for obj in all_objects:
        collection.objects.link(obj)

    # Export as OBJ
    bpy.ops.wm.obj_export(
        filepath=output_file,
        export_selected_objects=False,
        export_triangulated_mesh=True,
        export_materials=True
    )

    print(f"Conversion complete: {input_file} → {output_file}")


def parse_mtl_content(content):
    """Parses minimal MTL content and extracts relevant material info."""
    data = {
        "shininess": 0.0,
        "tint": [0.8, 0.8, 0.8, 1.0],  # default RGBA
        "transparent": False
    }

    for line in content.splitlines():
        tokens = line.strip().split()
        if not tokens:
            continue

        if tokens[0] == 'Ns':
            data['shininess'] = float(tokens[1])
        elif tokens[0] == 'Kd':  # Diffuse color
            r, g, b = map(float, tokens[1:4])
            data['tint'] = [r, g, b, 1.0]  # alpha set later by 'd'
        elif tokens[0] == 'd':  # Opacity
            alpha = float(tokens[1])
            data['tint'][3] = alpha
            data['transparent'] = alpha < 1.0

    return data


def update_json_config(config_path, obj_files, mtl_files, output_path):
    """Update the JSON configuration file with new mesh and material entries."""
    # Load the existing configuration
    with open(config_path, 'r') as f:
        config = json.load(f)

    existing_meshes = config['scene']['assets']['meshes']
    existing_world_entries = config['scene'].get('world', [])

    added_meshes = []

    # Update meshes and collect new ones
    for obj_file in obj_files:
        obj_basename = os.path.basename(obj_file)
        mesh_name = os.path.splitext(obj_basename)[0]
        relative_path = f"{output_path}/{obj_basename}"

        if mesh_name not in existing_meshes:
            config['scene']['assets']['meshes'][mesh_name] = relative_path
            print(f"Added mesh: {mesh_name} -> {relative_path}")
            added_meshes.append(mesh_name)

    # Add world entries for new meshes
    for mesh_name in added_meshes:
        already_present = any(
            entry.get("components", [{}])[0].get("mesh") == mesh_name
            for entry in existing_world_entries
        )

        if not already_present:
            world_entry = {
                "position": [0, 30, 0],
                "rotation": [0, 0, 0],
                "scale": [1, 1, 1],
                "components": [
                    {
                        "type": "Mesh Renderer",
                        "mesh": mesh_name,
                        "material": "moon"
                    }
                ]
            }
            config['scene']['world'].append(world_entry)
            print(f"🧱 Added world entry for mesh: {mesh_name}")

    # Update materials
    for mtl_file in mtl_files:
        mtl_basename = os.path.basename(mtl_file)
        material_name = os.path.splitext(mtl_basename)[0]

        with open(mtl_file, 'r') as f:
            mtl_content = f.read()

        mtl_data = parse_mtl_content(mtl_content)

        material_entry = {
            "type": "tinted",
            "shader": "tinted",
            "pipelineState": {
                "faceCulling": {
                    "enabled": False
                },
                "depthTesting": {
                    "enabled": True
                }
            },
            "tint": mtl_data['tint'],
            "transparent": mtl_data['transparent'],
            "shininess": mtl_data['shininess']
        }

        if material_name not in config['scene']['assets']['materials']:
            config['scene']['assets']['materials'][material_name] = material_entry
            print(f"Added material: {material_name}")

    # Save back to config
    with open(config_path, 'w') as f:
        json.dump(config, f, indent=4)

    print(f"✅ Updated JSON configuration: {config_path}")


def main():
    argv = sys.argv
    argv = argv[argv.index("--") + 1:]

    args = {
        "input_dir": argv[0],
        "output_dir": argv[1],
        "config_file": argv[2]
    }

    # Create output directory if it doesn't exist
    os.makedirs(args["output_dir"], exist_ok=True)

    # Get all GLB files in the input directory
    glb_files = [os.path.join(args["input_dir"], f) for f in os.listdir(args["input_dir"])
                 if f.lower().endswith('.glb')]

    if not glb_files:
        print(f"No GLB files found in {args['input_dir']}")
        return

    obj_files = []
    mtl_files = []

    # Convert each GLB file to OBJ
    for glb_file in glb_files:
        filename = os.path.basename(glb_file)
        name_without_ext = os.path.splitext(filename)[0]
        obj_file = os.path.join(args["output_dir"], f"{name_without_ext}.obj")

        convert_glb_to_obj(glb_file, obj_file)
        obj_files.append(obj_file)

        # Check for corresponding MTL file
        mtl_file = os.path.join(args["output_dir"], f"{name_without_ext}.mtl")
        if os.path.exists(mtl_file):
            mtl_files.append(mtl_file)

    # Get existing OBJ files in the output directory that weren't just created
    existing_obj_files = [os.path.join(args["output_dir"], f) for f in os.listdir(args["output_dir"])
                          if f.lower().endswith('.obj') and os.path.join(args["output_dir"], f) not in obj_files]
    obj_files.extend(existing_obj_files)

    # Get existing MTL files in the output directory that weren't just created
    existing_mtl_files = [os.path.join(args["output_dir"], f) for f in os.listdir(args["output_dir"])
                          if f.lower().endswith('.mtl') and os.path.join(args["output_dir"], f) not in mtl_files]
    mtl_files.extend(existing_mtl_files)

    # Update the JSON configuration file
    update_json_config(args["config_file"], obj_files,
                       mtl_files, args["output_dir"])


if __name__ == "__main__":
    # Check if script is running in Blender
    if bpy is None:
        print("This script must be run from within Blender")
        sys.exit(1)

    main()
