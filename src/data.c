/* Eye on Manga - Manga collection software for Maemo 5
   Copyright (C) 2010-2013  Tom Willemse

   Eye on Manga is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Eye on Manga is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Eye on Manga.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "data.h"

#include <errno.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "eom.h"

static gboolean check_and_create_database(gchar*);
static gint create_new_database(const gchar*);
static gboolean execute_non_query(const char *sql,
                                  int (*bind)(sqlite3_stmt*));
static sqlite3 *get_database(void);
static GList *get_manga_for_query(const gchar*);
static Manga *get_manga_from_statement(sqlite3_stmt*);

gboolean
data_add_manga(const gchar *name, gint total_qty)
{
    gboolean result = FALSE;
    char sql[] = "INSERT INTO manga (name, current_qty, total_qty) "
        "VALUES (?, 0, ?)";

    int bind_variables(sqlite3_stmt *stmt) {
        return (sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC)
                == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 2, total_qty) == SQLITE_OK);
    }

    return execute_non_query(sql, bind_variables);
}

gboolean
data_add_to_manga(gint manga_id, gint count)
{
    char sql[] = "UPDATE manga "
        "SET current_qty = current_qty + ? "
        "WHERE id = ?";

    int bind_variables(sqlite3_stmt *stmt) {
        return (sqlite3_bind_int(stmt, 1, count) == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 2, manga_id) == SQLITE_OK);
    }

    return execute_non_query(sql, bind_variables);
}

gboolean
data_add_volume_to_manga(gint manga_id, gint volume)
{
    char sql[] = "INSERT INTO volume "
        "VALUES (?, ?, 0)";

    int bind_variables(sqlite3_stmt *stmt) {
        return (sqlite3_bind_int(stmt, 1, manga_id) == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 2, volume) == SQLITE_OK);
    }

    return execute_non_query(sql, bind_variables);
}

gboolean
data_delete_manga(gint manga_id)
{
    gboolean ret;

    char *sql = "DELETE FROM volume "
        "WHERE manga_id = ?";

    int bind_variables(sqlite3_stmt *stmt) {
        return sqlite3_bind_int(stmt, 1, manga_id) == SQLITE_OK;
    }

    ret = execute_non_query(sql, bind_variables);

    if (ret) {
        sql = "DELETE FROM manga "
            "WHERE id = ?";

        ret = execute_non_query(sql, bind_variables);
    }

    return ret;
}

GList *
data_get_incomplete_manga(void)
{
    const char *sql =
        " SELECT   id, "
        "          name, "
        "          current_qty, "
        "          total_qty "
        " FROM     manga "
        " WHERE    current_qty != total_qty "
        " ORDER BY name "
        " COLLATE NOCASE ";

    return get_manga_for_query(sql);
}

GList *
data_get_manga(void)
{
    const char *sql =
        " SELECT   id, "
        "          name, "
        "          current_qty, "
        "          total_qty "
        " FROM     manga "
        " ORDER BY name "
        " COLLATE NOCASE ";

    return get_manga_for_query(sql);
}

Manga *
data_get_manga_by_id(gint manga_id)
{
    sqlite3 *db = get_database();
    sqlite3_stmt *stmt;
    Manga *manga = NULL;

    if (db) {
        int res;
        char *sql =
            "SELECT id, name, current_qty, total_qty "
            "FROM manga "
            "WHERE id = ?";

        res = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

        if (res == SQLITE_OK
            && (sqlite3_bind_int(stmt, 1, manga_id) == SQLITE_OK)
            && sqlite3_step(stmt) == SQLITE_ROW)
            manga = get_manga_from_statement(stmt);

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    return manga;
}

GList *
data_get_unread_manga(void)
{
    const char *sql =
        " SELECT   m.id, "
        "          m.name, "
        "          m.current_qty, "
        "          m.total_qty "
        " FROM     manga m "
        " JOIN     volume ON (m.id = manga_id) "
        " WHERE    read = 0 "
        " GROUP BY m.id "
        " ORDER BY name "
        " COLLATE NOCASE ";

    return get_manga_for_query(sql);
}

void
data_get_volumes_for_manga(Manga *manga)
{
    gint count;
    Volume *volumes = NULL;
    sqlite3 *db = get_database();
    sqlite3_stmt *stmt;

    count = 0;

    if (db) {
        int res;
        char *sql =
            "SELECT COUNT(id) "
            "FROM volume "
            "WHERE manga_id = ?";

        res = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

        if (res == SQLITE_OK
            && (sqlite3_bind_int(stmt, 1, manga->id) == SQLITE_OK)
            && sqlite3_step(stmt) == SQLITE_ROW)
            count = sqlite3_column_int(stmt, 0);

        sqlite3_finalize(stmt);
        volumes = calloc(sizeof(Volume), count);

        if (count > 0) {
            sql =
                "SELECT id, read "
                "FROM volume "
                "WHERE manga_id = ?";

            res = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

            if (res == SQLITE_OK
                && (sqlite3_bind_int(stmt, 1, manga->id) == SQLITE_OK)) {
                gint i = 0;
                while (sqlite3_step(stmt) == SQLITE_ROW) {
                    volumes[i].number = sqlite3_column_int(stmt, 0);
                    volumes[i].read   = sqlite3_column_int(stmt, 1);

                    i++;
                };
            }

            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
    }

    manga->vol_count = count;
    manga->volumes = volumes;
}

gboolean
data_mark_volume_read(int read, gint manga_id, gint volume)
{
    char sql[] = "UPDATE volume "
        "SET read = ? "
        "WHERE manga_id = ? "
        "AND id = ?";

    int bind_variables(sqlite3_stmt *stmt) {
        return (sqlite3_bind_int(stmt, 1, read) == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 2, manga_id) == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 3, volume) == SQLITE_OK);
    }

    return execute_non_query(sql, bind_variables);
}

gboolean
data_remove_volume_from_manga(gint manga_id, gint volume)
{
    char sql[] = "DELETE FROM volume "
        "WHERE manga_id = ? "
        "AND id = ?";

    int bind_variables(sqlite3_stmt *stmt) {
        return (sqlite3_bind_int(stmt, 1, manga_id) == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 2, volume) == SQLITE_OK);
    }

    return execute_non_query(sql, bind_variables);
}

gboolean
data_update_manga(gint manga_id, const gchar *name, gint total_qty)
{
    gboolean ret;
    char *sql = "UPDATE manga "
        "SET name = ?001, total_qty = ?002, "
        " current_qty = MIN(current_qty, ?002) "
        "WHERE id = ?003";

    int bind_variables(sqlite3_stmt *stmt) {
        return (sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC) == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 2, total_qty) == SQLITE_OK)
            && (sqlite3_bind_int(stmt, 3, manga_id) == SQLITE_OK);
    }

    ret = execute_non_query(sql, bind_variables);

    if (ret) {
        sql = "DELETE FROM volume "
            "WHERE manga_id = ?"
            "AND id > ?";

        int bind_variables(sqlite3_stmt *stmt) {
            return (sqlite3_bind_int(stmt, 1, manga_id) == SQLITE_OK)
                && (sqlite3_bind_int(stmt, 2, total_qty) == SQLITE_OK);
        }

        ret = execute_non_query(sql, bind_variables);
    }

    return ret;
}

static gboolean
check_and_create_database(gchar *data_file)
{
  if (!access(data_file, R_OK) == 0)
    if (create_new_database(data_file)) {
      g_printerr("Couldn't create a new database\n");

      return FALSE; /* Couldn't create database, can't continue */
    }

  return TRUE;
}

