#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "libcgi2.h"
#include "libblog.h"
#include "libauth.h"
#include "libmobile.h"
#include "blog_mobile_new_entry.h"

int error_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out);
char *Sjis_to_Euc(char *chp_string);
void Find_Team(char *chp_more, int *inp_year, int *inp_month, int *inp_day, char *chp_team);

int Post_Mobile_Admin_Modify_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	int in_entry_id;
	int in_point;
	int in_owner;
	int in_black;
	int in_rt;
	char cha_num[32];
	char cha_date[32];
	char cha_time[32];
	char cha_cookie[128];
	char cha_sql[16384];
	char *chp_euc;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	char *chp_escape;
	char *chpa_hidden[] = {NULL};

	in_owner = Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_sql);
	in_black = In_Black_List(db, nlp_out, in_owner, NULL);
	if (in_black) {
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		return 1;
	}

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "記事IDを取得できませんでした。<br>");
		return 1;
	}
	in_entry_id = atoi(chp_tmp);
	if (Exist_Entry_Id(db, nlp_out, in_entry_id, in_login_blog) == 0) {
		Put_Nlist(nlp_out, "ERROR", "その記事はありません。");
		return 1;
	}
/*
	in_specify = 0;
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	if (chp_tmp) {
		in_specify = atoi(chp_tmp);
	}
	if ((!chp_tmp || atoi(chp_tmp) != 0) && check_future(db, nlp_in, nlp_out, in_login_blog)) {
		return 1;
	}
*/
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
	if (error_check(db, nlp_in, nlp_out)) {
		return 1;
	}

	if (Begin_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "ﾄﾗﾝｻﾞｸｼｮﾝの開始に失敗しました<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}

	strcpy(cha_sql, "update at_entry");
	strcat(cha_sql, " set n_category_id = ");
	chp_tmp = Get_Nlist(nlp_in, "entry_category", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		strcat(cha_sql, chp_tmp);
	} else {
		strcat(cha_sql, "0");
	}
	strcat(cha_sql, ", b_comment = ");
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		strcat(cha_sql, chp_tmp);
	} else {
		strcat(cha_sql, "0");
	}
	strcat(cha_sql, ", b_trackback = ");
	chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		strcat(cha_sql, chp_tmp);
	} else {
		strcat(cha_sql, "0");
	}
	strcat(cha_sql, ", b_mode = ");
	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		strcat(cha_sql, chp_tmp);
	} else {
		strcat(cha_sql, "0");
	}
	strcat(cha_sql, ", c_entry_title = '");
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	chp_euc = Sjis_to_Euc(chp_tmp);
	chp_escape = My_Escape_SqlString(db, chp_euc);
	strcat(cha_sql, chp_escape);
	free(chp_euc);
	free(chp_escape);

	strcat(cha_sql, "', c_entry_summary = '");
	chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
	if (chp_tmp && *chp_tmp) {
		chp_euc = Sjis_to_Euc(chp_tmp);
		chp_escape = My_Escape_SqlString(db, chp_euc);
		strcat(cha_sql, chp_escape);
		free(chp_euc);
		free(chp_escape);
	}

	strcat(cha_sql, "', c_entry_body = '");
	sprintf(cha_num, "%d", in_entry_id);
	chp_tmp3 = Sjis_to_Euc(Get_Nlist(nlp_in, "entry_body", 1));
	chp_tmp2 = Remove_Invalid_Tags(chp_tmp3, nlp_out, 0);
	if (!chp_tmp2) {
		free(chp_tmp3);
		Rollback_Transact(db);
		return 1;
	}
	in_point = Calc_Body_Point(db, nlp_out, in_login_blog, in_entry_id, 0, chp_tmp2, Get_Nlist(nlp_in, cha_cookie, 1));
	if (in_point == INT_MAX) {
		free(chp_tmp3);
		Rollback_Transact(db);
		return 1;
	}
	chp_escape = My_Escape_SqlString(db, chp_tmp2);
	strcat(cha_sql, chp_escape);
	free(chp_tmp3);
	free(chp_escape);
	free(chp_tmp2);

	strcat(cha_sql, "', c_entry_more = '");
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

	strcat(cha_sql, "', d_entry_create_time = '");
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	if (!chp_tmp || !atoi(chp_tmp)) {
		Get_NowDateTime(cha_date, cha_time);
		cha_date[4] = '-';
		cha_date[7] = '-';
		strcat(cha_sql, cha_date);
		strcat(cha_sql, " ");
		strcat(cha_sql, cha_time);
	} else {
		strcat(cha_sql, Get_Nlist(nlp_in, "year", 1));
		strcat(cha_sql, "-");
		strcat(cha_sql, Get_Nlist(nlp_in, "month", 1));
		strcat(cha_sql, "-");
		strcat(cha_sql, Get_Nlist(nlp_in, "day", 1));
		strcat(cha_sql, " ");
		strcat(cha_sql, Get_Nlist(nlp_in, "hour", 1));
		strcat(cha_sql, ":");
		strcat(cha_sql, Get_Nlist(nlp_in, "minute", 1));
	}
	strcat(cha_sql, ":00'");

	sprintf(cha_sql + strlen(cha_sql), " where n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_login_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "更新ｸｴﾘーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	/* ﾄﾗｯｸﾊﾞｯｸPING・更新PING送信(公開時のみ) */
	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		/* ﾄﾗｯｸﾊﾞｯｸ */
		chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
		if (chp_tmp && strlen(chp_tmp) != 0) {
			chp_tmp2 = strtok(chp_tmp, "\n");
			while(chp_tmp2) {
				in_rt = trackback(db, nlp_in, nlp_out, in_entry_id, chp_tmp2, in_login_blog, 0, Get_Nlist(nlp_in, cha_cookie, 1));
				if (in_rt == 1){
					return 1;
				}
				chp_tmp2 = strtok(NULL, "\n");
			}
		}
		/*関連URL登録*/
		sprintf(cha_sql, "delete from at_sendlink where n_blog_id = %d and n_entry_id = %d and b_link_tb = 0", in_login_blog, in_entry_id);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "更新ｸｴﾘーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
	}
	if (Commit_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "ｺﾐｯﾄに失敗しました<br>");
		Rollback_Transact(db);
		return 1;
	}

	Put_Nlist(nlp_out, "ERROR", "記事を投稿しました。<br>");
	Mobile_Error(db, nlp_in, nlp_out, "ls", chpa_hidden);

	return 0;
}

