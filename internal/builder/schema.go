package builder

import (
	"bufio"
	"fmt"
	"net/url"
	"strings"

	. "github.com/tobshub/tobsdb/internal/parser"
	"github.com/tobshub/tobsdb/internal/types"
	"github.com/tobshub/tobsdb/pkg"
)

func NewSchemaFromURL(input *url.URL, data TDBData) (*Schema, error) {
	params, err := url.ParseQuery(input.RawQuery)
	if err != nil {
		return nil, err
	}
	schema_data := params.Get("schema")

	if len(schema_data) == 0 {
		return nil, fmt.Errorf("No schema provided")
	}

	schema := Schema{Tables: make(map[string]*Table), Data: data}

	if schema.Data == nil {
		schema.Data = make(TDBData)
	}

	scanner := bufio.NewScanner(strings.NewReader(schema_data))
	line_idx := 0

	current_table := &Table{IdTracker: 0}

	for scanner.Scan() {
		line_idx++
		line := strings.TrimSpace(scanner.Text())

		// Ignore empty lines & comments
		if len(line) == 0 || strings.HasPrefix(line, "//") {
			continue
		}

		state, data, err := LineParser(line)
		if err != nil {
			return nil, fmt.Errorf("Error parsing line %d: %s", line_idx, err)
		}

		switch state {
		case ParserStateTableStart:
			current_table.Name = data.Name
			current_table.Fields = make(map[string]*Field)
			current_table.Indexes = []string{}
		case ParserStateTableEnd:
			schema.Tables[current_table.Name] = current_table
			current_table = &Table{}
		case ParserStateNewField:
			new_field := Field{
				Name:             data.Name,
				Properties:       data.Properties,
				BuiltinType:      data.Builtin_type,
				IncrementTracker: 0,
			}
			current_table.Fields[new_field.Name] = &new_field

			if new_field.IndexLevel() > IndexLevelNone {
				current_table.Indexes = append(current_table.Indexes, new_field.Name)
			}
		}
	}

	err = ValidateSchemaRelations(&schema)
	if err != nil {
		return nil, err
	}

	for t_name, table := range schema.Tables {
		for key, t_data := range schema.Data[t_name] {
			if key > table.IdTracker {
				table.IdTracker = key
			}

			for f_name, field := range table.Fields {
				if field.BuiltinType != types.FieldTypeInt {
					continue
				}

				_f_data := t_data[f_name]
				if _f_data == nil {
					continue
				}

				f_data := pkg.NumToInt(_f_data)
				if f_data > field.IncrementTracker {
					field.IncrementTracker = f_data
				}
				table.Fields[f_name] = field
			}
		}
		schema.Tables[t_name] = table
	}

	return &schema, nil
}

// TODO: support many-to-many relations
// ValidateSchemaRelations() allows relations to be defined with non-unique fields.
//
// This logic means that relations defined with unqiue fields are 1-to-1 relations,
// while relations defined with non-unique fields are 1-to-many.
func ValidateSchemaRelations(schema *Schema) error {
	for table_key, table := range schema.Tables {
		for field_key, field := range table.Fields {
			relation, is_relation := field.Properties[types.FieldPropRelation]
			if !is_relation {
				continue
			}

			rel_table_name, rel_field_name := ParseRelationProp(relation)

			if len(rel_table_name) == 0 || len(rel_field_name) == 0 {
				return fmt.Errorf(
					"Invalid relation syntax on table %s in field %s",
					table_key,
					field_key,
				)
			}

			rel_table, rel_table_exists := schema.Tables[rel_table_name]

			if !rel_table_exists {
				return fmt.Errorf(
					"Invalid relation between %s and %s in field %s; \"%s\" is not a valid table",
					table_key,
					rel_table_name,
					field_key,
					rel_table_name,
				)
			}

			// (???) allow same-table relations
			// if relation == table_key {
			// 	return fmt.Errorf(
			// 		"Invalid relation between %s and %s in field %s; %s and %s are the same table",
			// 		table_key,
			// 		rel_table_name,
			// 		field_key,
			// 		table_key,
			// 		rel_table_name,
			// 	)
			// }

			rel_field, rel_field_ok := rel_table.Fields[rel_field_name]
			if !rel_field_ok {
				return fmt.Errorf(
					"Invalid relation between %s and %s in field %s; \"%s\" is not a valid field on table %s",
					table_key,
					rel_table_name,
					field_key,
					rel_field_name,
					rel_table_name,
				)
			}

			if rel_field.BuiltinType != field.BuiltinType {
				return fmt.Errorf(
					"Invalid relation between %s and %s in field %s; field types must match",
					table_key,
					rel_table_name,
					field_key,
				)
			}
		}
	}

	return nil
}