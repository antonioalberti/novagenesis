FROM ubuntu:latest

MAINTAINER Antonio Marcos Alberti "antonioalberti@gmail.com"

# Minimal install — compiler + cmake + debug tools + network tools
RUN apt-get update -y && \
    apt-get install gcc g++ -y && \
    apt-get install cmake -y && \
    apt-get install gdb valgrind strace -y && \
    apt-get install net-tools -y && \
    apt-get install tcpdump -y && \
    rm -rf /var/lib/apt/lists/*

# Create NovaGenesis folders
RUN mkdir -p /home/ng/workspace/novagenesis

# Copy only what CMakeLists.txt needs
COPY /Common /home/ng/workspace/novagenesis/Common
COPY /PGCS /home/ng/workspace/novagenesis/PGCS
COPY /ContentApp /home/ng/workspace/novagenesis/ContentApp
COPY /NRNCS /home/ng/workspace/novagenesis/NRNCS
COPY /IoTTestApp /home/ng/workspace/novagenesis/IoTTestApp
COPY /NBTestApp /home/ng/workspace/novagenesis/NBTestApp
COPY /IO /home/ng/workspace/novagenesis/IO
COPY /CMakeLists.txt /home/ng/workspace/novagenesis/CMakeLists.txt

# Build NovaGenesis from source
RUN cd /home/ng/workspace/novagenesis/ && mkdir Build
RUN cd /home/ng/workspace/novagenesis/Build && cmake .. -G "CodeBlocks - Unix Makefiles"
RUN cd /home/ng/workspace/novagenesis/Build && make

# Entry point — PGCS runs directly
WORKDIR /home/ng/workspace/novagenesis/Build
COPY /Docker/PGCS-Only/Run.sh /home/ng/workspace/novagenesis/Run.sh
RUN chmod +x /home/ng/workspace/novagenesis/Run.sh

ENTRYPOINT ["sh", "/home/ng/workspace/novagenesis/Run.sh"]
CMD ["/home/ng/workspace/novagenesis/IO/PGCS/", "0", "Intra_Domain", "-de", "Ethernet", "eth0", "1200"]