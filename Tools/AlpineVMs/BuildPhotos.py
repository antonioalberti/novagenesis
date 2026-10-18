import numpy as np
from PIL import Image
import random, subprocess, sys

qtdImagens = int(sys.argv[2])
width = 200
height = 200
channels = 3
h = subprocess.check_output("hostname", shell=True).decode('utf-8').strip()

if sys.argv[1] == "different":
    for i in range(qtdImagens):
        img = np.zeros((height, width, channels), dtype=np.uint8)
        for y in range(img.shape[0]):
            for x in range(img.shape[1]):
                img[y][x][0] = random.randrange(255)
                img[y][x][1] = random.randrange(255)
                img[y][x][2] = random.randrange(255)
        file = Image.fromarray(img)
        file.save(str('%05d' % i) + "-" + h + ".jpg")

if sys.argv[1] == "equal":
    for i in range(qtdImagens):
        img = np.zeros((height, width, channels), dtype=np.uint8)
        for y in range(img.shape[0]):
            for x in range(img.shape[1]):
                img[y][x][0] = int((255/qtdImagens)*i)
                img[y][x][1] = int((255/qtdImagens)*i)
                img[y][x][2] = int((255/qtdImagens)*i)
        file = Image.fromarray(img)
        file.save(str('%05d' % i) + "-" + h + ".jpg")