package db

import "github.com/pythonwithsean/sBase/config"

type DB struct {
	Host     string
	Port     string
	Username string
	Password string
}

func CreateDB(db_config config.DB_CONFIG) {
	// db := DB{
	// 	Host:     db_config.GetHostName(),
	// 	Port:     db_config.GetPort(),
	// 	Username: db_config.GetUsername(),
	// 	Password: db_config.GetPassword(),
	// }

	// tcp := db.Host + ":" + db.Port
	// Connect to the database
}
