FROM ubuntu:22.04 AS build1
RUN apt update -y && apt install -y g++ \
    make \
    libmysqlcppconn-dev \
    libjsoncpp-dev \
    libboost-system-dev\
    libgflags-dev \ 
    libfmt-dev \
    libspdlog-dev
COPY ./src* /src

WORKDIR /src
RUN make && mkdir logs
CMD ["/src/server","--flagfile=server.conf"]