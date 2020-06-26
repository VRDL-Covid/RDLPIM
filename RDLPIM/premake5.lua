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

project "RDLPluginManager"

	location"RDLPluginManager"
	kind "ConsoleApp"
	
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputDir .. "/%{prj.name}")
	objdir("bin-int/" .. outputDir .. "/%{prj.name}")

	--pchheader "rdlpch.h"
	--pchsource "gwcEngine/src/rdlpch.cpp"
	
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
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"

	--postbuildcommands { "{COPY} %{wks.location}vendor/GSE/mstG.lib %{cfg.targetdir}",  "{COPY} %{wks.location}vendor/GSE/s3dll.lib %{cfg.targetdir}"}
	postbuildcommands { "{COPY} C:/Users/gwc59/OneDrive/Desktop/RDLPIM2/RDLPIM/vendor/GSE/mstG.dll C:/Users/gwc59/OneDrive/Desktop/RDLPIM2/RDLPIM/1.dll*"}
	
	
	project "RDLPluginManager_testHarness"

	location"RDLPluginManager_testHarness"
	kind "ConsoleApp"
	
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputDir .. "/%{prj.name}")
	objdir("bin-int/" .. outputDir .. "/%{prj.name}")

	--pchheader "rdlpch.h"
	--pchsource "gwcEngine/src/rdlpch.cpp"
	
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
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"

	--postbuildcommands { "{COPY} %{wks.location}vendor/GSE/mstG.lib %{cfg.targetdir}",  "{COPY} %{wks.location}vendor/GSE/s3dll.lib %{cfg.targetdir}"}
	postbuildcommands { "{COPY} C:/Users/gwc59/OneDrive/Desktop/RDLPIM2/RDLPIM/vendor/GSE/mstG.dll C:/Users/gwc59/OneDrive/Desktop/RDLPIM2/RDLPIM/1.dll*"}