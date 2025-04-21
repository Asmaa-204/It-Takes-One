import os
import json

# Load the existing JSON config
# Replace with the path to your config file
config_file = "../../../config/map.jsonc"
with open(config_file, "r") as f:
    config = json.load(f)

# Get all .obj files in the current directory
obj_files = [f for f in os.listdir() if f.endswith('.obj')]

# # Add .obj files to the meshes section and world section
# for obj_file in obj_files:
#     # Add to meshes
#     # Remove extension to use as the mesh name
#     mesh_name = os.path.splitext(obj_file)[0].replace('_', '-')
#     mesh_entry = {
#         "path": f"assets/models/map/{obj_file}",
#         "isDynamic": False
#     }
#     config["scene"]["assets"]["meshes"][mesh_name] = mesh_entry
#
#     # Add to world
#     world_entry = {
#         "position": [0, 0, 0],  # Default position for all meshes
#         "scale": [1, 1, 1],
#         "components": [
#             {
#                 "type": "Mesh Renderer",
#                 "mesh": mesh_name,
#                 "material": "map-general"  # Use the "map-general" material
#             }
#         ]
#     }
#     config["scene"]["world"].append(world_entry)

for entry in config["scene"]["world"]:

    data = {"type": "Rigid Body", "mass": 0}
    entry["components"].append(data)


# Save the updated config back to the file
with open(config_file, "w") as f:
    json.dump(config, f, indent=2)

print(f"Successfully updated {len(obj_files)} .obj files into the config.")
