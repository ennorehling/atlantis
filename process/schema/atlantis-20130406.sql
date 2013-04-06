CREATE TABLE orders (date datetime NOT NULL default CURRENT_TIMESTAMP, email VARCHAR(32), filename varchar(32) NOT NULL UNIQUE, turn INT DEFAULT 0);
