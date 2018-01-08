# 批量生成所有服务器的makefile
project(summonerServers)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -fPIC -rdynamic -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall  -fPIC -rdynamic")

# 生成的可执行文件放到Linuxbin下，.a文件放到lib下
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/Linuxbin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# 链接库的搜索目录
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/External/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/External/libs/mysql
    ${CMAKE_CURRENT_SOURCE_DIR}/External/libs/jason
    ${CMAKE_CURRENT_SOURCE_DIR}/External/curl
)

# 将公共部分包含进来（它们会生成对应的静态目标）
include(Common/CMakeLists.txt)
include(External/CMakeLists.txt)

# 生成服务器
add_subdirectory(BattleServer)
add_subdirectory(CheckServer)
add_subdirectory(MatchServer)
add_subdirectory(CenterServer)
add_subdirectory(ServiceServer)
add_subdirectory(SessionServer)
add_subdirectory(UnionServer)
add_subdirectory(ChatServer)
