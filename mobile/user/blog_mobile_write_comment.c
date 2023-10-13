#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libnet.h"
#include "libblog.h"
#include "libauth.h"
#include "libmobile.h"
#include "blog_mobile_write_comment.h"

int Get_Mobile_User_Write_Comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	int in_entry;
	char cha_sql[8192];
	char cha_nickname[100];
	char *chp_tmp;
	char *chp_esc;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "記事を特定できません。<br>");
		return 1;
	}
	Put_Nlist(nlp_out, "EID", chp_tmp);
	in_entry = atoi(chp_tmp);
	Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp);

	chp_tmp = Get_Nlist(nlp_in, "start", 1);
	Put_Nlist(nlp_out, "START", chp_tmp ? chp_tmp : "0");

	sprintf(cha_sql,
		" select T2.c_entry_title"
		",date_format(T2.d_entry_create_time,'%%Y年%%m月%%d日')"
		" from at_entry T2"
		" where T2.n_blog_id = %d"
		" and T2.n_entry_id = %d", in_blog, in_entry);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "TITLE", chp_esc);
		free(chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "DATETIME", chp_esc);
		free(chp_tmp);
	}
	Db_CloseDyna(dbres);

	if (!Get_Nickname_From_Blog(db, nlp_out, in_blog, cha_nickname)) {
		return 1;
	}
	Put_Nlist(nlp_out, "NICKNAME", cha_nickname);

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_blog, NULL);

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_write_comment.skl");

	return 0;
}

