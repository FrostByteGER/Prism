#include "StaticMeshAssetFactory.hpp"

#include "StaticMeshAsset.hpp"
#include "../Utilities/Globals.hpp"

// Define LAST!
#define TINYOBJLOADER_IMPLEMENTATION
#include "../Utilities/tiny_obj_loader.h"

std::unique_ptr<Prism::Assets::Asset> Prism::Assets::StaticMeshAssetFactory::loadAsset(const std::string& fileName)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings;
    std::string errors;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, fileName.c_str()))
    {
        if (!warnings.empty())
        {
            LOG_WARN("Warnings while loading file: {}, warnings: {}", fileName, warnings);
        }
        if (!errors.empty())
        {
            LOG_ERROR("Errors while loading file: {}, errors: {}", fileName, errors);
        }
        return nullptr;
    }
    std::vector<Rendering::Vertex> vertices;
    std::vector<uint32_t> indices;
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Rendering::Vertex vertex{};

            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (!attrib.normals.empty())
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }
            else
            {
                vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
                LOG_INFO("No normals in file: {}", fileName);
            }


            if (!attrib.texcoords.empty())
            {
                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }
            else
            {
                vertex.texCoord = glm::vec2(0.0f, 0.0f);
                LOG_INFO("No texcoords in file: {}", fileName);
            }


            vertex.color = {1.0f, 1.0f, 1.0f};

            vertices.emplace_back(vertex);
            indices.emplace_back(static_cast<uint32_t>(indices.size()));
        }
    }

    return std::make_unique<StaticMeshAsset>(fileName, vertices, indices);
}
