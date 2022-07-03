# webserver

## MOST IMPORTANT BUG: Up- and downloads, longer than ca. 20 seconds are not working (TEST if yet not working)

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
run ./build-compile_static.sh
### Raspberry PI
run ./build-compile_static.sh

## Windows
# You need MinGW
# edit build-compile_static.bat --> change line set PATH=C:\MinGW\bin --> to your path!!!
run build-compile_static.bat
