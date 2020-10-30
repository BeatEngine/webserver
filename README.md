# webserver

The server sets the correct "content-type" for audio, video, image and unknown files by suffix or requested type.

You can bind events to a path, returning the result body
or
bind files direct to a request path
and
all files in the subdirectory "WEB" are mapped directly /index.html --> ./WEB/index.html

Usage:
./server (-p port)       (Default port is 443 HTTPS enabled) (port 80 will change automatically to insecure HTTP)

