; -- rocrail.iss --

[Setup]
AppName=Rocrail
AppVerName=Rocrail 1.2.0 snapshot-revxxxx-unicode
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
OutputBaseFilename=rocrail-setup-1.2.0-revxxxx-snapshot-unicode

[Tasks]
Name: service; Description: "Register Rocrail as Service"; GroupDescription: "Service"; Flags: unchecked
Name: demoplan; Description: "Demo plan"
Name: rocrailini; Description: "Demo rocrail.ini"
Name: rocguiini; Description: "Demo rocgui.ini"
Name: icons; Description: "Rocview icons"
Name: themes; Description: "Rocview SVG Themes"
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
Source: "..\..\winbin\rocview.exe"; DestDir: "{app}"
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
Source: "..\..\winbin\clock.dll"; DestDir: "{app}"
Source: "..\..\winbin\mcs2.dll"; DestDir: "{app}"
Source: "..\..\winbin\barjut.dll"; DestDir: "{app}"
Source: "..\..\winbin\mingwm10.dll"; DestDir: "{app}"
Source: "plan.xml"; DestDir: "{userappdata}\Rocrail"; Flags: onlyifdoesntexist
Source: "neustadt.xml"; DestDir: "{userappdata}\Rocrail"
Source: "rocraild.ico"; DestDir: "{userappdata}\Rocrail"
Source: "rocrail.ini"; DestDir: "{userappdata}\Rocrail"; Tasks:rocrailini; Flags: onlyifdoesntexist
Source: "rocgui.ini"; DestDir: "{userappdata}\Rocrail"; Tasks: rocguiini; Flags: onlyifdoesntexist
Source: "..\..\rocgui\icons\*.*"; DestDir: "{app}\icons"; Tasks: icons;
Source: "..\..\stylesheets\*.*"; DestDir: "{app}\stylesheets"
Source: "..\..\rocgui\svg\themes\Accessories\*.*"; DestDir: "{app}\svg\themes\Accessories"; Tasks: themes;
Source: "..\..\rocgui\svg\themes\Basic\*.*"; DestDir: "{app}\svg\themes\Basic"; Tasks: themes;
Source: "..\..\rocgui\svg\themes\DB\*.*"; DestDir: "{app}\svg\themes\DB"; Tasks: themes;
Source: "..\..\rocgui\svg\themes\SpDrS60\*.*"; DestDir: "{app}\svg\themes\SpDrS60"; Tasks: themes;
Source: "..\..\rocgui\svg\themes\SBB-RHB\*.*"; DestDir: "{app}\svg\themes\SBB-RHB"; Tasks: themes;
Source: "..\..\rocgui\svg\themes\NMBS-SNCB\*.*"; DestDir: "{app}\svg\themes\NMBS-SNCB"; Tasks: themes;
Source: "..\..\rocgui\svg\themes\NL\*.*"; DestDir: "{app}\svg\themes\NL"; Tasks: themes;
Source: "..\..\rocgui\svg\themes\Roads\*.*"; DestDir: "{app}\svg\themes\Roads"; Tasks: themes;
Source: "..\..\rocrail\symbols\*.*"; DestDir: "{app}\symbols"; Tasks: symbols; Flags: onlyifdoesntexist
Source: "..\..\doc\*.*"; DestDir: "{app}\doc"; Tasks: doc;
Source: "..\..\COPYING"; DestDir: "{app}"; Tasks: doc;

[Icons]
Name: "{group}\Rocrail GUI"; Parameters: "-icons ""{app}\icons"" -theme ""{app}\svg\themes\SpDrS60"""; WorkingDir: "{userappdata}\Rocrail"; Filename: "{app}\rocview.exe"
Name: "{group}\Rocrail Server"; IconFilename: "{app}\rocraild.ico"; Parameters: "-console -l ""{app}"" -img ""{userappdata}\Rocrail\images"""; WorkingDir: "{userappdata}\Rocrail"; Filename: "{app}\rocrail.exe"
Name: "{group}\Documentation"; WorkingDir: "{app}"; Filename: "{app}\doc\index.html"

