/*
+* ------------------------------------------------------------------------
 * Module-Name:   libmysql.c
 * First-Created: 2005/08/24 �١�����
%* ------------------------------------------------------------------------
 * Module-Description:
 *    MySQL���ò������ǡ����١������饤�֥��
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmysql.h"

char g_my_cha_last_error[1024] = {'\0'};

/*
+* ------------------------------------------------------------------------
 * Function: set_LastError
 * Description:
 *     ���顼��å��������������
%* ------------------------------------------------------------------------
 * Return:
 *     �ʤ�
-* ----------------------------------------------------------------------*/
static void Set_Last_Error(char *chp_errmsg)
{
	int in_len;

	g_my_cha_last_error[0] = '\0';
	if (!chp_errmsg) {
		return;
	}
	in_len = strlen(chp_errmsg);
	if (in_len >= sizeof(g_my_cha_last_error)) {
		in_len = sizeof(g_my_cha_last_error)-1;
	}
	strncpy(g_my_cha_last_error, chp_errmsg, sizeof(g_my_cha_last_error)-1);
	g_my_cha_last_error[in_len] = '\0';
}

/*
+* ========================================================================
 * Function:     My_Db_Connect
 * Include-file: libdb2.h
 * Description:
 *     �����С����ǡ����١���̾�������󡢥ѥ���ɤ���ꤷ
 *     �ǡ����١�������³����
%* ========================================================================
 * Return:
 *     DBase���ݥ��� :���ｪλ
 *                NULL :���顼
-* ======================================================================*/
MyDBase *My_Db_Connect(char *chp_server, char *chp_database, char *chp_login, char *chp_pass)
{
	MyDBase *db;

	db = (MyDBase*)mysql_init(NULL);
	if (!db) {
		Set_Last_Error("mysql_init failed.");
		return NULL;
	}
	if (!mysql_real_connect((MYSQL*)db, chp_server, chp_login, chp_pass, chp_database, 0, NULL, 0)) {
		Set_Last_Error((char*)mysql_error((MYSQL*)db));
		return NULL;
	}
//	mysql_real_query((MYSQL*)db, "SET AUTOCOMMIT=0;", strlen("SET AUTOCOMMIT=0;"));

	return db;
}

MyDBase *My_Db_Connect_SSL(char *chp_server, char *chp_database, char *chp_login, char *chp_pass)
{
	MyDBase *db = NULL;
	int in_ret;

	db = (MyDBase*)mysql_init(NULL);
	if (!db) {
		Set_Last_Error("mysql_init failed.");
		return NULL;
	}
	in_ret = mysql_options((MYSQL*)db, MYSQL_OPT_LOCAL_INFILE, 0);
	if(in_ret != CO_SQL_OK) {
		Set_Last_Error((char*)mysql_error((MYSQL*)db));
		return NULL;
	}
	mysql_ssl_set((MYSQL*)db, NULL, NULL, NULL, NULL, NULL);
	if (!mysql_real_connect((MYSQL*)db, chp_server, chp_login, chp_pass, chp_database, 0, NULL, CLIENT_SSL)) {
		Set_Last_Error((char*)mysql_error((MYSQL*)db));
		return NULL;
	}
	return db;
}

/*
+* ========================================================================
 * Function:     Db_Disconnect
 * Include-file: libdb2.h
 * Description:
 *     �ǡ����١�����³���Ĥ���
%* ========================================================================
 * Return:
 *     �ʤ�
-* ======================================================================*/
void My_Db_Disconnect(MyDBase *db)
{
	if(db) {
		mysql_close((MYSQL*)db);
	}
}

/*
+* ========================================================================
 * Function:     Db_ExecSql
 * Include-file: libdb2.h
 * Description:
 *     �����꡼��¹Ԥ��롣(�����꡼�η�̤������ʤ�)
%* ========================================================================
 * Return:
 *     CO_SQL_OK  :���ｪλ
 *     CO_SQL_ERR :���顼
-* ======================================================================*/
int My_Db_ExecSql(MyDBase *db, char *chp_sql)
{
	int in_ret;

	if(!chp_sql) {
		return CO_SQL_ERR;
	}
	in_ret = mysql_real_query((MYSQL*)db, chp_sql, strlen(chp_sql));
	if(in_ret == CO_SQL_OK) {
		return CO_SQL_OK;
	} else {
		Set_Last_Error((char*)mysql_error((MYSQL*)db));
		return CO_SQL_ERR;
	}
}

