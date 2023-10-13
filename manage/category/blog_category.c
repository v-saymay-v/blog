/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_category.c
 * First-Created:	2004/07/30 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	カテゴリーの編集
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_category.c,v 1.73 2008/02/15 09:26:04 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include "libcgi2.h"
#include "libdb2.h"
#include "libcommon2.h"
#include "libauth.h"
#include "libblog.h"
#include "libblogcart.h"
#include "libblogreserve.h"

#define CO_COLUMN_NAME_ID		"n_category_id"
#define CO_COLUMN_NAME_ORDER	"n_category_order"
#define CO_TABLE_NAME			"at_category"
#define CO_WHERE_ETC			"n_category_id <> 0"

/*
+* ------------------------------------------------------------------------
 * Function:	put_error_data
 * Description:
 *	エラー時の埋め込み。
%* ------------------------------------------------------------------------
 * Return:
 *	(無し)
-* ------------------------------------------------------------------------*/
void put_error_data(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chpa_esc[] = {
		  "BTN_DISP_CATEGORY", "BTN_DOWN_CATEGORY.x", "BTN_DOWN_CATEGORY.x" ,"BTN_UP_CATEGORY.y" ,"BTN_UP_CATEGORY.y"
		, "BTN_BOTTOM_CATEGORY.x", "BTN_BOTTOM_CATEGORY.x", "BTN_TOP_CATEGORY.x", "BTN_TOP_CATEGORY.x"
		, "BTN_MODIFY_CATEGORY.x", "BTN_MODIFY_CATEGORY.y", "blogid", "BTN_NEW_CATEGORY.x", "BTN_NEW_CATEGORY.y"
		, "BTN_DISP_DELETECATEGORY_ASK.x", "BTN_DISP_DELETECATEGORY_ASK.y", "BTN_DISP_NEWCATEGORY.x", "BTN_DISP_NEWCATEGORY.y"
		, "BTN_DISP_EDITCATEGORY.x", "BTN_DISP_EDITCATEGORY.y", "BTN_DELETE_CATEGORY", NULL
	};

	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	if(Get_Nlist(nlp_in, "BTN_DISP_CATEGORY", 1)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
	} else {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CATEGORY);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "from_error", "1");
	return;
}
/*
+* ------------------------------------------------------------------------
 * Function:		id_check()
 * Description:
 *	idが選択されているか。
 *	かつ、idが存在するか。
%* ------------------------------------------------------------------------
 * Return:
 *      正常終了 0
 *	未選択 1
 *	idに誤り/idが既に削除ずみ 2
 *	クエリエラー CO_ERROR
-* ------------------------------------------------------------------------*/
int id_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_id;
	char cha_sql[1024];

	chp_id = Get_Nlist(nlp_in, "categoryid", 1);
	if(!chp_id) {
		Put_Nlist(nlp_out, "ERROR", "カテゴリーを選択してください。");
		return 1;
	} else if(Check_Numeric(chp_id)) {
		Put_Nlist(nlp_out, "ERROR", "カテゴリー情報に誤りがあります。");
		return 2;
	}
	strcpy(cha_sql, "select T1.n_category_id");
	strcat(cha_sql, " from at_category T1");
	strcat(cha_sql, " where T1.n_category_id = ");
	strcat(cha_sql, chp_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "カテゴリーの有無の調査に失敗しました。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	if(!Db_GetRowCount(dbres)) {
		Put_Nlist(nlp_out, "ERROR", "選択したカテゴリーは既に削除されています。");
		Db_CloseDyna(dbres);
		return 2;
	}
	Db_CloseDyna(dbres);
	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	check_same_name_conflict_category()
 * Description:
 *	カテゴリー名の同名チェック
 *	新規登録時は、in_category_idに負数を渡すなどすればよい。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	同名のカテゴリ名があった場合 1
 *	クエリーエラー CO_ERROR
