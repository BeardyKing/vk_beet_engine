#pragma once

#include <beet/asset_loader.h>

#include <string>
#include <vector>

namespace beet {
class Renderer;
namespace gfx {
struct Mesh;
struct RawImage;
}  // namespace gfx
}  // namespace beet

namespace beet {

class AssetLoader {
   public:
    static std::vector<char> load_shader_binary(const std::string& path);
    static gfx::Mesh load_model(const std::string& modelPath);
    static gfx::RawImage load_image(const std::string& path);
};
}  // namespace beet
