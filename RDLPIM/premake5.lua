workspace "RDLPIM"
	architecture "x86"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

LibDirs = {}
LibDirs["GSE"]  = "vendor/GSE"

-------------------------------------------------------
--------- gwcRDLToolbox---------------------------------
-------------------------------------------------------	

project "gwcRDLToolbox"

	location"gwcRDLToolbox"
	kind "StaticLib"
	
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputDir .. "/%{prj.name}")
	objdir("bin-int/" .. outputDir .. "/%{prj.name}")

	pchheader "rdlpch.h"
	pchsource "gwcRDLToolbox/src/rdlpch.cpp"
	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"

	}

	includedirs
	{
		"%{prj.name}/src"
	}
	
	libdirs
	{
		"%{LibDirs.GSE}"
	}
	
	links
	{
		"s3dll.lib",
		"mstG.lib"
	}

	filter "system:windows"
		systemversion "latest"

	defines
	{
	}


	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines{"NDEBUG"}
		runtime "Release"
		symbols "on"
		optimize "on"

	filter "configurations:Dist"
		defines{"NDEBUG"}
		runtime "Release"
		optimize "on"

	
-------------------------------------------------------
--------- RDLPIM ---------------------------------
-------------------------------------------------------		
	project "RDLPluginManager"

	location"RDLPluginManager"
	kind "ConsoleApp"
	
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputDir .. "/%{prj.name}")
	objdir("bin-int/" .. outputDir .. "/%{prj.name}")

	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"

	}

	includedirs
	{
		"gwcRDLToolbox/src"
	}
	
	libdirs
	{

	}
	
	links
	{
		"gwcRDLToolbox"
	}

	filter "system:windows"
		systemversion "latest"

	defines
	{
	}
	
	postbuildcommands
	{
		"{COPY} ../vendor/GSE/mstG.dll ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}",
		"{COPY} ../vendor/GSE/s3dll.dll ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		symbols "on"
		optimize "on"

		
	filter "configurations:Dist"
		runtime "Release"
		optimize "on"


	
-------------------------------------------------------
--------- Test Harness ---------------------------------
-------------------------------------------------------	
	project "RDLPluginManager_testHarness"

	location"RDLPluginManager_testHarness"
	kind "ConsoleApp"
	
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputDir .. "/%{prj.name}")
	objdir("bin-int/" .. outputDir .. "/%{prj.name}")

	
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"

	}

	includedirs
	{
		"gwcRDLToolbox/src",
		"%{prj.name}/src"
	}
	
	libdirs
	{

	}
	
	links
	{
		"gwcRDLToolbox"
	}

	filter "system:windows"
		systemversion "latest"

	defines
	{
	}
	
	postbuildcommands
	{
		"{COPY} ../vendor/GSE/mstG.dll ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}",
		"{COPY} ../vendor/GSE/s3dll.dll ../bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}",
	}
	
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		

	filter "configurations:Release"
		runtime "Release"
		symbols "on"
		optimize "on"
		

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
		
