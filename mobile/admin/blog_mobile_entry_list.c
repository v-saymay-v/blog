#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libnkf.h"
#include "libmysql.h"
#include "libauth.h"
#include "libmobile.h"
#include "libblog.h"
#include "blog_mobile_entry_list.h"

extern char g_cha_mobile_user_cgi[22];

int Mobile_Admin_Entry_List(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	DBRes *dbres;
	DBRes *dbres2;
	NLIST *nlp_tmp;
	int i;
	int in_friends;
	int in_tbk;
	int in_cmt;
	int in_all;
	int in_lines;
	int in_start;
	char cha_sql[512];
	char cha_year[32];
	char *chp_tmp;
	char *chp_esc;
	char *chp_base;
	char *chp_year;
	char *chp_string;

	in_friends = 0;
	sprintf(cha_sql, "select c_blog_title,b_setting_friends_only from at_blog where n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "TITLE", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_friends = atoi(chp_tmp);
	}
	Db_CloseDyna(dbres);

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_login_blog, NULL);

	Get_NowDate(cha_year);
	cha_year[4] = '\0';
	chp_year = Get_Nlist(nlp_in, "y", 1);
	if (chp_year) {
		strcpy(cha_year, chp_year);
	}
	sprintf(cha_sql,
		" select distinct"
		" date_format(d_entry_create_time, '%%Y') as n_year"
		" from at_entry"
		" where n_blog_id = %d"
		" and d_entry_create_time < now()"
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

	sprintf(cha_sql, "select n_setting_disp_per_page from at_blog where n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	in_lines = chp_tmp ? atoi(chp_tmp) : 10;
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "start", 1);
	in_start = chp_tmp ? atoi(chp_tmp) : 0;

	asprintf(&chp_base,
		" select distinct sql_calc_found_rows"
		" T1.n_entry_id"
		",date_format(T1.d_entry_create_time, '%%Y-%%m-%%d')"
		",T1.c_entry_title"
		",if (T1.d_entry_create_time < now(), 1, 0)"
		" from"
		" at_entry T1"
		" where T1.n_blog_id = %d "
		" and T1.d_entry_real_time < now()"
		" order by T1.d_entry_create_time desc"
		" limit %d, %d",
		in_login_blog,
		in_start, in_lines);

	dbres = Db_OpenDyna(db, chp_base);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, chp_base);
		free(chp_base);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	free(chp_base);
	dbres2 = Db_OpenDyna(db, "select found_rows()");
	if(dbres2 == NULL) {
		sprintf(cha_sql, "総件数取得に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	in_all = atoi(Db_GetValue(dbres2, 0, 0));
	Db_CloseDyna(dbres2);
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		nlp_tmp = Init_Nlist();
		in_tbk = 0;
		in_cmt = 0;
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp) {
			in_tbk = Get_Total_Trackback_By_Entry_Id(db, nlp_in, nlp_out, atoi(chp_tmp), in_login_blog, 0);
			in_cmt = Get_Total_Comment_By_Entry_Id(db, nlp_in, nlp_out, atoi(chp_tmp), in_login_blog, 0);
		}
		Put_Nlist(nlp_tmp, "EID", chp_tmp);
		sprintf(cha_year, "%d", in_tbk);
		Put_Nlist(nlp_tmp, "TB", cha_year);
		sprintf(cha_year, "%d", in_cmt);
		Put_Nlist(nlp_tmp, "CMT", cha_year);
		sprintf(cha_year, "%d", in_start);
		Put_Nlist(nlp_tmp, "START", cha_year);
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (chp_tmp) {
			Put_Nlist(nlp_tmp, "DATETIME", chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, i, 2);
		if (chp_tmp) {
			chp_esc = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_tmp, "TITLE", chp_esc);
			free(chp_esc);
		}
		Put_Nlist(nlp_tmp, "ACCESS", in_friends ? "球" : "全");
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_login_blog, NULL);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Db_GetValue(dbres, i, 3);
		if (chp_tmp && atoi(chp_tmp) > 0) {
			chp_tmp = Page_Out_Mem(nlp_tmp, "parts_entry_list.skl");
		} else {
			chp_tmp = Page_Out_Mem(nlp_tmp, "parts_entry_list_no_link.skl");
		}
		if (chp_tmp) {
			Put_Nlist(nlp_out, "LIST", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	}
	Db_CloseDyna(dbres);

	if (in_start > 0) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_login_blog, NULL);
		sprintf(cha_sql, "%d", in_start - in_lines);
		Put_Nlist(nlp_tmp, "START", cha_sql);
		sprintf(cha_sql, "%d", in_lines);
		Put_Nlist(nlp_tmp, "PAGE", cha_sql);
		chp_string = Get_Nlist(nlp_in, "word", 1);
		if (chp_string && chp_string[0]) {
			Encode_Url(chp_string, &chp_tmp);
			Put_Format_Nlist(nlp_tmp, "SEARCH", "&word=%s", chp_tmp);
			free(chp_tmp);
		}
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_prev_page.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "PREV", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Format_Nlist(nlp_out, "PREV", "前の%d件", in_lines);
	}

	if (in_start + in_lines < in_all) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_login_blog, NULL);
		sprintf(cha_sql, "%d", in_start + in_lines);
		Put_Nlist(nlp_tmp, "START", cha_sql);
		sprintf(cha_sql, "%d", in_lines);
		Put_Nlist(nlp_tmp, "PAGE", cha_sql);
		chp_string = Get_Nlist(nlp_in, "word", 1);
		if (chp_string && chp_string[0]) {
			Encode_Url(chp_string, &chp_tmp);
			Put_Format_Nlist(nlp_tmp, "SEARCH", "&word=%s", chp_tmp);
			free(chp_tmp);
		}
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_next_page.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "NEXT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Format_Nlist(nlp_out, "NEXT", "次の%d件", in_lines);
	}

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_entry_list.skl");

	return 0;
}
