package parser

import (
	"fmt"
	"strings"

	"github.com/tobshub/tobsdb/internal/types"
)

func (table *Table) compareVector(field *Field, value []any, input any) bool {
	input, err := table.ValidateType(field, input, false)
	if err != nil {
		return false
	}

	v_type, v_level := ParseVectorProp(field.Properties[types.FieldPropVector])

	var v_field Field

	if v_level > 1 {
		v_field = Field{
			Name:        "vector field",
			BuiltinType: types.FieldTypeVector,
			Properties:  map[types.FieldProp]string{},
		}

		v_field.Properties[types.FieldPropVector] = fmt.Sprintf("%s,%d", v_type, v_level-1)
	} else {
		v_field = Field{Name: "vector field", BuiltinType: v_type}
	}

	for i, v_value := range value {
		if !table.Compare(&v_field, v_value, input.([]any)[i]) {
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

func (table *Table) compareInt(field *Field, value int, input any) bool {
	switch input := input.(type) {
	case map[string]any:
		valid := false
		for comp, val := range input {
			comp := IntCompare(comp)
			_val, err := table.ValidateType(field, val, false)
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
		input, err := table.ValidateType(field, input, false)
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

func (table *Table) compareString(field *Field, value string, input any) bool {
	switch input := input.(type) {
	case map[string]any:

		valid := false
		for comp, val := range input {
			comp := StringCompare(comp)
			_val, err := table.ValidateType(field, val, false)
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
		input, err := table.ValidateType(field, input, false)
		if err != nil {
			return false
		}

		return value == input
	}
}
