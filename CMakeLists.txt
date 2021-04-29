if(UNIX)
    cmake_minimum_required(VERSION 3.16)
else()
    cmake_minimum_required(VERSION 3.18)
endif()

set(PNAME magicflow)
project(${PNAME})
set(CMAKE_CXX_STANDARD 14)
execute_process(COMMAND git rev-parse --short HEAD
                TIMEOUT 5
                OUTPUT_VARIABLE GIT_COMMIT_ID
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )               
add_definitions(-DHEAD_COMMIT_ID=${GIT_COMMIT_ID})
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_SOURCE_DIR}/anyflow/src)
# include_directories(${CMAKE_SOURCE_DIR}/src/common)

include(FetchLibHelper.cmake)
include(LibHelper.cmake)

# boost
if(UNIX)
    find_package(Boost REQUIRED COMPONENTS system thread)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else()
    set(BoostLib boost_1_74_0-x86-v140)
    AddFetchLib(${DEP_REPO} ${BoostLib})
    FetchContent_GetProperties(${BoostLib}) # 安装一下, 就是获取一下对应的变量
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost REQUIRED COMPONENTS system thread)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

if(UNIX)
else()
    AddFetchLib(${DEP_REPO} opencv-249.all.dynamic)
    AddFetchLib(${DEP_REPO} spdlog-1.5.0)
    AddFetchLib(${DEP_REPO} directshow-dev-lib)
    AddFetchLib(${DEP_REPO} ffmpeg-v4.3-vs2013-v120)
    CopyFFmpegRuntimeFiles(${CMAKE_SOURCE_DIR}/anyflow_${PNAME})

    CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_core249.dll)
    CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_highgui249.dll)
    CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_imgproc249.dll)

    CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_core249d.dll)
    CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_highgui249d.dll)
    CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_imgproc249d.dll)
endif()

# json config 
set(JSON_DIR ${CMAKE_SOURCE_DIR}/src/jsoncpp)
AddSharedLib(libjson ${JSON_DIR}/src ${JSON_DIR}/include "")
set_target_properties(libjson PROPERTIES COMPILE_FLAGS -DJSON_DLL_BUILD)
source_group("JSON" FILES ${json_head} ${json_src})

# base64 config 
set(B64_DIR ${CMAKE_SOURCE_DIR}/src/base64)
AddSharedLib(libb64 ${B64_DIR}/src ${B64_DIR}/include "")
source_group("base64" FILES ${base64_head} ${base64_src})

# ice config
# AddFetchLib(${DEP_REPO} "ice-3.6.5")

AddSharedLib(magicthreadpool ${CMAKE_SOURCE_DIR}/src/magicthreadpool/src ${CMAKE_SOURCE_DIR}/src/magicthreadpool/include "")
AddSharedLib(libnode_exec ${CMAKE_SOURCE_DIR}/src/node_exec/src ${CMAKE_SOURCE_DIR}/src/node_exec/include "libjson")
AddSharedLib(libnode ${CMAKE_SOURCE_DIR}/src/node/src ${CMAKE_SOURCE_DIR}/src/node/include "libnode_exec;libjson;magicthreadpool")
AddSharedLib(libmagicflow ${CMAKE_SOURCE_DIR}/src/libmagicflow/src ${CMAKE_SOURCE_DIR}/src/libmagicflow/include "libnode_exec;libjson;magicthreadpool;libnode")
AddSharedLib(flowFactory ${CMAKE_SOURCE_DIR}/src/flowFactory/src ${CMAKE_SOURCE_DIR}/src/flowFactory/include  "libmagicflow;libnode_exec;libjson;magicthreadpool;libnode")
AddSharedLib(mynode ${CMAKE_SOURCE_DIR}/src/node-template/src ${CMAKE_SOURCE_DIR}/src/node-template/include  "libnode_exec;libjson")
AddSharedLib(ffmpeginput ${CMAKE_SOURCE_DIR}/src/ffmpeg-input/src ${CMAKE_SOURCE_DIR}/src/ffmpeg-input/include  "libnode_exec;libjson")

option(BuildTest "build test" ON)
if (BuildTest)
    set(TestTool "TestTool")
    AddFetchLib(${DEP_REPO} googletest)
    FetchContent_GetProperties(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

    file(GLOB test_src test/*.cpp test/*.c)
    file(GLOB test_head test/*.h test/*.hpp)
    add_executable(${TestTool} ${test_src} ${test_head} ${src} ${json_src})
    target_include_directories(${TestTool} PRIVATE ${googletest_SOURCE_DIR}/include 
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(${TestTool} gtest_main)
    target_link_libraries(${TestTool} debug ${CMAKE_BINARY_DIR}/Debug/flowFactory.lib optimized ${CMAKE_BINARY_DIR}/Release/flowFactory.lib)
    target_link_libraries(${TestTool} debug ${CMAKE_BINARY_DIR}/Debug/libmagicflow.lib optimized ${CMAKE_BINARY_DIR}/Release/libmagicflow.lib)
    set(pro_dir ${CMAKE_SOURCE_DIR}/build)
    add_custom_command(TARGET ${TestTool} 
                             POST_BUILD
                             COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/post-build.bat"
                                      ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/TestTool.exe
                                      ${CMAKE_CURRENT_SOURCE_DIR}/anyflow_${PNAME}
                             VERBATIM )
    set_target_properties(${TestTool} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/anyflow_${PNAME}")
    set_target_properties(${TestTool} PROPERTIES VS_DEBUGGER_COMMAND "${CMAKE_SOURCE_DIR}/anyflow_${PNAME}/TestTool.exe")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TestTool})
endif()