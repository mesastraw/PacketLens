package main

import (
	"fmt"
	"io"
	"log"
	"net"

	"github.com/google/gopacket"
	_ "github.com/google/gopacket"
	"github.com/google/gopacket/layers"
)

func main() {
	conn, err := net.Dial("unix", "/tmp/packetlens.sock")
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()

	for {
		buf := make([]byte, 65536)
		n, err := conn.Read(buf)
		if err != nil {
			if err == io.EOF {
				fmt.Println("ENd of file")
				return
			}
			panic(err)
		}

		packet := gopacket.NewPacket(buf[:n], layers.LayerTypeEthernet, gopacket.Default)
		fmt.Println("Got packet: ", packet)
	}
}
