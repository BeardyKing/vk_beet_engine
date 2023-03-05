
#include <beet/assert.h>
#include <beet/engine.h>
#include <beet/log.h>
#include <beet/scene.h>

#include <beet/component/camera.h>
#include <beet/component/material.h>
#include <beet/component/transform.h>

#include <beet/scripts/editor_camera_controller.h>

namespace beet {

Scene::Scene(Engine& engine) : m_engine(engine) {
    s_scene = std::ref(*this);
    m_world = std::make_shared<flecs::world>();
}

void Scene::on_awake() {
    build_scene_manual();
}

void Scene::build_scene_manual() {
    //=== CAMERA ===//
    {
        auto e_camera = m_world->entity("Camera");
        auto transform = e_camera.set(Transform(vec3(0.0f, 0.0f, -3.0f), vec3(1), vec3(0, 0, 0))).get_mut<Transform>();
        e_camera.set(Camera()).get_mut<Camera>()->set_look_target(transform->get_position() + transform->forward());
        e_camera.set(EditorCameraController());
    }
    //=== SKYBOX ===//
    {
        auto e_sky = m_world->entity("sky box");
        e_sky.set(Transform(vec3(0), vec3(10), vec3(0,0,0)));
        e_sky.set(ResourceManager::load_mesh("../res/misc/octahedron.fbx"));
        e_sky.set(gfx::SkyPipeline());

        auto material = e_sky.add<Material>().get_mut<Material>();
        material->set_pipeline_type(gfx::PipelineType::Sky);
    }
    //=== WORLD OBJECTS ===//
    {
        auto e_gun = m_world->entity("gun");
        e_gun.set(Transform(vec3(-50.0f, -30.0f, 100.0f), vec3(1), vec3(0, 90, 90)));
        e_gun.set(ResourceManager::load_mesh("../res/misc/Cerberus_LP.fbx"));
        e_gun.set(gfx::LitPipeline());

        auto material = e_gun.add<Material>().get_mut<Material>();
        material->set_pipeline_type(gfx::PipelineType::Lit);
        material->set_albedo_texture(ResourceManager::load_texture("../res/textures/Cerberus/Cerberus_A.jpg"));
        material->set_normal_texture(ResourceManager::load_texture("../res/textures/Cerberus/Cerberus_N.jpg"));
        material->set_metallic_texture(ResourceManager::load_texture("../res/textures/Cerberus/Cerberus_M.jpg"));
        material->set_roughness_texture(ResourceManager::load_texture("../res/textures/Cerberus/Cerberus_R.jpg"));
        material->set_occlusion_texture(ResourceManager::load_texture("../res/textures/UV_Grid_test.png"));
        material->update_material_descriptor();
    }
}

void Scene::on_update(double deltaTime) {
    m_world->progress((float)deltaTime);
}
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
