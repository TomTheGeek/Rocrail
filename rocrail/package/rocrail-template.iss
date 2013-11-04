; -- rocrail.iss --

[Setup]
AppName=Rocrail
AppVerName=Rocrail -rev<BZR>
DefaultDirName={pf}\Rocrail
DefaultGroupName=Rocrail
UninstallDisplayIcon={app}\rocview.exe
Compression=lzma
SolidCompression=yes
DirExistsWarning=auto
OutputDir=..\..\package
AppCopyright=GNU
AppPublisher=rocrail.net
AppPublisherURL=http://www.rocrail.net/
OutputBaseFilename=rocrail-<BZR>-win32

[Tasks]
;Name: service; Description: "Register Rocrail as Service"; GroupDescription: "Service"; Flags: unchecked
Name: demoplan; Description: "Demo plan"
Name: images; Description: "Loco images"
Name: themes; Description: "SVG Themes"
Name: americanthemes; Description: "SVG American Themes"
;Name: symbols; Description: "WebClient Symbols"

[Run]
;Filename: "{sys}\sc.exe"; Parameters: "create rocrail binPath= ""{app}\rocrail.exe -service -w {app}"" start= auto";Tasks: service
;Filename: "{sys}\sc.exe"; Parameters: "description rocrail ""Model Railroad Control Program""";Tasks: service

[UninstallRun]
;Filename: "{sys}\sc.exe"; Parameters: "stop rocrail "
;Filename: "{sys}\sc.exe"; Parameters: "delete rocrail "

[Code]
procedure StopService();
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{sys}\sc.exe'), 'stop rocrail', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
  Exec(ExpandConstant('{sys}\sc.exe'), 'delete rocrail', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
end;

[Files]
;Source: "..\..\winbin\rocrail.exe"; DestDir: "{app}"; BeforeInstall: StopService; Tasks: service
;Source: "..\..\winbin\rocrail.exe"; DestDir: "{app}"; Tasks: not service
Source: "..\..\winbin\rocrail.exe"; DestDir: "{app}"
Source: "stopservice.cmd"; DestDir: "{app}"
Source: "mkservice.cmd"; DestDir: "{app}"
Source: "..\..\winbin\rocview.exe"; DestDir: "{app}"
Source: "..\..\winbin\*.dll"; DestDir: "{app}"
Source: "plan.xml"; DestDir: "{userdocs}\Rocrail"; Tasks: demoplan; Flags: onlyifdoesntexist uninsneveruninstall
Source: "rocrail.mib"; DestDir: "{userdocs}\Rocrail";
Source: "rocraild.ico"; DestDir: "{app}"
Source: "images\*.*"; DestDir: "{userdocs}\Rocrail\images"; Tasks: images; Flags: onlyifdoesntexist uninsneveruninstall
Source: "..\..\stylesheets\*.*"; DestDir: "{userdocs}\Rocrail\stylesheets";
Source: "..\..\decspecs\*.*"; DestDir: "{userdocs}\Rocrail\decspecs"; Flags: recursesubdirs
Source: "..\..\rocview\svg\themes\*.*"; DestDir: "{app}\svg\themes"; Tasks: themes; Flags: recursesubdirs
Source: "..\..\rocview\svg\americanthemes\*.*"; DestDir: "{app}\svg\themes"; Tasks: americanthemes; Flags: recursesubdirs
;Source: "..\..\rocrail\symbols\*.*"; DestDir: "{app}\symbols"; Tasks: symbols; Flags: onlyifdoesntexist
Source: "..\..\COPYING"; DestDir: "{app}"

[Icons]
Name: "{group}\Rocview"; Parameters: "-sp ""{app}"" -themespath ""{app}"""; WorkingDir: "{userdocs}\Rocrail"; Filename: "{app}\rocview.exe"
Name: "{group}\Rocrail Server"; IconFilename: "{app}\rocraild.ico"; Parameters: "-console -l ""{app}"" -img ""{userdocs}\Rocrail\images"""; WorkingDir: "{userdocs}\Rocrail"; Filename: "{app}\rocrail.exe"

