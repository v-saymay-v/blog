/*
+* ------------------------------------------------------------------------
 * Module-Name:         libauth.c
 * First-Created:       2006/10/31 堀 正明
%* ------------------------------------------------------------------------
 * Module-Description:
 *	blogの認証関連のライブラリ。
-* ------------------------------------------------------------------------
 * Change-Log:
$* ------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <libnet.h>
#include "libblog.h"
#include "libauth.h"

/*
+* ========================================================================
 * Function:	Read_From_Pass_File
 * Description:
 *
%* ========================================================================
 * Return:	!=0	エラー
 *			==0	成功
-* ======================================================================
 */
int Read_From_Pass_File(NLIST *nlp_out, const char *chp_file, char *chp_user, char *chp_pass)
{
	FILE *fp;
	char cha_passfile[256];
	char *chp_tmp;

	fp = fopen(chp_file, "r");
	if (!fp) {
		Put_Format_Nlist(nlp_out, "ERROR", "パスワードの読み込みに失敗しました。<br>%s<br>%s", strerror(errno), cha_passfile);
		return 1;
	}
	fgets(cha_passfile, sizeof(cha_passfile), fp);
	fclose(fp);
	chp_tmp = strchr(cha_passfile, '\n');
	if (chp_tmp)
		*chp_tmp = '\0';
	chp_tmp = strchr(cha_passfile, '\r');
	if (chp_tmp)
		*chp_tmp = '\0';
	chp_tmp = strtok(cha_passfile, ":");
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "パスワードの形式が間違っています。<br>");
		return 1;
	}
	strcpy(chp_user, chp_tmp);
	chp_tmp = strtok(NULL, ":");
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "パスワードの形式が間違っています。<br>");
		return 1;
	}
	strcpy(chp_pass, chp_tmp);
	return 0;
}

/*
+* ========================================================================
 * Function:	Check_Login
 * Description:
 *
%* ========================================================================
 * Return:	<0	エラー
 * 			=0	未ログイン
 *			>0	ログイン済みブログID
-* ======================================================================
 */
int Check_Login(DBase *db, NLIST *nlp_out, int *inp_owner, int *inp_auth)
{
	DBase *authdb;
	DBase *commondb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_access[1024];
	char cha_host[256];
	char cha_host_common[256];	/* dbb */
	char cha_cookie_host[256];
	char cha_cookie_path[256];
	char cha_owner[32];
	char cha_db[21];
	char cha_user[21];
	char cha_passfile[256];
	char cha_password[21];
	char cha_table[21];
	char cha_table_login[21];
	char cha_column_login[21];
	char cha_column_password[21];
	char cha_column_session[21];
	char cha_column_blog[21];
	char cha_column_owner[21];
	char cha_cookie[21];
	char cha_owner_cookie[21];
	int in_blog;

	if (g_in_cart_mode == CO_CART_RESERVE) {
		*inp_owner = -2;	/* 0:admin -1:asjadmin で使用しているため */
	} else {
		*inp_owner = 0;
	}
	*inp_auth = 0;
	sprintf(cha_sql,
		"select c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_login,"
		" c_column_password,"
		" c_column_session,"
		" c_column_blog,"
		" c_cookie,"
		" c_owner_cookie,"
		" c_column_owner,"
		" c_cookie_host,"
		" c_cookie_path,"
		" c_passfile,"
		" c_table_login ");
	if(g_in_dbb_mode) {
		strcat(cha_sql, ", c_host_common");
	}
	strcat(cha_sql, " from sy_authinfo");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_login, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_password, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_password");
	strcpy(cha_column_session, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "c_access_key");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "n_blog_id");
	strcpy(cha_cookie, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "dbb_blog");
	strcpy(cha_owner_cookie, Db_FetchValue(dbres, 10) ? Db_FetchValue(dbres, 10) : "");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 11) ? Db_FetchValue(dbres, 11) : "");
	strcpy(cha_cookie_host, Db_FetchValue(dbres, 12) ? Db_FetchValue(dbres, 12) : "");
	strcpy(cha_cookie_path, Db_FetchValue(dbres, 13) ? Db_FetchValue(dbres, 13) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 14) ? Db_FetchValue(dbres, 14) : "");
	strcpy(cha_table_login, Db_FetchValue(dbres, 15) ? Db_FetchValue(dbres, 15) : "");
	if(g_in_dbb_mode) {
		strcpy(cha_host_common, Db_FetchValue(dbres, 16) ? Db_FetchValue(dbres, 16) : "localhost");
	}
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return -1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	nlp_cookie = Get_Cookie();
	if (!nlp_cookie) {
		return 0;
	}
	chp_val = Get_Nlist(nlp_cookie, cha_cookie, 1);
	if (!chp_val || !*chp_val) {
		Finish_Nlist(nlp_cookie);
		return 0;
	}
	strcpy(cha_access, chp_val);
	cha_owner[0] = '\0';
	if (cha_owner_cookie[0]) {
		chp_val = Get_Nlist(nlp_cookie, cha_owner_cookie, 1);
		if (chp_val && *chp_val) {
			strcpy(cha_owner, chp_val);
		}
	}
	Finish_Nlist(nlp_cookie);

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return -1;
	}
	commondb = NULL; 
	if(g_in_dbb_mode) {
		if (g_in_ssl_mode) {
			commondb = Db_ConnectSSL(cha_host_common, cha_db, cha_user, cha_password);
		} else {
			commondb = Db_ConnectWithParam(cha_host_common, cha_db, cha_user, cha_password);
		}
		if (!commondb){
			Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			return -1;
		}
	}
	if (cha_table_login[0]) {
		if (cha_owner_cookie[0] && cha_owner[0]) {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select T1.%s from %s T1 where T1.%s=%s and T1.%s='%s' and coalesce(T1.b_retire, 0) = 0",
						cha_column_owner, cha_table, cha_column_owner, cha_owner, cha_column_session, cha_access);
			} else {
				sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.%s='%s' and T1.%s=%s",
					cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_column_session, cha_access, cha_column_owner, cha_owner);
			}
		} else {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select T1.%s from %s T1 where T1.%s='%s' and coalesce(T1.b_retire, 0) = 0",
						cha_column_owner, cha_table, cha_column_session, cha_access);
			} else {
				sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.%s='%s'",
					cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_column_session, cha_access);
			}
		}
	} else {
		if (cha_owner_cookie[0] && cha_owner[0]) {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select T1.%s from %s T1 where T1.%s=%s and T1.%s='%s' and coalesce(T1.b_retire, 0) = 0",
						cha_column_owner, cha_table, cha_column_owner, cha_owner, cha_column_session, cha_access);
			} else {
				sprintf(cha_sql, "select %s,%s from %s where %s='%s' and %s=%s",
					cha_column_blog, cha_column_owner, cha_table, cha_column_session, cha_access, cha_column_owner, cha_owner);
			}
		} else {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select T1.%s from %s T1 where T1.%s='%s' and coalesce(T1.b_retire, 0) = 0",
						cha_column_owner, cha_table, cha_column_session, cha_access);
			} else {
				sprintf(cha_sql, "select %s,%s from %s where %s='%s'",
					cha_column_blog, cha_column_owner, cha_table, cha_column_session, cha_access);
			}
		}
	}
	if(g_in_dbb_mode) {
		dbres = Db_OpenDyna(commondb, cha_sql);
	} else {
		dbres = Db_OpenDyna(authdb, cha_sql);
	}
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		if (g_in_dbb_mode) {
			Db_CloseDyna(dbres);
			if (cha_owner_cookie[0] && cha_owner[0]) {
				sprintf(cha_sql, "select T1.%s from %s T1 where T1.c_admin_access_key='%s' and T1.%s=%s"
					, cha_column_owner, cha_table, cha_access, cha_column_owner, cha_owner);
			} else {
				sprintf(cha_sql, "select T1.%s from %s T1 where T1.c_admin_access_key='\
%s'"
					, cha_column_owner, cha_table, cha_access);
			}
			dbres = Db_OpenDyna(commondb, cha_sql);
			if (!dbres) {
				Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				Db_Disconnect(authdb);
				return -1;
			}
			if (Db_FetchNext(dbres) != CO_SQL_OK) {
				Db_CloseDyna(dbres);
				Db_Disconnect(authdb);
				return 0;
			}
			*inp_auth = 1;
		} else if (g_in_cart_mode == CO_CART_RESERVE) {
			sprintf(cha_sql, "select %s,%s from %s where c_admin_access_key='%s'", cha_column_blog, cha_column_owner, cha_table, cha_access);
			dbres = Db_OpenDyna(authdb, cha_sql);
			if (!dbres) {
				Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				Db_Disconnect(authdb);
				return -1;
			}
			if (Db_FetchNext(dbres) != CO_SQL_OK) {
				Db_CloseDyna(dbres);
				Db_Disconnect(authdb);
				return 0;
			}
			*inp_auth = 1;
		} else {
                        Db_CloseDyna(dbres);
			Db_Disconnect(authdb);
			return 0;
		}
	}
	if(g_in_dbb_mode) {
		sprintf(cha_sql, "select T1.%s,T1.%s from %s T1 where T1.%s = %s",
			cha_column_blog, cha_column_owner, cha_table, cha_column_owner, Db_FetchValue(dbres, 0));
		Db_CloseDyna(dbres);
		dbres = Db_OpenDyna(authdb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return -1;
		}
		if (Db_FetchNext(dbres) != CO_SQL_OK) {
			Db_CloseDyna(dbres);
			Db_Disconnect(authdb);
			return 0;
		}
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_blog = atoi(chp_val);
	} else {
		in_blog = INT_MAX;
	}
	chp_val = Db_FetchValue(dbres, 1);
	if (chp_val && chp_val[0]) {
		*inp_owner = atoi(chp_val);
		if (!*inp_owner)
			*inp_auth = 1;
	} else {
		*inp_owner = -1;
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);
	if(commondb) {
		Db_Disconnect(commondb);
	}
/*
	if (in_blog) {
		if (cha_cookie_host[0] && cha_cookie_path[0]) {
			chp_val = strchr(cha_cookie_host, '.');
			if (!chp_val)
				chp_val = cha_cookie_host;
			printf("Set-Cookie: %s=%s; path=%s; domain=%s\n", cha_cookie, cha_access, cha_cookie_path, chp_val);
			if (cha_owner[0]) {
				printf("Set-Cookie: %s=%s; path=%s; domain=%s\n", cha_owner_cookie, cha_owner, cha_cookie_path, chp_val);
			}
		} else {
			if (g_in_short_name || g_in_hb_mode) {
				printf("Set-Cookie: %s=%s; path=%s\n", cha_cookie, cha_access, g_cha_base_location);
				if (cha_owner_cookie[0]) {
					printf("Set-Cookie: %s=%s; path=%s\n", cha_owner_cookie, cha_owner, g_cha_base_location);
				}
			} else {
				Set_Cookie(cha_cookie, cha_access, "");
				if (cha_owner_cookie[0]) {
					Set_Cookie(cha_owner_cookie, cha_owner, "");
				}
			}
		}
	}
*/
//	if (in_blog && g_in_cart_mode == CO_CART_RESERVE) {
//		printf("Set-Cookie: %s=%s; path=%s\n", cha_cookie, cha_access, g_cha_user_cgi);
//	}
	return in_blog;
}

/*
+* ========================================================================
 * Function:	Auth_Login
 * Description:
 *
%* ========================================================================
 * Return:	<0	エラー
 * 			=0	ログイン失敗
 *			>0	ログイン済みブログID
-* ======================================================================
 */
int Auth_Login(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_loginname, char *chp_passname, int *inp_owner)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[1024];
	char cha_md5[64];
	char cha_loginval[256];
	char cha_passval[256];
	char cha_host[256];
	char cha_cookie_host[256];
	char cha_cookie_path[256];
	char cha_db[21];
	char cha_owner[32];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_login[21];
	char cha_column_password[21];
	char cha_column_session[21];
	char cha_column_blog[21];
	char cha_column_owner[21];
	char cha_cookie[21];
	char cha_owner_cookie[21];
	char cha_passfile[256];
	int in_blog;

	*inp_owner = 0;
	chp_val = Get_Nlist(nlp_in, chp_loginname, 1);
	if (!chp_val || !chp_val[0] || strlen(chp_val) > CO_MAXLEN_BLOG_LOGIN) {
		return 0;
	}
	strcpy(cha_loginval, chp_val);
	chp_val = Get_Nlist(nlp_in, chp_passname, 1);
	if (!chp_val || !chp_val[0] || strlen(chp_val) > CO_MAXLEN_BLOG_PASSWORD) {
		return 0;
	}
	strcpy(cha_passval, chp_val);

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_login,"
		" c_column_password,"
		" c_column_session,"
		" c_column_blog,"
		" c_cookie,"
		" c_owner_cookie,"
		" c_column_owner,"
		" c_cookie_host,"
		" c_cookie_path,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "認証情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "認証情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_login, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_password, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_password");
	strcpy(cha_column_session, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "c_access_key");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "n_blog_id");
	strcpy(cha_cookie, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "dbb_blog");
	strcpy(cha_owner_cookie, Db_FetchValue(dbres, 10) ? Db_FetchValue(dbres, 10) : "");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 11) ? Db_FetchValue(dbres, 11) : "");
	strcpy(cha_cookie_host, Db_FetchValue(dbres, 12) ? Db_FetchValue(dbres, 12) : "");
	strcpy(cha_cookie_path, Db_FetchValue(dbres, 13) ? Db_FetchValue(dbres, 13) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 14) ? Db_FetchValue(dbres, 14) : "");
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return -1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	if (g_in_dbb_mode) {
		sprintf(cha_sql, "select T1.%s,T1.%s from %s T1 where T1.b_retire, 0 and T1.%s='%s' and T1.%s='%s'",
			cha_column_blog, cha_column_owner, cha_table, cha_column_login, cha_loginval, cha_column_password, cha_passval);
	} else {
		sprintf(cha_sql, "select %s,%s from %s where %s='%s' and %s='%s'",
			cha_column_blog, cha_column_owner, cha_table, cha_column_login, cha_loginval, cha_column_password, cha_passval);
	}
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 0;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_blog = atoi(chp_val);
	} else {
		in_blog = INT_MAX;
	}
	cha_owner[0] = '\0';
