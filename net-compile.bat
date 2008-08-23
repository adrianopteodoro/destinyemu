@ECHO OFF
IF EXIST Server.exe DEL Server.exe
IF EXIST Server.pdb DEL Server.pdb

set FRAMEWORK=%windir%\Microsoft.NET\\Framework\v2.0.50727

cd Source
%FRAMEWORK%\csc.exe /noconfig /unsafe+ /nowarn:1701,1702 /errorreport:prompt /warn:4 /define:TRACE /main:server.Core /reference:%FRAMEWORK%\System.Data.dll /reference:%FRAMEWORK%\System.dll /reference:%FRAMEWORK%\System.Xml.dll /reference:%FRAMEWORK%\System.Drawing.dll /reference:%FRAMEWORK%\System.Windows.Forms.dll /debug+ /debug:full /optimize+ /out:..\Server.exe /target:exe /win32icon:icon.ico /recurse:*.cs
copy config.xml ..\config.xml
copy encryption.dll ..\encryption.dll