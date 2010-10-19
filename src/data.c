#include "data.h"
#include <sqlite3.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "collections.h"

static gboolean data_check_and_create_database(gchar *data_file);
static gint data_create_new_database(const gchar *filename);

GList *data_get_series(void)
{
  sqlite3 *database;
  sqlite3_stmt *statement;
  gchar *data_file;
  GList *list = NULL;

  data_file = collections_get_data_file();

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int res;
      const char *sqlStatement =
        " SELECT   id, "
        "          name, "
        "          current_qty, "
        "          total_qty "
        " FROM     collection "
        " ORDER BY name "
        " COLLATE NOCASE ";

      res = sqlite3_prepare_v2(database,
                               sqlStatement,
                               strlen(sqlStatement),
                               &statement, NULL);
      if (res == SQLITE_OK) {
        while (sqlite3_step(statement) == SQLITE_ROW) {
          struct collection *col =
            (struct collection *)malloc(sizeof(struct collection));

          col->id = sqlite3_column_int(statement, 0);
          col->name = g_strdup(sqlite3_column_text(statement, 1));
          col->current_qty = sqlite3_column_int(statement, 2);
          col->total_qty = sqlite3_column_int(statement, 3);

          list = g_list_append(list, (gpointer)col);
        }
      }
      else
        g_print("error %d: %s\n", res, sqlite3_errmsg(database));
      /* Release the compiled statement from memory */
      sqlite3_finalize(statement);
      }
      sqlite3_close(database);
  }

  return list;
}

gboolean data_add_series(gchar *name, gint total_qty)
{
  sqlite3 *database;
  sqlite3_stmt *statement;
  gchar *data_file;

  data_file = collections_get_data_file();

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int res;
      const char *sqlStatement =
        g_strdup_printf("INSERT INTO collection (name, current_qty, total_qty) "
                        " VALUES ('%s', 0, %d)", name, total_qty);

      res = sqlite3_prepare_v2(database,
                               sqlStatement,
                               strlen(sqlStatement),
                               &statement, NULL);
      if (res == SQLITE_OK) {
        if (sqlite3_step(statement) == SQLITE_DONE)
          return TRUE;
      }
    }
  }

  return FALSE;
}

gboolean data_add_to_series(gint collection_id, gint count)
{
  sqlite3 *database;
  sqlite3_stmt *statement;
  gchar *data_file;

  g_print("collection_id: %d, count: %d\n", collection_id, count);

  data_file = collections_get_data_file();

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int res;
      const char *sqlStatement =
        g_strdup_printf("UPDATE collection "
                        " SET current_qty = current_qty + %d "
                        " WHERE id = %d", count, collection_id);

      res = sqlite3_prepare_v2(database,
                               sqlStatement,
                               strlen(sqlStatement),
                               &statement, NULL);
      if (res == SQLITE_OK) {
        if (sqlite3_step(statement) == SQLITE_DONE)
          return TRUE;
      }
    }
  }

  return FALSE;
}

static gboolean data_check_and_create_database(gchar *data_file)
{
  if (!access(data_file, R_OK) == 0)
    if (data_create_new_database(data_file)) {
      g_printerr("Couldn't create a new database\n");
      return FALSE; /* Couldn't create database, can't continue */
    }

  return TRUE;
}

static gint data_create_new_database(const gchar *filename)
{
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  if (access(collections_get_config_dir(), R_OK) == -1) {
    if (mkdir(collections_get_config_dir(), S_IRWXU) == -1)
      g_printerr("Can't create directory: %s\n", strerror(errno));
  }

  /* Open database connection, create file */
  rc = sqlite3_open(filename, &db);
  if (rc) {
    g_printerr("Can't open database: %s\n", sqlite3_errmsg(db));
    if (db)
      sqlite3_close(db);
    return -1;
  }

  /* Create collections table */
  rc = sqlite3_exec(db, "CREATE TABLE collection("
                    "id INTEGER PRIMARY KEY,"
                    "name VARCHAR(100),"
                    "current_qty INTEGER,"
                    "total_qty INTEGER"
                    ")", NULL, NULL, &zErrMsg);
  if (rc != SQLITE_OK) {
    g_printerr("Can't create collection table: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return -1;
  }

  /* Create items table */
  rc = sqlite3_exec(db, "CREATE TABLE item("
                    "collection_id INTEGER,"
                    "id INTEGER,"
                    "title VARCHAR(100),"
                    "PRIMARY KEY(collection_id, id),"
                    "FOREIGN KEY(collection_id)"
                    "  REFERENCES collection(id)"
                    ")", NULL, NULL, &zErrMsg);
  if (rc != SQLITE_OK) {
    g_printerr("Can't create item table: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return -1;
  }

  sqlite3_close(db);
  return 0;
}
