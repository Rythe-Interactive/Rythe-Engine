@Echo off
Echo convert all source file EOL from CRLF to LF.
pause
Echo converting legion files...
cd ../legion
FOR /R %%F IN (*.h *.hpp *.c *.cpp *.inl) DO "../tools/dos2unix.exe" -o -s "%%F"
Echo finished converting legion files.
Echo applications will be next.
pause

Echo converting applications files...
cd ../applications
FOR /R %%F IN (*.h *.hpp *.c *.cpp *.inl) DO "../tools/dos2unix.exe" -o -s "%%F"
Echo finished converting all files.
pause