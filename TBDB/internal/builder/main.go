package builder

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"os/signal"
	"strconv"
	"syscall"
	"time"

	"github.com/gorilla/websocket"

	"github.com/tobshub/tobsdb/internal/parser"
	"github.com/tobshub/tobsdb/pkg"
)

type (
	// Maps row field name to its saved data
	tdbDataRow = map[string]any
	// Maps row id to its saved data
	tdbDataTable = map[int](tdbDataRow)
	// Maps table name to its saved data
	TDBData = map[string]tdbDataTable
)

type Schema struct {
	Tables map[string]*parser.Table
	Data   TDBData
}

type TDBWriteSettings struct {
	write_path     string
	in_mem         bool
	write_ticker   *time.Ticker
	write_interval int
}

func NewWriteSettings(write_path string, in_mem bool, write_interval int) *TDBWriteSettings {
	var write_ticker *time.Ticker
	if !in_mem {
		if len(write_path) == 0 {
			pkg.FatalLog("Must either provide db path or use in-memory mode")
		}
		write_ticker = time.NewTicker(time.Duration(write_interval) * time.Millisecond)
	}
	return &TDBWriteSettings{write_path, in_mem, write_ticker, write_interval}
}

type TobsDB struct {
	// db_name -> table_name -> row_id -> field_name
	data           map[string]TDBData
	write_settings *TDBWriteSettings
	last_change    time.Time
}

type LogOptions struct {
	Should_log      bool
	Show_debug_logs bool
}

func NewTobsDB(write_settings *TDBWriteSettings, log_options LogOptions) *TobsDB {
	if log_options.Should_log {
		if log_options.Show_debug_logs {
			pkg.SetLogLevel(pkg.LogLevelDebug)
		} else {
			pkg.SetLogLevel(pkg.LogLevelErrOnly)
		}
	} else {
		pkg.SetLogLevel(pkg.LogLevelNone)
	}

	data := make(map[string]TDBData)
	if len(write_settings.write_path) > 0 {
		f, open_err := os.Open(write_settings.write_path)
		if open_err != nil {
			pkg.ErrorLog(open_err)
		}
		defer f.Close()

		err := json.NewDecoder(f).Decode(&data)
		if err != nil {
			if err == io.EOF {
				pkg.WarnLog("read empty db file")
			} else {
				_, is_open_error := open_err.(*os.PathError)
				if !is_open_error {
					pkg.FatalLog("failed to decode db from file;", err)
				}
			}
		}

		pkg.InfoLog("loaded database from file", write_settings.write_path)
	}

	last_change := time.Now()
	return &TobsDB{data, write_settings, last_change}
}

type RequestAction string

const (
	RequestActionCreate     RequestAction = "create"
	RequestActionCreateMany RequestAction = "createMany"
	RequestActionFind       RequestAction = "findUnique"
	RequestActionFindMany   RequestAction = "findMany"
	RequestActionDelete     RequestAction = "deleteUnique"
	RequestActionDeleteMany RequestAction = "deleteMany"
	RequestActionUpdate     RequestAction = "updateUnique"
	RequestActionUpdateMany RequestAction = "updateMany"
)

type WsRequest struct {
	Action RequestAction `json:"action"`
	ReqId  string        `json:"__tdb_client_req_id__"` // used in tdb clients
}

