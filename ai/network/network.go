package network

import (
	"bufio"
	"fmt"
	"net"
	"os"
)

func InitServerConnection(fullAddress string, teamName string) (net.Conn, error) {
	conn, connErr := net.Dial("tcp4", fullAddress)
	if connErr != nil {
		_, _ = fmt.Fprintf(os.Stderr, "Failed to connect to remote server at %s: %s\n", fullAddress, connErr.Error())
		return nil, connErr
	}

	welcomeMsg, welcomeErr := bufio.NewReader(conn).ReadString('\n')
	if welcomeErr != nil || welcomeMsg != "WELCOME" {
		_ = conn.Close()
		_, _ = fmt.Fprintf(os.Stderr, "Invalid welcome message")
		return nil, welcomeErr
	}
	_, teamNameErr := fmt.Fprintf(conn, "%s\n", teamName)
	if teamNameErr != nil {
		_ = conn.Close()
		_, _ = fmt.Fprintf(os.Stderr, "Failed to send to remote server at %s: %s\n", fullAddress, teamNameErr.Error())
		return nil, teamNameErr
	}
	return conn, nil
}
