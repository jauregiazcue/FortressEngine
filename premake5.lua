workspace "FortressEngineWorkspace"
   configurations { "Debug", "Release" }
   location "build"
   architecture "x64"

project "FortressEngineLib"
   kind "StaticLib"
   language "C++"
   cppdialect "C++17"
   location "build/libmaker"
   files { "include/*.h", 
           "src/*.cpp" , "src/*.cc",  
           "src/shaders/*.frag","src/shaders/*.vert",
           "deps/stbimage/stb_image.h",
           "deps/glad/src/gl.c","deps/imgui-master/backends/*.cpp","deps/imgui-master/*.cpp"}

   includedirs { "include",
                 "deps/glad/include",
                 "deps/glfw-3.3.8.bin.WIN64/include",
                 "deps/imgui-master",
                 "deps/imgui-master/backends",
                 "deps/glm-master",
                 "deps/miniaudio",
                 "deps/tinyobj",
                 "deps/stbimage",}
   libdirs {"deps/glfw-3.3.8.bin.WIN64/lib-vc2019"}
   links {"glfw3","opengl32"}

   filter { "configurations:Debug" }
      defines { "DEBUG" }
      symbols "On"
      targetdir "bin/lib"

   filter { "configurations:Release" }
      defines { "NDEBUG" }
      optimize "On"
      targetdir "bin/lib"

project "FortressEngine"
      kind "ConsoleApp"
      language "C++"
      cppdialect "C++17"
      location "build"
      files { "src/notlib/*.cpp", "src/notlib/*.cc",
      }
      includedirs { "include",
                 "deps/glad/include",
                 "deps/glfw-3.3.8.bin.WIN64/include",
                 "deps/imgui-master",
                 "deps/imgui-master/backends",
                 "deps/glm-master",
                 "deps/stbimage",}

      libdirs {"deps/glfw-3.3.8.bin.WIN64/lib-vc2019"}
      links {"glfw3","opengl32","FortressEngineLib"}
   
      filter { "configurations:Debug" }
         defines { "DEBUG" }
         symbols "On"
         targetdir "bin"
   
      filter { "configurations:Release" }
         defines { "NDEBUG" }
         optimize "On"
         targetdir "bin"