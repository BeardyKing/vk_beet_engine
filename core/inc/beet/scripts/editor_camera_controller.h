#pragma once

#include <beet/types.h>

namespace beet {
class EditorCameraController {
   public:
    EditorCameraController();
    float moveSpeed = 150.0f;
    float sprintMultiplier = 3.0f;

    float mouseSpeed = 10.0f;
    vec3 trackedRotationOffset = vec3(0);

   private:
    void register_system();
};

}  // namespace beet
