sudo g++ ssl.cpp -Wall -O0 -g3 -std=c++11 -lcrypto -lssl -o ssl
sudo g++ ssl_tun.cpp -O2 -std=c++11 -lpthread -lcrypto -lssl -o ssl_tun
