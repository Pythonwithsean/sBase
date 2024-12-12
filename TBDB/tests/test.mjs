import test from "node:test";
import assert from "assert";
import crypto from "crypto";
import WebSocket from "ws";

const schema = `
// comment 1
$TABLE example {

  id Int key(primary) default(auto)

  name String default("Hello world")

  vector Vector vector(Int) 

  createdAt Date default(now)
}

$TABLE first {
  id  Int key(primary)
  createdAt Date default(now)
  updatedAt Date optional(true)
  user Int relation(example.id) 
  // comment 2
}

$TABLE second {
  id  Int key(primary)
  createdAt Date default(now)
  updatedAt Date optional(true)
  rel_str String relation(third.str)
}

$TABLE third {
  id Int key(primary)
  str String unique(true)
}

$TABLE nested_vec {
  id Int key(primary)
  vec2 Vector vector(Int, 2)
  vec3 Vector vector(String, 3) optional(true)
}

$TABLE fourth {
  id Int key(primary)
  num Int
}

$TABLE autoincr {
  id   Int key(primary)
  auto Int default(autoincrement)
}

$TABLE v_rel_1 {
  vector Vector vector(Int) relation(v_rel_2.id)
}

$TABLE v_rel_2 {
  id Int key(primary)
}
`;

const ws = new WebSocket(
  `ws://localhost:7085?db=test&schema=${encodeURIComponent(schema)}`,
  { headers: { Authorization: "user:pass" } }
);
await new Promise((res, rej) => {
  ws.onopen = () => {
    res();
  };

  ws.onerror = (e) => {
    rej(e);
  };
});

const API = (action, body) => {
  const message = JSON.stringify({ action, ...body });
  ws.send(message);

  return new Promise((res) => {
    ws.once("message", (ev) => {
      const data = JSON.parse(Buffer.from(ev.toString()).toString());
      res(data);
    });
  });
};

