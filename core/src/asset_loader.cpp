#include <beet/assert.h>
#include <beet/asset_loader.h>
#include <beet/gfx/vulkan_mesh.h>
#include <beet/log.h>
#include <beet/types.h>

#include <fstream>

#include <unordered_map>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

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

gfx::Mesh AssetLoader::load_model(const std::string& modelPath) {
    std::string fileName = modelPath;

    gfx::Mesh outMesh;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                                   aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    if (!scene) {
        log::error("Failed to load mesh : {}", fileName);
        BEET_ASSERT(BEET_FALSE);
    }

    // std::unordered_map<gfx::Vertex, uint32_t> uniqueVertices{};

    for (int x = 0; x < scene->mNumMeshes; ++x) {
        const aiMesh* model = scene->mMeshes[x];
        for (size_t i = 0; i < model->mNumVertices; i++) {
            aiVector3D position;
            aiVector3D normal;
            aiVector3D textureCoordinates{0, 0, 0};  // note assimp mTextureCoords are stored as vec3
            aiVector3D tangent{0, 0, 0};

            normal = model->mNormals[i];
            position = model->mVertices[i];

            if (model->HasTextureCoords(0)) {
                textureCoordinates = model->mTextureCoords[0][i];
            }

            if (model->HasTangentsAndBitangents()) {
                tangent = model->mTangents[i];
            }

            gfx::Vertex vertex{};
            vertex.position = {position.x, position.y, position.z};
            vertex.normal = {normal.x, normal.y, normal.z};
            vertex.color = {1.0f, 1.0f, 1.0f};

            outMesh.vertices.push_back(vertex);
        }
    }
    return outMesh;
}

}  // namespace beet
