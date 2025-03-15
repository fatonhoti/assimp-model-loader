#include "camera.hpp"

#include <GLFW/glfw3.h>
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

static bool is_first_mouse{ false };
static bool is_rmb_down{ false };
static double last_x{ 0.0f };
static double last_y{ 0.0f };
static glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(origin, origin + forward, up);
}

void Camera::update_projection_matrix() {
    projection_matrix = glm::perspective(glm::radians(vertical_fov), aspect_ratio, near_plane, far_plane);
}

void Camera::move(int dir, float delta_time)
{
    float speed = movement_speed * delta_time;
    auto posv4 = glm::vec4(origin, 1.0f);
    if (dir == FORWARD)
        origin = glm::translate(forward * speed) * posv4;
    if (dir == BACKWARD)
        origin = glm::translate(-forward * speed) * posv4;
    if (dir == RIGHT)
        origin = glm::translate(right * speed) * posv4;
    if (dir == LEFT)
        origin = glm::translate(-right * speed) * posv4;
    if (dir == UP)
        origin = glm::translate(WORLD_UP * speed) * posv4;
    if (dir == DOWN)
        origin = glm::translate(-WORLD_UP * speed) * posv4;
}

void Camera::mouse_callback(double xpos, double ypos)
{
    if (is_first_mouse) {
        last_x = xpos;
        last_y = ypos;
        is_first_mouse = false;
    }

    if (is_rmb_down) {
        float delta_x = static_cast<float>(xpos - last_x);
        float delta_y = static_cast<float>(last_y - ypos);
        last_x = xpos;
        last_y = ypos;

        static float sensitivity = 0.05f;
        delta_x *= sensitivity;
        delta_y *= sensitivity;

        glm::mat4 yaw = glm::rotate(rotation_speed * -delta_x, WORLD_UP);
        // TODO: Don't allow pitching more than 90 deg (up or down)
        glm::mat4 pitch = glm::rotate(rotation_speed * delta_y, glm::normalize(glm::cross(forward, WORLD_UP)));

        forward = glm::vec3(pitch * yaw * glm::vec4(forward, 0.0f));

        // Recalculate UP and RIGHT vectors
        right = glm::normalize(glm::cross(forward, WORLD_UP));
        up = glm::normalize(glm::cross(right, forward));
    }
}

void Camera::mouse_button_callback(double cursor_x, double cursor_y, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        is_rmb_down = true;
        last_x = cursor_x;
        last_y = cursor_y;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        is_rmb_down = false;
}