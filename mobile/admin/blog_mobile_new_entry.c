#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "libcgi2.h"
#include "libblog.h"
#include "libauth.h"
#include "libmobile.h"
#include "blog_mobile_new_entry.h"

int error_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out);

int Post_Mobile_Admin_New_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	int in_rt;
	int in_owner;
	int in_black;
	int in_error;
	int in_mode;
	int in_point;
	int in_new_entry_id;
	char cha_cookie[128];
	char cha_num[256];
	char cha_sql[16384];
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	char *chp_string;
	char *chp_escape;
	char *chp_euc;

	in_owner = Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_sql);
	in_black = In_Black_List(db, nlp_out, in_owner, NULL);
	if (in_black) {
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		return 1;
	}
	in_error = error_check(db, nlp_in, nlp_out);
	if (in_error) {
		return in_error;
	}
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	if ((!chp_tmp || atoi(chp_tmp) != 0) && check_future(db, nlp_in, nlp_out, in_login_blog)) {
		in_error++;
		return in_error;
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

	sprintf(cha_sql, "select n_next_entry from at_profile where n_blog_id = %d for update", in_login_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "新規投稿ｸｴﾘーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_new_entry_id = atoi(chp_tmp);
	} else {
		in_new_entry_id = 1;
	}
	Db_CloseDyna(dbres);

	if (Begin_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "ﾄﾗﾝｻﾞｸｼｮﾝ開始に失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	strcpy(cha_sql, "insert into at_entry");
	strcat(cha_sql, "(n_blog_id");
	strcat(cha_sql, ",n_entry_id");
	strcat(cha_sql, ",n_category_id");
	strcat(cha_sql, ",b_comment");
	strcat(cha_sql, ",b_trackback");
	strcat(cha_sql, ",b_mode");
	strcat(cha_sql, ",c_entry_title");
	strcat(cha_sql, ",c_entry_summary");
	strcat(cha_sql, ",c_entry_body");
	strcat(cha_sql, ",c_entry_more");
	strcat(cha_sql, ",d_entry_create_time");
	strcat(cha_sql, ",d_entry_real_time");
	strcat(cha_sql, ",d_entry_modify_time");
	strcat(cha_sql, ",c_item_id");
	strcat(cha_sql, ",c_item_kind");
	strcat(cha_sql, ",c_item_owner");
	strcat(cha_sql, ",c_item_url");
	strcat(cha_sql, ",n_point");
	sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d", in_login_blog, in_new_entry_id);	/* n_entry_id */
	strcat(cha_sql, ", ");
	strcat(cha_sql, "0");
	strcat(cha_sql, ", ");
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	sprintf(cha_sql + strlen(cha_sql), "%d", atoi(chp_tmp));	/* b_comment */
	strcat(cha_sql, ", ");
	chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
	sprintf(cha_sql + strlen(cha_sql), "%d", atoi(chp_tmp));	/* b_trackback */
	strcat(cha_sql, ", ");
	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	in_mode = chp_tmp ? atoi(chp_tmp) : 1;
	sprintf(cha_sql + strlen(cha_sql), "%d", in_mode);	/* b_mode */
	strcat(cha_sql, ", '");
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);	/* c_entry_title */
	chp_euc = Sjis_to_Euc(chp_tmp);
	chp_escape = My_Escape_SqlString(db, chp_euc);
	strcat(cha_sql, chp_escape);
	free(chp_euc);
	free(chp_escape);
	strcat(cha_sql, "', '");
	chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
	if (chp_tmp && *chp_tmp) {
		chp_euc = Sjis_to_Euc(chp_tmp);
		chp_escape = My_Escape_SqlString(db, chp_euc);	/* c_entry_summary */
		strcat(cha_sql, chp_escape);
		free(chp_euc);
		free(chp_escape);
	}
	strcat(cha_sql, "', '");
	sprintf(cha_num, "%d", in_new_entry_id);
	chp_tmp3 = Get_Nlist(nlp_in, "entry_body", 1);	/* c_entry_body */
	chp_euc = Sjis_to_Euc(chp_tmp3);
	chp_tmp2 = Remove_Invalid_Tags(chp_euc, nlp_out, 0);
	if (!chp_tmp2) {
		free(chp_euc);
		Rollback_Transact(db);
		return CO_ERROR;
	}
	in_point = 0;
	chp_string = malloc(strlen(chp_tmp2) + 1);
	strcpy(chp_string, chp_tmp2);
	in_point = Calc_Body_Point(db, nlp_out, in_login_blog, in_new_entry_id, 1, chp_string, Get_Nlist(nlp_in, cha_cookie, 1));
	free(chp_string);
	if (in_point == INT_MAX) {
		Rollback_Transact(db);
		return CO_ERROR;
	}
	chp_escape = My_Escape_SqlString(db, chp_tmp2);
	strcat(cha_sql, chp_escape);
	free(chp_escape);
	free(chp_tmp2);
	free(chp_euc);
	strcat(cha_sql, "', '");
	if (Get_Nlist(nlp_in, "INSERT_ORDER", 1)) {
		NLIST *nlp_tmp;
		char *chp_order;
		chp_order = NULL;
		chp_escape = NULL;
		nlp_tmp = Init_Nlist();
		if (Disp_Team_Player(db, nlp_in, nlp_tmp, nlp_out, "TEMP", "team")) {
			Rollback_Transact(db);
			Finish_Nlist(nlp_tmp);
			return CO_ERROR;
		}
		Conv_Depend_String(nlp_tmp);
		chp_order = Page_Out_Mem(nlp_tmp, "blog_temp.skl");
		if (chp_order) {
			chp_escape = My_Escape_SqlString(db, chp_order);	/* c_entry_more */
			strcat(cha_sql, chp_escape);
			free(chp_escape);
			free(chp_order);
		}
		Finish_Nlist(nlp_tmp);
	}
	strcat(cha_sql, "'");
	/* 更新時には、specify_dateがない */
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	if (!chp_tmp || atoi(chp_tmp) != 0) {
		/* d_entry_create_time */
		strcat(cha_sql, ", '");
		strcat(cha_sql, Get_Nlist(nlp_in, "year", 1));
		strcat(cha_sql, "-");
		strcat(cha_sql, Get_Nlist(nlp_in, "month", 1));
		strcat(cha_sql, "-");
		strcat(cha_sql, Get_Nlist(nlp_in, "day", 1));
		strcat(cha_sql, " ");
		strcat(cha_sql, Get_Nlist(nlp_in, "hour", 1));
		strcat(cha_sql, ":");
		strcat(cha_sql, Get_Nlist(nlp_in, "minute", 1));
		strcat(cha_sql, ":00'");
	} else {
		/* d_entry_create_time */
		strcat(cha_sql, ", now()");
	}
	/* d_entry_real_time */
	strcat(cha_sql, ", now()");
	/* d_entry_modify_time */
	strcat(cha_sql, ", now()");
	strcat(cha_sql, ", '");
	chp_tmp = Get_Nlist(nlp_in, "b", 1);
	if (chp_tmp && *chp_tmp) {
		strcat(cha_sql, chp_tmp);
	}
	strcat(cha_sql, "','");
	chp_tmp = Get_Nlist(nlp_in, "f", 1);
	if (chp_tmp && *chp_tmp) {
		strcat(cha_sql, chp_tmp);
	}
	strcat(cha_sql, "',");
	chp_tmp = Get_Nlist(nlp_in, "o", 1);
	if (chp_tmp && *chp_tmp) {
		in_rt = Temp_To_Owner(db, nlp_out, chp_tmp);
		if (in_rt > 0) {
			sprintf(cha_sql + strlen(cha_sql), "%d", in_rt);
		} else {
			strcat(cha_sql, "null");
		}
	} else {
		strcat(cha_sql, "null");
	}
	strcat(cha_sql, ",'");
	chp_tmp = Get_Nlist(nlp_in, "l", 1);
	if (chp_tmp && *chp_tmp) {
		strcat(cha_sql, chp_tmp);
	}
	sprintf(cha_sql + strlen(cha_sql), "', %d)", in_point);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "新規投稿ｸｴﾘーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return CO_ERROR;
	}

	sprintf(cha_sql, "update at_profile set n_next_entry=n_next_entry+1 where n_blog_id = %d", in_login_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "新規投稿ｸｴﾘーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return CO_ERROR;
	}

	/* 公開 */
	in_rt = 0;
	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		/*ﾄﾗｯｸﾊﾞｯｸPING送信*/
		chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
		if (trackback(db, nlp_in, nlp_out, in_new_entry_id, chp_tmp, in_login_blog, 0, Get_Nlist(nlp_in, cha_cookie, 1))){
			++in_rt;
		}
	}
	if (Commit_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "ｺﾐｯﾄに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return CO_ERROR;
	}

	Put_Nlist(nlp_out, "ERROR", "記事を投稿しました。<br>");
	Mobile_Error_Top(db, nlp_in, nlp_out, "ADMIN_URL", 1);

	return 0;
}

