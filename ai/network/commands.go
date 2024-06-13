package network

import (
	"fmt"
	"log"
)

type CommandType int

const (
	// RotateRight command
	RotateRight CommandType = iota
	// RotateLeft command
	RotateLeft
	// GoForward command
	GoForward
	// LookAround command
	LookAround
	// GetInventory command
	GetInventory
	// BroadcastText command
	BroadcastText
	// GetUnusedSlots command
	GetUnusedSlots
	// Fork command
	Fork
	// EjectPlayers command
	EjectPlayers
	// TakeObject command
	TakeObject
	// SetObject command
	SetObject
	// LevelUp command
	LevelUp
	// GetFrequency command
	GetFrequency
	// GetDirection command
	GetDirection
	// None means no command
	None
)

const EmptyBody = ""

func (conn ServerConn) rotateRight() {
	_, err := fmt.Fprintln(conn.Connection, "Right")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) rotateLeft() {
	_, err := fmt.Fprintln(conn.Connection, "Left")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) goForward() {
	_, err := fmt.Fprintln(conn.Connection, "Forward")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) lookAround() {
	_, err := fmt.Fprintln(conn.Connection, "Look")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) getInventory() {
	_, err := fmt.Fprintln(conn.Connection, "Inventory")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) broadcastText(text string) {
	_, err := fmt.Fprintf(conn.Connection, "Broadcast %s\n", text)
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) getUnusedSlots() {
	_, err := fmt.Fprintln(conn.Connection, "Connect_nbr")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) fork() {
	_, err := fmt.Fprintln(conn.Connection, "Fork")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) ejectPlayers() {
	_, err := fmt.Fprintln(conn.Connection, "Eject")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) takeObject(object string) {
	_, err := fmt.Fprintf(conn.Connection, "Take %s\n", object)
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) dropObject(object string) {
	_, err := fmt.Fprintf(conn.Connection, "Set %s\n", object)
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) levelUp() {
	_, err := fmt.Fprintln(conn.Connection, "Incantation")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) GetFrequency() {
	_, err := fmt.Fprintln(conn.Connection, "Frequency")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

func (conn ServerConn) GetDirection() {
	_, err := fmt.Fprintln(conn.Connection, "Direction")
	if err != nil {
		log.Fatal("Failed to send message to server: ", err)
	}
}

// SendCommand sends the specified command to the serv, using the body string if needed
func (conn ServerConn) SendCommand(cmdType CommandType, body string) {
	switch cmdType {
	case RotateRight:
		conn.rotateRight()
		break
	case RotateLeft:
		conn.rotateLeft()
		break
	case GoForward:
		conn.goForward()
		break
	case LookAround:
		conn.lookAround()
		break
	case GetInventory:
		conn.getInventory()
		break
	case BroadcastText:
		conn.broadcastText(body)
		break
	case GetUnusedSlots:
		conn.getUnusedSlots()
		break
	case Fork:
		conn.fork()
		break
	case EjectPlayers:
		conn.ejectPlayers()
		break
	case TakeObject:
		conn.takeObject(body)
		break
	case SetObject:
		conn.dropObject(body)
		break
	case LevelUp:
		conn.levelUp()
		break
	case GetFrequency:
		conn.GetFrequency()
		break
	case GetDirection:
		conn.GetDirection()
		break
	default:
		break
	}
	conn.LastCommandType = cmdType
}
