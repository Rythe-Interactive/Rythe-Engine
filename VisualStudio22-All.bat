cmake -E make_directory build
cmake . -G "Visual Studio 17 2022" -S . -B ./build/ -T ClangCL -DRYTHE_BUILD_APPLICATIONS=ON -DRYTHE_BUILD_OPTION_ASAN=ON -DRYTHE_FORCE_ENABLE_ALL_MODULES=ON
pause