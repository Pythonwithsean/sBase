import { readFileSync } from "fs";
import path from "path";
import WebSocket from "ws";
import { logger } from "./logger";
import crypto from "node:crypto";

type TobsDBOptions = {
  log: boolean;
};

export default class TobsDB<const Schema extends Record<string, object>> {
  /** Connect to a TobsDB server */
  static async connect<const SchemaType extends Record<string, object>>(
    url: string,
    db_name: string,
    conn_options: {
      auth?: { username: string; password: string };
      schema_path?: string;
    },
    options: TobsDBOptions
  ): Promise<TobsDB<SchemaType>> {
    const canonical_url = new URL(url);
    canonical_url.searchParams.set("db", db_name);
    conn_options.schema_path =
      conn_options.schema_path || path.join(process.cwd(), "schema.tdb");
    const schema_data = readFileSync(conn_options.schema_path).toString();
    canonical_url.searchParams.set("schema", schema_data);

    const db = new TobsDB<SchemaType>(
      canonical_url.toString(),
      conn_options.auth,
      options
    );
    await new Promise<void>((res, rej) => {
      db.ws.once("open", () => {
        db.logger.info("Connected to TobsDB server");
        res();
      });
      db.ws.once("error", (e) => {
        db.logger.error(e);
        rej(e);
      });
    });

    return db;
  }

  static async validateSchema(
    url: string,
    schema_path?: string
  ): Promise<TDBSchemaValidationResponse> {
    const canonical_url = new URL(url);
    const schema_data = readFileSync(
      schema_path || path.join(process.cwd(), "schema.tdb")
    ).toString();
    canonical_url.searchParams.set("schema", schema_data);
    canonical_url.searchParams.set("check_schema", "true");

    const res: TDBResponse<QueryType.Schema> = await fetch(canonical_url).then(
      (res) => res.json()
    );

    if (res.status === 200) {
      return { ok: true, message: res.message };
    }
    return { ok: false, message: res.message };
  }

  private ws: WebSocket;
  private logger: ReturnType<typeof logger>;
  private pending: Map<
    string,
    TDBResponse<QueryType.Unique | QueryType.Many, any>
  >;

  constructor(
    public readonly url: string,
    auth: { username: string; password: string } = {
      username: "",
      password: "",
    },
    public readonly options: Partial<TobsDBOptions>
  ) {
    this.ws = new WebSocket(url, {
      headers: { Authorization: `${auth.username}:${auth.password}` },
    });
    this.logger = logger(options?.log ?? false);
    this.pending = new Map();
  }

  async disconnect() {
    this.logger.info("Disconnecting from TobsDB server");
    this.ws.close(1000);
  }

  private __query<
    T extends QueryType.Unique | QueryType.Many,
    const Table extends keyof Schema & string
  >(
    action: QueryAction,
    table: Table,
    data: object | object[] | undefined,
    where?: object | undefined
  ) {
    const __tdb_client_req_id__ = crypto.randomUUID();
    const q = JSON.stringify({
      action,
      table,
      data,
      where,
      __tdb_client_req_id__,
    });
    this.logger.info(action, table);
    this.ws.send(q);
    return new Promise<TDBResponse<T, ParseFieldProps<Schema[Table]>>>(
      (resolve, reject) => {
        if (this.pending.has(__tdb_client_req_id__)) {
          const pending_res = this.pending.get(
            __tdb_client_req_id__
          ) as TDBResponse<QueryType.Unique | QueryType.Many, any>;
          return resolve(pending_res);
        }

        this.ws.once("message", (ev) => {
          const data = JSON.parse(
            Buffer.from(ev.toString()).toString()
          ) as TDBResponse<T, any>;
          if (data.__tdb_client_req_id__ === __tdb_client_req_id__) {
            return resolve(data);
          }

          this.pending.set(data.__tdb_client_req_id__, data);
          if (this.pending.has(__tdb_client_req_id__)) {
            const pending_res = this.pending.get(
              __tdb_client_req_id__
            ) as TDBResponse<QueryType.Unique | QueryType.Many, any>;
            return resolve(pending_res);
          }

          return reject();
        });
      }
    );
  }

