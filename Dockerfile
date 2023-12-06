# Use Ubuntu 22.04 as base image
FROM ubuntu:22.04

# Update the package lists
RUN \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y liblua5.4-0 libssl3 cmake make git \
    zlib1g-dev libcurl4-openssl-dev curl lua5.4 lua5.4-dev \
    libz-dev rapidjson-dev liblua5.4-dev libssl-dev \
    libwebsocketpp-dev g++ libboost1.74-all-dev 

COPY . /root
WORKDIR /root

# Build the BeamMP-Server
RUN cmake . -DCMAKE_BUILD_TYPE=Release
RUN make

RUN ./BeamMP-Server-tests

# Start the BeamMP-Server when the container is run
CMD [ "./BeamMP-Server" ]