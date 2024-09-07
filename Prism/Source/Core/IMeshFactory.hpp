#pragma once
#include <memory>
#include "../Core/Mesh.hpp"

namespace Prism::Core
{
    template <typename T>
    concept SubtypeOfMesh = std::is_base_of_v<Mesh, T> && !std::is_abstract_v<T>;

    template <typename T>
    concept SubtypeOfMeshAsset = std::is_base_of_v<Assets::MeshAsset, T> && !std::is_abstract_v<T>;

    // This is the base class for all mesh factories that merely serves as a type erasure mechanism.
    class IMeshFactoryBaseInternal
    {
    public:
        virtual ~IMeshFactoryBaseInternal() = default;
    };

    template <SubtypeOfMesh TMeshType, SubtypeOfMeshAsset TMeshAssetType>
    class IMeshFactory : public IMeshFactoryBaseInternal
    {
    public:
        virtual ~IMeshFactory() = default;
        [[nodiscard]] virtual std::unique_ptr<TMeshType> createMesh(RawPtr<TMeshAssetType> meshAsset) = 0;
    };
}
