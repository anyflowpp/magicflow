macro(AddSharedLib libname srcdir incdir)
    file(GLOB ${libname}_src ${srcdir}/*.cpp ${srcdir}/*.c)
    file(GLOB ${libname}_head ${incdir}/*.hpp ${incdir}/*.h)
    add_library(${libname} SHARED ${${libname}_src} ${${libname}_head} ${json_head} ${json_src} ${sdk_src} ${sdk_header} ${dshow_src} ${base64_src})
    set_target_properties(${libname} PROPERTIES COMPILE_FLAGS -DEBG_EXPORT_API_DLL)
    add_custom_command(TARGET ${libname} 
                                POST_BUILD
                                COMMAND "${CMAKE_SOURCE_DIR}/scripts/post-build.bat"
                                        ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${libname}.dll
                                # COMMAND "${CMAKE_SOURCE_DIR}/scripts/post-build.bat"
                                #         ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${libname}.lib
                                VERBATIM)
endmacro(AddSharedLib)