static gint
create_new_database(const gchar *filename)
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
    rc = sqlite3_exec(db,
                      "CREATE TABLE manga("
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
                      " manga_id INTEGER, "
                      " id INTEGER, "
                      " read INTEGER, "
                      " PRIMARY KEY(manga_id, id), "
                      " FOREIGN KEY(manga_id) "
                      "   REFERENCES manga(id)) ",
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

static gboolean
execute_non_query(const gchar *sql, int (*bind)(sqlite3_stmt *))
{
    sqlite3 *db = get_database();
    sqlite3_stmt *stmt;
    gboolean result;

    result = FALSE;

    if (db) {
        int res =
            sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

        result = (res == SQLITE_OK
                  && (!bind || bind(stmt))
                  && sqlite3_step(stmt) == SQLITE_DONE);

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    return result;
}

static sqlite3 *
get_database(void)
{
    sqlite3 *db;
    gchar *data_file = eom_get_data_file();

    if (check_and_create_database(data_file)) {
        if (sqlite3_open(data_file, &db) == SQLITE_OK)
            return db;
        else
            sqlite3_close(db);
    }

    return NULL;
}

static GList *
get_manga_for_query(const gchar *query)
{
    sqlite3 *database = get_database();
    sqlite3_stmt *statement;
    GList *list = NULL;

    if (database) {
        int res = sqlite3_prepare_v2(database, query, strlen(query),
                                     &statement, NULL);

        if (res == SQLITE_OK) {
            while (sqlite3_step(statement) == SQLITE_ROW) {
                Manga *manga = get_manga_from_statement(statement);
                list = g_list_append(list, (gpointer)manga);
            }
        }
        else
            g_print("error %d: %s\n", res, sqlite3_errmsg(database));

        /* Release the compiled statement from memory */
        sqlite3_finalize(statement);
        sqlite3_close(database);
    }

    return list;
}

static Manga *
get_manga_from_statement(sqlite3_stmt *stmt)
{
  Manga *manga = (Manga *)malloc(sizeof(Manga));

  manga->id = sqlite3_column_int(stmt, 0);
  manga->name = g_strdup(sqlite3_column_text(stmt, 1));
  manga->current_qty = sqlite3_column_int(stmt, 2);
  manga->total_qty = sqlite3_column_int(stmt, 3);
  manga->volumes = NULL;
  manga->vol_count = 0;

  return manga;
}
