package parser

import (
	"fmt"
	"strconv"
	"time"

	"github.com/tobshub/tobsdb/internal/types"
)

func (table *Table) Compare(field *Field, value any, input any) bool {
	value, err := table.ValidateType(field, value, false)
	if err != nil {
		return false
	}

	switch field.BuiltinType {
	case types.FieldTypeVector:
		return table.compareVector(field, value.([]any), input)
	case types.FieldTypeInt:
		return table.compareInt(field, value.(int), input)
	case types.FieldTypeString:
		return table.compareString(field, value.(string), input)
	default:
		input, err := table.ValidateType(field, input, false)
		if err != nil {
			return false
		}
		return value == input
	}
}

func validateTypeInt(table *Table, field *Field, input any, allow_default bool) (any, error) {
	switch input := input.(type) {
	case int:
		return input, nil
	case float64:
		return int(input), nil
	case nil:
		if field.Name == "id" {
			return table.CreateId(), nil
		}

		if default_val, ok := field.Properties[types.FieldPropDefault]; ok && allow_default {
			if default_val == "auto" {
				return int(time.Now().UnixMicro()), nil
			}
			if default_val == "autoincrement" {
				return field.AutoIncrement(), nil
			}
			str_int, err := strconv.ParseInt(default_val, 10, 0)
			if err != nil {
				return nil, err
			}
			return int(str_int), nil

		}

		if field.Properties[types.FieldPropOptional] == "true" {
			return nil, nil
		}
	}
	return nil, invalidFieldTypeError(input, field.Name)
}

func validateTypeFloat(field *Field, input any, allow_default bool) (any, error) {
	switch input := input.(type) {
	case float64:
		return input, nil
	case int:
		return float64(input), nil
	case nil:
		if default_val, ok := field.Properties[types.FieldPropDefault]; ok && allow_default {
			str_float, err := strconv.ParseFloat(default_val, 64)
			if err != nil {
				return nil, err
			}
			return str_float, nil
		}

		if field.Properties[types.FieldPropOptional] == "true" {
			return nil, nil
		}
	}
	return nil, invalidFieldTypeError(input, field.Name)
}

func validateTypeString(field *Field, input any, allow_default bool) (any, error) {
	switch input := input.(type) {
	case string:
		return input, nil
	case nil:
		if default_val, ok := field.Properties[types.FieldPropDefault]; ok && allow_default {
			// we assume the user's text starts and ends with " or '
			default_val = default_val[1 : len(default_val)-1]
			return default_val, nil
		}

		if field.Properties[types.FieldPropOptional] == "true" {
			return nil, nil
		}
	}
	return nil, invalidFieldTypeError(input, field.Name)
}

func validateTypeDate(field *Field, input any, allow_default bool) (any, error) {
	switch input := input.(type) {
	case time.Time:
		return input, nil
	case string:
		val, err := time.Parse(time.RFC3339, input)
		if err != nil {
			return nil, err
		}
		return val, nil
	case float64:
		val := time.UnixMilli(int64(input))
		return val, nil
	case int:
		val := time.UnixMilli(int64(input))
		return val, nil
	case nil:
		if default_val, ok := field.Properties[types.FieldPropDefault]; ok && allow_default {
			if default_val == "now" {
				time_string, _ := time.Now().MarshalText()
				t, _ := time.Parse(time.RFC3339, string(time_string))
				return t, nil
			}
		} else if field.Properties[types.FieldPropOptional] == "true" {
			return nil, nil
		}
	}
	return nil, invalidFieldTypeError(input, field.Name)
}

func validateTypeBool(field *Field, input any, allow_default bool) (any, error) {
	switch input := input.(type) {
	case bool:
		return input, nil
	case string:
		val, err := strconv.ParseBool(input)
		if err != nil {
			return nil, invalidFieldTypeError(input, field.Name)
		}
		return val, nil
	case nil:
		if default_val, ok := field.Properties[types.FieldPropDefault]; ok && allow_default {
			if default_val == "true" {
				return true, nil
			}
			return false, nil
		} else if field.Properties[types.FieldPropOptional] == "true" {
			return nil, nil
		}
	}
	return nil, invalidFieldTypeError(input, field.Name)
}

