#include <beet/component/camera.h>

namespace beet {

mat4 Camera::get_projection(float aspectRatio) {
    auto projection = glm::perspective(glm::radians(m_fov), aspectRatio, m_zNear, m_zFar);
    projection[1][1] *= -1;
    return projection;
}

vec3 Camera::world_up() {
    return vec3(0, 1, 0);
}

}  // namespace beet