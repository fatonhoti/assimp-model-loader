#version 460 core

uniform vec3 u_CameraPosition;

in vec3 worldSpacePos_;
in vec3 normal_;
in vec2 texCoord_;

out vec4 FragColor;

const vec3 light_dir = vec3(0, -1, 0);

void main() {
    FragColor = vec4(normal_*2.0f - 1.0f, 1.0f);
}
