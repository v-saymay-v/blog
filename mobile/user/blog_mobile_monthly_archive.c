#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libmysql.h"
#include "libauth.h"
#include "libmobile.h"
#include "libblog.h"
#include "blog_mobile_monthly_archive.h"

int Mobile_User_Monthly_Archive(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	NLIST *nlp_tmp;
	int in_entry;
	int in_year;
	int in_latest;
	int in_oldest;
	int in_cur;
	char cha_top[10];
	char cha_end[10];
	char cha_sql[512];
	char cha_nickname[100];
	char *chp_tmp;
	char *chp_esc;
	char *chp_bdy;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	in_entry = chp_tmp ? atoi(chp_tmp) : 0;

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
		chp_bdy = Conv_Long_Ascii(chp_esc, 56);
		chp_tmp = Conv_Blog_Br(chp_bdy);
		Put_Nlist(nlp_out, "DESCRIPTION", chp_tmp);
		free(chp_bdy);
		free(chp_esc);
	}
	Db_CloseDyna(dbres);

	if (!Get_Nickname_From_Blog(db, nlp_out, in_blog, cha_nickname)) {
		return 1;
	}
	chp_esc = Escape_HtmlString(cha_nickname);
	Put_Nlist(nlp_out, "NICKNAME", chp_esc);
	free(chp_esc);

	chp_tmp = Get_Nlist(nlp_in, "year", 1);
	if (chp_tmp) {
		in_year = atoi(chp_tmp);
	} else {
		Get_NowDate(cha_sql);
		in_year = atoi(cha_sql);
	}
	sprintf(cha_sql, "%d", in_year);
	Put_Nlist(nlp_out, "YEAR", cha_sql);

	in_latest = Get_Latest_Entry_Year(db, in_blog);
	if (in_latest == CO_ERROR) {
		Put_Nlist(nlp_out, "ERROR", "記事が投稿されていません。");
		return 1;
	}
	in_oldest = Get_Oldest_Entry_Year(db, in_blog);
	if (in_oldest == CO_ERROR) {
		Put_Nlist(nlp_out, "ERROR", "記事が投稿されていません。");
		return 1;
	}

	if (in_year > in_oldest) {
		for (in_cur = in_year - 1; in_cur >= in_oldest; --in_cur) {
			sprintf(cha_top, "%04d0101", in_cur);
			sprintf(cha_end, "%04d1231", in_cur);
			if (get_count_article_by_day(db, cha_top, cha_end, in_blog) > 0) {
				nlp_tmp = Init_Nlist();
				Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
				sprintf(cha_sql, "%d", in_cur);
				Put_Nlist(nlp_tmp, "YEAR", cha_sql);
				Conv_Depend_String(nlp_tmp);
				chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_prev_year.skl");
				if (chp_tmp) {
					Put_Nlist(nlp_out, "PREV", chp_tmp);
					free(chp_tmp);
				}
				Finish_Nlist(nlp_tmp);
				break;
			}
		}
	} else {
		Put_Nlist(nlp_out, "PREV", "前年");
	}

	if (in_year < in_latest) {
		for (in_cur = in_year + 1; in_cur <= in_latest; ++in_cur) {
			sprintf(cha_top, "%04d0101", in_cur);
			sprintf(cha_end, "%04d1231", in_cur);
			if (get_count_article_by_day(db, cha_top, cha_end, in_blog) > 0) {
				nlp_tmp = Init_Nlist();
				Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
				sprintf(cha_sql, "%d", in_cur);
				Put_Nlist(nlp_tmp, "YEAR", cha_sql);
				Conv_Depend_String(nlp_tmp);
				chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_next_year.skl");
				if (chp_tmp) {
					Put_Nlist(nlp_out, "NEXT", chp_tmp);
					free(chp_tmp);
				}
				Finish_Nlist(nlp_tmp);
				break;
			}
		}
	} else {
		Put_Nlist(nlp_out, "NEXT", "次年");
	}

	for (in_cur = 1; in_cur <= 12; ++in_cur) {
		sprintf(cha_top, "%04d%02d01", in_year, in_cur);
		sprintf(cha_end, "%04d%02d%02d", in_year, in_cur, Get_MonthDay(in_year, in_cur));
		if (get_count_article_by_day(db, cha_top, cha_end, in_blog) > 0) {
			nlp_tmp = Init_Nlist();
			Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
			sprintf(cha_sql, "%d", in_year);
			Put_Nlist(nlp_tmp, "YEAR", cha_sql);
			sprintf(cha_sql, "%d", in_cur);
			Put_Nlist(nlp_tmp, "MONTH", cha_sql);
			Conv_Depend_String(nlp_tmp);
			chp_tmp = Page_Out_Mem(nlp_tmp, "parts_monthly_archive.skl");
			if (chp_tmp) {
				Put_Nlist(nlp_out, "LIST", chp_tmp);
				free(chp_tmp);
			}
			Finish_Nlist(nlp_tmp);
		} else {
			Put_Format_Nlist(nlp_out, "LIST", "%d月<br />\n", in_cur);
		}
	}

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_blog, NULL);

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_monthly_archive.skl");

	return 0;
}
