package ai

import "testing"

func Test_computeFoodPriority(t *testing.T) {
	type args struct {
		lifeTime int
	}
	tests := []struct {
		name string
		args args
		want int
	}{
		{"1 cycle left", args{lifeTime: 1}, foodMaxPriority},
		{"2 food units left", args{lifeTime: 2 * foodLifeTimeIncrement}, foodMaxPriority},
		{"5 food units left", args{lifeTime: 5 * foodLifeTimeIncrement}, 8},
		{"10 food units left", args{lifeTime: 10 * foodLifeTimeIncrement}, 3},
		{"15 food units left", args{lifeTime: 15 * foodLifeTimeIncrement}, foodMinPriority},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := computeFoodPriority(tt.args.lifeTime); got != tt.want {
				t.Errorf("computeFoodPriority() = %v, want %v", got, tt.want)
			}
		})
	}
}
