package parser

import (
	"fmt"
	"strconv"
	"time"

	"github.com/tobshub/tobsdb/cmd/types"
)

func (field *Field) ValidateType(table_name string, input any, allow_default bool) (any, error) {
	data_type := fmt.Sprintf("%T", input)
	switch field.builtin_type {
	case types.Int:
		{
			switch data_type {
			case "float64":
				return int(input.(float64)), nil
			case "<nil>":
				if default_val, ok := field.properties[types.Default]; ok && allow_default {
					if default_val == "auto" {
						return CreateId(), nil
					} else {
						str_int, err := strconv.ParseInt(default_val, 10, 0)
						if err != nil {
							return nil, err
						}
						return str_int, nil
					}
				} else if field.name == "id" {
					return CreateId(), nil
				} else if field.properties[types.Optional] == "true" {
					return nil, nil
				} else {
					return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
				}
			default:
				return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
			}
		}
	case types.Float:
		{
			switch data_type {
			case "float64":
				return input.(float64), nil
			case "<nil>":
				if default_val, ok := field.properties[types.Default]; ok && allow_default {
					str_float, err := strconv.ParseFloat(default_val, 64)
					if err != nil {
						return nil, err
					}
					return str_float, nil
				} else if field.properties[types.Optional] == "true" {
					return nil, nil
				} else {
					return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
				}
			default:
				return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
			}
		}
	case types.String:
		{
			switch data_type {
			case "string":
				return input.(string), nil
			case "<nil>":
				if default_val, ok := field.properties[types.Default]; ok && allow_default {
					return default_val, nil
				} else if field.properties[types.Optional] == "true" {
					return nil, nil
				} else {
					return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
				}
			default:
				return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
			}
		}
	case types.Date:
		{
			switch data_type {
			case "string":
				val, err := time.Parse(time.RFC3339, input.(string))
				if err != nil {
					return nil, err
				}
				return val, nil
			case "float64":
				val := time.UnixMilli(int64(input.(float64)))
				return val, nil
			case "<nil>":
				if default_val, ok := field.properties[types.Default]; ok && allow_default {
					if default_val == "now" {
						return time.Now(), nil
					}
				} else if field.properties[types.Optional] == "true" {
					return nil, nil
				} else {
					return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
				}
			default:
				return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
			}
		}
	case types.Bool:
		{
			switch data_type {
			case "bool":
				return input.(bool), nil
			case "<nil>":
				if default_val, ok := field.properties[types.Default]; ok && allow_default {
					if default_val == "true" {
						return true, nil
					} else {
						return false, nil
					}
				} else if field.properties[types.Optional] == "true" {
					return nil, nil
				} else {
					return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
				}
			default:
				return nil, InvalidFieldTypeError(data_type, table_name, field.name, string(field.builtin_type))
			}
		}
	default:
		return nil, UnsupportedFieldTypeError(string(field.builtin_type), table_name, field.name)
	}
	return nil, UnsupportedFieldTypeError(string(field.builtin_type), table_name, field.name)
}

func InvalidFieldTypeError(invalid_type, table_name, field_name, field_type string) error {
	return fmt.Errorf("Invalid field type %s: %s.%s should be type %s", invalid_type, table_name, field_name, field_type)
}

// if schema validation is working properly this error should never occur
func UnsupportedFieldTypeError(invalid_type, table_name, field_name string) error {
	return fmt.Errorf("Unsupported field type %s: %s.%s in schema", invalid_type, table_name, field_name)
}