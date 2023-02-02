#pragma once

#include <beet/types.h>

namespace beet {
class Camera {
   public:
    Camera() = default;
    ~Camera() = default;

    vec3 get_look_target() { return m_lookTarget; };
    float get_fov() { return radians(m_fov); };
    float get_z_near() { return m_zNear; };
    float get_z_far() { return m_zFar; };

    mat4 get_projection(float aspectRatio);

    vec3 world_up();

    void set_look_target(vec3 target) { m_lookTarget = target; };
    void set_fov(float fov) { m_fov = fov; };
    void set_z_near(float zNear) { m_zNear = zNear; };
    void set_z_far(float zFar) { m_zFar = zFar; };

   private:
    glm::vec3 m_lookTarget = vec3(0, 0, -1);
    float m_fov = 70.0f;
    float m_zNear = 0.1f;
    float m_zFar = 2000.0f;
};

}  // namespace beet