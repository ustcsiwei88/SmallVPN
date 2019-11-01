sudo ip addr add 10.0.4.1/24 dev tun0
sudo ifconfig tun0 up
sudo ip route add 10.0.3.0/24 dev tun0
sudo ip route add 172.217.12.174 dev tun0
