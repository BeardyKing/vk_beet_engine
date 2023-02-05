#pragma once

#include <beet/types.h>

namespace beet {

class Transform {
   public:
    explicit Transform(const vec3& position = vec3(0.0f),
                       const vec3& scale = vec3(1.0f),
                       const vec3& rotation = vec4(0));

    vec3 get_position() const;
    vec3 get_scale() const;
    quat get_rotation() const;
    vec3 get_rotation_euler() const;

    void set_position(const vec3& position);
    void set_scale(const vec3& scale);
    void set_rotation(const quat& rotation);
    void set_rotation_euler(const vec3& rotation);

    void rotate_euler(const vec3& rotation);

    mat4 get_model_matrix() const;

    vec3 forward() const;
    vec3 up() const;
    vec3 right() const;

   private:
    vec3 m_position;
    vec3 m_scale;
    vec3 m_rotation;
};

}  // namespace beet