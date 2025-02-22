FROM ubuntu:22.04 AS build1
RUN apt update -y && apt install -y g++ \
    make \
    libmysqlcppconn-dev \
    libjsoncpp-dev \
    libboost-system-dev
COPY ./src* /src
RUN echo '{"host" : "0.0.0.0","port" : 80}' > /src/serverConfig.json && \
    echo  '{"database" : "BeautyGoose","host" : "db","password" : "Password!1","port" : "3306","user" : "btyGooseUser"}' > /src/DBConfig.json

WORKDIR /src
RUN make
CMD ["/src/server"]