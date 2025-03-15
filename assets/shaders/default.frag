#version 460 core

uniform vec3 u_CameraPosition;

in vec3 worldSpacePos_;
in vec3 normal_;
in vec2 texCoord_;

out vec4 FragColor;

const vec3 light_dir_1 = vec3(0, -1, 0);
const vec3 light_dir_2 = vec3(0, 0, -1);

const vec3 ambient = 0.01f * vec3(1.0f);

void main() {
    const vec3 normal = normalize(normal_);

    // diffuse
    const float cosine1 = max(0.0f, dot(normal, -light_dir_1));
    const float cosine2 = max(0.0f, dot(normal, -light_dir_2));
    const vec3 diffuse = vec3(0.5f)*cosine1 + vec3(0.5f)*cosine2;

    const vec3 res = ambient + diffuse;

    //FragColor = vec4(normal, 1.0f);
    FragColor = vec4(res, 1.0f);
}
