FROM ubuntu:22.04 AS build1
RUN apt update -y && apt install -y g++ \
    make \
    git \
    cmake \
    libmysqlcppconn-dev \
    libjsoncpp-dev \
    libboost-all-dev\
    libgflags-dev \ 
    libfmt-dev \
    libspdlog-dev \
    libhiredis-dev 

WORKDIR /redispp
RUN git clone https://github.com/sewenew/redis-plus-plus.git
RUN mkdir -p /redispp/redis-plus-plus/build
WORKDIR /redispp/redis-plus-plus/build
RUN cmake ..
RUN make 
RUN make install
RUN ldconfig

COPY ./src* /src

WORKDIR /src
RUN make && mkdir logs
#DEBUG
# RUN make server_g && mkdir logs
#end
CMD ["/src/server","--flagfile=server.conf"]