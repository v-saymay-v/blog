/*
+* ------------------------------------------------------------------------
 * Module-Name:         libblogreserve.c
 * First-Created:       2006/11/01 堀 正明
%* ------------------------------------------------------------------------
 * Module-Description:
 *	blogの認証関連のライブラリ。
-* ------------------------------------------------------------------------
 * Change-Log:
$* ------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <errno.h>
#include <sys/types.h>
#include <libcgi2.h>
#include "libblog.h"
#include "libauth.h"
#include "libblogreserve.h"

static char g_cha_database[256];
static char g_cha_imageloc[256];
static char g_cha_purchase_cgi[1024] = "rsv_reserve.cgi";
static const char *chpa_category_table[] = {
	"rt_hotel_category",
	"rt_salon_category",
	"rt_hospital_category",
	"rt_restaurant_category",
	"rt_school_category",
	"rt_facilities_category",
	"rt_rental_category"
};

/*
+* ========================================================================
 * Function:	eリザーブ用のデータベースをオープンする
 * Description:
 *	データベースからeリザーブ用DBのアクセス情報を取り出し、オープンする。
%* ========================================================================
 * Return:
 *	!= NULL: 正常終了
 *	== NULL: エラー
=* ======================================================================*/
DBase* Open_Reserve_Db(DBase *db, NLIST *nlp_out)
{
	char cha_host[256];
	char cha_userid[21];
	char cha_password[21];
	char cha_passfile[256];
	char cha_sql[1024];
	char *chp_tmp;
	DBRes *dbres;
	DBase *cartdb;
	FILE *fp;
	struct passwd *stp_pwd;

	sprintf(cha_sql,
		" select c_host"
		",c_database"
		",c_passfile"
		",c_imageloc"
		",c_purchase_cgi"
		" from sy_cartinfo");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "商品の情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return NULL;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "商品の情報を得るクエリに失敗しました。<br>");
		Db_CloseDyna(dbres);
		return NULL;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "");
	strcpy(g_cha_database, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "");
	strcpy(g_cha_imageloc, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "");
	strcpy(g_cha_purchase_cgi, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
	}

	fp = fopen(cha_passfile, "r");
	if (!fp) {
		Put_Format_Nlist(nlp_out, "ERROR", "パスワードの読み込みに失敗しました。<br>%s<br>%s", strerror(errno), cha_passfile);
		return NULL;
	}
	fgets(cha_passfile, sizeof(cha_passfile), fp);
	fclose(fp);
	chp_tmp = strchr(cha_passfile, '\n');
	if (chp_tmp)
		*chp_tmp = '\0';
	chp_tmp = strchr(cha_passfile, '\r');
	if (chp_tmp)
		*chp_tmp = '\0';
	chp_tmp = strtok(cha_passfile, ":");
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "パスワードの形式が間違っています。<br>");
		return NULL;
	}
	strcpy(cha_userid, chp_tmp);
	chp_tmp = strtok(NULL, ":");
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "パスワードの形式が間違っています。<br>");
		return NULL;
	}
	strcpy(cha_password, chp_tmp);
	Replace_String("#DBUSER#", cha_userid, g_cha_database);

	if (g_in_ssl_mode) {
		cartdb = Db_ConnectSSL(cha_host, g_cha_database, cha_userid, cha_password);
	} else {
		cartdb = Db_ConnectWithParam(cha_host, g_cha_database, cha_userid, cha_password);
	}
	if (!cartdb) {
		Put_Format_Nlist(nlp_out, "ERROR", "カートDBへの接続に失敗しました。<br>%s", Gcha_last_error);
	}

	return cartdb;
}

