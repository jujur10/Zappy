package ai

import (
	"testing"
	"time"
)

func helperTest_awaitResponseToCommand(channel chan<- bool, value bool) {
	time.Sleep(10 * time.Millisecond)
	channel <- value
}

func Test_awaitResponseToCommand(t *testing.T) {
	channelFalse := make(chan bool)
	t.Run("Test False", func(t *testing.T) {
		go helperTest_awaitResponseToCommand(channelFalse, false)
		if got := awaitResponseToCommand(channelFalse); got != false {
			t.Errorf("awaitResponseToCommand() = %v, want %v", got, false)
		}
	})
	close(channelFalse)
	channelTrue := make(chan bool)
	t.Run("Test True", func(t *testing.T) {
		go helperTest_awaitResponseToCommand(channelTrue, true)
		if got := awaitResponseToCommand(channelTrue); got != true {
			t.Errorf("awaitResponseToCommand() = %v, want %v", got, true)
		}
	})
	close(channelTrue)
}
