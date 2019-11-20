To generate the private key, run:
'''
$ openssl genrsa -des3 -passout pass:ABCD -out server.pass.key 2048
$ openssl rsa -passin pass:ABCD -in server.pass.key -out server.key
$ rm -f server.pass.key
'''

To genereate the cerificate signing request (.crs), run:
'''
$ openssl req -new -key server.key -out server.csr
'''

To genereate the self signed cerificate (.crt), run:
'''
$ openssl x509 -req -sha256 -days 365 -in server.csr -signkey server.key -out server.crt
$ rm -f server.csr
'''

Install openssl (in root):
'''
$ wget http://zlib.net/zlib-1.2.8.tar.gz
$ tar -zxf zlib-1.2.8.tar.gz
$ cd zlib-1.2.8/
$ ./configure  —-prefix=/usr/local
$ make
$ make install
'''