int Get_Mobile_Admin_Modify_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	NLIST *nlp_tmp;
	char cha_sql[512];
	char cha_date[32];
	char cha_time[32];
	char cha_year[32];
	char cha_month[32];
	char cha_day[32];
	char cha_hour[32];
	char cha_minute[32];
	char cha_team[100];
	char *chp_euc;
	char *chp_esc;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	char *chp_title;
	char *chp_summary;
	char *chp_body;
	int i;
	int in_owner;
	int in_black;
	int in_entry;
	int in_count;
	int in_year;
	int in_month;
	int in_day;
	int in_deftb;
	int in_defcm;
	int in_defmd;

	in_owner = Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_sql);
	in_black = In_Black_List(db, nlp_out, in_owner, NULL);
	if (in_black) {
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "記事を特定できません。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp);
	in_entry = atoi(chp_tmp);

	Get_NowDateTime(cha_date, cha_time);
	cha_date[4] = '\0';
	cha_date[7] = '\0';
	strcpy(cha_year, cha_date);
	strcpy(cha_month, cha_date + 5);
	strcpy(cha_day, cha_date + 8);
	cha_time[2] = '\0';
	strcpy(cha_hour, cha_time);
	strcpy(cha_minute, cha_time + 3);

	sprintf(cha_sql,
		"select c_entry_title"
		",c_entry_summary"
		",c_entry_body"
		",b_comment"
		",b_trackback"
		",b_mode"
		",date_format(d_entry_create_time, '%%Y')"
		",date_format(d_entry_create_time, '%%m')"
		",date_format(d_entry_create_time, '%%d')"
		",date_format(d_entry_create_time, '%%H')"
		",date_format(d_entry_create_time, '%%i')"
		",c_entry_more"
		" from at_entry"
		" where n_blog_id = %d"
		" and n_entry_id = %d", in_login_blog, in_entry);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		chp_title = (char*)malloc(strlen(chp_tmp) + 1);
		strcpy(chp_title, chp_tmp);
	} else {
		chp_title = (char*)malloc(1);
		*chp_title = '\0';
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		chp_summary = (char*)malloc(strlen(chp_tmp) + 1);
		strcpy(chp_summary, chp_tmp);
	} else {
		chp_summary = (char*)malloc(1);
		*chp_summary = '\0';
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		chp_body = (char*)malloc(strlen(chp_tmp) + 1);
		strcpy(chp_body, chp_tmp);
	} else {
		chp_body = (char*)malloc(1);
		*chp_body = '\0';
	}
	chp_tmp = Db_GetValue(dbres, 0, 3);
	if (chp_tmp) {
		in_defcm = atoi(chp_tmp);
	} else {
		in_defcm = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 4);
	if (chp_tmp) {
		in_deftb = atoi(chp_tmp);
	} else {
		in_deftb = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 5);
	if (chp_tmp) {
		in_defmd = atoi(chp_tmp);
	} else {
		in_defmd = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 6);
	if (chp_tmp) {
		strcpy(cha_year, chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "year", 1);
	if (chp_tmp) {
		strcpy(cha_year, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 7);
	if (chp_tmp) {
		strcpy(cha_month, chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "month", 1);
	if (chp_tmp) {
		strcpy(cha_month, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 8);
	if (chp_tmp) {
		strcpy(cha_day, chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "day", 1);
	if (chp_tmp) {
		strcpy(cha_day, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 9);
	if (chp_tmp) {
		strcpy(cha_hour, chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "hour", 1);
	if (chp_tmp) {
		strcpy(cha_hour, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 10);
	if (chp_tmp) {
		strcpy(cha_minute, chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "minute", 1);
	if (chp_tmp) {
		strcpy(cha_minute, chp_tmp);
	}
	in_year = 0;
	in_month = 0;
	in_day = 0;
	cha_team[0] = '\0';
	Find_Team(Db_GetValue(dbres, 0, 11), &in_year, &in_month, &in_day, cha_team);
	Db_CloseDyna(dbres);

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

	if (in_defmd) {
		Put_Nlist(nlp_out, "DATE_START", "<!--");
		Put_Nlist(nlp_out, "DATE_END", "-->");
		Put_Nlist(nlp_out, "MODE_START", "<!--");
		Put_Nlist(nlp_out, "MODE_END", "-->");
		Build_Hidden(nlp_out, "HIDDEN", "year", cha_year);
		Build_Hidden(nlp_out, "HIDDEN", "month", cha_month);
		Build_Hidden(nlp_out, "HIDDEN", "day", cha_day);
		Build_Hidden(nlp_out, "HIDDEN", "hour", cha_hour);
		Build_Hidden(nlp_out, "HIDDEN", "minute", cha_minute);
		Build_Hidden(nlp_out, "HIDDEN", "entry_mode", "1");
		Build_Hidden(nlp_out, "HIDDEN", "specify_date", "1");
	} else {
		in_count = atoi(cha_year);
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
		for (i = 0; i < Db_GetRowCount(dbres); ++i) {
			chp_tmp = Db_GetValue(dbres, i, 0);
			if (chp_tmp) {
				nlp_tmp = Init_Nlist();
				Put_Nlist(nlp_tmp, "YEAR", chp_tmp);
				if (strcmp(cha_year, chp_tmp) == 0) {
					Put_Nlist(nlp_tmp, "SELECTED", "selected");
				}
				in_count = atoi(chp_tmp);
				Conv_Depend_String(nlp_tmp);
				chp_tmp = Page_Out_Mem(nlp_tmp, "parts_year_select_option.skl");
				if (chp_tmp) {
					Put_Nlist(nlp_out, "YEARS", chp_tmp);
					free(chp_tmp);
				}
				Finish_Nlist(nlp_tmp);
			}
		}
		Db_CloseDyna(dbres);
		nlp_tmp = Init_Nlist();
		++in_count;
		sprintf(cha_sql, "%d", in_count);
		Put_Nlist(nlp_tmp, "YEAR", cha_sql);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_year_select_option.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "YEARS", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);

		sprintf(cha_sql, "MON%s", cha_month);
		Put_Nlist(nlp_out, cha_sql, " selected");

		sprintf(cha_sql, "DAY%s", cha_day);
		Put_Nlist(nlp_out, cha_sql, " selected");

		sprintf(cha_sql, "HR%s", cha_hour);
		Put_Nlist(nlp_out, cha_sql, " selected");

		sprintf(cha_sql, "MIN%s", cha_minute);
		Put_Nlist(nlp_out, cha_sql, " selected");
	}

	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	if (chp_tmp) {
		chp_euc = Sjis_to_Euc(chp_tmp);
		chp_esc = Escape_HtmlString(chp_euc);
		Put_Nlist(nlp_out, "ENTRYTITLE", chp_euc);
		free(chp_esc);
		free(chp_euc);
	} else {
		chp_esc = Escape_HtmlString(chp_title);
		Put_Nlist(nlp_out, "ENTRYTITLE", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_body", 1);
	if (chp_tmp) {
		chp_euc = Sjis_to_Euc(chp_tmp);
		chp_esc = Escape_HtmlString(chp_euc);
		Put_Nlist(nlp_out, "ENTRYBODY", chp_euc);
		free(chp_esc);
		free(chp_euc);
	} else {
		chp_esc = Escape_HtmlString(chp_body);
		Put_Nlist(nlp_out, "ENTRYBODY", chp_esc);
		free(chp_esc);
	}

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
		Put_Nlist(nlp_out, "ENTRYSUMMARY", chp_euc);
		free(chp_esc);
		free(chp_euc);
	} else {
		chp_esc = Escape_HtmlString(chp_summary);
		Put_Nlist(nlp_out, "ENTRYSUMMARY", chp_esc);
		free(chp_esc);
	}

	free(chp_title);
	free(chp_summary);
	free(chp_body);

	chp_tmp = Get_Nlist(nlp_in, "INSERT_ORDER", 1);
	if (chp_tmp || cha_team[0]) {
		Put_Nlist(nlp_out, "INSORD", " checked");
	} else if (in_year && in_month && in_day) {
		Put_Nlist(nlp_out, "INSORD", " checked");
	}
	cha_sql[0] = '\0';
	chp_tmp = Get_Nlist(nlp_in, "ORDYEAR", 1);
	chp_tmp2 = Get_Nlist(nlp_in, "ORDMONTH", 1);
	chp_tmp3 = Get_Nlist(nlp_in, "ORDDAY", 1);
	if (chp_tmp && chp_tmp2 && chp_tmp3) {
		sprintf(cha_sql, "%s/%s/%s", chp_tmp, chp_tmp2, chp_tmp3);
	} else if (in_year && in_month && in_day) {
		sprintf(cha_sql, "%04d/%02d/%02d", in_year, in_month, in_day);
	}
	if (Mobile_Team_Combo(db, nlp_in, nlp_out, in_login_blog, "team", cha_team, cha_sql)) {
		return 1;
	}

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_login_blog, NULL);

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_modify_entry.skl");

	return 0;
}

int Get_Mobile_Admin_Delete_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	int in_entry;
	char cha_sql[512];
	char *chp_esc;
	char *chp_tmp;
	char *chpa_hidden[] = {
		"trackback_url", "dbb_topic_url", "entry_title", "entry_body", "start",
		"trackback", "comment", "entry_mode", "entry_summary", "eid", NULL
	};

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "記事を特定できません。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp);
	in_entry = atoi(chp_tmp);

	sprintf(cha_sql,
		"select c_entry_title"
		" from at_entry"
		" where n_blog_id = %d"
		" and n_entry_id = %d", in_login_blog, in_entry);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		Put_Format_Nlist(nlp_out, "ERROR", "記事ﾀｲﾄﾙを取得できませんでした。(%d)<br>", __LINE__);
		return 1;
	}
	chp_esc = Escape_HtmlString(chp_tmp);
	Put_Format_Nlist(nlp_out, "ERROR", "記事「%s」を削除してもよろしいですか？<br>", chp_esc);
	free(chp_esc);
	Mobile_Confirm(db, nlp_in, nlp_out, "me", chpa_hidden);
	Db_CloseDyna(dbres);

	return 0;
}

int Post_Mobile_Admin_Delete_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	int in_entry;
	char cha_sql[512];
	char cha_str[512];
	char cha_temp[128];
	char cha_cookie[32];
	char *chp_start;
	char *chp_owner;
	char *chp_key;
	char *chp_tmp;

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

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "記事を特定できません。<br>");
		return 1;
	}
	in_entry = atoi(chp_tmp);

	chp_start = Get_Nlist(nlp_in, "start", 1);
	chp_owner = Get_Nlist(nlp_in, "owner", 1);
	chp_key = Get_Nlist(nlp_in, cha_cookie, 1);
	chp_tmp = Get_Nlist(nlp_in, "yes", 1);
	if (chp_tmp) {
		if (Begin_Transact(db)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ﾄﾗﾝｻﾞｸｼｮﾝの開始に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		/* ｴﾝﾄﾘ本体 */
		sprintf(cha_sql, "delete from at_entry where n_entry_id = %d and n_blog_id = %d", in_entry, in_login_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		/* ｺﾒﾝﾄ */
		sprintf(cha_sql, "delete from at_comment where n_entry_id = %d  and n_blog_id = %d", in_entry, in_login_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		/* ﾄﾗｯｸﾊﾞｯｸ受信 */
		sprintf(cha_sql, "delete from at_trackback where n_entry_id = %d and n_blog_id = %d", in_entry, in_login_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		/* ﾄﾗｯｸﾊﾞｯｸ送信 */
		sprintf(cha_sql, "delete from at_sendtb where n_entry_id = %d and n_blog_id = %d", in_entry, in_login_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		/* 添付ﾌｧｲﾙﾃーﾌﾞﾙ */
		sprintf(cha_sql, "delete from at_uploadfile where n_entry_id = %d and n_blog_id = %d", in_entry, in_login_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		/* 送信ﾘﾝｸﾃーﾌﾞﾙ */
		sprintf(cha_sql, "delete from at_sendlink where n_entry_id = %d and n_blog_id = %d", in_entry, in_login_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		/* 受信ﾘﾝｸﾃーﾌﾞﾙ */
		sprintf(cha_sql, "delete from at_recvlink where n_entry_id = %d and n_blog_id = %d", in_entry, in_login_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		Blog_To_Temp(db, nlp_out, in_login_blog, cha_temp);
		sprintf(cha_str, "%s%s%s/%s?eid=%d&bid=%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_entry, cha_temp);
		sprintf(cha_sql, "delete from at_sendlink where c_link_url = '%s'", cha_str);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		sprintf(cha_str, "%s%s%s/%s/?eid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, cha_temp, in_entry);
		sprintf(cha_sql, "delete from at_trackback where c_tb_url = '%s'", cha_str);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｴﾝﾄﾘの削除に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Format_Nlist(nlp_out, "ERROR", "ｺﾐｯﾄに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		printf("Location: %s%s%s/%s?pg=ls&start=%s&owner=%s&%s=%s\n\n",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, g_cha_mobile_admin_cgi,
			chp_start ? chp_start : "0", chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");
	} else {
		printf("Location: %s%s%s/%s?pg=me&eid=%d&start=%s&owner=%s&%s=%s\n\n",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, g_cha_mobile_admin_cgi,
			in_entry, chp_start ? chp_start : "0", chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");
	}

	return 0;
}

int Mobile_Admin_Cancel_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char cha_cookie[32];
	char *chp_start;
	char *chp_owner;
	char *chp_key;

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

	chp_start = Get_Nlist(nlp_in, "start", 1);
	chp_owner = Get_Nlist(nlp_in, "owner", 1);
	chp_key = Get_Nlist(nlp_in, cha_cookie, 1);
	printf("Location: %s%s%s/%s?pg=ls&start=%s&owner=%s&%s=%s\n\n",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, g_cha_mobile_admin_cgi,
		chp_start ? chp_start : "0", chp_owner ? chp_owner : "0", cha_cookie, chp_key ? chp_key : "0");

	return 0;
}

int Mobile_Admin_Modify_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	int in_error;
	char *chpa_hidden[] = {
		"eid", "start",
		"trackback_url", "dbb_topic_url", "entry_title", "entry_body",
		"INSERT_ORDER", "ORDYEAR", "ORDMONTH", "ORDDAY",
		"specify_date", "year", "month", "day", "hour", "minute", "team",
		"trackback", "comment", "entry_mode", "entry_summary", NULL
	};
	char *chpa_hidden1[] = {"eid", "start", NULL};

	if (Is_Post_Method() && Get_Nlist(nlp_in, "BTN_EDIT_ENTRY", 1)) {
		in_error = Post_Mobile_Admin_Modify_Entry(db, nlp_in, nlp_out, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "me", chpa_hidden);
		}
	} else if (Is_Post_Method() && Get_Nlist(nlp_in, "BTN_DELETE_ASK", 1)) {
		in_error = Get_Mobile_Admin_Delete_Entry(db, nlp_in, nlp_out, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "me", chpa_hidden);
		}
	} else if (Is_Post_Method() && Get_Nlist(nlp_in, "BTN_DELETE_ENTRY", 1)) {
		in_error = Post_Mobile_Admin_Delete_Entry(db, nlp_in, nlp_out, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "me", chpa_hidden);
		}
	} else if (Is_Post_Method() && Get_Nlist(nlp_in, "cancel", 1)) {
		in_error = Mobile_Admin_Cancel_Entry(db, nlp_in, nlp_out, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "ls", chpa_hidden1);
		}
	} else {
		in_error = Get_Mobile_Admin_Modify_Entry(db, nlp_in, nlp_out, in_login_blog);
		if (in_error) {
			Mobile_Error(db, nlp_in, nlp_out, "ls", chpa_hidden1);
		}
	}
	return in_error;
}
