# TobsDB NodeJS client

This is the official TobsDB NodeJS client.

## Usage

```js
import TobsDB from "tobsdb";

const db = await TobsDB.connect(
    "ws://localhost:7085", 
    "db_name", 
    { 
        schema_path: "path/to/schema.tdb",
        auth: {username: "user", password: "password" }
    }
);

const res = db.create("table_name", { hello: "world" });
```
