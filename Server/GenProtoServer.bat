SET curPath=%cd%
SET projectPath=%1

CD %projectPath%
CD ..

SET rootPath=%cd%
SET outputPath=%rootPath%%2

CD Common\Protocol

protoc.exe -I=./ --csharp_out=%outputPath% ./Protocol.proto ./Enum.proto ./Struct.proto
::IF ERRORLEVEL 1 PAUSE

CD %curPath%

EXIT