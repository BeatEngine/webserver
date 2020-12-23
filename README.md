# webserver

The server sets the correct "content-type" for audio, video, image and unknown files by suffix or requested type.

You can bind events to a path, accessing the request body and returning the result body
or
bind files direct to a request path
and
all files in the subdirectory "WEB" are mapped directly /index.html --> ./WEB/index.html

The server set each new client a Cookie "session" with an random uuid (accessable in HTTPrequest.cookies).

Usage:
You need the certificate files in ./certs/ -> newcert.pem, privkey.pem, dh2048.pem  --> use the generateCertificates.sh script.
./server (-p port) (-s)(silent - no console output)    (Default port is 443 HTTPS enabled) (port 80 will change automatically to insecure HTTP)

# Build
## Linux
Use build-compile_static.sh or alternate build-compile.sh
### Raspberry PI
Use build-compile_raspberrypi.sh

## Windows
Install Visual Studio (Desktop-Apps C++, Chose newest Windows (10) SDK and check also).

Run WindowsIncludeAndLib.exe for decompressing the include and lib folder into the webserver-master directory.

Open WebserverCpp.vcxproj with Visual Studio
Go to Menu Project->Settings->Configuration Windows SDK --> choose the version one you have installed before.

Build x64 (Release)