/*
+* ========================================================================
 * Function:     Db_OpenDyna
 * Include-file: libdb2.h
 * Description:
 *     �����꡼��¹Ԥ��롣(�����꡼�η�̤�������)
%* ========================================================================
 * Return:
 *     DBRes���ݥ��� :���ｪλ
 *     CO_SQL_ERR      :���顼
-* ======================================================================*/
MyDBRes *My_Db_OpenDyna(MyDBase *db, char *chp_sql)
{
	MyDBRes* my_res;
	MYSQL_RES *result;
	int in_ret;

	my_res = NULL;
	if (!chp_sql) {
		return NULL;
	}

	in_ret = mysql_real_query((MYSQL*)db, chp_sql, strlen(chp_sql));
	if (!in_ret) {
		result = mysql_store_result((MYSQL*)db);
		if (!result) {
			Set_Last_Error((char*)mysql_error((MYSQL*)db));
		} else {
			my_res = (MyDBRes*)malloc(sizeof(MyDBRes));
			my_res->db = (MYSQL*)db;
			my_res->res = result;
			my_res->row = NULL;
			my_res->off = -1;
		}
	} else {
		Set_Last_Error((char*)mysql_error((MYSQL*)db));
	}

	return my_res;
}

/*
+* ========================================================================
 * Function:     Db_CloseDyna
 * Include-file: libdb2.h
 * Description:
 *     Db_OpenDyna������줿�����꡼�η�̤��˴�����
%* ========================================================================
 * Return:
 *     �ʤ�
-* ======================================================================*/
void My_Db_CloseDyna(MyDBRes *dbres)
{
	mysql_free_result(dbres->res);
	free(dbres);
}

/*
+* ========================================================================
 * Function:     Db_GetName
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤���in_col���ܤΥ����̾���������
%* ========================================================================
 * Return:
 *     char���ݥ���
-* ======================================================================*/
char *My_Db_GetName(MyDBRes *dbres, int in_col)
{
	MYSQL_FIELD *field;
	field = mysql_fetch_field_direct(dbres->res, in_col);
	if (!field) {
		Set_Last_Error((char*)mysql_error(dbres->db));
		return NULL;
	}
	return field->name;
}

/*
+* ========================================================================
 * Function:     Db_GetValue_Fast
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤���in_row,in_col���ܤ��ͤ��������
 *     MySQL�Ǥ�®�٤���夵�������
%* ========================================================================
 * Return:
 *     char���ݥ���
-* ======================================================================*/
char *My_Db_GetValue(MyDBRes *dbres, int in_row, int in_col)
{
	if (in_row != dbres->off) {
		if (in_row != dbres->off + 1) {
			mysql_data_seek(dbres->res, in_row);
		}
		dbres->row = mysql_fetch_row(dbres->res);
		if (!dbres->row) {
			Set_Last_Error((char*)mysql_error(dbres->db));
			return NULL;
		}
		dbres->off = in_row;
	} else if (!dbres->row) {
		return NULL;
	}

	return dbres->row[in_col];
}

/*
+* ========================================================================
 * Function:     Db_GetValue_Fast
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤���in_row,in_col���ܤ��ͤ��������
 *     MySQL�Ǥ�®�٤���夵�������
%* ========================================================================
 * Return:
 *     char���ݥ���
-* ======================================================================*/
char *My_Db_FixGetValue(MyDBRes *dbres, int in_row, int in_col)
{
	MYSQL_ROW row;
	if(mysql_num_rows(dbres->res) == 0)
		return NULL;
	if(in_row > mysql_num_rows(dbres->res) - 1)
		return NULL;
	mysql_data_seek(dbres->res, in_row);
	row = mysql_fetch_row(dbres->res);
	if (!row) {
		Set_Last_Error((char*)mysql_error(dbres->db));
		return NULL;
	}
	return row[in_col];
}

/*
+* ========================================================================
 * Function:     Db_FetchNext
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤���μ��Υ쥳���ɤ�fetch����
%* ========================================================================
 * Return:
 *     char���ݥ���
-* ======================================================================*/
int My_Db_FetchNext(MyDBRes *dbres)
{
	dbres->row = mysql_fetch_row(dbres->res);
	if (!dbres->row) {
		Set_Last_Error((char*)mysql_error(dbres->db));
		return CO_SQL_NODATA;
	}

	return CO_SQL_OK;
}

