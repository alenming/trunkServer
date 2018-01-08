# 设置要参与编译的代码目录
set(ServiceSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
)

include_directories(${ServiceSrcDirs})

foreach(f ${ServiceSrcDirs})
aux_source_directory(${f} ServiceSrcs)
endforeach(f)

add_executable ( ServiceServer ${ServiceSrcs})

# 链接kxServer、Common、DBDriver
target_link_libraries( ServiceServer  KxServer)
target_link_libraries( ServiceServer  Common)
target_link_libraries( ServiceServer  DBDriver)
target_link_libraries( ServiceServer  mysqlclient)