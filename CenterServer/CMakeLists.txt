# 设置要参与编译的代码目录
set(CenterSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
)

include_directories(${CenterSrcDirs})

foreach(f ${CenterSrcDirs})
aux_source_directory(${f} CenterSrcs)
endforeach(f)

add_executable ( CenterServer ${CenterSrcs})

# 链接kxServer、Common、DBDriver
target_link_libraries( CenterServer  KxServer)
target_link_libraries( CenterServer  Common)
target_link_libraries( CenterServer  DBDriver)
target_link_libraries( CenterServer  mysqlclient)