/*
+* ========================================================================
 * Function:     Db_FetchValue
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤���in_col���ܤ��ͤ��������
%* ========================================================================
 * Return:
 *     char���ݥ���
-* ======================================================================*/
char *My_Db_FetchValue(MyDBRes *dbres, int in_col)
{
	return dbres->row[in_col];
}

/*
+* ========================================================================
 * Function:     Db_GetLength
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤���in_row,in_col���ܤ��ͤ�Ĺ�����������
%* ========================================================================
 * Return:
 *     char���ݥ���
-* ======================================================================*/
unsigned int My_Db_FetchLength(MyDBRes *dbres, int in_col)
{
	unsigned long *l = mysql_fetch_lengths(dbres->res);
	return (unsigned int)l[in_col];
}

/*
+* ========================================================================
 * Function:     My_Db_RowSeek
 * Include-file: libdb2.h
 * Description:
 *      �������̥��åȤ�Ǥ�դΥ쥳���ɤ˥�������������ꤷ�ޤ���
%* ========================================================================
 * Return:
 *     �ʤ�
-* ======================================================================*/
void My_Db_RowSeek(MyDBRes *dbres, MyDBOff row)
{
	mysql_row_seek(dbres->res, row);
}

/*
+* ========================================================================
 * Function:     My_Db_RowSeek
 * Include-file: libdb2.h
 * Description:
 *      ���ߤΥ������̥��åȤΥ쥳���ɤΥ�����������֤�������ޤ���
%* ========================================================================
 * Return:
 *     �������������
-* ======================================================================*/
MyDBOff My_Db_RowTell(MyDBRes *dbres)
{
	return mysql_row_tell(dbres->res);
}

/*
+* ========================================================================
 * Function:     Db_GetRowCount
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤Υǡ����������
%* ========================================================================
 * Return:
 *     int(�ǡ�����)
-* ======================================================================*/
int My_Db_GetRowCount(MyDBRes *dbres)
{
	return mysql_num_rows(dbres->res);
}

/*
+* ========================================================================
 * Function:     Db_GetColCount
 * Include-file: libdb2.h
 * Description:
 *     �����꡼�η�̤Υ����������
%* ========================================================================
 * Return:
 *	int(������)
-* ======================================================================*/
int My_Db_GetColCount(MyDBRes *dbres)
{
	return mysql_num_fields(dbres->res);
}

/*
+* ========================================================================
 * Function:     Db_GetInsertId
 * Include-file: libdb2.h
 * Description:
 *     Autoincrement�ʥ����˺Ǹ�˥��󥵡��Ȥ����祭�����ͤ�������롣
%* ========================================================================
 * Return:
 *	int(������)
-* ======================================================================*/
int My_Db_GetInsertId(MyDBase *db)
{
	return (int)mysql_insert_id((MYSQL*)db);
}

/*
+* ========================================================================
 * Function:     Db_GetAffectedRows
 * Include-file: libdb2.h
 * Description:
 *     �Ǹ�˼¹Ԥ�����������ѹ��Τ��ä��쥳���ɿ���������롣
%* ========================================================================
 * Return:
 *	int(������)
-* ======================================================================*/
int My_Db_GetAffectedRows(MyDBase *db)
{
	return (int)mysql_affected_rows((MYSQL*)db);
}

/*
+* ========================================================================
 * Function:     Begin_Transact
 * Include-file: libdb2.h
 * Description:
 *     �ȥ�󥶥����������򳫻Ϥ���
%* ========================================================================
 * Return:
 *     CO_SQL_OK  :���ｪλ
 *     CO_SQL_ERR :���顼
-* ======================================================================*/
int My_Begin_Transact(MyDBase *db)
{
	return My_Db_ExecSql(db, "begin");
}

/*
+* ========================================================================
 * Function:     Commit_Transact
 * Include-file: libdb2.h
 * Description:
 *     �ȥ�󥶥����������򥳥ߥåȤ���
%* ========================================================================
 * Return:
 *     CO_SQL_OK  :���ｪλ
 *     CO_SQL_ERR :���顼
-* ======================================================================*/
int My_Commit_Transact(MyDBase *db)
{
	return My_Db_ExecSql(db, "commit");
}

