package ai

import (
	"reflect"
	"testing"
)

func TestCreateViewMap(t *testing.T) {
	type args struct {
		parsedList [][]string
	}
	tests := []struct {
		name    string
		args    args
		want    ViewMap
		wantErr bool
	}{
		{"Valid view map", args{[][]string{{"player"}, {"linemate", "food"}, {}, {"sibur"}, {"thystame"}, {}, {}, {"deraumere"}, {"mendiane"}}}, ViewMap{{Player}, {Linemate, Food}, {}, {Sibur}, {Thystame}, {}, {}, {Deraumere}, {Mendiane}}, false},
		{"Invalid argument", args{[][]string{{"player"}, {"linemate"}, {"phiras"}, {"sibure"}, {}, {}, {}, {"deraumere"}, {"mendiane"}}}, nil, true},
		{"Invalid number of tiles", args{[][]string{{"player"}, {"linemate"}, {}, {"sibur"}, {}, {}, {"deraumere"}, {"mendiane"}}}, nil, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := CreateViewMap(tt.args.parsedList)
			if (err != nil) != tt.wantErr {
				t.Errorf("CreateViewMap() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("CreateViewMap() got = %v, want %v", got, tt.want)
			}
		})
	}
}