//	if (cha_owner_cookie[0]) {
		chp_val = Db_FetchValue(dbres, 1);
		if (chp_val) {
		*inp_owner = atoi(chp_val);
			sprintf(cha_owner, "%d", *inp_owner);
		}
//	}
	Db_CloseDyna(dbres);

	if (in_blog) {
		struct timeval tv;
		double t1;
		gettimeofday (&tv, NULL);
		t1 = tv.tv_sec + (double) tv.tv_usec * 1e-6;
		sprintf(cha_sql, "%s%f%s", cha_loginval, t1, cha_passval);
		Create_Md5(cha_sql, cha_md5);
		sprintf(cha_sql, "update %s set %s='%s' where %s='%s' and %s='%s'", cha_table, cha_column_session, cha_md5, cha_column_login, cha_loginval, cha_column_password, cha_passval);
		if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
			Put_Format_Nlist(nlp_out, "ERROR", "認証情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return -1;
		}
		if (cha_cookie_host[0] && cha_cookie_path[0]) {
			chp_val = strchr(cha_cookie_host, '.');
			if (!chp_val)
				chp_val = cha_cookie_host;
			printf("Set-Cookie: %s=%s; path=%s; domain=%s\n", cha_cookie, cha_md5, cha_cookie_path, chp_val);
			if (cha_owner[0]) {
				printf("Set-Cookie: %s=%s; path=%s; domain=%s\n", cha_owner_cookie, cha_owner, cha_cookie_path, chp_val);
			}
			fflush(stdout);
		} else {
			if (g_in_short_name || g_in_hb_mode) {
				printf("Set-Cookie: %s=%s; path=%s\n", cha_cookie, cha_md5, g_cha_base_location);
				if (cha_owner[0]) {
					printf("Set-Cookie: %s=%s; path=%s\n", cha_owner_cookie, cha_owner, g_cha_base_location);
				}
			} else if (g_in_cart_mode == CO_CART_RESERVE) {
				printf("Set-Cookie: %s=%s; path=%s\n", cha_cookie, cha_md5, g_cha_user_cgi);
				if (cha_owner[0]) {
					printf("Set-Cookie: %s=%s; path=%s\n", cha_owner_cookie, cha_owner, g_cha_user_cgi);
				}
			} else {
				Set_Cookie(cha_cookie, cha_md5, "");
				if (cha_owner[0]) {
					Set_Cookie(cha_owner_cookie, cha_owner, "");
				}
			}
			fflush(stdout);
		}
	}
	Db_Disconnect(authdb);

	return in_blog;
}

/*
+* ========================================================================
 * Function:	Clear_Login
 * Description:
 *
%* ========================================================================
 * Return:	!=0	エラー
 *			=0	成功
-* ======================================================================
 */
int Clear_Login(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char cha_sql[1024];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_table_login[21];
	char cha_column_login[21];
	char cha_column_password[21];
	char cha_column_session[21];
	char cha_column_blog[21];
	char cha_column_owner[21];
	char cha_cookie[21];
	char cha_owner_cookie[21];
	char cha_passfile[256];

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_login,"
		" c_column_password,"
		" c_column_session,"
		" c_column_blog,"
		" c_cookie,"
		" c_owner_cookie,"
		" c_column_owner,"
		" c_table_login "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "認証情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_login, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_password, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_password");
	strcpy(cha_column_session, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "c_access_key");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "n_blog_id");
	strcpy(cha_cookie, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "dbb_blog");
	strcpy(cha_owner_cookie, Db_FetchValue(dbres, 10) ? Db_FetchValue(dbres, 10) : "");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 11) ? Db_FetchValue(dbres, 11) : "");
	strcpy(cha_table_login, Db_FetchValue(dbres, 12) ? Db_FetchValue(dbres, 12) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		char cha_hostname[256];
		gethostname(cha_hostname, sizeof(cha_hostname));
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}

	if (cha_table_login) {
		sprintf(cha_sql, "update %s T1,%s T2 set T2.%s = '' where T1.%s = T2.%s and T1.%s = %d",
			cha_table, cha_table_login, cha_column_session, cha_column_owner, cha_column_owner, cha_column_blog, in_blog);
	} else {
		sprintf(cha_sql, "update %s set %s = '' where %s = %d", cha_table, cha_column_session, cha_column_blog, in_blog);
	}
	if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	Set_Cookie(cha_cookie, "0", "Thu, 1-Jan-1980 00:00:00 GMT");
	if (cha_owner_cookie[0]) {
		Set_Cookie(cha_owner_cookie, "0", "Thu, 1-Jan-1980 00:00:00 GMT");
	}
	Db_Disconnect(authdb);
	return 0;
}

/*
+* ========================================================================
 * Function:	Check_Retire
 * Description:
 *
%* ========================================================================
 * Return:	=<0	エラー
 *			=1	解約墨
 *			=0	有効
-* ======================================================================
 */
int Check_Retire(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char cha_sql[1024];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_blog[21];
	char cha_passfile[256];
	char *chp_val;
	int in_ret;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_blog,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "認証情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_passfile, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_login");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		char cha_hostname[256];
		gethostname(cha_hostname, sizeof(cha_hostname));
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return -1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	sprintf(cha_sql, "select b_retire from %s where %s = %d", cha_table, cha_column_blog, in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 0;
	}
	chp_val = Db_FetchValue(dbres, 0);
	in_ret = chp_val ? atoi(chp_val) : 0;
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_ret;
}

/*
+* ========================================================================
 * Function:	Check_Remote_User
 * Description:
 *
%* ========================================================================
 * Return:	<0	エラー
 * 			=0	未ログイン
 *			>0	ログイン済みブログID
-* ======================================================================
 */
int Check_Remote_User(DBase *db, NLIST *nlp_out)
{
	int in_blog;
	char cha_md5[64];
	char cha_sql[1024];
	char *chp_user;
	DBRes *dbres;

	chp_user = getenv("REMOTE_USER");
	if (!chp_user) {
		return 0;
	}

	sprintf(cha_sql, "select n_blog_id from at_blog where c_blog_id = '%s';", chp_user);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)%s<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		return 0;
	}
	in_blog = atoi(Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "0");
	Db_CloseDyna(dbres);

	if (in_blog) {
		sprintf(cha_sql, "%s%ld", chp_user, (long int)time(NULL));
		Create_Md5(cha_sql, cha_md5);
		sprintf(cha_sql, "update at_profile set c_access_key = '%s' where n_blog_id = %d", cha_md5, in_blog);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)%s<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return -1;
		}
		printf("Set-Cookie: as_blog_remote_user=%s; path=%s\n", cha_md5, g_cha_user_cgi);
	}

	return in_blog;
}

/*
+* ========================================================================
 * Function:	Check_Unix_User
 * Description:
 *
%* ========================================================================
 * Return:	<0	エラー
 * 			=0	未ログイン
 *			>0	ログイン済みブログID
-* ======================================================================
 */
int Check_Unix_User(DBase *db, NLIST *nlp_out)
{
	int in_blog;
	char cha_sql[1024];
	DBRes *dbres;
	struct passwd *stp_pwd;

	stp_pwd = getpwuid(getuid());
	if (!stp_pwd) {
		return 0;
	}

	sprintf(cha_sql, "select n_blog_id from at_blog where c_blog_id = '%s';", stp_pwd->pw_name);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		return 0;
	}
	in_blog = atoi(Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "0");
	Db_CloseDyna(dbres);

	return in_blog;
}


/*
+* ========================================================================
 * Function:	Disp_Login_Page
 * Description:
 *
%* ========================================================================
 * Return:	void
-* ======================================================================
 */
void Disp_Login_Page(NLIST *nlp_in, NLIST *nlp_out, char* chp_action)
{
	int i;
	NLIST *nlp_cur;
	NLIST *nlp_cur2;

	for (i = 0; i < CO_MAX_SLOT; i++) {
		for (nlp_cur = nlp_in + i; nlp_cur->nlp_next; ) {
			nlp_cur = nlp_cur->nlp_next;
			if (strcmp(nlp_cur->chp_name, CO_LOGIN_ID) != 0 && strcmp(nlp_cur->chp_name, CO_PASSWORD)) {
				Build_HiddenEncode(nlp_out, "HIDDEN", nlp_cur->chp_name, nlp_cur->chp_value);
				for (nlp_cur2 = nlp_cur; nlp_cur2->nlp_name_next; ) {
					nlp_cur2 = nlp_cur2->nlp_name_next;
					if (strcmp(nlp_cur2->chp_name, CO_LOGIN_ID) != 0 && strcmp(nlp_cur2->chp_name, CO_PASSWORD)) {
						Build_HiddenEncode(nlp_out, "HIDDEN", nlp_cur2->chp_name, nlp_cur2->chp_value);
					}
				}
			}
		}
	}
	Put_Nlist(nlp_out, "ACTION", chp_action);
}

/*
+* ========================================================================
 * Function:	Disp_Remind_Finish
 * Description:
 *
%* ========================================================================
 * Return:	void
-* ======================================================================
 */
void Disp_Remind_Finish(NLIST *nlp_in, NLIST *nlp_out, char* chp_action)
{
	int i;
	NLIST *nlp_cur;
	NLIST *nlp_cur2;

	for (i = 0; i < CO_MAX_SLOT; i++) {
		for (nlp_cur = nlp_in + i; nlp_cur->nlp_next; ) {
			nlp_cur = nlp_cur->nlp_next;
			if (strcmp(nlp_cur->chp_name, CO_LOGIN_ID) != 0 &&
				strcmp(nlp_cur->chp_name, CO_PASSWORD) != 0 &&
				strcmp(nlp_cur->chp_name, CO_DISP_REMIND) != 0 &&
				strcmp(nlp_cur->chp_name, CO_SEND_REMIND) != 0 &&
				strcmp(nlp_cur->chp_name, CO_QUESTION) != 0 &&
				strcmp(nlp_cur->chp_name, CO_ANSWER) != 0) {
				Build_HiddenEncode(nlp_out, "HIDDEN", nlp_cur->chp_name, nlp_cur->chp_value);
				for (nlp_cur2 = nlp_cur; nlp_cur2->nlp_name_next; ) {
					nlp_cur2 = nlp_cur2->nlp_name_next;
					if (strcmp(nlp_cur2->chp_name, CO_LOGIN_ID) != 0 &&
						strcmp(nlp_cur2->chp_name, CO_PASSWORD) != 0 &&
						strcmp(nlp_cur2->chp_name, CO_DISP_REMIND) != 0 &&
						strcmp(nlp_cur2->chp_name, CO_SEND_REMIND) != 0 &&
						strcmp(nlp_cur2->chp_name, CO_QUESTION) != 0 &&
						strcmp(nlp_cur2->chp_name, CO_ANSWER) != 0) {
						Build_HiddenEncode(nlp_out, "HIDDEN", nlp_cur2->chp_name, nlp_cur2->chp_value);
					}
				}
			}
		}
	}
	Put_Nlist(nlp_out, "ACTION", chp_action);
}

/*
+* ========================================================================
 * Function:	Disp_Remind_Page
 * Description:
 *
%* ========================================================================
 * Return:	void
-* ======================================================================
 */
int Disp_Remind_Page(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char* chp_action)
{
	DBase *authdb;
	DBRes *dbres;
	int in_ret;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table_remind[21];
	char cha_remind_id[21];
	char cha_remind_name[21];
	static char* chpa_top[2][2] = {{"","(↓選択してください↓)"},{NULL, NULL}};

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table_remind,"
		" c_remind_id,"
		" c_remind_name "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table_remind, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "sy_remind");
	strcpy(cha_remind_id, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "n_remind");
	strcpy(cha_remind_name, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_remind");
	Db_CloseDyna(dbres);

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "リマインダ情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}
	sprintf(cha_sql, "select %s,%s from %s where %s > 0 order by %s", cha_remind_id, cha_remind_name, cha_table_remind, cha_remind_id, cha_remind_id);
	Disp_Remind_Finish(nlp_in, nlp_out, chp_action);
	in_ret = Build_ComboDb(OldDBase(authdb), nlp_out, "REMINDER", cha_sql, CO_QUESTION, chpa_top, NULL, "(↓選択してください↓)");
	Db_Disconnect(authdb);
	return in_ret;
}

