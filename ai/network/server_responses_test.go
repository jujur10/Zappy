package network

import (
	"reflect"
	"testing"
)

func TestIsInArray(t *testing.T) {
	type args[T comparable] struct {
		value T
		array []T
	}
	type testCase[T comparable] struct {
		name string
		args args[T]
		want bool
	}
	testsStr := []testCase[string]{
		{"Simple test 1 String", args[string]{"AA", []string{"AA", "BB"}}, true},
		{"Simple test 2 String", args[string]{"player", []string{"hey !", "player", "listen"}}, true},
		{"Not in array String", args[string]{"Link", []string{"hey !", "player", "listen"}}, false},
	}

	testsInt := []testCase[int]{
		{"Simple test 1 Int", args[int]{2, []int{5, 2, 1}}, true},
		{"Simple test 2 Int", args[int]{-20252, []int{88, 2551, 345, -20252, 8555555, 6858}}, true},
		{"Not in array Int", args[int]{-5, []int{0, 42, 69}}, false},
	}
	for _, tt := range testsStr {
		t.Run(tt.name, func(t *testing.T) {
			if got := IsInArray(tt.args.value, tt.args.array); got != tt.want {
				t.Errorf("IsInArray() = %v, want %v", got, tt.want)
			}
		})
	}
	for _, tt := range testsInt {
		t.Run(tt.name, func(t *testing.T) {
			if got := IsInArray(tt.args.value, tt.args.array); got != tt.want {
				t.Errorf("IsInArray() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_checkValues(t *testing.T) {
	type args struct {
		values [][]string
	}
	tests := []struct {
		name string
		args args
		want bool
	}{
		{"Valid view", args{[][]string{{"player", "thystame"}, {}, {"food"}, {"linemate"}}}, true},
		{"No player in current position", args{[][]string{{"thystame"}, {}, {"food"}, {"linemate"}}}, false},
		{"Unknown element", args{[][]string{{"player", "thystame"}, {}, {"foode"}, {"linemate"}}}, false},
		{"Invalid number of tiles", args{[][]string{{"player"}, {"food"}, {"deraumere"}}}, false},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := checkValues(tt.args.values); got != tt.want {
				t.Errorf("checkValues() = %v, want %v", got, tt.want)
			}
		})
	}
}

func TestParseArray(t *testing.T) {
	type args struct {
		line string
	}
	tests := []struct {
		name    string
		args    args
		want    MessageType
		want1   any
		wantErr bool
	}{
		{"Valid inventory", args{"[food 5, linemate 2,deraumere 1, sibur 8, mendiane 0,phiras 1, thystame 0]"}, Inventory, map[string]int{"food": 5, "linemate": 2, "deraumere": 1, "sibur": 8, "mendiane": 0, "phiras": 1, "thystame": 0}, false},
		{"Invalid inventory no value", args{"[food 5, linemate 2,deraumere 1, sibur 8, mendiane,phiras 1, thystame 0]"}, Nil, nil, true},
		{"Invalid inventory bad name", args{"[food 5, linemate 2,deraumere 1, sibur 8, mendiant 0,phiras 1, thystame 0]"}, Nil, nil, true},
		{"Invalid inventory missing entry", args{"[food 5, linemate 2,deraumere 1, sibur 8, mendiane 0,phiras 1]"}, Nil, nil, true},
		{"Valid view", args{"[food player, linemate food,, sibur,,,,phiras,]"}, View, [][]string{{"food", "player"}, {"linemate", "food"}, {}, {"sibur"}, {}, {}, {}, {"phiras"}, {}}, false},
		{"Invalid view no player", args{"[food, linemate food,, sibur,,,,phiras,]"}, Nil, nil, true},
		{"Invalid view invalid nb tiles", args{"[food player, linemate food, sibur,,,,phiras,]"}, Nil, nil, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, got1, err := ParseArray(tt.args.line)
			if (err != nil) != tt.wantErr {
				t.Errorf("ParseArray() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if got != tt.want {
				t.Errorf("ParseArray() got = %v, want %v", got, tt.want)
			}
			if !reflect.DeepEqual(got1, tt.want1) {
				t.Errorf("ParseArray() got1 = %v, want %v", got1, tt.want1)
			}
		})
	}
}

func Test_parseInventory(t *testing.T) {
	type args struct {
		values [][]string
	}
	tests := []struct {
		name    string
		args    args
		want    any
		wantErr bool
	}{
		{"Valid inventory", args{[][]string{{"food", "5"}, {"linemate", "2"}, {"deraumere", "1"}, {"sibur", "8"}, {"mendiane", "0"}, {"phiras", "1"}, {"thystame", "0"}}}, map[string]int{"food": 5, "linemate": 2, "deraumere": 1, "sibur": 8, "mendiane": 0, "phiras": 1, "thystame": 0}, false},
		{"Invalid inventory no value", args{[][]string{{"food", "5"}, {"linemate", "2"}, {"deraumere", "1"}, {"sibur", "8"}, {"mendiane", "0"}, {"phiras"}, {"thystame", "0"}}}, nil, true},
		{"Invalid inventory bad name", args{[][]string{{"food", "5"}, {"linemate", "2"}, {"deraumer", "1"}, {"sibur", "8"}, {"mendiane", "0"}, {"phiras", "1"}, {"thystame", "0"}}}, nil, true},
		{"Invalid inventory missing entry", args{[][]string{{"food", "5"}, {"deraumere", "1"}, {"sibur", "8"}, {"mendiane", "0"}, {"phiras", "1"}, {"thystame", "0"}}}, nil, true},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := parseInventory(tt.args.values)
			if (err != nil) != tt.wantErr {
				t.Errorf("parseInventory() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("parseInventory() got = %v, want %v", got, tt.want)
			}
		})
	}
}
