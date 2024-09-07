-- Premake 5

-- Dependency Inclusion
include("conandeps.premake5.lua")

workspace "Prism"
	conan_setup()
	startproject "Sandbox"
	architecture "x64"
	configurations {"Debug", "Release"}

-- General Variables
outputDir = "%{cfg.buildcfg}-%{cfg.system}-x64"

project "Prism"
	location "Prism"
	kind "StaticLib"
	staticruntime "off"
	language "C++"
	cppdialect "C++20"
	cdialect "C17"
	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-intermediates/" .. outputDir .. "/%{prj.name}")
	flags { "MultiProcessorCompile" }
	linkoptions { conan_exelinkflags, "-IGNORE:4006" }
	-- VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 and IMGUI_IMPL_VULKAN_NO_PROTOTYPES allow for dynamic loading of Vulkan functions instead of using vulkan-loader
	defines { "GLM_FORCE_DEPTH_ZERO_TO_ONE", "GLM_FORCE_LEFT_HANDED", "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1", "IMGUI_IMPL_VULKAN_NO_PROTOTYPES" }
	files
	{
		"%{prj.name}/Source/**.h", 
		"%{prj.name}/Source/**.hpp", 
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Source/**.ixx",
		"%{prj.name}/Source/**.cxx"
	}
	
	filter "system:windows"
		systemversion "latest"
		-- _CRT_SECURE_NO_WARNINGS is because a few dependencies use the old windows *cpy functions that are somewhat insecure
		defines { "_CRT_SECURE_NO_WARNINGS" }
		postbuildcommands
		{
			[[
python "%{wks.location}Build-Scripts\PostBuildExecutor.py" COPYFOLDER "../Distribution/%{cfg.buildcfg}/" "%{cfg.targetdir}/" --ignoredFiles=".gitkeep"
	if %errorlevel% neq 0 exit %errorlevel% else exit 0
			]]
		}

	filter "system:linux"
		postbuildcommands
		{
			[[
python3 "%{wks.location}/Build-Scripts/PostBuildExecutor.py" COPYFOLDER "../Distribution/%{cfg.buildcfg}/" "%{cfg.targetdir}/" --ignoredFiles=".gitkeep"; \
	if [ $$? -ne 0 ]; then \
		exit $$?; \
	fi; \
			]]
		}

	filter "configurations:Debug"
		defines { "Prism_DEBUG" }
		runtime "Debug"
		symbols "on"
		filter "toolset:msc"
			--We don't want this annoying "Member not initialized" spam.
			disablewarnings{"26495"}

		
	filter "configurations:Release"
		defines { "Prism_RELEASE" }
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	staticruntime "off"
	language "C++"
	cppdialect "C++20"
	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-intermediates/" .. outputDir .. "/%{prj.name}")
	flags { "MultiProcessorCompile" }
	linkoptions { conan_exelinkflags }
	files
	{
		"%{prj.name}/Source/**.h", 
		"%{prj.name}/Source/**.hpp", 
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/Source/**.ixx",
		"%{prj.name}/Source/**.cxx"
	}
	
	links
	{
		"Prism"
	}

	includedirs
	{
		"%{wks.location}/Prism/Source"
	}
	
	filter "system:windows"
		-- _CRT_SECURE_NO_WARNINGS is because a few dependencies use the old windows *cpy functions that are somewhat insecure
		defines { "_CRT_SECURE_NO_WARNINGS" }
		systemversion "latest"
		postbuildcommands
		{
			[[ 
python "%{wks.location}Build-Scripts\PostBuildExecutor.py" COPYFOLDER "../Distribution/%{cfg.buildcfg}/" "%{cfg.targetdir}/" --ignoredFiles=".gitkeep"
python "%{wks.location}Build-Scripts\PostBuildExecutor.py" COPYFOLDER "./Assets/" "%{cfg.targetdir}/Assets/" --ignoredFiles=".gitkeep" --deleteExistingTarget
python "%{wks.location}Build-Scripts\PostBuildExecutor.py" SHADERCOMPILE "%{cfg.targetdir}/Assets/" "../Bootstrap-Tools/glslc.exe" --deleteShaderSources
	if %errorlevel% neq 0 exit %errorlevel% else exit 0
			]]
		}
				
	filter "system:linux"
		postbuildcommands
		{
			[[
	python3 "%{wks.location}/Build-Scripts/PostBuildExecutor.py" COPYFOLDER "../Distribution/%{cfg.buildcfg}/" "%{cfg.targetdir}/" --ignoredFiles=".gitkeep"; \
	python3 "%{wks.location}/Build-Scripts/PostBuildExecutor.py" COPYFOLDER "./Assets/" "%{cfg.targetdir}/Assets/" --ignoredFiles=".gitkeep" --deleteExistingTarget; \
	python3 "%{wks.location}/Build-Scripts/PostBuildExecutor.py" SHADERCOMPILE "%{cfg.targetdir}/Assets/" "../Bootstrap-Tools/glslc" --deleteShaderSources; \
	if [ $$? -ne 0 ]; then \
		exit $$?; \
	fi; \
			]]
		}
	
	filter "configurations:Debug"
		-- SKIP_INPUT_PROMPT disables the waiting for a user input (usually enter) to close the cmd window
		defines { "Prism_DEBUG", "Sandbox_DEBUG", "SKIP_INPUT_PROMPT" }
		symbols "on"
		runtime "Debug"
		filter "toolset:msc"
			--We don't want this annoying "Member not initialized" spam.
			disablewarnings{"26495"}
		debugdir ("bin/" .. outputDir .. "/%{prj.name}")
		
	filter "configurations:Release"
		-- SKIP_INPUT_PROMPT disables the waiting for a user input (usually enter) to close the cmd window
		defines { "Prism_RELEASE", "Sandbox_RELEASE", "SKIP_INPUT_PROMPT" }
		optimize "on"
		runtime "Release"
		debugdir ("bin/" .. outputDir .. "/%{prj.name}")

