rem roclcdr.cmd for making the DLL's.
echo You must install the Windows SDK for the [iphlpapi.lib] support!!!

..\rocs\bin\ogen roclcdr.xml
md bin
cd bin
del /Q *.*
cl /c /MD /I..\.. ..\impl\*.c
link /DLL /out:..\..\bin\lcdriver.dll /DEF:..\..\roclcdr\lcdriver.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib lcdriver.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\locdriver.dll /DEF:..\..\roclcdr\lcdriver.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib locdriver.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
cd ..
