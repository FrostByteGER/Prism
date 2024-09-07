#include <fstream>
#include <iostream>
#include "ShaderAssetFactory.hpp"

std::unique_ptr<Prism::Assets::Asset> Prism::Assets::ShaderAssetFactory::loadAsset(const std::string& fileName)
{
    std::fstream file;
    file.open(fileName, std::ios::in | std::ios::ate | std::ios::binary);
    if (!file)
    {
        constexpr size_t bufferSize = 256;
        char errMsgBuffer[bufferSize];
#ifdef _WIN64
        const auto returnCode = strerror_s(errMsgBuffer, bufferSize, errno);
#else
        const auto returnCode = strerror_r(errno, errMsgBuffer, bufferSize);
#endif
        if (returnCode == 0)
        {
            LOG_ERROR("Failed to open file: {}, error: {}", fileName, errMsgBuffer);
        }
        else
        {
            LOG_ERROR("Failed to open file: {}, failed to retrieve specific errormessage", fileName);
        }
        return nullptr;
    }
    const auto fileSize = file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return std::make_unique<ShaderAsset>(fileName, buffer);
}
