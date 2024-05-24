package network

import (
	"bufio"
	"fmt"
	"golang.org/x/sys/unix"
	"net"
	"os"
	"syscall"
)

func CreateConnectionContext() *net.Dialer {
	dialer := &net.Dialer{
		Control: func(network, address string, conn syscall.RawConn) error {
			var syscallError error
			err := conn.Control(
				func(fd uintptr) {
					syscallError = syscall.SetsockoptInt(int(fd), unix.SOL_SOCKET, unix.TCP_QUICKACK, 1)
				})
			if err != nil {
				return err
			}
			return syscallError
		},
	}
	return dialer
}

func InitServerConnection(fullAddress string, teamName string, context *net.Dialer) (net.Conn, error) {
	conn, connErr := context.Dial("tcp4", fullAddress)
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
