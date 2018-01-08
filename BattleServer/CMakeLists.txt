# 设置要参与编译的代码目录
set(BattleSrcDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/Classes
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/gameFrame
    ${CMAKE_CURRENT_SOURCE_DIR}/logic
)

include_directories(${BattleSrcDirs})

foreach(f ${BattleSrcDirs})
aux_source_directory(${f} BattleSrcs)
endforeach(f)

add_executable ( BattleServer ${BattleSrcs})

# 链接kxServer、Common、DBDriver
target_link_libraries( BattleServer  KxServer)
target_link_libraries( BattleServer  Common)
target_link_libraries( BattleServer  DBDriver)
target_link_libraries( BattleServer  mysqlclient)