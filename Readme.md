### WhereRU

This is the homepage or WhereRU. It is implemented in userspace and quite simple, but it's highly performant.

The SSL version: AES128-SHA

Naive encryption version: tun_naive

No encrpytion version: tun

---

Here is how to run WhereRU (with tun_ssl for example)

Server
```
$make tun_ssl
$sudo ./tun_ssl [PORT] 0
```

Client
```
$make tun_naive
$sudo ./tun_naive 1 [PORT] [Server IP]
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
