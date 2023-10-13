#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <limits.h>
#include <sys/types.h>
#include "libnet.h"
#include "libnkf.h"
#include "libblog.h"
#include "libauth.h"
#include "libmobile.h"

char g_cha_mobile_admin_cgi[] = "blog_mobile_admin.cgi";
char g_cha_mobile_user_cgi[] = "blog_mobile_user.cgi";

static struct {
	int in_single;
	char *chp_hankana;
} st_conv_hankana_table[] = {
	{ 0xa0, " " },
	{ 0xa1, "ｧ" },
	{ 0xa2, "ｱ" },
	{ 0xa3, "ｨ" },
	{ 0xa4, "ｲ" },
	{ 0xa5, "ｩ" },
	{ 0xa6, "ｳ" },
	{ 0xa7, "ｪ" },
	{ 0xa8, "ｴ" },
	{ 0xa9, "ｫ" },
	{ 0xaa, "ｵ" },
	{ 0xab, "ｶ" },
	{ 0xac, "ｶﾞ" },
	{ 0xad, "ｷ" },
	{ 0xae, "ｷﾞ" },
	{ 0xaf, "ｸ" },
	{ 0xb0, "ｸﾞ" },
	{ 0xb1, "ｹ" },
	{ 0xb2, "ｹﾞ" },
	{ 0xb3, "ｺ" },
	{ 0xb4, "ｺﾞ" },
	{ 0xb5, "ｻ" },
	{ 0xb6, "ｻﾞ" },
	{ 0xb7, "ｼ" },
	{ 0xb8, "ｼﾞ" },
	{ 0xb9, "ｽ" },
	{ 0xba, "ｽﾞ" },
	{ 0xbb, "ｾ" },
	{ 0xbc, "ｾﾞ" },
	{ 0xbd, "ｿ" },
	{ 0xbe, "ｿﾞ" },
	{ 0xbf, "ﾀ" },
	{ 0xc0, "ﾀﾞ" },
	{ 0xc1, "ﾁ" },
	{ 0xc2, "ﾁﾞ" },
	{ 0xc3, "ｯ" },
	{ 0xc4, "ﾂ" },
	{ 0xc5, "ﾂﾞ" },
	{ 0xc6, "ﾃ" },
	{ 0xc7, "ﾃﾞ" },
	{ 0xc8, "ﾄ" },
	{ 0xc9, "ﾄﾞ" },
	{ 0xca, "ﾅ" },
	{ 0xcb, "ﾆ" },
	{ 0xcc, "ﾇ" },
	{ 0xcd, "ﾈ" },
	{ 0xce, "ﾉ" },
	{ 0xcf, "ﾊ" },
	{ 0xd0, "ﾊﾞ" },
	{ 0xd1, "ﾊﾟ" },
	{ 0xd2, "ﾋ" },
	{ 0xd3, "ﾋﾞ" },
	{ 0xd4, "ﾋﾟ" },
	{ 0xd5, "ﾌ" },
	{ 0xd6, "ﾌﾞ" },
	{ 0xd7, "ﾌﾟ" },
	{ 0xd8, "ﾍ" },
	{ 0xd9, "ﾍﾞ" },
	{ 0xda, "ﾍﾟ" },
	{ 0xdb, "ﾎ" },
	{ 0xdc, "ﾎﾞ" },
	{ 0xdd, "ﾎﾟ" },
	{ 0xde, "ﾏ" },
	{ 0xdf, "ﾐ" },
	{ 0xe0, "ﾑ" },
	{ 0xe1, "ﾒ" },
	{ 0xe2, "ﾓ" },
	{ 0xe3, "ｬ" },
	{ 0xe4, "ﾔ" },
	{ 0xe5, "ｭ" },
	{ 0xe6, "ﾕ" },
	{ 0xe7, "ｮ" },
	{ 0xe8, "ﾖ" },
	{ 0xe9, "ﾗ" },
	{ 0xea, "ﾘ" },
	{ 0xeb, "ﾙ" },
	{ 0xec, "ﾚ" },
	{ 0xed, "ﾛ" },
	{ 0xee, "ヮ" },
	{ 0xef, "ﾜ" },
	{ 0xf0, "ヰ" },
	{ 0xf1, "ヱ" },
	{ 0xf2, "ｦ" },
	{ 0xf3, "ﾝ" },
	{ 0xf4, "ｳﾞ" },
	{ 0xf5, "ヵ" },
	{ 0xf6, "ヶ" },
	{ 0xf7, "" },
	{ 0xf8, "" },
	{ 0xf9, "" },
	{ 0xfa, "" },
	{ 0xfb, "" },
	{ 0xfc, "" },
	{ 0xfd, "" },
	{ 0xfe, "" },
	{ 0xff, "" },
	{ 0, 0 }
};