/*
+* ========================================================================
 * Function:	Auth_Reminder
 * Description:
 *
%* ========================================================================
 * Return:	void
-* ======================================================================
 */
int Auth_Reminder(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char* chp_action)
{
	DBase *authdb;
	DBRes *dbres;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_login[21];
	char cha_column_password[21];
	char cha_column_question[21];
	char cha_column_answer[21];
	char cha_column_mail[21];
	char cha_mail[256];
	char cha_pass[128];
	char cha_system[256];
	char cha_sender[256];
	char cha_subject[256];
	char cha_body[1024];
	char *chp_tmp;
	char *chp_answer;
	char *chp_login;
	int in_question;
	int in_error;

	in_error = 0;
	chp_login = Get_Nlist(nlp_in, CO_LOGIN_ID, 1);
	if (!chp_login || !chp_login[0]) {
		Put_Nlist(nlp_out, "ERROR", "ログインIDを入力してください。<br>");
		++in_error;
	}
	in_question = 0;
	chp_tmp = Get_Nlist(nlp_in, CO_QUESTION, 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "質問が選択されていません。<br>");
		++in_error;
	} else {
		in_question = atoi(chp_tmp);
		if (!in_question) {
			Put_Nlist(nlp_out, "ERROR", "質問が選択されていません。<br>");
			++in_error;
		}
	}
	chp_answer = Get_Nlist(nlp_in, CO_ANSWER, 1);
	if (!chp_answer || !chp_answer[0]) {
		Put_Nlist(nlp_out, "ERROR", "答えを入力してください。<br>");
		++in_error;
	}
	if (in_error)
		return in_error;

	sprintf(cha_sql,
		"select T1.c_host,"
		" T1.c_db,"
		" T1.c_user,"
		" T1.c_password,"
		" T1.c_table,"
		" T1.c_column_login,"
		" T1.c_column_password,"
		" T1.c_column_question,"
		" T1.c_column_answer,"
		" T1.c_column_mail "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "リマインダ情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_login, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_password, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_password");
	strcpy(cha_column_question, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "n_remind");
	strcpy(cha_column_answer, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "c_remind");
	strcpy(cha_column_mail, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "c_author_mail");
	Db_CloseDyna(dbres);

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "リマインダ情報の照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}
	sprintf(cha_sql, "select %s,%s from %s where %s=%d and %s='%s' and %s='%s'",
		cha_column_password, cha_column_mail, cha_table, cha_column_question, in_question, cha_column_answer, chp_answer, cha_column_login, chp_login);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "リマインダ情報の照合に失敗しました。(%s:%d)", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_body);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Nlist(nlp_out, "ERROR", "リマインダに一致する店長が見つかりません。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "リマインダに一致する作者が見つかりません。<br>");
		}
		return 1;
	}
	strcpy(cha_pass, Db_FetchValue(dbres, 0));
	strcpy(cha_mail, Db_FetchValue(dbres, 1));
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	strcpy(cha_system, "blog");
	strcpy(cha_sender, "manager@asj.ad.jp");
	dbres = Db_OpenDyna(db, "select c_systemname,c_managermail from sy_baseinfo");
	if(dbres) {
		strcpy(cha_system, Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "blog");
		strcpy(cha_sender, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "manager@asj.ad.jp");
		Db_CloseDyna(dbres);
	}
	sprintf(cha_subject, "[%s]ログイン情報のお知らせ", cha_system);
	sprintf(cha_body,
		"ログイン情報は下記のとおりです。\n\n"
		"ログインID：%s\n"
		"パスワード：%s\n\n"
		"安全のため、パスワードを変更されることをお勧めします。\n",
		chp_login, cha_pass);
	if (Send_Mail(cha_sender, cha_mail, cha_subject, cha_body)) {
		Put_Nlist(nlp_out, "ERROR", "メール送信に失敗しました。");
		return 1;
	}

	return 0;
}

/*
+* =============================================================================
 * Function:		Set_My_Avatar
 * Description:
 * 				アバタ―設置
%* =============================================================================
 * Return:
 *			0: 正常終了
 *			1: エラー
-* =============================================================================*/
int Set_My_Avatar(DBase *db, NLIST *nlp_out, const char *chp_name, int in_blog)
{
#define CO_SEX_BOTH						3
#define CO_AVATAR_PARTS_HEAD_OVER		24
#define CO_AVATAR_KIND_JACKET			27
#define CO_AVATAR_PARTS_JACKET_BACK		2
#define CO_AVATAR_PARTS_HEAD			5
	DBase *authdb;
	DBRes *dbres;
	char *chp_val;
	char *chp_item;
	char cha_sql[512];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_nickname[21];
	char cha_column_owner[21];
	char cha_column_blog[21];
	char cha_image_path[256];
	int in_owner;
	int in_row;
	int in_jacket_flag;		/* ジャケットの裏地を表示するか */
	int in_cap_flag;		/* 帽子用の髪の毛に切り替えるか */
	int in_part;
	int in_kind;
	int in_jacket_item_id;
	int in_item_id;
	int in_count;
	int i;

	in_jacket_flag = 0;
	in_cap_flag = 0;
	in_jacket_item_id = 0;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_nickname,"
		" c_column_owner,"
		" c_column_blog,"
		" c_image_location"
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "アバタの取得に失敗しました。<br>");
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_nickname, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "");
	strcpy(cha_image_path, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "/images/");
	Db_CloseDyna(dbres);

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースの接続に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}

	sprintf(cha_sql, "select %s from %s where %s=%d", cha_column_owner, cha_table, cha_column_blog, in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アバタの取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 0;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_owner = atoi(chp_val);
	} else {
		in_owner = 0;
	}
	Db_CloseDyna(dbres);

	/* 特殊なアバターが装着されているかチェック */
	strcpy(cha_sql, "select T5.n_part");		/*0 部位ID*/
	strcat(cha_sql, ", T2.n_avatar_kind");		/*1 アバター種別*/
	strcat(cha_sql, ", T2.n_item");			/*2 アイテムID*/
	strcat(cha_sql, " from ");
	strcat(cha_sql, " dt_item_avatar T2");
	strcat(cha_sql, ", dt_owner T3");
	strcat(cha_sql, ", dm_part T4");
	strcat(cha_sql, ", dt_owner_item_avatar T5");
	sprintf(&cha_sql[strlen(cha_sql)], " where T3.n_owner = %d", in_owner);
	sprintf(&cha_sql[strlen(cha_sql)], " and T5.n_owner = %d", in_owner);
	strcat(cha_sql, " and T5.n_item = T2.n_item");
	strcat(cha_sql, " and T5.n_part = T4.n_part");
	sprintf(&cha_sql[strlen(cha_sql)], " and T2.n_sex in (T3.n_sex,%d)", CO_SEX_BOTH);
	strcat(cha_sql, " order by T4.n_seq");
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "アバタの取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		Db_Disconnect(authdb);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	for(i = 0; i < in_row; i++) {
		chp_val = Db_GetValue(dbres, i, 0);
		if(!chp_val || *chp_val == '\0') {
			Put_Format_Nlist(nlp_out, "ERROR", "アバタの取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			Db_Disconnect(authdb);
			return 1;
		}
		in_part = atoi(chp_val);
		chp_val = Db_GetValue(dbres, i, 1);
		if(!chp_val || *chp_val == '\0') {
			Put_Format_Nlist(nlp_out, "ERROR", "アバタの取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			Db_Disconnect(authdb);
			return 1;
		}
		in_kind = atoi(chp_val);
		switch(in_part) {
		case CO_AVATAR_PARTS_HEAD_OVER:
			in_cap_flag++;
			break;
		}
		switch(in_kind) {
		case CO_AVATAR_KIND_JACKET:
			in_jacket_flag++;
			in_jacket_item_id = atoi(Db_GetValue(dbres, i, 2));
			break;
		}
	}
	Db_CloseDyna(dbres);

	/* query */
	sprintf(cha_sql,
		" select T1.n_item"
		",T1.n_part"
		" from dt_owner_item_avatar T1"
		",dt_item_avatar T2"
		",dt_owner T3"
		",dm_part T4"
		" where T1.n_owner = %d"
		" and T1.n_item = T2.n_item"
		" and T1.n_owner = T3.n_owner"
		" and T1.n_part = T4.n_part"
		" and T2.n_sex in (T3.n_sex,%d)"
		" order by T4.n_seq", in_owner, CO_SEX_BOTH);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "アバタの取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		Db_Disconnect(authdb);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, (char*)chp_name, "<div id=character style=\"position:relative; width:130px; height:190px; overflow:visible\">\n");
	for(i = 0, in_count = 0; i < in_row; i++, in_count++) {
		in_part = atoi(Db_GetValue(dbres, i, 1));
		if(in_jacket_flag && in_part > CO_AVATAR_PARTS_JACKET_BACK) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql
				, "<div id=\"layer%d\" style=\"position:absolute; left:0px; top:0px; z-index:%d\">"
				, in_jacket_item_id, in_count);
			sprintf(&cha_sql[strlen(cha_sql)]
				, "<img id=\"image%d\" src=\"%sitem%db.gif\" border=\"0\" name=\"image%d\" alt=\"アバター\">"
				, in_jacket_item_id, cha_image_path, in_jacket_item_id, in_jacket_item_id);
			strcat(cha_sql, "</div>\n");
			Put_Nlist(nlp_out, (char*)chp_name, cha_sql);
			in_jacket_flag = 0;
			in_count++;
		}
		chp_item = Db_GetValue(dbres, i, 0);
		in_item_id = atoi(chp_item);
		memset(cha_sql, '\0', sizeof(cha_sql));
		if(in_cap_flag && in_part == CO_AVATAR_PARTS_HEAD) {
			sprintf(cha_sql
				, "<div id=\"layer%d\" style=\"position:absolute; left:0px; top:0px; z-index:%d\">"
				, in_item_id, in_count);
			sprintf(&cha_sql[strlen(cha_sql)]
				, "<img id=\"image%d\" src=\"%sitem%db.gif\" border=\"0\" name=\"image%d\" alt=\"アバター\">"
				, in_item_id, cha_image_path, in_item_id, in_item_id);
			strcat(cha_sql, "</div>\n");
			in_cap_flag = 0;
		} else {
			sprintf(cha_sql
				, "<div id=\"layer%d\" style=\"position:absolute; left:0px; top:0px; z-index:%d\">"
				, in_item_id, in_count);
			sprintf(&cha_sql[strlen(cha_sql)]
				, "<img id=\"image%d\" src=\"%sitem%d.gif\" border=\"0\" name=\"image%d\" alt=\"アバター\">"
				, in_item_id, cha_image_path, in_item_id, in_item_id);
			strcat(cha_sql, "</div>\n");
		}
		Put_Nlist(nlp_out, (char*)chp_name, cha_sql);
	}
	Put_Nlist(nlp_out, (char*)chp_name, "<div>\n");
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	今年度を取得
 * Description:	次のシーズンの開幕日までを1シーズンとする
 *
%* ------------------------------------------------------------------------
 * Return:
 *      年度  ：     正常終了
 *      -1　　：     クエリエラー
-* ----------------------------------------------------------------------*/
int Get_DbbSeasonYear(DBase *db)
{
	DBRes *dbres;
	char cha_sql[256];
	int in_year;

	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select T1.n_year");             /*0*/
	strcat(cha_sql, " from dm_option T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		return -1;
	}
	in_year = atoi(Db_GetValue(dbres, 0, 0));
	Db_CloseDyna(dbres);
	return in_year;
}

