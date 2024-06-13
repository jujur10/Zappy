package ai

import (
	"reflect"
	"testing"
)

func Test_interpretMessage(t *testing.T) {
	type args struct {
		message string
	}
	tests := []struct {
		name    string
		args    args
		want    broadcastMessageContent
		wantErr bool
	}{
		{"Invalid message 1", args{message: "aaa123 Hello 6"}, broadcastMessageContent{}, true},
		{"Invalid message 2", args{message: "aaa123"}, broadcastMessageContent{}, true},
		{"Valid Ready", args{message: "aaa123 Ready 5, missing 3"}, broadcastMessageContent{uuid: "aaa123", msgType: missingPlayers, targetLevel: 5, missingPlayers: 3}, false},
		{"Invalid Ready level 1", args{message: "aaa123 Ready 0, missing 3"}, broadcastMessageContent{}, true},
		{"Invalid Ready level 2", args{message: "aaa123 Ready 9, missing 3"}, broadcastMessageContent{}, true},
		{"Invalid Ready level 3", args{message: "aaa123 Ready a, missing 3"}, broadcastMessageContent{}, true},
		{"Invalid Ready missing 1", args{message: "aaa123 Ready 5, missing 0"}, broadcastMessageContent{}, true},
		{"Invalid Ready missing 2", args{message: "aaa123 Ready 5, missing 5"}, broadcastMessageContent{}, true},
		{"Invalid Ready missing 3", args{message: "aaa123 Ready 5, missing b"}, broadcastMessageContent{}, true},
		{"Valid Cancel", args{message: "aaa123 Cancel 4"}, broadcastMessageContent{uuid: "aaa123", msgType: cancelLvlUp, targetLevel: 4}, false},
		{"Invalid Cancel level 1", args{message: "aaa123 Cancel 0"}, broadcastMessageContent{}, true},
		{"Invalid Cancel level 2", args{message: "aaa123 Cancel 9"}, broadcastMessageContent{}, true},
		{"Invalid Cancel level 3", args{message: "aaa123 Cancel c"}, broadcastMessageContent{}, true},
		{"Valid Join", args{message: "aaa123 Join 4"}, broadcastMessageContent{uuid: "aaa123", msgType: announcePresence, targetLevel: 4}, false},
		{"Invalid Join level 1", args{message: "aaa123 Join 0"}, broadcastMessageContent{}, true},
		{"Invalid Join level 2", args{message: "aaa123 Join 9"}, broadcastMessageContent{}, true},
		{"Invalid Join level 3", args{message: "aaa123 Join j"}, broadcastMessageContent{}, true},
		{"Valid Starting", args{message: "aaa123 Starting 4"}, broadcastMessageContent{uuid: "aaa123", msgType: startLvlUp, targetLevel: 4}, false},
		{"Invalid Starting level 1", args{message: "aaa123 Starting 0"}, broadcastMessageContent{}, true},
		{"Invalid Starting level 2", args{message: "aaa123 Starting 9"}, broadcastMessageContent{}, true},
		{"Invalid Starting level 3", args{message: "aaa123 Starting s"}, broadcastMessageContent{}, true},
		{"Valid Reached", args{message: "aaa123 Reached 4"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpComplete, targetLevel: 4}, false},
		{"Invalid Reached level 1", args{message: "aaa123 Reached 0"}, broadcastMessageContent{}, true},
		{"Invalid Reached level 2", args{message: "aaa123 Reached 9"}, broadcastMessageContent{}, true},
		{"Invalid Reached level 3", args{message: "aaa123 Reached r"}, broadcastMessageContent{}, true},
		{"Valid Failed", args{message: "aaa123 Failed 4"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpFailed, targetLevel: 4}, false},
		{"Invalid Failed level 1", args{message: "aaa123 Failed 0"}, broadcastMessageContent{}, true},
		{"Invalid Failed level 2", args{message: "aaa123 Failed 9"}, broadcastMessageContent{}, true},
		{"Invalid Failed level 3", args{message: "aaa123 Failed f"}, broadcastMessageContent{}, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := parsePlayerMessage(tt.args.message)
			if (err != nil) != tt.wantErr {
				t.Errorf("parsePlayerMessage() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("parsePlayerMessage() got = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_getMessageIndex(t *testing.T) {
	type args struct {
		content     broadcastMessageContent
		messageList []broadcastMessageContent
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{"Value in array", args{content: broadcastMessageContent{uuid: "aaa123"}, messageList: []broadcastMessageContent{{uuid: "abc124"}, {uuid: "aaa123"}, {uuid: "azerty1235"}}}, 1},
		{"Value not in array", args{content: broadcastMessageContent{uuid: "aaa123"}, messageList: []broadcastMessageContent{{uuid: "abc124"}, {uuid: "azerty1235"}}}, -1},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := getMessageIndex(tt.args.content, tt.args.messageList); got != tt.want {
				t.Errorf("getMessageIndex() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_parseMessageLevelAndReturn(t *testing.T) {
	type args struct {
		levelStr string
		uuid     string
		msgType  broadcastType
	}
	tests := []struct {
		name    string
		args    args
		want    broadcastMessageContent
		wantErr bool
	}{
		{"Test invalid value", args{levelStr: "aaa", uuid: "aaa123", msgType: announcePresence}, broadcastMessageContent{}, true},
		{"Test level too small", args{levelStr: "1", uuid: "aaa123", msgType: announcePresence}, broadcastMessageContent{}, true},
		{"Test level too big", args{levelStr: "9", uuid: "aaa123", msgType: announcePresence}, broadcastMessageContent{}, true},
		{"Test valid", args{levelStr: "6", uuid: "aaa234", msgType: startLvlUp}, broadcastMessageContent{uuid: "aaa234", msgType: startLvlUp, targetLevel: 6}, false},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := parseMessageLevelAndReturn(tt.args.levelStr, tt.args.uuid, tt.args.msgType)
			if (err != nil) != tt.wantErr {
				t.Errorf("parseMessageLevelAndReturn() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("parseMessageLevelAndReturn() got = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_parsePlayerMessage(t *testing.T) {
	type args struct {
		message string
	}
	tests := []struct {
		name    string
		args    args
		want    broadcastMessageContent
		wantErr bool
	}{
		{"Test invalid message", args{message: "Hello World!"}, broadcastMessageContent{}, true},
		{"Test no UUID", args{message: "Heya!"}, broadcastMessageContent{}, true},
		{"Test Cancel", args{message: "aaa123 Cancel 2"}, broadcastMessageContent{uuid: "aaa123", msgType: cancelLvlUp, targetLevel: 2}, false},
		{"Test Join", args{message: "aaa123 Join 3"}, broadcastMessageContent{uuid: "aaa123", msgType: announcePresence, targetLevel: 3}, false},
		{"Test Leave", args{message: "aaa123 Leave 4"}, broadcastMessageContent{uuid: "aaa123", msgType: announceDeparture, targetLevel: 4}, false},
		{"Test Starting", args{message: "aaa123 Starting 5"}, broadcastMessageContent{uuid: "aaa123", msgType: startLvlUp, targetLevel: 5}, false},
		{"Test Reached", args{message: "aaa123 Reached 6"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpComplete, targetLevel: 6}, false},
		{"Test Failed", args{message: "aaa123 Failed 7"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpFailed, targetLevel: 7}, false},
		{"Test Ready", args{message: "aaa123 Ready 8, missing 6"}, broadcastMessageContent{uuid: "aaa123", msgType: missingPlayers, targetLevel: 8, missingPlayers: 6}, false},
		{"Test Ready invalid level", args{message: "aaa123 Ready rrr, missing 6"}, broadcastMessageContent{}, true},
		{"Test Ready invalid missing players", args{message: "aaa123 Ready 8, missing mm"}, broadcastMessageContent{}, true},
		{"Test Ready invalid no missing part", args{message: "aaa123 Ready 8"}, broadcastMessageContent{}, true},
		{"Test Ready invalid bad missing part", args{message: "aaa123 Ready 8, mssing 6"}, broadcastMessageContent{}, true},
		{"Test Ready missing players too high", args{message: "aaa123 Ready 8, missing 7"}, broadcastMessageContent{}, true},
		{"Test Ready missing players too low", args{message: "aaa123 Ready 8, missing 0"}, broadcastMessageContent{}, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := parsePlayerMessage(tt.args.message)
			if (err != nil) != tt.wantErr {
				t.Errorf("parsePlayerMessage() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("parsePlayerMessage() got = %v, want %v", got, tt.want)
			}
		})
	}
}