static struct {
	int in_single;
	char *chp_hankana;
} st_conv_hankana_table2[] = {
	{ 0xa3, "｡" },
	{ 0xd6, "｢" },
	{ 0xd7, "｣" },
	{ 0xa2, "､" },
	{ 0xa6, "･" },
	{ 0xbc, "ｰ" },
	{ 0, 0 }
};

void Conv_Depend_String(NLIST *nlp_out)
{
	int in_type;
	in_type = Get_Handytype();
	switch (in_type) {
	case CO_USER_DOCOMO:
		Put_Nlist(nlp_out, "DOCTYPE", "<!DOCTYPE html PUBLIC \"-//i-mode group (ja)//DTD XHTML i-XHTML(Locale/Ver.=ja/1.0) 1.0//EN\" \"i-xhtml_4ja_10.dtd\">");
		Put_Nlist(nlp_out, "ACCESSKEY", "accesskey");
		Put_Nlist(nlp_out, "NUMBERKEY1", "&#63879;");
		Put_Nlist(nlp_out, "NUMBERKEY2", "&#63880;");
		Put_Nlist(nlp_out, "NUMBERKEY3", "&#63881;");
		Put_Nlist(nlp_out, "NUMBERKEY4", "&#63882;");
		Put_Nlist(nlp_out, "NUMBERKEY5", "&#63883;");
		Put_Nlist(nlp_out, "NUMBERKEY6", "&#63884;");
		Put_Nlist(nlp_out, "NUMBERKEY7", "&#63885;");
		Put_Nlist(nlp_out, "NUMBERKEY8", "&#63886;");
		Put_Nlist(nlp_out, "NUMBERKEY9", "&#63887;");
		Put_Nlist(nlp_out, "NUMBERKEY0", "&#63888;");
		Put_Nlist(nlp_out, "NUMBERKEYS", "[#]");
		Put_Nlist(nlp_out, "NUMBERKEYA", "[*]");
		break;
	case CO_USER_AU:
		Put_Nlist(nlp_out, "DOCTYPE", "<!DOCTYPE html PUBLIC \"-//OPENWAVE//DTD XHTML 1.0//EN\" \"http://www.openwave.com/DTD/xhtml-basic.dtd\">");
		Put_Nlist(nlp_out, "ACCESSKEY", "accesskey");
		Put_Nlist(nlp_out, "NUMBERKEY1", "&#63879;");
		Put_Nlist(nlp_out, "NUMBERKEY2", "&#63880;");
		Put_Nlist(nlp_out, "NUMBERKEY3", "&#63881;");
		Put_Nlist(nlp_out, "NUMBERKEY4", "&#63882;");
		Put_Nlist(nlp_out, "NUMBERKEY5", "&#63883;");
		Put_Nlist(nlp_out, "NUMBERKEY6", "&#63884;");
		Put_Nlist(nlp_out, "NUMBERKEY7", "&#63885;");
		Put_Nlist(nlp_out, "NUMBERKEY8", "&#63886;");
		Put_Nlist(nlp_out, "NUMBERKEY9", "&#63887;");
		Put_Nlist(nlp_out, "NUMBERKEY0", "&#63888;");
		Put_Nlist(nlp_out, "NUMBERKEYS", "&#62601;");
		Put_Nlist(nlp_out, "NUMBERKEYA", "[*]");
		break;
	case CO_USER_JPHONE:
		Put_Nlist(nlp_out, "DOCTYPE", "<!DOCTYPE html PUBLIC \"-//J-PHONE//DTD XHTML Basic 1.0 Plus//EN\" \"xhtml-basic10-plus.dtd\">");
		Put_Nlist(nlp_out, "ACCESSKEY", "directkey");
		Put_Nlist(nlp_out, "NUMBERKEY1", "&#xE21C;"/*"&#27;$F<&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY2", "&#xE21D;"/*"&#27;$F=&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY3", "&#xE21E;"/*"&#27;$F>&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY4", "&#xE21F;"/*"&#27;$F?&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY5", "&#xE220;"/*"&#27;$F@&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY6", "&#xE221;"/*"&#27;$FA&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY7", "&#xE222;"/*"&#27;$FB&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY8", "&#xE223;"/*"&#27;$FC&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY9", "&#xE224;"/*"&#27;$FD&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEY0", "&#xE225;"/*"&#27;$FE&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEYS", "&#xE210;"/*"&#27;$F0&#15;"*/);
		Put_Nlist(nlp_out, "NUMBERKEYA", "[*]");
		break;
	default:
		Put_Nlist(nlp_out, "DOCTYPE", "<!DOCTYPE html PUBLIC \"-//i-mode group (ja)//DTD XHTML i-XHTML(Locale/Ver.=ja/1.0) 1.0//EN\" \"i-xhtml_4ja_10.dtd\">");
		Put_Nlist(nlp_out, "ACCESSKEY", "accesskey");
		Put_Nlist(nlp_out, "NUMBERKEY1", "&#63879;");
		Put_Nlist(nlp_out, "NUMBERKEY2", "&#63880;");
		Put_Nlist(nlp_out, "NUMBERKEY3", "&#63881;");
		Put_Nlist(nlp_out, "NUMBERKEY4", "&#63882;");
		Put_Nlist(nlp_out, "NUMBERKEY5", "&#63883;");
		Put_Nlist(nlp_out, "NUMBERKEY6", "&#63884;");
		Put_Nlist(nlp_out, "NUMBERKEY7", "&#63885;");
		Put_Nlist(nlp_out, "NUMBERKEY8", "&#63886;");
		Put_Nlist(nlp_out, "NUMBERKEY9", "&#63887;");
		Put_Nlist(nlp_out, "NUMBERKEY0", "&#63888;");
		Put_Nlist(nlp_out, "NUMBERKEYS", "[#]");
		Put_Nlist(nlp_out, "NUMBERKEYA", "[*]");
		break;
	}
}

