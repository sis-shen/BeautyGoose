FROM ubuntu:22.04 AS build1

RUN apt update -y && apt install -y git 
WORKDIR /redispp
RUN git clone https://github.com/sewenew/redis-plus-plus.git
RUN mkdir -p /redispp/redis-plus-plus/build
WORKDIR /
RUN git clone https://github.com/drogonframework/drogon.git
WORKDIR /drogon
RUN git submodule update --init

RUN sed -i 's|http://archive.ubuntu.com/ubuntu|http://mirrors.aliyun.com/ubuntu|g' /etc/apt/sources.list && \
    apt-get update


RUN apt-get update -y && apt-get install -y g++ \
    make \
    cmake \
    libmysqlcppconn-dev \
    libjsoncpp-dev \
    libboost-all-dev\
    libgflags-dev \ 
    libfmt-dev \
    libspdlog-dev \
    libhiredis-dev \
    libssl-dev \
    uuid-dev \
    zlib1g-dev \
    libprotobuf-dev \
    protobuf-compiler-grpc \
    libgrpc-dev \
    libgrpc++-dev \
    libcpprest-dev


WORKDIR /redispp/redis-plus-plus/build
RUN cmake .. && make && make install
RUN ldconfig

WORKDIR /drogon
RUN mkdir -p build && cd ./build  && cmake .. && make && make install
RUN ldconfig

COPY ./src* /src

WORKDIR /src
RUN make && mkdir -p logs/drogon
RUN ls -al
#DEBUG
# RUN make server_g && mkdir logs
#end
CMD ["/src/server","--flagfile=server.conf"]