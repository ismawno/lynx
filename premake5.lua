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
filter {}

staticruntime "off"
kind "ConsoleApp"
--buildoptions "-Xclang -fopenmp"

targetdir("bin/" .. outputdir)
objdir("build/" .. outputdir)

files {
   "src/**.cpp",
   "include/**.hpp"
}