/*
+* =============================================================================
 * Function:	disp_team_player チーム所属カード表示
 * Include-file:
 * Description:
 *	チームメンバーの一覧を表示する。
%* =============================================================================
 * Return:
 *      CO_RET_TRUE            :正常
 *      1           :異常
 *	CO_RET_JUMP		:ページ移動
-* =============================================================================*/
int Disp_Team_Player(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_err, const char *chp_name, const char *chp_hidden)
{
#define CO_DBB_TEAM_STATUS_DELETE	5
#define CO_NUM_TEAM_PLAYER			15	/* 1チーム人数 */
#define CO_PRIME_SINCE_YEAR	2010
	DBase *authdb;
	DBRes *dbres;
	char *chp_val;
	char *chp_temp;
	char cha_lock[16];
	char cha_move[16];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_exhibition2[16];
	char cha_sql[4096];
	char cha_date[16];
	char cha_team[65];
	char cha_prefix[10];
	int in_before;
	int in_owner_id;
	int in_team_id;
	int in_complete;
	int in_card_kind;
	int in_game;
	int in_year;
	int in_year_spec;
	int b_new_data;
	int b_farm;
	int i;
	double dl_base;
	double dl_total;
	double dl_point;
	double dl_bonus;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_err, "ERROR", "オーダーの取得に失敗しました。<br>");
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	Db_CloseDyna(dbres);

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_err, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}

	/* チームIDを取得 */
	chp_temp = Get_Nlist(nlp_in, (char*)chp_hidden, 1);
	if(!chp_temp || *chp_temp == '\0') {
		Put_Nlist(nlp_err, "ERROR", "チームが選択されていません。<br>");
		Db_Disconnect(authdb);
		return 1;
	}
	in_team_id = atoi(chp_temp);

	/* チームIDからオーナーID、オーナー名取得 */
	strcpy(cha_sql, " select T1.n_owner");	/* 0 */
	strcat(cha_sql, ",T2.c_name");			/* 1 */
	strcat(cha_sql, ",T1.b_farm");			/* 2 */
	strcat(cha_sql, " from dt_team T1");
	strcat(cha_sql, ",dt_team_detail T2");
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_team = %d", in_team_id);
	strcat(cha_sql, " and T1.n_team = T2.n_team");
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_temp = Db_GetValue(dbres, 0, 0);
	in_owner_id = 0;
	if(chp_temp) {
		in_owner_id = atoi(chp_temp);
	}
	chp_temp = Db_GetValue(dbres, 0, 1);
	if(chp_temp) {
		strcpy(cha_team, chp_temp);
	}
	chp_temp = Db_GetValue(dbres, 0, 2);
	b_farm = 0;
	if(chp_temp) {
		b_farm = atoi(chp_temp);
	}
	Db_CloseDyna(dbres);

	in_year = Get_DbbSeasonYear(authdb);
	if(in_year == -1) {
		Put_Nlist(nlp_err, "ERROR", "今年度の取得ができません。<br>");
		Db_Disconnect(authdb);
		return 1;
	}

	/* データのある最新日付取得 */
	strcpy(cha_sql, "select date_format(T1.d_recent_exhibition, '%Y/%m/%d')");	/* 0 */
	strcat(cha_sql, ", T1.b_complete");						/* 1 */
	strcat(cha_sql, ", T1.b_game");							/* 2 */
	strcat(cha_sql, " from dt_move_data_option T1");
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "データのある最新日付取得クエリーに失敗。%s(%d)", Gcha_last_error, __LINE__);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_temp = Db_GetValue(dbres, 0, 0);
	if(chp_temp) {
		strcpy(cha_exhibition2, chp_temp);
	}
	in_complete = 0;
	chp_temp = Db_GetValue(dbres, 0, 1);
	if(chp_temp) {
		in_complete  = atoi(chp_temp);
	}
	in_game = 0;
	chp_temp = Db_GetValue(dbres, 0, 2);
	if(chp_temp) {
		in_game  = atoi(chp_temp);
	}
	Db_CloseDyna(dbres);

	if (!Get_Nlist(nlp_in, "ORDYEAR", 1) || !Get_Nlist(nlp_in, "ORDMONTH", 1) || !Get_Nlist(nlp_in, "ORDDAY", 1)) {
		Put_Nlist(nlp_err, "ERROR", "日付を指定してください。");
		return 1;
	}
	sprintf(cha_date, "%s/%s/%s", Get_Nlist(nlp_in, "ORDYEAR", 1), Get_Nlist(nlp_in, "ORDMONTH", 1), Get_Nlist(nlp_in, "ORDDAY", 1));
	strcpy(cha_sql, "select T1.n_year");
	strcat(cha_sql, " from dm_season_year T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.d_season_start <= '%s'", cha_date);
	sprintf(cha_sql + strlen(cha_sql), " and T1.d_season_end >= '%s'", cha_date);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "日にちの判定に失敗しました。(%s)(%d)", Gcha_last_error, __LINE__);
		return 1;
	}
	in_year_spec = 0;
	if(Db_GetRowCount(dbres)) {
		in_year_spec = atoi(Db_GetValue(dbres, 0, 0));
	}
	Db_CloseDyna(dbres);
	if(!in_year_spec) {
		Put_Nlist(nlp_err, "ERROR", "試合のある日付を選んでください。");
		return 1;
	}
	if (in_year == in_year_spec) {
		cha_prefix[0] = '\0';
	} else {
		sprintf(cha_prefix, "_%d", in_year_spec);
	}

	b_new_data = 0;
	strcpy(cha_sql, " select date_format(T1.d_lock_exhibition, '%Y/%m/%d')");					/*0*/
	strcat(cha_sql, ",date_format(T1.d_data_exhibition, '%Y/%m/%d')");							/*1*/
	strcat(cha_sql, ",if((T1.b_move = 0 and T1.b_all_move = 1) or T1.b_data = 1, 1, null)");	/*2*/
	strcat(cha_sql, " from dt_move_data_option T1");
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.b_farm = %d", b_farm);
/*
	strcpy(cha_sql, "select date_format(T1.d_lock_exhibition, '%Y/%m/%d')");
	strcat(cha_sql, ", date_format(T1.d_data_exhibition, '%Y/%m/%d')");
	strcat(cha_sql, ", if(T1.b_all_move = 1 or T1.b_data = 1, 1, null)");
	strcat(cha_sql, " from dt_move_data_option T1");
	strcat(cha_sql, " order by T1.b_farm");
*/
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "日にちの判定に失敗しました。(%s)(%d)", Gcha_last_error, __LINE__);
		return 1;
	}
	strcpy(cha_lock, Db_GetValue(dbres, 0, 0));
	strcpy(cha_move, Db_GetValue(dbres, 0, 1));
	if(!strcmp(cha_date, cha_lock) && Db_GetValue(dbres, 0, 2)) {
		b_new_data = 1;
	}
	Db_CloseDyna(dbres);

	strcpy(cha_sql, "select T1.c_team_name");			/* 0*/
	strcat(cha_sql, ", T1.c_state");					/* 1*/
	strcat(cha_sql, ", T1.b_public");					/* 2*/
	strcat(cha_sql, ", T1.b_dh");						/* 3*/
	strcat(cha_sql, ", T1.c_league");					/* 4*/
	strcat(cha_sql, ", T1.n_team_money");				/* 5*/
	strcat(cha_sql, ", as_round(T1.f_point_new, 1)");	/* 6*/
	strcat(cha_sql, ", as_round(T1.f_point_total, 1)");	/* 7*/
	strcat(cha_sql, ", as_round(T1.f_point, 1)");		/* 8*/
	strcat(cha_sql, ", as_round(T1.f_base_point, 1)");	/* 9*/
	strcat(cha_sql, ", as_round(T1.f_team_add, 1)");	/*10*/
	strcat(cha_sql, ", as_round(T1.f_team_base, 1)");	/*11*/
	if(b_new_data) {
		strcat(cha_sql, " from dt_team_point_contents_day T1");
	} else {
		sprintf(cha_sql + strlen(cha_sql), " from dt_team_point_contents%s T1", cha_prefix);
	}
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_team = %d", in_team_id);
	if(!b_new_data) {
		sprintf(&cha_sql[strlen(cha_sql)], " and T1.d_exhibition = '%s'", cha_date);
	}
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	dl_point = 0.0;
	dl_base = 0.0;
	dl_bonus = 0.0;
	dl_total = 0.0;
	if(Db_GetRowCount(dbres)) {
		chp_val = Db_GetValue(dbres, 0, 0);
		if(chp_val) {
			strcpy(cha_team, chp_val);
		}
		chp_val = Db_GetValue(dbres, 0, 6);
		if(chp_val) {
			dl_total = atof(chp_val);
		}
		chp_val = Db_GetValue(dbres, 0, 11);
		if(chp_val) {
			dl_point = atof(chp_val);
		}
		chp_val = Db_GetValue(dbres, 0, 10);
		if(chp_val) {
			dl_bonus = atof(chp_val);
		}
		chp_val = Db_GetValue(dbres, 0, 9);
		if(chp_val) {
			dl_base = atof(chp_val);
		}
	}
	Db_CloseDyna(dbres);

	/* チームメンバー表示 */
	strcpy(cha_sql, "select T1.n_player");			/* 0 プレイヤーID */
	strcat(cha_sql, ", T1.n_owner_card");			/* 1 カードID */
	strcat(cha_sql, ", T1.c_position");				/* 2 自チーム内でのポジション(名前) */
	strcat(cha_sql, ", T1.c_player");				/* 3 選手名 */
	strcat(cha_sql, ", T1.c_team");					/* 4 元々の所属チーム */
	strcat(cha_sql, ", T1.n_level");				/* 5 カードレベル */
	strcat(cha_sql, ", T1.n_card_kind");			/* 6 カード種類 */
	strcat(cha_sql, ", T1.n_salary");			/* 7 年俸*/
	strcat(cha_sql, ", T1.n_order");			/* 8 打順 */
	strcat(cha_sql, ", as_round(T1.f_point, 1)");		/* 9 最新ポイント */
	strcat(cha_sql, ", T1.n_photo_type");			/* 10 フォトタイプ */
	if(in_year_spec >= CO_PRIME_SINCE_YEAR) {
		strcat(cha_sql, ", T1.b_prime");			/* 11 プライム */
	} else {
		strcat(cha_sql, ", 0");					/* 11 プライム */
	}
/////////////////////////////////////////////////////////////////////////////////////////////////
//	strcat(cha_sql, ", as_round(T1.f_bbr_new, 1)");
//	strcat(cha_sql, ", T1.c_effect_image");
//	strcat(cha_sql, ", T1.c_sign_image");
/////////////////////////////////////////////////////////////////////////////////////////////////
	if(b_new_data) {
		strcat(cha_sql, " from dt_card_point_contents_day T1");
	} else {
		sprintf(cha_sql + strlen(cha_sql), " from dt_card_point_contents%s T1", cha_prefix);
	}
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_team = %d", in_team_id);
	if(!b_new_data) {
		sprintf(&cha_sql[strlen(cha_sql)], " and T1.d_exhibition = '%s'", cha_date);
	}
	strcat(cha_sql, " order by T1.n_order");
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}

	/* 登録データ表 */
	Put_Nlist(nlp_out, (char*)chp_name, "<!-- オーダーリスト：ここから -->\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<!-- オーダーリスト内のタグは書きかえないでください。  -->\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<table border=\"0\" class=\"order_list\" cellpadding=\"0\" cellspacing=\"1\" style=\"color:#000000; font-size:10px;\">\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<tr valign=\"top\" bgcolor=\"#eeeeee\">\n");
	Put_Format_Nlist(nlp_out, (char*)chp_name, "	<td colspan=\"7\" align=\"center\" nowrap><b>%s</b></td>\n", cha_team);
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<tr valign=\"top\" align=\"center\">\n");
	Put_Format_Nlist(nlp_out, (char*)chp_name, "<td colspan=\"7\" bgcolor=\"#eeeeee\"><b>%s年%d月%d日のオーダーリスト<b></td>\n",
		Get_Nlist(nlp_in, "ORDYEAR", 1), atoi(Get_Nlist(nlp_in, "ORDMONTH", 1)), atoi(Get_Nlist(nlp_in, "ORDDAY", 1)));
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<tr valign=\"top\" bgcolor=\"#eeeeee\">\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>打順</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>守備</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>名前</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>所属</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>カード</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>レベル</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>Point</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	in_card_kind = 0;
	in_before = Cmp_Date(cha_date, cha_move);
	for(i = 0; i < CO_NUM_TEAM_PLAYER; i++) {
		if (i % 2 ) {
			Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
		} else {
			Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
		}
		/* 打順 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>");
		chp_temp = Db_GetValue(dbres, i, 8);
		if(chp_temp) {
			if (atoi(chp_temp) > 9) {
				Put_Nlist(nlp_out, (char*)chp_name, "-");
			} else {
				Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
			}
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* ポジション名 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td nowrap>");
		chp_temp = Db_GetValue(dbres, i, 2);
		if(chp_temp) {
			Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* 氏名 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td nowrap>");
		chp_temp = Db_GetValue(dbres, i, 3);
		if(chp_temp) {
			Put_Format_Nlist(nlp_out, (char*)chp_name, "%s", chp_temp);
		} else {
			Put_Nlist(nlp_out, (char*)chp_name, "未選択");
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* 所属 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td nowrap>");
		chp_temp = Db_GetValue(dbres, i, 4);
		if(chp_temp) {
			Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* カード */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>");
		chp_temp = Db_GetValue(dbres, i, 11);
		if(chp_temp && atoi(chp_temp) > 0) {
			Put_Nlist(nlp_out, (char*)chp_name, "P");
		}
		chp_temp = Db_GetValue(dbres, i, 6);
		if(chp_temp) {
			switch (atoi(chp_temp)) {
			case 2:
				Put_Nlist(nlp_out, (char*)chp_name, "G");
				break;
			case 3:
				Put_Nlist(nlp_out, (char*)chp_name, "B");
				break;
			case 4:
				Put_Nlist(nlp_out, (char*)chp_name, "W");
				break;
			case 5:
				Put_Nlist(nlp_out, (char*)chp_name, "I");
				break;
			default:
				Put_Nlist(nlp_out, (char*)chp_name, "N");
			}
		}
		chp_temp = Db_GetValue(dbres, i, 10);
		if(chp_temp) {
			switch (atoi(chp_temp)) {
			case 2:
				Put_Nlist(nlp_out, (char*)chp_name, "D");
				break;
			case 3:
				Put_Nlist(nlp_out, (char*)chp_name, "E");
				break;
			case 4:
				Put_Nlist(nlp_out, (char*)chp_name, "R");
				break;
			default:
				Put_Nlist(nlp_out, (char*)chp_name, "S");
			}
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* レベル */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>");
		chp_temp = Db_GetValue(dbres, i, 5);
		if(chp_temp) {
			Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* ポイント */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>");
		chp_temp = Db_GetValue(dbres, i, 9);
		if(chp_temp && in_before >= 0) {
			Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"6\" nowrap>チームポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>\n", dl_point);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;

	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"6\" nowrap>基本ポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>\n", dl_base);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;

	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"6\" nowrap>ボーナスポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>", dl_bonus);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;

	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"6\" nowrap>合計ポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>\n", dl_total);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;

	Put_Nlist(nlp_out, (char*)chp_name, "</table>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<!-- オーダーリスト：ここまで -->\n");

	return 0;
}

