project "lynx"
language "C++"
cppdialect "C++17"

function script_path()
   local str = debug.getinfo(2, "S").source:sub(2)
   return str:match("(.*/)")
end

shaderpath = script_path() .. "shaders/"
defines {'LYNX_SHADER_PATH="' .. shaderpath .. '"'}

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
   rpath = "-Wl,-rpath,".. rootpath .."vendor/vulkan-sdk/macOS/lib"
   linkoptions {rpath}
filter {}

staticruntime "off"
kind "ConsoleApp"

targetdir("bin/" .. outputdir)
objdir("build/" .. outputdir)

pchheader "lynx/pch.hpp"
pchsource "src/pch.cpp"

files {
   "src/**.cpp",
   "include/**.hpp"
}

includedirs {
   "include",
   "%{wks.location}/debug-log-tools/include",
   "%{wks.location}/allocators/include",
   "%{wks.location}/vendor/spdlog/include",
   "%{wks.location}/vendor/glfw/include",
   "%{wks.location}/vendor/glm",
   "%{wks.location}/vendor/vulkan-sdk/macOS/include"
}