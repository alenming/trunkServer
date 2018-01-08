# 设置要参与编译的代码目录
set(MatchSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
)

include_directories(${MatchSrcDirs})

foreach(f ${MatchSrcDirs})
aux_source_directory(${f} MatchSrcs)
endforeach(f)

add_executable ( MatchServer ${MatchSrcs})

# 链接kxServer、Common、DBDriver
target_link_libraries( MatchServer  KxServer)
target_link_libraries( MatchServer  Common)
target_link_libraries( MatchServer  DBDriver)
target_link_libraries( MatchServer  mysqlclient)