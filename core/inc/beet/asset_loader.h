#pragma once

#include <beet/asset_loader.h>

#include <string>
#include <vector>

namespace beet {
namespace gfx {
struct Mesh;
}  // namespace gfx
}  // namespace beet

namespace beet {

class AssetLoader {
   public:
    static std::vector<char> read_file(const std::string& path);
    static gfx::Mesh load_model(const std::string& modelPath);
};
}  // namespace beet