int Post_Mobile_User_Write_Comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	char *chp_eid;
	char *chp_escape;
	char *chp_author_mail;
	char *chp_comment;
	char *chp_server;
	char *chp_start;
	char *chp_owner;
	char *chp_key;
	char *chp_tmp;
	char *chp_euc;
	char cha_cookie[32];
	char cha_sql[8192];
	char cha_error[2048];
	char cha_subject[1024];
	char cha_system[256];
	char cha_sender[256];
	char cha_author_url[256];
	char cha_author_name[100];
	int in_match;
	int in_accept;
	int in_comment_id;
	int in_error = 0;

	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if(!chp_eid || Check_Numeric(chp_eid)) {
		Put_Nlist(nlp_out, "ERROR", "記事の指定に誤りがあります。");
		return 1;
	}
	if(Get_Flag_Public(db, nlp_out, atoi(chp_eid), in_blog) != 1) {
		Put_Nlist(nlp_out, "ERROR", "記事が存在しません。");
		return 1;
	}
	if(Get_Flag_Acceptable_Comment(db, nlp_out, atoi(chp_eid), in_blog, in_login_blog) == 0) {
		Put_Nlist(nlp_out, "ERROR", "この記事はｺﾒﾝﾄを受け付けていません。");
		return 1;
	}
	/* 名前 */
	if (!Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_author_name)) {
		return 1;
	}
	/* URL */
	if (Blog_To_Temp(db, nlp_out, in_login_blog, cha_sql)) {
		in_error++;
	} else {
		sprintf(cha_author_url, "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, cha_sql);
	}
	chp_comment = Get_Nlist(nlp_in, "comment", 1);
	if(!chp_comment || strlen(chp_comment) == 0 || Check_Space_Only(chp_comment)) {
		Put_Nlist(nlp_out, "ERROR", "ｺﾒﾝﾄを入力してください。<br>");
		in_error++;
	} else if(strlen(chp_comment) > CO_MAXLEN_COMMENT) {
		Put_Format_Nlist(nlp_out, "ERROR", "ｺﾒﾝﾄは半角%d文字（全角%d文字）以内で入力してください。<br>"
			, CO_MAXLEN_COMMENT, CO_MAXLEN_COMMENT/2);
		in_error++;
	}
	if(in_error) {
		return 1;
	}

	sprintf(cha_sql, "select b_comment from at_entry T1 where T1.n_blog_id = %d and n_entry_id = %s", in_blog, chp_eid);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_accept = atoi(chp_tmp);
	} else {
		in_accept = 0;
	}
	Db_CloseDyna(dbres);

	if(Begin_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "ﾄﾗﾝｻﾞｸｼｮﾝ開始に失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	sprintf(cha_sql, "select coalesce(max(T1.n_comment_id), 0) + 1 from at_comment T1 where T1.n_blog_id = %d for update", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_comment_id = atoi(chp_tmp);
	} else {
		in_comment_id = 1;
	}
	Db_CloseDyna(dbres);

	/* ｾﾚｸﾄｲﾝｻーﾄで重複をできるだけ防ぐ */
	strcpy(cha_sql, "insert into at_comment");
	strcat(cha_sql, "(n_blog_id");
	strcat(cha_sql, ",n_comment_id");
	strcat(cha_sql, ",n_entry_id");
	strcat(cha_sql, ",c_comment_author");
	strcat(cha_sql, ",c_comment_ip");
	strcat(cha_sql, ",c_comment_mail");
	strcat(cha_sql, ",c_comment_url");
	strcat(cha_sql, ",c_comment_body");
	strcat(cha_sql, ",d_comment_create_time");
	strcat(cha_sql, ",b_comment_read");
	if (in_accept == 2) {
		strcat(cha_sql, ",b_comment_accept");
	}
	sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d", in_blog, in_comment_id);	/* 0 ｺﾒﾝﾄID */
	strcat(cha_sql, ", ");
	strcat(cha_sql, chp_eid);		/* 1 ｴﾝﾄﾘID */
	strcat(cha_sql, ", '");
	chp_escape = My_Escape_SqlString(db, cha_author_name);
	strcat(cha_sql, chp_escape);
	free(chp_escape);				/* 2 名前 */
	strcat(cha_sql, "', '");
	chp_tmp = Get_Nlist(nlp_in, "owner", 1);
	if (chp_tmp) {
		strcat(cha_sql, chp_tmp);			/* 3 ip or owner */
	}
	strcat(cha_sql, "', '");
	strcat(cha_sql, "', '");
	strcat(cha_sql, cha_author_url);
	strcat(cha_sql, "', '");
	chp_euc = Sjis_to_Euc(chp_comment);
	chp_escape = My_Escape_SqlString(db, chp_euc);
	strcat(cha_sql, chp_escape);			/* 6 comment */
	free(chp_escape);
	free(chp_euc);
	strcat(cha_sql, "', now()");
	sprintf(cha_sql + strlen(cha_sql), ", %d", in_blog == in_login_blog);
	if (in_accept == 2) {
		strcat(cha_sql, ", 0");
	}
	strcat(cha_sql, ")");
	if(Db_ExecSql(db, cha_sql)) {
		Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	in_match = 0;
	sprintf(cha_sql, "select c_filter from at_comment_filter where b_valid != 0 and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(dbres) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp && chp_tmp[0]) {
			in_match = Match_Filter(chp_comment, chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	if (in_match) {
		sprintf(cha_sql, "update at_comment set b_comment_accept = 0, b_comment_filter = 1 where n_blog_id = %d and n_entry_id = %s and n_comment_id = %d", in_blog, chp_eid, in_comment_id);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
	}
	if(Commit_Transact(db)) {
		Put_Format_Nlist(nlp_out, "ERROR", "ｺﾐｯﾄに失敗しました。<br>%d:%s<br>", __LINE__, Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}
	dbres = Db_OpenDyna(db, "select c_systemname,c_managermail from sy_baseinfo");
	if(dbres) {
		strcpy(cha_system, Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "blog");
		strcpy(cha_sender, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "manager@asj.ad.jp");
		Db_CloseDyna(dbres);
	}
	sprintf(cha_sql, "select c_author_mail from at_profile where b_send_mail != 0 and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(dbres) {
		chp_author_mail = Db_GetValue(dbres, 0, 0);
		if (chp_author_mail) {
			chp_server = getenv("SERVER_NAME");
			if(chp_server && *chp_server) {
				sprintf(cha_subject, "[%s]ｺﾒﾝﾄが追加されました", cha_system);
				sprintf(cha_error, "ｺﾒﾝﾄが追加されました。下記URLで確認できます。\n\n%s%s%s/%s/?eid=%s#c%d\n"
					, g_cha_protocol, chp_server, g_cha_base_location, g_cha_blog_temp, chp_eid, in_comment_id);
				Send_Mail(cha_sender, chp_author_mail, cha_subject, cha_error);
			}
		}
		Db_CloseDyna(dbres);
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
	printf("Location: %s%s%s/%s?pg=tp&eid=%s&start=%s&blog=%s&owner=%s&%s=%s\n\n",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, g_cha_mobile_user_cgi, chp_eid,
		chp_start ? chp_start : "0", chp_tmp ? chp_tmp : "0",
		chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");

	return 0;
}

int Post_Mobile_User_Cancel_Comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	char cha_cookie[32];
	char cha_sql[8192];
	char *chp_eid;
	char *chp_tmp;
	char *chp_start;
	char *chp_owner;
	char *chp_key;

	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if(!chp_eid || Check_Numeric(chp_eid)) {
		Put_Nlist(nlp_out, "ERROR", "記事の指定に誤りがあります。");
		return 1;
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
	printf("Location: %s%s%s/%s?pg=tp&eid=%s&start=%s&blog=%s&owner=%s&%s=%s\n\n",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, g_cha_mobile_user_cgi, chp_eid,
		chp_start ? chp_start : "0", chp_tmp ? chp_tmp : "0",
		chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");

	return 0;
}

int Mobile_User_Write_Comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	int in_error;
	char *chpa_hidden[] = {"eid", "start", NULL};
	if (Is_Post_Method() && Get_Nlist(nlp_in, "WRITE_COMMENT", 1)) {
		in_error = Post_Mobile_User_Write_Comment(db, nlp_in, nlp_out, in_blog, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "wc", chpa_hidden);
		}
	} else if (Is_Post_Method() && Get_Nlist(nlp_in, "cancel", 1)) {
		in_error = Post_Mobile_User_Cancel_Comment(db, nlp_in, nlp_out, in_blog, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "wc", chpa_hidden);
		}
	} else {
		in_error = Get_Mobile_User_Write_Comment(db, nlp_in, nlp_out, in_blog, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "sa", chpa_hidden);
		}
	}
	return 0;
}
