#pragma once

#include <string>
#include <vector>

namespace beet {

class AssetLoader {
   public:
    static std::vector<char> read_file(const std::string& path);
};
}  // namespace beet
