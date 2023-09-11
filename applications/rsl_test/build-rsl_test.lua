createProject("applications","rsl_test","ConsoleApp")
targetdir "$(SolutionDir)bin\\build\\"
includedirs {
    "rsl_test/",
    "rsl_test/third_party/*",
    "rsl_test/third_party/*/include",
    "rsl_test/third_party/*/src",
    "rsl_test/third_party/*/third_party",
    "$(SolutionDir)rythe/engine/core/third_party/rythe-standard-library/src/",
    "$(SolutionDir)rythe/engine/core/third_party/rythe-standard-library/third_party/"
}   
files { 
    "**.h",
    "**.hpp",
    "**.inl",
    "**.c",
    "**.cpp"
     }

