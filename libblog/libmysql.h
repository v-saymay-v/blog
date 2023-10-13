#ifndef _LIBMYSQL_H__
#define _LIBMYSQL_H__

#include <mysql.h>

#define DBase			MyDBase
#define DBRes			MyDBRes
#define OldDBase(db)	(void*)db
#define OldDBRes(dbres)	(void*)dbres->res

#define CO_SQL_OK		0
#define CO_SQL_NODATA	1
#define CO_SQL_ERR		-1

#define Gcha_last_error		g_my_cha_last_error

#define Db_ConnectWithParam	My_Db_Connect
#define Db_ConnectSSL		My_Db_Connect_SSL
#define Db_Disconnect		My_Db_Disconnect
#define Db_ExecSql			My_Db_ExecSql
#define Db_OpenDyna			My_Db_OpenDyna
#define Db_CloseDyna		My_Db_CloseDyna
#define Db_GetName			My_Db_GetName
#define Db_GetValue			My_Db_GetValue
#define Db_GetValue_Fast	My_Db_GetValue
#define Db_FetchNext		My_Db_FetchNext
#define Db_FetchValue		My_Db_FetchValue
#define Db_RowSeek			My_Db_RowSeek
#define Db_RowTell			My_Db_RowTell
#define Db_GetInsertId		My_Db_GetInsertId
#define Db_GetAffectedRows	My_Db_GetAffectedRows
#define Db_GetRowCount		My_Db_GetRowCount
#define Db_GetColCount		My_Db_GetColCount
#define Begin_Transact		My_Begin_Transact
#define Commit_Transact		My_Commit_Transact
#define Rollback_Transact	My_Rollback_Transact
#define Db_GetColumnSize	My_Db_GetColumnSize

typedef MYSQL MyDBase;
typedef struct _MyDBRes {
	MYSQL		*db;
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	int			off;
} MyDBRes;
typedef MYSQL_ROW_OFFSET MyDBOff;

extern char g_my_cha_last_error[1024];

MyDBase *My_Db_Connect(char *chp_server, char *chp_database, char *chp_login, char *chp_pass);
MyDBase *My_Db_Connect_SSL(char *chp_server, char *chp_database, char *chp_login, char *chp_pass);
void My_Db_Disconnect(MyDBase *db);
int My_Db_ExecSql(MyDBase *db, char *chp_sql);
MyDBRes *My_Db_OpenDyna(MyDBase *db, char *chp_sql);
void My_Db_CloseDyna(MyDBRes *dbres);
char *My_Db_GetName(MyDBRes *dbres, int in_col);
char *My_Db_GetValue(MyDBRes *dbres, int in_row, int in_col);
char *My_Db_FixGetValue(MyDBRes *dbres, int in_row, int in_col);
int My_Db_FetchNext(MyDBRes *dbres);
char *My_Db_FetchValue(MyDBRes *dbres, int in_col);
unsigned int My_Db_FetchLength(MyDBRes *dbres, int in_col);
void My_Db_RowSeek(MyDBRes *dbres, MyDBOff off);
MyDBOff My_Db_RowTell(MyDBRes *dbres);
int My_Db_GetInsertId(MyDBase *db);
int My_Db_GetAffectedRows(MyDBase *db);
int My_Db_GetRowCount(MyDBRes *dbres);
int My_Db_GetColCount(MyDBRes *dbres);
int My_Begin_Transact(MyDBase *db);
int My_Commit_Transact(MyDBase *db);
int My_Rollback_Transact(MyDBase *db);
int My_Db_GetColumnSize(MyDBase *db, char *chp_table_name, char *chp_column_name);
unsigned long My_Db_EscapeString(MyDBase *db, char *to, const char *from, unsigned long length);
char *My_Escape_SqlString(MyDBase *db, const char *from);

#endif
