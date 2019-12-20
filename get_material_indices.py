# get_material_indices.py
# Get C++ int array containing indices of objects with
# materials ending in input string.

import os

fp = "assets/junwon/junwon"

def get_material_indices(material_names):
    print("Add the following to private variables in raytrace.h")
    for material_name in material_names:
        with open("{}.obj".format(fp), "r+") as f:
            index_count = 0
            material_indices = []
            for line in f:
                if "usemtl" == line[:6]:
                    index_count += 1
                    if "{}\n".format(material_name) == line[-len(material_name)-1:].lower():                
                        material_indices.append(index_count-1)
        print(
            "int {}_indices[{}] = {};".format(
                material_name, 
                len(material_indices), 
                str(material_indices).replace(
                    "[", "{"
                ).replace(
                    "]", "}"
                )
            )
        )
    print("Total Number of Objects: {}".format(index_count))

materials = [
    "glass",
    "marble",
    "floor",
    "gloss",
    "metal",
    "black",
    "bg",
    "matte"
]

materials.sort()
get_material_indices(materials)