rem rocgui.cmd

echo You must install the Windows SDK for the [iphlpapi.lib] support!!!



..\rocs\bin\wgen public\wrapper.xml

..\rocs\bin\xml2cstr res\messages.xml res\messages.c messages
..\rocs\bin\xml2cstr public\wrapper.xml wrapper\impl\guiwrapperinfo.c guiwrapperinfo



set wxroot=D:\E_Daten\workspace\wxWidgets-2.6.2\



md bin

cd bin

del /Q *.*

md wrapper

cd wrapper

del /Q *.*

cl /c /MD /I..\..\.. ..\..\wrapper\impl\*.c*

cd ..

cl /c /MD /I..\.. ..\res\*.c*

cl /c -D_UNICODE -D__WXMSW__ /MD /GX /I..\.. /I%wxroot%\include /I%wxroot%\include\msvc ..\impl\*.c*
cl /c -D_UNICODE -D__WXMSW__ /MD /GX /I..\.. /I%wxroot%\include /I%wxroot%\include\msvc ..\dialogs\*.c*
cl /c -D_UNICODE -D__WXMSW__ /MD /GX /I..\.. /I%wxroot%\include /I%wxroot%\include\msvc ..\dialogs\controllers\*.c*
cl /c -D_UNICODE -D__WXMSW__ /MD /GX /I..\.. /I%wxroot%\include /I%wxroot%\include\msvc ..\dialogs\lnmodules\*.c*
cl /c -D_UNICODE -D__WXMSW__ /MD /GX /I..\.. /I%wxroot%\include /I%wxroot%\include\msvc ..\symbols\*.c*

rc /forocgui.res /d WIN32 /d __WXMSW__ /i %wxroot%\include ..\rocgui.rc

link /subsystem:windows /out:..\..\bin\rocgui.exe /LIBPATH:%wxroot%\lib\vc_lib iphlpapi.lib mpr.lib wsock32.lib advapi32.lib *.obj rocgui.res wrapper\*.obj ..\..\rocrail\bin\rcon.obj ..\..\rocs\bin\rocs.lib ..\..\rocrail\bin\wrapper.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib winmm.lib

cd ..