-* ------------------------------------------------------------------------*/
int check_same_name_conflict_category(
	  DBase *db
	, NLIST *nlp_out
	, char *chp_category_name	/* 登録しようとしているカテゴリ名 */
	, int in_category_id		/* 更新するID */
	, int in_blog
)
{
	DBRes *dbres;
	char *chp_escape;
	char cha_sql[512];
	char cha_msg[1024];
	int in_err;

	sprintf(cha_sql, "select count(*) from at_category where n_blog_id = %d and n_category_id > 0", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Format_Nlist(nlp_out, "ERROR", "カテゴリー数をチェックするクエリに失敗しました。(%s)", Gcha_last_error);
		return CO_ERROR;
	}
	chp_escape = Db_GetValue(dbres, 0, 0);
	if (!chp_escape) {
		Put_Nlist(nlp_out, "ERROR", "カテゴリー数を取得できませんでした。");
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_err = atoi(chp_escape);
	Db_CloseDyna(dbres);
	if (in_err >= CO_MAX_CATEGORIES) {
		Put_Format_Nlist(nlp_out, "ERROR", "カテゴリーがすでに %d件 登録されています。", CO_MAX_CATEGORIES);
		return 1;
	}

	/* カテゴリIDが指定したもの以外で、カテゴリ名が一致するものがあるかどうか調べる */
	strcpy(cha_sql, "select T1.n_category_id");	/* 0 カテゴリID */
	strcat(cha_sql, " from at_category T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_category_id <> %d", in_category_id);
	strcat(cha_sql, " and T1.c_category_name = '");
	chp_escape = My_Escape_SqlString(db, chp_category_name);
	strcat(cha_sql, chp_escape);
	free(chp_escape);
	strcat(cha_sql, "'");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		memset(cha_msg, '\0', sizeof(cha_msg));
		sprintf(cha_msg, "カテゴリー名をチェックするクエリに失敗しました。(%s)", Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_msg);
		return CO_ERROR;
	}
	in_err = 0;
	/* 存在したらエラー */
	if(Db_GetRowCount(dbres)) {
		memset(cha_msg, '\0', sizeof(cha_msg));
		chp_escape = Escape_HtmlString(chp_category_name);
		sprintf(cha_msg, "カテゴリー名「%s」は、すでに使われています。<br>", chp_escape);
		free(chp_escape);
		Put_Nlist(nlp_out, "ERROR", cha_msg);
		in_err = 1;
	}
	Db_CloseDyna(dbres);
	return in_err;
}

