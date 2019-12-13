###
This is the homepage or WhereRU, we have implemented a highly performant software

With a naive 
$make tun_naive 

With not encrpytion
$make tun

The SSL version is still buggy: ssl_tun.cpp and tun_ssl.cpp

Here is how to run WhereRU (with tun_naive for example)

Server
```
$make tun_naive
$sudo ./tun 0
```

Client
```
$make tun_naive
$sudo ./tun 1 [Server IP]
```

Then the secure connection is established, run the configuration scripts now

Server
```
$sudo sh util_server.sh
```

Client
```
$sudo sh util_client.sh
```
