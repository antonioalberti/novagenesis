# BuildPhotos — Generates test JPEG images for NovaGenesis ContentApp testing.
#
# Dependencies:
#   sudo apt-get install python3-numpy python3-pil
#   pip3 install numpy Pillow
#
# Usage:
#   python3 BuildPhotos.py different <count> [width] [height]
#   python3 BuildPhotos.py equal     <count> [width] [height]
#
#   python3 BuildPhotos.py different 100          # 100 random images, 200x200
#   python3 BuildPhotos.py different 50 800 600   # 50 random images, 800x600
#   python3 BuildPhotos.py equal 10               # 10 uniform grayscale images, 200x200

import numpy as np
from PIL import Image
import random
import subprocess
import sys

image_count = int(sys.argv[2])

# Image size (configurable via command line, defaults to 200x200)
width = int(sys.argv[3]) if len(sys.argv) > 3 else 200
height = int(sys.argv[4]) if len(sys.argv) > 4 else 200
channels = 3

hostname = subprocess.check_output("hostname", shell=True).decode().strip()

mode = sys.argv[1]

if mode == "different":
    for i in range(image_count):
        img = np.zeros((height, width, channels), dtype=np.uint8)

        for y in range(img.shape[0]):
            for x in range(img.shape[1]):
                img[y][x][0] = random.randrange(255)
                img[y][x][1] = random.randrange(255)
                img[y][x][2] = random.randrange(255)

        file = Image.fromarray(img)
        file.save(str('%05d' % i) + "-" + hostname + ".jpg")

elif mode == "equal":
    for i in range(image_count):
        img = np.zeros((height, width, channels), dtype=np.uint8)

        for y in range(img.shape[0]):
            for x in range(img.shape[1]):
                img[y][x][0] = (255 / image_count) * i
                img[y][x][1] = (255 / image_count) * i
                img[y][x][2] = (255 / image_count) * i

        file = Image.fromarray(img)
        file.save(str('%05d' % i) + "-" + hostname + ".jpg")