/*
+* ========================================================================
 * Function:	Check_Mobile_Login
 * Description:
 *
%* ========================================================================
 * Return:	<0	エラー
 * 			=0	未ログイン
 *			>0	ログイン済みブログID
-* ======================================================================
 */
int Check_Mobile_Login(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_owner, int *inp_admin)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char *chp_val;
	char cha_sql[255];
	char cha_access[1024];
	char cha_host[256];
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

	*inp_admin = 0;
	sprintf(cha_sql,
		"select c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_login,"
		" c_column_password,"
		" c_column_session,"
		" c_column_blog,");
	if(g_in_dbb_mode) {
		strcat(cha_sql,
			   " c_m_cookie,"
			   " c_m_owner_cookie,");
	} else {
		strcat(cha_sql,
			   " c_cookie,"
			   " c_owner_cookie,");
	}
	strcat(cha_sql,
		" c_column_owner,"
		" c_cookie_host,"
		" c_cookie_path,"
		" c_passfile,"
		" c_table_login "
		" from sy_authinfo");
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

	chp_val = Get_Nlist(nlp_in, cha_cookie, 1);
	if (!chp_val || !*chp_val) {
		return 0;
	}
	strcpy(cha_access, chp_val);
	cha_owner[0] = '\0';
	if (cha_owner_cookie[0]) {
		chp_val = Get_Nlist(nlp_in, cha_owner_cookie, 1);
		if (chp_val && *chp_val) {
			strcpy(cha_owner, chp_val);
		}
	}

	authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return -1;
	}
	if (cha_table_login[0]) {
		if (cha_owner_cookie[0] && cha_owner[0]) {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.%s='%s' and T1.%s=%s and coalesce(T1.b_retire, 0)=0",
					cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_column_session, cha_access, cha_column_owner, cha_owner);
			} else {
				sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.%s='%s' and T1.%s=%s",
					cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_column_session, cha_access, cha_column_owner, cha_owner);
			}
		} else {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.%s='%s' and coalesce(T1.b_retire,0)=0",
					cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_column_session, cha_access);
			} else {
				sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.%s='%s'",
					cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_column_session, cha_access);
			}
		}
	} else {
		if (cha_owner_cookie[0] && cha_owner[0]) {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select %s,%s from %s where %s='%s' and %s=%s and coalesce(T1.b_retire,0)=0",
					cha_column_blog, cha_column_owner, cha_table, cha_column_session, cha_access, cha_column_owner, cha_owner);
			} else {
				sprintf(cha_sql, "select %s,%s from %s where %s='%s' and %s=%s",
					cha_column_blog, cha_column_owner, cha_table, cha_column_session, cha_access, cha_column_owner, cha_owner);
			}
		} else {
			if (g_in_dbb_mode) {
				sprintf(cha_sql, "select %s,%s from %s where %s='%s' and coalesce(T1.b_retire,0)=0",
					cha_column_blog, cha_column_owner, cha_table, cha_column_session, cha_access);
			} else {
				sprintf(cha_sql, "select %s,%s from %s where %s='%s'",
					cha_column_blog, cha_column_owner, cha_table, cha_column_session, cha_access);
			}
		}
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
		if (cha_owner_cookie[0] && cha_owner[0]) {
			sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.c_admin_access_key='%s' and T1.%s=%s",
				cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_access, cha_column_owner, cha_owner);
		} else {
			sprintf(cha_sql, "select T1.%s,T1.%s from %s T1,%s T2 where T1.%s=T2.%s and T2.c_admin_access_key='%s'",
				cha_column_blog, cha_column_owner, cha_table, cha_table_login, cha_column_owner, cha_column_owner, cha_access);
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
		*inp_admin = 1;
	}
	chp_val = Db_FetchValue(dbres, 0);
	if (chp_val && chp_val[0]) {
		in_blog = atoi(chp_val);
	} else {
		in_blog = INT_MAX;
	}
	chp_val = Db_FetchValue(dbres, 1);
	if (chp_val && chp_val[0]) {
		if (in_owner != atoi(chp_val)) {
			in_blog = 0;
		}
	} else {
		in_blog = 0;
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(authdb);

	return in_blog;
}

