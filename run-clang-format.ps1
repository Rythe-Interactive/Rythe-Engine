Get-ChildItem -Path . -Directory -Recurse |
    foreach {
        cd $_.FullName
	Write-host "Formatting: $_"
        &D:\Perforce\rythe-interactive\Rythe-Engine\tools\clang-format -style=file -i *.cpp
        &D:\Perforce\rythe-interactive\Rythe-Engine\tools\clang-format -style=file -i *.hpp
        &D:\Perforce\rythe-interactive\Rythe-Engine\tools\clang-format -style=file -i *.inl
    }