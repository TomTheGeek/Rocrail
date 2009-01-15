echo You must install the Windows SDK for the [iphlpapi.h] support!!!
echo ...Or use the rocs/impl/win/iphlpapi.zip...

bin\ogen rocs.xml
md bin
cd bin
del /Q *.*
cl /c /MD /I..\.. ..\impl\*.c

rem cl /c /MD /D__OPENSSL__ /I..\.. ..\impl\win\*.c
cl /c /MD /I..\.. ..\impl\win\*.c
link -lib /out:rocs.lib *.obj
cl /c /MD /I..\.. ..\gen\*.c
link /out:wgen.exe iphlpapi.lib mpr.lib wsock32.lib advapi32.lib wgen.obj rocs.lib
link /out:ogen.exe iphlpapi.lib mpr.lib wsock32.lib advapi32.lib ogen.obj rocs.lib
link /out:xml2cstr.exe iphlpapi.lib mpr.lib wsock32.lib advapi32.lib xml2cstr.obj rocs.lib
cd ..
