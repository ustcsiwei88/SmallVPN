### WhereRU

This is the homepage or WhereRU. It is implemented in userspace and quite simple, but it's highly performant.

With a naive encryption: naive_tun

With no encrpytion: tun

The SSL version is still buggy: ssl_tun and tun_ssl

Here is how to run WhereRU (with tun_naive for example)

Server
```
$make tun_naive
$sudo ./tun_naive 0
```

Client
```
$make tun_naive
$sudo ./tun_naive 1 [Server IP]
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
