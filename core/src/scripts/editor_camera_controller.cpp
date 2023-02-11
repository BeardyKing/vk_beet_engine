#include <beet/scripts/editor_camera_controller.h>

#include <beet/component/camera.h>
#include <beet/component/transform.h>

#include <beet/input_manager.h>
#include <beet/scene.h>

namespace beet {

EditorCameraController::EditorCameraController() {
    register_system();
}

void EditorCameraController::register_system() {
    static bool s_editorCameraRegistered = false;

    if (s_editorCameraRegistered) {
        return;
    }

    auto& world = *Scene::get_world();
    world.system<Transform, Camera, EditorCameraController>().each(
        [](flecs::entity entity, Transform& transform, Camera& camera, EditorCameraController& controller) {
            Input& input = Input::get_input().value().get();
            auto dt = entity.delta_time();

            //===TOGGLE===//
            if (input.mouse_button_held_down(MouseButtonCode::Right)) {
                Input::set_cursor_state(MouseState::Disabled);

                //===MOVE===//
                vec3 moveDirection{0};
                if (input.key_held_down(KeyCode::W)) {
                    moveDirection += transform.forward();
                }
                if (input.key_held_down(KeyCode::S)) {
                    moveDirection += -transform.forward();
                }
                if (input.key_held_down(KeyCode::A)) {
                    moveDirection += transform.right();
                }
                if (input.key_held_down(KeyCode::D)) {
                    moveDirection += -transform.right();
                }
                if (input.key_held_down(KeyCode::R)) {
                    moveDirection += vec3_up;
                }
                if (input.key_held_down(KeyCode::F)) {
                    moveDirection += -vec3_up;
                }
                moveDirection *= controller.moveSpeed;
                if (input.key_held_down(KeyCode::LeftShift)) {
                    moveDirection *= controller.sprintMultiplier;
                }

                transform.set_position(transform.get_position() + moveDirection * dt);

                //===VIEW===//
                vec2 mouseDelta = input.get_relative_position_raw() * controller.mouseSpeed;
                controller.trackedRotationOffset.x += mouseDelta.y * dt;
                controller.trackedRotationOffset.y += -mouseDelta.x * dt;
                controller.trackedRotationOffset.z = 0;
                transform.set_rotation_euler(controller.trackedRotationOffset);

                camera.set_look_target(transform.get_position() + transform.forward());
            } else {
                Input::set_cursor_state(MouseState::Normal);
            }
        });

    s_editorCameraRegistered = true;
}

}  // namespace beet