/*
+* ========================================================================
 * Function:	関連予約コンボ作成
 * Description:
 *	記事投稿時の関連する予約を選択するコンボボックスを作成する
%* ========================================================================
 * Return:
 *	0: 正常終了
 *	1: エラー
=* ======================================================================*/
int Build_Reserve_Combo(DBase *db, NLIST *nlp_out, char *chp_target, char *chp_name, char *chp_select, int in_blog, int in_owner)
{
	DBase *rsvdb;
	DBRes *dbres;
	char cha_sql[512];
	char cha_value[128];
	char *chp_tmp;
	char *chp_esc;
	int i, j;
	int in_cnt;
	int in_type;
	int in_branch;
	int in_category;
	int in_arrange;

	// DBをオープンする
	rsvdb = Open_Reserve_Db(db, nlp_out);
	if (!rsvdb) {
		return 1;
	}

	// 業種をえる
	sprintf(cha_sql,
		" select T2.n_type"
		",T2.n_branch"
		",T3.n_arrange"
		",T2.c_name"
		" from sy_login T1"
		",rt_category_type T2"
		",rt_option T3"
		" where T1.n_admin = %d"
		" and T2.n_blog_id = %d"
		" and T2.b_use != 0"
		" and T1.n_branch = T2.n_branch"
		" and T2.n_type = T3.n_type"
		" and T2.n_branch = T3.n_branch"
		, in_owner, in_blog);
	dbres = Db_OpenDyna(rsvdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "予約情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return 1;
	}
	in_cnt = Db_GetRowCount(dbres);
//	if (!in_cnt) {
//		Put_Nlist(nlp_out, "ERROR", "業種を特定できません。<br>");
//		Db_CloseDyna(dbres);
//		return 1;
//	}
	Put_Format_Nlist(nlp_out, chp_target, "<select name=\"%s\">\n", chp_name);
	Put_Nlist(nlp_out, chp_target, "<option value=\"\">(関連予約なし)</option>\n");
	for (i = 0; i < in_cnt; ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (!chp_tmp)
			continue;
		in_type = atoi(chp_tmp);
		if (!in_type || in_type >= (int)(sizeof(chpa_category_table)/sizeof(char*)) + 1)
			continue;
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (!chp_tmp)
			continue;
		in_branch = atoi(chp_tmp);
		chp_tmp = Db_GetValue(dbres, i, 2);
		if (!chp_tmp)
			continue;
		in_arrange = atoi(chp_tmp);
		if (in_arrange == 1) {
			DBRes *dbres2;
			sprintf(cha_sql, "select n_category,c_name from %s where n_branch = %d", chpa_category_table[in_type - 1], in_branch);
			dbres2 = Db_OpenDyna(rsvdb, cha_sql);
			if (!dbres2) {
				Put_Format_Nlist(nlp_out, "ERROR", "予約情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
				return 1;
			}
			for (j = 0; j < Db_GetRowCount(dbres2); ++j) {
				chp_tmp = Db_GetValue(dbres2, j, 0);
				if (!chp_tmp) {
					Db_CloseDyna(dbres2);
					continue;
				}
				in_category = atoi(chp_tmp);
				sprintf(cha_value, "%d:%d:%d:1", in_type, in_branch, in_category);
				chp_tmp = Db_GetValue(dbres2, j, 1);
				chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
				Put_Format_Nlist(nlp_out, chp_target, "<option value=\"%s\"", cha_value);
				if (chp_select && strcmp(cha_value, chp_select) == 0) {
					Put_Nlist(nlp_out, chp_target, " selected");
				}
				Put_Format_Nlist(nlp_out, chp_target, ">%s</option>\n", chp_esc);
				free(chp_esc);
			}
			Db_CloseDyna(dbres2);
		} else {
			sprintf(cha_value, "%d:%d::", in_type, in_branch);
			chp_tmp = Db_GetValue(dbres, i, 3);
			chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
			Put_Format_Nlist(nlp_out, chp_target, "<option value=\"%s\"", cha_value);
			if (chp_select && strcmp(cha_value, chp_select) == 0) {
				Put_Nlist(nlp_out, chp_target, " selected");
			}
			Put_Format_Nlist(nlp_out, chp_target, ">%s</option>\n", chp_esc);
			free(chp_esc);
		}
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(rsvdb);
	Put_Nlist(nlp_out, chp_target, "</select>\n");
	return 0;
}

/*
+* ========================================================================
 * Function:	関数呼び出し
 * Description:
 *	各関数を呼び出す
%* ========================================================================
 * Return:
 *	0: 正常終了
 *	1: エラー
=* ========================================================================
 */
int Build_Reserve_Item(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_item_id, char *chp_target)
{
#define CO_RSV_TYPE_HOTEL		1
#define CO_RSV_TYPE_SALON		2
#define CO_RSV_TYPE_HOSPITAL	3
#define CO_RSV_TYPE_RESTAURANT	4
#define CO_RSV_TYPE_SCHOOL		5
#define CO_RSV_TYPE_FACILITIES	6
#define CO_RSV_TYPE_RENTAL		7
#define CO_RSV_FUNCTION_HOME_BUTTON	112
	DBase *rsvdb;
	DBRes *dbres;
	char cha_tmp[1024];
	char cha_sql[1024];
	char cha_image_path[256];
	char cha_reserve_url[256];
	char cha_plan_url[256];
	char cha_func112_url[256];
	char *chp_tmp;
	char *chp_esc;
	char *chp_type;
	char *chp_branch;
	char *chp_category;
	char *chp_calendar;
	char *chp_plan_name;
	char *chp_plan_detail;
	bool b_func112;

	strcpy(cha_tmp, chp_item_id);
	chp_type = strtok(cha_tmp, ":");
	chp_branch = strtok(NULL, ":");
	chp_category = strtok(NULL, ":");
	chp_calendar = strtok(NULL, ":");

	if (!chp_type || !chp_branch) {
		return 0;
	}

	chp_plan_name = "";
	chp_plan_detail = "";
	cha_image_path[0] = '\0';
	cha_reserve_url[0] = '\0';
	cha_plan_url[0] = '\0';
	
	// DBをオープンする
	rsvdb = Open_Reserve_Db(db, nlp_out);
	if (!rsvdb) {
		return 1;
	}
	sprintf(cha_reserve_url, "%s?type=%s&branch=%s", g_cha_purchase_cgi, chp_type, chp_branch);
	if (chp_category && chp_calendar) {
		sprintf(cha_reserve_url + strlen(cha_reserve_url), "&category=%s&calendar=%s", chp_category, chp_calendar);
		sprintf(cha_sql, "select c_name from %s where n_branch = %s and n_category = %s", chpa_category_table[atoi(chp_type) - 1], chp_branch, chp_category);
	} else {
		sprintf(cha_sql, "select c_name from rt_category_type where n_type = %s and n_branch = %s", chp_type, chp_branch);
	}
	dbres = Db_OpenDyna(rsvdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "予約情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_esc = Db_GetValue(dbres, 0, 0);
	chp_plan_name = Escape_HtmlString(chp_esc ? chp_esc : "");
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select c_imageloc from sy_cartinfo");
	dbres = Db_OpenDyna(rsvdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "予約情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_esc = Db_GetValue(dbres, 0, 0);
	strcpy(cha_image_path, chp_esc ? chp_esc : "");
	Db_CloseDyna(dbres);

	switch (atoi(chp_type)) {
	case CO_RSV_TYPE_HOTEL:
		sprintf(cha_sql, "select c_image,n_category_detail,c_category_detail_url,c_comment from rt_hotel_category where n_branch = %s and n_category = %s", chp_branch, chp_category);
		break;
	case CO_RSV_TYPE_SALON:
		sprintf(cha_sql, "select c_image,n_category_detail,c_category_detail_url,c_comment from rt_salon_category where n_branch = %s and n_category = %s", chp_branch, chp_category);
		break;
	case CO_RSV_TYPE_HOSPITAL:
		sprintf(cha_sql, "select c_image,n_category_detail,c_category_detail_url,c_comment from rt_hospital_category where n_branch = %s and n_category = %s", chp_branch, chp_category);
		break;
	case CO_RSV_TYPE_RESTAURANT:
		sprintf(cha_sql, "select c_image,n_category_detail,c_category_detail_url,c_comment from rt_restaurant_category where n_branch = %s and n_category = %s", chp_branch, chp_category);
		break;
	case CO_RSV_TYPE_SCHOOL:
		sprintf(cha_sql, "select c_image,n_category_detail,c_category_detail_url,c_comment from rt_school_category where n_branch = %s and n_category = %s", chp_branch, chp_category);
		break;
	case CO_RSV_TYPE_FACILITIES:
		sprintf(cha_sql, "select c_image,n_category_detail,c_category_detail_url,c_comment from rt_facilities_category where n_branch = %s and n_category = %s", chp_branch, chp_category);
		break;
	case CO_RSV_TYPE_RENTAL:
		sprintf(cha_sql, "select c_image,n_category_detail,c_category_detail_url,c_comment from rt_rental_category where n_branch = %s and n_category = %s", chp_branch, chp_category);
		break;
	default:
		Put_Nlist(nlp_out, "ERROR", "業種を特定できません。<br>");
		return 1;
	}
	dbres = Db_OpenDyna(rsvdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "予約情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_esc = Db_GetValue(dbres, 0, 0);
	if (chp_esc && chp_esc[0]) {
		strcat(cha_image_path, "/");
		strcat(cha_image_path, chp_esc);
	} else {
		cha_image_path[0] = '\0';
	}
	chp_esc = Db_GetValue(dbres, 0, 1);
	if (chp_esc && chp_esc[0]) {
		switch (atoi(chp_esc)) {
		case 1:
			sprintf(cha_plan_url, "%s?type=%s&branch=%s&category=%s&no_login=1&disp_detail=1", g_cha_purchase_cgi, chp_type, chp_branch, chp_category);
			break;
		case 2:
			chp_esc = Db_GetValue(dbres, 0, 2);
			if (chp_esc && chp_esc[0]) {
				strcpy(cha_plan_url, chp_esc);
			}
			break;
		}
	}
	chp_esc = Db_GetValue(dbres, 0, 3);
	chp_plan_detail = Escape_HtmlString(chp_esc ? chp_esc : "");
	Db_CloseDyna(dbres);
	/*カスタマイズ*/
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select count(*)");	/*0*/
	strcat(cha_sql, " from sy_function T1");
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_function_id = %d", CO_RSV_FUNCTION_HOME_BUTTON);
	sprintf(&cha_sql[strlen(cha_sql)], " and T1.b_use = %d", CO_TRUE);
	dbres = Db_OpenDyna(rsvdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "予約情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return 1;
	}
	b_func112= atoi(Db_GetValue(dbres, 0, 0));
	Db_CloseDyna(dbres);
	memset(cha_func112_url, '\0', sizeof(cha_func112_url));
	if(b_func112) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "select c_url");	/*0*/
		strcat(cha_sql, " from rm_home T1");
		dbres = Db_OpenDyna(rsvdb, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "予約情報を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
			return 1;
		}
		strcpy(cha_func112_url, Db_GetValue(dbres, 0, 0));
		Db_CloseDyna(dbres);
	}
	Db_Disconnect(rsvdb);
	Put_Nlist(nlp_out, chp_target, "<div class=\"reservebody\">\n");
	Put_Nlist(nlp_out, chp_target, "	<table class=\"reservebox\">\n");
	Put_Nlist(nlp_out, chp_target, "	<tr>\n");
	Put_Nlist(nlp_out, chp_target, "		<th colspan=\"2\">▼こちらからも予約をすることができます。</th>\n");
	Put_Nlist(nlp_out, chp_target, "	</tr>\n");
	Put_Nlist(nlp_out, chp_target, "	<tr>\n");
	if (cha_image_path[0]) {
		Put_Format_Nlist(nlp_out, chp_target, "		<td rowspan=\"2\" class=\"reserveimg\" valign=\"top\"><img src=\"%s\" border=\"0\" alt=\"プランの画像\" width=\"100\" height=\"100\"></td>\n", cha_image_path);
	} else {
		Put_Nlist(nlp_out, chp_target, "		<td rowspan=\"2\" class=\"reserveimg\" valign=\"top\"></td>\n");
	}
	chp_tmp = Conv_Br(chp_plan_name);
	if (cha_plan_url[0]) {
		Put_Format_Nlist(nlp_out, chp_target, "		<td><a href=\"%s\"><span class=\"reserve_plan\">%s</span></a><br />\n", cha_plan_url, chp_tmp);
	} else {
		Put_Format_Nlist(nlp_out, chp_target, "		<td><span class=\"reserve_plan\">%s</span><br />\n", chp_tmp);
	}
	free(chp_tmp);
	chp_tmp = Conv_Br(chp_plan_detail);
	Put_Format_Nlist(nlp_out, chp_target, "%s\n", chp_tmp);
	free(chp_tmp);
	Put_Nlist(nlp_out, chp_target, "		</td>\n");
	Put_Nlist(nlp_out, chp_target, "	</tr>\n");
	Put_Nlist(nlp_out, chp_target, "	<tr>\n");
	Put_Nlist(nlp_out, chp_target, "		<td class=\"reservebtn\">\n");
	Put_Format_Nlist(nlp_out, chp_target, "		<input type=\"button\" value=\"予約申込\" onclick=\"location.href='%s&no_login=1'\">\n", cha_reserve_url);
	if(b_func112) {
		Put_Format_Nlist(nlp_out, chp_target, "		&nbsp;&nbsp;<input type=\"button\" value=\"HOME\" onclick=\"location.href='%s'\">\n", cha_func112_url);
	}
	Put_Nlist(nlp_out, chp_target, "		</td>\n");
	Put_Nlist(nlp_out, chp_target, "	</tr>\n");
	Put_Nlist(nlp_out, chp_target, "	</table>\n");
	Put_Nlist(nlp_out, chp_target, "</div>\n");
	free(chp_plan_name);
	free(chp_plan_detail);

	return 0;
}

/*
+* ========================================================================
 * Function:	Get_Blog_Priv
 * Description:
 *	指定の管理者のブログを扱う権限を取得する。
%* ========================================================================
 * Return:
 *	-1: エラー
 *	 0: 権限なし
 *	 1: 権限あり
=* ========================================================================
 */
int Get_Blog_Priv(DBase *db, NLIST *nlp_out, int in_owner)
{
	DBase *rsvdb;
	DBRes *dbres;
	char cha_sql[1024];
	char *chp_tmp;
	int in_priv;

	// DBをオープンする
	rsvdb = Open_Reserve_Db(db, nlp_out);
	if (!rsvdb) {
		return -1;
	}
	sprintf(cha_sql, "select n_blog_auth from sy_login where n_admin = %d", in_owner);
	dbres = Db_OpenDyna(rsvdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログの権限を得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return -1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		Db_Disconnect(rsvdb);
		return 0;
	}
	in_priv = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	Db_Disconnect(rsvdb);

	return in_priv;
}

/*
+* ========================================================================
 * Function:	Get_Sample_Mode
 * Description:
 *	サンプルサイトとして運用されているか。
%* ========================================================================
 * Return:
 *	-1: エラー
 *	 0: 通常モード
 *	 1: サンプルモード
=* ========================================================================
 */
int Get_Sample_Mode(DBase *db, NLIST *nlp_out)
{
	DBase *rsvdb;
	DBRes *dbres;
	char cha_sql[1024];
	char *chp_tmp;
	int in_priv;

	// DBをオープンする
	rsvdb = Open_Reserve_Db(db, nlp_out);
	if (!rsvdb) {
		return -1;
	}
	sprintf(cha_sql, "select b_use from sy_function where n_function_id = 1");
	dbres = Db_OpenDyna(rsvdb, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "サンプルモードを得るクエリに失敗しました。<br>%s<br>%s", Gcha_last_error, cha_sql);
		return -1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		Db_Disconnect(rsvdb);
		return 0;
	}
	in_priv = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	Db_Disconnect(rsvdb);

	return in_priv;
}
