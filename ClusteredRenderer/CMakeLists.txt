add_executable (ClusteredRenderer )

target_sources(ClusteredRenderer PRIVATE "ClusteredRenderer.cpp" "ClusteredRenderer.hpp")

add_subdirectory("src")
target_link_libraries(ClusteredRenderer PRIVATE ClusteredRendererLib)