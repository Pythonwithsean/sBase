package parser

import (
	"fmt"
	"time"

	"github.com/tobshub/tobsdb/cmd/types"
)

func (table *Table) Create(d map[string]any) (map[string]any, error) {
	row := make(map[string]any)

	for _, field := range table.fields {
		data := d[field.name]
		data_type := fmt.Sprintf("%T", data)
		switch field.builtin_type {
		case types.Int:
			if data_type == "float64" {
				row[field.name] = int(data.(float64))
			} else if data_type == "<nil>" && field.name == "id" {
				row[field.name] = CreateId()
			} else if data_type == "<nil>" && field.properties[types.Optional] == "true" {
				row[field.name] = nil
			} else {
				return row, InvalidFieldTypeError(data_type, table.Name, field.name, string(field.builtin_type))
			}
		case types.String:
			if data_type == "string" {
				row[field.name] = data.(string)
			} else if data_type == "<nil>" && field.properties[types.Optional] == "true" {
				row[field.name] = nil
			} else {
				return row, InvalidFieldTypeError(data_type, table.Name, field.name, string(field.builtin_type))
			}
		case types.Date:
			if data_type == "string" || data_type == "float64" {
				switch data_type {
				case "string":
					val, err := time.Parse(time.RFC3339, data.(string))
					if err != nil {
						return row, err
					}
					row[field.name] = val
				case "float64":
					val := time.UnixMilli(int64(data.(float64)))
					row[field.name] = val
				}
			} else if data_type == "<nil>" && field.properties[types.Optional] == "true" {
				row[field.name] = nil
			} else {
				return row, InvalidFieldTypeError(data_type, table.Name, field.name, string(field.builtin_type))
			}
		}
	}

	return row, nil
}

func CreateId() int {
	return 1
}

func InvalidFieldTypeError(invalid_type, table_name, field_name, field_type string) error {
	return fmt.Errorf("Invalid field type %s: %s.%s should be type %s", invalid_type, table_name, field_name, field_type)
}