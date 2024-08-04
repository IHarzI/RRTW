workspace "RTW"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release"
	}
	outputdir = "%{cfg.buildcfg}"


project "RTWCore"
	kind "WindowedApp" 
	language "C++"
	cppdialect "C++17"

	targetdir("bin/" .. outputdir .. "/%{prj.name}")
	objdir("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/source/**.h",
		"%{prj.name}/source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/source"
	}

	filter {"system:windows"}
		systemversion "latest"
		defines "RTW_SYSTEM_WINDOWS"

	filter {"configurations:Debug"}
		defines "RTW_BUILD_DEBUG"
		runtime "Debug"
		buildoptions "/MD"
		symbols "on"
		optimize "off"
	
	filter {"configurations:Release"}
		defines "RTW_BUILD_RELEASE"
		runtime "Release"
		buildoptions "/MD"
		optimize "on"