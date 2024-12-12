DIR = ./cmd/tdb
TARGET = out

$(TARGET):
	go build -o $(TARGET) $(DIR)

start: 
	go run $(DIR) -u user -p pass -log -dbg

test:
	go test ./pkg
	node ./tests/test.mjs

run:
	air -- -m -log -dbg -u user -p pass

clean:
	rm -f $(TARGET)

check:
	go run $(DIR) -check
