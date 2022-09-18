#pragma once

#include <string>
#include <vector>

namespace beet {

class AssetLoader {
   public:
    AssetLoader(const std::string& vertPath, const std::string& fragPath);

   private:
    static std::vector<char> read_file(const std::string& path);
    void create_shader(const std::string& vertPath, const std::string& fragPath);
};
}  // namespace beet
