add_executable (ClusteredRenderer )

target_sources(ClusteredRenderer PRIVATE "ClusteredRenderer.cpp" "ClusteredRenderer.hpp")


if(MSVC)
  target_compile_options(ClusteredRenderer PRIVATE 
    /W4 
    $<$<CONFIG:RELEASE>:/O2>
    $<$<CONFIG:DEBUG>:/Od>)
else()
  target_compile_options(ClusteredRenderer PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


set_property(TARGET ClusteredRenderer PROPERTY CXX_STANDARD 20)

add_subdirectory("src")
target_link_libraries(ClusteredRenderer PRIVATE ClusteredRendererLib)