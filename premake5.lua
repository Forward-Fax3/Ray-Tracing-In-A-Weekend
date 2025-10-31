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
	architecture "x86_64"

	configurations
	{
		"Debug",
		"Release",
		"Dist",
	}

	platforms
	{
		"AVX512",
		"AVX2",
		"SSE2"
	}

	floatingpoint "strict"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["glm"]  = "RTIAW/3rd Party/Git/glm/"
IncludeDir["CTPL"] = "RTIAW/3rd Party/Git/CTPL/"
IncludeDir["stb"]  = "RTIAW/3rd Party/Git/stb/"

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
	cppdialect "C++latest"
	cdialect "c17"
	staticruntime "on"
	toolset "msc"
--	toolset "clang"
	linker "LLD"
	warnings "Extra"
	fatalwarnings "ALL"
	intrinsics "On"

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
		"%{IncludeDir.CTPL}",
		"%{IncludeDir.stb}",
	}

	links
	{
	}

	defines
	{
		"GLM_ENABLE_EXPERIMENTAL",
		"GLM_FORCE_DEFAULT_ALIGNED_GENTYPES",
		"GLM_FORCE_INLINE",
		"GLM_FORCE_SIZE_T_LENGTH"
	}

	flags
	{
		"MultiProcessorCompile",
	}

	buildoptions
	{
--		"-Wno-c++98-compat",
--		"-Wno-c++98-compat-pedantic",
--		"-Wno-extra-semi-stmt"
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
		inlining "Explicit"
		linktimeoptimization "Off"
	
	filter { "configurations:Release" }
		runtime "Release"
		defines 
		{
			"_RELEASE",
			"NDEBUG",
			"ENGINE_RELEASE",
			"EN_ENABLE_ASSERTS"
		}
		optimize "Speed"
		symbols "On"
		inlining "auto"
		linktimeoptimization "On"

	filter { "configurations:Dist" }
		runtime "Release"
		defines
		{
			"_DIST",
			"NDEBUG",
			"ENGINE_DIST"
		}
		linktimeoptimization "on"
		optimize "Speed"
		symbols "Off"
		inlining "auto"
		linktimeoptimization "On"
	
	filter { "platforms:AVX512" }
		vectorextensions "AVX512"
		defines
		{
			"GLM_FORCE_AVX2", -- GLM doesnt seem to support AVX512 anymore so we use AVX2 instead though the compiler should still compile it with AVX512
			"GLM_FORCE_FMA",
			"RTW_AVX512"
		} 
	filter { "platforms:AVX2" }
		vectorextensions "AVX2"
		defines
		{
			"GLM_FORCE_AVX2",
			"GLM_FORCE_FMA",
			"RTW_AVX2"
		} 
	filter { "platforms:SSE2" }
		vectorextensions "SSE2"
		defines
		{
			"GLM_FORCE_SSE2",
			"RTW_SSE2"
		}
