#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmobile.h"
#include "libauth.h"
#include "libblog.h"
#include "blog_mobile_top_page.h"
#include "blog_mobile_show_list.h"
#include "blog_mobile_write_comment.h"
#include "blog_mobile_comment_list.h"
#include "blog_mobile_tb_list.h"
#include "blog_mobile_about_blog.h"
#include "blog_mobile_monthly_archive.h"
#include "blog_mobile_monthly_list.h"

typedef int PageProc(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog);
typedef struct _st_pageitem {
	char		*chp_name;
	PageProc	*stp_proc;
} st_pageitem;

int Mobile_User_Add_Favorite(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	int in_entry;
	char cha_cookie[32];
	char cha_sql[8192];
	char *chp_eid;
	char *chp_tmp;
	char *chp_start;
	char *chp_owner;
	char *chp_key;

	if (Add_Favorite_Blog(db, nlp_out, in_blog, in_login_blog)) {
		return 1;
	}

	in_entry = 0;
	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if(!chp_eid || Check_Numeric(chp_eid)) {
		sprintf(cha_sql,
			" select  T1.n_entry_id"
			" from at_entry T1"
			" where T1.n_blog_id = %d"
			" and T1.d_entry_create_time < now()"
			" and T1.b_mode != 0"
			" order by T1.d_entry_create_time desc"
			" limit 1",
			in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Put_Nlist(nlp_out, "ERROR", cha_sql);
			return 1;
		}
		chp_eid = Db_GetValue(dbres, 0, 0);
		if (chp_eid) {
			in_entry = atoi(chp_eid);
		}
		Db_CloseDyna(dbres);
	} else {
		in_entry = atoi(chp_eid);
	}

	cha_cookie[0] = '\0';
	if(g_in_dbb_mode) {
		sprintf(cha_sql, "select c_m_cookie from sy_authinfo");
	} else {
		sprintf(cha_sql, "select c_cookie from sy_authinfo");
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres) {
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			strcpy(cha_cookie, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "dbb_access_key");
		}
		Db_CloseDyna(dbres);
	}

	chp_tmp = Get_Nlist(nlp_in, "blog", 1);
	chp_start = Get_Nlist(nlp_in, "start", 1);
	chp_owner = Get_Nlist(nlp_in, "owner", 1);
	chp_key = Get_Nlist(nlp_in, cha_cookie, 1);
	printf("Location: %s%s%s/%s?pg=tp&eid=%d&start=%s&blog=%s&owner=%s&%s=%s\n\n",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, g_cha_mobile_user_cgi, in_entry,
		chp_start ? chp_start : "0", chp_tmp ? chp_tmp : "0",
		chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");

	return 0;
}

int Mobile_User_Del_Favorite(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	int in_entry;
	char cha_cookie[32];
	char cha_sql[8192];
	char *chp_eid;
	char *chp_tmp;
	char *chp_start;
	char *chp_owner;
	char *chp_key;

	if (Del_Favorite_Blog(db, nlp_out, in_blog, in_login_blog)) {
		return 1;
	}

	in_entry = 0;
	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if(!chp_eid || Check_Numeric(chp_eid)) {
		sprintf(cha_sql,
			" select  T1.n_entry_id"
			" from at_entry T1"
			" where T1.n_blog_id = %d"
			" and T1.d_entry_create_time < now()"
			" and T1.b_mode != 0"
			" order by T1.d_entry_create_time desc"
			" limit 1",
			in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Put_Nlist(nlp_out, "ERROR", cha_sql);
			return 1;
		}
		chp_eid = Db_GetValue(dbres, 0, 0);
		if (chp_eid) {
			in_entry = atoi(chp_eid);
		}
		Db_CloseDyna(dbres);
	} else {
		in_entry = atoi(chp_eid);
	}

	cha_cookie[0] = '\0';
	if(g_in_dbb_mode) {
		sprintf(cha_sql, "select c_m_cookie from sy_authinfo");
	} else {
		sprintf(cha_sql, "select c_cookie from sy_authinfo");
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres) {
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			strcpy(cha_cookie, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "dbb_access_key");
		}
		Db_CloseDyna(dbres);
	}

	chp_tmp = Get_Nlist(nlp_in, "blog", 1);
	chp_start = Get_Nlist(nlp_in, "start", 1);
	chp_owner = Get_Nlist(nlp_in, "owner", 1);
	chp_key = Get_Nlist(nlp_in, cha_cookie, 1);
	printf("Location: %s%s%s/%s?pg=tp&eid=%d&start=%s&blog=%s&owner=%s&%s=%s\n\n",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, g_cha_mobile_user_cgi, in_entry,
		chp_start ? chp_start : "0", chp_tmp ? chp_tmp : "0",
		chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");

	return 0;
}

