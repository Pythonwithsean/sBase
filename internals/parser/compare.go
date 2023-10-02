package parser

import (
	"fmt"
	"strings"

	TDBTypes "github.com/tobshub/tobsdb/internals/types"
)

func CompareVector(schema *Table, field *Field, value []any, input any) bool {
	input, err := field.ValidateType(schema, input, false)
	if err != nil {
		return false
	}

	v_type, v_level := ParseVectorProp(field.Properties[TDBTypes.FieldPropVector])

	var v_field Field

	if v_level > 1 {
		v_field = Field{
			Name:        "vector field",
			BuiltinType: TDBTypes.FieldTypeVector,
			Properties:  map[TDBTypes.FieldProp]string{},
		}

		v_field.Properties[TDBTypes.FieldPropVector] = fmt.Sprintf("%s,%d", v_type, v_level-1)
	} else {
		v_field = Field{Name: "vector field", BuiltinType: v_type}
	}

	for i, v_value := range value {
		if !v_field.Compare(schema, v_value, input.([]any)[i]) {
			return false
		}
	}

	return true
}

type IntCompare string

const (
	IntCompareEqual          IntCompare = "eq"
	IntCompareNotEqual       IntCompare = "ne"
	IntCompareGreater        IntCompare = "gt"
	IntCompareLess           IntCompare = "lt"
	IntCompareGreaterOrEqual IntCompare = "gte"
	IntCompareLessOrEqual    IntCompare = "lte"
)

func CompareInt(schema *Table, field *Field, value int, input any) bool {
	switch input := input.(type) {
	case map[string]any:
		valid := false
		for comp, val := range input {
			comp := IntCompare(comp)
			_val, err := field.ValidateType(schema, val, false)
			if err != nil {
				return false
			}
			val := _val.(int)
			switch comp {
			case IntCompareEqual:
				valid = value == val
			case IntCompareNotEqual:
				valid = value != val
			case IntCompareGreater:
				valid = value > val
			case IntCompareLess:
				valid = value < val
			case IntCompareGreaterOrEqual:
				valid = value >= val
			case IntCompareLessOrEqual:
				valid = value <= val
			}

			if !valid {
				break
			}
		}
		return valid
	default:
		input, err := field.ValidateType(schema, input, false)
		if err != nil {
			return false
		}

		return value == input
	}
}

type StringCompare string

const (
	StringCompareContains   StringCompare = "contains"
	StringCompareStartsWith StringCompare = "startsWith"
	StringCompareEndsWith   StringCompare = "endsWith"
)

func CompareString(schema *Table, field *Field, value string, input any) bool {
	switch input := input.(type) {
	case map[string]any:

		valid := false
		for comp, val := range input {
			comp := StringCompare(comp)
			_val, err := field.ValidateType(schema, val, false)
			if err != nil {
				return false
			}
			val := _val.(string)
			switch comp {
			case StringCompareContains:
				valid = strings.Contains(value, val)
			case StringCompareStartsWith:
				valid = strings.Index(value, val) == 0
			case StringCompareEndsWith:
				valid = strings.LastIndex(value, val) == (len(value) - len(val))
			}

			if !valid {
				break
			}
		}
		return valid
	default:
		input, err := field.ValidateType(schema, input, false)
		if err != nil {
			return false
		}

		return value == input
	}
}