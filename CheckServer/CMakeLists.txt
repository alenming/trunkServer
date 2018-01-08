# 设置要参与编译的代码目录
set(CheckSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
)

include_directories(${CheckSrcDirs})

foreach(f ${CheckSrcDirs})
aux_source_directory(${f} CheckSrcs)
endforeach(f)

add_executable ( CheckServer ${CheckSrcs})

# 链接kxServer、Common、DBDriver
target_link_libraries( CheckServer  KxServer)
target_link_libraries( CheckServer  Common)
target_link_libraries( CheckServer  DBDriver)
target_link_libraries( CheckServer  mysqlclient)
target_link_libraries( CheckServer  Jason)