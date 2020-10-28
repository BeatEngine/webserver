mkdir certs
cd certs
openssl dhparam -outform PEM -out dh2048.pem 2048
openssl genrsa -out privkey.pem 2048
openssl req -new -key privkey.pem -out certreq.csr
openssl x509 -req -days 3650 -in certreq.csr -signkey privkey.pem -out newcert.pem
