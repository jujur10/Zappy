package network

import (
	"fmt"
)

type CommandType int
type MessageType int

const (
	RotateRight CommandType = iota
	RotateLeft
	GoForward
	LookAround
	GetInventory
	BroadcastText
	GetUnusedSlots
	Fork
	EjectPlayers
	TakeObject
	SetObject
	LevelUp
)

func (conn ServerConn) rotateRight() {
	_, _ = fmt.Fprintln(conn.Connection, "Right")
}

func (conn ServerConn) rotateLeft() {
	_, _ = fmt.Fprintln(conn.Connection, "Left")
}

func (conn ServerConn) goForward() {
	_, _ = fmt.Fprintln(conn.Connection, "Forward")
}

func (conn ServerConn) lookAround() {
	_, _ = fmt.Fprintln(conn.Connection, "Look")
}

func (conn ServerConn) getInventory() {
	_, _ = fmt.Fprintln(conn.Connection, "Inventory")
}

func (conn ServerConn) broadcastText(text string) {
	_, _ = fmt.Fprintf(conn.Connection, "Broadcast %s\n", text)
}

func (conn ServerConn) getUnusedSlots() {
	_, _ = fmt.Fprintln(conn.Connection, "Connect_nbr")
}

func (conn ServerConn) fork() {
	_, _ = fmt.Fprintln(conn.Connection, "Fork")
}

func (conn ServerConn) ejectPlayers() {
	_, _ = fmt.Fprintln(conn.Connection, "Eject")
}

func (conn ServerConn) takeObject(object string) {
	_, _ = fmt.Fprintf(conn.Connection, "Take %s\n", object)
}

func (conn ServerConn) dropObject(object string) {
	_, _ = fmt.Fprintf(conn.Connection, "Set %s\n", object)
}

func (conn ServerConn) levelUp() {
	_, _ = fmt.Fprintln(conn.Connection, "Incantation")
}

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
	default:
		break
	}
}
