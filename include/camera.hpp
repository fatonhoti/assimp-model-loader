#pragma once

#include "glm/glm.hpp"

constexpr int FORWARD{ 0 };
constexpr int BACKWARD{ 1 };
constexpr int RIGHT{ 2 };
constexpr int LEFT{ 3 };
constexpr int UP{ 4 };
constexpr int DOWN{ 5 };

struct Camera
{
    // orthonomal basis
    glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 right{ 1.0f, 0.0f, 0.0f };
    glm::vec3 origin{ 0.0f, 0.0f, 0.0f };

    // projection matrix properties
    glm::mat4 projection_matrix{ 1.0f };
    float vertical_fov{ 45.0f };
    float aspect_ratio{ 1.0f };
    float near_plane{ 0.01f };
    float far_plane{ 300.0f };

    float movement_speed{ 5.0f };
    float rotation_speed{ 0.05f };

    [[nodiscard]] glm::mat4 get_view_matrix() const;
    void update_projection_matrix();
    void move(int dir, float delta_time);
    void mouse_callback(double xpos, double ypos);
    void mouse_button_callback(double cursor_x, double cursor_y, int button, int action, int mods);
};