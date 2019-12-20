# process_texture.py
# Remove Ka from mtl file.

import os

texture_fp = "assets/junwon/junwon"

os.system("mv {}.mtl {}_raw.mtl".format(texture_fp, texture_fp))

with open("{}_raw.mtl".format(texture_fp), "r+") as source:
    with open("{}.mtl".format(texture_fp), "w+") as target:
        for line in source:
            if "Ka" != line[:2]:
                target.write(line)