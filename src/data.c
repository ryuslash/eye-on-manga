#include "data.h"
#include <sqlite3.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "eom.h"

static gboolean  data_check_and_create_database(gchar *data_file);
static gint      data_create_new_database(const gchar *filename);
static Manga    *data_get_manga_from_statement(sqlite3_stmt *stmt);

GList *data_get_manga(void)
{
  sqlite3      *database;
  sqlite3_stmt *statement;
  gchar        *data_file;
  GList        *list = NULL;

  data_file = eom_get_data_file();

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int         res;
      const char *sql =
        " SELECT   id,          "
        "          name,        "
        "          current_qty, "
        "          total_qty    "
        " FROM     manga        "
        " ORDER BY name         "
        " COLLATE NOCASE        ";

      res = sqlite3_prepare_v2(database,
                               sql,
                               strlen(sql),
                               &statement, NULL);
      if (res == SQLITE_OK) {
        while (sqlite3_step(statement) == SQLITE_ROW) {
          Manga *manga = data_get_manga_from_statement(statement);
          list = g_list_append(list, (gpointer)manga);
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

Manga *data_get_manga_by_id(gint manga_id)
{
  sqlite3      *database;
  sqlite3_stmt *statement;
  gchar        *data_file;
  Manga        *manga = NULL;

  data_file = eom_get_data_file();

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int         res;
      const char *sql = g_strdup_printf(
        " SELECT id,          "
        "        name,        "
        "        current_qty, "
        "        total_qty    "
        " FROM   manga        "
        " WHERE  id = %d      ", manga_id);

      res = sqlite3_prepare_v2(database, sql, strlen(sql), &statement, NULL);
      if (res == SQLITE_OK) {
        if (sqlite3_step(statement) == SQLITE_ROW) {
          manga = data_get_manga_from_statement(statement);
        }
      }
      sqlite3_finalize(statement);
    }
    sqlite3_close(database);
  }

  return manga;
}

gint *data_get_volumes_by_manga_id(gint manga_id)
{
  gint          count;
  gint         *volumes;
  sqlite3      *database;
  sqlite3_stmt *statement;
  gchar        *data_file;

  data_file = eom_get_data_file();
  count = 0;

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int res;
      const char *sql = g_strdup_printf(
        " SELECT COUNT(number)      "
        " FROM   volume             "
        " WHERE  collection_id = %d ", manga_id);

      res = sqlite3_prepare_v2(database, sql, strlen(sql), &statement, NULL);
      if (res == SQLITE_OK) {
        if (sqlite3_step(statement) == SQLITE_ROW) {
          count = sqlite3_column_int(statement, 0);
        }
      }
      sqlite3_finalize(statement);

      if (count > 0) {
        sql = g_strdup_printf(
          " SELECT number            "
          " FROM volume              "
          " WHERE collection_id = %d ", manga_id);

        res = sqlite3_prepare_v2(database, sql, strlen(sql), &statement, NULL);
        if (res == SQLITE_OK) {
          gint i = 0;
          while (sqlite3_step(statement) == SQLITE_ROW) {
            volumes[i++] = sqlite3_column_int(statement, 0);
          }
        }
      }
    }
    sqlite3_close(database);
  }

  return volumes;
}

gboolean data_add_manga(gchar *name, gint total_qty)
{
  sqlite3      *database;
  sqlite3_stmt *statement;
  gchar        *data_file;
  gboolean      result;

  result = FALSE;
  data_file = eom_get_data_file();

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int         res;
      const char *sql = g_strdup_printf(
        " INSERT INTO manga (name, current_qty, total_qty) "
        " VALUES ('%s', 0, %d)", name, total_qty);

      res = sqlite3_prepare_v2(database,
                               sql,
                               strlen(sql),
                               &statement, NULL);
      if (res == SQLITE_OK) {
        if (sqlite3_step(statement) == SQLITE_DONE)
          result = TRUE;
      }
      sqlite3_finalize(statement);
    }
    sqlite3_close(database);
  }

  return result;
}

gboolean data_add_to_manga(gint manga_id, gint count)
{
  sqlite3      *database;
  sqlite3_stmt *statement;
  gchar        *data_file;
  gboolean      result;

  data_file = eom_get_data_file();
  result = FALSE;

  if (data_check_and_create_database(data_file)) {
    if (sqlite3_open(data_file, &database) == SQLITE_OK) {
      int         res;
      const char *sql = g_strdup_printf(
        " UPDATE manga "
        " SET current_qty = current_qty + %d "
        " WHERE id = %d", count, manga_id);

      res = sqlite3_prepare_v2(database,
                               sql,
                               strlen(sql),
                               &statement, NULL);
      if (res == SQLITE_OK) {
        if (sqlite3_step(statement) == SQLITE_DONE)
          result = TRUE;
      }
      sqlite3_finalize(statement);
    }
    sqlite3_close(database);
  }

  return result;
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

  if (access(eom_get_config_dir(), R_OK) == -1) {
    if (mkdir(eom_get_config_dir(), S_IRWXU) == -1)
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
  rc = sqlite3_exec(db, "CREATE TABLE manga("
                    "id INTEGER PRIMARY KEY,"
                    "name VARCHAR(100),"
                    "current_qty INTEGER,"
                    "total_qty INTEGER"
                    ")", NULL, NULL, &zErrMsg);
  if (rc != SQLITE_OK) {
    g_printerr("Can't create manga table: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return -1;
  }

  /* Create items table */
  rc = sqlite3_exec(db,
                    " CREATE TABLE volume( "
                    " collection_id INTEGER, "
                    " id INTEGER, "
                    " PRIMARY KEY(collection_id, id), "
                    " FOREIGN KEY(collection_id) "
                    "   REFERENCES collection(id)) ",
                    NULL, NULL, &zErrMsg);
  if (rc != SQLITE_OK) {
    g_printerr("Can't create volume table: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return -1;
  }

  sqlite3_close(db);
  return 0;
}

static Manga *data_get_manga_from_statement(sqlite3_stmt *stmt)
{
  Manga *manga =
    (Manga *)malloc(sizeof(Manga));

  manga->id = sqlite3_column_int(stmt, 0);
  manga->name = g_strdup(sqlite3_column_text(stmt, 1));
  manga->current_qty = sqlite3_column_int(stmt, 2);
  manga->total_qty = sqlite3_column_int(stmt, 3);

  return manga;
}