st_pageitem sta_pageprocs[] = {
	{"tp", Mobile_User_Top_Page},
	{"sl", Mobile_User_Show_List},
	{"wc", Mobile_User_Write_Comment},
	{"cl", Mobile_User_Comment_List},
	{"tl", Mobile_User_Trackback_List},
	{"ab", Mobile_User_About_Blog},
	{"ma", Mobile_User_Monthly_Archive},
	{"ml", Mobile_User_Monthly_List},
	{"af", Mobile_User_Add_Favorite},
	{"df", Mobile_User_Del_Favorite},
};

int main()
{
	NLIST *nlp_in;
	NLIST *nlp_out;
	DBase *db;
	int i;
	int in_error;
	int in_blog;
	int in_owner;
	int in_admin;
	int in_login_blog;
	char cha_mess[1024];
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char *chp_blog;
	char *chp_page;

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	db = NULL;
	in_blog = 0;
	in_login_blog = 0;
	chp_page = "";

	in_error = Read_Blog_Ini(cha_host,cha_db, cha_username, cha_password, &g_in_ssl_mode);
	if (in_error) {
		sprintf(cha_mess, "INIﾌｧｲﾙの読み込みに失敗しました。(%s)", strerror(in_error));
		Put_Nlist(nlp_out, "ERROR", cha_mess);
		goto clear_finish;
	}
	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		sprintf(cha_mess, "ﾃﾞーﾀﾍﾞーｽ接続に失敗しました。(%s - %s - %s - %s)", cha_host, cha_db, cha_username, cha_password);
		Put_Nlist(nlp_out, "ERROR", cha_mess);
		++in_error;
		goto clear_finish;
	}
	if (Read_Blog_Option(db)) {
		Put_Nlist(nlp_out, "ERROR", "ﾌﾞﾛｸﾞｵﾌﾟｼｮﾝの読み込みに失敗しました。");
		++in_error;
		goto clear_finish;
	}
	Set_SkelPath(g_cha_user_skeleton);

	chp_blog = Get_Nlist(nlp_in, "owner", 1);
	in_owner = chp_blog ? atoi(chp_blog) : 0;
	in_login_blog = Check_Mobile_Login(db, nlp_in, nlp_out, in_owner, &in_admin);
	if (in_login_blog < 0) {
		++in_error;
		goto clear_finish;
	}
	if (!in_login_blog) {
		if (Jump_To_Login_Mobile_Cgi(db, nlp_out)) {
			++in_error;
		}
		goto clear_finish;
	}
	g_in_admin_mode = in_admin;

	chp_blog = Get_Nlist(nlp_in, "blog", 1);
	in_blog = Temp_To_Blog(db, nlp_out, chp_blog);
	if (in_blog < 0) {
		++in_error;
		goto clear_finish;
	}

	if (!in_admin && in_blog != in_login_blog) {
		update_access_conter(db, in_blog, in_login_blog);
	}

	chp_page = Get_Nlist(nlp_in, "pg", 1);
	if (!chp_page) {
		chp_page = "tp";
	}
	for (i = 0; i < (int)(sizeof(sta_pageprocs)/sizeof(st_pageitem)); ++i) {
		if (!strcmp(sta_pageprocs[i].chp_name, chp_page)) {
			in_error = sta_pageprocs[i].stp_proc(db, nlp_in, nlp_out, in_blog, in_login_blog);
			break;
		}
	}

clear_finish:
	if (in_error) {
		if (strcmp(chp_page, "tp") == 0) {
			Mobile_Error_Top(db, nlp_in, nlp_out, "USER_URL", 0);
		} else {
			Mobile_Error(db, nlp_in, nlp_out, NULL, NULL);
		}
	} else {
		update_visitor_list(db, nlp_in, nlp_out, in_blog, in_login_blog);
	}
	if (db) {
		Db_Disconnect(db);
	}
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);

	return in_error;
}
