sudo ip addr add 10.0.3.2/24 dev tun0
sudo ifconfig tun0 up
sudo ip route add 10.0.3.0/24 dev tun0
sudo ip route add 172.217.12.174 dev tun0
sudo ip route add 91.189.88.0/24 dev tun0
#sudo ip route add default via 10.0.3.1 dev tun0
#echo "modify /etc/resolv.conf if dns is not working, 128.6.1.1"