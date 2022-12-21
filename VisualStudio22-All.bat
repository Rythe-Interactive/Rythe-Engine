cmake -E make_directory build
cmake . -G "Visual Studio 17 2022" -S . -B ./build/ -T ClangCL -D RYTHE_BUILD_APPLICATIONS=ON -D RYTHE_BUILD_OPTION_ASAN=ON -D RYTHE_FORCE_ENABLE_ALL_MODULES=ON
pause