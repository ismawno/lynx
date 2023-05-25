project "lynx"
language "C++"
cppdialect "C++17"

filter "system:macosx"
   buildoptions {
      "-Wall",
      "-Wextra",
      "-Wpedantic",
      "-Wconversion",
      "-Wno-unused-parameter"
   }

   libdirs "%{wks.location}/vendor/vulkan-sdk/macOS/lib"
   links {
      "glfw",
      "Cocoa.framework",
      "IOKit.framework",
      "CoreFoundation.framework",
      "vulkan"
   }
   linkoptions "-Wl,-rpath,%{wks.location}/vendor/vulkan-sdk/macOS/lib"
filter {}

staticruntime "off"
kind "ConsoleApp"

targetdir("bin/" .. outputdir)
objdir("build/" .. outputdir)

files {
   "src/**.cpp",
   "include/**.hpp"
}

includedirs {
   "%{wks.location}/vendor/glfw/include",
   "%{wks.location}/vendor/glm",
   "%{wks.location}/vendor/vulkan-sdk/macOS/include"
}