int Get_Mobile_Admin_New_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	NLIST *nlp_tmp;
	char cha_sql[512];
	char cha_year[32];
	char cha_month[32];
	char cha_day[32];
	char cha_hour[32];
	char cha_minute[32];
	char *chp_euc;
	char *chp_esc;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	int i;
	int in_owner;
	int in_black;
	int in_year;
	int in_deftb;
	int in_defcm;
	int in_defmd;

	in_owner = Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_sql);
	in_black = In_Black_List(db, nlp_out, in_owner, NULL);
	if (in_black) {
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	if (chp_tmp) {
		chp_euc = Sjis_to_Euc(chp_tmp);
		chp_esc = Escape_HtmlString(chp_euc);
		Put_Nlist(nlp_out, "ENTRYTITLE", chp_esc);
		free(chp_esc);
		free(chp_euc);
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_body", 1);
	if (chp_tmp) {
		chp_euc = Sjis_to_Euc(chp_tmp);
		chp_esc = Escape_HtmlString(chp_euc);
		Put_Nlist(nlp_out, "ENTRYBODY", chp_esc);
		free(chp_esc);
		free(chp_euc);
	}
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	if (chp_tmp) {
		if (atoi(chp_tmp)) {
			Put_Nlist(nlp_out, "SD1", /*" checked"*/" selected");
		} else {
			Put_Nlist(nlp_out, "SD0", /*" checked"*/" selected");
		}
	} else {
		Put_Nlist(nlp_out, "SD0", /*" checked"*/" selected");
	}

	Get_NowDateTime(cha_year, cha_hour);
	cha_year[4] = '\0';
	cha_year[7] = '\0';
	strcpy(cha_month, cha_year + 5);
	strcpy(cha_day, cha_year + 8);

	chp_tmp = Get_Nlist(nlp_in, "year", 1);
	if (chp_tmp) {
		strcpy(cha_year, chp_tmp);
	}
	in_year = atoi(cha_year);
	sprintf(cha_sql,
		" select distinct"
		" date_format(d_entry_create_time, '%%Y') as n_year"
		" from at_entry"
		" where n_blog_id = %d"
		" and d_entry_create_time < now()"
		" and b_mode != 0"
		" order by n_year;",
		in_login_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	if (Db_GetRowCount(dbres)) {
		for (i = 0; i < Db_GetRowCount(dbres); ++i) {
			chp_tmp = Db_GetValue(dbres, i, 0);
			if (chp_tmp) {
				nlp_tmp = Init_Nlist();
				Put_Nlist(nlp_tmp, "YEAR", chp_tmp);
				if (strcmp(cha_year, chp_tmp) == 0) {
					Put_Nlist(nlp_tmp, "SELECTED", "selected");
				}
				in_year = atoi(chp_tmp);
				Conv_Depend_String(nlp_tmp);
				chp_tmp = Page_Out_Mem(nlp_tmp, "parts_year_select_option.skl");
				if (chp_tmp) {
					Put_Nlist(nlp_out, "YEARS", chp_tmp);
					free(chp_tmp);
				}
				Finish_Nlist(nlp_tmp);
			}
		}
	} else {
		nlp_tmp = Init_Nlist();
		Put_Nlist(nlp_tmp, "YEAR", cha_year);
		Put_Nlist(nlp_tmp, "SELECTED", "selected");
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_year_select_option.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "YEARS", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	}
	Db_CloseDyna(dbres);

	nlp_tmp = Init_Nlist();
	++in_year;
	sprintf(cha_sql, "%d", in_year);
	Put_Nlist(nlp_tmp, "YEAR", cha_sql);
	Conv_Depend_String(nlp_tmp);
	chp_tmp = Page_Out_Mem(nlp_tmp, "parts_year_select_option.skl");
	if (chp_tmp) {
		Put_Nlist(nlp_out, "YEARS", chp_tmp);
		free(chp_tmp);
	}
	Finish_Nlist(nlp_tmp);

	chp_tmp = Get_Nlist(nlp_in, "month", 1);
	if (chp_tmp) {
		strcpy(cha_month, chp_tmp);
	}
	sprintf(cha_sql, "MON%s", cha_month);
	Put_Nlist(nlp_out, cha_sql, " selected");

	chp_tmp = Get_Nlist(nlp_in, "day", 1);
	if (chp_tmp) {
		strcpy(cha_day, chp_tmp);
	}
	sprintf(cha_sql, "DAY%s", cha_day);
	Put_Nlist(nlp_out, cha_sql, " selected");

	cha_hour[2] = '\0';
	cha_hour[5] = '\0';
	strcpy(cha_minute, cha_hour + 3);

	chp_tmp = Get_Nlist(nlp_in, "hour", 1);
	if (chp_tmp) {
		strcpy(cha_hour, chp_tmp);
	}
	sprintf(cha_sql, "HR%s", cha_hour);
	Put_Nlist(nlp_out, cha_sql, " selected");

	chp_tmp = Get_Nlist(nlp_in, "minute", 1);
	if (chp_tmp) {
		strcpy(cha_minute, chp_tmp);
	}
	sprintf(cha_sql, "MIN%s", cha_minute);
	Put_Nlist(nlp_out, cha_sql, " selected");

	/* 初期値に沿ってﾗｼﾞｵﾎﾞﾀﾝのﾁｪｯｸ */
	strcpy(cha_sql, " select T1.b_default_trackback");	/* 0 ﾄﾗｯｸﾊﾞｯｸ初期値 */
	strcat(cha_sql, ",T1.b_default_comment");			/* 1 ｺﾒﾝﾄ初期値 */
	strcat(cha_sql, ",T1.b_default_mode");				/* 2 投稿ﾓーﾄﾞ初期値 */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ﾗｼﾞｵﾎﾞﾀﾝ初期値を得るｸｴﾘに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_deftb = atoi(chp_tmp);
	} else {
		in_deftb = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_defcm = atoi(chp_tmp);
	} else {
		in_defcm = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		in_defmd = atoi(chp_tmp);
	} else {
		in_defmd = 0;
	}
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
	if (chp_tmp) {
		sprintf(cha_sql, "TRACKBACK%s", chp_tmp);
		Put_Nlist(nlp_out, cha_sql, /*" checked"*/" selected");
	} else {
		sprintf(cha_sql, "TRACKBACK%d", in_deftb);
		Put_Nlist(nlp_out, cha_sql, /*" checked"*/" selected");
	}

	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (chp_tmp) {
		sprintf(cha_sql, "COMMENT%s", chp_tmp);
		Put_Nlist(nlp_out, cha_sql, /*" checked"*/" selected");
	} else {
		sprintf(cha_sql, "COMMENT%d", in_defcm);
		Put_Nlist(nlp_out, cha_sql, /*" checked"*/" selected");
	}

	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	if (chp_tmp) {
		sprintf(cha_sql, "MODE%s", chp_tmp);
		Put_Nlist(nlp_out, cha_sql, /*" checked"*/" selected");
	} else {
		sprintf(cha_sql, "MODE%d", in_defmd);
		Put_Nlist(nlp_out, cha_sql, /*" checked"*/" selected");
	}

	chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
	if (chp_tmp) {
		chp_euc = Sjis_to_Euc(chp_tmp);
		chp_esc = Escape_HtmlString(chp_euc);
		Put_Nlist(nlp_out, "SUMMARY", chp_esc);
		free(chp_esc);
		free(chp_euc);
	}

	if (Get_Nlist(nlp_in, "BTN_EASY_TRACKBACK", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "blog", 1);
		chp_tmp2 = Get_Nlist(nlp_in, "eid", 1);
		if (chp_tmp && chp_tmp2) {
			Put_Format_Nlist(nlp_out, "TRACKBACKURL", "%s%s%s/%s/%s-%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_TB, chp_tmp, chp_tmp2);
		}
	}

	chp_tmp = Get_Nlist(nlp_in, "INSERT_ORDER", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "INSORD", " checked");
	}
	cha_sql[0] = '\0';
	chp_tmp = Get_Nlist(nlp_in, "ORDYEAR", 1);
	chp_tmp2 = Get_Nlist(nlp_in, "ORDMONTH", 1);
	chp_tmp3 = Get_Nlist(nlp_in, "ORDDAY", 1);
	if (chp_tmp && chp_tmp2 && chp_tmp3) {
		sprintf(cha_sql, "%s/%s/%s", chp_tmp, chp_tmp2, chp_tmp3);
	}
	if (Mobile_Team_Combo(db, nlp_in, nlp_out, in_login_blog, "team", NULL, cha_sql)) {
		return 1;
	}

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_login_blog, NULL);

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_new_entry.skl");

	return 0;
}