/*
+* =============================================================================
 * Function:		Build_Team_Combo
 * Description:
 * 				チーム一覧コンボボックス作成
%* =============================================================================
 * Return:
 *			0: 正常終了
 *			1: エラー
-* =============================================================================*/
int Build_Team_Combo(DBase *db, NLIST *nlp_in, NLIST *nlp_out, const char *chp_name, int in_blog, const char *chp_hidden, const char *chp_team, const char *chp_date)
{
	DBase *authdb;
	DBRes *dbres;
	char *chp_val;
	char *chp_insert;
	char cha_sql[512];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_nickname[21];
	char cha_column_owner[21];
	char cha_column_blog[21];
	char cha_image_path[256];
	char cha_date[21];
	int in_ret;
	int in_owner;
	int in_year_last;
	int in_jacket_flag;		/* ジャケットの裏地を表示するか */
	int in_cap_flag;		/* 帽子用の髪の毛に切り替えるか */
	int in_jacket_item_id;
	static char* chpa_top[2][2] = {{"","(↓チーム選択↓)"},{NULL, NULL}};

	in_jacket_flag = 0;
	in_cap_flag = 0;
	in_jacket_item_id = 0;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_nickname,"
		" c_column_owner,"
		" c_column_blog,"
		" c_image_location"
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "保有チーム一覧の取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_nickname, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "");
	strcpy(cha_image_path, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "/images/");
	Db_CloseDyna(dbres);

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}

	sprintf(cha_sql, "select %s from %s where %s=%d", cha_column_owner, cha_table, cha_column_blog, in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "保有チーム一覧の取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_owner = atoi(chp_val);
	} else {
		in_owner = 0;
	}
	Db_CloseDyna(dbres);

	cha_host[0] = '\0';
	if (chp_team && chp_team[0]) {
		sprintf(cha_sql, "select n_team from dt_team_detail where c_name = '%s'", chp_team);
		dbres = Db_OpenDyna(authdb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return 1;
		}
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			chp_val = Db_FetchValue(dbres, 0);
			if (chp_val) {
				strcpy(cha_host, chp_val);
			}
		}
		Db_CloseDyna(dbres);
	}
	if (!cha_host[0]) {
		chp_val = Get_Nlist(nlp_in, (char*)chp_hidden, 1);
		if (chp_val) {
			strcpy(cha_host, chp_val);
		}
	}
	Get_NowDate(cha_date);
	in_year_last = atoi(cha_date);
	if (chp_date && chp_date[0]) {
		strcpy(cha_date, chp_date);
	}
	chp_insert = Get_Nlist(nlp_in, "INSERT_ORDER", 1);
	Put_Format_Nlist(nlp_out, (char*)chp_name,
		"<table width=\"100%%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">\n"
		"<tr>\n"
		"	<td style=\"font-size:12px; padding:2 0 8 10;\">\n"
		"		<input type=\"checkbox\" value=\"1\" name=\"INSERT_ORDER\"%s>以下の日付のオーダーを挿入する。\n"
		"	</td>\n"
		"</tr>\n"
		"<tr>\n"
		"	<td style=\"font-size:12px; padding:6 0 8 10;\">\n",
		 (chp_insert && atoi(chp_insert)) ? " checked" : "");
	Put_Nlist(nlp_out, (char*)chp_name, "<small>1.</small> 日付を選択 ");
	Build_Date(nlp_out, (char*)chp_name, "ORDYEAR", "ORDMONTH", "ORDDAY", 2007, in_year_last, cha_date);
	Put_Nlist(nlp_out, (char*)chp_name,
		"	</td>\n"
		"</tr>\n"
		"<tr>\n"
		"	<td style=\"font-size:12px; padding:2 0 8 10;\">\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<small>2.</small> チームを選択 ");
	sprintf(cha_sql, "select T1.n_team,T2.c_name from dt_team T1,dt_team_detail T2 where T1.n_team = T2.n_team and T1.n_owner = %d", in_owner);
	in_ret =  Build_ComboDb(OldDBase(authdb), nlp_out, (char*)chp_name, cha_sql, (char*)chp_hidden, chpa_top, NULL, cha_host[0] ? cha_host : "");
	Put_Nlist(nlp_out, (char*)chp_name,
		"	</td>\n"
		"</tr>\n"
		"<tr>\n"
		"	<td style=\"font-size:12px; padding:6 0 6 10; background-color:#F0F5FD;\">\n"
		"		<small>3.</small> 「 1. 」 「 2. 」 を選択したのち、　<input type=\"submit\" value=\"プレビュー\" name=\"BTN_INSERT_ORDER\">\n"
		"	</td>\n"
		"</tr>\n"
		"</table>\n");
	if (in_ret) {
		if (in_ret == 1) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		} else {
			Put_Nlist(nlp_out, "ERROR", "保有チームが見つかりません。");
		}
	}
	Db_Disconnect(authdb);
	return in_ret;
}

/*
+* ========================================================================
 * Function:            Get_Nickname()
 * Description:
 *      現在ログインしているユーザーのニックネームを得る
%* ========================================================================
 * Return:              成功 オーナーID
 *      戻り値          失敗 0
-* ========================================================================*/
int Get_Nickname(DBase *db, NLIST *nlp_out, char *chp_nickname, char *chp_mailaddr, char *chp_key)
{
	DBase *authdb;
	DBase *commondb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_access[1024];
	char cha_host[256];
	char cha_host_common[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_table_login[21];
	char cha_column_nickname[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_column_mail[21];
	char cha_cookie[21];
	char cha_table_detail[21];
	char cha_mail_open[21];
	char cha_passfile[256];
	int in_owner;

	chp_nickname[0] = '\0';
	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_nickname,"
		" c_column_session,"
		" c_column_owner,"
		" c_cookie,"
		" c_column_mail,"
		" c_table_detail,"
		" c_mail_open,"
		" c_passfile,"
		" c_table_login, ");
	if(g_in_dbb_mode) {
		strcat(cha_sql, " c_host_common");
	}
	strcat(cha_sql, " from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 0;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_nickname, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_session, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "");
	strcpy(cha_cookie, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "dbb_blog");
	strcpy(cha_column_mail, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "");
	strcpy(cha_table_detail, Db_FetchValue(dbres, 10) ? Db_FetchValue(dbres, 10) : "");
	strcpy(cha_mail_open, Db_FetchValue(dbres, 11) ? Db_FetchValue(dbres, 11) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 12) ? Db_FetchValue(dbres, 12) : "");
	strcpy(cha_table_login, Db_FetchValue(dbres, 13) ? Db_FetchValue(dbres, 13) : "");
	if(g_in_dbb_mode) {
		strcpy(cha_host_common, Db_FetchValue(dbres, 14) ? Db_FetchValue(dbres, 14) : "localhost");
	}
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (chp_key && chp_key[0]) {
		strcpy(cha_access, chp_key);
	} else {
		nlp_cookie = Get_Cookie();
		if (!nlp_cookie) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			return 0;
		}
		chp_val = Get_Nlist(nlp_cookie, cha_cookie, 1);
		if (!chp_val || !*chp_val) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			Finish_Nlist(nlp_cookie);
			return 0;
		}
		strcpy(cha_access, chp_val);
		Finish_Nlist(nlp_cookie);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースにアクセスできませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 0;
	}
	if(g_in_dbb_mode) {
		if (g_in_ssl_mode) {
			commondb = Db_ConnectSSL(cha_host_common, cha_db, cha_user, cha_password);
		} else {
			commondb = Db_ConnectWithParam(cha_host_common, cha_db, cha_user, cha_password);
		}
		if (!commondb){
			Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			return -1;
		}
		sprintf(cha_sql, "select T1.%s,T1.%s from %s T1 where (T1.%s='%s' or T1.c_admin_access_key='%s')",cha_column_owner, cha_column_mail, cha_table, cha_column_session, cha_access, cha_access);
		dbres = Db_OpenDyna(commondb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return 0;
		}
		if (Db_FetchNext(dbres) != CO_SQL_OK) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			Db_CloseDyna(dbres);
			Db_Disconnect(authdb);
			return 0;
		}
		sprintf(cha_sql, "select T1.%s,T1.%s,'%s' from %s T1 where T1.%s=%s",cha_column_owner, cha_column_nickname, Db_FetchValue(dbres, 1), cha_table, cha_column_owner, Db_FetchValue(dbres, 0));
		Db_CloseDyna(dbres);
	} else {
		if (cha_table_login[0]) {
			sprintf(cha_sql, "select T1.%s,T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.%s='%s'",
				cha_column_owner, cha_column_nickname, cha_column_mail, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_column_session, cha_access);
		} else {
			sprintf(cha_sql, "select %s,%s,%s from %s where %s='%s'",
				cha_column_owner, cha_column_nickname, cha_column_mail, cha_table, cha_column_session, cha_access);
		}
	}
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 0;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_owner = atoi(chp_val);
	} else {
		in_owner = 0;
	}
	chp_val = Db_FetchValue(dbres, 1);
	if (chp_val && chp_val[0]) {
		strcpy(chp_nickname, chp_val);
	}
	chp_val = Db_FetchValue(dbres, 2);
	if (chp_val && chp_val[0] && chp_mailaddr) {
		strcpy(chp_mailaddr, chp_val);
	}
	Db_CloseDyna(dbres);

	if (g_in_dbb_mode) {
		if (chp_mailaddr) {
			chp_mailaddr[0] = '\0';
		}
	} else if (chp_mailaddr && cha_mail_open[0] && cha_table_detail[0]) {
		sprintf(cha_sql, "select %s from %s where %s=%d", cha_mail_open, cha_table_detail, cha_column_owner, in_owner);
		dbres = Db_OpenDyna(authdb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return 0;
		}
		if (Db_FetchNext(dbres) != CO_SQL_OK) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			Db_CloseDyna(dbres);
			Db_Disconnect(authdb);
			return 0;
		}
		chp_val = Db_FetchValue(dbres, 0);
		if (!chp_val || !atoi(chp_val)) {
			chp_mailaddr[0] = '\0';
		}
		Db_CloseDyna(dbres);
	}
	Db_Disconnect(authdb);

	return in_owner;
}

/*
+* ========================================================================
 * Function:            Get_Nickname_From_Blog()
 * Description:
 *      現在ログインしているユーザーのニックネームを得る
%* ========================================================================
 * Return:              成功 オーナーID
 *      戻り値          失敗 0
-* ========================================================================*/
int Get_Nickname_From_Blog(DBase *db, NLIST *nlp_out, int in_blog, char *chp_nickname)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_nickname[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_column_blog[21];
	char cha_cookie[21];
	char cha_passfile[256];
	int in_owner;

	chp_nickname[0] = '\0';
	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_nickname,"
		" c_column_session,"
		" c_column_owner,"
		" c_column_blog,"
		" c_cookie,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 0;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_nickname, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_session, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "");
	strcpy(cha_cookie, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 10) ? Db_FetchValue(dbres, 10) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 0;
	}
	sprintf(cha_sql, "select %s,%s from %s where %s=%d", cha_column_owner, cha_column_nickname, cha_table, cha_column_blog, in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 0;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_owner = atoi(chp_val);
	} else {
		in_owner = 0;
	}
	chp_val = Db_FetchValue(dbres, 1);
	if (chp_val && chp_val[0]) {
		strcpy(chp_nickname, Db_FetchValue(dbres, 1));
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_owner;
}

