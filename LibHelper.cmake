macro(AddSharedLib libname srcdir incdir depends)
    file(GLOB ${libname}_src ${srcdir}/*.cpp ${srcdir}/*.c)
    file(GLOB ${libname}_head ${incdir}/*.hpp ${incdir}/*.h)
    include_directories(${incdir})
    add_library(${libname} SHARED ${${libname}_src} ${${libname}_head})
    foreach(dlib ${depends})
        target_link_libraries(${libname} debug ${CMAKE_BINARY_DIR}/Debug/${dlib}.lib optimized ${CMAKE_BINARY_DIR}/Release/${dlib}.lib)
    endforeach(dlib ${depends})
    set_target_properties(${libname} PROPERTIES COMPILE_FLAGS -DMAGIC_EXPORT_API_DLL)
    set_target_properties(${libname} PROPERTIES LINKER_LANGUAGE CXX)
    add_custom_command(TARGET ${libname} 
                                POST_BUILD
                                COMMAND "${CMAKE_SOURCE_DIR}/scripts/post-build.bat"
                                        ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${libname}.dll
                                        ${CMAKE_SOURCE_DIR}/Megvii_${PNAME}
                                VERBATIM)
endmacro(AddSharedLib)