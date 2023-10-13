#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libnkf.h"
#include "libmysql.h"
#include "libauth.h"
#include "libmobile.h"
#include "libblog.h"
#include "blog_mobile_show_list.h"

extern char g_cha_mobile_user_cgi[22];

int Mobile_User_Show_List(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	DBRes *dbres2;
	NLIST *nlp_tmp;
	CP_NKF nkf;
	int i;
	int in_tbk;
	int in_cmt;
	int in_all;
	int in_lines;
	int in_start;
	char cha_sql[512];
	char cha_year[32];
	char cha_min[32];
	char cha_max[32];
	char cha_nickname[100];
	char *chp_tmp;
	char *chp_esc;
	char *chp_add;
	char *chp_base;
	char *chp_year;
	char *chp_month;
	char *chp_string;
	char *chp_search;
	char *chp_subject;
	char *chp_content;
	char *chp_comment;
	char *chp_trackback;

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
	chp_year = Get_Nlist(nlp_in, "year", 1);
	if (chp_year) {
		strcpy(cha_year, chp_year);
	}
	sprintf(cha_sql,
		" select distinct"
		" date_format(d_entry_create_time, '%%Y') as n_year"
		" from at_entry"
		" where n_blog_id = %d"
		" and d_entry_create_time < now()"
		" and b_mode != 0"
		" order by n_year;",
		in_blog);
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

	sprintf(cha_sql, "select n_setting_disp_per_page from at_blog where n_blog_id = %d", in_blog);
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

	cha_min[0] = '\0';
	cha_max[0] = '\0';
	chp_year = Get_Nlist(nlp_in, "y", 1);
	chp_month = Get_Nlist(nlp_in, "month", 1);
	if (chp_year && chp_month) {
		in_tbk = Get_MonthDay(atoi(chp_year), atoi(chp_month));
		sprintf(cha_min, "%s-%s-01 00:00:00", chp_year, chp_month);
		sprintf(cha_max, "%s-%s-%02d 23:59:59", chp_year, chp_month, in_tbk);
		sprintf(cha_sql, "MON%s", chp_month);
		Put_Nlist(nlp_out, cha_sql, " selected");
	}

	chp_subject = Get_Nlist(nlp_in, "search_subject", 1);
	chp_content = Get_Nlist(nlp_in, "search_content", 1);
	chp_comment = Get_Nlist(nlp_in, "search_comment", 1);
	chp_trackback = Get_Nlist(nlp_in, "search_trackback", 1);

	asprintf(&chp_base,
		" select sql_calc_found_rows DISTINCT"
		" T1.n_entry_id"
//		",date_format(T1.d_entry_create_time, '%%Y-%%m-%%d %%H:%%i')"
		",date_format(T1.d_entry_create_time, '%%Y-%%m-%%d')"
		",T1.c_entry_title"
		" from"
		" at_entry T1"
		" left join at_comment T2 on T2.n_entry_id = T1.n_entry_id and T2.n_blog_id = %d"
		" left join at_trackback T3 on T3.n_entry_id = T1.n_entry_id and T3.n_blog_id = %d"
		" where T1.b_mode != 0"
		" and T1.n_blog_id = %d ",
		 in_blog, in_blog, in_blog);

	chp_string = Get_Nlist(nlp_in, "word", 1);
	if (chp_string && chp_string[0]) {
		nkf = Create_Nkf();
		Set_Input_String(nkf, chp_string);
		Set_Input_Code(nkf, NKF_SJIS);
		Set_Output_Code(nkf, NKF_EUC);
		Convert_To_String(nkf, &chp_search);
		Destroy_Nkf_Engine(nkf);
		Put_Nlist(nlp_out, "WORD", chp_string);
		chp_tmp = strtok(chp_search, " ");
		while (chp_tmp) {
			if (chp_tmp[0]) {
				int in_first = 1;
				chp_esc = (char*)malloc(strlen(chp_tmp) * 5);
				My_Db_EscapeString(db, chp_esc, chp_tmp, strlen(chp_tmp));
				if (chp_subject) {
					asprintf(&chp_add, "and (T1.c_entry_title like '%%%s%%' or T1.c_entry_title like '%s%%' or T1.c_entry_title like '%%%s' or T1.c_entry_title = '%s'"
						, chp_esc, chp_esc, chp_esc, chp_esc);
					Add_Str(&chp_base, chp_add);
					free(chp_add);
					in_first = 0;
				}
				if (chp_content) {
					if (in_first) {
						asprintf(&chp_add,
							" and (T1.c_entry_body like '%%%s%%' or T1.c_entry_body like '%%%s' or T1.c_entry_body like '%s%%' or T1.c_entry_body = '%s'"
							" or T1.c_entry_more like '%%%s%%' or T1.c_entry_more like '%%%s' or T1.c_entry_more like '%s%%' or T1.c_entry_more = '%s'"
							 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
					} else {
						asprintf(&chp_add,
							" or T1.c_entry_body like '%%%s%%' or T1.c_entry_body like '%%%s' or T1.c_entry_body like '%s%%' or T1.c_entry_body = '%s'"
							" or T1.c_entry_more like '%%%s%%' or T1.c_entry_more like '%%%s' or T1.c_entry_more like '%s%%' or T1.c_entry_more = '%s'"
							 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
					}
					Add_Str(&chp_base, chp_add);
					free(chp_add);
					in_first = 0;
				}
				if (chp_comment) {
					if (in_first) {
						asprintf(&chp_add,
							" and (T2.c_comment_body like '%%%s%%' or T2.c_comment_body like '%%%s' or T2.c_comment_body like '%s%%' or T2.c_comment_body = '%s'"
							 , chp_esc, chp_esc, chp_esc, chp_esc);
					} else {
						asprintf(&chp_add,
							" or T2.c_comment_body like '%%%s%%' or T2.c_comment_body like '%%%s' or T2.c_comment_body like '%s%%' or T2.c_comment_body = '%s'"
							 , chp_esc, chp_esc, chp_esc, chp_esc);
					}
					Add_Str(&chp_base, chp_add);
					free(chp_add);
					in_first = 0;
				}
				if (chp_trackback) {
					if (in_first) {
						asprintf(&chp_add,
							" and (T3.c_tb_title like '%%%s%%' or T3.c_tb_title like '%%%s' or T3.c_tb_title like '%s%%' or T3.c_tb_title = '%s'"
							" or T3.c_tb_excerpt like '%%%s%%' or T3.c_tb_excerpt like '%%%s' or T3.c_tb_excerpt like '%s%%' or T3.c_tb_excerpt = '%s'"
							 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
					} else {
						asprintf(&chp_add,
							" or T3.c_tb_title like '%%%s%%' or T3.c_tb_title like '%%%s' or T3.c_tb_title like '%s%%' or T3.c_tb_title = '%s'"
							" or T3.c_tb_excerpt like '%%%s%%' or T3.c_tb_excerpt like '%%%s' or T3.c_tb_excerpt like '%s%%' or T3.c_tb_excerpt = '%s'"
							 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
					}
					Add_Str(&chp_base, chp_add);
					free(chp_add);
					in_first = 0;
				}
				free(chp_esc);
				if (!in_first) {
					Add_Str(&chp_base, ")");
				}
			}
			chp_tmp = strtok(NULL, " ");
		}
		free(chp_search);
	}

	if (cha_min[0] && cha_max[0]) {
		sprintf(cha_sql,
			" and T1.d_entry_create_time < now()"
			" and T1.d_entry_create_time >= '%s'"
			" and T1.d_entry_create_time < '%s'"
			" and T1.b_mode != 0"
			" order by T1.d_entry_create_time desc"
			" limit %d, %d",
			cha_min, cha_max, in_start, in_lines);
	} else {
		sprintf(cha_sql,
			" and T1.d_entry_create_time < now()"
			" and T1.b_mode != 0"
			" order by T1.d_entry_create_time desc"
			" limit %d, %d",
			in_start, in_lines);
	}
	Add_Str(&chp_base, cha_sql);

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
			in_tbk = Get_Total_Trackback_By_Entry_Id(db, nlp_in, nlp_out, atoi(chp_tmp), in_blog, 0);
			in_cmt = Get_Total_Comment_By_Entry_Id(db, nlp_in, nlp_out, atoi(chp_tmp), in_blog, 0);
		}
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		Put_Nlist(nlp_tmp, "EID", chp_tmp);
		sprintf(cha_year, "%d", in_tbk);
		Put_Nlist(nlp_tmp, "TB", cha_year);
		sprintf(cha_year, "%d", in_cmt);
		Put_Nlist(nlp_tmp, "CMT", cha_year);
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
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_article_list.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "LIST", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	}
	Db_CloseDyna(dbres);

	if (in_start > 0) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_start - in_lines);
		Put_Nlist(nlp_tmp, "START", cha_sql);
		sprintf(cha_sql, "%d", in_lines);
		Put_Nlist(nlp_tmp, "PAGE", cha_sql);
		chp_string = Get_Nlist(nlp_in, "word", 1);
		if (chp_string && chp_string[0]) {
			Encode_Url(chp_string, &chp_tmp);
			Put_Format_Nlist(nlp_tmp, "SEARCH", "&word=%s", chp_tmp);
			free(chp_tmp);
			if (chp_subject) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_subject=1");
			}
			if (chp_content) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_content=1");
			}
			if (chp_comment) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_comment=1");
			}
			if (chp_trackback) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_trackback=1");
			}
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
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_start + in_lines);
		Put_Nlist(nlp_tmp, "START", cha_sql);
		sprintf(cha_sql, "%d", in_lines);
		Put_Nlist(nlp_tmp, "PAGE", cha_sql);
		chp_string = Get_Nlist(nlp_in, "word", 1);
		if (chp_string && chp_string[0]) {
			Encode_Url(chp_string, &chp_tmp);
			Put_Format_Nlist(nlp_tmp, "SEARCH", "&word=%s", chp_tmp);
			free(chp_tmp);
			if (chp_subject) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_subject=1");
			}
			if (chp_content) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_content=1");
			}
			if (chp_comment) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_comment=1");
			}
			if (chp_trackback) {
				Put_Nlist(nlp_tmp, "SEARCH", "&search_trackback=1");
			}
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
	Page_Out_Mobile(nlp_out, "mobile_show_list.skl");

	return 0;
}
