# 设置要参与编译的代码目录
set(UnionSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
)

include_directories(${UnionSrcDirs})

foreach(f ${UnionSrcDirs})
aux_source_directory(${f} UnionSrcs)
endforeach(f)

add_executable ( UnionServer ${UnionSrcs})

# 链接kxServer、Common、DBDriver
target_link_libraries( UnionServer  KxServer)
target_link_libraries( UnionServer  Common)
target_link_libraries( UnionServer  DBDriver)
target_link_libraries( UnionServer  mysqlclient)