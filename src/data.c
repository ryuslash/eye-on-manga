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
static gboolean execute_non_query(const char *sql);
static GList *get_manga_for_query(const gchar*);
static Manga *get_manga_from_statement(sqlite3_stmt*);

gboolean
data_add_manga(const gchar *name, gint total_qty)
{
    gchar *sql =
        g_strdup_printf(" INSERT INTO manga (name, current_qty, "
                        "total_qty) VALUES ('%s', 0, %d)",
                        name, total_qty);
    gboolean ret = execute_non_query(sql);

    g_free(sql);

    return ret;
}

gboolean
data_add_to_manga(gint manga_id, gint count)
{
    gchar *sql =
        g_strdup_printf(" UPDATE manga "
                        " SET current_qty = current_qty + %d "
                        " WHERE id = %d", count, manga_id);
    gboolean ret = execute_non_query(sql);

    g_free(sql);

    return ret;
}

gboolean
data_add_volume_to_manga(gint manga_id, gint volume)
{
    char *sql = g_strdup_printf(" INSERT INTO volume "
                                " VALUES (%d, %d, 0) ",
                                manga_id, volume);
    gboolean ret = execute_non_query(sql);

    g_free(sql);

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
    sqlite3 *db;
    sqlite3_stmt *stmt;
    gchar *data_file;
    Manga *manga = NULL;

    data_file = eom_get_data_file();

    if (check_and_create_database(data_file)) {
        if (sqlite3_open(data_file, &db) == SQLITE_OK) {
            int res;
            char *sql =
                g_strdup_printf(" SELECT id, "
                                "        name, "
                                "        current_qty, "
                                "        total_qty "
                                " FROM   manga "
                                " WHERE  id = %d ", manga_id);

            res = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
            g_free(sql);

            if (res == SQLITE_OK) {
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    manga = get_manga_from_statement(stmt);
                }
            }

            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
    }

    return manga;
}

void
data_get_volumes_for_manga(Manga *manga)
{
    gint count;
    Volume *volumes;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    gchar *data_file;

    data_file = eom_get_data_file();
    count = 0;

    if (check_and_create_database(data_file)) {
        if (sqlite3_open(data_file, &db) == SQLITE_OK) {
            int res;
            char *sql = g_strdup_printf(" SELECT COUNT(id) "
                                        " FROM   volume "
                                        " WHERE  manga_id = %d ",
                                        manga->id);

            res = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
            g_free(sql);

            if (res == SQLITE_OK) {
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    count = sqlite3_column_int(stmt, 0);
                }
            }

            sqlite3_finalize(stmt);
            volumes = calloc(sizeof(Volume), count);

            if (count > 0) {
                sql = g_strdup_printf(" SELECT id, "
                                      "        read "
                                      " FROM   volume "
                                      " WHERE  manga_id = %d ",
                                      manga->id);

                res = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt,
                                         NULL);
                g_free(sql);

                if (res == SQLITE_OK) {
                    gint i = 0;
                    while (sqlite3_step(stmt) == SQLITE_ROW) {
                        volumes[i].number = sqlite3_column_int(stmt, 0);
                        volumes[i].read   = sqlite3_column_int(stmt, 1);

                        i++;
                    };
                }

                sqlite3_finalize(stmt);
            }
        }
        sqlite3_close(db);
    }

    manga->vol_count = count;
    manga->volumes = volumes;
}

gboolean
data_mark_volume_read(int read, gint manga_id, gint volume)
{
    gchar *sql = g_strdup_printf(" UPDATE volume "
                                 " SET    read = %d "
                                 " WHERE  manga_id = %d "
                                 " AND    id = %d ",
                                 read, manga_id, volume);
    gboolean ret = execute_non_query(sql);

    g_free(sql);

    return ret;
}

gboolean
data_remove_volume_from_manga(gint manga_id, gint volume)
{
    char *sql = g_strdup_printf(" DELETE FROM volume "
                                " WHERE manga_id = %d "
                                " AND id = %d ", manga_id, volume);
    gboolean ret = execute_non_query(sql);

    g_free(sql);

    return ret;
}

gboolean
data_update_manga(gint manga_id, const gchar *name, gint total_qty)
{
    gchar *sql =
        g_strdup_printf("UPDATE manga SET name = '%s', total_qty = %d "
                        "WHERE id = %d", name, total_qty, manga_id);
    gboolean ret = execute_non_query(sql);

    g_free(sql);

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
execute_non_query(const gchar *sql)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    gchar *data_file;
    gboolean result;

    result = FALSE;
    data_file = eom_get_data_file();

    if (check_and_create_database(data_file)) {
        if (sqlite3_open(data_file, &db) == SQLITE_OK) {
            int res =
                sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);

            if (res == SQLITE_OK) {
                if (sqlite3_step(stmt) == SQLITE_DONE)
                    result = TRUE;
            }

            sqlite3_finalize(stmt);
        }
        sqlite3_close(db);
    }

    return result;
}

static GList *
get_manga_for_query(const gchar *query)
{
    sqlite3 *database;
    sqlite3_stmt *statement;
    gchar *data_file;
    GList *list = NULL;

    data_file = eom_get_data_file();

    if (check_and_create_database(data_file)) {
        if (sqlite3_open(data_file, &database) == SQLITE_OK) {
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
        }
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
