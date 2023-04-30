### SmallVPN

This is the homepage for SmallVPN, a VPN built for a school project. It is implemented completely in the userspace and is quite simple, but it is highly performant.

The SSL version: AES128-SHA

Naive encryption version: tun_naive

No encrpytion version: tun

---

Here is how to run SmallVPN (with tun_ssl for example)

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
