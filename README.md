# TCP_ClientServer
Simple TCP Client Server project

# Server

Build the server :

```
cd Server
build.sh
```

Launch the server : 

```
./server
```

You will see the server launched successfully with the message :
```
Server listening on port 8080
```

# QT Client

Build the client :

```
cd QT_Client
qmake
make
```

Launch a client : 

On Linux :
```
./QTCPClient
```

On MacOS :
```
./QTCPClient.app/Contents/MacOS/QTCPClient
```


Select the server address, then connect.

<img width="392" alt="TCP1" src="https://github.com/DigitalBox98/TCP_ClientServer/assets/57635141/7a8d648e-55dd-4d15-8cd0-41cd005bdaf6">

Type your message : 

<img width="395" alt="TCP2" src="https://github.com/DigitalBox98/TCP_ClientServer/assets/57635141/a5429cce-db29-4911-b815-0af0695e0a0b">

The message is then broadcasted from the server to all TCP connected clients :)






