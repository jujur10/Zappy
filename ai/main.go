package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"os"
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
	conn.GetFrequency()
	respType := network.Nil
	for respType != network.Int {
		rType, value, err := conn.GetAndParseResponse()
		if rType == network.Int && err == nil {
			return value.(int)
		}
		respType = rType
	}
	return 0
}

// main is the main function DUH
func main() {
	teamName, fullAddress := parseArguments()
	connectionContext := network.CreateConnectionContext()
	conn, err := network.InitServerConnection(fullAddress, teamName, connectionContext)
	if err != nil {
		log.Fatal("Init server connection\n", err)
	}
	serverConn := network.GetTextReader(conn)
	slotsLeft, dimX, dimY, err := network.GetIdAndDims(serverConn.Reader)
	if err != nil {
		log.Fatal("Get id and dims\n", err)
	}
	fmt.Printf("Slots left: %d\ndimX %d\ndimY %d\n", slotsLeft, dimX, dimY)
	timeStep := getFrequencyFromServer(serverConn)
	if timeStep == 0 {
		log.Fatal("Failed to get timestep : Time step cannot be zero")
	}
	_ = ai.InitGame(serverConn, teamName, timeStep, slotsLeft)

	ai.AI()
}
