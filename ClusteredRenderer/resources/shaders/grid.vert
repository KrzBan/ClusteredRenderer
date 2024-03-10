#version 400 core

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec2 uv;

const vec3 pos[4] = vec3[4](
  vec3(-1.0, 0.0, -1.0),
  vec3( 1.0, 0.0, -1.0),
  vec3( 1.0, 0.0,  1.0),
  vec3(-1.0, 0.0,  1.0)
);
const int indices[6] = int[6](0, 1, 2, 2, 3, 0);

const float gridSize = 100.0;

void main() {
  vec3 vpos = pos[indices[gl_VertexID]]* gridSize;
  gl_Position = projection * view * vec4(vpos, 1.0);
  uv = vpos.xz;
}