await test("Validate schema", async (t) => {
  await t.test("simple schema", async () => {
    const canonical_url = new URL("http://localhost:7085");
    canonical_url.searchParams.set("schema", "$TABLE c {\n f Int \n }");
    canonical_url.searchParams.set("check_schema", "true");
    const res = await fetch(canonical_url)
      .then((res2) => res2.json())
      .catch((e) => console.log(e));

    assert.strictEqual(res.status, 200);
  });

  await t.test("invalid schema: bad vector relaton", async () => {
    const url = new URL("http://localhost:7085");
    url.searchParams.set(
      "schema",
      `
$TABLE a {
  b Vector vector(String)
}

$TABLE b {
  a Vector vector(Int) relation(a.b)
}
`
    );
    url.searchParams.set("check_schema", true);
    const res = await fetch(url)
      .then((res) => res.json())
      .catch((e) => console.log(e));

    assert.strictEqual(res.status, 400);
  });

  await t.test("invalid schema: invalid prop name", async () => {
    const url = new URL("http://localhost:7085");
    url.searchParams.set(
      "schema",
      `
$TABLE a {
  b Int unqiue(true)
}
`
    );
    url.searchParams.set("check_schema", true);
    const res = await fetch(url)
      .then((res) => res.json())
      .catch((e) => console.log(e));

    assert.strictEqual(res.status, 400);
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

    const res = await API("create", {
      table: "nested_vec",
      data: {
        vec2: [[1], [2], [3]],
        vec3,
      },
    });

    assert.strictEqual(res.status, 201);
    assert.ok(res.data.id);
    assert.deepStrictEqual(res.data.vec2, [[1], [2], [3]]);
    assert.deepStrictEqual(res.data.vec3, vec3);
  });

  await t.test("Nested vectors: Find tables with nested vector", async () => {
    const count = 20;
    const vec2 = [[101], [6969], [420]];
    const r_create = await API("createMany", {
      table: "nested_vec",
      data: Array(count).fill({
        vec2,
      }),
    });

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(r_create.data.length, count);

    const res = await API("findMany", {
      table: "nested_vec",
      where: { vec2 },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length % count, 0);
    assert.deepStrictEqual(res.data[0].vec2, vec2);
  });
});

await test("CREATE", async (t) => {
  await t.test("Create a new table", async () => {
    const res = await API("create", {
      table: "example",
      data: { vector: [1, 2, 3] },
    });

    assert.strictEqual(res.data.name, "Hello world");
    assert.ok(res.data.id, "Returned row should have an id");
    assert.ok(res.data.createdAt, "Returned row should have a createdAt");
  });

  await t.test("Create a new table with relation(Int)", async () => {
    const r_create = await API("create", {
      table: "example",
      data: { name: "relation example", vector: [1, 2, 3] },
    });

    assert.strictEqual(r_create.status, 201);

    const updatedAt = Date.now();
    const res = await API("create", {
      table: "first",
      data: { updatedAt, user: r_create.data.id },
    });

    assert.ok(res.data.id, "Returned row should have an id");
    assert.strictEqual(new Date(res.data.updatedAt).getTime(), updatedAt);
    assert.ok(res.data.createdAt, "Returned row should have a createdAt");
    assert.strictEqual(res.data.user, r_create.data.id);
  });

  await t.test("Create a new table with relation(String)", async () => {
    const uniqueStr = crypto.randomUUID();
    const r_create = await API("create", {
      table: "third",
      data: { str: uniqueStr },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("create", {
      table: "second",
      data: { rel_str: uniqueStr },
    });

    assert.strictEqual(res.status, 201);
    assert.ok(res.data.id, "Returned row should have an id");
    assert.ok(res.data.createdAt, "Returned row should have a createdAt");
    assert.strictEqual(res.data.rel_str, uniqueStr);
  });

  await t.test("CreateUnique: 500 new tables", async () => {
    const count = 500;
    const data = Array(count).fill({ name: `1 of ${count}`, vector: [count] });

    for (let i = 0; i < data.length; i++) {
      const row = data[i];
      const res = await API("create", {
        table: "example",
        data: row,
      });
      assert.strictEqual(res.status, 201, "Status code should be 201");
    }
  });

  await t.test("CreateMany: 10_000 new tables", async () => {
    const table = "example";
    const count = 10000;
    const res = await API("createMany", {
      table: table,
      data: Array(count).fill({
        name: `group of ${count}`,
        createdAt: Date.now(),
        vector: [count],
      }),
    });

    assert.strictEqual(res.data.length, count);
    assert.strictEqual(
      res.data[res.data.length - 1].id - res.data[0].id,
      count - 1
    );
    assert.strictEqual(
      res.message,
      `Created ${count} new rows in table ${table}`
    );
  });

  await t.test(
    "CreateMany with autoincrement(check with findMany gte)",
    async () => {
      const res = await API("createMany", {
        table: "autoincr",
        data: Array(2).fill({}),
      });

      assert.strictEqual(res.status, 201);
      assert.strictEqual(res.data.length, 2);
      assert.strictEqual(res.data[1].auto - res.data[0].auto, 1);

      const check = await API("findMany", {
        table: "autoincr",
        where: { auto: { gte: res.data[0].auto } },
      });

      assert.strictEqual(check.status, 200);
      assert.strictEqual(check.data.length, 2);
    }
  );

  await t.test("Error because of missing required field", async () => {
    const res = await API("create", {
      table: "example",
      data: {},
    });

    assert.strictEqual(res.status, 400);
  });

  await t.test("Error because of passing unknown table", async () => {
    const res = await API("create", {
      table: "bad_example",
      data: { deez: "nuts" },
    });

    assert.strictEqual(res.status, 404);
  });

  await t.test("Error because of existing primary key", async () => {
    const r_create = await API("create", {
      table: "example",
      data: { name: "bad example", vector: [1, 2, 3] },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("create", {
      table: "example",
      data: { id: r_create.data.id, name: "bad example", vector: [1, 2, 3] },
    });

    assert.strictEqual(res.status, 409);
    assert.strictEqual(res.message, "Primary key already exists");
  });
});

await test("FIND", async (t) => {
  await t.test("Find a table", async () => {
    // create row
    const r_create = await API("create", {
      table: "example",
      data: { name: "find example", vector: [1, 2, 3] },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("findUnique", {
      table: "example",
      where: { id: r_create.data.id },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.id, r_create.data.id);
    assert.strictEqual(res.data.name, "find example");
  });

  await t.test("Find Many", async () => {
    // create rows
    const count = 50;
    const uniqueName = crypto.randomUUID();

    const r_create = await API("createMany", {
      table: "example",
      data: Array(count).fill({ name: uniqueName, vector: [1, 2, 3] }),
    });

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(r_create.data.length, count);

    const res = await API("findMany", {
      table: "example",
      where: { name: uniqueName },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, count);
  });

  await t.test("Find Many with empty where", async () => {
    const res = await API("findMany", {
      table: "second",
      where: {},
    });

    assert.strictEqual(res.status, 200);
    assert.ok(res.data.length >= 0);
  });

  await t.test("Find Many with contains", async () => {
    // create rows
    const count = 50;
    const uniqueName = crypto.randomUUID();

    const r_create = await API("createMany", {
      table: "example",
      data: Array(count).fill({ name: uniqueName, vector: [1, 2, 3] }),
    });

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(r_create.data.length, count);

    const res = await API("findMany", {
      table: "example",
      where: { name: { contains: uniqueName.substring(0, 7) } },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, count);
  });

  await t.test("Find with Date field (manual)", async () => {
    const date = Date.now();
    const r_create = await API("create", {
      table: "example",
      data: { createdAt: date, vector: [1, 2, 3] },
    });

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(new Date(r_create.data.createdAt).getTime(), date);

    const res = await API("findMany", {
      table: "example",
      where: { createdAt: date },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, 1);
    assert.strictEqual(new Date(res.data[0].createdAt).getTime(), date);
  });

  await t.test("Find with Date field (auto)", async () => {
    const name = crypto.randomUUID();
    const r_create = await API("create", {
      table: "example",
      data: { name, vector: [1, 2, 3] },
    });

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(r_create.data.name, name);

    let res = await API("findMany", {
      table: "example",
      where: { name },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, 1);
    assert.strictEqual(res.data[0].name, name);

    const createdAt = res.data[0].createdAt;

    res = await API("findMany", {
      table: "example",
      where: { createdAt },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, 1);
    assert.strictEqual(res.data[0].createdAt, createdAt);
  });

  await t.test("Find with Vector field", async () => {
    const vector = [];
    for (let i = 0; i < 100; i++) {
      vector.push(parseInt(Math.random() * 100) * i);
    }

    const r_create = await API("create", {
      table: "example",
      data: { vector },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("findMany", {
      table: "example",
      where: { vector },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, 1);
    assert.strictEqual(res.data[0].vector.length, 100);
  });

  await t.test("Find with gt & lte value", async () => {
    for (let i = 1; i <= 50; i++) {
      const r_create = await API("create", {
        table: "fourth",
        data: { id: i, num: i },
      });

      // allow 409 error here
      // it just means that the test has been run before
      assert.notStrictEqual(r_create.status, 400);
    }

    const res = await API("findMany", {
      table: "fourth",
      where: { num: { gt: 25, lte: 50 } },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, 25);
  });

  await t.test("Find with eq value", async () => {
    const num = 69420;
    const r_create = await API("create", {
      table: "fourth",
      data: { num },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("findMany", {
      table: "fourth",
      where: { num: { eq: num } },
    });

    assert.strictEqual(res.status, 200);
    assert.ok(res.data.length >= 1);

    for (const item of res.data) {
      assert.strictEqual(item.num, num);
    }
  });
});

await test("UPDATE", async (t) => {
  await t.test("Update a table", async () => {
    // create row
    const r_create = await API("create", {
      table: "example",
      data: { name: "update example", vector: [1, 2, 3] },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("updateUnique", {
      table: "example",
      where: { id: r_create.data.id },
      data: { name: "updated", vector: [3, 2, 1] },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.id, r_create.data.id);
    assert.strictEqual(res.data.name, "updated");
    assert.deepStrictEqual(res.data.vector, [3, 2, 1]);

    const check = await API("findUnique", {
      table: "example",
      where: { id: r_create.data.id },
    });

    assert.strictEqual(check.status, 200);
    assert.strictEqual(check.data.id, r_create.data.id);
    assert.strictEqual(check.data.name, "updated");
  });

  await t.test("Update a table(dynamic Int)", async () => {
    const num = 69420;
    const r_create = await API("create", {
      table: "fourth",
      data: { num },
    });

    assert.strictEqual(r_create.status, 201);

    const inc = 20,
      dec = 5;
    const res = await API("updateUnique", {
      table: "fourth",
      where: { id: r_create.data.id },
      data: { num: { increment: inc, decrement: dec } },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.num, num + inc - dec);

    const check = await API("findUnique", {
      table: "fourth",
      where: { id: r_create.data.id },
    });

    assert.strictEqual(check.status, 200);
    assert.strictEqual(check.data.num, num + inc - dec);
  });

  await t.test("Update a table(dynamic Vector)", async () => {
    const r_create = await API("create", {
      table: "example",
      data: { vector: [1, 2, 3], name: "dynamic vector example" },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("updateUnique", {
      table: "example",
      where: { id: r_create.data.id },
      data: { vector: { push: [4, 5, 6] } },
    });

    assert.strictEqual(res.status, 200);
    assert.deepStrictEqual(res.data.vector, [1, 2, 3, 4, 5, 6]);
  });

  await t.test("Update a table(relation)", async () => {
    const c_uniqueStr = crypto.randomUUID();
    const r_create = await API("create", {
      table: "third",
      data: { str: c_uniqueStr },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("create", {
      table: "second",
      data: { rel_str: c_uniqueStr },
    });

    assert.strictEqual(res.status, 201);
    assert.ok(res.data.rel_str, c_uniqueStr);

    const uniqueStr = crypto.randomUUID();
    const r_create2 = await API("create", {
      table: "third",
      data: { str: uniqueStr },
    });

    assert.strictEqual(r_create2.status, 201);

    const res2 = await API("updateUnique", {
      table: "second",
      where: { id: res.data.id },
      data: { rel_str: uniqueStr },
    });

    assert.strictEqual(res2.status, 200);
    assert.strictEqual(res2.data.rel_str, uniqueStr);
  });

  await t.test("Failed to Update a table(relation): wrong type", async () => {
    const c_uniqueStr = crypto.randomUUID();
    const r_create = await API("create", {
      table: "third",
      data: { str: c_uniqueStr },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("create", {
      table: "second",
      data: { rel_str: c_uniqueStr },
    });

    assert.strictEqual(res.status, 201);
    assert.ok(res.data.rel_str, c_uniqueStr);

    const res2 = await API("updateUnique", {
      table: "second",
      where: { id: res.data.id },
      data: { rel_str: 1 },
    });

    assert.strictEqual(res2.status, 400);
  });

  await t.test(
    "Failed to Update a table(relation): relation not found",
    async () => {
      const c_uniqueStr = crypto.randomUUID();
      const r_create = await API("create", {
        table: "third",
        data: { str: c_uniqueStr },
      });

      assert.strictEqual(r_create.status, 201);

      const res = await API("create", {
        table: "second",
        data: { rel_str: c_uniqueStr },
      });

      assert.strictEqual(res.status, 201);
      assert.ok(res.data.rel_str, c_uniqueStr);

      const res2 = await API("updateUnique", {
        table: "second",
        where: { id: res.data.id },
        data: { rel_str: "no table has this rel_str value" },
      });

      assert.strictEqual(res2.status, 400);
      assert.ok(res2.message.includes("No row found for relation"));
    }
  );

  await t.test("Failed to Update a table: duplicate unique field", async () => {
    const c_uniqueStr = crypto.randomUUID();
    const r_create = await API("create", {
      table: "third",
      data: { str: c_uniqueStr },
    });

    assert.strictEqual(r_create.status, 201);

    const c_uniqueStr_2 = crypto.randomUUID();
    const r_create_2 = await API("create", {
      table: "third",
      data: { str: c_uniqueStr_2 },
    });

    assert.strictEqual(r_create_2.status, 201);

    const res = await API("updateUnique", {
      table: "third",
      where: { str: c_uniqueStr },
      data: { str: c_uniqueStr_2 },
    });

    assert.strictEqual(res.status, 409);
  });

  await t.test("Update 1_000 tables", async () => {
    const count = 1000;
    const uniqueName = crypto.randomUUID();

    const r_create = await API("createMany", {
      table: "example",
      data: Array(count).fill({ name: uniqueName, vector: [1, 2, 3] }),
    });

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(r_create.data.length, count);

    const res = await API("updateMany", {
      table: "example",
      where: { name: uniqueName },
      data: { name: `updated ${count}: ${uniqueName}` },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, count);

    for (let i = 0; i < count; i++) {
      assert.strictEqual(res.data[i].name, `updated ${count}: ${uniqueName}`);
    }
  });
});

await test("DELETE", async (t) => {
  await t.test("Delete a table", async () => {
    // create row
    const r_create = await API("create", {
      table: "example",
      data: { name: "delete example", vector: [1, 2, 3] },
    });

    assert.strictEqual(r_create.status, 201);

    const res = await API("deleteUnique", {
      table: "example",
      where: { id: r_create.data.id },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.id, r_create.data.id);
  });

  await t.test("Delete 1_000 tables", async () => {
    const count = 1000;
    const uniqueName = crypto.randomUUID();

    const r_create = await API("createMany", {
      table: "example",
      data: Array(count).fill({ name: uniqueName, vector: [1, 2, 3] }),
    });

    assert.strictEqual(r_create.status, 201);
    assert.strictEqual(r_create.data.length, count);

    const res = await API("deleteMany", {
      table: "example",
      where: { name: uniqueName },
    });

    assert.strictEqual(res.status, 200);
    assert.strictEqual(res.data.length, count);
  });

  await t.test(
    "Error because of passing empty where statement to deleteUnique",
    async () => {
      const res = await API("deleteUnique", {
        table: "example",
        where: {},
      });

      assert.strictEqual(res.status, 400);
      assert.strictEqual(res.message, "Where constraints cannot be empty");
    }
  );

  await t.test("Error because of passing unknown table", async () => {
    const res = await API("deleteUnique", {
      table: "bad_example",
      where: {},
    });

    assert.strictEqual(res.status, 404);
    assert.strictEqual(res.message, "Table not found");
  });
});

// cleanup
while (ws.listenerCount("message") > 0) {}
ws.close(1000);