  create<const Table extends keyof Schema & string>(
    table: Table,
    data: CreateData<Schema[Table]>
  ) {
    return this.__query<QueryType.Unique, Table>(
      QueryAction.Create,
      table,
      data
    );
  }

  createMany<const Table extends keyof Schema & string>(
    table: Table,
    data: CreateData<Schema[Table]>[]
  ) {
    return this.__query<QueryType.Many, Table>(
      QueryAction.CreateMany,
      table,
      data
    );
  }

  findUnique<const Table extends keyof Schema & string>(
    table: Table,
    where: QueryWhere<Schema[Table], QueryType.Unique>
  ) {
    return this.__query<QueryType.Unique, Table>(
      QueryAction.Find,
      table,
      undefined,
      where
    );
  }

  findMany<const Table extends keyof Schema & string>(
    table: Table,
    where: QueryWhere<Schema[Table], QueryType.Many>
  ) {
    return this.__query<QueryType.Many, Table>(
      QueryAction.FindMany,
      table,
      undefined,
      where
    );
  }

  updateUnique<const Table extends keyof Schema & string>(
    table: Table,
    where: QueryWhere<Schema[Table], QueryType.Unique>,
    data: UpdateData<Schema[Table]>
  ) {
    return this.__query<QueryType.Unique, Table>(
      QueryAction.Update,
      table,
      data,
      where
    );
  }

  updateMany<const Table extends keyof Schema & string>(
    table: Table,
    where: QueryWhere<Schema[Table], QueryType.Many>,
    data: UpdateData<Schema[Table]>
  ) {
    return this.__query<QueryType.Many, Table>(
      QueryAction.UpdateMany,
      table,
      data,
      where
    );
  }

  deleteUnique<const Table extends keyof Schema & string>(
    table: Table,
    where: QueryWhere<Schema[Table], QueryType.Unique>
  ) {
    return this.__query<QueryType.Unique, Table>(
      QueryAction.Delete,
      table,
      undefined,
      where
    );
  }

  deleteMany<const Table extends keyof Schema & string>(
    table: Table,
    where: QueryWhere<Schema[Table], QueryType.Many>
  ) {
    return this.__query<QueryType.Many, Table>(
      QueryAction.DeleteMany,
      table,
      undefined,
      where
    );
  }
}

/** TobsDB primary key */
interface FieldProp<T, N> {
  type: T;
  prop: N;
}

/** Make field the primary key of the table */
export interface PrimaryKey<T> extends FieldProp<T, "primaryKey"> {}
/** Treat field as unique in the table  */
export interface Unique<T> extends FieldProp<T, "unique"> {}
/** Field has default prop in the schema */
export interface Default<T> extends FieldProp<T, "default"> {}

export type CreateData<Table extends object> = ParseFieldProps<
  OptDefaultFields<Table>
>;

export type ParseFieldProps<Table> = {
  [K in keyof Table]: ParseFieldProp<Table[K]>;
};

export type ParseFieldProp<T> = NonNullable<T> extends FieldProp<any, any>
  ? NonNullable<T>["type"]
  : T;

// courtesy of Maya <3
type OptDefaultFields<Table> = {
  [K in keyof Table as NonNullable<Table[K]> extends
    | PrimaryKey<any>
    | Default<any>
    ? K
    : never]?: NonNullable<Table[K]> extends PrimaryKey<any> | Default<any>
    ? NonNullable<Table[K]>["type"]
    : never;
} & {
  [k in keyof Table as NonNullable<Table[k]> extends
    | PrimaryKey<any>
    | Default<any>
    ? never
    : k]: Table[k];
};

