package config

type DB_CONFIG struct {
	PORT     string
	USERNAME string
	password string
	DB_NAME  string
}

func (db *DB_CONFIG) GetPort() string {
	return db.PORT
}

func (db *DB_CONFIG) SetPort(port string) *DB_CONFIG {
	db.PORT = port
	return db
}

func (db *DB_CONFIG) GetUsername() string {
	return db.USERNAME
}

func (db *DB_CONFIG) SetUsername(username string) *DB_CONFIG {
	db.USERNAME = username
	return db
}

func (db *DB_CONFIG) GetPassword() string {
	return db.password
}

func (db *DB_CONFIG) SetPassword(password string) *DB_CONFIG {
	db.password = password
	return db
}

func (db *DB_CONFIG) GetDBName() string {
	return db.DB_NAME
}

func (db *DB_CONFIG) SetDBName(dbname string) *DB_CONFIG {
	db.DB_NAME = dbname
	return db
}
