#include <stdlib.h>
#include "libcgi2.h"
#include "libdb2.h"
#include "libblog.h"
#include "libauth.h"
#include "libmobile.h"
#include "blog_mobile_comment_list.h"

int Mobile_User_Comment_List(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	DBRes *dbres2;
	NLIST *nlp_tmp;
	int i;
	int in_all;
	int in_start;
	int in_entry;
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
		" select sql_calc_found_rows"
		" n_comment_id"
		",c_comment_author"
//		",date_format(d_comment_create_time,'%%Y-%%m-%%d %%H:%%i')"
		",date_format(d_comment_create_time,'%%y年%%m月%%d日')"
		",c_comment_body"
		",c_comment_ip"
		" from at_comment"
		" where n_blog_id = %d"
		" and n_entry_id = %d"
		" and b_comment_accept != 0"
		" order by d_comment_create_time"
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
	Put_Nlist(nlp_out, "CMT", chp_tmp);
	in_all = atoi(chp_tmp);
	Db_CloseDyna(dbres2);

	if (Begin_Transact(db) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "ﾄﾗﾝｻﾞｸｼｮﾝ開始に失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}

	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		NLIST *nlp_tmp;
		nlp_tmp = Init_Nlist();
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (chp_tmp) {
			chp_esc = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_tmp, "NICKNAME", chp_esc);
			free(chp_esc);
		}
		chp_tmp = Db_GetValue(dbres, i, 2);
		if (chp_tmp) {
			Put_Nlist(nlp_tmp, "DATETIME", chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, i, 3);
		if (chp_tmp) {
			chp_bdy = Conv_Long_Ascii(chp_tmp, 56);
			chp_esc = Escape_HtmlString(chp_bdy);
			chp_tmp = Conv_Blog_Br(chp_esc);
			Put_Nlist(nlp_tmp, "BODY", chp_tmp);
			free(chp_bdy);
			free(chp_esc);
		}
		chp_tmp = Db_GetValue(dbres, i, 4);
		if (chp_tmp && !Owner_To_Temp(db, nlp_out, atoi(chp_tmp), cha_sql)) {
			Put_Nlist(nlp_tmp, "FRIEND", cha_sql);
		}
		sprintf(cha_sql, "%d", i + 1);
		Put_Nlist(nlp_tmp, "CUR", cha_sql);
		sprintf(cha_sql, "%d", i + 2);
		Put_Nlist(nlp_tmp, "NEXT", cha_sql);
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_comment_list.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "LIST", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
		if (in_login_blog == in_blog && !g_in_admin_mode) {
			sprintf(cha_sql,
				" update at_comment T1"
				" set b_comment_read = 1"
				" where T1.n_entry_id = %d"
				" and T1.n_blog_id = %d"
				" and T1.n_comment_id = %s"
				, in_entry, in_blog, Db_GetValue(dbres, i, 0));
			if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
				Put_Format_Nlist(nlp_out, "ERROR", "ｸｴﾘーに失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
				Rollback_Transact(db);
				return 1;
			}
		}
	}
	Db_CloseDyna(dbres);

	if (Commit_Transact(db) != CO_SQL_OK) {
		Put_Format_Nlist(nlp_out, "ERROR", "ﾄﾗﾝｻﾞｸｼｮﾝ終了に失敗しました。<br>%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}

	nlp_tmp = Init_Nlist();
	Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
	sprintf(cha_sql, "%d", in_entry);
	Put_Nlist(nlp_tmp, "EID", cha_sql);
	sprintf(cha_sql, "%d", in_start - 5);
	Put_Nlist(nlp_tmp, "START", cha_sql);
	Conv_Depend_String(nlp_tmp);
	if (in_start > 0) {
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_prev_comment.skl");
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
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_next_comment.skl");
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
	Page_Out_Mobile(nlp_out, "mobile_comment_list.skl");

	return 0;
}
