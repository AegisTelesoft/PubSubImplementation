protoc -I="%cd%" --cpp_out="%cd%" "%cd%/Message.proto"
protoc -I="%cd%" --csharp_out="%cd%" "%cd%/Message.proto"
protoc -I="%cd%" --java_out="%cd%" "%cd%/Message.proto"

pause