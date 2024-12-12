package builder

import (
	"encoding/json"
	"fmt"
	"net/http"

	"github.com/tobshub/tobsdb/internal/parser"
	"github.com/tobshub/tobsdb/internal/types"
	"github.com/tobshub/tobsdb/pkg"
)

func (schema *Schema) filterRows(t_schema *parser.Table, field_name string, value any, exit_first bool) []map[string]any {
	found_rows := []map[string]any{}
	table := schema.Data[t_schema.Name]

	s_field := t_schema.Fields[field_name]

	for _, row := range table {
		if row[field_name] == nil && value == nil {
			found_rows = append(found_rows, row)
		} else if t_schema.Compare(s_field, row[field_name], value) {
			found_rows = append(found_rows, row)
			if exit_first {
				return found_rows
			}
		}
	}

	return found_rows
}

// validateRelation() checks if the row implied by the relation exists
// before the new row is added
func (schema *Schema) validateRelation(table_name string, field *parser.Field, id *int, data any) error {
	relation := field.Properties[types.FieldPropRelation]
	rel_table_name, rel_field_name := parser.ParseRelationProp(relation)
	rel_table_schema := schema.Tables[rel_table_name]

	// TODO: validate many-to-one, many-to-many relations
	if field.BuiltinType == types.FieldTypeVector {
		return nil
	}

	rel_row, err := schema.FindUnique(rel_table_schema, map[string]any{rel_field_name: data})
	if err != nil {
		return err
	}

	if rel_row == nil {
		if is_opt, ok := field.Properties[types.FieldPropOptional]; !ok || is_opt != "true" {
			return fmt.Errorf("No row found for relation table %s", rel_table_name)
		}
	}

	if table_name == rel_table_name && id != nil && pkg.NumToInt(rel_row["id"]) == *id {
		return fmt.Errorf("Row cannot create a relation to itself")
	}

	return nil
}

func HttpError(w http.ResponseWriter, status int, err string) {
	pkg.InfoLog("http error:", err)
	w.WriteHeader(status)
	json.NewEncoder(w).Encode(Response{
		Message: err,
		Status:  status,
	})
}

type QueryError struct {
	msg    string
	status int
}

func NewQueryError(status int, msg string) *QueryError {
	return &QueryError{msg: msg, status: status}
}

func (e QueryError) Error() string { return e.msg }
func (e QueryError) Status() int   { return e.status }
