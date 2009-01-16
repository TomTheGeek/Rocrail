rem rocrail.cmd
echo You must install the Windows SDK for the [iphlpapi.lib] support!!!

..\rocs\bin\ogen rocrail.xml
..\rocs\bin\wgen public\wrapper.xml
..\rocs\bin\xml2cstr public\wrapper.xml wrapper\impl\wrapperinfo.c wrapperinfo
..\rocs\bin\xml2cstr wrapper\doc\wrapper-en.html wrapper\impl\rocrail_doc.c rocrail_doc

md bin
cd bin
del /Q *.*
md wrapper
cd wrapper
del /Q *.*
cl /Zm150 /c /MD /I..\..\.. ..\..\wrapper\impl\*.c
cd ..
cl /c /MD /I..\.. ..\impl\*.c
link /out:..\..\bin\rocrail.exe iphlpapi.lib wsock32.lib mpr.lib advapi32.lib *.obj wrapper\*.obj ..\..\rocs\bin\rocs.lib
link -lib /out:wrapper.lib wrapper\*.obj

cd ..
