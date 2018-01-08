# 设置要参与编译的代码目录
set(ChatSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
)

include_directories(${ChatSrcDirs})

foreach(f ${ChatSrcDirs})
aux_source_directory(${f} ChatSrcs)
endforeach(f)

add_executable ( ChatServer ${ChatSrcs})

# 链接kxServer、Common、DBDriver
target_link_libraries( ChatServer  KxServer)
target_link_libraries( ChatServer  Common)
target_link_libraries( ChatServer  DBDriver)
target_link_libraries( ChatServer  mysqlclient)