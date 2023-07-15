project "lynx"
language "C++"
cppdialect "C++17"

staticruntime "off"
kind "StaticLib"

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

   filter "kind:ConsoleApp"
      libdirs "%{wks.location}/vendor/vulkan-sdk/macOS/lib"
      links {
         "cpp-kit",
         "glfw",
         "Cocoa.framework",
         "IOKit.framework",
         "CoreFoundation.framework",
         "vulkan",
         "imgui"
      }
      rpath = "-Wl,-rpath,".. rootpath .."vendor/vulkan-sdk/macOS/lib"
      linkoptions {rpath}
   filter {}
filter {}

targetdir("bin/" .. outputdir)
objdir("build/" .. outputdir)

pchheader "lynx/internal/pch.hpp"
pchsource "src/internal/pch.cpp"

files {
   "src/**.cpp",
   "include/**.hpp"
}

filter "kind:not ConsoleApp"
   removefiles "src/main.cpp"
filter {}

includedirs {
   "include",
   "%{wks.location}/cpp-kit/include",
   "%{wks.location}/vendor/spdlog/include",
   "%{wks.location}/vendor/glfw/include",
   "%{wks.location}/vendor/glm",
   "%{wks.location}/vendor/vulkan-sdk/macOS/include",
   "%{wks.location}/vendor/imgui",
   "%{wks.location}/vendor/imgui/backends"
}