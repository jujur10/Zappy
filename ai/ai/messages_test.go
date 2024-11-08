package ai

import (
	"crypto/aes"
	"crypto/cipher"
	"fmt"
	"log"
	"reflect"
	"testing"
)

func Test_interpretMessage(t *testing.T) {
	type args struct {
		message string
	}
	CipherMessages = false
	aesCipher, err := aes.NewCipher([]byte(growAesKey("aaa")))
	if err != nil {
		fmt.Println("Failed to create AES cipher, defaulting to no cipher")
		CipherMessages = false
		log.Fatal("Failed to create AES cipher", err)
	}
	gcm, err := cipher.NewGCM(aesCipher)
	if err != nil {
		fmt.Println("Failed to create GCM", err)
		CipherMessages = false
		log.Fatal("Failed to create GCM", err)
	}
	tests := []struct {
		name    string
		args    args
		want    broadcastMessageContent
		wantErr bool
	}{
		{"Invalid message 1", args{message: "aaa123_Hello_6"}, broadcastMessageContent{}, true},
		{"Invalid message 2", args{message: "aaa123"}, broadcastMessageContent{}, true},
		{"Valid Ready", args{message: "aaa123_Ready_5,_missing_3"}, broadcastMessageContent{uuid: "aaa123", msgType: missingPlayers, targetLevel: 5, missingPlayers: 3}, false},
		{"Invalid Ready level 1", args{message: "aaa123_Ready_0,_missing_3"}, broadcastMessageContent{}, true},
		{"Invalid Ready level 2", args{message: "aaa123_Ready_9,_missing_3"}, broadcastMessageContent{}, true},
		{"Invalid Ready level 3", args{message: "aaa123_Ready_a,_missing_3"}, broadcastMessageContent{}, true},
		{"Invalid Ready missing 1", args{message: "aaa123_Ready_5,_missing_0"}, broadcastMessageContent{}, true},
		{"Invalid Ready missing 2", args{message: "aaa123_Ready_5,_missing_5"}, broadcastMessageContent{}, true},
		{"Invalid Ready missing 3", args{message: "aaa123_Ready_5,_missing_b"}, broadcastMessageContent{}, true},
		{"Valid Cancel", args{message: "aaa123_Cancel_4"}, broadcastMessageContent{uuid: "aaa123", msgType: cancelLvlUp, targetLevel: 4}, false},
		{"Invalid Cancel level 1", args{message: "aaa123_Cancel_0"}, broadcastMessageContent{}, true},
		{"Invalid Cancel level 2", args{message: "aaa123_Cancel_9"}, broadcastMessageContent{}, true},
		{"Invalid Cancel level 3", args{message: "aaa123_Cancel_c"}, broadcastMessageContent{}, true},
		{"Valid Join", args{message: "aaa123_Join_4"}, broadcastMessageContent{uuid: "aaa123", msgType: announcePresence, targetLevel: 4}, false},
		{"Invalid Join level 1", args{message: "aaa123_Join_0"}, broadcastMessageContent{}, true},
		{"Invalid Join level 2", args{message: "aaa123_Join_9"}, broadcastMessageContent{}, true},
		{"Invalid Join level 3", args{message: "aaa123_Join_j"}, broadcastMessageContent{}, true},
		{"Valid Starting", args{message: "aaa123_Starting_4"}, broadcastMessageContent{uuid: "aaa123", msgType: startLvlUp, targetLevel: 4}, false},
		{"Invalid Starting level 1", args{message: "aaa123_Starting_0"}, broadcastMessageContent{}, true},
		{"Invalid Starting level 2", args{message: "aaa123_Starting_9"}, broadcastMessageContent{}, true},
		{"Invalid Starting level 3", args{message: "aaa123_Starting_s"}, broadcastMessageContent{}, true},
		{"Valid Reached", args{message: "aaa123_Reached_4"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpComplete, targetLevel: 4}, false},
		{"Invalid Reached level 1", args{message: "aaa123_Reached_0"}, broadcastMessageContent{}, true},
		{"Invalid Reached level 2", args{message: "aaa123_Reached_9"}, broadcastMessageContent{}, true},
		{"Invalid Reached level 3", args{message: "aaa123_Reached_r"}, broadcastMessageContent{}, true},
		{"Valid Failed", args{message: "aaa123_Failed_4"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpFailed, targetLevel: 4}, false},
		{"Invalid Failed level 1", args{message: "aaa123_Failed_0"}, broadcastMessageContent{}, true},
		{"Invalid Failed level 2", args{message: "aaa123_Failed_9"}, broadcastMessageContent{}, true},
		{"Invalid Failed level 3", args{message: "aaa123_Failed_f"}, broadcastMessageContent{}, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := parsePlayerMessage(tt.args.message, gcm)
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
	aesCipher, err := aes.NewCipher([]byte(growAesKey("aaa")))
	if err != nil {
		fmt.Println("Failed to create AES cipher, defaulting to no cipher")
		CipherMessages = false
		log.Fatal("Failed to create AES cipher", err)
	}
	gcm, err := cipher.NewGCM(aesCipher)
	if err != nil {
		fmt.Println("Failed to create GCM", err)
		CipherMessages = false
		log.Fatal("Failed to create GCM", err)
	}
	tests := []struct {
		name    string
		args    args
		want    broadcastMessageContent
		wantErr bool
	}{
		{"Test invalid message 1", args{message: "Hello World!"}, broadcastMessageContent{}, true},
		{"Test invalid message 2", args{message: "Hello_World!"}, broadcastMessageContent{}, true},
		{"Test no UUID", args{message: "Heya!"}, broadcastMessageContent{}, true},
		{"Test Cancel", args{message: "aaa123_Cancel_2"}, broadcastMessageContent{uuid: "aaa123", msgType: cancelLvlUp, targetLevel: 2}, false},
		{"Test Join", args{message: "aaa123_Join_3"}, broadcastMessageContent{uuid: "aaa123", msgType: announcePresence, targetLevel: 3}, false},
		{"Test Leave", args{message: "aaa123_Leave_4"}, broadcastMessageContent{uuid: "aaa123", msgType: announceDeparture, targetLevel: 4}, false},
		{"Test Starting", args{message: "aaa123_Starting_5"}, broadcastMessageContent{uuid: "aaa123", msgType: startLvlUp, targetLevel: 5}, false},
		{"Test Reached", args{message: "aaa123_Reached_6"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpComplete, targetLevel: 6}, false},
		{"Test Failed", args{message: "aaa123_Failed_7"}, broadcastMessageContent{uuid: "aaa123", msgType: lvlUpFailed, targetLevel: 7}, false},
		{"Test Ready", args{message: "aaa123_Ready_8,_missing_6"}, broadcastMessageContent{uuid: "aaa123", msgType: missingPlayers, targetLevel: 8, missingPlayers: 6}, false},
		{"Test Ready invalid level", args{message: "aaa123_Ready_rrr,_missing_6"}, broadcastMessageContent{}, true},
		{"Test Ready invalid missing players", args{message: "aaa123_Ready_8,_missing_mm"}, broadcastMessageContent{}, true},
		{"Test Ready invalid no missing part", args{message: "aaa123_Ready_8"}, broadcastMessageContent{}, true},
		{"Test Ready invalid bad missing part", args{message: "aaa123_Ready_8,_mssing_6"}, broadcastMessageContent{}, true},
		{"Test Ready missing players too high", args{message: "aaa123_Ready_8,_missing_7"}, broadcastMessageContent{}, true},
		{"Test Ready missing players too low", args{message: "aaa123_Ready_8,_missing_0"}, broadcastMessageContent{}, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := parsePlayerMessage(tt.args.message, gcm)
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
