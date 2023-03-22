### SmallVPN

This is the homepage or WhereRU, a VPN built for school projects. It is implemented in userspace and quite simple, but it's highly performant.

The SSL version: AES128-SHA

Naive encryption version: tun_naive

No encrpytion version: tun

---

Here is how to run WhereRU (with tun_ssl for example)

Server
```
$make tun_ssl
$sudo ./tun_ssl 0 [PORT]
```

Client
```
$make tun_ssl
$sudo ./tun_ssl 1 [PORT] [Server IP]
```

Then the secure connection will be established, run the configuration scripts.

Server
```
$sudo sh util_server.sh
```

Client
```
$sudo sh util_client.sh
```
