sudo sysctl -w net.ipv4.ip_forward=1
sudo ip addr add 6.0.0.3 dev tun0
sudo ifconfig tun0 up
