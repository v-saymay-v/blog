/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_configure.c
 * First-Created:	2004/08/12 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *
 *
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_createblog.c,v 1.24 2008/01/24 03:24:50 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "libblog.h"
#include "libcgi2.h"
#include "libcommon2.h"

/*
int Write_To_Dbb_Db(DBase *db, NLIST *nlp_out, int in_blog, char *chp_owner)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_blog[21];
	char cha_column_owner[21];
	char cha_cookie_host[256];
	char cha_cookie_path[256];
	char cha_passfile[256];

	sprintf(cha_sql,
		"select c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_blog,"
		" c_column_owner,"
		" c_passfile "
		" from sy_authinfo");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリの実行に失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "QUERY", "ログイン情報のレコードがありません。");
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		char cha_hostname[256];
		char *chp;
		gethostname(cha_hostname, sizeof(cha_hostname));
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
		Replace_String("#HOST#", cha_cookie_host, cha_hostname);
		chp = getenv("SCRIPT_NAME");
		if (chp) {
			strcpy(cha_hostname, chp);
			chp = strchr(cha_hostname, '/');
			if (chp) {
				++chp;
				*chp = '\0';
			}
			Replace_String("#SCRIPT#", cha_hostname, cha_cookie_path);
		}
	}
	if (strcmp(cha_password, "#FILE#") == 0) {
		FILE *fp;
		char *chp;
		fp = fopen(cha_passfile, "r");
		if (fp) {
			fread(cha_password, sizeof(cha_password), 1, fp);
			chp = strchr(cha_password, '\r');
			if (chp)
				*chp = '\0';
			chp = strchr(cha_password, '\n');
			if (chp)
				*chp = '\0';
			fclose(fp);
		}
	}

	authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	if (!authdb){
		Put_Nlist(nlp_out, "ERROR", "データベースの接続に失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}
	if(Begin_Transact(authdb)) {
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		Db_Disconnect(authdb);
		return 1;
	}
	sprintf(cha_sql, "update %s set %s=%d where %s=%s", cha_table, cha_column_blog, in_blog, cha_column_owner, chp_owner);
	if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "クエリの実行に失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	if(Commit_Transact(authdb)) {
		Put_Nlist(nlp_out, "ERROR", "トランザクションの終了に失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		Rollback_Transact(authdb);
		Db_Disconnect(authdb);
		return 1;
	}
	Db_Disconnect(authdb);

	return 0;
}
*/

/*
+* ------------------------------------------------------------------------
 * Function:	 	main()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int main()
{
	NLIST *nlp_in;
	NLIST *nlp_out;
	DBase *db;
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char cha_sql[1024];
//	char *chp_owner;
	int in_ssl;
	int in_blog;
	int in_error;

	in_error = Read_Blog_Ini(cha_host,cha_db, cha_username, cha_password, &in_ssl);
	if (in_error) {
		printf("\n\nINIファイルの読み込みに失敗しました。(%s)", strerror(in_error));
		return in_error;
	}
	if (in_ssl) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		printf("\n\nデータベース接続に失敗しました。(%s,%s,%s,%s):%s", cha_host, cha_db, cha_username, cha_password, Gcha_last_error);
		fprintf(stderr, "Failed to connect database(%s,%s,%s,%s):%s", cha_host, cha_db, cha_username, cha_password, Gcha_last_error);
		return 1;
	}
	if (Read_Blog_Option(db)) {
		printf("\n\nブログオプションの読み込みに失敗しました。");
		Db_Disconnect(db);
		return 1;
	}
	Set_SkelPath(g_cha_asp_skeleton);

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();
/*
	chp_owner = Get_Nlist(nlp_in, "owner", 1);
	if (!chp_owner || !chp_owner[0]) {
		Put_Nlist(nlp_out, "ERROR", "オーナーIDが指定されていません。");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		return 1;
	}
*/
	in_blog = Create_Blog(db);
	if (!in_blog) {
		Db_Disconnect(db);
		return 1;
	}
	Db_Disconnect(db);

	printf("Content-type: text/plain\n\n");
	printf("Success !!(%d)\n", in_blog);

	return 0;
}
