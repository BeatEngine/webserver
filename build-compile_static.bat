setlocal
set PATH=C:\MinGW\bin
g++ main.cpp -fpermissive --static -static-libstdc++ -static-libgcc -lwsock32 -lws2_32 -w -o webserverCpp.exe