/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_menu.c
 * First-Created:	2004/08/17 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	Blogシステム管理側トップメニュー
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_ranking.c,v 1.31 2008/01/24 05:07:57 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include "libcgi2.h"
#include "libmysql.h"
#include "libcommon2.h"
#include "libauth.h"
#include "libblog.h"
#include "libblogcart.h"
#include "libblogreserve.h"

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
	DBase *db;
	DBRes *dbres;
	NLIST *nlp_in;
	NLIST *nlp_out;
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char cha_sql[1024];
	char cha_nickname[256];
	char *chp_esc;
	char *chp_tmp;
	int i;
	int in_error;
	int in_blog;
	int in_owner;
	int in_start;
	int in_tbstart;

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

	chp_tmp = Get_Nlist(nlp_in, "page1", 1);
	Put_Nlist(nlp_out, "PAGE1", chp_tmp ? chp_tmp : "");
	chp_tmp = Get_Nlist(nlp_in, "page2", 1);
	Put_Nlist(nlp_out, "PAGE2", chp_tmp ? chp_tmp : "none");

	if (g_in_hb_mode) {
		Delete_Old_Entry(db);
	}

	in_error = 0;
	if (g_in_hosting_mode) {
		in_blog = Check_Remote_User(db, nlp_out);
	} else {
		in_blog = Check_Login(db, nlp_out, &in_owner, &g_in_admin_mode);
	}
	if (in_blog < 0) {
		Put_Nlist(nlp_out, "ERR_START", "-->");
		Put_Nlist(nlp_out, "ERR_END", "<!--");
		Page_Out(nlp_out, CO_SKEL_LOGIN);
		goto clear_finish;
	}
	if (!in_blog) {
		if (Get_Nlist(nlp_in, CO_DISP_REMIND, 1)) {
			Disp_Remind_Page(db, nlp_in, nlp_out, CO_CGI_MENU);
			Page_Out(nlp_out, CO_SKEL_REMIND);
			goto clear_finish;
		} else if (Get_Nlist(nlp_in, CO_SEND_REMIND, 1)) {
			if (Auth_Reminder(db, nlp_in, nlp_out, CO_CGI_MENU)) {
				Put_Nlist(nlp_out, "ERR_START", "-->");
				Put_Nlist(nlp_out, "ERR_END", "<!--");
				Disp_Remind_Page(db, nlp_in, nlp_out, CO_CGI_MENU);
				Page_Out(nlp_out, CO_SKEL_REMIND);
				goto clear_finish;
			} else {
				Disp_Remind_Finish(nlp_in, nlp_out, CO_CGI_MENU);
				Page_Out(nlp_out, CO_SKEL_REMIND_FINISH);
				goto clear_finish;
			}
		} else if (Get_Nlist(nlp_in, CO_LOGIN_ID, 1) && Get_Nlist(nlp_in, CO_PASSWORD, 1)) {
			in_blog = Auth_Login(db, nlp_in, nlp_out, CO_LOGIN_ID, CO_PASSWORD, &in_owner);
			if (in_blog < 0) {
				Put_Nlist(nlp_out, "ERR_START", "-->");
				Put_Nlist(nlp_out, "ERR_END", "<!--");
				Page_Out(nlp_out, CO_SKEL_LOGIN);
				in_error = 1;
				goto clear_finish;
			} else if (!in_blog) {
				Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
				Put_Nlist(nlp_out, "ERROR", "ログインIDまたはパスワードが間違っています。<br>");
				Put_Nlist(nlp_out, "ERR_START", "-->");
				Put_Nlist(nlp_out, "ERR_END", "<!--");
				Page_Out(nlp_out, CO_SKEL_LOGIN);
				goto clear_finish;
			}
		} else {
			if (g_in_dbb_mode) {
				in_error = Jump_To_Login_Cgi(db, nlp_out);
				if (in_error < 0) {
					Disp_Login_Page(nlp_in, nlp_out, CO_CGI_MENU);
					Page_Out(nlp_out, CO_SKEL_LOGIN);
					goto clear_finish;
				} else if (in_error) {
					Put_Nlist(nlp_out, "ACTION", CO_CGI_BLOG_LIST);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else if (g_in_hosting_mode) {
				in_blog = Check_Unix_User(db, nlp_out);
				if (in_blog < 0) {
					Put_Nlist(nlp_out, "ERR_START", "-->");
					Put_Nlist(nlp_out, "ERR_END", "<!--");
					Page_Out(nlp_out, CO_SKEL_LOGIN);
					goto clear_finish;
				} else if (!in_blog) {
					Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "認証情報を取得できません。<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_MENU);
				Page_Out(nlp_out, CO_SKEL_LOGIN);
				goto clear_finish;
			}
		}
	}
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
	if (g_in_dbb_mode && In_Black_List(db, nlp_out, in_owner, NULL)) {
		Put_Nlist(nlp_out, "ACTION", CO_RANKING_BLOGS);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_RANKING_BLOGS);
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
	if (g_in_dbb_mode) {
		Put_Nlist(nlp_out, "PROFILE_START", "<!--");
		Put_Nlist(nlp_out, "PROFILE_END", "-->");
		if (Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
			Put_Nlist(nlp_out, "ACTION", CO_RANKING_BLOGS);
			Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
			Page_Out(nlp_out, CO_SKEL_ERROR);
			goto clear_finish;
		}
	}

	chp_tmp = Get_Nlist(nlp_in, "start", 1);
	in_start = chp_tmp ? atoi(chp_tmp) : 0;
	chp_tmp = Get_Nlist(nlp_in, "tbstart", 1);
	in_tbstart = chp_tmp ? atoi(chp_tmp) : 0;

	sprintf(cha_sql,
		" select T1.n_blog_id"
		",T1.c_blog_title"
		",date_format(max(T2.d_entry_modify_time),'%%Y年%%c月%%e日')"
		",count(T3.n_comment_id) as n_count"
		" from at_blog T1"
		" left join at_entry T2 on T1.n_blog_id = T2.n_blog_id"
		" left join at_comment T3 on T1.n_blog_id = T3.n_blog_id"
		" group by T1.n_blog_id"
		" order by n_count desc"
		" limit %d, %d;", in_start, CO_RANKING_BLOGS);
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres == NULL) {
		Put_Nlist(nlp_out, "ERROR", "コメントランキングの表示に失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}

	if (in_start) {
		Put_Format_Nlist(nlp_out, "PREV",
			"<a href=\"%s?start=0&tbstart=%d\"><img src=\"%s/mae.gif\" alt=\"前へ\" border=\"0\" style=\"width:40px\"></a>",
			CO_CGI_RANKING, in_tbstart, g_cha_admin_image);
	} else if (Db_GetRowCount(dbres) > CO_RANKING_BLOGS) {
		Put_Format_Nlist(nlp_out, "NEXT",
			"<a href=\"%s?start=%d&tbstart=%d\"><img src=\"%s/mae.gif\" alt=\"次へ\" border=\"0\" style=\"width:40px\"></a>",
			CO_CGI_RANKING, CO_RANKING_BLOGS, in_tbstart, g_cha_admin_image);
	} else {
	}

	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp && chp_tmp[0]) {
			in_blog = atoi(chp_tmp);
			Put_Nlist(nlp_out, "COMMENTLIST", "<tr>");
			chp_tmp = Db_GetValue(dbres, i, 1);
			chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
			Put_Format_Nlist(nlp_out, "COMMENTLIST", "<td>%s</td>", chp_esc);
			free(chp_esc);
			if (!Get_Nickname_From_Blog(db, nlp_out, in_blog, cha_nickname)) {
				Put_Nlist(nlp_out, "ERROR", "ランキングの表示に失敗しました。");
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				Page_Out(nlp_out, CO_SKEL_ERROR);
				goto clear_finish;
			}
			chp_esc = Escape_HtmlString(cha_nickname);
			Put_Format_Nlist(nlp_out, "COMMENTLIST", "<td>%s</td>", chp_esc);
			free(chp_esc);
			chp_tmp = Db_GetValue(dbres, i, 2);
			Put_Format_Nlist(nlp_out, "COMMENTLIST", "<td nowrap>%s</td>", chp_tmp ? chp_tmp : "");
			chp_tmp = Db_GetValue(dbres, i, 3);
			Put_Format_Nlist(nlp_out, "COMMENTLIST", "<td align=\"right\">%s</td>", chp_tmp ? chp_tmp : "");
			Put_Nlist(nlp_out, "COMMENTLIST", "</tr>\n");
		}
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		" select T1.n_blog_id"
		",T1.c_blog_title"
		",date_format(max(T2.d_entry_modify_time),'%%Y年%%c月%%e日')"
		",count(T3.n_tb_id) as n_count"
		" from at_blog T1"
		" left join at_entry T2 on T1.n_blog_id = T2.n_blog_id"
		" left join at_trackback T3 on T1.n_blog_id = T3.n_blog_id"
		" group by T1.n_blog_id"
		" order by n_count desc"
		" limit %d, %d;", in_tbstart, CO_RANKING_BLOGS);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "引用記事ランキングの表示に失敗しました。");
		} else {
			Put_Nlist(nlp_out, "ERROR", "トラックバックランキングの表示に失敗しました。");
		}
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}

	if (in_tbstart) {
		Put_Format_Nlist(nlp_out, "PREV",
			"<a href=\"%s?start=%d&tbstart=0&page1=none\"><img src=\"%s/mae.gif\" alt=\"前へ\" border=\"0\" style=\"width:40px\"></a>",
			CO_CGI_RANKING, in_start, g_cha_admin_image);
	} else if (Db_GetRowCount(dbres) > CO_RANKING_BLOGS) {
		Put_Format_Nlist(nlp_out, "NEXT",
			"<a href=\"%s?start=%d&tbstart=%d&page1=none\"><img src=\"%s/mae.gif\" alt=\"次へ\" border=\"0\" style=\"width:40px\"></a>",
			CO_CGI_RANKING, in_start, CO_RANKING_BLOGS, g_cha_admin_image);
	} else {
	}

	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp && chp_tmp[0]) {
			in_blog = atoi(chp_tmp);
			Put_Nlist(nlp_out, "TRACKBACKLIST", "<tr>");
			chp_tmp = Db_GetValue(dbres, i, 1);
			chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
			Put_Format_Nlist(nlp_out, "TRACKBACKLIST", "<td>%s</td>", chp_esc);
			free(chp_esc);
			if (!Get_Nickname_From_Blog(db, nlp_out, in_blog, cha_nickname)) {
				Put_Nlist(nlp_out, "ERROR", "ランキングの表示に失敗しました。");
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				Page_Out(nlp_out, CO_SKEL_ERROR);
				goto clear_finish;
			}
			chp_esc = Escape_HtmlString(cha_nickname);
			Put_Format_Nlist(nlp_out, "TRACKBACKLIST", "<td>%s</td>", chp_esc);
			free(chp_esc);
			chp_tmp = Db_GetValue(dbres, i, 2);
			Put_Format_Nlist(nlp_out, "TRACKBACKLIST", "<td nowrap>%s</td>", chp_tmp ? chp_tmp : "");
			chp_tmp = Db_GetValue(dbres, i, 3);
			Put_Format_Nlist(nlp_out, "TRACKBACKLIST", "<td align=\"right\">%s</td>", chp_tmp ? chp_tmp : "");
			Put_Nlist(nlp_out, "TRACKBACKLIST", "</tr>\n");
		}
	}
	Db_CloseDyna(dbres);

	Page_Out(nlp_out, CO_SKEL_BLOG_RANKING);

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
