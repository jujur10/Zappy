package network

import (
	"bufio"
	"fmt"
	"net"
	"net/textproto"
	"os"
	"strconv"
	"strings"
	"syscall"
)

const SOL_SOCKET = 0x1
const TCP_QUICKACK = 0xc

type ServerConn struct {
	Connection net.Conn
	Reader     *textproto.Reader
}

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

func GetTextReader(conn net.Conn) ServerConn {
	return ServerConn{conn, textproto.NewReader(bufio.NewReader(conn))}
}

func GetIdAndDims(reader *textproto.Reader) (int, int, int, error) {
	idLine, err := reader.ReadLine()
	if err != nil {
		return 0, 0, 0, err
	}
	if idLine == "ko" {
		return 0, 0, 0, fmt.Errorf("Invalid team name\n")
	}
	id, err := strconv.Atoi(idLine)
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

	return id, xDim, yDim, nil
}

func InitServerConnection(fullAddress string, teamName string, context *net.Dialer) (net.Conn, error) {
	conn, connErr := context.Dial("tcp4", fullAddress)
	if connErr != nil {
		_, _ = fmt.Fprintf(os.Stderr, "Failed to connect to remote server at %s: %s\n", fullAddress, connErr.Error())
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

	_, teamNameErr := fmt.Fprintf(conn, "%s\n", teamName)
	if teamNameErr != nil {
		_ = conn.Close()
		_, _ = fmt.Fprintf(os.Stderr, "Failed to send to remote server at %s: %s\n", fullAddress, teamNameErr.Error())
		return nil, teamNameErr
	}
	return conn, nil
}
