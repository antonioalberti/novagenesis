#@Elcio
import numpy as np
from PIL import Image
import random,subprocess,sys

"""
antes de rodar o scripts precisamos instalar as bibliotecas
sudo apt-get install python-numpy
sudo apt-get install python-imaging

exemplo para executar o script
python CriarFotos.py different 100

ou

python CriarFotos.py equal 100


"""
#Trocar aqui o valor da quantidade de imagens criadas


qtdImagens = int(sys.argv[2])
# Image size
width = 200
height = 200
channels = 3
h = subprocess.check_output("hostname", shell=True)#Grava o nome do host no nome da foto criada


if(sys.argv[1] == "different"):
	for i in range(qtdImagens):
		# Create an empty image
		img = np.zeros((height, width, channels), dtype=np.uint8)
		
		for y in range(img.shape[1]):
			for x in range(img.shape[1]):
				img[y][x][0] = random.randrange(255)
				img[y][x][1] = random.randrange(255)
				img[y][x][2] = random.randrange(255)

		file = Image.fromarray(img)
		file.save(str('%05d' % i) +"-" + h[:-1] +".jpg")

#########################################################################################

if(sys.argv[1] == "equal"):
	for i in range(qtdImagens):
		# Create an empty image
		img = np.zeros((height, width, channels), dtype=np.uint8)
	
		for y in range(img.shape[1]):
			for x in range(img.shape[1]):
				img[y][x][0] = (255/qtdImagens)*i
				img[y][x][1] = (255/qtdImagens)*i
				img[y][x][2] = (255/qtdImagens)*i


		file = Image.fromarray(img)
		file.save(str('%05d' % i) +"-" + h[:-1] +".jpg")


