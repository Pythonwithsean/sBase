package pkg_test

import (
	"testing"

	. "github.com/tobshub/tobsdb/pkg"
)

func TestFilter(t *testing.T) {
	res := Filter([]int{1, 2, 3, 4, 5, 6}, func(i int) bool {
		return i%2 == 0
	})

	if len(res) != 3 {
		t.Errorf("Expected 3, got %d", len(res))
	}

	if res[0] != 2 || res[1] != 4 || res[2] != 6 {
		t.Errorf("Expected 2, 4, 6, got %d, %d, %d", res[0], res[1], res[2])
	}
}

func TestNumToInt(t *testing.T) {
	if NumToInt(1) != 1 {
		t.Errorf("Expected 1, got %d", NumToInt(1))
	}

	if NumToInt(1.1) != 1 {
		t.Errorf("Expected 1, got %d", NumToInt(1))
	}
}
