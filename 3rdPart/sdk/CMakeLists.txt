cmake_minimum_required(VERSION 2.8.12)

project(infrared-sdk)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

include(${CMAKE_SOURCE_DIR}/3rdPart/opencv/OpenCVConfig.cmake)

set(src src/sdk.cpp src/kafface.cpp)
add_library(infrared-sdk SHARED ${src})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(infrared-sdk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(infrared-sdk ${OpenCV_LIBS})
set_target_properties(infrared-sdk PROPERTIES COMPILE_FLAGS -DKAG_DLL)
        add_custom_command(TARGET infrared-sdk
                        POST_BUILD
                            COMMAND "${CMAKE_SOURCE_DIR}/scripts/debug_post_build.bat"
                            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                            COMMENT "run env config"
                            VERBATIM
                            )                  
