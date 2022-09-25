#include <beet/asset_loader.h>
#include <beet/log.h>
#include <fstream>

namespace beet {
std::vector<char> AssetLoader::read_file(const std::string& path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        log::warn("failed to find path: {}", path);
        return std::vector<char>{};
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}
}  // namespace beet
