cmake_minimum_required(VERSION 3.18)
set(PNAME threadflow)
project(${PNAME})
set(CMAKE_CXX_STANDARD 14)
execute_process(COMMAND git rev-parse --short HEAD
                TIMEOUT 5
                OUTPUT_VARIABLE GIT_COMMIT_ID
                OUTPUT_STRIP_TRAILING_WHITESPACE
                )               
add_definitions(-DHEAD_COMMIT_ID=${GIT_COMMIT_ID})
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
include_directories(${CMAKE_SOURCE_DIR}/src)

include(FetchLibHelper.cmake)

# src file config
file(GLOB src src/*.cpp src/*.c src/core/*.cpp)
file(GLOB head src/*.h src/*.hpp src/core/*.h)

# boost
# set(BoostLib boost_1_74_0-x86-v141)
set(BoostLib boost_1_74_0-x86-v140)
AddFetchLib(${DEP_REPO} ${BoostLib})
# include(D:/megvii/opt/boost_1_74_0/boost_1_74_0-vc12-x86/Config.cmake)
FetchContent_GetProperties(${BoostLib}) # 安装一下, 就是获取一下对应的变量
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

AddFetchLib(${DEP_REPO} opencv-249.all.dynamic)
AddFetchLib(${DEP_REPO} spdlog-1.5.0)
AddFetchLib(${DEP_REPO} directshow-dev-lib)
AddFetchLib(${DEP_REPO} mgliveness.nolimit)

set(mgflib ebg_v2.13.0.7)
AddFetchLib(${DEP_REPO} ${mgflib})
CopyLibFileToSource(${mgflib} ${Megface_relative_dll_path})

CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_core249.dll)
CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_highgui249.dll)
CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_imgproc249.dll)
CopyLibFileToSource(mgliveness.nolimit lib/mgliveness.dll)

CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_core249d.dll)
CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_highgui249d.dll)
CopyLibFileToSource(opencv-249.all.dynamic x86/vc12/bin/opencv_imgproc249d.dll)


# json config 
set(JSON_DIR ${CMAKE_SOURCE_DIR}/3rdPart/jsoncpp)
file(GLOB json_src ${JSON_DIR}/*cpp)
file(GLOB json_head ${JSON_DIR}/json/*.h)
source_group("JSON" FILES ${json_head} ${json_src})
include_directories(${JSON_DIR})

# base64 config 
set(B64_DIR ${CMAKE_SOURCE_DIR}/3rdPart/base64)
file(GLOB base64_src ${B64_DIR}/src/*cpp)
file(GLOB base64_head ${B64_DIR}/include/*.h)
source_group("base64" FILES ${base64_head} ${base64_src})
include_directories(${B64_DIR}/include)

# megface sdk cofnig
set(SDK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPart/sdk)

# ice config
# AddFetchLib(${DEP_REPO} "ice-3.6.5")

include(LibHelper.cmake)
AddSharedLib(testboostdll ${CMAKE_SOURCE_DIR}/src/testboostdll ${CMAKE_SOURCE_DIR}/src/testboostdll)
AddSharedLib(detect ${CMAKE_SOURCE_DIR}/src/detect_node ${CMAKE_SOURCE_DIR}/src/detect_node)

AddSharedLib(megthreadpool ${CMAKE_SOURCE_DIR}/src/megthreadpool ${CMAKE_SOURCE_DIR}/src/megthreadpool))

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
    set(pro_dir ${CMAKE_SOURCE_DIR}/build)
    add_custom_command(TARGET ${TestTool} 
                             POST_BUILD
                             COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/post-build.bat"
                                      ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/TestTool.exe
                             VERBATIM )
    set_target_properties(${TestTool} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Megvii_${PNAME}")
    set_target_properties(${TestTool} PROPERTIES VS_DEBUGGER_COMMAND "${CMAKE_SOURCE_DIR}/Megvii_${PNAME}/TestTool.exe")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TestTool})
endif()