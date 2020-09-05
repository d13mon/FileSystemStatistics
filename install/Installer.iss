; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!


#define AppName "FileSystemStatistics"  
#define AppExeName "FileSystemStatistics.exe"  
#define BuildVersion "1.0"    

[Setup]
AppName={#AppName}
AppVersion={#BuildVersion}
VersionInfoVersion = {#BuildVersion}   
DefaultDirName={userdesktop}\{#AppName}  
Compression=zip
SolidCompression=yes
OutputDir=..\build\Release
OutputBaseFilename={#AppName}_{#BuildVersion} 
Uninstallable=false          
                                                                             
[Files]   

Source: "..\build\Release\FileSystemStatistics.exe"; DestDir: "{app}"; Flags: ignoreversion;   

Source: "..\build\Release\Qt5Core.dll"; DestDir: "{app}";  Flags: ignoreversion;
Source: "..\build\Release\Qt5Gui.dll"; DestDir: "{app}";  Flags: ignoreversion;    
Source: "..\build\Release\Qt5Widgets.dll"; DestDir: "{app}";  Flags: ignoreversion;    

Source: "..\build\Release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs;
Source: "..\build\Release\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs; 

Source: "data\vc_redist\vc_redist_2017_x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall nocompression;    

[Run] 

Filename: "{tmp}\vc_redist_2017_x64.exe"; Parameters: "/install /passive /norestart /silent"; StatusMsg: "Installing Microsoft Visual C++ 2017 Redistributable..."; Description: "Install VC++ 2017 Redistributable"  

[Icons]
;Name: "{app}\{#AppExeName}"; Filename: "{app}\{#AppExeName}"; Parameters: --np;     

[Code]


function ShouldSkipPage(PageID: Integer): Boolean;
begin
    if PageID = wpWelcome then      
        Result := true;  
    if PageID = wpSelectTasks then      
        Result := true;         
    if PageID = wpLicense then
      Result := true;
    if PageID = wpSelectDir then
      Result := false;
    if PageID = wpSelectComponents then
      Result := true;
    if PageID = wpSelectProgramGroup then
      Result := true;
   
    if PageID = wpReady then
      Result := true;
    if PageID = wpInfoAfter then
      Result := true; 
    if PageID = wpFinished then
      Result := true;            
end;