func validateTypeVector(table *Table, field *Field, input any, allow_default bool) (any, error) {
	v_type, v_level := ParseVectorProp(field.Properties[types.FieldPropVector])
	err := validateFieldType(v_type)
	if err != nil {
		return nil, err
	}

	var v_field Field

	if v_level > 1 {
		v_field = Field{
			Name:        fmt.Sprintf("vector_value.%d", v_level-1),
			BuiltinType: types.FieldTypeVector,
			Properties:  map[types.FieldProp]string{},
		}

		v_field.Properties[types.FieldPropVector] = fmt.Sprintf("%s,%d", v_type, v_level-1)
	} else {
		v_field = Field{Name: "vector_value.0", BuiltinType: v_type}
	}

	switch input := input.(type) {
	case []interface{}:
		for i := 0; i < len(input); i++ {
			val, err := table.ValidateType(&v_field, input[i], false)
			if err != nil {
				return nil, err
			}
			input[i] = val
		}

		return input, nil
	case nil:
		if field.Properties[types.FieldPropOptional] == "true" {
			return nil, nil
		}
	}
	return nil, invalidFieldTypeError(input, field.Name)
}

func validateTypeBytes(table *Table, field *Field, input any, allow_default bool) (any, error) {
	switch input := input.(type) {
	case []byte:
		return input, nil
	case string:
		return []byte(input), nil
	case nil:
		if field.Properties[types.FieldPropOptional] == "true" {
			return nil, nil
		}
	}

	return nil, invalidFieldTypeError(input, field.Name)
}

func (table *Table) ValidateType(field *Field, input any, allow_default bool) (any, error) {
	switch field.BuiltinType {
	case types.FieldTypeInt:
		return validateTypeInt(table, field, input, allow_default)
	case types.FieldTypeFloat:
		return validateTypeFloat(field, input, allow_default)
	case types.FieldTypeString:
		return validateTypeString(field, input, allow_default)
	case types.FieldTypeDate:
		return validateTypeDate(field, input, allow_default)
	case types.FieldTypeBool:
		return validateTypeBool(field, input, allow_default)
	case types.FieldTypeVector:
		return validateTypeVector(table, field, input, allow_default)
	case types.FieldTypeBytes:
		return validateTypeBytes(table, field, input, allow_default)
	}

	return nil, unsupportedFieldTypeError(string(field.BuiltinType), field.Name)
}

func invalidFieldTypeError(input any, field_name string) error {
	return fmt.Errorf("Invalid field type for %s: %T", field_name, input)
}

// if schema validation is working properly this error should never occur
func unsupportedFieldTypeError(invalid_type, field_name string) error {
	return fmt.Errorf("Unsupported field type for %s: %s", field_name, invalid_type)
}

func (table *Table) CreateId() int {
	table.IdTracker++
	return table.IdTracker
}

func (field *Field) AutoIncrement() int {
	if field.BuiltinType == types.FieldTypeInt {
		field.IncrementTracker++
		return field.IncrementTracker
	}

	return 0
}

type IndexLevel int

const (
	IndexLevelNone IndexLevel = iota
	IndexLevelUnique
	IndexLevelPrimary
)

func (field *Field) IndexLevel() IndexLevel {
	key_prop, has_key_prop := field.Properties[types.FieldPropKey]
	if has_key_prop && key_prop == "primary" {
		return IndexLevelPrimary
	}

	unique_prop, has_unique_prop := field.Properties[types.FieldPropUnique]
	if has_unique_prop && unique_prop == "true" {
		return IndexLevelUnique
	}

	return IndexLevelNone
}
