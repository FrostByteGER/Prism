#pragma once
#include <ranges>

#include "IService.hpp"
#include "TypeMap.hpp"
#include "Globals.hpp"

namespace Prism::Utility
{
    template <typename T>
    concept ImplementsIService = std::is_base_of_v<IService, T>;

    template <typename T>
    concept InterfaceImplementsIService = ImplementsIService<T> && std::is_abstract_v<T>;

    template <typename U, typename T>
    concept ClassImplementsInterfaceOfIService = ImplementsIService<T> && std::is_base_of_v<T, U> && !std::is_abstract_v
        <U>;

    class ServiceLocator
    {
    public:
        ServiceLocator(const ServiceLocator& other) = delete;
        ServiceLocator(ServiceLocator&& other) noexcept = delete;
        ServiceLocator& operator=(const ServiceLocator& other) = delete;
        ServiceLocator& operator=(ServiceLocator&& other) noexcept = delete;

        ~ServiceLocator()
        {
            deInitializeServicesInternal();
        }

        template <typename I, ClassImplementsInterfaceOfIService<I> T>
        static RawPtr<I> registerService()
        {
            ServiceLocator& instance = serviceLocatorInstance();
            const auto result = instance.services.find<T>();
            if (result != instance.services.end())
                return RawPtr<I>();
            auto service = std::make_unique<T>();
            auto returnServicePtr = static_cast<I*>(service.get());
            instance.servicesBootOrder.push_back(returnServicePtr);
            auto returnService = RawPtr<I>(returnServicePtr);
            instance.services.put<I>(std::move(service));
            LOG_DEBUG("Registered service: {}", returnService->getFullName());
            return returnService;
        }

        template <typename I, ClassImplementsInterfaceOfIService<I> T>
        static RawPtr<I> registerService(std::unique_ptr<T>&& service)
        {
            ServiceLocator& instance = serviceLocatorInstance();
            const auto result = instance.services.find<I>();
            if (result != instance.services.end())
                return RawPtr<I>();
            auto returnServicePtr = static_cast<I*>(service.get());
            instance.servicesBootOrder.push_back(returnServicePtr);
            auto returnService = RawPtr<I>(returnServicePtr);
            instance.services.put<I>(std::move(service));
            LOG_DEBUG("Registered service: {}", returnService->getFullName());
            return returnService;
        }

        template <ImplementsIService T>
        static RawPtr<T> getService()
        {
            const ServiceLocator& instance = serviceLocatorInstance();
            auto result = instance.services.find<T>();
            assert(result != instance.services.end());
            if (result == instance.services.end())
            {
                LOG_WARN("Service instance of type {} not found, nullptr is returned!", typeid(T).name());
                return nullptr;
            }

            return RawPtr<T>(static_cast<T*>(result->second.get()));
        }

        static void initializeServicesInternal()
        {
            const ServiceLocator& instance = serviceLocatorInstance();
            for (const auto service : instance.servicesBootOrder)
            {
                service->initialize();
                LOG_DEBUG("Initialized service: {}", service->getFullName());
            }
        }

        static void deferredInitializeServicesInternal()
        {
            const ServiceLocator& instance = serviceLocatorInstance();
            for (const auto service : instance.servicesBootOrder)
            {
                service->initializeDeferred();
                LOG_DEBUG("Deferred Initialized service: {}", service->getFullName());
            }
        }

        static void deInitializeServicesInternal()
        {
            ServiceLocator& instance = serviceLocatorInstance();
            for (auto it = instance.servicesBootOrder.rbegin(); it != instance.servicesBootOrder.rend(); ++it)
            {
                auto service = *it;
                service->deInitialize();
                LOG_DEBUG("Deinitialized service: {}", service->getFullName());
            }
            instance.servicesBootOrder.clear();
            instance.services.clear();
        }

        // instance generation and retrieval
        static ServiceLocator& serviceLocatorInstance()
        {
            static ServiceLocator instance;
            return instance;
        }

    private:
        TypeMap<std::unique_ptr<IService>> services;
        std::vector<IService*> servicesBootOrder;
        ServiceLocator() = default;
    };
}
