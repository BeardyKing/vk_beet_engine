
#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/scene.h>

#include <beet/component/camera.h>
#include <beet/component/material.h>
#include <beet/component/transform.h>

namespace beet {

Scene::Scene(Engine& engine) : m_engine(engine) {
    s_scene = std::ref(*this);
    m_world = std::make_shared<flecs::world>();
}

void Scene::on_awake() {
    build_scene_manual();
}

void Scene::build_scene_manual() {
    {
        auto e_camera = m_world->entity("Camera");
        e_camera.set(Transform(vec3(0.0f, 0.0f, -3.0f)));
        e_camera.set(Camera());
    }
    {
        auto e_vikingRoom = m_world->entity("viking room");
        e_vikingRoom.set(Transform(vec3(0.0f, 0.0f, 2.0f)));
        e_vikingRoom.set(ResourceManager::load_mesh("../res/misc/viking_room.obj"));

        auto material = e_vikingRoom.set(Material(gfx::PipelineType::Lit)).get_mut<Material>();
        material->set_albedo_texture(ResourceManager::load_texture("../res/textures/viking_room.png"));
        material->update_material_descriptor();
    }
}

void Scene::on_update(double deltaTime) {}
void Scene::on_late_update() {}
void Scene::on_destroy() {}

Scene::~Scene() {
    log::debug("Destroyed Scene");
}

std::optional<std::reference_wrapper<Scene>> Scene::get_scene() {
    BEET_ASSERT_MESSAGE(s_scene.has_value(), "AssetManager does not exist");
    return s_scene;
}

std::shared_ptr<flecs::world> Scene::get_world() {
    return Scene::get_scene().value().get().get_world_internal();
}

std::shared_ptr<flecs::world> Scene::get_world_internal() {
    return m_world;
}

}  // namespace beet
