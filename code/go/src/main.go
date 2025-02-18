package main

import (
	"log"
	"time"

	ui "github.com/gizak/termui/v3"
	"github.com/gizak/termui/v3/widgets"
	"github.com/mesastraw/PacketLens/internal/network"
)

func main() {
	packet := new(network.Packet)

	go packet.GetPackets()

	if err := ui.Init(); err != nil {
		log.Fatalf("failed to initialize termui: %v", err)
	}
	defer ui.Close()

	headerRow := [][]string{
		{"No. ", "Time", "Source", "Destination", "Protocol", "Length"},
	}

	dataRow := [][]string{}

	table := widgets.NewTable()
	table.Rows = headerRow

	table.TextStyle = ui.NewStyle(ui.ColorWhite)
	termWidth, termHeight := ui.TerminalDimensions()
	table.SetRect(0, 0, termWidth, termHeight)

	grid := ui.NewGrid()
	grid.SetRect(0, 0, termWidth, termHeight)
	grid.Set(
		ui.NewRow(1.0, table),
	)

	ui.Render(grid)
	uiEvents := ui.PollEvents()
	ticker := time.NewTicker(time.Second).C

	scroll := 0
	maxRows := termHeight

	for {
		select {
		case e := <-uiEvents:
			switch e.ID {
			case "q", "<C-c>":
				network.Quit = true
				return
			case "<MouseWheelUp>":
				if scroll > 0 {
					scroll--

				}
			case "<MouseWheelDown>":
				if scroll < len(dataRow)-maxRows {
					scroll++
				}
			}

		case <-ticker:
			dataRow = append(dataRow, packet.ConvRow())

			visStart := scroll
			visEnd := scroll + maxRows

			if visEnd > len(dataRow) {
				visEnd = len(dataRow)
			}

			termWidth, termHeight = ui.TerminalDimensions()
			grid.SetRect(0, 0, termWidth, termHeight)
			table.Rows = append(headerRow, dataRow[visStart:visEnd]...)
			ui.Render(grid)
		}

	}
}
