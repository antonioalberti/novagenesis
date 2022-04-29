FROM ubuntu:latest

MAINTAINER Antonio Marcos Alberti "antonioalberti@gmail.com"

# Initial installations
RUN apt-get update -y && \
    apt-get install gcc g++ -y && \
    apt-get install cmake -y && \
    apt-get install net-tools -y

# Prepare multiprocess supervisor
RUN apt-get update && apt-get install -y supervisor
RUN mkdir -p /var/log/supervisor

# Create NovaGenesis folders
RUN cd /home && mkdir ng && cd ng && mkdir workspace && cd workspace && mkdir novagenesis && cd novagenesis && mkdir Scripts && cd Scripts && mkdir Docker && cd Docker && mkdir Results

# Copy NovaGenesis folders to the container
COPY /Common /home/ng/workspace/novagenesis/Common
COPY /NBTestApp /home/ng/workspace/novagenesis/NBTestApp
COPY /PGCS /home/ng/workspace/novagenesis/PGCS
COPY /Scripts /home/ng/workspace/novagenesis/Scripts
COPY /IO /home/ng/workspace/novagenesis/IO
COPY /Docker/NBTestApp/CMakeLists.txt /home/ng/workspace/novagenesis/CMakeLists.txt

# Build NovaGenesis from source code
RUN cd /home/ng/workspace/novagenesis/ && mkdir Build
RUN cd /home/ng/workspace/novagenesis/Build && cmake .. -G "CodeBlocks - Unix Makefiles"
RUN cd /home/ng/workspace/novagenesis/Build && make

# Copy multiprocess config and running script as suggested here: https://tech.paulcz.net/blog/multi-process-docker-images-done-right/ and here: https://docs.docker.com/config/containers/multi-service_container/
COPY /Docker/NBTestApp/Run.sh Run.sh
COPY /Docker/NBTestApp/supervisord.conf /home/ng/workspace/novagenesis/supervisord.conf

# Run
CMD ["sh","Run.sh"]
