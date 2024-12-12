import test from "node:test";
import assert from "node:assert";
import TobsDB from "../dist/index.mjs";
import { fileURLToPath } from "node:url";
import path from "node:path";

/** @type {TobsDB} */
let db;

const tdb_url = "http://localhost:7085";
const __dirname = fileURLToPath(new URL(".", import.meta.url));
const schema_path = path.join(__dirname, "schema.tdb");

await test("Schema Validation", async (t) => {
  await t.test("Valid schema", async () => {
    const valid = await TobsDB.validateSchema(tdb_url, schema_path);
    assert.ok(valid.ok);
  });

  await t.test("Invalid schema", async () => {
    const invalid = await TobsDB.validateSchema(
      tdb_url,
      path.join(__dirname, "invalid_schema.tdb")
    );
    assert.ok(!invalid.ok);
  });

  await t.test("No schema", async () => {
    const invalid = await TobsDB.validateSchema(tdb_url).catch(() => "deez");
    assert.strictEqual(invalid, "deez");
  });
});

await test("Connection", async () => {
  db = await TobsDB.connect(tdb_url, "test_nodejs_client", {
    schema_path: schema_path,
    auth: { username: "user", password: "pass" },
  });
});

await test("NESTED vectors", async (t) => {
  await t.test("Nested vectors: Create a new table", async () => {
    const vec3 = [
      [
        ["hello", "world"],
        ["world", "hello"],
      ],
      [["hi there"], ["how are you?"]],
    ];

    const res = await db.create("nested_vec", {
      vec2: [[1], [2], [3]],
      vec3,
    });

    assert.strictEqual(res.status, 201);
    assert.ok(res.data.id);
    assert.deepStrictEqual(res.data.vec2, [[1], [2], [3]]);
    assert.deepStrictEqual(res.data.vec3, vec3);
  });

  await t.test("Nested vectors: Find tables with nested vector", async () => {
    const count = 20;
    const vec2 = [[101], [6969], [420]];
    const r_create = await db.createMany(
      "nested_vec",
      Array(count).fill({ vec2 })
    );

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(r_create.data.length, count);

    const res = await db.findMany("nested_vec", { vec2 });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length % count, 0);
    assert.deepStrictEqual(res.data[0].vec2, vec2);
  });
});

await test("FIND", async (t) => {
  await t.test("Find a table", async () => {
    // create row
    const r_create = await db.create("example", {
      name: "find example",
      vector: [1, 2, 3],
    });

    assert.strictEqual(r_create.status, 201);

    const res = await db.findUnique("example", { id: r_create.data.id });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.id, r_create.data.id);
    assert.strictEqual(res.data.name, "find example");
    assert.ok(res.__tdb_client_req_id__);
  });
});

while (db.ws.listenerCount("message") > 0) {}
db.disconnect();
