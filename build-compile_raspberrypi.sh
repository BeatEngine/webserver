if dpkg -s libboost1.62-all-dev | grep "Status:" = "Status: install ok installed"
  then
   echo "Checked dependencies!\n"
  else
   sudo apt install libboost1.62-all-dev
fi
g++ -I include -L lib -g main.cpp -w -static-libstdc++ -static-libgcc lib/libboost_system.a lib/libboost_thread.a -lboost -lpthread -lssl -lcrypto -o webserverCpp
