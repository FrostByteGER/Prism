#pragma once
#include "../Utilities/IService.hpp"
#include "../Utilities/TypeMap.hpp"
#include "../Utilities/Globals.hpp"
#include "IMeshFactory.hpp"

namespace Prism::Core
{
    class MeshFactoryRegistry : public Utility::IService
    {
    public:
        MeshFactoryRegistry() = default;
        ~MeshFactoryRegistry() override = default;

        void initialize() override
        {
        }

        void initializeDeferred() override
        {
        }

        void deInitialize() override
        {
        }

        template <typename T>
        void registerMeshFactory()
        {
            meshFactories.put<T>(std::make_unique<T>());
        }

        template <typename T>
        [[nodiscard]] RawPtr<T> getMeshFactory() const
        {
            return RawPtr<T>(static_cast<T*>(meshFactories.find<T>()->second.get()));
        }

        std::string getFullName() override
        {
            return "Prism::Core::MeshFactoryRegistry";
        }

    private:
        TypeMap<std::unique_ptr<IMeshFactoryBaseInternal>> meshFactories;
    };
}
