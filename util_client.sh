sudo ip addr add 6.0.0.2 dev tun0
sudo ifconfig tun0 up
sudo ip route add 172.217.10.78 via 6.0.0.2
