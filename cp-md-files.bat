@echo off

xcopy *.MD .\docs\source\ /y
push docs
doxygen
make html
popd