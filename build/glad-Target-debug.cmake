# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(glad_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(glad_FRAMEWORKS_FOUND_DEBUG "${glad_FRAMEWORKS_DEBUG}" "${glad_FRAMEWORK_DIRS_DEBUG}")

set(glad_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET glad_DEPS_TARGET)
    add_library(glad_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET glad_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${glad_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${glad_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### glad_DEPS_TARGET to all of them
conan_package_library_targets("${glad_LIBS_DEBUG}"    # libraries
                              "${glad_LIB_DIRS_DEBUG}" # package_libdir
                              "${glad_BIN_DIRS_DEBUG}" # package_bindir
                              "${glad_LIBRARY_TYPE_DEBUG}"
                              "${glad_IS_HOST_WINDOWS_DEBUG}"
                              glad_DEPS_TARGET
                              glad_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "glad"    # package_name
                              "${glad_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${glad_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Debug ########################################
    set_property(TARGET glad::glad
                 PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Debug>:${glad_OBJECTS_DEBUG}>
                 $<$<CONFIG:Debug>:${glad_LIBRARIES_TARGETS}>
                 APPEND)

    if("${glad_LIBS_DEBUG}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET glad::glad
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     glad_DEPS_TARGET
                     APPEND)
    endif()

    set_property(TARGET glad::glad
                 PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Debug>:${glad_LINKER_FLAGS_DEBUG}> APPEND)
    set_property(TARGET glad::glad
                 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Debug>:${glad_INCLUDE_DIRS_DEBUG}> APPEND)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET glad::glad
                 PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Debug>:${glad_LIB_DIRS_DEBUG}> APPEND)
    set_property(TARGET glad::glad
                 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Debug>:${glad_COMPILE_DEFINITIONS_DEBUG}> APPEND)
    set_property(TARGET glad::glad
                 PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Debug>:${glad_COMPILE_OPTIONS_DEBUG}> APPEND)

########## For the modules (FindXXX)
set(glad_LIBRARIES_DEBUG glad::glad)
