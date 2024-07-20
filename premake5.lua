project "lynx"
language "C++"
cppdialect "c++20"

staticruntime "off"
kind "StaticLib"

function script_path()
   local str = debug.getinfo(2, "S").source:sub(2)
   return str:match("(.*/)")
end

shaderpath = script_path() .. "shaders/"
defines {'LYNX_SHADER_PATH="' .. shaderpath .. '"'}

filter "system:macosx or linux"
   buildoptions {
      "-Wall",
      "-Wextra",
      "-Wpedantic",
      "-Wconversion",
      "-Wno-unused-parameter",
      "-Wno-sign-conversion",
      "-Wno-gnu-anonymous-struct",
      "-Wno-nested-anon-types"
   }
filter {}

targetdir("bin/" .. outputdir)
objdir("build/" .. outputdir)

pchheader "lynx/internal/pch.hpp"
pchsource "src/internal/pch.cpp"

files {
   "src/**.cpp",
   "include/**.hpp"
}

includedirs {
   "include",
   "%{wks.location}/cpp-kit/include",
   "%{wks.location}/vendor/spdlog/include",
   "%{wks.location}/vendor/glfw/include",
   "%{wks.location}/vendor/glm",
   "%{wks.location}/vendor/imgui",
   "%{wks.location}/vendor/imgui/backends",
   "%{wks.location}/vendor/implot",
   "%{wks.location}/vendor/yaml-cpp/include"
}
VULKAN_SDK = os.getenv("VULKAN_SDK")
filter "system:windows"
   includedirs "%{VULKAN_SDK}/Include"