package network

import (
	"fmt"
	"io"
	"log"
	"net"
	"time"

	"github.com/google/gopacket"
	"github.com/google/gopacket/layers"
)

type Packet struct {
	No            int
	Time          time.Time
	SourceIP      string
	DestinationIP string
	Protocol      string
	length        uint16
	info          string
}

var (
	counter  int = 0
	metaData gopacket.PacketMetadata
	Quit     bool = false
)

func (p *Packet) GetPackets() {
	conn, err := net.Dial("unix", "/tmp/packetlens.sock")
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()

	func() {
		for {
			buf := make([]byte, 65536)
			n, err := conn.Read(buf)
			if err != nil {
				if err == io.EOF {
					fmt.Println("End of file")
					return
				}
				panic(err)
			}

			if Quit == true {
				conn.Write([]byte("quit"))
				return
			}

			ethPacket := gopacket.NewPacket(buf[:n], layers.LayerTypeEthernet, gopacket.Default)
			metaData.Timestamp = time.Now()
			ethPacket.Metadata().CaptureInfo = metaData.CaptureInfo

			if ethPacket != nil {
				p.Time = ethPacket.Metadata().Timestamp
				p.decodePacket(ethPacket)
			}
		}
	}()

}

func (p *Packet) decodePacket(packet gopacket.Packet) {

	ipLayer := packet.Layer(layers.LayerTypeIPv4)
	if ipLayer != nil {
		ip, _ := ipLayer.(*layers.IPv4)

		p.Protocol = ip.Protocol.String()
		p.length = ip.Length
		p.SourceIP = ip.SrcIP.String()
		p.DestinationIP = ip.DstIP.String()
	}

}

func (p *Packet) ConvRow() []string {
	counter++

	p.No = counter
	return []string{
		fmt.Sprintf("%d", p.No),
		p.Time.Format("15:04:05"),
		p.SourceIP,
		p.DestinationIP,
		p.Protocol,
		fmt.Sprintf("%d", p.length),
	}
}
