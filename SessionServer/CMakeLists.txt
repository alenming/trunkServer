# 设置要参与编译的代码目录
set(SessionSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
    ${CMAKE_CURRENT_SOURCE_DIR}/GameLogic
)
# 单独引用CsvLoader
set(SessionSrcs ${CMAKE_CURRENT_SOURCE_DIR}/../Common/GameConfig/CsvLoader.cpp )

include_directories(${SessionSrcDirs})

foreach(f ${SessionSrcDirs})
aux_source_directory(${f} SessionSrcs)
endforeach(f)

# 生成SessionServer
add_executable ( SessionServer ${SessionSrcs})

# 链接kxServer
target_link_libraries( SessionServer  KxServer)
