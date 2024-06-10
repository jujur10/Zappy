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
			got, err := interpretMessage(tt.args.message)
			if (err != nil) != tt.wantErr {
				t.Errorf("interpretMessage() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("interpretMessage() got = %v, want %v", got, tt.want)
			}
		})
	}
}
