g++ -I include -L lib -g main.cpp -w -static-libstdc++ -static-libgcc lib/libboost_system.a lib/libboost_thread.a lib/libboost_filesystem.a -lpthread -lssl -lcrypto -o webserverCpp

