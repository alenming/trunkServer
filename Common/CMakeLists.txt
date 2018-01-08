### 公共目录CMakeLists.txt
# 包含协议头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common/Protocol)

###### 设置通用目录变量
set(CommDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/GameCommon
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/GameConfig
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/GameModel
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/GameUser
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/SimpleCocos
)
# Game公共库 + GameUser + GameModel +  GameConfig
include_directories(${CommDirs})
foreach(f ${CommDirs})
aux_source_directory(${f} CommSrcs)
endforeach(f)
add_library ( Common ${CommSrcs})

####### DBDriver公用库
# 设置通用目录变量
set(DBDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/External/mysqld
    ${CMAKE_CURRENT_SOURCE_DIR}/External/Linuxhiredis
    ${CMAKE_CURRENT_SOURCE_DIR}/Common/DBDriver
)
# 设置头文件搜索路径
include_directories(${DBDirs})
# 添加源码
foreach(f ${DBDirs})
aux_source_directory(${f} DBSrcs)
endforeach(f)
# 生成dbdriver库
add_library ( DBDriver ${DBSrcs})
