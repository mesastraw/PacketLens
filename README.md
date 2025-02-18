# PacketLens

## Purpose
This was written as a project to showcase for the Nokia highschool summer internship. 

Simple packet catcher that displays basic information about the packets caught.
The server that catches the packets is written in c.
The client is written in go and displays the information about the packets in the terminal.

Note: This was only tested on Linux

## Installation

Clone the git repo:
```
git clone https://github.com/mesastraw/PacketLens.git
```

Enter the dir of the cloned repo and run the build script:
``` 
cd PacketLens
./build.sh
```

run packetCather first with sudo:
```
sudo ./bin/packetCatcher
```

IMPORTAT!!!: packetCatcher needs to be run before client or the unix socket wont be created

Then run client in a seperate terminal with sudo aswell:
```
sudo ./bin/client
```

To quit press q

give the table some time to fill and then you can scroll up and down the table using scroll wheel