type RequireAtLeastOne<T> = Pick<T, Exclude<keyof T, keyof T>> &
  {
    [K in keyof T]-?: Required<Pick<T, K>> &
      Partial<Pick<T, Exclude<keyof T, K>>>;
  }[keyof T];

type QueryWhere<
  Table extends object,
  Type extends QueryType
> = Type extends QueryType.Unique
  ? RequireAtLeastOne<QueryWhereUnique<Table>>
  : QueryWhereMany<Table>;

type QueryWhereUnique<Table extends object> = {
  [K in keyof Table as NonNullable<Table[K]> extends
    | PrimaryKey<any>
    | Unique<any>
    ? K
    : never]: NonNullable<Table[K]> extends PrimaryKey<any> | Unique<any>
    ? NonNullable<Table[K]>["type"]
    : never;
};

type QueryWhereMany<Table extends object> = Partial<{
  [K in keyof Table]:
    | DynamicWhere<ParseFieldProp<Table[K]>>
    | ParseFieldProp<Table[K]>;
}>;

// support dynamic queries
type DynamicWhere<T> = T extends number
  ? {
      gt?: number;
      lt?: number;
      lte?: number;
      gte?: number;
      eq?: number;
      neq?: number;
    }
  : T extends string
  ? { contains?: string; startsWith?: string; endsWith?: string }
  : never;

type UpdateData<Table extends object> = _UpdateData<
  ParseFieldProps<OptDefaultFields<Table>>
>;

type _UpdateData<Table> = {
  [K in keyof Table]?: DynamicUpdate<Table[K]> | Table[K];
};

// support dynamic updates
type DynamicUpdate<T> = T extends number
  ? {
      increment?: number;
      decrement?: number;
    }
  : T extends Array<any>
  ? { push?: T }
  : never;

enum QueryAction {
  Create = "create",
  CreateMany = "createMany",
  Update = "updateUnique",
  UpdateMany = "updateMany",
  Delete = "deleteUnique",
  DeleteMany = "deleteMany",
  Find = "findUnique",
  FindMany = "findMany",
}

export type QueryActionCreate = QueryAction.Create;
export type QueryActionCreateMany = QueryAction.CreateMany;
export type QueryActionUpdate = QueryAction.Update;
export type QueryActionUpdateMany = QueryAction.UpdateMany;
export type QueryActionDelete = QueryAction.Delete;
export type QueryActionDeleteMany = QueryAction.DeleteMany;
export type QueryActionFind = QueryAction.Find;
export type QueryActionFindMany = QueryAction.FindMany;

enum QueryType {
  Unique,
  Many,
  Schema,
}

export type QueryTypeUnique = QueryType.Unique;
export type QueryTypeMany = QueryType.Many;

export interface TDBResponse<U extends QueryType, Table extends object = {}> {
  status: number;
  message: string;
  data: U extends QueryType.Unique
    ? Table
    : U extends QueryType.Many
    ? Table[]
    : string;
  __tdb_client_req_id__: string;
}

export interface TDBSchemaValidationResponse {
  ok: boolean;
  message: string;
}

// async () => {
//   type DB = {
//     hello: {
//       id: PrimaryKey<number>;
//       world: string;
//       hi: Unique<string>;
//       deez?: string;
//     };
//     world: {
//       id: PrimaryKey<number>;
//       pew: Unique<string>;
//       hello: string;
//     };
//   };

//   const t = await TobsDB.connect<DB>("", "", {});

//   const p = await t.create("hello", { world: "", hi: "string" });
//   t.findUnique("hello", { id: 0 }, {});
//   t.findUnique("world", { id: 0 }, { hello: true }, {});
//   // t.findMany("hello", { id: { eq: 69 }, world: "deez" });
//   // t.updateUnique()
//   // t.updateMany("hello", { id: { lte: 69 } }, { id: { decrement: 1 } });
//   // t.deleteUnique("hello", { id: 0 });
//   // t.deleteMany("hello", { id: { lte: 69 } });
// };
