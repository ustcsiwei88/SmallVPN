ALL: tun tun_ssl tun_naive

tun: tun.cpp
	g++ -std=c++11 -O2 tun.cpp -o tun -lpthread
tun_ssl: tun_ssl.cpp
	g++ -std=c++11 -O2 tun_ssl.cpp -o tun_ssl -lpthread -lcrypto -lssl
tun_naive: tun_naive.cpp
	g++ -std=c++11 -O2 tun_naive.cpp -o tun_naive -lpthread