int Post_Mobile_Admin_Cancel_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char cha_cookie[32];
	char cha_cgi[1024];
	char *chp_owner;
	char *chp_key;

	cha_cookie[0] = '\0';
	cha_cgi[0] = '\0';
	if(g_in_dbb_mode) {
		sprintf(cha_sql, "select c_m_cookie,c_login_mobile_cgi from sy_authinfo");
	} else {
		sprintf(cha_sql, "select c_cookie,c_login_mobile_cgi from sy_authinfo");
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres) {
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			strcpy(cha_cookie, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "dbb_access_key");
			strcpy(cha_cgi, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : g_cha_mobile_admin_cgi);
		}
		Db_CloseDyna(dbres);
	}

	chp_owner = Get_Nlist(nlp_in, "owner", 1);
	chp_key = Get_Nlist(nlp_in, cha_cookie, 1);
	printf("Location: %s?owner=%s&%s=%s\n\n", cha_cgi, chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");

	return 0;
}

int Mobile_Admin_New_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	int in_error;
	char *chpa_hidden[] = {
		"trackback_url", "dbb_topic_url", "entry_title", "entry_body",
		"INSERT_ORDER", "ORDYEAR", "ORDMONTH", "ORDDAY",
		"specify_date", "year", "month", "day", "hour", "minute", "team",
		"trackback", "comment", "entry_mode", "entry_summary", NULL
	};
	if (Is_Post_Method() && Get_Nlist(nlp_in, "BTN_EDIT_ENTRY", 1)) {
		in_error = Post_Mobile_Admin_New_Entry(db, nlp_in, nlp_out, in_login_blog);
	} else if (Is_Post_Method() && Get_Nlist(nlp_in, "cancel", 1)) {
		in_error = Post_Mobile_Admin_Cancel_Entry(db, nlp_in, nlp_out, in_login_blog);
	} else {
		in_error = Get_Mobile_Admin_New_Entry(db, nlp_in, nlp_out, in_login_blog);
	}
	if (in_error) {
		Mobile_Error(db, nlp_in, nlp_out, "mn", chpa_hidden);
	}
	return in_error;
}
