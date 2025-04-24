cmake_minimum_required(VERSION 3.12)
project(server)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "Release")  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto")  # 链接时优化
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)  # IPO优化

# 包含目录设置
include_directories(
    /usr/include  # 显式添加常见路径
    /usr/local/include    # 第三方库常见安装位置
)

# 查找依赖库
find_package(Threads REQUIRED)
# 查找Drogon依赖
find_package(Drogon REQUIRED COMPOMENTS jsoncpp)

set(CONTROLLERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/controllers")
message(STATUS "控制器路径是: ${CONTROLLERS_DIR}")

# 添加可执行文件
add_executable(
    server
    DatabaseClient.cpp 
    logger.cpp 
    main.cpp 
    RedisClient.cpp 
    # ${CONTROLLERS_DIR}/btyGoose_TestCtrl.cc
)

target_link_libraries(server
    PRIVATE 
    mysqlcppconn
    Threads::Threads 
    gflags
    fmt 
    redis++
    hiredis
    Drogon::Drogon 
    ${JSONCPP_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)