func (db *TobsDB) Listen(port int) {
	exit := make(chan os.Signal, 2)
	signal.Notify(exit, os.Interrupt, syscall.SIGTERM)

	s := &http.Server{
		Addr:         fmt.Sprintf(":%d", port),
		ReadTimeout:  0,
		WriteTimeout: 0,
	}

	upgrader := websocket.Upgrader{
		WriteBufferSize: 1024 * 10,
		ReadBufferSize:  1024 * 10,
		CheckOrigin:     func(r *http.Request) bool { return true },
	}

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		url_query := r.URL.Query()
		db_name := url_query.Get("db")
		check_schema_only, check_schema_only_err := strconv.ParseBool(r.URL.Query().Get("check_schema"))

		if len(db_name) == 0 && !check_schema_only {
			HttpError(w, http.StatusBadRequest, "Missing db name")
			return
		}

		db_data := db.data[db_name]
		schema, err := NewSchemaFromURL(r.URL, db_data)
		if err != nil {
			HttpError(w, http.StatusBadRequest, err.Error())
			return
		}

		if check_schema_only_err == nil && check_schema_only {
			pkg.InfoLog("Schema checks completed: Schema is valid")
			json.NewEncoder(w).Encode(Response{
				Status:  http.StatusOK,
				Data:    *schema,
				Message: "Schema checks completed: Schema is valid",
			})
			return
		}

		env_auth := fmt.Sprintf("%s:%s", os.Getenv("TDB_USER"), os.Getenv("TDB_PASS"))
		var conn_auth string
		if url_query.Has("auth") {
			conn_auth = url_query.Get("auth")
		} else if url_query.Has("username") || url_query.Has("password") {
			conn_auth = url_query.Get("username") + ":" + url_query.Get("password")
		} else {
			conn_auth = r.Header.Get("Authorization")
		}
		if conn_auth != env_auth {
			HttpError(w, http.StatusUnauthorized, "connection unauthorized")
			return
		}

		conn, err := upgrader.Upgrade(w, r, nil)
		if err != nil {
			pkg.ErrorLog(err)
			return
		}
		pkg.InfoLog("New connection established")
		defer conn.Close()

		for {
			_, message, err := conn.ReadMessage()
			if err != nil {
				if websocket.IsUnexpectedCloseError(err, websocket.CloseNormalClosure, websocket.CloseGoingAway) {
					pkg.ErrorLog("unexpected close", err)
				} else {
					pkg.DebugLog("connection closed", err)
				}
				return
			}

			if db.write_settings.write_ticker != nil {
				// reset write timer when a reqeuest is received
				db.write_settings.write_ticker.Reset(time.Duration(db.write_settings.write_interval) * time.Millisecond)
			}

			var req WsRequest
			json.NewDecoder(bytes.NewReader(message)).Decode(&req)

			var res Response

			switch req.Action {
			case RequestActionCreate:
				res = CreateReqHandler(schema, message)
			case RequestActionCreateMany:
				res = CreateManyReqHandler(schema, message)
			case RequestActionFind:
				res = FindReqHandler(schema, message)
			case RequestActionFindMany:
				res = FindManyReqHandler(schema, message)
			case RequestActionDelete:
				res = DeleteReqHandler(schema, message)
			case RequestActionDeleteMany:
				res = DeleteManyReqHandler(schema, message)
			case RequestActionUpdate:
				res = UpdateReqHandler(schema, message)
			case RequestActionUpdateMany:
				res = UpdateManyReqHandler(schema, message)
			}

			res.ReqId = req.ReqId

			if err := conn.WriteJSON(res); err != nil {
				pkg.ErrorLog("writing response", err)
				return
			}

			if req.Action != RequestActionFind && req.Action != RequestActionFindMany {
				db.data[db_name] = schema.Data
				db.last_change = time.Now()
			}
		}
	})

	// listen for requests on non-blocking thread
	go func() {
		err := s.ListenAndServe()
		if err != http.ErrServerClosed {
			pkg.FatalLog(err)
		}
	}()

	go func() {
		if db.write_settings.write_ticker == nil {
			return
		}

		last_write := db.last_change

		for {
			<-db.write_settings.write_ticker.C
			if db.last_change.After(last_write) {
				pkg.DebugLog("writing database to file")
				db.writeToFile()
				last_write = db.last_change
			}
		}
	}()

	pkg.InfoLog("TobsDB listening on port", port)
	<-exit
	pkg.DebugLog("Shutting down...")
	s.Shutdown(context.Background())
	db.writeToFile()
}

func (db *TobsDB) writeToFile() {
	if db.write_settings.in_mem {
		return
	}

	data, err := json.Marshal(db.data)
	if err != nil {
		pkg.FatalLog("marshalling database for write", err)
	}

	err = os.WriteFile(db.write_settings.write_path, data, 0644)

	if err != nil {
		pkg.FatalLog("writing database to file", err)
	}
}
