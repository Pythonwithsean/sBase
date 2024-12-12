package parser

import (
	"errors"
	"fmt"
	"regexp"
	"strings"

	"github.com/tobshub/tobsdb/internal/types"
	"golang.org/x/exp/slices"
)

type Table struct {
	Name      string
	Fields    map[string]*Field
	Indexes   []string
	IdTracker int
}

type Field struct {
	Name             string
	BuiltinType      types.FieldType
	Properties       map[types.FieldProp]string
	IncrementTracker int
}

type LineParserState int

const (
	ParserStateTableStart LineParserState = iota
	ParserStateTableEnd
	ParserStateNewField
	ParserStateIdle
)

type ParserData struct {
	Name         string
	Builtin_type types.FieldType
	Properties   map[types.FieldProp]string
}

func LineParser(line string) (LineParserState, ParserData, error) {
	if strings.HasPrefix(line, "$TABLE") {
		name := line[7:]
		name_end := strings.Index(name, " ")

		if name_end > 0 {
			open_bracket := strings.TrimSpace(name[name_end:])
			if open_bracket != "{" {
				return ParserStateIdle, ParserData{}, fmt.Errorf("Table name cannot include space")
			}
			name = name[:name_end]
			return ParserStateTableStart, ParserData{Name: name}, nil
		}
	} else if line == "}" {
		return ParserStateTableEnd, ParserData{}, nil
	} else {
		splits := cleanLineSplit(strings.Split(line, " "))
		if len(splits) < 2 {
			return ParserStateIdle, ParserData{}, errors.New("Invalid line")
		}
		builtin_type := types.FieldType(splits[1])
		err := validateFieldType(builtin_type)
		if err != nil {
			return ParserStateIdle, ParserData{}, err
		}

		field_props, err := parseRawFieldProps(strings.Join(splits[2:], " "))
		if err != nil {
			return ParserStateIdle, ParserData{}, err
		}

		return ParserStateNewField, ParserData{
			Name:         splits[0],
			Builtin_type: builtin_type,
			Properties:   field_props,
		}, nil
	}
	return ParserStateIdle, ParserData{}, errors.New("Invalid line")
}

func parseRawFieldProps(raw string) (map[types.FieldProp]string, error) {
	props := make(map[types.FieldProp]string)

	r := regexp.MustCompile(`(?m)(\w+)\(([^)]+)\)`)

	for _, entry := range r.FindAllString(raw, -1) {
		split := strings.Split(entry, "(")
		prop, value := types.FieldProp(split[0]), strings.TrimRight(split[1], ")")
		if !slices.Contains(types.VALID_BUILTIN_PROPS, prop) {
			return nil, fmt.Errorf("Invalid field prop: %s", prop)
		}
		props[prop] = value
	}

	return props, nil
}

func validateFieldType(builtin_type types.FieldType) error {
	if slices.Contains(types.VALID_BUILTIN_TYPES, builtin_type) {
		return nil
	}
	return fmt.Errorf("Invalid field type: %s", builtin_type)
}

func cleanLineSplit(splits []string) []string {
	for i := 0; i < len(splits); i++ {
		if len(splits[i]) == 0 {
			splits = append(splits[:i], splits[i+1:]...)
			i--
		}
	}
	return splits
}
