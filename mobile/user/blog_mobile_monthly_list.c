#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libnkf.h"
#include "libmysql.h"
#include "libauth.h"
#include "libmobile.h"
#include "libblog.h"
#include "blog_mobile_monthly_list.h"

int Mobile_User_Monthly_List(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	NLIST *nlp_tmp;
	int i;
	int in_fnd;
	int in_cury;
	int in_curm;
	int in_year;
	int in_month;
	int in_latest;
	int in_oldest;
	char cha_nickname[100];
	char cha_sql[512];
	char cha_year[32];
	char cha_min[32];
	char cha_max[32];
	char cha_top[32];
	char cha_end[32];
	char *chp_tmp;
	char *chp_esc;
	char *chp_year;
	char *chp_month;

	sprintf(cha_sql, "select c_blog_title,c_blog_subtitle,c_blog_description from at_blog where n_blog_id = %d", in_blog);
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
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "SUBTITLE", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "DESCRIPTION", chp_esc);
		free(chp_esc);
	}
	Db_CloseDyna(dbres);

	if (!Get_Nickname_From_Blog(db, nlp_out, in_blog, cha_nickname)) {
		return 1;
	}
	chp_esc = Escape_HtmlString(cha_nickname);
	Put_Nlist(nlp_out, "NICKNAME", chp_esc);
	free(chp_esc);

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_blog, NULL);

	Get_NowDate(cha_year);
	cha_year[4] = '\0';
	cha_year[7] = '\0';
	cha_min[0] = '\0';
	cha_max[0] = '\0';
	chp_year = Get_Nlist(nlp_in, "year", 1);
	chp_month = Get_Nlist(nlp_in, "month", 1);
	if (!chp_year || !chp_month) {
		chp_year = cha_year;
		chp_month = cha_year + 5;
	}
	sprintf(cha_min, "%s-%s-01 00:00:00", chp_year, chp_month);
	sprintf(cha_max, "%s-%s-%02d 23:59:59", chp_year, chp_month, Get_MonthDay(atoi(chp_year), atoi(chp_month)));
	Put_Nlist(nlp_out, "YEAR", chp_year);
	Put_Nlist(nlp_out, "MONTH", chp_month);

	in_year = atoi(chp_year);
	in_month = atoi(chp_month);

	in_latest = Get_Latest_Entry_Year(db, in_blog);
	if (in_latest == CO_ERROR) {
		return 1;
	}
	in_oldest = Get_Oldest_Entry_Year(db, in_blog);
	if (in_oldest == CO_ERROR) {
		return 1;
	}

	in_fnd = 0;
	in_cury = in_year;
	in_curm = in_month;
	--in_curm;
	if (!in_curm) {
		--in_cury;
		in_curm = 12;
	}
	for (; in_cury >= in_oldest && !in_fnd; --in_cury) {
		for (; in_curm > 0; --in_curm) {
			sprintf(cha_top, "%04d%02d01", in_cury, in_curm);
			sprintf(cha_end, "%04d%02d%02d", in_cury, in_curm, Get_MonthDay(in_cury, in_curm));
			if (get_count_article_by_day(db, cha_top, cha_end, in_blog) > 0) {
				nlp_tmp = Init_Nlist();
				Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
				sprintf(cha_sql, "%d", in_cury);
				Put_Nlist(nlp_tmp, "YEAR", cha_sql);
				sprintf(cha_sql, "%d", in_curm);
				Put_Nlist(nlp_tmp, "MONTH", cha_sql);
				Conv_Depend_String(nlp_tmp);
				chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_prev_month.skl");
				if (chp_tmp) {
					Put_Nlist(nlp_out, "PREV", chp_tmp);
					free(chp_tmp);
				}
				Finish_Nlist(nlp_tmp);
				in_fnd = 1;
				break;
			}
		}
		in_curm = 12;
	}
	if (!in_fnd) {
		Put_Nlist(nlp_out, "PREV", "前月");
	}

	in_fnd = 0;
	in_cury = in_year;
	in_curm = in_month;
	++in_curm;
	if (in_curm > 12) {
		++in_cury;
		in_curm = 1;
	}
	for (; in_cury <= in_latest && !in_fnd; ++in_cury) {
		for (; in_curm <=12; ++in_curm) {
			sprintf(cha_top, "%04d%02d01", in_cury, in_curm);
			sprintf(cha_end, "%04d%02d%02d", in_cury, in_curm, Get_MonthDay(in_cury, in_curm));
			if (get_count_article_by_day(db, cha_top, cha_end, in_blog) > 0) {
				nlp_tmp = Init_Nlist();
				Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
				sprintf(cha_sql, "%d", in_cury);
				Put_Nlist(nlp_tmp, "YEAR", cha_sql);
				sprintf(cha_sql, "%d", in_curm);
				Put_Nlist(nlp_tmp, "MONTH", cha_sql);
				Conv_Depend_String(nlp_tmp);
				chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_next_month.skl");
				if (chp_tmp) {
					Put_Nlist(nlp_out, "NEXT", chp_tmp);
					free(chp_tmp);
				}
				Finish_Nlist(nlp_tmp);
				in_fnd = 1;
				break;
			}
		}
		in_curm = 1;
	}
	if (!in_fnd) {
		Put_Nlist(nlp_out, "NEXT", "翌月");
	}

	sprintf(cha_sql,
		" select"
		" T1.n_entry_id"
//		",date_format(T1.d_entry_create_time, '%%Y-%%m-%%d %%H:%%i')"
		",date_format(T1.d_entry_create_time, '%%Y-%%m-%%d')"
		",T1.c_entry_title"
		" from"
		" at_entry T1"
		" where T1.b_mode != 0"
		" and T1.n_blog_id = %d "
		" and T1.d_entry_create_time < now()"
		" and T1.d_entry_create_time >= '%s'"
		" and T1.d_entry_create_time <= '%s'"
		" and T1.b_mode != 0"
		" order by T1.d_entry_create_time desc",
		 in_blog, cha_min, cha_max);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp) {
			Put_Nlist(nlp_tmp, "EID", chp_tmp);
		}
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
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_article_list.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "LIST", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	}
	Db_CloseDyna(dbres);

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_monthly_list.skl");

	return 0;
}
