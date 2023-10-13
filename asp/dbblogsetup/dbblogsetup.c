/*
+* ------------------------------------------------------------------------
 * Module-Name:		tb.c
 * First-Created:	2004/08/20 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	トラックバック受信
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: dbblogsetup.c,v 1.15 2008/01/24 03:24:50 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "libcommon2.h"
#include "libblog.h"
#include "blog.h"

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

int Write_To_Dbb_Db(DBase *db, DBase *authdb, int in_blog, char *chp_owner, char *chp_nickname)
{
	char cha_sql[1024];
	char cha_blog[2048];
	char *chp_blog;

	sprintf(cha_sql, "update dt_owner set n_blog_id = %d where n_owner = %s", in_blog, chp_owner);
	if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		return 1;
	}
	sprintf(cha_sql, "update at_visitor set n_visitor_id = %d where n_owner_id = %s", in_blog, chp_owner);
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_blog = My_Escape_SqlString(db, chp_nickname);
	strcpy(cha_blog, chp_blog);
	strcat(cha_blog, "日記");
	free(chp_blog);
	sprintf(cha_sql, "update at_blog set c_blog_title = '%s' where n_blog_id = %d", cha_blog, in_blog);
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		return 1;
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	main()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int main(int argc, char**argv)
{
	DBase *db;
	DBase *authdb;
	DBRes *dbres;
	DBRes *dbles;
	struct passwd *pwd;
	char cha_host[256];
	char cha_db[256];
	char cha_user[21];
	char cha_table[21];
	char cha_cookie_host[256];
	char cha_cookie_path[256];
	char cha_passfile[256];
	char cha_sql[1024];
	char cha_username[256];
	char cha_password[256];
	char ch;
	char *chp_tmp;
	char *chp_tmp2;
	int i;
	int in_blog;
	int in_error;
	int in_ssl;

	cha_user[0] = '\0';
	while ((ch = getopt(argc, argv, "u:")) != (char)-1) {
		switch (ch) {
		case 'u':
			if (strlen(optarg) < sizeof(cha_user)) {
				strcpy(cha_user, optarg);
			}
			break;
		case '?':
			printf("Usage: dbblogsetrup -u unixuser\n");
			printf("\t-u user   : unix user of blog\n");
			return 1;
		}
	}

	if (!cha_user[0]) {
		printf("user not specified\n");
		return 1;
	}
	pwd = getpwnam(cha_user);
	if (!pwd) {
		printf("failed to get user info (%s)\n", cha_user);
		return 1;
	}
	strcpy(cha_sql, pwd->pw_dir);
	strcat(cha_sql, "/data/dbb_blog.ini");
	in_error = Read_Ini(cha_sql, cha_host, cha_db, cha_username, cha_password, &in_ssl);
	if (in_error) {
		printf("failed to read INI file(%s)\n", strerror(in_error));
		return 1;
	}
	if (in_ssl) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		printf("can not to connect to database(%s)\n", Gcha_last_error);
		return 1;
	}
	if (Read_Blog_Option(db)) {
		printf("can not to read option");
		return 1;
	}

	sprintf(cha_sql,
		"select c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_passfile "
		" from sy_authinfo");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		printf("no records for login.\n");
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "");
	Db_CloseDyna(dbres);

	pwd = getpwuid(getuid());
	if (pwd) {
		char cha_hostname[256];
		char *chp;
		gethostname(cha_hostname, sizeof(cha_hostname));
		Replace_String("#USER#", pwd->pw_name, cha_db);
		Replace_String("#USER#", pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", pwd->pw_dir, cha_passfile);
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
		printf("failed to connect db(%s)\n", Gcha_last_error);
		Db_Disconnect(db);
		return 1;
	}

	dbres = Db_OpenDyna(authdb, "select T1.n_owner,T1.c_nickname from dt_owner T1,dt_owner_detail T2 where T1.n_owner = T2.n_owner and T2.n_state is not null and T1.n_blog_id is null");
	if (!dbres) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Db_Disconnect(db);
		Db_Disconnect(authdb);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_tmp2 = Db_GetValue(dbres, i, 1);
		if (chp_tmp) {
			if(Begin_Transact(db)) {
				printf("failed to start transaction(%s)\n", Gcha_last_error);
				Db_Disconnect(db);
				Db_Disconnect(authdb);
				return 0;
			}
			if(Begin_Transact(authdb)) {
				printf("failed to start transaction(%s)\n", Gcha_last_error);
				Db_Disconnect(db);
				Db_Disconnect(authdb);
				return 1;
			}
			in_blog = Create_Blog(db);
			if (!in_blog) {
				Rollback_Transact(db);
				Rollback_Transact(authdb);
				Db_Disconnect(db);
				Db_Disconnect(authdb);
				return 1;
			}
			if (Write_To_Dbb_Db(db, authdb, in_blog, chp_tmp, chp_tmp2 ? chp_tmp2 : "")) {
				Rollback_Transact(db);
				Rollback_Transact(authdb);
				Db_Disconnect(db);
				Db_Disconnect(authdb);
				return 1;
			}
			if (Commit_Transact(authdb)) {
				printf("failed to commit transaction(%s)\n", Gcha_last_error);
				Rollback_Transact(db);
				Rollback_Transact(authdb);
				Db_Disconnect(db);
				Db_Disconnect(authdb);
				return 1;
			}
			if (Commit_Transact(db)) {
				printf("failed to commit transaction(%s)\n", Gcha_last_error);
				Rollback_Transact(db);
				Rollback_Transact(authdb);
				Db_Disconnect(db);
				Db_Disconnect(authdb);
				return 0;
			}
		}
	}
	Db_CloseDyna(dbres);

	dbres = Db_OpenDyna(db, "select n_blog_id from at_blog where (c_blog_title is null or c_blog_title = '' or c_blog_title = '日記')");
	if (!dbres) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Db_Disconnect(db);
		Db_Disconnect(authdb);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp) {
			sprintf(cha_sql, "select c_nickname from dt_owner where n_blog_id = %s", chp_tmp);
			dbles = Db_OpenDyna(authdb, cha_sql);
			if (!dbles) {
				printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
				Db_Disconnect(db);
				Db_Disconnect(authdb);
				return 1;
			}
			chp_tmp2 = Db_GetValue(dbles, 0, 0);
			if (chp_tmp2) {
				sprintf(cha_sql, "update at_blog set c_blog_title = '%s日記' where n_blog_id = %s", chp_tmp2, chp_tmp);
				Db_ExecSql(db, cha_sql);
			}
			Db_CloseDyna(dbles);
		}
	}
	Db_CloseDyna(dbres);

	Db_Disconnect(db);
	Db_Disconnect(authdb);

	return 0;
}
