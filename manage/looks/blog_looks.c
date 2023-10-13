/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_looks.c
 * First-Created:	2004/08/ 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	デザインの設定
 *
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_looks.c,v 1.98 2008/02/15 09:26:05 hori Exp $";

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

/*
+* ------------------------------------------------------------------------
 * Function:            put_error_data()
 * Description:
 *      エラー時の埋め込み
%* ------------------------------------------------------------------------
 * Return:
 *      (無し)
-* ------------------------------------------------------------------------*/
void put_error_data(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chpa_esc[] = {
		"BTN_UPDATE_LOOKS.x", "BTN_UPDATE_LOOKS.y", "preview", "blogid", "category", "design", "select_skin", NULL
	};

	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	Put_Nlist(nlp_out, "ACTION", CO_CGI_LOOKS);
	Put_Nlist(nlp_out, "BUTTON_NAME", "BTN_DISP_SETTING_LOOKS");
	Build_HiddenEncode(nlp_out, "HIDDEN", "from_error", "1");
	return ;
}

/*
+* ------------------------------------------------------------------------
 * Function:            disp_preview()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *      正常終了 0
 *      エラー時 1
-* ------------------------------------------------------------------------*/
int disp_preview(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_skel)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char cha_sql[512];

	chp_tmp = Get_Nlist(nlp_in, "preview", 1);
	chp_tmp2 = Get_Nlist(nlp_in, "category", 1);
	if (Check_Numeric(chp_tmp) || Check_Numeric(chp_tmp2)) {
		Put_Nlist(nlp_out, "ERROR", "値の指定に間違いがあります。");
		return 1;
	}
	sprintf(chp_skel, "%s_01.skl", CO_SKEL_LOOKS_PREVIEW);

	/* テンプレートナンバーが0のときはユーザー指定 */
	/* それ以外のときは、用意されたCSSへのパスを改めてコピー */
	strcpy(cha_sql, " select T1.c_theme_file");	/* 0 用意されたテーマファイル */
	strcat(cha_sql, ",T1.c_theme_prev_entry_icon");
	strcat(cha_sql, ",T1.c_theme_next_entry_icon");
	strcat(cha_sql, ",T1.c_theme_prev_page_icon");
	strcat(cha_sql, ",T1.c_theme_next_page_icon");
	strcat(cha_sql, ",T1.c_theme_first_icon");
	strcat(cha_sql, ",T1.c_theme_last_icon");
	strcat(cha_sql, ",T1.c_theme_plus_icon");
	strcat(cha_sql, ",T1.c_theme_minus_icon");
	strcat(cha_sql, ",T1.c_theme_page_top_icon");
	strcat(cha_sql, ",T1.c_theme_blog_top_icon");
	strcat(cha_sql, " from sy_theme T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_theme_id = %s and T1.n_category_id = %s", chp_tmp, chp_tmp2);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "用意されたCSSファイル名取得に失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	if (atoi(chp_tmp)) {
		Put_Format_Nlist(nlp_out, "PATHCSS", "%s%s%s/%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_css_location, Db_GetValue(dbres, 0, 0));
	}
	strcpy(gcha_theme_prev_entry_icon, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "news_back_01.gif");
	strcpy(gcha_theme_next_entry_icon, Db_GetValue(dbres, 0, 2) ? Db_GetValue(dbres, 0, 2) : "news_next_01.gif");
	strcpy(gcha_theme_prev_page_icon, Db_GetValue(dbres, 0, 3) ? Db_GetValue(dbres, 0, 3) : "news_backpage_01.gif");
	strcpy(gcha_theme_next_page_icon, Db_GetValue(dbres, 0, 4) ? Db_GetValue(dbres, 0, 4) : "news_nextpage_01.gif");
	strcpy(gcha_theme_first_icon, Db_GetValue(dbres, 0, 5) ? Db_GetValue(dbres, 0, 5) : "news_back_01_01.gif");
	strcpy(gcha_theme_last_icon, Db_GetValue(dbres, 0, 6) ? Db_GetValue(dbres, 0, 6) : "news_next_01_01.gif");
	strcpy(gcha_theme_plus_icon, Db_GetValue(dbres, 0, 7) ? Db_GetValue(dbres, 0, 7) : "plus_01.gif");
	strcpy(gcha_theme_minus_icon, Db_GetValue(dbres, 0, 8) ? Db_GetValue(dbres, 0, 8) : "minus_01.gif");
	strcpy(gcha_theme_back_top_icon, Db_GetValue(dbres, 0, 9) ? Db_GetValue(dbres, 0, 9) : "back_top.gif");
	strcpy(gcha_theme_back_blogtop_icon, Db_GetValue(dbres, 0, 10) ? Db_GetValue(dbres, 0, 10) : "back_blogtop.gif");
	Db_CloseDyna(dbres);
	Put_Nlist(nlp_out, "PLUS", gcha_theme_plus_icon);
	Put_Nlist(nlp_out, "MINUS", gcha_theme_minus_icon);
	Put_Nlist(nlp_out, "BACK_TOP", gcha_theme_back_top_icon);
	Put_Nlist(nlp_out, "BACK_BLOGTOP", gcha_theme_back_blogtop_icon);

	return Set_My_Avatar(db, nlp_out, "AVATAR", in_blog);
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_looks()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int disp_page_looks(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char *chp_usercss;
	char *chp_fromuser;
	char cha_sql[512];
	char cha_str[2048];
	char cha_target[30];
	int in_skin_no = 1;
	int in_category_no = 1;
	int in_checked = 0;
	int in_chkcate = 0;
	int in_page;
	int in_total_page = 1;
	int in_total_theme;
	int in_row;
	int i;

	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	chp_tmp = Get_Nlist(nlp_in, "category", 1);
	if(chp_tmp) {
		in_category_no = atoi(chp_tmp);
	}
	if (Get_Nlist(nlp_in, "from_error", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "disign", 1);
		if(chp_tmp && *chp_tmp) {
			in_checked = atoi(chp_tmp);
			if(in_checked == 0) {
				Put_Nlist(nlp_out, "SELECTED_SKIN000", CO_CHECKED);
			}
		} else {
			in_checked = 0;
		}
		in_chkcate = in_category_no;
		chp_tmp = Get_Nlist(nlp_in, "sidebar", 1);
		if(chp_tmp) {
			sprintf(cha_target, "SELECTED_SIDEBAR%s", chp_tmp);
			Put_Nlist(nlp_out, cha_target, CO_CHECKED);
		}
		chp_usercss = Get_Nlist(nlp_in, "css_path", 1);
	} else {
		strcpy(cha_sql, " select T1.n_theme_id");	/* 0 スキンナンバー */
		strcat(cha_sql, ",T1.n_sidebar");			/* 1 サイドバー位置 */
		strcat(cha_sql, ",T1.c_user_css");			/* 2 ユーザーCSS */
		strcat(cha_sql, ",T1.n_category_id");		/* 3 カテゴリID */
		strcat(cha_sql, " from at_looks T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		chp_escape = Db_GetValue(dbres, 0, 3);
		if(chp_tmp && chp_escape) {
			in_checked = atoi(chp_tmp);
			in_chkcate = atoi(chp_escape);
			if(!in_checked && !in_chkcate) {
				Put_Nlist(nlp_out, "SELECTED_SKIN000", CO_CHECKED);
			}
		}
		chp_tmp = Db_GetValue(dbres, 0, 1);
		if(chp_tmp) {
			sprintf(cha_target, "SELECTED_SIDEBAR%s", chp_tmp);
			Put_Nlist(nlp_out, cha_target, CO_CHECKED);
		}
		chp_usercss = Db_GetValue(dbres, 0, 2);
		Db_CloseDyna(dbres);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "select_skin", "1");
	Put_Format_Nlist(nlp_out, "HIDDEN", "<input type=\"hidden\" name=\"category\" value=\"%d\">\n", in_chkcate);
	Put_Format_Nlist(nlp_out, "HIDDEN", "<input type=\"hidden\" name=\"design\" value=\"%d\">\n", in_checked);
	Put_Format_Nlist(nlp_out, "PREVIEW", "%s%s%s/%s?preview=%d&category=%d&blogid=%d%s",
		 g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_checked, in_chkcate, in_blog,
		(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
	Put_Format_Nlist(nlp_out, "SELECT", "%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&blogid=%d%s",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_blog,
		(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");

	if ((!in_chkcate || !in_checked) && chp_usercss) {
		Put_Nlist(nlp_out, "SELECTED_SKIN000", "checked");
		Put_Nlist(nlp_out, "CURSKIN_START", "<!--");
		Put_Nlist(nlp_out, "CURSKIN_END", "-->");
	} else {
		sprintf(cha_sql, "select c_theme_thumbnail,c_theme_title from sy_theme where n_category_id=%d and n_theme_id=%d", in_chkcate, in_checked);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (!chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "選択中のブログデザインの取得に失敗しました。");
			return 1;
		}
		Put_Format_Nlist(nlp_out, "CURRENT", "%s/%s", g_cha_theme_image, chp_tmp);
		chp_tmp = Db_GetValue(dbres, 0, 1);
		if (!chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "選択中のブログデザインの取得に失敗しました。");
			return 1;
		}
		Put_Nlist(nlp_out, "TITLE", chp_tmp);
		Db_CloseDyna(dbres);

		sprintf(cha_sql, "select c_category_title from sy_theme_category where n_category_id=%d", in_category_no);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (!chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "選択中のブログデザインの取得に失敗しました。");
			return 1;
		}
		Put_Nlist(nlp_out, "CATEGORY", chp_tmp);
		Db_CloseDyna(dbres);
	}

	chp_tmp = Get_Nlist(nlp_in, "page", 1);
	if(chp_tmp && *chp_tmp) {
		if(Check_Numeric(chp_tmp)) {
			Put_Nlist(nlp_out, "ERROR", "そのページ指定は無効です。<br>");
			return 1;
		}
		in_page = atoi(chp_tmp);
	} else {
		if(in_checked == 0) {
			in_page = 0;
		} else {
			if(in_checked % CO_PERPAGE_THEME == 0) {
				in_page = in_checked / CO_PERPAGE_THEME - 1;	/* 1から始まるため。*/
			} else {
				in_page = in_checked / CO_PERPAGE_THEME;
			}
		}
	}
	/* テーマのページめくりリンク */
	sprintf(cha_sql, "select count(*) from sy_theme where n_theme_id > 0 and n_category_id=%d", in_category_no);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "テーマが登録されていません。");
		return 1;
	}
	in_total_theme = atoi(chp_tmp);
	in_total_page = (in_total_theme + CO_PERPAGE_THEME - 1) / CO_PERPAGE_THEME;
	if(in_page  >= in_total_page) {
		Put_Nlist(nlp_out, "ERROR", "そのページ指定は無効です。<br>");
		return 1;
	}
	if(in_total_page > 1) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "PAGEUP",
				"		<tr>\n"
				"			<td colspan=\"4\">\n"
				"				<table align=\"center\"><tbody>\n"
				"				<tr>\n"
				"					<td>\n");
			if (in_page > 0) {
				Put_Format_Nlist(nlp_out, "PAGEUP", "						<a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&page=%d&category=%d&blogid=%d%s\">\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_page - 1, in_category_no, in_blog,
					(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
			}
			Put_Nlist(nlp_out, "PAGEUP","							<span style=\"font-size: 12px;\">&lt;&lt; 前へ&nbsp;</span>\n");
			if (in_page > 0) {
				Put_Nlist(nlp_out, "PAGEUP","						</a>\n");
			}
			Put_Nlist(nlp_out, "PAGEUP","					</td>\n");
			for(i = 0; i < in_total_page; i++) {
				if (in_page != i) {
					Put_Format_Nlist(nlp_out, "PAGEUP", "					<td><a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&page=%d&category=%d&blogid=%d%s\">[%d] </a></td>\n",
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, i, in_category_no, in_blog,
						(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "", i + 1);
				} else {
					Put_Format_Nlist(nlp_out, "PAGEUP", "					<td>[%d] </td>\n", i + 1);
				}
			}
			Put_Format_Nlist(nlp_out, "PAGEUP", "					<td>\n");
			if (in_page <  in_total_page - 1) {
				Put_Format_Nlist(nlp_out, "PAGEUP", "						<a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&page=%d&category=%d&blogid=%d%s\">\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_page + 1, in_category_no, in_blog,
					(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
			}
			Put_Format_Nlist(nlp_out, "PAGEUP", "						<span style=\"font-size: 12px;\">&nbsp;次へ &gt;&gt;</span>\n");
			if (in_page <  in_total_page - 1) {
				Put_Nlist(nlp_out, "PAGEUP", "						</a>\n");
			}
			Put_Nlist(nlp_out, "PAGEUP",
				"					</td>\n"
				"				</tr>\n"
				"				</table>\n"
				"			</td>\n"
				"		</tr>\n");
		} else {
			Put_Nlist(nlp_out, "THEME", "<table><tr>");
			for(i = 0; i < in_total_page; i++) {
				if(i != 0 && (i % CO_RETURN_THEME) == 0) {	/* CO_RETURN_THEMEずつで折り返す */
					Put_Nlist(nlp_out, "THEME", "</tr>\n<tr>");
				}
				Put_Nlist(nlp_out, "THEME", "<td>");
				if (in_page != i) {
					Put_Format_Nlist(nlp_out, "THEME",
						"<a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&page=%d&category=%d&blogid=%d%s\">page %d </a>",
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, i, in_category_no, in_blog,
						(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "", i + 1);
				} else {
					Put_Format_Nlist(nlp_out, "THEME", "page %d ", i + 1);
				}
				Put_Nlist(nlp_out, "THEME", "</td>\n");
			}
			Put_Nlist(nlp_out, "THEME", "</tr>\n</table>\n");
		}
	}
	/* ページめくりリンクここまで */
	/*テーマの画像とラジオボタン*/
	strcpy(cha_sql, "select T1.n_theme_id");	/* 0 テーマID */
	strcat(cha_sql, ", T1.c_theme_title");		/* 1 テーマタイトル */
	strcat(cha_sql, ", T1.c_theme_thumbnail");	/* 2 サムネイル */
	strcat(cha_sql, " from sy_theme T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_category_id = %d", in_category_no);
	strcat(cha_sql, " and T1.n_theme_id > 0");
	strcat(cha_sql, " order by T1.n_theme_id");
	sprintf(cha_sql + strlen(cha_sql), " limit %d, %d", in_page * CO_PERPAGE_THEME, CO_PERPAGE_THEME);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "THEME", "<tr bgcolor=\"#FFFFFF\">\n");
	for (i = 0; i < in_row; i++) {
		in_skin_no = atoi(Db_GetValue(dbres, i, 0));
		if(i && (i % CO_RETURN_THEME) == 0){
			Put_Nlist(nlp_out, "THEME", "</tr>\n<tr bgcolor=\"#FFFFFF\">\n");
		}
		chp_tmp = Escape_HtmlString(Db_GetValue(dbres, i, 1));
		Put_Nlist(nlp_out, "THEME", "<td nowrap align=\"center\" width=\"\">\n<br>\n");
		Put_Format_Nlist(nlp_out, "THEME", "<a href=\"%s%s%s/%s?category=%d&preview=%d&blogid=%d\" target=\"_blank\">",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_category_no, in_skin_no, in_blog);
		Put_Format_Nlist(nlp_out, "THEME", "<img src=\"%s/%s\" border=\"0\" alt=\"%s\">", g_cha_theme_image, Db_GetValue(dbres, i, 2), chp_tmp);
		Put_Nlist(nlp_out, "THEME", "</a><br>\n");
		sprintf(cha_str, "%d:%d", in_category_no, in_skin_no);
		if (in_checked == in_skin_no && in_chkcate == in_category_no) {
			Build_Radio_With_Id(nlp_out, "THEME", "skin", cha_str, 1, chp_tmp, chp_tmp);
		} else {
			Build_Radio_With_Id(nlp_out, "THEME", "skin", cha_str, 0, chp_tmp, chp_tmp);
		}
		Put_Nlist(nlp_out, "THEME", "</td>\n");
	}
	if ((in_row % CO_RETURN_THEME) != 0){
		memset(cha_str, '\0', sizeof(cha_str));
		for (i = 0; i < CO_RETURN_THEME - (in_row % CO_RETURN_THEME); i++){
			Put_Nlist(nlp_out, "THEME", "<td nowrap align=\"center\" width=\"\"><br></td>\n");
		}
	}
	Put_Nlist(nlp_out, "THEME", "</tr>\n");	/* </table>はスケルトン内 */
	if(in_total_page > 1 && g_in_dbb_mode) {
		Put_Nlist(nlp_out, "PAGEDN",
			"		<tr>\n"
			"			<td colspan=\"4\" height=\"50px\" valign=\"bottom\">\n"
			"				<table align=\"center\"><tbody>\n"
			"				<tr>\n"
			"					<td>\n");
		if (in_page > 0) {
			Put_Format_Nlist(nlp_out, "PAGEDN", "						<a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&page=%d&category=%d&blogid=%d%s\">\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_page - 1, in_category_no, in_blog,
				(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
		}
		Put_Nlist(nlp_out, "PAGEDN","							<span style=\"font-size: 12px;\">&lt;&lt; 前へ&nbsp;</span>\n");
		if (in_page > 0) {
			Put_Nlist(nlp_out, "PAGEDN","						</a>\n");
		}
		Put_Nlist(nlp_out, "PAGEDN","					</td>\n");
		for(i = 0; i < in_total_page; i++) {
			if (in_page != i) {
				Put_Format_Nlist(nlp_out, "PAGEDN", "					<td><a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&page=%d&category=%d&blogid=%d%s\">[%d] </a></td>\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, i, in_category_no, in_blog,
					(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "", i + 1);
			} else {
				Put_Format_Nlist(nlp_out, "PAGEDN", "					<td>[%d] </td>\n", i + 1);
			}
		}
		Put_Format_Nlist(nlp_out, "PAGEDN", "					<td>\n");
		if (in_page <  in_total_page - 1) {
			Put_Format_Nlist(nlp_out, "PAGEDN", "						<a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&page=%d&category=%d&blogid=%d%s\">\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_page + 1, in_category_no, in_blog,
				(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
		}
		Put_Format_Nlist(nlp_out, "PAGEDN", "						<span style=\"font-size: 12px;\">&nbsp;次へ &gt;&gt;</span>\n");
		if (in_page <  in_total_page - 1) {
			Put_Nlist(nlp_out, "PAGEDN", "						</a>\n");
		}
		Put_Nlist(nlp_out, "PAGEDN",
			"					</td>\n"
			"				</tr>\n"
			"				</table>\n"
			"			</td>\n"
			"		</tr>\n");
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_looks()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int disp_page_looks_category(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_usercss;
	char *chp_fromuser;
	char cha_sql[512];
	char cha_str[2048];
	char cha_target[30];
	int in_skin_no = 1;
	int in_category_no = 1;
	int in_row;
	int i;

	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	chp_usercss = NULL;
	if(Get_Nlist(nlp_in, "from_error", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "sidebar", 1);
		if(chp_tmp) {
			sprintf(cha_target, "SELECTED_SIDEBAR%s", chp_tmp);
			Put_Nlist(nlp_out, cha_target, CO_CHECKED);
		}
		chp_usercss = Get_Nlist(nlp_in, "css_path", 1);
		chp_tmp = Get_Nlist(nlp_in, "category", 1);
		if(chp_tmp) {
			in_category_no = atoi(chp_tmp);
		}
		chp_tmp = Get_Nlist(nlp_in, "design", 1);
		if(chp_tmp) {
			in_skin_no = atoi(chp_tmp);
		}
	} else {
		strcpy(cha_sql, " select ");
		strcat(cha_sql, " T1.n_sidebar");			/* 0 サイドバー位置 */
		strcat(cha_sql, ",T1.c_user_css");			/* 1 ユーザーCSS */
		strcat(cha_sql, ",T1.n_category_id");		/* 2 テーマカテゴリ */
		strcat(cha_sql, ",T1.n_theme_id");			/* 3 テーマ番号 */
		strcat(cha_sql, " from at_looks T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(chp_tmp) {
			sprintf(cha_target, "SELECTED_SIDEBAR%s", chp_tmp);
			Put_Nlist(nlp_out, cha_target, CO_CHECKED);
		}
		chp_usercss = Db_GetValue(dbres, 0, 1);
		chp_tmp = Db_GetValue(dbres, 0, 2);
		if(chp_tmp) {
			in_category_no = atoi(chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, 0, 3);
		if(chp_tmp) {
			in_skin_no = atoi(chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	Put_Format_Nlist(nlp_out, "HIDDEN", "<input type=\"hidden\" name=\"category\" value=\"%d\">\n", in_category_no);
	Put_Format_Nlist(nlp_out, "HIDDEN", "<input type=\"hidden\" name=\"design\" value=\"%d\">\n", in_skin_no);
	Put_Format_Nlist(nlp_out, "PREVIEW", "%s%s%s/%s?preview=%d&category=%d&blogid=%d",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_skin_no, in_category_no, in_blog);
	Put_Format_Nlist(nlp_out, "SEARCH", "%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&category=%d&blogid=%d%s",
		g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_category_no, in_blog,
		(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
	if ((!in_category_no || !in_skin_no) && chp_usercss) {
		Put_Nlist(nlp_out, "SELECTED_SKIN000", "checked");
		Put_Nlist(nlp_out, "CURSKIN_START", "<!--");
		Put_Nlist(nlp_out, "CURSKIN_END", "-->");
	} else {
		sprintf(cha_sql, "select c_theme_thumbnail,c_theme_title from sy_theme where n_category_id=%d and n_theme_id=%d", in_category_no, in_skin_no);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (!chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "選択中のブログデザインの取得に失敗しました。");
			return 1;
		}
		Put_Format_Nlist(nlp_out, "THEME", "%s/%s", g_cha_theme_image, chp_tmp);
		chp_tmp = Db_GetValue(dbres, 0, 1);
		if (!chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "選択中のブログデザインの取得に失敗しました。");
			return 1;
		}
		Put_Nlist(nlp_out, "TITLE", chp_tmp);
		Db_CloseDyna(dbres);
	}
	/* ページめくりリンクここまで */
	/*テーマの画像とラジオボタン*/
	strcpy(cha_sql, "select T1.n_category_id");		/* 0 テーマID */
	strcat(cha_sql, ",T1.c_category_title");		/* 1 テーマタイトル */
	strcat(cha_sql, ",T1.c_theme_thumbnail");		/* 2 サムネイル */
	strcat(cha_sql, ",count(T2.n_theme_id)");		/* 3 テーマ個数 */
	strcat(cha_sql, " from sy_theme_category T1");
	strcat(cha_sql, ",sy_theme T2");
	strcat(cha_sql, " where T1.n_category_id = T2.n_category_id");
	strcat(cha_sql, " group by T1.n_category_id");
	strcat(cha_sql, " order by T1.n_category_id");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "CATEGORY", "<tr>\n");
	for (i = 0; i < in_row; i++) {
		in_skin_no = atoi(Db_GetValue(dbres, i, 0));
		if(i && (i % CO_RETURN_CATEGORY) == 0){
			Put_Nlist(nlp_out, "CATEGORY", "</tr>\n<tr>\n");
		}
		chp_tmp = Escape_HtmlString(Db_GetValue(dbres, i, 1));
		Put_Format_Nlist(nlp_out, "CATEGORY", "<td width=\"25%%\" align=\"center\" style=\"border:solid 1px #B6D6FF;\">%s<br>", chp_tmp);
		Put_Format_Nlist(nlp_out, "CATEGORY", "<a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&category=%d&blogid=%d%s\">",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_skin_no, in_blog,
			(chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
		Put_Format_Nlist(nlp_out, "CATEGORY", "<img src=\"%s/%s\" border=\"0\" alt=\"%s\"><br>%s種類</a></td>\n",
			g_cha_theme_image, Db_GetValue(dbres, i, 2), chp_tmp, Db_GetValue(dbres, i, 3));
		free(chp_tmp);
	}
	if ((in_row % CO_RETURN_CATEGORY) != 0){
		memset(cha_str, '\0', sizeof(cha_str));
		for (i = 0; i < (in_row % CO_RETURN_CATEGORY) - 1; i++){
			Put_Nlist(nlp_out, "CATEGORY", "<td nowrap align=\"center\" width=\"\">\n&nbsp;</td>\n");
		}
	}
	Put_Nlist(nlp_out, "CATEGORY", "</tr>\n");	/* </table>はスケルトン内 */
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	copy_file()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int copy_file(const char* pszSrc, const char* pszDst)
{
	FILE* sfp;
	FILE* dfp;
	unsigned int t;
	char buff[2048];

	sfp = fopen(pszSrc, "rb");
	if (!sfp)
		return 1;
	dfp = fopen(pszDst, "wb");
	if (!dfp) {
		fclose(dfp);
		return 1;
	}

	do {
		t = fread(buff, 1, sizeof(buff), sfp);
		fwrite(buff, 1, t, dfp);
	} while ( t >= sizeof(buff) );

	fclose(dfp);
	fclose(sfp);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_looks()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int update_looks(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_skin;
	char *chp_cate;
	char *chp_sidebar;
	char *chp_user_css;
	char *chp_select_skin;
	char cha_user_css[1024];
	char cha_old_css[1024];
	char cha_skin[512];
	char cha_sql[2048];
	int in_error;
	int in_skip;

	chp_select_skin = Get_Nlist(nlp_in, "select_skin", 1);

	cha_old_css[0] = '\0';
	strcpy(cha_sql, " select T1.c_user_css");			/* 2 ユーザーCSS */
	strcat(cha_sql, " from at_looks T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp && chp_tmp[0]) {
		strcpy(cha_sql, chp_tmp);
		chp_tmp = strrchr(cha_sql, '/');
		if (chp_tmp) {
			++chp_tmp;
		} else {
			chp_tmp = cha_sql;
		}
		strcpy(cha_old_css, g_cha_css_path);
		strcat(cha_old_css, "/");
		strcat(cha_old_css, chp_tmp);
	}
	Db_CloseDyna(dbres);

	in_error = 0;
	chp_cate = NULL;
	chp_skin = NULL;
	chp_tmp = Get_Nlist(nlp_in, "skin", 1);
	if (chp_tmp && *chp_tmp) {
		strcpy(cha_skin, chp_tmp);
		chp_cate = strtok(cha_skin, ":");
		chp_skin = strtok(NULL, ":");
		if (!chp_skin) {
			Put_Nlist(nlp_out, "ERROR", "選択されたスキンのIDが間違っています<br>");
			in_error++;
		}
	} else if (chp_select_skin) {
		Put_Nlist(nlp_out, "ERROR", "スキンを選択してください。<br>");
		in_error++;
	}
	chp_sidebar = Get_Nlist(nlp_in, "sidebar", 1);
	if(!chp_sidebar || !*chp_sidebar) {
		Put_Nlist(nlp_out, "ERROR", "サイドバーの位置を選択してください。<br>");
		in_error++;
	}
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	chp_user_css = NULL;
	if (chp_skin && atoi(chp_skin) == 0) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "スキンを選択してください。<br>");
			in_error++;
		} else {
			chp_user_css = Get_Nlist(nlp_in, "css_path", 1);
			if (chp_user_css && *chp_user_css) {
				if(Check_Space_Only(chp_user_css)) {
					Put_Nlist(nlp_out, "ERROR", "CSSのパスを入力してください。<br>");
					in_error++;
				}
				if (!in_skip) {
					chp_tmp = strrchr(chp_user_css, '/');
					if (chp_tmp) {
						++chp_tmp;
					} else {
						chp_tmp = chp_user_css;
					}
					strcpy(cha_user_css, g_cha_css_path);
					strcat(cha_user_css, "/");
					strcat(cha_user_css, chp_tmp);
					strcat(cha_user_css, ".css");
					if (copy_file(chp_user_css, cha_user_css)) {
						Put_Nlist(nlp_out, "ERROR", "CSSのアップロードに失敗しました。<br>");
						in_error++;
					} else {
						strcpy(cha_user_css, chp_tmp);
						strcat(cha_user_css, ".css");
						chp_user_css = cha_user_css;
					}
					remove(chp_user_css);
				}
			} else if (!cha_old_css[0]) {
				Put_Nlist(nlp_out, "ERROR", "CSSのパスを入力してください。<br>");
				in_error++;
			}
		}
	}
	if(in_error) {
		return 1;
	}
	if (!in_skip) {
		strcpy(cha_sql, "update at_looks set");
		if (chp_select_skin || (chp_user_css && *chp_user_css)) {
			strcat(cha_sql, " n_category_id = ");
			strcat(cha_sql, chp_cate);
			strcat(cha_sql, ", n_theme_id = ");
			strcat(cha_sql, chp_skin);
			strcat(cha_sql, ", n_sidebar = ");
		} else {
			strcat(cha_sql, " n_sidebar = ");
		}
		strcat(cha_sql, chp_sidebar);
		if(chp_user_css && *chp_user_css) {
			strcat(cha_sql, ", c_user_css = '");
			strcat(cha_sql, chp_user_css);
			strcat(cha_sql, "'");
		} else if (chp_skin && atoi(chp_skin) != 0) {
			strcat(cha_sql, ", c_user_css = null");
		}
		sprintf(cha_sql + strlen(cha_sql), " where n_blog_id = %d", in_blog);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "登録するクエリーに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
	}
	if(chp_user_css && *chp_user_css && cha_old_css[0]) {
		remove(cha_old_css);
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	main()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
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
	char cha_skel[64];
	char cha_blog[32];
	char *chp_fromuser;
	char *chp_skel;
	char *chp_tmp;
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
	/* 画像ファイルの保存場所 */
	Set_SavePath(g_cha_upload_path);
	Set_SkelPath(g_cha_admin_skeleton);
	/* ファイルサイズ制限 kb単位 */
	Set_MaxFileSize(CO_MAX_UPLOAD_SIZE * 1024);

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	Put_Nlist(nlp_out, "CSS", g_cha_css_location);
	Put_Nlist(nlp_out, "SCRIPTS", g_cha_script_location);
	Put_Nlist(nlp_out, "IMAGES", g_cha_admin_image);
	Put_Nlist(nlp_out, "USRIMG", g_cha_user_image);

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
			//	Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
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
					Put_Nlist(nlp_out, "ACTION", CO_CGI_LOOKS);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "認証情報を取得できません。<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_LOOKS);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_LOOKS);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_LOOKS);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_LOOKS);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_LOOKS);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを操作する権限がありません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_LOOKS);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを作成されていない方はご利用になれません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);

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
	if (Get_Nlist(nlp_in, "BTN_DISP_SETTING_LOOKS", 1)) {
		if (Get_Nlist(nlp_in, "category", 1)){
			if (disp_page_looks(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
			chp_skel = CO_SKEL_LOOKS;
		} else {
			if (disp_page_looks_category(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
			chp_skel = CO_SKEL_LOOKS_CATEGORY;
		}
	} else if(Get_Nlist(nlp_in, "BTN_UPDATE_LOOKS.x", 1)) {
		if (update_looks(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if (disp_page_looks(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
			chp_skel = CO_SKEL_LOOKS;
		}
	} else if(Get_Nlist(nlp_in, "preview", 1)) {
		if (disp_preview(db, nlp_in, nlp_out, in_blog, cha_skel)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if (disp_page_looks(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
			chp_skel = CO_SKEL_LOOKS;
		}
		chp_skel = cha_skel;
	} else {
		Put_Nlist(nlp_out, "ERROR", "直接CGIを実行できません。<br>");
		in_error++;
	}
	if(in_error) {
		put_error_data(nlp_in, nlp_out);
		Page_Out(nlp_out, CO_SKEL_ERROR);
	} else {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "DIRECT_START", "<!--");
			Put_Nlist(nlp_out, "DIRECT_END", "-->");
		}
		if(chp_skel && *chp_skel) {
			Page_Out(nlp_out, chp_skel);
		} else {
			char cha_jump[1024];
			if (chp_fromuser) {
				if (g_in_dbb_mode) {
					sprintf(cha_jump, "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
				} else if (g_in_short_name) {
					sprintf(cha_jump, "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
				} else {
					sprintf(cha_jump, "%s%s%s/%s?bid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
				}
			} else {
				sprintf(cha_jump, "%s%s%s/%s?blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
			}
			Put_Nlist(nlp_out, "MESSAGE", "ブログのデザインを変更しました。");
			Put_Nlist(nlp_out, "RETURN", cha_jump);
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "TITLE", "DBB_blog 管理者メニュー");
			} else if (g_in_cart_mode == CO_CART_SHOPPER) {
				Put_Nlist(nlp_out, "TITLE", "店長ブログ管理メニュー");
			} else if (g_in_cart_mode == CO_CART_RESERVE) {
				Put_Nlist(nlp_out, "TITLE", "eリザーブブログ管理メニュー");
			} else {
				Put_Nlist(nlp_out, "TITLE", "AS-BLOG 管理者メニュー");
			}
			Page_Out(nlp_out, CO_SKEL_CONFIRM);
		}
	}

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
