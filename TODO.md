📋 Tasks Checklist (Step-by-Step)

1. Set up the project
   Use a main.cpp file.

Use a JSON library like nlohmann/json (popular and easy to use).

Organize your code in classes:

KeyValueStore for main logic.

Use std::filesystem for file paths.

2. Design the data file
   Store all entries in a single file (e.g., data.json).

File format: JSON array of objects:

json
Copy
Edit
[
{ "key": "username", "value": { "name": "Alice" } },
{ "key": "score", "value": 42 }
] 3. Implement file loading/saving
On startup: Load the JSON file into memory (a std::vector<nlohmann::json>).

On every change (Create, Update, Delete): Write the entire JSON array back to disk.

4. CRUD Operations
   ✅ Create
   Add a { "key": ..., "value": ... } object to memory.

Check for key collisions.

Rewrite JSON file to disk.

🔍 Read
Look up a key in memory.

Return the value.

✏️ Update
Find key in memory, replace value.

Rewrite JSON file.

❌ Delete
Remove object with matching key.

Rewrite JSON file.

5. Build a simple CLI
   Accept commands like: create, read, update, delete, exit.

Example: create username {"name":"Alice"}

6. Error handling
   Missing file? Create it.

Invalid JSON input? Catch and print an error.

Duplicates or missing keys? Return clear error messages.

7. (Optional) Add some tests
   Create basic unit tests or test your logic through the CLI manually.

🧠 Concepts You'll Practice
File I/O (fstream)

JSON parsing

Vectors, strings, maps

Object-oriented design

CLI input parsing

Serialization/deserialization
