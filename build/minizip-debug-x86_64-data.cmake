########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(minizip_COMPONENT_NAMES "")
list(APPEND minizip_FIND_DEPENDENCY_NAMES BZip2 ZLIB)
list(REMOVE_DUPLICATES minizip_FIND_DEPENDENCY_NAMES)
set(BZip2_FIND_MODE "NO_MODULE")
set(ZLIB_FIND_MODE "NO_MODULE")

########### VARIABLES #######################################################################
#############################################################################################
set(minizip_PACKAGE_FOLDER_DEBUG "C:/Users/Krzysztof/.conan2/p/miniz9a355f7ac94b1/p")
set(minizip_BUILD_MODULES_PATHS_DEBUG )


set(minizip_INCLUDE_DIRS_DEBUG )
set(minizip_RES_DIRS_DEBUG )
set(minizip_DEFINITIONS_DEBUG "-DHAVE_BZIP2")
set(minizip_SHARED_LINK_FLAGS_DEBUG )
set(minizip_EXE_LINK_FLAGS_DEBUG )
set(minizip_OBJECTS_DEBUG )
set(minizip_COMPILE_DEFINITIONS_DEBUG "HAVE_BZIP2")
set(minizip_COMPILE_OPTIONS_C_DEBUG )
set(minizip_COMPILE_OPTIONS_CXX_DEBUG )
set(minizip_LIB_DIRS_DEBUG "${minizip_PACKAGE_FOLDER_DEBUG}/lib")
set(minizip_BIN_DIRS_DEBUG )
set(minizip_LIBRARY_TYPE_DEBUG STATIC)
set(minizip_IS_HOST_WINDOWS_DEBUG 1)
set(minizip_LIBS_DEBUG minizip)
set(minizip_SYSTEM_LIBS_DEBUG )
set(minizip_FRAMEWORK_DIRS_DEBUG )
set(minizip_FRAMEWORKS_DEBUG )
set(minizip_BUILD_DIRS_DEBUG )
set(minizip_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(minizip_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${minizip_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${minizip_COMPILE_OPTIONS_C_DEBUG}>")
set(minizip_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${minizip_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${minizip_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${minizip_EXE_LINK_FLAGS_DEBUG}>")


set(minizip_COMPONENTS_DEBUG )