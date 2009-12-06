; -- rocrail.iss --

[Setup]
AppName=Rocrail
AppVerName=Rocrail <VER>.<PATCH> <TYPE>-rev<BZR>-<DIST>
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
OutputBaseFilename=rocrail-setup-<VER>.<PATCH>-rev<BZR>-<TYPE>-<DIST>

[Tasks]
Name: service; Description: "Register Rocrail as Service"; GroupDescription: "Service"; Flags: unchecked
Name: demoplan; Description: "Demo plan"
Name: images; Description: "Rocview images"
Name: themes; Description: "Rocview SVG Themes"
Name: symbols; Description: "WebClient Symbols"

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
Source: "plan.xml"; DestDir: "{userappdata}\Rocrail"; Flags: onlyifdoesntexist uninsneveruninstall
Source: "rocraild.ico"; DestDir: "{app}"
Source: "images\*.*"; DestDir: "{userappdata}\Rocrail\images"; Tasks: images; Flags: onlyifdoesntexist uninsneveruninstall
Source: "..\..\stylesheets\*.*"; DestDir: "{app}\stylesheets"
Source: "..\..\rocview\svg\themes\Accessories\*.*"; DestDir: "{app}\svg\themes\Accessories"; Tasks: themes;
Source: "..\..\rocview\svg\themes\Basic\*.*"; DestDir: "{app}\svg\themes\Basic"; Tasks: themes;
Source: "..\..\rocview\svg\themes\DB\*.*"; DestDir: "{app}\svg\themes\DB"; Tasks: themes;
Source: "..\..\rocview\svg\themes\SpDrS60\*.*"; DestDir: "{app}\svg\themes\SpDrS60"; Tasks: themes;
Source: "..\..\rocview\svg\themes\SBB-RHB\*.*"; DestDir: "{app}\svg\themes\SBB-RHB"; Tasks: themes;
Source: "..\..\rocview\svg\themes\NMBS-SNCB\*.*"; DestDir: "{app}\svg\themes\NMBS-SNCB"; Tasks: themes;
Source: "..\..\rocview\svg\themes\NL\*.*"; DestDir: "{app}\svg\themes\NL"; Tasks: themes;
Source: "..\..\rocview\svg\themes\Roads\*.*"; DestDir: "{app}\svg\themes\Roads"; Tasks: themes;
Source: "..\..\rocrail\symbols\*.*"; DestDir: "{app}\symbols"; Tasks: symbols; Flags: onlyifdoesntexist
Source: "..\..\COPYING"; DestDir: "{app}"

[Icons]
Name: "{group}\Rocview"; Parameters: "-sp ""{app}"" -themespath ""{app}"""; WorkingDir: "{userdocs}\Rocrail"; Filename: "{app}\rocview.exe"
Name: "{group}\Rocrail"; IconFilename: "{app}\rocraild.ico"; Parameters: "-console -l ""{app}"" -img ""{userappdata}\Rocrail\images"""; WorkingDir: "{userappdata}\Rocrail"; Filename: "{app}\rocrail.exe"

