#include "GraphicsDebugBridgeImplNvidiaNsightGraphics.hpp"
#include "../Utilities/Logging/Log.hpp"

#include <codecvt>
#include <locale>

#ifdef INCLUDE_NGFX_HEADERS
bool Prism::Rendering::GraphicsDebugBridgeImplNvidiaNsightGraphics::initialize(
    const GraphicsDebugBridgeActivityType& activityType)
{
    uint32_t installationCount = 0;
    NGFX_Injection_InstallationInfo installations[255];
    const auto enumerateInstallationResult = NGFX_Injection_EnumerateInstallations(&installationCount, installations);
    if (enumerateInstallationResult != NGFX_INJECTION_RESULT_OK)
    {
        LOG_ERROR("Failed to enumerate Nvidia NSight Graphics installations! {}",
                  convertNGFX_Injection_ResultToString(enumerateInstallationResult));
        return false;
    }

    LOG_DEBUG("Found {} Nvidia NSight Graphics installations", installationCount);
    for (size_t i = 0; i < installationCount; ++i)
    {
        uint32_t activityCount = 0;
        NGFX_Injection_Activity activities[255];
        const auto enumerateActivitiesResult = NGFX_Injection_EnumerateActivities(
            &installations[i], &activityCount, activities);
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        if (enumerateActivitiesResult != NGFX_INJECTION_RESULT_OK)
        {
            LOG_ERROR("Failed to enumerate activities for NSight Graphics installation '{}'",
                      converter.to_bytes(std::wstring(installations[i].installationPath)));
            return false;
        }

        std::string activitiesString;
        for (size_t j = 0; j < activityCount; ++j)
        {
            activitiesString += "'" + convertNGFX_Injection_ActivityTypeToString(activities[j].type) + "'";
            if (j < activityCount - 1)
            {
                activitiesString += ", ";
            }
        }

        LOG_DEBUG("Found {} activities for installation '{}': {}", activityCount,
                  converter.to_bytes(std::wstring(installations[i].installationPath)), activitiesString);

        for (size_t j = 0; j < activityCount; ++j)
        {
            if (activities[j].type == NGFX_INJECTION_ACTIVITY_FRAME_DEBUGGER)
            {
                const auto injectResult = NGFX_Injection_InjectToProcess(&installations[i], &activities[j]);
                if (injectResult != NGFX_INJECTION_RESULT_OK)
                {
                    LOG_ERROR("Failed to inject Nvidia NSight Graphics! {}",
                              convertNGFX_Injection_ResultToString(injectResult));
                    return false;
                }
                LOG_DEBUG("Successfully injected Nvidia NSight Graphics!");
                return true;
            }
        }
    }
    return false;
}

void Prism::Rendering::GraphicsDebugBridgeImplNvidiaNsightGraphics::deInitialize()
{
}

bool Prism::Rendering::GraphicsDebugBridgeImplNvidiaNsightGraphics::startSingleFrameCapture()
{
    const auto result = NGFX_Injection_ExecuteActivityCommand();
    if (result == NGFX_INJECTION_RESULT_OK)
    {
        LOG_DEBUG("Started single frame capture!");
        return true;
    }

    LOG_ERROR("Failed to start single frame capture! {}", convertNGFX_Injection_ResultToString(result));
    return false;
}

void Prism::Rendering::GraphicsDebugBridgeImplNvidiaNsightGraphics::startFrameCapture()
{
    //TODO: Unsupported!
}

void Prism::Rendering::GraphicsDebugBridgeImplNvidiaNsightGraphics::endFrameCapture()
{
    //TODO: Unsupported!
}

std::string Prism::Rendering::GraphicsDebugBridgeImplNvidiaNsightGraphics::convertNGFX_Injection_ResultToString(
    const NGFX_Injection_Result& result) const
{
    switch (result)
    {
    case NGFX_INJECTION_RESULT_OK:
        return "NGFX_INJECTION_RESULT_OK";
    case NGFX_INJECTION_RESULT_FAILURE:
        return "NGFX_INJECTION_RESULT_FAILURE";
    case NGFX_INJECTION_RESULT_INVALID_ARGUMENT:
        return "NGFX_INJECTION_RESULT_INVALID_ARGUMENT";
    case NGFX_INJECTION_RESULT_INJECTION_FAILED:
        return "NGFX_INJECTION_RESULT_INJECTION_FAILED";
    case NGFX_INJECTION_RESULT_ALREADY_INJECTED:
        return "NGFX_INJECTION_RESULT_ALREADY_INJECTED";
    case NGFX_INJECTION_RESULT_NOT_INJECTED:
        return "NGFX_INJECTION_RESULT_NOT_INJECTED";
    case NGFX_INJECTION_RESULT_DRIVER_STILL_LOADED:
        return "NGFX_INJECTION_RESULT_DRIVER_STILL_LOADED";
    case NGFX_INJECTION_RESULT_INVALID_PROJECT:
        return "NGFX_INJECTION_RESULT_INVALID_PROJECT";
    case NGFX_INJECTION_RESULT_INJECTION_REJECTED:
        return "NGFX_INJECTION_RESULT_INJECTION_REJECTED";
    default:
        return "Unknown NGFX_Injection_Result enum";
    }
}

std::string Prism::Rendering::GraphicsDebugBridgeImplNvidiaNsightGraphics::convertNGFX_Injection_ActivityTypeToString(
    const NGFX_Injection_ActivityType& activityType) const
{
    switch (activityType)
    {
    case NGFX_INJECTION_ACTIVITY_UNKNOWN:
        return "NGFX_INJECTION_ACTIVITY_UNKNOWN";
    case NGFX_INJECTION_ACTIVITY_FRAME_DEBUGGER:
        return "NGFX_INJECTION_ACTIVITY_FRAME_DEBUGGER";
    case NGFX_INJECTION_ACTIVITY_GENERATE_CPP_CAPTURE:
        return "NGFX_INJECTION_ACTIVITY_GENERATE_CPP_CAPTURE";
    case NGFX_INJECTION_ACTIVITY_GPU_TRACE:
        return "NGFX_INJECTION_ACTIVITY_GPU_TRACE";
    case NGFX_INJECTION_ACTIVITY_PYLON_CAPTURE:
        return "NGFX_INJECTION_ACTIVITY_PYLON_CAPTURE";
    case NGFX_INJECTION_ACTIVITY_SHADER_DEBUGGER:
        return "NGFX_INJECTION_ACTIVITY_SHADER_DEBUGGER";
    default:
        return "Unknown NGFX_Injection_ActivityType enum";
    }
}
#endif