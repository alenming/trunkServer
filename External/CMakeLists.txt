###  KxServer CMakeLists.txt
set(KxServerDirs 
    ${CMAKE_CURRENT_SOURCE_DIR}/External/kxServer
    ${CMAKE_CURRENT_SOURCE_DIR}/External/kxServer/commnication
    ${CMAKE_CURRENT_SOURCE_DIR}/External/kxServer/core
    ${CMAKE_CURRENT_SOURCE_DIR}/External/kxServer/helper
    ${CMAKE_CURRENT_SOURCE_DIR}/External/kxServer/pulgins
    ${CMAKE_CURRENT_SOURCE_DIR}/External/kxServer/server
)
include_directories(${KxServerDirs})
foreach(f ${KxServerDirs})
aux_source_directory(${f} kxServerSrcs)
endforeach(f)
add_library ( KxServer ${kxServerSrcs})


####### Jason 库
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/External/Jason)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/External/Jason JasonSrcs)
add_library ( Jason ${JasonSrcs})

####### HttpHelper
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/External)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/External/HttpHelper)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/External/HttpHelper HttpHelperSrcs)
add_library ( HttpHelper ${HttpHelperSrcs})