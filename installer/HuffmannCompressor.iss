
#define MyAppName "Huffman Compressor"
#define MyAppVersion "1.0.0"
#define MyAppExeName "compressor.exe"

[Setup]
AppId=HuffmanCompressor
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher=Huffman Compressor
Uninstallable=yes
UninstallDisplayName=Huffman Compressor

DefaultDirName={localappdata}\Programs\Huffman Compressor
DefaultGroupName=Huffman Compressor

PrivilegesRequired=lowest

ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

OutputDir=..\Output
OutputBaseFilename=HuffmanCompressorSetup

Compression=lzma2
SolidCompression=yes
WizardStyle=modern

ChangesAssociations=yes
UninstallDisplayIcon={app}\compressor.exe

[Files]
Source: "..\src\compressor.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\assets\HuffmanArchive.ico"; DestDir: "{app}"; Flags: ignoreversion

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Icons]
Name: "{autoprograms}\Huffman Compressor"; Filename: "{app}\compressor.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\Huffman Compressor"; Filename: "{app}\compressor.exe"; WorkingDir: "{app}"; Tasks: desktopicon
Name: "{autoprograms}\Uninstall Huffman Compressor"; Filename: "{uninstallexe}"

[Registry]

; Extract context-menu option
Root: HKA; Subkey: "Software\Classes\HuffmanCompressor.Archive\shell\extract"; ValueType: string; ValueName: ""; ValueData: "Extract with Huffman Compressor"; Flags: uninsdeletekey

; Extraction command
Root: HKA; Subkey: "Software\Classes\HuffmanCompressor.Archive\shell\extract\command"; ValueType: string; ValueName: ""; ValueData: """{app}\compressor.exe"" extract ""%1"""

; -----------------------------------------
; COMPRESS: Right-click any file
; -----------------------------------------

Root: HKA; Subkey: "Software\Classes\*\shell\HuffmanCompressor.Compress"; ValueType: string; ValueName: ""; ValueData: "Compress with Huffman Compressor"; Flags: uninsdeletekey

Root: HKA; Subkey: "Software\Classes\*\shell\HuffmanCompressor.Compress"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\compressor.exe,0"

Root: HKA; Subkey: "Software\Classes\*\shell\HuffmanCompressor.Compress\command"; ValueType: string; ValueName: ""; ValueData: """{app}\compressor.exe"" compress ""%1"""

; -----------------------------------------
; EXTRACT: Right-click .huff archives
; -----------------------------------------

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.huff\shell\HuffmanCompressor.Extract"; ValueType: string; ValueName: ""; ValueData: "Extract with Huffman Compressor"; Flags: uninsdeletekey

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.huff\shell\HuffmanCompressor.Extract"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\compressor.exe,0"

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.huff\shell\HuffmanCompressor.Extract\command"; ValueType: string; ValueName: ""; ValueData: """{app}\compressor.exe"" extract ""%1"""

; Register the .huff extension
Root: HKA; Subkey: "Software\Classes\.huff"; ValueType: string; ValueName: ""; ValueData: "HuffmanCompressor.Archive"; Flags: uninsdeletevalue

; Register the archive file type
Root: HKA; Subkey: "Software\Classes\HuffmanCompressor.Archive"; ValueType: string; ValueName: ""; ValueData: "Huffman Compressed Archive"; Flags: uninsdeletekey

; Assign the custom icon
Root: HKA; Subkey: "Software\Classes\HuffmanCompressor.Archive\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\HuffmanArchive.ico,0"

; Double-click to extract
Root: HKA; Subkey: "Software\Classes\HuffmanCompressor.Archive\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\compressor.exe"" extract ""%1"""

[Run]
Filename: "{app}\compressor.exe"; Description: "Launch Huffman Compressor"; Flags: postinstall nowait skipifsilent
