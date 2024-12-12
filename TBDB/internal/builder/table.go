package builder

import (
	"fmt"
	"net/http"

	"github.com/tobshub/tobsdb/internal/parser"
	"github.com/tobshub/tobsdb/internal/types"
	"github.com/tobshub/tobsdb/pkg"
)

func (schema *Schema) Create(t_schema *parser.Table, data map[string]any) (map[string]any, error) {
	row := make(map[string]any)
	for _, field := range t_schema.Fields {
		input := data[field.Name]
		res, err := t_schema.ValidateType(field, input, true)
		if err != nil {
			return nil, err
		}

		if _, ok := field.Properties[types.FieldPropRelation]; ok {
			err := schema.validateRelation(t_schema.Name, field, nil, res)
			if err != nil {
				return nil, err
			}
		}

		if idx_level := field.IndexLevel(); idx_level > parser.IndexLevelNone && input != nil {
			check_row, err := schema.FindUnique(t_schema, map[string]any{field.Name: res})
			if err != nil {
				return nil, err
			}

			if check_row != nil {
				if idx_level > parser.IndexLevelUnique {
					return nil, NewQueryError(http.StatusConflict, "Primary key already exists")
				}

				return nil, NewQueryError(
					http.StatusConflict,
					fmt.Sprintf("Value for unique field %s already exists", field.Name),
				)
			}
		}

		// we need this incase `AutoIncrement` in used
		t_schema.Fields[field.Name] = field
		row[field.Name] = res
	}

	// Enforce id on every table.
	// We do this because "id" field is not required in the schema
	// but a few actions require it - i.e. update and delete queries
	// so even if the user does not define an "id" field,
	// we still have one to work with
	if _, ok := row["id"]; !ok {
		row["id"] = t_schema.CreateId()
	}

	return row, nil
}

func (schema *Schema) Update(t_schema *parser.Table, row, data map[string]any) (map[string]any, error) {
	res := make(map[string]any)
	for field_name, input := range data {
		field, ok := t_schema.Fields[field_name]

		if !ok {
			continue
		}

		field_data := row[field_name]

		switch input := input.(type) {
		case map[string]any:
			switch field.BuiltinType {
			case types.FieldTypeVector:
				// TODO: make this more dynamic
				to_push := input["push"].([]any)
				field_data = append(field_data.([]any), to_push...)
			case types.FieldTypeInt:
				for k, v := range input {
					_v, err := t_schema.ValidateType(field, v, true)
					if err != nil {
						return nil, err
					}

					v := _v.(int)
					switch k {
					case "increment":
						field_data = field_data.(int) + v
					case "decrement":
						field_data = field_data.(int) - v
					}
				}
			}
		default:
			v, err := t_schema.ValidateType(field, input, false)
			if err != nil {
				return nil, err
			}
			field_data = v
		}

		if _, ok := field.Properties[types.FieldPropRelation]; ok {
			id := row["id"].(int)
			err := schema.validateRelation(t_schema.Name, field, &id, field_data)
			if err != nil {
				return nil, err
			}
		}

		if idx_level := field.IndexLevel(); idx_level > parser.IndexLevelNone && input != nil {
			check_row, err := schema.FindUnique(t_schema, map[string]any{field.Name: field_data})
			if err != nil {
				return nil, err
			}

			if check_row != nil {
				if idx_level > parser.IndexLevelUnique {
					return nil, NewQueryError(http.StatusConflict, "Primary key already exists")
				}

				return nil, NewQueryError(
					http.StatusConflict,
					fmt.Sprintf("Value for unique field %s already exists", field.Name),
				)
			}
		}

		res[field_name] = field_data
	}

	return res, nil
}

// Note: returns a nil value when no row is found(does not throw errow).
// Always make sure to account for this case
func (schema *Schema) FindUnique(t_schema *parser.Table, where map[string]any) (map[string]any, error) {
	if len(where) == 0 {
		return nil, fmt.Errorf("Where constraints cannot be empty")
	}

	for _, index := range t_schema.Indexes {
		if input, ok := where[index]; ok {
			found := schema.filterRows(t_schema, index, input, true)
			if len(found) > 0 {
				return found[0], nil
			}

			return nil, nil
		}
	}

	if len(t_schema.Indexes) > 0 {
		return nil, fmt.Errorf("Unique fields not included in findUnique request")
	} else {
		return nil, fmt.Errorf("Table does not have any unique fields")
	}
}

