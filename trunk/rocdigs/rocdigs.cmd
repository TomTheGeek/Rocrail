rem rocdigs.cmd for making the DLL's.
echo You must install the Windows SDK for the [iphlpapi.lib] support!!!

..\rocs\bin\ogen rocdigs.xml
md bin
cd bin
del /Q *.*
cl /c /MD /I..\.. ..\impl\*.c
link /DLL /out:..\..\bin\p50.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib p50.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\p50x.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib p50x.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\srcp.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib srcp.obj srcp07.obj srcp08.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\hsi88.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib hsi88.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\rci.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib rci.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\lenz.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib lenz.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\zimo.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib zimo.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\editspro.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib editspro.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\dinamo.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib dinamo.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\om32.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib om32.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\slx.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib slx.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\barjut.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib barjut.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\inter10.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib inter10.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib
link /DLL /out:..\..\bin\loconet.dll /DEF:..\..\rocdigs\rocdigs.def iphlpapi.lib wsock32.lib mpr.lib advapi32.lib loconet.obj ..\..\rocrail\bin\wrapper\*.obj ..\..\rocs\bin\rocs.lib

cd ..
