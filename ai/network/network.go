package network

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"net/textproto"
	"os"
	"strconv"
	"strings"
	"syscall"
)

const SOL_SOCKET = 0x1
const TCP_NODELAY = 0x1
const TCP_QUICKACK = 0xc

type ServerConn struct {
	Connection      net.Conn
	Reader          *textproto.Reader
	LastCommandType CommandType
	// ResponseFeedbackChannel from the response routine
	ResponseFeedbackChannel chan bool
}

// CreateConnectionContext creates a new dialer context with specified options
func CreateConnectionContext() *net.Dialer {
	dialer := &net.Dialer{
		Control: func(network, address string, conn syscall.RawConn) error {
			var syscallError error = nil
			err := conn.Control(
				func(fd uintptr) {
					// syscallError = syscall.SetsockoptInt(int(fd), SOL_SOCKET, TCP_QUICKACK, 1)
				})
			if err != nil {
				return err
			}
			return syscallError
		},
	}
	return dialer
}

// GetTextReader returns a complete server connection with a text protocol reader
func GetTextReader(conn net.Conn) ServerConn {
	return ServerConn{conn, textproto.NewReader(bufio.NewReader(conn)), None, make(chan bool)}
}

// GetIdAndDims parses and returns the information sent by the server during the handshake
func GetIdAndDims(reader *textproto.Reader) (int, int, int, error) {
	slotsLine, err := reader.ReadLine()
	if err != nil {
		return 0, 0, 0, err
	}
	if slotsLine == "ko" {
		return 0, 0, 0, fmt.Errorf("Invalid number of slots\n")
	}
	slotsLeft, err := strconv.Atoi(slotsLine)
	if err != nil {
		return 0, 0, 0, err
	}

	dimsLine, err := reader.ReadLine()
	if err != nil {
		return 0, 0, 0, err
	}

	dims := strings.Split(dimsLine, " ")
	if len(dims) != 2 {
		return 0, 0, 0, fmt.Errorf("Invalid number of dimensions: %d\n", len(dims))
	}
	xDim, err := strconv.Atoi(dims[0])
	if err != nil {
		return 0, 0, 0, err
	}
	yDim, err := strconv.Atoi(dims[1])
	if err != nil {
		return 0, 0, 0, err
	}

	return slotsLeft, xDim, yDim, nil
}

// InitServerConnection opens a TCP socket to the server and does the handshake
func InitServerConnection(fullAddress string, teamName string, context *net.Dialer) (net.Conn, error) {
	conn, connErr := context.Dial("tcp4", fullAddress)
	if connErr != nil {
		return nil, connErr
	}

	welcomeMsg, welcomeErr := textproto.NewReader(bufio.NewReader(conn)).ReadLine()
	if welcomeErr != nil {
		_ = conn.Close()
		return nil, welcomeErr
	}
	if welcomeMsg != "WELCOME" {
		fmt.Println(welcomeMsg)
		_ = conn.Close()
		return nil, fmt.Errorf("Invalid welcome message\n")
	}
	log.Println("Welcome message received")

	_, teamNameErr := fmt.Fprintf(conn, "%s\n", teamName)
	if teamNameErr != nil {
		_ = conn.Close()
		_, _ = fmt.Fprintf(os.Stderr, "Failed to send to remote server at %s: %s\n", fullAddress, teamNameErr.Error())
		return nil, teamNameErr
	}
	log.Println("Sent team name", teamName)
	return conn, nil
}
