package ai

import (
	"reflect"
	"testing"
)

func Test_createInventory(t *testing.T) {
	type args struct {
		parsedInv map[string]int
	}
	tests := []struct {
		name    string
		args    args
		want    Inventory
		wantErr bool
	}{
		{"Test empty inv", args{parsedInv: map[string]int{}}, map[TileItem]int{}, false},
		{"Basic test", args{parsedInv: map[string]int{"food": 5, "linemate": 2, "deraumere": 3, "sibur": 1, "mendiane": 2, "phiras": 1, "thystame": 0}},
			map[TileItem]int{Food: 5, Linemate: 2, Deraumere: 3, Sibur: 1, Mendiane: 2, Phiras: 1, Thystame: 0}, false},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := createInventory(tt.args.parsedInv)
			if (err != nil) != tt.wantErr {
				t.Errorf("createInventory() error = %v, wantErr %v", err, tt.wantErr)
				return
			}
			if !reflect.DeepEqual(got, tt.want) {
				t.Errorf("createInventory() got = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_growAesKey(t *testing.T) {
	type args struct {
		key string
	}
	tests := []struct {
		name string
		args args
		want string
	}{
		{"Less than 32B key", args{"smallkey"}, "smallkey------------------------"},
		{"32B key", args{"thisisakey32bytesofalengthof32Ba"}, "thisisakey32bytesofalengthof32Ba"},
		{"More than 32B key", args{"thisisakey32bytesofalengthof32Bazertyaa"}, "thisisakey32bytesofalengthof32Ba"},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := growAesKey(tt.args.key); got != tt.want {
				t.Errorf("growAesKey() = %v, want %v", got, tt.want)
			}
		})
	}
}
