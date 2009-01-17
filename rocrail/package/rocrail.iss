; -- rocrail.iss --

[Setup]
AppName=Rocrail
AppVerName=Rocrail 1.2.0 snapshot-revxxxx-unicode
DefaultDirName={pf}\Rocrail
DefaultGroupName=Rocrail
UninstallDisplayIcon={app}\rocgui.exe
Compression=lzma
SolidCompression=yes
DirExistsWarning=auto
OutputDir=..\..\package
AppCopyright=GNU
AppPublisher=rocrail.net
AppPublisherURL=http://www.rocrail.net/
OutputBaseFilename=rocrail-setup-1.2.0-revxxxx-snapshot-unicode

[Tasks]
Name: service; Description: "Register Rocrail as Service"; GroupDescription: "Service"; Flags: unchecked
Name: demoplan; Description: "Demo plan"
Name: rocrailini; Description: "Demo rocrail.ini"
Name: rocguiini; Description: "Demo rocgui.ini"
Name: icons; Description: "Rocgui icons"
Name: symbols; Description: "WebClient Symbols"
Name: doc; Description: "Documentation"

[Run]
Filename: "{sys}\sc.exe"; Parameters: "create rocrail binPath= ""{app}\rocrail.exe -service -w {app}"" start= auto";Tasks: service
Filename: "{sys}\sc.exe"; Parameters: "description rocrail ""Model Railroad Control Program""";Tasks: service

[UninstallRun]
Filename: "{sys}\sc.exe"; Parameters: "stop rocrail "
Filename: "{sys}\sc.exe"; Parameters: "delete rocrail "

[Code]
procedure StopService();
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{sys}\sc.exe'), 'stop rocrail', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
  Exec(ExpandConstant('{sys}\sc.exe'), 'delete rocrail', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
end;

[Files]
Source: "..\..\winbin\rocrail.exe"; DestDir: "{app}"; BeforeInstall: StopService; Tasks: service
Source: "..\..\winbin\rocrail.exe"; DestDir: "{app}"; Tasks: not service
Source: "stopservice.cmd"; DestDir: "{app}"
Source: "mkservice.cmd"; DestDir: "{app}"
Source: "..\..\winbin\rocgui.exe"; DestDir: "{app}"
Source: "..\..\winbin\lcdriver.dll"; DestDir: "{app}"
Source: "..\..\winbin\p50.dll"; DestDir: "{app}"
Source: "..\..\winbin\p50x.dll"; DestDir: "{app}"
Source: "..\..\winbin\hsi88.dll"; DestDir: "{app}"
Source: "..\..\winbin\dinamo.dll"; DestDir: "{app}"
Source: "..\..\winbin\om32.dll"; DestDir: "{app}"
Source: "..\..\winbin\srcp.dll"; DestDir: "{app}"
Source: "..\..\winbin\lenz.dll"; DestDir: "{app}"
Source: "..\..\winbin\roco.dll"; DestDir: "{app}"
Source: "..\..\winbin\slx.dll"; DestDir: "{app}"
Source: "..\..\winbin\zimo.dll"; DestDir: "{app}"
Source: "..\..\winbin\loconet.dll"; DestDir: "{app}"
Source: "..\..\winbin\ecos.dll"; DestDir: "{app}"
Source: "..\..\winbin\ddx.dll"; DestDir: "{app}"
Source: "..\..\winbin\virtual.dll"; DestDir: "{app}"
Source: "..\..\winbin\opendcc.dll"; DestDir: "{app}"
Source: "..\..\winbin\mingwm10.dll"; DestDir: "{app}"
Source: "plan.xml"; DestDir: "{app}"; Flags: onlyifdoesntexist
Source: "neustadt.xml"; DestDir: "{app}"
Source: "rocraild.ico"; DestDir: "{app}"
Source: "rocrail.ini"; DestDir: "{app}"; Tasks:rocrailini; Flags: onlyifdoesntexist
Source: "rocgui.ini"; DestDir: "{app}"; Tasks: rocguiini; Flags: onlyifdoesntexist
Source: "..\..\rocgui\icons\*.*"; DestDir: "{app}\icons"; Tasks: icons;
Source: "..\..\stylesheets\*.*"; DestDir: "{app}\stylesheets"
Source: "..\..\rocgui\svg\themes\Accessories\*.*"; DestDir: "{app}\svg\themes\Accessories"; Tasks: icons;
Source: "..\..\rocgui\svg\themes\Basic\*.*"; DestDir: "{app}\svg\themes\Basic"; Tasks: icons;
Source: "..\..\rocgui\svg\themes\DB\*.*"; DestDir: "{app}\svg\themes\DB"; Tasks: icons;
Source: "..\..\rocgui\svg\themes\SpDrS60\*.*"; DestDir: "{app}\svg\themes\SpDrS60"; Tasks: icons;
Source: "..\..\rocgui\svg\themes\SBB-RHB\*.*"; DestDir: "{app}\svg\themes\SBB-RHB"; Tasks: icons;
Source: "..\..\rocgui\svg\themes\NMBS-SNCB\*.*"; DestDir: "{app}\svg\themes\NMBS-SNCB"; Tasks: icons;
Source: "..\..\rocgui\svg\themes\NL\*.*"; DestDir: "{app}\svg\themes\NL"; Tasks: icons;
Source: "..\..\rocgui\svg\themes\Roads\*.*"; DestDir: "{app}\svg\themes\Roads"; Tasks: icons;
Source: "..\..\rocrail\symbols\*.*"; DestDir: "{app}\symbols"; Tasks: symbols; Flags: onlyifdoesntexist
Source: "..\..\doc\*.*"; DestDir: "{app}\doc"; Tasks: doc;
Source: "..\..\COPYING"; DestDir: "{app}"; Tasks: doc;

[Icons]
Name: "{group}\Rocrail GUI"; WorkingDir: "{app}"; Filename: "{app}\rocgui.exe"
Name: "{group}\Rocrail Server"; IconFilename: "{app}\rocraild.ico"; Parameters: "-console -l ""{app}"""; WorkingDir: "{app}"; Filename: "{app}\rocrail.exe"
Name: "{group}\Documentation"; WorkingDir: "{app}"; Filename: "{app}\doc\index.html"

