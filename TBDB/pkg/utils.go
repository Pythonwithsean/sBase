package pkg

func Filter[T any](items []T, predicate func(T) bool) []T {
	filtered := []T{}
	for _, item := range items {
		if predicate(item) {
			filtered = append(filtered, item)
		}
	}
	return filtered
}

// Converts a value suspected to be either an int or float64 to an int.
// This kind of logic is used all over the code(due to json decoding all numbers as float64) so it's kinda needed
func NumToInt(num any) int {
	switch num := num.(type) {
	case int:
		return num
	case float64:
		return int(num)
	}
	return 0
}

// Takes a map base and a map overwrite and returns a new map
// with the similar key-values from both with overwrite taking precedence
func MergeMaps[V any](base, overwrite map[string]V) map[string]V {
	result := make(map[string]V)
	for k, v := range base {
		if _, ok := overwrite[k]; ok {
			result[k] = overwrite[k]
		} else {
			result[k] = v
		}
	}
	return result
}
