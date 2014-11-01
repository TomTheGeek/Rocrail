; -- rocrail.iss --

[Setup]
AppName=Rocrail
AppVerName=Rocrail -rev<BZR>
DefaultDirName=C:\Rocrail
DefaultGroupName=Rocrail
UninstallDisplayIcon=C:\Rocrail\rocview.exe
Compression=lzma
SolidCompression=yes
DirExistsWarning=auto
OutputDir=..\..\package
AppCopyright=GNU
AppPublisher=rocrail.net
AppPublisherURL=http://www.rocrail.net/
OutputBaseFilename=rocrail-root-<BZR>-win32

[Tasks]
Name: demoplan; Description: "Demo plan"
Name: images; Description: "Loco images"
Name: themes; Description: "SVG Themes"
Name: americanthemes; Description: "SVG American Themes"; Flags: unchecked

[Files]
Source: "..\..\winbin\rocrail.exe"; DestDir: "C:\Rocrail"
Source: "stopservice.cmd"; DestDir: "C:\Rocrail"
Source: "mkservice.cmd"; DestDir: "C:\Rocrail"
Source: "..\..\winbin\rocview.exe"; DestDir: "C:\Rocrail"
Source: "..\..\winbin\*.dll"; DestDir: "C:\Rocrail"
Source: "plan.xml"; DestDir: "C:\Rocrail\Demo"; Tasks: demoplan; Flags: onlyifdoesntexist uninsneveruninstall
Source: "rocrail.mib"; DestDir: "C:\Rocrail";
Source: "rocraild.ico"; DestDir: "C:\Rocrail"
Source: "images\*.*"; DestDir: "C:\Rocrail\images"; Tasks: images; Flags: onlyifdoesntexist uninsneveruninstall
Source: "..\..\stylesheets\*.*"; DestDir: "C:\Rocrail\stylesheets";
Source: "..\..\decspecs\*.*"; DestDir: "C:\Rocrail\decspecs"; Flags: recursesubdirs
Source: "..\..\rocview\svg\themes\*.*"; DestDir: "C:\Rocrail\svg\themes"; Tasks: themes; Flags: recursesubdirs
Source: "..\..\rocview\svg\americanthemes\*.*"; DestDir: "C:\Rocrail\svg\themes"; Tasks: americanthemes; Flags: recursesubdirs
Source: "..\..\COPYING"; DestDir: "C:\Rocrail"

[Icons]
Name: "{group}\Rocview"; Parameters: "-sp ""C:\Rocrail"" -themespath ""C:\Rocrail"""; WorkingDir: "C:\Rocrail"; Filename: "C:\Rocrail\rocview.exe"
Name: "{group}\Rocrail Server"; IconFilename: "C:\Rocrail\rocraild.ico"; Parameters: "-console -l ""C:\Rocrail"" -img ""C:\Rocrail\images"""; WorkingDir: "C:\Rocrail"; Filename: "C:\Rocrail\rocrail.exe"

