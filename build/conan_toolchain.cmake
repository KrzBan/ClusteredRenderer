

# Conan automatically generated toolchain file
# DO NOT EDIT MANUALLY, it will be overwritten

# Avoid including toolchain file several times (bad if appending to variables like
#   CMAKE_CXX_FLAGS. See https://github.com/android/ndk/issues/323
include_guard()

message(STATUS "Using Conan toolchain: ${CMAKE_CURRENT_LIST_FILE}")

if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeToolchain' generator only works with CMake >= 3.15")
endif()












# Definition of VS runtime, defined from build_type, compiler.runtime, compiler.runtime_type
cmake_policy(GET CMP0091 POLICY_CMP0091)
if(NOT "${POLICY_CMP0091}" STREQUAL NEW)
    message(FATAL_ERROR "The CMake policy CMP0091 must be NEW, but is '${POLICY_CMP0091}'")
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Release>:MultiThreadedDLL>$<$<CONFIG:Debug>:MultiThreadedDebugDLL>")

message(STATUS "Conan toolchain: C++ Standard 20 with extensions OFF")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Extra c, cxx, linkflags and defines


if(DEFINED CONAN_CXX_FLAGS)
  string(APPEND CMAKE_CXX_FLAGS_INIT " ${CONAN_CXX_FLAGS}")
endif()
if(DEFINED CONAN_C_FLAGS)
  string(APPEND CMAKE_C_FLAGS_INIT " ${CONAN_C_FLAGS}")
endif()
if(DEFINED CONAN_SHARED_LINKER_FLAGS)
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${CONAN_SHARED_LINKER_FLAGS}")
endif()
if(DEFINED CONAN_EXE_LINKER_FLAGS)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${CONAN_EXE_LINKER_FLAGS}")
endif()

get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if(_CMAKE_IN_TRY_COMPILE)
    message(STATUS "Running toolchain IN_TRY_COMPILE")
    return()
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

# Definition of CMAKE_MODULE_PATH
# the generators folder (where conan generates files, like this toolchain)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Definition of CMAKE_PREFIX_PATH, CMAKE_XXXXX_PATH
# The Conan local "generators" folder, where this toolchain is saved.
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} )
list(PREPEND CMAKE_LIBRARY_PATH "C:/Users/Krzysztof/.conan2/p/spdloc85815db9798e/p/lib" "C:/Users/Krzysztof/.conan2/p/fmt26f902364af87/p/lib" "C:/Users/Krzysztof/.conan2/p/glad746f393738090/p/lib" "C:/Users/Krzysztof/.conan2/p/glfw754621de0fdd2/p/lib" "C:/Users/Krzysztof/.conan2/p/imguic2beffdd4e3e9/p/lib" "C:/Users/Krzysztof/.conan2/p/assimc328af72455d4/p/lib" "C:/Users/Krzysztof/.conan2/p/miniz9a355f7ac94b1/p/lib" "C:/Users/Krzysztof/.conan2/p/bzip25e3167db905ff/p/lib" "lib" "C:/Users/Krzysztof/.conan2/p/pugixac09b49913775/p/lib" "C:/Users/Krzysztof/.conan2/p/kuba-25a98d53b5a73/p/lib" "C:/Users/Krzysztof/.conan2/p/poly2fc55060a23099/p/lib" "C:/Users/Krzysztof/.conan2/p/rapid4cabb31a09329/p/lib" "C:/Users/Krzysztof/.conan2/p/zlib68724b092caa7/p/lib" "C:/Users/Krzysztof/.conan2/p/draco0c5b46e22d95e/p/lib" "C:/Users/Krzysztof/.conan2/p/clippa07d3219b0876/p/lib" "C:/Users/Krzysztof/.conan2/p/opend0564534e1b3fe/p/lib")
list(PREPEND CMAKE_INCLUDE_PATH "C:/Users/Krzysztof/.conan2/p/spdloc85815db9798e/p/include" "C:/Users/Krzysztof/.conan2/p/fmt26f902364af87/p/include" "C:/Users/Krzysztof/.conan2/p/glmb8583f8d3b716/p/include" "C:/Users/Krzysztof/.conan2/p/glad746f393738090/p/include" "C:/Users/Krzysztof/.conan2/p/glfw754621de0fdd2/p/include" "C:/Users/Krzysztof/.conan2/p/imguic2beffdd4e3e9/p/include" "C:/Users/Krzysztof/.conan2/p/assimc328af72455d4/p/include" "C:/Users/Krzysztof/.conan2/p/miniz9a355f7ac94b1/p/include" "C:/Users/Krzysztof/.conan2/p/miniz9a355f7ac94b1/p/include/minizip" "C:/Users/Krzysztof/.conan2/p/bzip25e3167db905ff/p/include" "include" "C:/Users/Krzysztof/.conan2/p/pugixac09b49913775/p/include" "C:/Users/Krzysztof/.conan2/p/kuba-25a98d53b5a73/p/include" "C:/Users/Krzysztof/.conan2/p/poly2fc55060a23099/p/include" "C:/Users/Krzysztof/.conan2/p/rapid4cabb31a09329/p/include" "C:/Users/Krzysztof/.conan2/p/zlib68724b092caa7/p/include" "C:/Users/Krzysztof/.conan2/p/draco0c5b46e22d95e/p/include" "C:/Users/Krzysztof/.conan2/p/clippa07d3219b0876/p/include" "C:/Users/Krzysztof/.conan2/p/stbb3237e7146d86/p/include" "C:/Users/Krzysztof/.conan2/p/opend0564534e1b3fe/p/include" "C:/Users/Krzysztof/.conan2/p/entt4626e134c934c/p/include")



if (DEFINED ENV{PKG_CONFIG_PATH})
set(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_LIST_DIR};$ENV{PKG_CONFIG_PATH}")
else()
set(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_LIST_DIR};")
endif()




# Variables
# Variables  per configuration


# Preprocessor definitions
# Preprocessor definitions per configuration