/*
+* ========================================================================
 * Function:            Get_Owner_From_Nickname()
 * Description:
 *      現在ログインしているユーザーのニックネームからオーナーIDを得る
%* ========================================================================
 * Return:              成功 オーナーID
 *      戻り値          失敗 0
-* ========================================================================*/
int Get_Owner_From_Nickname(DBase *db, NLIST *nlp_out, char *chp_nickname)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_nickname[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_cookie[21];
	char cha_passfile[256];
	int in_blog;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_nickname,"
		" c_column_session,"
		" c_column_owner,"
		" c_cookie,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 0;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_nickname, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_session, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "n_blog_id");
	strcpy(cha_cookie, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。<br>(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 0;
	}
	sprintf(cha_sql, "select %s from %s where replace(replace(%s,' ',''),'　','')=replace(replace('%s',' ',''),'　','')",
		cha_column_owner, cha_table, cha_column_nickname, chp_nickname);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 0;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_blog = atoi(chp_val);
	} else {
		in_blog = 0;
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_blog;
}

/*
+* ========================================================================
 * Function:            Get_Nickname_From_Owner()
 * Description:
 *      現在ログインしているユーザーのニックネームからオーナーIDを得る
%* ========================================================================
 * Return:              成功 0
 *      戻り値          失敗 1
-* ========================================================================*/
int Get_Nickname_From_Owner(DBase *db, NLIST *nlp_out, int in_owner, char *chp_nickname)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_nickname[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_cookie[21];
	char cha_passfile[256];

	chp_nickname[0] = '\0';
	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_nickname,"
		" c_column_session,"
		" c_column_owner,"
		" c_cookie,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_nickname, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_session, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "n_blog_id");
	strcpy(cha_cookie, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}
	sprintf(cha_sql, "select %s from %s where %s=%d", cha_column_nickname, cha_table, cha_column_owner, in_owner);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		strcpy(chp_nickname, "");
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 0;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		strcpy(chp_nickname, chp_val);
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return 0;
}

/*
+* ========================================================================
 * Function:            In_Black_List()
 * Description:
 *      ブラックリストに登録されているか?
%* ========================================================================
 * Return:              成功 オーナーID
 *      戻り値          失敗 0
-* ========================================================================*/
int In_Black_List(DBase *db, NLIST *nlp_out, int in_owner, char *chp_key)
{
	DBase *authdb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[1024];
	char cha_access[1024];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_cookie[21];
	char cha_table[21];
	char cha_table_login[21];
	char cha_table_black[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_black_blog[21];
	char cha_passfile[256];
	int in_blog;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_session,"
		" c_column_owner,"
		" c_table_black,"
		" c_black_blog,"
		" c_cookie,"
		" c_table_login "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dt_owner");
	strcpy(cha_column_session, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_table_black, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "sy_blacklist");
	strcpy(cha_black_blog, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "b_blog");
	strcpy(cha_cookie, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "b_blog");
	strcpy(cha_table_login, Db_FetchValue(dbres, 10) ? Db_FetchValue(dbres, 10) : "b_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (!in_owner) {
		if (chp_key && chp_key[0]) {
			strcpy(cha_access, chp_key);
		} else {
			nlp_cookie = Get_Cookie();
			if (!nlp_cookie) {
				Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
				return -1;
			}
			chp_val = Get_Nlist(nlp_cookie, cha_cookie, 1);
			if (!chp_val || !*chp_val) {
				Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
				Finish_Nlist(nlp_cookie);
				return -1;
			}
			strcpy(cha_access, chp_val);
			Finish_Nlist(nlp_cookie);
		}
	}

	in_blog = 0;
	if (cha_table_black[0] && cha_black_blog[0]) {
		if (g_in_ssl_mode) {
			authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
		} else {
			authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
		}
		if (!authdb){
			Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			return -1;
		}
		if (!in_owner) {
			if (cha_table_login[0]) {
				sprintf(cha_sql,
					" select count(*)"
					" from %s T1"
					" left join %s T4 on T1.n_parent = T4.%s"
					" left join %s T5 on T4.%s = T5.%s"
					",%s T2"
					",%s T3"
					" where T1.%s=T2.%s"
					" and T2.%s=T3.%s"
					" and T3.%s='%s'"
					" and T2.d_create <= NOW()"
					" and (T2.d_end is null or T2.d_end >= NOW())"
					" and ((T2.%s != 0 and T2.%s is not null) or (T5.%s != 0 and T5.%s is not null))",
					cha_table,
					cha_table, cha_column_owner,
					cha_table_black, cha_column_owner, cha_column_owner,
					cha_table_black,
					cha_table_login,
					cha_column_owner, cha_column_owner,
					cha_column_owner, cha_column_owner,
					cha_column_session, cha_access,
					cha_black_blog, cha_black_blog, cha_black_blog, cha_black_blog);
			} else {
				sprintf(cha_sql,
					" select T2.count(*)"
					" from %s T1"
					" left join %s T3 on T1.n_parent = T3.%s"
					" left join %s T4 on T3.%s = T4.%s"
					",%s T2"
					" where T1.%s=T2.%s"
					" and T1.%s='%s'"
					" and T2.d_create <= NOW()"
					" and (T2.d_end is null or T2.d_end >= NOW())"
					" and ((T2.%s != 0 and T2.%s is not null) or (T4.%s != 0 and T4.%s is not null))",
					cha_table,
					cha_table, cha_column_owner,
					cha_table_black, cha_column_owner, cha_column_owner,
					cha_table_black,
					cha_column_owner, cha_column_owner,
					cha_column_session, cha_access,
					cha_black_blog, cha_black_blog, cha_black_blog, cha_black_blog);
			}
		} else {
			sprintf(cha_sql,
				" select count(*)"
				" from %s T1"
				" left join %s T3 on T1.n_parent = T3.%s"
				" left join %s T4 on T3.%s = T4.%s"
				",%s T2"
				" where T1.%s=T2.%s"
				" and T1.%s=%d"
				" and T2.d_create <= NOW()"
				" and (T2.d_end is null or T2.d_end >= NOW())"
				" and ((T2.%s != 0 and T2.%s is not null) or (T4.%s != 0 and T4.%s is not null))",
				cha_table,
				cha_table, cha_column_owner,
				cha_table_black, cha_column_owner, cha_column_owner,
				cha_table_black,
				cha_column_owner, cha_column_owner,
				cha_column_owner, in_owner,
				cha_black_blog, cha_black_blog, cha_black_blog, cha_black_blog);
//			sprintf(cha_sql,
//				"select %s from %s where %s=%d and d_create <= CURDATE() and (d_end is null or d_end >= CURDATE())",
//				cha_black_blog, cha_table_black, cha_column_owner, in_owner);
		}
		dbres = Db_OpenDyna(authdb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return -1;
		}
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			chp_val = Db_FetchValue(dbres, 0);
			if (chp_val && chp_val[0]) {
				in_blog = atoi(chp_val);
			} else {
				in_blog = 0;
			}
		}
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
	}
	return in_blog;
}

int Is_Black_List(DBase *db, NLIST *nlp_out, const char *chp_access_key)
{
	DBase *authdb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[1024];
	char cha_access[1024];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_cookie[21];
	char cha_table[21];
	char cha_table_login[21];
	char cha_table_black[21];
	char cha_black_blog[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_passfile[256];
	int in_blog;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_session,"
		" c_column_owner,"
		" c_table_black,"
		" c_black_blog,"
		" c_cookie,"
		" c_table_login "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dt_owner");
	strcpy(cha_column_session, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_table_black, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "sy_blacklist");
	strcpy(cha_black_blog, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "b_blog");
	strcpy(cha_cookie, Db_FetchValue(dbres, 9) ? Db_FetchValue(dbres, 9) : "b_blog");
	strcpy(cha_table_login, Db_FetchValue(dbres, 10) ? Db_FetchValue(dbres, 10) : "b_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return -1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (chp_access_key) {
		strcpy(cha_access, chp_access_key);
	} else {
		nlp_cookie = Get_Cookie();
		if (!nlp_cookie) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			return -1;
		}
		chp_val = Get_Nlist(nlp_cookie, cha_cookie, 1);
		if (!chp_val || !*chp_val) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			Finish_Nlist(nlp_cookie);
			return -1;
		}
		strcpy(cha_access, chp_val);
		Finish_Nlist(nlp_cookie);
	}

	in_blog = 0;
	if (cha_table_black[0] && cha_black_blog[0]) {
		if (g_in_ssl_mode) {
			authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
		} else {
			authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
		}
		if (!authdb){
			Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			return -1;
		}
		if (cha_table_login[0]) {
			sprintf(cha_sql,
				" select count(*)"
				" from %s T1"
				",%s T2"
				",%s T3"
				" where T1.%s = T2.%s"
				" and T2.%s = T3.%s"
				" and T3.%s = '%s'"
				" and T2.%s != 0"
				" and T2.%s is not null",
				cha_table,
				cha_table_black,
				cha_table_login,
				cha_column_owner, cha_column_owner,
				cha_column_owner, cha_column_owner,
				cha_column_session, cha_access,
				cha_black_blog,
				cha_black_blog);
		} else {
			sprintf(cha_sql,
				" select count(*)"
				" from %s T1"
				",%s T2"
				" where T1.%s = T2.%s"
				" and T1.%s = '%s'"
				" and T2.%s != 0"
				" and T2.%s is not null",
				cha_table,
				cha_table_black,
				cha_column_owner, cha_column_owner,
				cha_column_session, cha_access,
				cha_black_blog,
				cha_black_blog);
		}
		dbres = Db_OpenDyna(authdb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return -1;
		}
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			chp_val = Db_FetchValue(dbres, 0);
			if (chp_val && chp_val[0]) {
				in_blog = atoi(chp_val);
			} else {
				in_blog = 0;
			}
		}
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
	}
	return in_blog;
}

int Is_Caution_Owner(DBase *db, NLIST *nlp_out, const char *chp_access_key)
{
	DBase *authdb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[1024];
	char cha_access[1024];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_cookie[21];
	char cha_table[21];
	char cha_table_login[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_passfile[256];
	int in_blog;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_session,"
		" c_column_owner,"
		" c_cookie,"
		" c_table_login "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dt_owner");
	strcpy(cha_column_session, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_cookie, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "b_blog");
	strcpy(cha_table_login, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "b_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return -1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (chp_access_key) {
		strcpy(cha_access, chp_access_key);
	} else {
		nlp_cookie = Get_Cookie();
		if (!nlp_cookie) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			return -1;
		}
		chp_val = Get_Nlist(nlp_cookie, cha_cookie, 1);
		if (!chp_val || !*chp_val) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			Finish_Nlist(nlp_cookie);
			return -1;
		}
		strcpy(cha_access, chp_val);
		Finish_Nlist(nlp_cookie);
	}

	in_blog = 0;
	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return -1;
	}
	if (cha_table_login[0]) {
		sprintf(cha_sql,
			" select T2.b_black"
			" from %s T1"
			",dt_owner_status T2"
			",%s T3"
			" where T1.%s=T2.%s"
			" and T2.%s=T3.%s"
			" and T3.%s='%s'",
			cha_table,
			cha_table_login,
			cha_column_owner, cha_column_owner,
			cha_column_owner, cha_column_owner,
			cha_column_session, cha_access);
	} else {
		sprintf(cha_sql,
			" select T2.b_black"
			" from %s T1"
			",dt_owner_status T2"
			" where T1.%s=T2.%s"
			" and T1.%s='%s'",
			cha_table,
			cha_column_owner, cha_column_owner,
			cha_column_session, cha_access);
	}
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	if (Db_FetchNext(dbres) == CO_SQL_OK) {
		chp_val = Db_FetchValue(dbres, 0);
		if (chp_val && chp_val[0]) {
			in_blog = atoi(chp_val);
		} else {
			in_blog = 0;
		}
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_blog;
}

/*
+* =============================================================================
 * Function:		Get_Blog_Auth
 * Description:
 * 		指定のメンバーが持っているブログへの権限を得る。
%* =============================================================================
 * Return:	権限
 *			>=0: 正常終了
 *			-1: エラー
-* =============================================================================*/
int Get_Blog_Auth(DBase *db, NLIST *nlp_out, int in_owner, int in_blog)
{
#define CO_FUNC_BLOG	19
#define CO_DELETE_AUTH	3
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_auth;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return -1;
	}
	sprintf(cha_sql, "select USEAUTH from FUNCTIONS where FUNCID=%d", CO_FUNC_BLOG);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return -1;
	}
	chp_val = Db_FetchValue(dbres, 0);
	Db_CloseDyna(dbres);
	if (chp_val && atoi(chp_val)) {
		sprintf(cha_sql, "select substring(WRITES, %d, 1) from AUTHBLOG where OBJECTID=%d", in_owner, in_blog);
		dbres = Db_OpenDyna(authdb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return -1;
		}
		if (Db_FetchNext(dbres) != CO_SQL_OK) {
			Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
			Db_CloseDyna(dbres);
			Db_Disconnect(authdb);
			return -1;
		}
		chp_val = Db_FetchValue(dbres, 0);
		if (chp_val && chp_val[0]) {
			in_auth = atoi(chp_val);
		} else {
			in_auth = 0;
		}
		Db_CloseDyna(dbres);
	} else {
		in_auth = CO_DELETE_AUTH;
	}
	Db_Disconnect(authdb);

	return in_auth;
}

