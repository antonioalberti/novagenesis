#!/bin/sh

docker run -itd --privileged -e CA_PATH="/home/ng/workspace/novagenesis/IO/Source"$1"/" -e CA_TYPE="Source" --name Source$1 ng-contentapp:latest

echo "Creating Source$1 Input/Output folder"

docker exec -w /home/ng/workspace/novagenesis/IO/ Source$1 mkdir Source$1

echo "Changing Source$1 Input/Output folder permissions"

docker exec -w /home/ng/workspace/novagenesis/IO/ Source$1 chmod -R 777 Source$1

echo "Copying App.ini from local Includes folder to Source$1 Input/Output folder"

#Copy local App.ini to the container to set up Source app parameters
docker cp Includes-ContentApp/App.ini Source$1:/home/ng/workspace/novagenesis/IO/Source$1/

echo "Copying make-photos.sh from local Includes folder to Source$1 Input/Output folder"

#Copy make-photos script to create the content inside the source container 
docker cp Includes-ContentApp/make-photos.sh Source$1:/home/ng/workspace/novagenesis/IO/Source$1/

echo "Copying image-generator software from local Includes folder to Source$1 Input/Output folder"

#Copy the image-generator program to the container  
docker cp Includes-ContentApp/image_generator Source$1:/home/ng/workspace/novagenesis/IO/Source$1/

echo "Sleeping 10 seconds"

sleep 10

echo "Running make-photos-sh at Source$1 Input/Output folder to create $2 photos with size $3 x $4 pixels"

sudo docker exec -w /home/ng/workspace/novagenesis/IO/Source$1/ Source$1 sh make-photos.sh $2 $3 $4
