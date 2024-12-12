# Client Overview

TobsDB has official clients for the following languages:

- JavaScript/Typescript

Support for other languages will be added.

## Making a TobsDB client.

Anything can be a tobsdb client if it:

1. Connects to a tobsdb server via a websocket connect request.
2. Sends requests to the tobsdb servers through the connected websocket.

That's it.

Well there's a little bit more to it:

All queries require these two arguments:

- `action`: The query action to execute. 
One of: `create`, `createMany`, `findUnique`, `findMany`, `updateUnique`, 
`updateMany`, `deleteUnique`, `deleteMany`
- `table`: The name of the table in the tobsdb server to run the query on.

For the purpose of this explanation let's divide all the possible queries in 3 groups:

- Change-only queries: create, createMany
- Search-only queries: findUnique, findMany, deleteUnique, deleteMany
- and Search-Change queries: updateUnique, updateMany

The reason this grouping was made because of the arguments they take.

Change-only queries require the following additional argument:

- `data`: The data to use in the query. 
In the case of `createMany` queries, this is expected to be an array.

Search-only queries require the following additional argument:

- `where`: The conditions to check against in the query.

Search-Change queries require the following additional arguments:

- `where`: The conditions to check against in the query.
- `data`: The data to use in the query. 

In Search-only and Search-Change `*Unique` queries, in the `where` argument, 
only unique or primary-key fields are used in the operation even if others are provided.

In Search-only and Search-Change `*Many` queries, in the `where` argument,
there are dynamic search options available for `Int` and `String` fields:

- `Int`: eq, ne, gt, gte, lt, lte
- `String`: contains, startsWith, endsWith

In Search-Change queries, in the `data` argument, there are dynamic search change
options available for `Int` and `Vector` fields:

- `Int`: increment, decrement
- `Vector`: push

*In all queries, only fields that are defined in the schema are used,
even if others are provided.

## Example Request 

With of a schema.tdb:

```sql
$TABLE user {
    id      Int key(primary)
    name    String
    age     Int
    height  String default("short")
}
```

### createUnique

```json
{
    "action": "create",
    "table": "user",
    "data": { "name": "Maya", "age": 28 }, 
}
```

### createMany

```json
{
    "action": "createMany",
    "table": "user",
    "data": [
        { "name": "Karolis", "age": 21, "height": "tall" }
        { "name": "Wolfred", "age": 24 }, 
    ]
}
```

### findUnique

```json
{
    "action": "findUnique",
    "table": "user",
    "where": { "id": 1 }
}
```

### findMany

```json
{
    "action": "findMany",
    "table": "user",
    "where": { "age": { "gte": 18 }}
}
```

### updateUnique

```json
{
    "action": "updateUnique",
    "table": "user",
    "where": { "id": 1 },
    "data": { "age": { "increment": 1 }, "height": "tall" } 
}
```

### updateMany

```json
{
    "action": "updateMany",
    "table": "user",
    "where": { "age": { "gte": 69 }},
    "data": { "age": { "decrement": 1 }} 
}
```

### deleteUnique

```json
{
    "action": "deleteUnique",
    "table": "user",
    "where": { "id": 1 },
}
```

### deleteMany

```json
{
    "action": "deleteMany",
    "table": "user",
    "where": { "name": { "contains": "o" }},
}
```
