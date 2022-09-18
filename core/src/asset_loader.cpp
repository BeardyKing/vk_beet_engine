#include <beet/asset_loader.h>
#include <beet/log.h>
#include <fstream>

namespace beet {

AssetLoader::AssetLoader(const std::string& vertPath, const std::string& fragPath) {
    create_shader(vertPath, fragPath);
}

std::vector<char> AssetLoader::read_file(const std::string& path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        log::warn("failed to find path: {}", path);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

void AssetLoader::create_shader(const std::string& vertPath, const std::string& fragPath) {
    std::vector<char> vert = read_file(vertPath);
    std::vector<char> frag = read_file(fragPath);

    log::info("vert size: {}", vert.size());
    log::info("frag size: {}", frag.size());
}
}  // namespace beet