/*
+* =============================================================================
 * Function:		Get_DBB_Topic
 * Description:
 * 		DBBトピックを得る
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
char *Get_DBB_Topic(DBase *db, NLIST *nlp_out, int in_blog, char *chp_func, char *chp_id, char *chp_owner, char *chp_url)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char *chp_esc;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];

	if (!chp_func || (chp_func[0] != 'C' && chp_func[0] != 'c' && chp_func[0] != 'L' && chp_func[0] != 'l' && chp_func[0] != 'B' && chp_func[0] != 'b')) {
		return NULL;
	}

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		return NULL;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		return NULL;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		return NULL;
	}

	if (chp_func[0] == 'C' || chp_func[0] == 'c') {
		sprintf(cha_sql,
			"select c_name from dt_owner_coliseum where n_owner = %s and n_coliseum = %s",
			chp_owner ? chp_owner : "0", chp_id ? chp_id : "0");
	} else if(chp_func[0] == 'B' || chp_func[0] == 'b') {
		sprintf(cha_sql,
			"select concat(T2.c_nickname, ' vs ', T3.c_nickname)"
			" from dt_battle_game T1"
			", dt_owner T2"
			", dt_owner T3"
			" where T1.n_battle = %s"
			" and T1.n_owner_first = T2.n_owner"
			" and T1.n_owner_second = T3.n_owner"
			, chp_id ? chp_id : "0");
	} else {
		if (chp_owner && chp_owner[0]) {
			sprintf(cha_sql,
				" select concat(T1.c_name, ' - ' ,T3.c_bbs, ' - ', T4.c_subject)"
				" from dt_club T1"
				",dt_bbs_club T2"
				",dt_bbs_name T3"
				",dt_bbs T4"
				" where T1.n_club=T2.n_club"
				" and T2.n_bcat_id=T3.n_bcat_id"
				" and T3.n_bbsno=T4.n_bbsno"
				" and T3.n_bbsno=%s"
				" and T4.n_no=%s"
				, (chp_id && chp_id[0]) ? chp_id : "0", chp_owner);
		} else {
			sprintf(cha_sql,
				" select concat(T1.c_name, ' - ' ,T3.c_bbs)"
				" from dt_club T1"
				",dt_bbs_club T2"
				",dt_bbs_name T3"
				" where T1.n_club=T2.n_club"
				" and T2.n_bcat_id=T3.n_bcat_id"
				" and T3.n_bbsno=%s"
				, (chp_id && chp_id[0]) ? chp_id : "0");
		}
	}
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Db_Disconnect(authdb);
		return NULL;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (chp_val) {
		chp_esc = Escape_HtmlString(chp_val);
		if (chp_url) {
			asprintf(&chp_func, "<a href=\"%s\" target=\"_blank\">%s</a>", chp_url, chp_esc);
		} else {
			asprintf(&chp_func, "%s", chp_esc);
		}
		free(chp_esc);
	} else {
		chp_func = NULL;
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return chp_func;
}

/*
+* =============================================================================
 * Function:		Add_Favorite_Blog
 * Description:
 * 		お気に入りにブログを追加する
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Is_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_cnt;
	int in_owner;
	int in_login_owner;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return -1;
	}
	in_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_login_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return -1;
	}
	in_login_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select count(*) from dt_blog_favorite where n_owner=%d and n_owner_favorite=%d", in_login_owner, in_owner);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	in_cnt = 0;
	chp_val = Db_GetValue(dbres, 0, 0);
	if (chp_val) {
		in_cnt = atoi(chp_val);
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_cnt;
}

/*
+* =============================================================================
 * Function:		Add_Favorite_Blog
 * Description:
 * 		お気に入りにブログを追加する
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Add_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_cnt;
	int in_owner;
	int in_login_owner;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	in_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_login_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	in_login_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select count(*) from dt_blog_favorite where n_owner=%d and n_owner_favorite=%d", in_login_blog, in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	in_cnt = 0;
	chp_val = Db_GetValue(dbres, 0, 0);
	if (chp_val) {
		in_cnt = atoi(chp_val);
	}
	Db_CloseDyna(dbres);
	if (!in_cnt) {
		sprintf(cha_sql, "insert into dt_blog_favorite (n_owner, n_owner_favorite) values (%d, %d)", in_login_owner, in_owner);
		if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Db_Disconnect(authdb);
			return 1;
		}
	}

	Db_Disconnect(authdb);

	return 0;
}

/*
+* =============================================================================
 * Function:		Del_Favorite_Blog
 * Description:
 * 		お気に入りにブログから削除
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Del_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_owner;
	int in_login_owner;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	in_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_login_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	in_login_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "delete from dt_blog_favorite where n_owner=%d and n_owner_favorite=%d", in_login_owner, in_owner);
	if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}

	Db_Disconnect(authdb);

	return 0;
}

/*
+* =============================================================================
 * Function:		Count_Favorite_Blog
 * Description:
 * 		お気に入りにブログ登録数取得
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Count_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_owner;
	int in_count;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return -1;
	}
	in_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select count(T1.n_owner_favorite) from dt_blog_favorite T1,dt_owner T2 where T2.b_retire = 0 and T2.n_owner = T1.n_owner and T1.n_owner_favorite = %d", in_owner);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		in_count = 0;
	} else {
		in_count = atoi(chp_val);
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_count;
}

/*
+* =============================================================================
 * Function:		Clear_Favorite_Blog
 * Description:
 * 		球友以外のブログをお気に入りにブログから削除
%* =============================================================================
 * Return:	結果
 *			= 0: 正常終了
 *			!=0: エラー
-* =============================================================================*/
int Clear_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int i;
	int in_owner;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select n_owner from dt_owner where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	in_owner = atoi(chp_val);
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select n_owner from dt_blog_favorite where n_owner_favorite = %d", in_owner);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_val = Db_GetValue(dbres, i, 0);
		if (chp_val) {
			DBRes *dbles;
			int in_favorite = atoi(chp_val);
			sprintf(cha_sql, "select count(*) from dt_owner_friend where n_status = 2 and n_owner = %d and n_owner_friend = %d", in_favorite, in_owner);
			dbles = Db_OpenDyna(authdb, cha_sql);
			if (!dbles) {
				Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				Db_CloseDyna(dbres);
				Db_Disconnect(authdb);
				return 1;
			}
			chp_val = Db_GetValue(dbles, 0, 0);
			if (!chp_val || !atoi(chp_val)) {
				Db_CloseDyna(dbles);
				sprintf(cha_sql, "select count(*) from dt_owner_friend where n_status = 2 and n_owner = %d and n_owner_friend = %d", in_owner, in_favorite);
				dbles = Db_OpenDyna(authdb, cha_sql);
				if (!dbles) {
					Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
					Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
					Db_CloseDyna(dbres);
					Db_Disconnect(authdb);
					return 1;
				}
				chp_val = Db_GetValue(dbles, 0, 0);
				if (!chp_val || !atoi(chp_val)) {
					sprintf(cha_sql, "delete from dt_blog_favorite where n_owner = %d and n_owner_favorite = %d", in_favorite, in_owner);
					if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
						Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
						Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
						Db_CloseDyna(dbres);
						Db_Disconnect(authdb);
						return 1;
					}
				}
			}
			Db_CloseDyna(dbles);
		}
	}
	Db_CloseDyna(dbres);

	Db_Disconnect(authdb);

	return 0;
}

/*
+* =============================================================================
 * Function:		Temp_To_Owner
 * Description:
 * 		Temporaryなowneridを真のowneridに変換
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Temp_To_Owner(DBase *db, NLIST *nlp_out, const char* chp_temp_id)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_blog;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select n_owner from dt_owner where c_access_id = '%s'", chp_temp_id);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%s)の持ち主が見つかりません。(%s:%d)", chp_temp_id, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return -1;
	}
	in_blog = atoi(chp_val);
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_blog;
}

/*
+* =============================================================================
 * Function:		Temp_To_Blog
 * Description:
 * 		Temporaryなblogidを真のblogidに変換
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Temp_To_Blog(DBase *db, NLIST *nlp_out, const char* chp_temp_id)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_blog;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return -1;
	}
	sprintf(cha_sql, "select n_blog_id from dt_owner where c_access_id = '%s'", chp_temp_id);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%s)の持ち主が見つかりません。(%s:%d)", chp_temp_id, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return -1;
	}
	in_blog = atoi(chp_val);
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_blog;
}

/*
+* =============================================================================
 * Function:		Owner_To_Temp
 * Description:
 * 		真のowneridをTemporaryなblogidに変換
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Owner_To_Temp(DBase *db, NLIST *nlp_out, int in_owner, char* chp_temp_id)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select c_access_id from dt_owner where n_owner = %d", in_owner);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "オーナー(%d)が見つかりません。(%s:%d)", in_owner, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	strcpy(chp_temp_id, chp_val);
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return 0;
}

/*
+* =============================================================================
 * Function:		Blog_To_Temp
 * Description:
 * 		真のblogidをTemporaryなblogidに変換
%* =============================================================================
 * Return:	権限
 *			!=NULL: 正常終了(トピック文字列)
 *			NULL: エラー
-* =============================================================================*/
int Blog_To_Temp(DBase *db, NLIST *nlp_out, int in_blog, char* chp_temp_id)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql, "select c_access_id from dt_owner where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_GetValue(dbres, 0, 0);
	if (!chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)の持ち主が見つかりません。(%s:%d)", in_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	strcpy(chp_temp_id, chp_val);
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return 0;
}

/*
+* =============================================================================
 * Function:		Accept_Only_Friend
 * Description:
 * 		球友以外のオーナーは拒否するかどうか
%* =============================================================================
 * Return:	= 0 拒否しない
 *			!=0 拒否する
 *			< 0 エラー
-* =============================================================================*/
int Accept_Only_Friend(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[255];
	char *chp_tmp;
	int in_ret;

	sprintf(cha_sql, "select b_setting_friends_only from at_blog where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログ(%d)が見つかりません。(%s:%d)<br>", in_blog, __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	in_ret = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_ret;
}

/*
+* =============================================================================
 * Function:		Is_Ball_Friend
 * Description:
 * 		訪れたオーナーが球友かどうか
%* =============================================================================
 * Return:	= 0 球友じゃない
 *			!=0 球友です
 *			< 0 エラー
-* =============================================================================*/
int Is_Ball_Friend(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_passfile[256];
	int in_ret;

	if (in_blog == in_login_blog) {
		return 1;
	}

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_passfile "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>%s", __FILE__, __LINE__, cha_sql);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_passfile, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dbb_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return -1;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
	}
	sprintf(cha_sql,
		" select count(*)"
		" from dt_owner_friend T1"
		",dt_owner T2"
		",dt_owner T3"
		" where T1.n_owner = T2.n_owner"
		" and T1.n_owner_friend = T3.n_owner"
		" and T1.n_status = 2"
		" and T2.n_blog_id = %d"
		" and T3.n_blog_id = %d"
		, in_blog, in_login_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	in_ret = 0;
	chp_val = Db_GetValue(dbres, 0, 0);
	if (chp_val) {
		in_ret = atoi(chp_val);
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_ret;
}

/*
+* =============================================================================
 * Function:		Get_String_After
 * Description:
 * 				指定の文字列のin_len文字後の文字列を返す。
 *				ただし、Tagは計算に入れない
%* =============================================================================
 * Return:
 *			0: 正常終了
 *			1: エラー
-* =============================================================================*/
int Jump_To_Login_Cgi(DBase *db, NLIST *nlp_out)
{
	DBRes *dbres;
//	CP_NKF nkf;
	char cha_sql[512];
	char cha_host[256];
	char cha_location[2048];
	char *chp_escape;
	struct passwd *stp_pwd;

	sprintf(cha_sql, "select T1.c_login_cgi from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "ログイン用CGIの取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return 1;
	}
	if (!Db_FetchValue(dbres, 0)) {
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0));
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		char *chp;
		chp = getenv("SCRIPT_NAME");
		if (chp) {
			char cha_hostname[256];
			strcpy(cha_hostname, chp);
			chp = strrchr(cha_hostname, '/');
			if (chp) {
				*chp = '\0';
			}
			Replace_String("#SCRIPT#", cha_hostname, cha_host);
		}
	}
/*
	strcpy(cha_location, getenv("REQUEST_URI"));
debug_log("%s\n", cha_location);
	Decode_Blog_Form(cha_location);
debug_log("%s\n", cha_location);
	nkf = Create_Nkf_String(cha_location);
	Set_Output_Code(nkf, NKF_EUC);
	Convert_To_String(nkf, &chp_escape);
	Destroy_Nkf_Engine(nkf);
debug_log("%s\n", chp_escape);
	free(chp_escape);
*/
	sprintf(cha_location, "%s%s%s", g_cha_protocol, getenv("SERVER_NAME"), getenv("REQUEST_URI"));
	Encode_Url(cha_location, &chp_escape);
	printf("Location: %s?blog=%s\n\n", cha_host, chp_escape);
	free(chp_escape);
	return 0;
}