/*
+* ------------------------------------------------------------------------
 * Function:            error_check
 * Description:
 *	エラーチェック
%* ------------------------------------------------------------------------
 * Return:
 * 	正常終了 0
 *	エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int error_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char cha_msg[8192];
	int in_err;

	in_err = 0;
	chp_tmp = Get_Nlist(nlp_in, "category_name", 1);
	if(!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "カテゴリー名が取得できません。");
		return 1;
	}
	if(strlen(chp_tmp) == 0) {
		Put_Nlist(nlp_out, "ERROR", "カテゴリー名を入力してください。");
		in_err = 1;
	} else {
		if(Check_Space_Only(chp_tmp)) {
			Put_Nlist(nlp_out, "ERROR", "カテゴリー名は、空白以外の文字を含めて入力してください。<br>");
			in_err = 1;
		}
		if(strlen(chp_tmp) > CO_MAXLEN_CATEGORY_NAME) {
			sprintf(cha_msg, "カテゴリー名は半角%d文字（全角%d文字）以内で入力してください。<br>"
				, CO_MAXLEN_CATEGORY_NAME, CO_MAXLEN_CATEGORY_NAME/2);
			Put_Nlist(nlp_out, "ERROR", cha_msg);
			in_err = 1;
		}
		if(in_err == 0) {
			if(Get_Nlist(nlp_in, "BTN_NEW_CATEGORY.x", 1)) {
				if(check_same_name_conflict_category(db, nlp_out, chp_tmp, -1, in_blog)) {
					in_err = 1;
				}
			} else if(Get_Nlist(nlp_in, "BTN_MODIFY_CATEGORY.x", 1)) {
				if(check_same_name_conflict_category(db, nlp_out, chp_tmp, atoi(Get_Nlist(nlp_in, "categoryid", 1)), in_blog)) {
					in_err = 1;
				}
			}
		}
	}
	return in_err;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_category()
 * Description:
 *	カテゴリーの編集ページを表示する。カテゴリ未登録のときは専用ページへ。
%* ------------------------------------------------------------------------
 * Return:
 *
-* ------------------------------------------------------------------------*/
char *disp_page_category(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_id;
	char cha_sql[512];
	int in_count;

	chp_id = Get_Nlist(nlp_in, "categoryid", 1);
	in_count = Get_Count_Category(db, in_blog);
	if(in_count == CO_ERROR) {
		Put_Nlist(nlp_out, "ERROR", "カテゴリー数を取得できませんでした。");
		return NULL;
	/* カテゴリーID[0]で「カテゴリー無し」が常に登録されているため、常に1個ある。 */
	} else if(in_count == 1) {
		return CO_SKEL_CATEGORY_LIST_EMPTY;
	} else {
		strcpy(cha_sql, "select T1.n_category_id");
		strcat(cha_sql, " , T1.c_category_name");
		strcat(cha_sql, " from at_category T1");
		strcat(cha_sql, " where T1.n_category_id > 0");
		strcat(cha_sql, " and T1.n_category_id != 65535");
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id=%d", in_blog);
		strcat(cha_sql, " order by n_category_order");
		Build_MultiComboDb(OldDBase(db), nlp_out, "LIST", "categoryid", cha_sql, 15, chp_id);
		return CO_SKEL_CATEGORY_LIST;
	}
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_deletecategory_ask()
 * Description:
 *	カテゴリ削除確認画面を表示する
%* ------------------------------------------------------------------------
 * Return:
 *	正常0　失敗 1
-* ------------------------------------------------------------------------*/
int disp_page_deletecategory_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_id;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	char cha_msg[512];

	/* 選択されてなければエラー */
	if(id_check(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	chp_id = Get_Nlist(nlp_in, "categoryid", 1);
	Build_HiddenEncode(nlp_out, "HIDDEN", "categoryid", chp_id);
	strcpy(cha_sql, "select T1.c_category_name");	/* カテゴリ名 */
	strcat(cha_sql, " from at_category T1");
	strcat(cha_sql, " where n_category_id =");
	strcat(cha_sql, chp_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "カテゴリーの有無を確認するクエリに失敗しました。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	if(!Db_GetRowCount(dbres)) {
		Put_Nlist(nlp_out, "ERROR", "このカテゴリーは既に削除されています。");
		Db_CloseDyna(dbres);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		sprintf(cha_msg, "カテゴリー「%s」を削除してもよろしいですか？", chp_escape);
		free(chp_escape);
	} else {
		strcpy(cha_msg, "選択されたカテゴリーを削除してもよろしいですか?");
	}
	Put_Nlist(nlp_out, "MSG", cha_msg);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	insert_new_category()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int insert_new_category(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_tmp;
	char *chp_escape;
	int in_category;
	int in_order;
	int in_skip;

	if(error_check(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	if(Begin_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
		return 1;
	}
	sprintf(cha_sql,
		" select coalesce(max(T1.n_category_id) + 1, 1),"
		" coalesce(max(T1.n_category_order) + 1, 1)"
		" from at_category T1"
		" where T1.n_blog_id = %d"
		" and T1.n_category_id < 65535"
		" for update", in_blog);	/* カテゴリID最大値 */
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_category = atoi(chp_tmp);
	} else {
		in_category = 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_order = atoi(chp_tmp);
	} else {
		in_order = 1;
	}
	Db_CloseDyna(dbres);

	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		strcpy(cha_sql, "insert into at_category");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_category_id");
		strcat(cha_sql, ",n_category_order");
		strcat(cha_sql, ",c_category_name");
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d, %d", in_blog, in_category, in_order);	/* カテゴリID最大値 */
		strcat(cha_sql, ", '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "category_name", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "')");	/* 初期選択カテゴリ。初期値は常に0 */
		if(Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
	}
	if(Commit_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
		Rollback_Transact(db);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	modify_category()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1,2,3
-* ------------------------------------------------------------------------*/
int modify_category(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_id;
	char *chp_escape;
	char cha_sql[512];
	int in_skip;

	if(id_check(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	if(error_check(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	chp_id = Get_Nlist(nlp_in, "categoryid", 1);
	if(Begin_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
		return 1;
	}
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		strcpy(cha_sql, "update at_category");
		strcat(cha_sql, " set c_category_name = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "category_name", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "' where n_category_id = ");
		strcat(cha_sql, chp_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id=%d", in_blog);
		if(Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
	}
	if(Commit_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
		Rollback_Transact(db);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_category()
 * Description:
 *	カテゴリを削除する。
 *	同時に、所属エントリーを「カテゴリー無し」に移す。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int delete_category(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_id;
	char *chp_tmp;
	char cha_sql[512];
	int in_skip;

	if (id_check(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	chp_id = Get_Nlist(nlp_in, "categoryid", 1);

	sprintf(cha_sql, "select count(*) from at_entry where n_category_id = %s and n_blog_id = %d", chp_id, in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "記事数取得クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s", Gcha_last_error, cha_sql);
		Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_CATEGORY", "1");
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp && atoi(chp_tmp)) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "このカテゴリには記事が存在するため、削除できません。<br>");
		Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_CATEGORY", "1");
		return 1;
	}
	Db_CloseDyna(dbres);

	if(Begin_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_CATEGORY", "1");
		return 1;
	}
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		strcpy(cha_sql, "delete from at_category");
		strcat(cha_sql, " where n_category_id = ");
		strcat(cha_sql, chp_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id=%d", in_blog);
		if(Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "削除するクエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_CATEGORY", "1");
			return 1;
		};
	}
	if(Commit_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
		Rollback_Transact(db);
		Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_CATEGORY", "1");
		return 1;
	}
	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_newcategory()
 * Description:
 *	カテゴリー新規作成ページを表示
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
-* ------------------------------------------------------------------------*/
int disp_page_newcategory(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;

	Put_Nlist(nlp_out, "HIDDEN", "<INPUT TYPE=\"hidden\" NAME=\"BTN_NEW_CATEGORY.x\">\n");
	Put_Format_Nlist(nlp_out, "RESET", "%s?blogid=%d&BTN_DISP_NEWCATEGORY.x=1", CO_CGI_CATEGORY, in_blog);
	Put_Format_Nlist(nlp_out, "BUTTON", "<input type=\"image\" src=\"%s/touroku.gif\" value=\"登録\" name=\"BTN_NEW_CATEGORY\">", g_cha_admin_image);
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_CATEGORY=1&blogid=%d%s",
		CO_CGI_CATEGORY, in_blog, (chp_tmp && atoi(chp_tmp)) ? "&from_user=1" : "");
	Put_Nlist(nlp_out, "TITLE", "登録");
	/* エラー時引継ぎ */
	if(Get_Nlist(nlp_in, "from_error", 1)) {
		chp_tmp = Escape_HtmlString(Get_Nlist(nlp_in, "category_name", 1));
		Put_Nlist(nlp_out, "CATEGORYNAME", chp_tmp);
		free(chp_tmp);
	}
	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_editcategory()
 * Description:
 *	カテゴリー編集ページを表示
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int disp_page_editcategory(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_id;
	char *chp_from_error;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];

	if(id_check(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	chp_id = Get_Nlist(nlp_in, "categoryid", 1);
	Put_Nlist(nlp_out, "HIDDEN", "<INPUT TYPE=\"hidden\" NAME=\"BTN_MODIFY_CATEGORY.x\">\n");
	Build_HiddenEncode(nlp_out, "HIDDEN", "categoryid", chp_id);
	chp_from_error = Get_Nlist(nlp_in, "from_error", 1);
	dbres = NULL;	/* エラー抑制 */
	if(chp_from_error) {
		chp_tmp = Get_Nlist(nlp_in, "category_name", 1);
	} else {
		strcpy(cha_sql, "select T1.c_category_name");
		strcat(cha_sql, " from at_category T1");
		strcat(cha_sql, " where n_category_id =");
		strcat(cha_sql, chp_id);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id=%d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(!chp_tmp){
			Put_Nlist(nlp_out , "ERROR" ,"カテゴリー名の取得に失敗しました。");
			Db_CloseDyna(dbres);
			return 1;
		}
	}
	chp_escape = Escape_HtmlString(chp_tmp);
	Put_Nlist(nlp_out, "CATEGORYNAME", chp_escape);
	free(chp_escape);
	Put_Format_Nlist(nlp_out, "RESET", "%s?BTN_DISP_EDITCATEGORY.x=1&blogid=%d&categoryid=%s", CO_CGI_CATEGORY, in_blog, chp_id);
	Put_Format_Nlist(nlp_out, "BUTTON", "<input type=\"image\" src=\"%s/reload.gif\" value=\"更新\" name=\"BTN_MODIFY_CATEGORY\">", g_cha_admin_image);
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_CATEGORY=1&blogid=%d%s",
		CO_CGI_CATEGORY, in_blog, (chp_tmp && atoi(chp_tmp)) ? "&from_user=1" : "");
	Put_Nlist(nlp_out, "TITLE", "編集");
	if(!chp_from_error) {
		Db_CloseDyna(dbres);
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	main()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int main()
{
	NLIST *nlp_in;
	NLIST *nlp_out;
	DBase *db;
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char cha_where[1024];
	char cha_blog[32];
	char *chp_tmp;
	char *chp_skel;
	char *chp_fromuser;
	int in_error;
	int in_blog;
	int in_orig;

	in_error = Read_Blog_Ini(cha_host,cha_db, cha_username, cha_password, &g_in_ssl_mode);
	if (in_error) {
		printf("\n\nINIファイルの読み込みに失敗しました。(%s)", strerror(in_error));
		return in_error;
	}
	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		printf("\n\nデータベース接続に失敗しました。");
		return 1;
	}
	if (Read_Blog_Option(db)) {
		printf("\n\nブログオプションの読み込みに失敗しました。");
		Db_Disconnect(db);
		return 1;
	}
	Set_SkelPath(g_cha_admin_skeleton);

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	Put_Nlist(nlp_out, "CSS", g_cha_css_location);
	Put_Nlist(nlp_out, "SCRIPTS", g_cha_script_location);
	Put_Nlist(nlp_out, "IMAGES", g_cha_admin_image);

	if (g_in_hb_mode) {
		Delete_Old_Entry(db);
	}

	if (g_in_hosting_mode) {
		in_blog = Check_Remote_User(db, nlp_out);
	} else {
		in_blog = Check_Login(db, nlp_out, &g_in_login_owner, &g_in_admin_mode);
	}
	if (in_blog < 0) {
		Put_Nlist(nlp_out, "ERR_START", "-->");
		Put_Nlist(nlp_out, "ERR_END", "<!--");
		Page_Out(nlp_out, CO_SKEL_LOGIN);
		goto clear_finish;
	}
	if (!in_blog) {
		if (Get_Nlist(nlp_in, CO_LOGIN_ID, 1) && Get_Nlist(nlp_in, CO_PASSWORD, 1)) {
			in_blog = Auth_Login(db, nlp_in, nlp_out, CO_LOGIN_ID, CO_PASSWORD, &g_in_login_owner);
			if (in_blog < 0) {
				Put_Nlist(nlp_out, "ERR_START", "-->");
				Put_Nlist(nlp_out, "ERR_END", "<!--");
				Page_Out(nlp_out, CO_SKEL_LOGIN);
				goto clear_finish;
			} else if (!in_blog) {
				Put_Nlist(nlp_out, "ERROR", "ログインIDまたはパスワードが間違っています。<br>");
				Put_Nlist(nlp_out, "ERR_START", "-->");
				Put_Nlist(nlp_out, "ERR_END", "<!--");
				Page_Out(nlp_out, CO_SKEL_LOGIN);
				goto clear_finish;
			}
		} else {
			if (g_in_dbb_mode) {
				in_error = Jump_To_Login_Cgi(db, nlp_out);
				goto clear_finish;
			} else if (g_in_hosting_mode) {
				in_blog = Check_Unix_User(db, nlp_out);
				if (in_blog < 0) {
					Put_Nlist(nlp_out, "ERR_START", "-->");
					Put_Nlist(nlp_out, "ERR_END", "<!--");
					Page_Out(nlp_out, CO_SKEL_LOGIN);
					goto clear_finish;
				} else if (!in_blog) {
					Put_Nlist(nlp_out, "ACTION", CO_CGI_CATEGORY);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "認証情報を取得できません。<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_CATEGORY);
				Page_Out(nlp_out, CO_SKEL_LOGIN);
				goto clear_finish;
			}
		}
	}
	in_orig = in_blog;
	chp_tmp = Get_Nlist(nlp_in, "blogid", 1);
	if (g_in_hb_mode || g_in_cart_mode == CO_CART_RESERVE) {
		if (chp_tmp && chp_tmp[0]) {
			Put_Nlist(nlp_out, "BLOGID", chp_tmp);
			in_blog = atoi(chp_tmp);
		} else {
			Put_Nlist(nlp_out, "BLOGID", "1");
			in_blog = 1;
		}
	}
	if (g_in_dbb_mode && In_Black_List(db, nlp_out, g_in_login_owner, NULL)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CATEGORY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CATEGORY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CATEGORY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログはお使いになれません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_RESERVE && Get_Blog_Priv(db, nlp_out, g_in_login_owner) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_BOOKMARKLET);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを操作する権限がありません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_hb_mode && in_orig == INT_MAX && Get_Blog_Auth(db, nlp_out, g_in_login_owner, in_blog) < CO_AUTH_DELETE) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CATEGORY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを操作する権限がありません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CATEGORY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを作成されていない方はご利用になれません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);
	sprintf(cha_where, "n_category_id <> 0 and n_blog_id = %d", in_blog);

	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_fromuser) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_fromuser);
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
		Put_Nlist(nlp_out, "TOP", "ブログ");
		Put_Nlist(nlp_out, "GIF", "go_blog.gif");
	} else {
		Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%s?blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
		Put_Nlist(nlp_out, "TOP", "管理メニュー");
		Put_Nlist(nlp_out, "GIF", "go_kanri.gif");
	}

	in_error = 0;
	chp_skel = NULL;
	if(Get_Nlist(nlp_in, "BTN_DISP_CATEGORY", 1)) {
		chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
	} else if(Get_Nlist(nlp_in, "BTN_NEW_CATEGORY.x", 1)) {
		if(insert_new_category(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if(disp_page_newcategory(db, nlp_in, nlp_out, in_blog)) {
				chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
			} else {
				chp_skel = CO_SKEL_CATEGORY_EDIT;
			}
		} else {
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s?blogid=%d&BTN_DISP_CATEGORY=1%s",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_CATEGORY, in_blog, (chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
			Put_Nlist(nlp_out, "MESSAGE", "新しいカテゴリーを追加しました。");
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "TITLE", "DBB_blog 管理者メニュー");
			} else if (g_in_cart_mode == CO_CART_SHOPPER) {
				Put_Nlist(nlp_out, "TITLE", "店長ブログ管理メニュー");
			} else if (g_in_cart_mode == CO_CART_RESERVE) {
				Put_Nlist(nlp_out, "TITLE", "eリザーブブログ管理メニュー");
			} else {
				Put_Nlist(nlp_out, "TITLE", "AS-BLOG 管理者メニュー");
			}
			chp_skel = CO_SKEL_CONFIRM;
		}
	} else if(Get_Nlist(nlp_in, "BTN_MODIFY_CATEGORY.x", 1)) {
		if(modify_category(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Build_HiddenEncode(nlp_out, "HIDDEN", "from_error", "1");
			if (!Get_Nlist(nlp_in, "from_error", 1)) {
				Put_Nlist(nlp_in, "from_error", "1");
			}
			if(disp_page_editcategory(db, nlp_in, nlp_out, in_blog)) {
				chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
			} else {
				chp_skel = CO_SKEL_CATEGORY_EDIT;
			}
		} else {
			Put_Nlist(nlp_out, "MESSAGE", "カテゴリーを更新しました。");
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s?blogid=%d&BTN_DISP_CATEGORY=1%s",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_CATEGORY, in_blog, chp_fromuser ? "&from_user=1" : "");
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "TITLE", "DBB_blog 管理者メニュー");
			} else if (g_in_cart_mode == CO_CART_SHOPPER) {
				Put_Nlist(nlp_out, "TITLE", "店長ブログ管理メニュー");
			} else if (g_in_cart_mode == CO_CART_RESERVE) {
				Put_Nlist(nlp_out, "TITLE", "eリザーブブログ管理メニュー");
			} else {
				Put_Nlist(nlp_out, "TITLE", "AS-BLOG 管理者メニュー");
			}
			chp_skel = CO_SKEL_CONFIRM;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DISP_DELETECATEGORY_ASK.x", 1)) {
		if(disp_page_deletecategory_ask(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
		} else {
			chp_skel = CO_SKEL_DELETE_CATEGORY_ASK;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DISP_NEWCATEGORY.x", 1)) {
		if(disp_page_newcategory(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
		} else {
			chp_skel = CO_SKEL_CATEGORY_EDIT;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DISP_EDITCATEGORY.x", 1)) {
		if(disp_page_editcategory(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
		} else {
			chp_skel = CO_SKEL_CATEGORY_EDIT;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DELETE_CATEGORY", 1)) {
		in_error = delete_category(db, nlp_in, nlp_out, in_blog);
		if (in_error) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
		} else {
			sprintf(cha_where, "%s%s%s/%s?blogid=%d&BTN_DISP_CATEGORY=1%s",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_CATEGORY, in_blog, chp_fromuser ? "&from_user=1" : "");
			Put_Nlist(nlp_out, "MESSAGE", "カテゴリーを削除しました。");
			Put_Nlist(nlp_out, "RETURN", cha_where);
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "TITLE", "DBB_blog 管理者メニュー");
			} else if (g_in_cart_mode == CO_CART_SHOPPER) {
				Put_Nlist(nlp_out, "TITLE", "店長ブログ管理メニュー");
			} else if (g_in_cart_mode == CO_CART_RESERVE) {
				Put_Nlist(nlp_out, "TITLE", "eリザーブブログ管理メニュー");
			} else {
				Put_Nlist(nlp_out, "TITLE", "AS-BLOG 管理者メニュー");
			}
			chp_skel = CO_SKEL_CONFIRM;
		}
	} else if(Get_Nlist(nlp_in, "BTN_TOP_CATEGORY.x", 1)) {
		if(Change_Order_Db(OldDBase(db), Get_Nlist(nlp_in, "categoryid", 1), CO_COLUMN_NAME_ID, CO_COLUMN_NAME_ORDER, CO_TABLE_NAME, cha_where, "t")) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
	} else if(Get_Nlist(nlp_in, "BTN_UP_CATEGORY.x", 1)) {
		if(Change_Order_Db(OldDBase(db), Get_Nlist(nlp_in, "categoryid", 1), CO_COLUMN_NAME_ID, CO_COLUMN_NAME_ORDER, CO_TABLE_NAME, cha_where, "u")) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
	} else if(Get_Nlist(nlp_in, "BTN_DOWN_CATEGORY.x", 1)) {
		if(Change_Order_Db(OldDBase(db), Get_Nlist(nlp_in, "categoryid", 1), CO_COLUMN_NAME_ID, CO_COLUMN_NAME_ORDER, CO_TABLE_NAME, cha_where, "d")) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
	} else if(Get_Nlist(nlp_in, "BTN_BOTTOM_CATEGORY.x", 1)) {
		if(Change_Order_Db(OldDBase(db), Get_Nlist(nlp_in, "categoryid", 1), CO_COLUMN_NAME_ID, CO_COLUMN_NAME_ORDER, CO_TABLE_NAME, cha_where, "b")) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		chp_skel = disp_page_category(db, nlp_in, nlp_out, in_blog);
	} else {
		Put_Nlist(nlp_out, "ERROR", "直接CGIを実行できません。<br>");
		in_error++;
	}
	if(in_error || !chp_skel) {
		put_error_data(nlp_in, nlp_out);
		Page_Out(nlp_out, CO_SKEL_ERROR);
	} else {
		Page_Out(nlp_out, chp_skel);
	}

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
