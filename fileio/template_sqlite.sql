--create database with: sqlite3 sqlite.db < template_sqlite.sql
create table objects ("id" INTEGER PRIMARY KEY,"path" TEXT UNIQUE,"version" DATE, "type" TEXT, "creation_params" BLOB,"data" BLOB);
create table dependencies ("id" INTEGER, "depends_id" INTEGER);

