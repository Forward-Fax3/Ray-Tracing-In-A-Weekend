newoption
{
	trigger = "cleanType",
	allowed =
	{
		"vs2022",
		"gmake"
	},
	description = "select what type of clean you want to do."
}

workspace "RTIAW"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist",
	}

	platforms
	{
		"AVX512",
--		"AVX2",
--		"SSE2"
	}

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glm"]    = "RTIAW/3rd Party/Git/glm/"

if _ACTION == "clean" then
	os.rmdir("bin");
	os.remove("RTIAW/image.ppm")
	filter { "options:cleanType=vs2022" }
		os.remove("RTIAW.sln")
		os.remove("RTIAW/RTIAW.vcxproj")
		os.remove("RTIAW/RTIAW.vcxproj.filters")
		os.remove("RTIAW/RTIAW.vcxproj.user")
	filter{}
end

output = "%{cfg.system}/%{cfg.buildcfg}/%{cfg.platform}"

--include "Wulled/Engine/vendor/proj/git"

-- disabled for now
-- pchheader "WLDPCH.h"
-- pchsource "%{prj.name}/Engine/PCH/WLDPCH.cpp"

project "RTIAW"
	location "%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++17"
	cdialect "c17"
	staticruntime "on"
	toolset "clang"
	linker "LLD"
	fatalwarnings "ALL"

	targetdir ("Bin/" .. output .. "/%{prj.name}")
	objdir ("Bin/intermediate/" .. output .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.cpp",

		"%{prj.name}/src/**.h",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/src/**",
		
		"%{IncludeDir.glm}",
	}

	links
	{
	}

	defines
	{
		"GLM_FORCE_SWIZZLE",
		"GLM_ENABLE_EXPERIMENTAL",
		"GLM_FORCE_DEFAULT_ALIGNED_GENTYPES",
	}

	flags
	{
		"MultiProcessorCompile",
	}

	buildoptions
	{
		"-Wno-c++98-compat",
		"-Wno-c++98-compat-pedantic",
		"-Wno-extra-semi-stmt"
	}

	filter "system:windows"
		systemversion "latest"
	
	filter { "configurations:Debug" }
		runtime "Debug"
		defines 
		{
			"_DEBUG",
			"ENGINE_DEBUG",
			"DX12_ENABLE_DEBUG_LAYER",
			"EN_ENABLE_ASSERTS"
		}
		optimize "Off"
		symbols "On"
	
	filter { "configurations:Release" }
		runtime "Release"
		defines 
		{
			"_RELEASE",
			"ENGINE_RELEASE",
			"EN_ENABLE_ASSERTS"
		}
		optimize "Speed"
		symbols "On"

	filter { "configurations:Dist" }
		runtime "Release"
		defines
		{
			"_DIST",
			"ENGINE_DIST"
		}
		linktimeoptimization "on"
		optimize "Speed"
		symbols "Off"
	
	filter { "platforms:AVX512" }
--		vectorextensions "AVX512" currently not supported need to use buildoptions instead
		buildoptions { "/arch:AVX512" }
		defines { "GLM_FORCE_AVX2" } -- GLM doesnt seem to support AVX512 anymore so we use AVX2 instead though the compiler should still compile it with AVX512
	filter { "platforms:AVX2" }
		vectorextensions "AVX2"
		defines { "GLM_FORCE_AVX2" }
	filter { "platforms:SSE2" }
		vectorextensions "SSE2"
		defines { "GLM_FORCE_SSE2" }