func (schema *Schema) Find(t_schema *parser.Table, where map[string]any, allow_empty_where bool) ([]map[string]any, error) {
	if allow_empty_where && (where == nil || len(where) == 0) {
		// nil comparison works here
		return schema.filterRows(t_schema, "", nil, false), nil
	} else if where == nil || len(where) == 0 {
		return nil, fmt.Errorf("Where constraints cannot be empty")
	}

	found_rows := [](map[string]any){}
	contains_index := false

	// filter with indexes first
	for _, index := range t_schema.Indexes {
		input, ok := where[index]
		if !ok {
			continue
		}

		contains_index = true
		if len(found_rows) > 0 {
			found_rows = pkg.Filter(found_rows, func(row map[string]any) bool {
				s_field := t_schema.Fields[index]
				return t_schema.Compare(s_field, row[index], input)
			})
		} else {
			found_rows = schema.filterRows(t_schema, index, where[index], false)
		}
	}

	// filter with non-indexes
	if len(found_rows) > 0 {
		for field_name := range t_schema.Fields {
			s_field := t_schema.Fields[field_name]
			input, ok := where[field_name]
			if s_field.IndexLevel() > parser.IndexLevelNone || !ok {
				continue
			}

			found_rows = pkg.Filter(found_rows, func(row map[string]any) bool {
				return t_schema.Compare(s_field, row[field_name], input)
			})
		}
	} else if !contains_index {
		for field_name := range t_schema.Fields {
			if input, ok := where[field_name]; ok {
				found_rows = schema.filterRows(t_schema, field_name, input, false)
			}
		}
	}

	return found_rows, nil
}

type FindArgs struct {
	Where   map[string]any
	Take    map[string]int
	OrderBy map[string]string
	Cursor  map[string]int
}

// TODO: support "take" & "order_by" & "cursor" options
//
// take can only be used when order_by is used
// and cursor can only be used when take is used
func (schema *Schema) FindWithArgs(t_schema *parser.Table, args FindArgs, allow_empty_where bool) ([]map[string]any, error) {
	if allow_empty_where && (args.Where == nil || len(args.Where) == 0) {
		// nil comparison works here
		return schema.filterRows(t_schema, "", nil, false), nil
	} else if args.Where == nil || len(args.Where) == 0 {
		return nil, fmt.Errorf("Where constraints cannot be empty")
	}

	found_rows := [](map[string]any){}
	contains_index := false

	// filter with indexes first
	for _, index := range t_schema.Indexes {
		input, ok := args.Where[index]
		if !ok {
			continue
		}

		contains_index = true
		if len(found_rows) > 0 {
			found_rows = pkg.Filter(found_rows, func(row map[string]any) bool {
				s_field := t_schema.Fields[index]
				return t_schema.Compare(s_field, row[index], input)
			})
		} else {
			found_rows = schema.filterRows(t_schema, index, args.Where[index], false)
		}
	}

	// filter with non-indexes
	if len(found_rows) > 0 {
		for field_name := range t_schema.Fields {
			s_field := t_schema.Fields[field_name]
			input, ok := args.Where[field_name]
			if s_field.IndexLevel() > parser.IndexLevelNone || !ok {
				continue
			}

			found_rows = pkg.Filter(found_rows, func(row map[string]any) bool {
				return t_schema.Compare(s_field, row[field_name], input)
			})
		}
	} else if !contains_index {
		for field_name := range t_schema.Fields {
			if input, ok := args.Where[field_name]; ok {
				found_rows = schema.filterRows(t_schema, field_name, input, false)
			}
		}
	}

	return found_rows, nil
}

func (schema *Schema) Delete(t_schema *parser.Table, row map[string]any) {
	delete(schema.Data[t_schema.Name], pkg.NumToInt(row["id"]))
}
