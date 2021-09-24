FROM ubuntu:latest

MAINTAINER Antonio Marcos Alberti "antonioalberti@gmail.com"

RUN apt-get update -y && \
    apt-get install gcc g++ -y && \
    apt-get install cmake -y && \
    apt-get install net-tools -y

RUN cd /home && mkdir ng && cd ng && mkdir workspace && cd workspace && mkdir novagenesis && cd novagenesis

# Copy NovaGenesis folder to the container
COPY /Common /home/ng/workspace/novagenesis/Common
COPY /ContentApp /home/ng/workspace/novagenesis/ContentApp
COPY /GIRS /home/ng/workspace/novagenesis/GIRS
COPY /NRNCS /home/ng/workspace/novagenesis/NRNCS
COPY /PSS /home/ng/workspace/novagenesis/PSS
COPY /HTS /home/ng/workspace/novagenesis/HTS
COPY /PGCS /home/ng/workspace/novagenesis/PGCS
COPY /NBTestApp /home/ng/workspace/novagenesis/NBTestApp
COPY /IoTTestApp /home/ng/workspace/novagenesis/IoTTestApp
COPY /Scripts /home/ng/workspace/novagenesis/Scripts
COPY /IO /home/ng/workspace/novagenesis/IO
COPY CMakeLists.txt /home/ng/workspace/novagenesis/CMakeLists.txt

RUN cd /home/ng/workspace/novagenesis/ && mkdir Build

RUN cd /home/ng/workspace/novagenesis/Build && cmake .. -G "CodeBlocks - Unix Makefiles"

RUN cd /home/ng/workspace/novagenesis/Build && make
