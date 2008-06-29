#!/bin/sh
cd Source
gmcs -define:MONO -unsafe+ -optimize -target:exe -out:../Server_Mono.exe -win32icon:icon.ico -recurse:*.cs -reference:bin/MySql.Data.dll -reference:System.Data -reference:System.Drawing -reference:System.Xml -reference:System.Windows.Forms
cp config.xml ../config.xml
cp encryption.dll ../encryption.dll
cp bin/MySql.Data.dll ../MySql.Data.dll 
