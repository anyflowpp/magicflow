
set(DEP_REPO http://10.122.104.62)
include(FetchContent)

macro(AddFetchLib addr name)
    FetchContent_Declare(
        ${name}
        URL   ${addr}/${name}.7z
    )
    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)    # if 没有安装了第三方库 
        FetchContent_Populate(${name}) # 安装一下, 就是获取一下对应的变量
    endif()
    include(${${name}_SOURCE_DIR}/Config.cmake)
endmacro(AddFetchLib)

function(CopyLibFileToSource name src) 
    FetchContent_GetProperties(${name})
    file(COPY ${${name}_SOURCE_DIR}/${src} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Megvii_${PNAME})
endfunction()