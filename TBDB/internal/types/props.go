package types

type FieldProp string

var VALID_BUILTIN_PROPS = []FieldProp{
	FieldPropOptional, FieldPropDefault, FieldPropRelation,
	FieldPropKey, FieldPropUnique, FieldPropVector,
}

const (
	FieldPropOptional FieldProp = "optional"
	FieldPropDefault  FieldProp = "default"
	FieldPropRelation FieldProp = "relation" // relation(table.field)
	FieldPropKey      FieldProp = "key"
	FieldPropUnique   FieldProp = "unique"
	FieldPropVector   FieldProp = "vector" // vector(type, level)
)
