# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(entt_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(entt_FRAMEWORKS_FOUND_DEBUG "${entt_FRAMEWORKS_DEBUG}" "${entt_FRAMEWORK_DIRS_DEBUG}")

set(entt_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET entt_DEPS_TARGET)
    add_library(entt_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET entt_DEPS_TARGET
             PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${entt_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${entt_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:>
             APPEND)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### entt_DEPS_TARGET to all of them
conan_package_library_targets("${entt_LIBS_DEBUG}"    # libraries
                              "${entt_LIB_DIRS_DEBUG}" # package_libdir
                              "${entt_BIN_DIRS_DEBUG}" # package_bindir
                              "${entt_LIBRARY_TYPE_DEBUG}"
                              "${entt_IS_HOST_WINDOWS_DEBUG}"
                              entt_DEPS_TARGET
                              entt_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "entt"    # package_name
                              "${entt_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${entt_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Debug ########################################
    set_property(TARGET EnTT::EnTT
                 PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Debug>:${entt_OBJECTS_DEBUG}>
                 $<$<CONFIG:Debug>:${entt_LIBRARIES_TARGETS}>
                 APPEND)

    if("${entt_LIBS_DEBUG}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET EnTT::EnTT
                     PROPERTY INTERFACE_LINK_LIBRARIES
                     entt_DEPS_TARGET
                     APPEND)
    endif()

    set_property(TARGET EnTT::EnTT
                 PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Debug>:${entt_LINKER_FLAGS_DEBUG}> APPEND)
    set_property(TARGET EnTT::EnTT
                 PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Debug>:${entt_INCLUDE_DIRS_DEBUG}> APPEND)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET EnTT::EnTT
                 PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Debug>:${entt_LIB_DIRS_DEBUG}> APPEND)
    set_property(TARGET EnTT::EnTT
                 PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Debug>:${entt_COMPILE_DEFINITIONS_DEBUG}> APPEND)
    set_property(TARGET EnTT::EnTT
                 PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Debug>:${entt_COMPILE_OPTIONS_DEBUG}> APPEND)

########## For the modules (FindXXX)
set(entt_LIBRARIES_DEBUG EnTT::EnTT)
