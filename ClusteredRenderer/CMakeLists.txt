add_executable (ClusteredRenderer )

set_property(TARGET ClusteredRenderer PROPERTY CXX_STANDARD 20)

target_sources(ClusteredRenderer PRIVATE "ClusteredRenderer.cpp" "ClusteredRenderer.hpp")

add_subdirectory("src")
target_link_libraries(ClusteredRenderer PRIVATE ClusteredRendererLib)