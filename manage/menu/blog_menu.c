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
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_menu.c,v 1.72 2008/01/24 05:07:57 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
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
 *	エラー画面に埋め込み
%* ------------------------------------------------------------------------
 * Return:                      正常終了 0
 *      戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
void put_error_data(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chpa_esc[] = {
		 "BTN_DISP_LOGOFF", "BTN_BLOG_EXPORT", "BTN_DISP_BLOG_IMPORT", "BTN_BLOG_IMPORT.x", "BTN_BLOG_IMPORT.y"
		,"from_error","from_user", "blogid", "entry_file", "x", "y", NULL
	};

	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	Build_HiddenEncode(nlp_out, "HIDDEN", "from_error", "1");
	Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
	return ;
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
	DBase *db;
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	char *chp_fromuser;
	NLIST *nlp_in;
	NLIST *nlp_out;
	int in_entry_count;
	int in_browser;
	int in_error;
	int in_blog;
	int in_skip;

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
	in_error = 0;
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
			in_blog = Auth_Login(db, nlp_in, nlp_out, CO_LOGIN_ID, CO_PASSWORD, &g_in_login_owner);
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
	if (g_in_dbb_mode && In_Black_List(db, nlp_out, g_in_login_owner, NULL)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
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
	if (g_in_hb_mode && in_blog == INT_MAX && Get_Blog_Auth(db, nlp_out, g_in_login_owner, in_blog) < CO_AUTH_WRITE) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを操作する権限がありません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		in_error = 1;
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_MENU);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを作成されていない方はご利用になれません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		in_error = 1;
		goto clear_finish;
	}
	if (g_in_dbb_mode) {
		Put_Nlist(nlp_out, "PROFILE_START", "<!--");
		Put_Nlist(nlp_out, "PROFILE_END", "-->");
	}

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
	put_error_data(nlp_in, nlp_out);

	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}

	if (Get_Nlist(nlp_in, "BTN_DISP_LOGOFF", 1)) {
		if (Clear_Login(db, nlp_out, in_blog)) {
			++in_error;
			Page_Out(nlp_out, CO_SKEL_ERROR);
		}
		goto location_release;
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_BLOG_EXPORT", 1)) {
		if (!in_skip) {
			if (Write_To_Xml(db, nlp_in, nlp_out, in_blog, stdout)) {
				++in_error;
				Page_Out(nlp_out, CO_SKEL_ERROR);
			}
			goto clear_finish;
		} else {
			goto location_release;
		}
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_BLOG_IMPORT", 1)) {
		Page_Out(nlp_out, CO_SKEL_BLOG_IMPORT);
		goto clear_finish;
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_BLOG_IMPORT.x", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "entry_file", 1);
		chp_tmp2 = Get_Nlist(nlp_in, "entry_file", 2);
		chp_tmp3 = Get_Nlist(nlp_in, "entry_file", 3);
		if (chp_tmp && chp_tmp2 && chp_tmp3) {
			if (!in_skip) {
				if (Read_Xml_From_File(db, nlp_in, nlp_out, in_blog, chp_tmp)) {
					++in_error;
					Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_BLOG_IMPORT", "1");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				} else {
					goto location_release;
				}
			} else {
				goto location_release;
			}
		} else {
			++in_error;
			Put_Nlist(nlp_out, "ERROR", "アップロードするXMLファイルを指定してください。<br>");
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_BLOG_IMPORT", "1");
			Page_Out(nlp_out, CO_SKEL_ERROR);
			goto clear_finish;
		}
	} else {
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "BLOGTOP", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "BLOGTOP", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "BLOGTOP", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
		/* 投稿がない場合は編集ボタンを眠らせる。 */
		in_entry_count = Get_Total_Entry(db, nlp_in, nlp_out, 0, in_blog);
		if(in_entry_count <= 0){
			Put_Nlist(nlp_out, "ENABLE_START", "<!--");
			Put_Nlist(nlp_out, "ENABLE_END", "-->");
		} else {
			Put_Nlist(nlp_out, "DISABLE_START", "<!--");
			Put_Nlist(nlp_out, "DISABLE_END", "-->");
		}
		/* ブックマークレット使用不可のブラウザを蹴る */
		in_browser = Get_Browser();
		if(in_browser == CO_BROWSER_NS47 || in_browser == CO_BROWSER_OTHER) {
			Put_Nlist(nlp_out, "COMMENT_START", "<!--");
			Put_Nlist(nlp_out, "COMMENT_END", "-->");
		}
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "IMPORT_START", "<!--");
			Put_Nlist(nlp_out, "IMPORT_END", "-->");
		}
		Page_Out(nlp_out, CO_SKEL_TOPMENU);
		goto clear_finish;
	}

location_release:
	if (Get_Nlist(nlp_in, "from_user", 1)) {
		if (g_in_dbb_mode) {
			printf("Location: %s%s%s/%s/\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			printf("Location: %s%s%s/%08d/\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			printf("Location: %s%s%s/%s?bid=%d\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
	} else {
		printf("Location: %s%s%s/%s?blogid=%d\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
	}

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
