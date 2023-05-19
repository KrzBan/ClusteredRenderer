########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(glm_COMPONENT_NAMES "")
set(glm_FIND_DEPENDENCY_NAMES "")

########### VARIABLES #######################################################################
#############################################################################################
set(glm_PACKAGE_FOLDER_DEBUG "C:/Users/Krzysztof/.conan2/p/glmb8583f8d3b716/p")
set(glm_BUILD_MODULES_PATHS_DEBUG )


set(glm_INCLUDE_DIRS_DEBUG "${glm_PACKAGE_FOLDER_DEBUG}/include")
set(glm_RES_DIRS_DEBUG )
set(glm_DEFINITIONS_DEBUG )
set(glm_SHARED_LINK_FLAGS_DEBUG )
set(glm_EXE_LINK_FLAGS_DEBUG )
set(glm_OBJECTS_DEBUG )
set(glm_COMPILE_DEFINITIONS_DEBUG )
set(glm_COMPILE_OPTIONS_C_DEBUG )
set(glm_COMPILE_OPTIONS_CXX_DEBUG )
set(glm_LIB_DIRS_DEBUG )
set(glm_BIN_DIRS_DEBUG )
set(glm_LIBRARY_TYPE_DEBUG UNKNOWN)
set(glm_IS_HOST_WINDOWS_DEBUG 1)
set(glm_LIBS_DEBUG )
set(glm_SYSTEM_LIBS_DEBUG )
set(glm_FRAMEWORK_DIRS_DEBUG )
set(glm_FRAMEWORKS_DEBUG )
set(glm_BUILD_DIRS_DEBUG )
set(glm_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(glm_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${glm_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${glm_COMPILE_OPTIONS_C_DEBUG}>")
set(glm_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${glm_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${glm_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${glm_EXE_LINK_FLAGS_DEBUG}>")


set(glm_COMPONENTS_DEBUG )