/*
+* =============================================================================
 * Function:		Jump_To_Login_Mobile_Cgi
 * Description:
 * 		指定の文字列のin_len文字後の文字列を返す。
 *		ただし、Tagは計算に入れない
%* =============================================================================
 * Return:
 *		0: 正常終了
 *		1: エラー
-* =============================================================================*/
int Jump_To_Login_Mobile_Cgi(DBase *db, NLIST *nlp_out)
{
	DBRes *dbres;
	char cha_sql[512];
	char cha_host[256];
	struct passwd *stp_pwd;

	sprintf(cha_sql, "select T1.c_login_mobile_cgi from sy_authinfo T1");
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
		return 1;
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
	printf("Location: %s\n\n", cha_host);

	return 0;
}

void Mobile_Prepare_Form(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_page)
{
	DBRes *dbres;
	char cha_image_path[256];
	char cha_sql[512];
	char cha_cookie[128];
	char cha_cgi[256];
	char cha_dbbtop[256];
	char cha_ownerroom[256];
	char cha_inquiry[256];
	char *chp_tmp;

	cha_cookie[0] = '\0';
	cha_image_path[0] = '\0';
	cha_cgi[0] = '\0';
	cha_dbbtop[0] = '\0';
	cha_ownerroom[0] = '\0';
	cha_inquiry[0] = '\0';
	if(g_in_dbb_mode) {
		sprintf(cha_sql, "select c_dbb_mobile_image_location,c_m_cookie,c_login_mobile_cgi,c_dbb_mobile_cgi,c_dbb_mobile_owner_cgi,c_dbb_mobile_inquiry_cgi from sy_authinfo");
	} else {
		sprintf(cha_sql, "select c_dbb_mobile_image_location,c_cookie,c_login_mobile_cgi,c_dbb_mobile_cgi,c_dbb_mobile_owner_cgi,c_dbb_mobile_inquiry_cgi from sy_authinfo");
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres) {
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			strcpy(cha_image_path, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "/images/");
			strcpy(cha_cookie, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_access_key");
			strcpy(cha_cgi, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : g_cha_mobile_admin_cgi);
			strcpy(cha_dbbtop, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : g_cha_mobile_admin_cgi);
			strcpy(cha_ownerroom, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : g_cha_mobile_admin_cgi);
			strcpy(cha_inquiry, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : g_cha_mobile_admin_cgi);
		}
		Db_CloseDyna(dbres);
	}

	if (chp_page) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "pg", chp_page);
	}
	chp_tmp = Get_Nlist(nlp_in, "blog", 1);
	if (chp_tmp && chp_tmp[0]) {
		Put_Nlist(nlp_out, "BLOG", chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "blog", chp_tmp);
	} else if (in_blog && !Blog_To_Temp(db, nlp_out, in_blog, cha_sql)) {
		Put_Nlist(nlp_out, "BLOG", cha_sql);
		Build_HiddenEncode(nlp_out, "HIDDEN", "blog", cha_sql);
	}
	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "owner", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "OWNER", chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "owner", chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, cha_cookie, 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "KEY", chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", cha_cookie, chp_tmp);
	}
	Put_Nlist(nlp_out, "IMAGE_PATH", cha_image_path);
	Put_Format_Nlist(nlp_out, "ADMIN_URL", "%s%s%s/%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, g_cha_mobile_admin_cgi);
	Put_Format_Nlist(nlp_out, "USER_URL", "%s%s%s/%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, g_cha_mobile_user_cgi);
	Put_Nlist(nlp_out, "TOP_URL", cha_cgi);
	Put_Nlist(nlp_out, "HOME_URL", cha_dbbtop);
	Put_Nlist(nlp_out, "OWNER_URL", cha_ownerroom);
	Put_Nlist(nlp_out, "INQUIRY_URL", cha_inquiry);
}

void Mobile_Error(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_page, char **chpp_hidden)
{
	Mobile_Prepare_Form(db, nlp_in, nlp_out, 0, chp_page);
	if (chpp_hidden) {
		int i = 0;
		while (chpp_hidden[i]) {
			char *chp_tmp = Get_Nlist(nlp_in, chpp_hidden[i], 1);
			if (chp_tmp) {
				char *chp_euc = Sjis_to_Euc(chp_tmp);
				char *chp_esc = Escape_HtmlString(chp_euc);
				Build_HiddenEncode(nlp_out, "HIDDEN", chpp_hidden[i], chp_euc);
				free(chp_esc);
				free(chp_euc);
			}
			++i;
		}
	}
	Page_Out_Mobile(nlp_out, "mobile_error.skl");
}

void Mobile_Error_Top(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_name, int in_confirm)
{
	DBRes *dbres;
	char cha_image_path[256];
	char cha_sql[512];
	char cha_cookie[128];
	char cha_cgi[1024];
	char *chp_tmp;

	cha_cookie[0] = '\0';
	cha_image_path[0] = '\0';
	if(g_in_dbb_mode) {
		sprintf(cha_sql, "select c_dbb_mobile_image_location,c_m_cookie,c_login_mobile_cgi from sy_authinfo");
	} else {
		sprintf(cha_sql, "select c_dbb_mobile_image_location,c_cookie,c_login_mobile_cgi from sy_authinfo");
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres) {
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			strcpy(cha_image_path, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "/images/");
			strcpy(cha_cookie, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_access_key");
			strcpy(cha_cgi, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : g_cha_mobile_admin_cgi);
		}
		Db_CloseDyna(dbres);
	}

	if (chp_name) {
		Put_Nlist(nlp_out, chp_name, cha_cgi);
	}
	chp_tmp = Get_Nlist(nlp_in, "owner", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "OWNER", chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "owner", chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, cha_cookie, 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "KEY", chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", cha_cookie, chp_tmp);
	}
	Put_Nlist(nlp_out, "IMAGE_PATH", cha_image_path);

	if (in_confirm) {
		Page_Out_Mobile(nlp_out, "mobile_commit.skl");
	} else {
		Page_Out_Mobile(nlp_out, "mobile_error.skl");
	}
}

void Mobile_Confirm(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_page, char **chpp_hidden)
{
	Mobile_Prepare_Form(db, nlp_in, nlp_out, 0, chp_page);
	if (chpp_hidden) {
		int i = 0;
		while (chpp_hidden[i]) {
			char *chp_tmp = Get_Nlist(nlp_in, chpp_hidden[i], 1);
			if (chp_tmp) {
				Build_HiddenEncode(nlp_out, "HIDDEN", chpp_hidden[i], chp_tmp);
			}
			++i;
		}
	}
	Page_Out_Mobile(nlp_out, "mobile_confirm.skl");
}


/*
+* ========================================================================
 * Function:		POSTリクエストかどうか
 * Include-file:
 * Description:
 *	環境変数REQUEST_METHODを読み、POSTリクエストかどうかを判別する。
%* ========================================================================
 * Return:
 *	戻り値 1:POST、0:POSTではない
-* ======================================================================*/
int Is_Post_Method()
{
	char* chp_tmp;
	chp_tmp = getenv("REQUEST_METHOD");
	if (chp_tmp && strcasecmp(chp_tmp, "POST") == 0) {
		return 1;
	}
	return 0;
}


/*
+* =============================================================================
 * Function:		Mobile_Team_Combo
 * Description:
 * 				チーム一覧コンボボックス作成
%* =============================================================================
 * Return:
 *			0: 正常終了
 *			1: エラー
-* =============================================================================*/
int Mobile_Team_Combo(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, const char *chp_hidden, const char *chp_team, const char *chp_date)
{
	DBase *authdb;
	DBRes *dbres;
	NLIST *nlp_tmp;
	char *chp_val;
	char *chp_id;
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
	int i;
	int in_owner;
	int in_year_last;
	int in_jacket_flag;		/* ジャケットの裏地を表示するか */
	int in_cap_flag;		/* 帽子用の髪の毛に切り替えるか */
	int in_jacket_item_id;

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
		" c_dbb_mobile_image_location"
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>%s<br>%s<br>", __FILE__, __LINE__, Gcha_last_error, cha_sql);
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

	authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	if (!authdb){
		Put_Format_Nlist(nlp_out, "ERROR", "データベースに接続できませんでした。(%s:%d)<br>%s<br>", __FILE__, __LINE__, Gcha_last_error);
		return 1;
	}

	sprintf(cha_sql, "select %s from %s where %s=%d", cha_column_owner, cha_table, cha_column_blog, in_blog);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>%s<br>%s<br>", __FILE__, __LINE__, Gcha_last_error, cha_sql);
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
			Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>%s<br>%s<br>", __FILE__, __LINE__, Gcha_last_error, cha_sql);
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
	if (chp_date && chp_date[0]) {
		strcpy(cha_date, chp_date);
	} else {
		Get_NowDate(cha_date);
	}
	cha_date[4] = '\0';
	cha_date[7] = '\0';
	in_year_last = atoi(cha_date);
	for (i = 2006; i <= in_year_last; ++i) {
		nlp_tmp = Init_Nlist();
		Put_Format_Nlist(nlp_tmp, "YEAR", "%d", i);
		if (atoi(cha_date) == i) {
			Put_Nlist(nlp_tmp, "SELECTED", "selected");
		}
		chp_val = Page_Out_Mem(nlp_tmp, "parts_year_select_option.skl");
		if (chp_val) {
			Put_Nlist(nlp_out, "ORDYEARS", chp_val);
			free(chp_val);
		}
		Finish_Nlist(nlp_tmp);
	}
	sprintf(cha_sql, "OMON%s", cha_date + 5);
	Put_Nlist(nlp_out, cha_sql, " selected");
	sprintf(cha_sql, "ODAY%s", cha_date + 8);
	Put_Nlist(nlp_out, cha_sql, " selected");

	sprintf(cha_sql, "select T1.n_team,T2.c_name from dt_team T1,dt_team_detail T2 where T1.n_team = T2.n_team and T1.n_owner = %d", in_owner);
	dbres = Db_OpenDyna(authdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "クエリに失敗しました。(%s:%d)<br>%s<br>%s<br>", __FILE__, __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		Put_Format_Nlist(nlp_out, "ERROR", "保有チームが見つかりません。(%s:%d)<br>%s<br>", __FILE__, __LINE__, cha_sql);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		nlp_tmp = Init_Nlist();
		chp_id = Db_GetValue(dbres, i, 0);
		if (chp_id) {
			Put_Nlist(nlp_tmp, "VALUE", chp_id);
		}
		chp_val = Db_GetValue(dbres, i, 1);
		if (chp_val) {
			Put_Nlist(nlp_tmp, "TEAM", chp_val);
			if (strcmp(chp_id, cha_host) == 0) {
				Put_Nlist(nlp_tmp, "SELECT", " selected");
			}
		}
		chp_val = Page_Out_Mem(nlp_tmp, "parts_team_list.skl");
		if (chp_val) {
			Put_Nlist(nlp_out, "TEAMS", chp_val);
			free(chp_val);
		}
		Finish_Nlist(nlp_tmp);
	}
	Db_Disconnect(authdb);
	return 0;
}

char *Sjis_to_Euc(char *chp_string)
{
	CP_NKF nkf;
	char *chp_search;

	nkf = Create_Nkf();
	Set_Input_String(nkf, chp_string);
	Set_Input_Code(nkf, NKF_SJIS);
	Set_Output_Code(nkf, NKF_EUC);
	Set_Assume_Hankaku_Kana(nkf, 0);
	Convert_To_String(nkf, &chp_search);
	Destroy_Nkf_Engine(nkf);
	return chp_search;
}

/*
+* ------------------------------------------------------------------------
 * Function:
 *		U_conv_hankana
 * Description:
 *		EUC全角カタカナをEUC半角カタカナに変換
 *		他の文字は処理しない
 *		セルの限られたデザイン用途以外に使用しないこと
%* ------------------------------------------------------------------------
 * Return:
 *		無し
-* ----------------------------------------------------------------------*/
void conv_hankana(unsigned char *chup_source, char *chp_str)
{
	int in_length;
	int i, j, k;

	i = 0;
	j = 0;
	k = 0;
	in_length = strlen(chup_source);
	while(i < in_length) {
		if (*(chup_source + i) == 0xa5 && i + 1 < in_length) {
			for(j = 0; st_conv_hankana_table[j].in_single != 0; j++) {
				if(*(chup_source + i + 1) == st_conv_hankana_table[j].in_single) {
					strcat(chp_str, st_conv_hankana_table[j].chp_hankana);
					k += strlen(st_conv_hankana_table[j].chp_hankana);
					break;
				}
			}
			i += 2;
		} else if(*(chup_source + i) == 0xa1 && i + 1 < in_length) {
			int b_hankana = 0;
			for(j = 0; st_conv_hankana_table2[j].in_single != 0; j++) {
				if(*(chup_source + i + 1) == st_conv_hankana_table2[j].in_single) {
					strcat(chp_str, st_conv_hankana_table2[j].chp_hankana);
					k += strlen(st_conv_hankana_table2[j].chp_hankana);
					b_hankana = 1;
					break;
				}
			}
			if(!b_hankana) {
				*(chp_str + k) = *(chup_source + i);
				*(chp_str + k + 1) = *(chup_source + i + 1);
				k += 2;
			}
			i += 2;
		} else if ((*(chup_source + i) == 0x8e || *(chup_source + i) > 0xa1) && i + 1 < in_length) {
			*(chp_str + k) = *(chup_source + i);
			*(chp_str + k + 1) = *(chup_source + i + 1);
			i += 2;
			k += 2;
/*
		} else if(*(chup_source + i) >= 32 && *(chup_source + i) < 128) {
			*(chp_str + k) = *(chup_source + i);
			i++;
			k++;
		} else if(*(chup_source + i) > 0xa1 && i + 1 != in_length) {
			*(chp_str + k) = *(chup_source + i);
			*(chp_str + k + 1) = *(chup_source + i + 1);
			i += 2;
			k += 2;
*/
		} else {
			*(chp_str + k) = *(chup_source + i);
			i++;
			k++;
		}
	}
	return;
}

int Page_Out_Mobile(NLIST *nlp_name, char *chp_skeleton)
{
	CP_NKF nkf;
	char *chp_exp;
	char *chp_search;

	printf("Content-type: text/html;charset=Shift_JIS\n");
	chp_exp = Page_Out_Mem(nlp_name, chp_skeleton);
	if (chp_exp) {
		chp_search = malloc(strlen(chp_exp) * 2 + 1);
		bzero(chp_search, strlen(chp_exp) * 2 + 1);
//		conv_hankana((unsigned char*)chp_exp, chp_search);
		strcpy(chp_search, chp_exp);
		free(chp_exp);
		nkf = Create_Nkf();
		Set_Input_String(nkf, chp_search);
		Set_Assume_Hankaku_Kana(nkf, 0);
		Set_Input_Code(nkf, NKF_EUC);
		Set_Output_Code(nkf, NKF_SJIS);
		Convert_To_String(nkf, &chp_exp);
		Destroy_Nkf_Engine(nkf);
		free(chp_search);
		printf("Content-length: %d\n", strlen(chp_exp));
		printf("Pragma: no-cache\n");
		printf("Cache-Control: no-cache\n");
		printf("Expires: -1\n\n");
		printf("%s", chp_exp);
	} else {
		asprintf(&chp_exp, "Can not allocate memory in Page_Out_Mobile.\n");
		printf("Content-length: %d\n\n", strlen(chp_exp));
		printf("%s", chp_exp);
	}
	free(chp_exp);

	return 0;
}
