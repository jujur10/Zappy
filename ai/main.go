package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"net"
	"os"
	"time"
	"zappy_ai/ai"
	"zappy_ai/network"
)

// parseArguments parses the CLI arguments and returns them in a useful form
func parseArguments() (string, string) {
	if len(os.Args) == 2 && (os.Args[1] == "-help" || os.Args[1] == "--help") {
		fmt.Println("USAGE: ./zappy_ai -p port -n name -h machine")
		os.Exit(0)
	}

	if len(os.Args) != 5 && len(os.Args) != 7 {
		_, _ = fmt.Fprintf(os.Stderr, "Invalid number of arguments: %d\n", len(os.Args)-1)
		os.Exit(84)
	}
	pArg := flag.Int("p", -1, "port number")
	nArg := flag.String("n", "", "name of the team")
	hArg := flag.String("h", "localhost", "name of the machine; localhost by default")
	flag.Parse() // Parse specified arguments

	port := *pArg
	name := *nArg
	host := *hArg
	// Check error cases
	if port < 1 || port > 65535 {
		_, _ = fmt.Fprintf(os.Stderr, "Invalid port number, port must be between 1 and 65535\n")
		os.Exit(84)
	}
	if name == "" {
		_, _ = fmt.Fprintf(os.Stderr, "Please specify a name for the team\n")
		os.Exit(84)
	}
	if host == "" {
		_, _ = fmt.Fprintf(os.Stderr, "Please specify a name for the host\n")
		os.Exit(84)
	}
	fullAddr := fmt.Sprintf("%s:%d", host, port)
	_, hostPortErr := net.ResolveTCPAddr("tcp4", fullAddr)
	if hostPortErr != nil {
		_, _ = fmt.Fprintf(os.Stderr, "Invalid host / port combination: %s:%d\n", host, port)
		os.Exit(84)
	}
	return name, fullAddr
}

// getFrequencyFromServer fetches the time step / frequency of the AI from the server
// It ignores any other messages coming before this one
func getFrequencyFromServer(conn network.ServerConn) int {
	conn.SendCommand(network.GetFrequency, network.EmptyBody)
	for {
		rType, value, err := conn.GetAndParseResponse()
		if err != nil {
			log.Fatal("Failed to get frequency from server: ", err)
		}
		if rType == network.Boolean && value == false {
			ai.FrequencyCommandAvailable = false
			log.Println("Failed to get frequency from server; using approximation method")
			timingSum := time.Duration(0)
			count := 0
			for count < 5 {
				timerStart := time.Now()
				conn.SendCommand(network.GetInventory, network.EmptyBody)
				invType, _, _ := conn.GetAndParseResponse()
				if invType == network.Inventory {
					duration := time.Since(timerStart)
					timingSum += duration
					log.Println("Sampling duration:", duration)
					count++
				}
			}
			timingSumSecs := timingSum.Microseconds() / 5
			timeStepFloat := float64(time.Second.Microseconds()) / float64(timingSumSecs)
			log.Println("Timing Mean:", timingSumSecs, ";Approximated timestep:", int(math.Round(timeStepFloat)))
			return int(math.Round(timeStepFloat))
		}
		if rType == network.Frequency {
			return value.(int)
		}
	}
	return 0
}

// main is the main function DUH
func main() {
	teamName, fullAddress := parseArguments()
	connectionContext := network.CreateConnectionContext()
	conn, err := network.InitServerConnection(fullAddress, teamName, connectionContext)
	if err != nil {
		log.Fatal("Init server connection: ", err)
	}
	serverConn := network.GetTextReader(conn)
	slotsLeft, dimX, dimY, err := network.GetIdAndDims(serverConn.Reader)
	if err != nil {
		log.Fatal("Get id and dims: ", err)
	}
	log.Printf("Slots left: %d; dimX %d; dimY %d\n", slotsLeft, dimX, dimY)
	log.Println("Handshake complete")
	timeStep := getFrequencyFromServer(serverConn)
	if timeStep == 0 {
		log.Fatal("Failed to get timestep: Time step cannot be zero")
	}
	log.Println("Got timestep:", timeStep)
	game := ai.InitGame(serverConn, teamName, timeStep, slotsLeft, ai.RelativeCoordinates{dimX, dimY})
	log.Println("AI initialized")
	game.StartRoutines()
	game.MainLoop()
	game.EndGame()
}
