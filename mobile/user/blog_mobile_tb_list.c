#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libdb2.h"
#include "libblog.h"
#include "libauth.h"
#include "libmobile.h"
#include "blog_mobile_tb_list.h"

int Mobile_User_Trackback_List(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	DBRes *dbres2;
	NLIST *nlp_tmp;
	int i;
	int in_all;
	int in_start;
	int in_entry;
	int in_link_blog;
	int in_link_entry;
	char cha_sql[512];
	char *chp_tmp;
	char *chp_esc;
	char *chp_bdy;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "記事を特定できません。<br>");
		return 1;
	}
	Put_Nlist(nlp_out, "EID", chp_tmp);
	in_entry = atoi(chp_tmp);
	sprintf(cha_sql, "%d", in_blog);
	Put_Nlist(nlp_out, "BID", cha_sql);

	chp_tmp = Get_Nlist(nlp_in, "start", 1);
	in_start = chp_tmp ? atoi(chp_tmp) : 0;

	sprintf(cha_sql,
		" select T1.c_blog_title"
		",T1.c_blog_subtitle"
		",T2.c_entry_title"
		",date_format(T2.d_entry_create_time,'%%Y年%%m月%%d日')"
		" from at_blog T1"
		",at_entry T2"
		" where T1.n_blog_id = T2.n_blog_id"
		" and T2.n_blog_id = %d"
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
		Put_Nlist(nlp_out, "SUBTITLE", chp_esc);
		free(chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "SUBJECT", chp_esc);
		free(chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 3);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "DATETIME", chp_esc);
		free(chp_tmp);
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		"select sql_calc_found_rows"
		" n_tb_id"
		",c_tb_title"
//		",date_format(d_tb_create_time,'%%Y-%%m-%%d %%H:%%i')"
		",date_format(d_tb_create_time,'%%y年%%m月%%d日')"
		",c_tb_excerpt"
		",c_tb_url"
		",c_tb_ip"
		" from at_trackback"
		" where n_blog_id = %d"
		" and n_entry_id = %d"
		" order by d_tb_create_time"
		" limit %d,5"
		, in_blog, in_entry, in_start);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	dbres2 = Db_OpenDyna(db, "select found_rows()");
	if(dbres2 == NULL) {
		sprintf(cha_sql, "総件数取得に失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres2, 0, 0);
	Put_Nlist(nlp_out, "TRB", chp_tmp);
	in_all = atoi(chp_tmp);
	Db_CloseDyna(dbres2);
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		NLIST *nlp_tmp;
		nlp_tmp = Init_Nlist();
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (chp_tmp) {
			chp_esc = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_tmp, "TITLE", chp_esc);
			free(chp_esc);
		}
		chp_tmp = Db_GetValue(dbres, i, 2);
		if (chp_tmp) {
			Put_Nlist(nlp_tmp, "DATETIME", chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, i, 3);
		if (chp_tmp) {
			chp_esc = Escape_HtmlString(chp_tmp);
			chp_bdy = Conv_Long_Ascii(chp_esc, 56);
			chp_tmp = Conv_Blog_Br(chp_bdy);
			Put_Nlist(nlp_tmp, "BODY", chp_tmp);
			free(chp_bdy);
			free(chp_esc);
		}
		chp_tmp = Db_GetValue(dbres, i, 4);
		if (chp_tmp) {
			int in_rt;
			char cha_cgi[512];
			char cha_url[1024];
			char cha_blog_id[128] = {0};
			in_link_blog = 0;
			in_link_entry = 0;
			strcpy(cha_url, chp_tmp);
			sprintf(cha_cgi, "%s%s%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location);
			chp_tmp = strchr(cha_url + strlen(cha_cgi), '/');
			if (chp_tmp) {
				in_rt = chp_tmp - (cha_url + strlen(cha_cgi));
				strncpy(cha_blog_id, cha_url + strlen(cha_cgi), in_rt);
				cha_blog_id[in_rt] = '\0';
				if (strlen(cha_blog_id) != 12) {
					cha_blog_id[0] = '\0';
				}
				++chp_tmp;
				if (strncmp(chp_tmp, "?eid=", strlen("?eid=")) == 0) {
					in_link_entry = atoi(chp_tmp + strlen("?eid="));
				}
			}
			if (!cha_blog_id[0]) {
				Put_Format_Nlist(nlp_out, "ERROR", "記事の検索に失敗しました。(%d)<br>", __LINE__);
				Finish_Nlist(nlp_tmp);
				Db_CloseDyna(dbres);
				return 1;
			}
			in_rt = Temp_To_Blog(db, nlp_out, cha_blog_id);
			if (in_rt < 0) {
				Finish_Nlist(nlp_tmp);
				Db_CloseDyna(dbres);
				return 1;
			}
			if (!in_rt) {
				Put_Format_Nlist(nlp_out, "ERROR", "記事の検索に失敗しました。(%d)<br>", __LINE__);
				Finish_Nlist(nlp_tmp);
				Db_CloseDyna(dbres);
				return 1;
			}
			in_link_blog = in_rt;
			Put_Nlist(nlp_tmp, "TBLOG", cha_blog_id);
			Put_Format_Nlist(nlp_tmp, "TEID", "%d", in_link_entry);
			sprintf(cha_sql, "select T1.c_blog_title from at_blog T1 where T1.n_blog_id = %d", in_link_blog);
			dbres2 = Db_OpenDyna(db, cha_sql);
			if(!dbres2) {
				Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
				Finish_Nlist(nlp_tmp);
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_tmp = Db_GetValue(dbres2, 0, 0);
			if (chp_tmp) {
				chp_esc = Escape_HtmlString(chp_tmp);
				Put_Nlist(nlp_tmp, "BLOG_TITLE", chp_esc);
				free(chp_esc);
			}
			Db_CloseDyna(dbres2);
		}
		sprintf(cha_sql, "%d", i + 1);
		Put_Nlist(nlp_tmp, "CUR", cha_sql);
		sprintf(cha_sql, "%d", i + 2);
		Put_Nlist(nlp_tmp, "NEXT", cha_sql);
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_tb_list.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "LIST", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	}
	Db_CloseDyna(dbres);

	nlp_tmp = Init_Nlist();
	Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
	sprintf(cha_sql, "%d", in_entry);
	Put_Nlist(nlp_tmp, "EID", cha_sql);
	sprintf(cha_sql, "%d", in_start - 5);
	Put_Nlist(nlp_tmp, "START", cha_sql);
	Conv_Depend_String(nlp_tmp);
	if (in_start > 0) {
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_prev_tb.skl");
	} else {
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_back_article.skl");
	}
	if (chp_tmp) {
		Put_Nlist(nlp_out, "PREV", chp_tmp);
		free(chp_tmp);
	}
	Finish_Nlist(nlp_tmp);

	if (in_start + 5 < in_all) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_entry);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		sprintf(cha_sql, "%d", in_start + 5);
		Put_Nlist(nlp_tmp, "START", cha_sql);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_next_tb.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "NEXT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "NEXT", "次の5件");
	}

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_blog, NULL);
	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_tb_list.skl");

	return 0;
}