/*
+* =============================================================================
 * Function:	Disp_Battle_Player チーム所属カード表示
 * Include-file:
 * Description:
 *	チームメンバーの一覧を表示する。
%* =============================================================================
 * Return:
 *      CO_RET_TRUE            :正常
 *      1           :異常
 *	CO_RET_JUMP		:ページ移動
-* =============================================================================*/
int Disp_Battle_Player(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_err, const char *chp_name, const char *chp_hidden, int in_blog)
{
#define CO_NUM_TEAM_PLAYER2			14	/* 1チーム人数 */
	DBase *authdb;
	DBRes *dbres;
	char *chp_val;
	char *chp_temp;
	char cha_move[16];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_nickname[21];
	char cha_column_owner[21];
	char cha_column_blog[21];
	char cha_sql[4096];
	char cha_date[16];
	char cha_team[65];
	int in_before;
	int in_owner;
	int in_battle_id;
	int in_team_id;
	int in_card_kind;
	int in_year;
	int i;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_nickname,"
		" c_column_owner,"
		" c_column_blog"
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_err, "ERROR", "オーダーの取得に失敗しました。<br>");
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "at_profile");
	strcpy(cha_column_nickname, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_login");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "");
	Db_CloseDyna(dbres);

	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_err, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}

	/* バトルIDを取得 */
	chp_temp = Get_Nlist(nlp_in, (char*)chp_hidden, 1);
	if(!chp_temp || *chp_temp == '\0') {
		Put_Nlist(nlp_err, "ERROR", "バトルIDが選択されていません。<br>");
		Db_Disconnect(authdb);
		return 1;
	}
	in_battle_id = atoi(chp_temp);

	sprintf(cha_sql, "select %s from %s where %s=%d", cha_column_owner, cha_table, cha_column_blog, in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "オーナーの取得に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_owner = atoi(chp_val);
	} else {
		in_owner = 0;
	}
	Db_CloseDyna(dbres);

	/* バトルIDからチームID、チーム名取得 */
	strcpy(cha_sql, "select T2.c_name");		/* 0 */
	strcat(cha_sql, ",T2.n_battle_team");		/* 1 */
	strcat(cha_sql, " from dt_battle_game_team T1");
	strcat(cha_sql, ",dt_battle_team T2");
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_battle = %d", in_battle_id);
	sprintf(&cha_sql[strlen(cha_sql)], " and T1.n_owner = %d", in_owner);
	strcat(cha_sql, " and T1.n_battle_team = T2.n_battle_team");
	strcat(cha_sql, " and T1.n_owner = T2.n_owner");
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	chp_temp = Db_GetValue(dbres, 0, 0);
	if(chp_temp) {
		strcpy(cha_team, chp_temp);
	}
	chp_temp = Db_GetValue(dbres, 0, 1);
	in_team_id = 0;
	if(chp_temp) {
		in_team_id = atoi(chp_temp);
	}
	Db_CloseDyna(dbres);

	in_year = Get_DbbSeasonYear(authdb);
	if(in_year == -1) {
		Put_Nlist(nlp_err, "ERROR", "今年度の取得ができません。<br>");
		Db_Disconnect(authdb);
		return 1;
	}
#if 0
	strcpy(cha_sql, "select T1.n_year");
	strcat(cha_sql, " from dm_season_year T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.d_season_start <= '%s'", cha_date);
	sprintf(cha_sql + strlen(cha_sql), " and T1.d_season_end >= '%s'", cha_date);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "日にちの判定に失敗しました。(%s)(%d)", Gcha_last_error, __LINE__);
		return 1;
	}
	in_year_spec = 0;
	if(Db_GetRowCount(dbres)) {
		in_year_spec = atoi(Db_GetValue(dbres, 0, 0));
	}
	Db_CloseDyna(dbres);
	if(!in_year_spec) {
		Put_Nlist(nlp_err, "ERROR", "試合のある日付を選んでください。");
		return 1;
	}

	if (in_year == in_year_spec) {
		cha_prefix[0] = '\0';
	} else {
		sprintf(cha_prefix, "_%d", in_year_spec);
	}
#endif
	/* チームメンバー表示 */
	strcpy(cha_sql, "select T2.n_player");			/* 0 プレイヤーID */
	strcat(cha_sql, ", T1.n_owner_card");			/* 1 カードID */
	strcat(cha_sql, ", T5.c_name");				/* 2 自チーム内でのポジション(名前) */
	strcat(cha_sql, ", T3.c_name");				/* 3 選手名 */
	strcat(cha_sql, ", T4.c_name");				/* 4 元々の所属チーム */
	strcat(cha_sql, ", null");				/* 5 カードレベル */
	strcat(cha_sql, ", T1.n_card_kind");			/* 6 カード種類 */
	strcat(cha_sql, ", T2.n_salary");			/* 7 年俸*/
	strcat(cha_sql, ", T1.n_order");			/* 8 打順 */
	strcat(cha_sql, ", T1.n_photo_type");			/* 9 フォトタイプ */
	strcat(cha_sql, ", T1.b_prime");			/* 10 プライム */
	strcat(cha_sql, " from dt_battle_game_order T1");
	strcat(cha_sql, ", dt_card T2");
	strcat(cha_sql, ", gt_player T3");
	strcat(cha_sql, ", gt_team T4");
	strcat(cha_sql, ", gm_position T5");
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_battle_team = %d", in_team_id);
	sprintf(&cha_sql[strlen(cha_sql)], " and T1.n_battle = %d", in_battle_id);
	strcat(cha_sql, " and T1.n_card = T2.n_card");
	strcat(cha_sql, " and T2.n_player = T3.n_player");
	strcat(cha_sql, " and T2.n_real_team = T4.n_team");
	strcat(cha_sql, " and T1.n_position = T5.n_position");
	strcat(cha_sql, " order by T1.n_order");
	dbres = Db_OpenDyna(authdb, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_err, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_err, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}

	/* 登録データ表 */
	Put_Nlist(nlp_out, (char*)chp_name, "<!-- オーダーリスト：ここから -->\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<!-- オーダーリスト内のタグは書きかえないでください。  -->\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<table border=\"0\" class=\"order_list\" cellpadding=\"0\" cellspacing=\"1\" style=\"color:#000000; font-size:10px;\">\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<tr valign=\"top\" bgcolor=\"#eeeeee\">\n");
	Put_Format_Nlist(nlp_out, (char*)chp_name, "	<td colspan=\"5\" align=\"center\" nowrap><b>%s</b></td>\n", cha_team);
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<tr valign=\"top\" align=\"center\">\n");
	Put_Format_Nlist(nlp_out, (char*)chp_name, "<td colspan=\"5\" bgcolor=\"#eeeeee\"><b>対戦のオーダーリスト<b></td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<tr valign=\"top\" bgcolor=\"#eeeeee\">\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>打順</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>守備</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>名前</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>所属</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "	<td align=\"center\" nowrap>カード</td>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	in_card_kind = 0;
	in_before = Cmp_Date(cha_date, cha_move);
	for(i = 0; i < CO_NUM_TEAM_PLAYER2; i++) {
		if (i % 2 ) {
			Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
		} else {
			Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
		}
		/* 打順 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>");
		chp_temp = Db_GetValue(dbres, i, 8);
		if(chp_temp) {
			if (atoi(chp_temp) > 9) {
				Put_Nlist(nlp_out, (char*)chp_name, "-");
			} else {
				Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
			}
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* ポジション名 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td nowrap>");
		chp_temp = Db_GetValue(dbres, i, 2);
		if(chp_temp) {
			Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* 氏名 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td nowrap>");
		chp_temp = Db_GetValue(dbres, i, 3);
		if(chp_temp) {
			Put_Format_Nlist(nlp_out, (char*)chp_name, "%s", chp_temp);
		} else {
			Put_Nlist(nlp_out, (char*)chp_name, "未選択");
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* 所属 */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td nowrap>");
		chp_temp = Db_GetValue(dbres, i, 4);
		if(chp_temp) {
			Put_Nlist(nlp_out, (char*)chp_name, chp_temp);
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		/* カード */
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>");
		chp_temp = Db_GetValue(dbres, i, 10);
		if(chp_temp && atoi(chp_temp) > 0) {
			Put_Nlist(nlp_out, (char*)chp_name, "P");
		}
		chp_temp = Db_GetValue(dbres, i, 6);
		if(chp_temp) {
			switch (atoi(chp_temp)) {
			case 2:
				Put_Nlist(nlp_out, (char*)chp_name, "G");
				break;
			case 3:
				Put_Nlist(nlp_out, (char*)chp_name, "B");
				break;
			case 4:
				Put_Nlist(nlp_out, (char*)chp_name, "W");
				break;
			case 5:
				Put_Nlist(nlp_out, (char*)chp_name, "I");
				break;
			default:
				Put_Nlist(nlp_out, (char*)chp_name, "N");
			}
		}
		chp_temp = Db_GetValue(dbres, i, 9);
		if(chp_temp) {
			switch (atoi(chp_temp)) {
			case 2:
				Put_Nlist(nlp_out, (char*)chp_name, "D");
				break;
			case 3:
				Put_Nlist(nlp_out, (char*)chp_name, "E");
				break;
			case 4:
				Put_Nlist(nlp_out, (char*)chp_name, "R");
				break;
			default:
				Put_Nlist(nlp_out, (char*)chp_name, "S");
			}
		}
		Put_Nlist(nlp_out, (char*)chp_name, "</td>\n");
		Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);
#if 0
	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"4\" nowrap>チームポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>\n", dl_point);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;

	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"4\" nowrap>基本ポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>\n", dl_base);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;

	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"4\" nowrap>ボーナスポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>", dl_bonus);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;

	if (i % 2 ) {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#F7F7FF\">\n");
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "<tr align=\"left\" bgcolor=\"#ffffff\">\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" colspan=\"4\" nowrap>合計ポイント</td>\n");
	if (in_before >= 0) {
		Put_Format_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"right\" nowrap>%.1f</td>\n", dl_total);
	} else {
		Put_Nlist(nlp_out, (char*)chp_name, "\t<td align=\"center\" nowrap>-</td>\n");
	}
	Put_Nlist(nlp_out, (char*)chp_name, "</tr>\n");
	++i;
#endif
	Put_Nlist(nlp_out, (char*)chp_name, "</table>\n");
	Put_Nlist(nlp_out, (char*)chp_name, "<!-- オーダーリスト：ここまで -->\n");

	return 0;
}

/*
+* ========================================================================
 * Function:            In_Battle_Owner
 * Description:
 *			バトルの対戦者かどうか
%* ========================================================================
 * Return:		成功 オーナーID
 *			失敗 0
-* ========================================================================*/
int In_Battle_Owner(DBase *db, NLIST *nlp_out, int in_owner, char *chp_battle)
{
	DBase *authdb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[1024];
	char cha_access[1024];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_cookie[21];
	char cha_table[21];
	char cha_table_login[21];
	char cha_column_session[21];
	char cha_column_owner[21];
	char cha_passfile[256];
	int in_blog;

	sprintf(cha_sql,
		"select T1.c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_session,"
		" c_column_owner,"
		" c_cookie,"
		" c_table_login "
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "アクセスキーの照合に失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Db_CloseDyna(dbres);
		return -1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "dt_owner");
	strcpy(cha_column_session, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "c_access_key");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "c_access_key");
	strcpy(cha_cookie, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "b_blog");
	strcpy(cha_table_login, Db_FetchValue(dbres, 8) ? Db_FetchValue(dbres, 8) : "b_blog");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
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
	} else if (strcmp(cha_password, "#DBFILE#") == 0 && strcmp(cha_user, "#DBFILE#") == 0) {
		if (Read_From_Pass_File(nlp_out, cha_passfile, cha_user, cha_password)) {
			return 0;
		}
		Replace_String("#DBUSER#", cha_user, cha_db);
	}

	if (!in_owner) {
		nlp_cookie = Get_Cookie();
		if (!nlp_cookie) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			return -1;
		}
		chp_val = Get_Nlist(nlp_cookie, cha_cookie, 1);
		if (!chp_val || !*chp_val) {
		Put_Format_Nlist(nlp_out, "ERROR", "正常にログインされていません。(%s:%d)<br>", __FILE__, __LINE__);
			Finish_Nlist(nlp_cookie);
			return -1;
		}
		strcpy(cha_access, chp_val);
		Finish_Nlist(nlp_cookie);
	}

	in_blog = 0;
	if (g_in_ssl_mode) {
		authdb = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できません。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return -1;
	}
	if (!in_owner) {
		if (cha_table_login[0]) {
			sprintf(cha_sql,
				" select count(*)"
				" from %s T1"
				",%s T2"
				",%s T3"
				" where (T1.%s=T2.n_owner_first"
				" or T1.%s=T2.n_owner_second)"
				" and T1.%s=T3.%s"
				" and T3.%s='%s'"
				" and T2.n_battle=%s",
				cha_table,
				"dt_battle_game",
				cha_table_login,
				cha_column_owner,
				cha_column_owner,
				cha_column_owner, cha_column_owner,
				cha_column_session, cha_access,
				chp_battle);
		} else {
			sprintf(cha_sql,
				" select T2.count(*)"
				" from %s T1"
				",%s T2"
				" where (T1.%s=T2.n_owner_first"
				" or T1.%s=T2.n_owner_second)"
				" and T1.%s='%s'"
				" and T2.n_battle=%s",
				cha_table,
				"dt_battle_game",
				cha_column_owner,
				cha_column_owner,
				cha_column_session, cha_access,
				chp_battle);
		}
	} else {
		sprintf(cha_sql,
			" select count(*)"
			" from %s T1"
			",%s T2"
			" where (T1.%s=T2.n_owner_first"
			" or T1.%s=T2.n_owner_second)"
			" and T1.%s=%d"
			" and T2.n_battle=%s",
			cha_table,
			"dt_battle_game",
			cha_column_owner,
			cha_column_owner,
			cha_column_owner, in_owner,
			chp_battle);
	}
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return -1;
	}
	if (Db_FetchNext(dbres) == CO_SQL_OK) {
		chp_val = Db_FetchValue(dbres, 0);
		if (chp_val && chp_val[0]) {
			in_blog = atoi(chp_val);
		} else {
			in_blog = 0;
		}
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);
	return in_blog;
}
