
set(DEP_REPO http://www.lib_repo.com)  # ͨ��hosts�ļ� ����������Ļ�ȡ��ַ
include(FetchContent)

macro(AddFetchLib addr name)
    FetchContent_Declare(
        ${name}
        URL   ${addr}/${name}.7z
    )
    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)    # if û�а�װ�˵������� 
        FetchContent_Populate(${name}) # ��װһ��, ���ǻ�ȡһ�¶�Ӧ�ı���
    endif()
    include(${${name}_SOURCE_DIR}/Config.cmake)
endmacro(AddFetchLib)

function(CopyLibFileToSource name src) 
    FetchContent_GetProperties(${name})
    file(COPY ${${name}_SOURCE_DIR}/${src} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/anyflow_${PNAME})
endfunction()