/*
+* ========================================================================
 * Function:     Rollback_Transact
 * Include-file: libdb2.h
 * Description:
 *     �ȥ�󥶥��������������Хå�����
%* ========================================================================
 * Return:
 *     CO_SQL_OK  :���ｪλ
 *     CO_SQL_ERR :���顼
-* ======================================================================*/
int My_Rollback_Transact(MyDBase *db)
{
	return My_Db_ExecSql(db, "rollback");
}


/*
+* ========================================================================
 * Function:	My_Db_GetColumnSize
 * Description:
 *
%* ========================================================================
 * Return:	0�ʲ� �����꡼���顼
 *		0�ʾ� MAXLENGTH
-* ======================================================================*/
int My_Db_GetColumnSize(MyDBase *db, char *chp_table_name, char *chp_column_name)
{
	MyDBRes *dbres;
	char *chp_val;
	char cha_sql[128];
	char cha_buf[20];
	int i;

	if (!chp_table_name || !chp_table_name[0] || !chp_column_name || !chp_column_name[0])
		return 0;

	sprintf(cha_sql, "SHOW COLUMNS FROM %s LIKE '%s'", chp_table_name, chp_column_name);
	dbres = My_Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		return -1;
	}
	if (My_Db_FetchNext(dbres) != CO_SQL_OK) {
		My_Db_CloseDyna(dbres);
		return 0;
	}

	chp_val = My_Db_FetchValue(dbres, 1);
	if (!chp_val) {
		My_Db_CloseDyna(dbres);
		return 0;
	}
	if (!strncmp(chp_val, "varchar", 7)) {
		if (strlen(chp_val) < 10) {
			My_Db_CloseDyna(dbres);
			return 0;
		}
	} else if (!strncmp(chp_val, "tinyint", 7)) {
		 if (strlen(chp_val) < 10) {
			My_Db_CloseDyna(dbres);
			return 4;
		 }
	} else if (!strncmp(chp_val, "smallint", 8)) {
		 if (strlen(chp_val) < 11) {
			My_Db_CloseDyna(dbres);
			return 6;
		 }
	} else if (!strncmp(chp_val, "mediumint", 9)) {
		 if (strlen(chp_val) < 12) {
			My_Db_CloseDyna(dbres);
			return 8;
		 }
	} else if (!strncmp(chp_val, "int", 3)) {
		 if (strlen(chp_val) < 6) {
			My_Db_CloseDyna(dbres);
			return 11;
		 }
	} else if (!strncmp(chp_val, "bigint", 6)) {
		 if (strlen(chp_val) < 9) {
			My_Db_CloseDyna(dbres);
			return 20;
		 }
	} else if (!strcmp(chp_val, "text")) {
		My_Db_CloseDyna(dbres);
		return 8192;
	} else {
		My_Db_CloseDyna(dbres);
		return 0;
	}
	while (*chp_val != '(') {
		chp_val++;
	}
	if (!*chp_val) {
		My_Db_CloseDyna(dbres);
		return 0;
	}
	chp_val++;

	memset(cha_buf, '\0', sizeof(cha_buf));
	for (i = 0; *chp_val && *chp_val != ')'; ++i) {
		cha_buf[i] = *chp_val++;
	}
	My_Db_CloseDyna(dbres);

	return atoi(cha_buf);
}

/*
+* ========================================================================
 * Function:	My_Db_EscapeString
 * Description:
 *
%* ========================================================================
 * Return:	0�ʲ� �����꡼���顼
 *		0�ʾ� MAXLENGTH
-* ======================================================================*/
unsigned long My_Db_EscapeString(MyDBase *db, char *to, const char *from, unsigned long length)
{
	return mysql_real_escape_string((MYSQL*)db, to, from, length);
}
char *My_Escape_SqlString(MyDBase *db, const char *from)
{
	unsigned long in_ret;
	char *chp_ret;
	int in_len;

	in_len = strlen(from);
	chp_ret = malloc(in_len * 5 + 1);
	in_ret = My_Db_EscapeString(db, chp_ret, from, (unsigned long)in_len);
	chp_ret[in_ret] = '\0';

	return chp_ret;
}
