## ProdCon

A server/client model that allows up to 10 active clients from multiple machines to request transactions to be preformed.
The server will open a socket and listen for incoming client transaction requests, preform the desired action and send results back.

###Run:
Server:
 - make server
 - ./server [port]

Client: 
 - make client
 - ./client [port] [server-ip]
 - Send Transation: T<n>
 - Sleep client: S<n>

Man Pages:
 - make server.man / client.man generates man pages with further guidelines / instructions.