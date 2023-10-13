/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_sidecontent.c
 * First-Created:	2004/08/12 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	サイドコンテンツの編集
 *	各サイドコンテンツの設定
-* ------------------------------------------------------------------------
 * Change-Log:
 *	2005/07/08 関根大輔
 *		再構築完了。ただしサブクエリを連発しているので、載せ替えは困難になりそう。
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_sidecontent.c,v 1.164 2009/02/24 04:37:18 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include "libcgi2.h"
#include "libdb2.h"
#include "libnet.h"
#include "libcommon2.h"
#include "libauth.h"
#include "libblog.h"
#include "libblogcart.h"
#include "libblogreserve.h"

#define CO_SIDECONTENT_NAME_ID		"n_sidecontent_id"
#define CO_SIDECONTENT_NAME_SEQ		"n_sidecontent_order"
#define CO_SIDECONTENT_NAME_TABLE	"at_sidecontent"
#define CO_SIDECONTENT_WHERE_ETC1	"b_display = 1 and n_blog_id = %d and n_sidecontent_type = %d"
#define CO_SIDECONTENT_WHERE_ETC2	"b_display = 1 and n_blog_id = %d"
#define CO_LINKITEM_NAME_ID			"n_linkitem_id"
#define CO_LINKITEM_NAME_SEQ		"n_linkitem_order"
#define CO_LINKITEM_NAME_TABLE		"at_linkitem"
#define CO_LINKITEM_WHERE_ETC		"n_linkgroup_id = %d and n_blog_id = %d"
#define CO_MODE_UP		"u"
#define CO_MODE_DOWN	"d"
#define CO_MODE_TOP		"t"
#define CO_MODE_BOTTOM	"b"
#define CO_DEFAULT_RSS	5

static int g_in_same_database = 1;

/*
+* ------------------------------------------------------------------------
 * Function:		put_error_page
 * Description:
 *	エラーページに埋め込み。
%* ------------------------------------------------------------------------
 * Return:
 *	なし
-* ------------------------------------------------------------------------*/
static void put_error_page(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chpa_escape[] = {
		  "BTN_UP_SIDECONTENT.x", "BTN_UP_SIDECONTENT.y", "BTN_DOWN_SIDECONTENT.x", "BTN_DOWN_SIDECONTENT.y"
		, "BTN_NEW_RSSITEM.x", "BTN_NEW_RSSITEM.y", "BTN_TOP_SIDECONTENT.x", "BTN_TOP_SIDECONTENT.y"
		, "BTN_UNABLE_DISP_SIDECONTENT.x", "BTN_UNABLE_DISP_SIDECONTENT.y", "BTN_DISP_EDIT_RSS"
		, "BTN_DISP_SETTING_SIDECONTENT", "BTN_EDIT_SIDECONTENT", "BTN_ADD_SIDECONTENT.x", "BTN_ADD_SIDECONTENT.y"
		, "BTN_ADD_RSS.x", "BTN_ADD_RSS.y", "BTN_EDIT_RSS.x", "BTN_EDIT_RSS.y", "BTN_DELETE_RSS_ASK", "BTN_NEW_LINKITEM.x", "BTN_NEW_LINKITEM.y"
		, "BTN_DELETE_RSS", "BTN_SET_ARCHIVE.x", "BTN_SET_ARCHIVE.y", "BTN_EDIT_PROFILE.x", "BTN_EDIT_PROFILE.y", "BTN_ADD_NEW_LINKLIST.x", "BTN_ADD_NEW_LINKLIST.y"
		, "BTN_DISP_LINKLIST_TITLE_EDIT", "BTN_DISP_ADD_LINKITEM.x", "BTN_DISP_ADD_LINKITEM.y", "BTN_DISP_EDIT_LINKITEM.x", "BTN_DISP_EDIT_LINKITEM.y"
		, "BTN_ADD_LINKITEM.x", "BTN_ADD_LINKITEM.y", "BTN_EDIT_LINKITEM.x", "BTN_EDIT_LINKITEM.y"
		, "BTN_DISP_DELETE_LINKITEM_ASK.x", "BTN_DISP_DELETE_LINKITEM_ASK.y", "BTN_DELETE_LINKITEM"
		, "BTN_UP_LINKITEM.x", "BTN_UP_LINKITEM.y", "BTN_DOWN_LINKITEM.x", "BTN_DOWN_LINKITEM.y"
		, "BTN_EDIT_LINKLIST_TITLE.x", "BTN_EDIT_LINKLIST_TITLE.y", "BTN_DISP_DELETE_LINKLIST_ASK", "BTN_DELETE_LINKLIST"
		, "BTN_DISP_EDIT_BLOGPARTS", "BTN_EDIT_BLOGPARTS.x", "BTN_EDIT_BLOGPARTS.y", "BTN_DELETE_PARTS_ASK"
		, "author_image", "sidecontent", "blogid", NULL
	};
	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_escape);
	Build_HiddenEncode(nlp_out, "HIDDEN", "from_error", "1");
	Put_Nlist(nlp_out, "ACTION", CO_CGI_SIDECONTENT);
	return;
}

/*
+* ------------------------------------------------------------------------
 * Function:            linkitem_error_check()
 * Description:
 *      個々のリンクのエラーチェック
%* ------------------------------------------------------------------------
 * Return:
 *      正常 0
 *      エラー時 1以上
-* ------------------------------------------------------------------------*/
static int linkitem_error_check(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chp_tmp;
	char cha_error[512];
	int in_error;

	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "linkitem_title", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンクのタイトルを入力してください。<br>");
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "リンクのタイトルを入力してください。<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_LINKITEM_TITLE) {
		sprintf(cha_error, "リンクのタイトルは半角%d文字(全角%d文字)以内で入力してください。<br>"
			, CO_MAXLEN_LINKITEM_TITLE, CO_MAXLEN_LINKITEM_TITLE / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "linkitem_url", 1);
	if (!chp_tmp || !*chp_tmp || strcmp(chp_tmp, "http://") == 0) {
		Put_Nlist(nlp_out, "ERROR", "リンクのURLを入力してください。<br>");
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "リンクのURLを入力してください。<br>");
                in_error++;
	} else if (Check_URL(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "リンクのURLに誤りがあります。<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_LINKITEM_URL) {
		sprintf(cha_error, "リンクのURLは半角%d文字以内で入力してください。<br>", CO_MAXLEN_LINKITEM_URL);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:            linklist_error_check()
 * Description:
 *      リンク集タイトルのエラーチェック
%* ------------------------------------------------------------------------
 * Return:
 *      正常 0
 *      エラー時 1以上
-* ------------------------------------------------------------------------*/
static int linklist_error_check(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chp_tmp;
	char cha_error[512];
	int in_error;

	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "linklist_title", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンク集のタイトルを入力してください。<br>");
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "リンク集のタイトルを入力してください。<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_LINKLIST_TITLE) {
		sprintf(cha_error, "リンク集のタイトルは、半角%d文字(全角%d文字)以内で入力してください。<br>"
			, CO_MAXLEN_LINKLIST_TITLE, CO_MAXLEN_LINKLIST_TITLE/2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	divide_sidecontent2()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			なし
-* ------------------------------------------------------------------------*/
static void divide_sidecontent2(char* chp_tmp, int *inp_id, int *inp_type)
{
	char *chp_tmp2;

	*inp_id = 0;
	*inp_type = 0;

	chp_tmp2 = strchr(chp_tmp, ':');
	if (!chp_tmp2) {
		*inp_id = atoi(chp_tmp);
		return;
	}
	*inp_type = atoi(chp_tmp2 + 1);
	*chp_tmp2 = '\0';
	*inp_id = atoi(chp_tmp);
	*chp_tmp2 = ':';
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	divide_sidecontent()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			なし
-* ------------------------------------------------------------------------*/
static void divide_sidecontent(NLIST *nlp_in, char* chp_name, int *inp_id, int *inp_type)
{
	char *chp_tmp;

	*inp_id = 0;
	*inp_type = 0;

	chp_tmp = Get_Nlist(nlp_in, chp_name, 1);
	if (chp_tmp)
		divide_sidecontent2(chp_tmp, inp_id, inp_type);
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_calendar()
 * Description:
 *	カレンダーを日本語表示するかどうかを設定
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_calendar(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_from_error;
	char *chp_tmp;
	char cha_sql[512];
	int in_japanese;

	chp_from_error = Get_Nlist(nlp_in, "from_error", 1);

	if (chp_from_error && *chp_from_error) {
		chp_tmp = Get_Nlist(nlp_in, "japanese_calendar", 1);
		if(!chp_tmp){
			in_japanese = 0;
		} else {
			in_japanese = atoi(chp_tmp);
		}
	} else {
		sprintf(cha_sql, "select b_japanese_calendar from at_profile where n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres , 0, 0);
		if(!chp_tmp){
			in_japanese = 0;
		} else {
			in_japanese = atoi(chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	if (in_japanese) {
		Put_Nlist(nlp_out, "JAPANESE_CALENDAR", "checked");
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_archive()
 * Description:
 *	コメント･トラックバック･月間アーカイブの設定画面を表示
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_archive(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_from_error;
	char *chp_range_type;
	char *chp_archive_value;
	char cha_sql[512];
	int i;
	int in_archive_id;
	int in_archive_type;

	divide_sidecontent(nlp_in, "sidecontent", &in_archive_id, &in_archive_type);
	chp_from_error = Get_Nlist(nlp_in, "from_error", 1);
	chp_range_type = NULL;		/* エラー抑制処理 */
	chp_archive_value = NULL;	/* エラー抑制処理 */
	dbres = NULL;			/* エラー抑制処理 */
	if (chp_from_error && *chp_from_error) {
		chp_archive_value = Get_Nlist(nlp_in, "archive_comment", 1);
		chp_tmp = Escape_HtmlString(chp_archive_value);
		Put_Nlist(nlp_out, "ARCHIVE_COMMENT", chp_tmp);
		free(chp_tmp);
		chp_archive_value = Get_Nlist(nlp_in, "archive_trackback", 1);
		chp_tmp = Escape_HtmlString(chp_archive_value);
		Put_Nlist(nlp_out, "ARCHIVE_TRACKBACK", chp_tmp);
		free(chp_tmp);
		chp_archive_value = Get_Nlist(nlp_in, "archive_month", 1);
		chp_tmp = Escape_HtmlString(chp_archive_value);
		Put_Nlist(nlp_out, "ARCHIVE_MONTH_VALUE", chp_tmp);
		free(chp_tmp);
		if (chp_range_type && atoi(chp_range_type) == 1) {
			Put_Nlist(nlp_out, "RANGE_TYPE1", CO_CHECKED);
		} else {
			Put_Nlist(nlp_out, "RANGE_TYPE0", CO_CHECKED);
		}
		chp_archive_value = Get_Nlist(nlp_in, "archive_newentry", 1);
		chp_tmp = Escape_HtmlString(chp_archive_value);
		Put_Nlist(nlp_out, "ARCHIVE_NEWENTRY", chp_tmp);
		free(chp_tmp);
	} else {
		strcpy(cha_sql, "select T1.n_archive_value");
		strcat(cha_sql, ",T1.n_archive_id");
		strcat(cha_sql, " from at_archive T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres){
			Put_Nlist(nlp_out, "ERROR", "表示件数を得るクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		for (i = 0; i < Db_GetRowCount(dbres); ++i) {
			chp_archive_value = Db_GetValue(dbres, i, 0);
			if (!chp_archive_value || !*chp_archive_value) {
				Db_CloseDyna(dbres);
				Put_Nlist(nlp_out, "ERROR", "表示件数が登録されていません。<br>");
				return 1;
			}
			chp_tmp = Db_GetValue(dbres, i, 1);
			if (chp_tmp) {
				in_archive_id = atoi(chp_tmp);
			} else {
				in_archive_id = 0;
			}
			switch(in_archive_id) {
			case CO_SIDECONTENT_ARCHIVE_COMMENT:
				Put_Nlist(nlp_out, "ARCHIVE_COMMENT", chp_archive_value);
				break;
			case CO_SIDECONTENT_ARCHIVE_TRACKBACK:
				Put_Nlist(nlp_out, "ARCHIVE_TRACKBACK", chp_archive_value);
				break;
			case CO_SIDECONTENT_ARCHIVE_MONTHLY:
				if (atoi(chp_archive_value)) {
					Put_Nlist(nlp_out, "RANGE_TYPE1", CO_CHECKED);
					Put_Nlist(nlp_out, "ARCHIVE_MONTH_VALUE", chp_archive_value);
				} else {
					Put_Nlist(nlp_out, "RANGE_TYPE0", CO_CHECKED);
				}
				break;
			case CO_SIDECONTENT_ARCHIVE_NEWENTRY:
				Put_Nlist(nlp_out, "ARCHIVE_NEWENTRY", chp_archive_value);
				break;
			default:
				Put_Nlist(nlp_out, "ERROR", "予期しないリクエストです。トップページからやり直してください。<br>");
				return 1;
			}
		}
		Db_CloseDyna(dbres);
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_profile()
 * Description:
 *	プロフィール登録画面を表示。
%* ------------------------------------------------------------------------
 * Return:			正常M終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_profile(DBase *db, NLIST *nlp_in, NLIST* nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_from_error;
	char *chp_nickname;
	char *chp_selfintro;
	char *chp_mail;
	char *chp_login;
	char *chp_pass1;
	char *chp_pass2;
	char *chp_send;
	char *chp_escape;
	char *chp_question;
	char *chp_answer;
	char cha_sql[1024];
	char cha_str[1024];
	static char* chpa_top[2][2] = {{"","--"},{NULL, NULL}};

	sprintf(cha_str, "%d", CO_MAX_UPLOAD_SIZE);
	Edit_Money(cha_str);
	Put_Nlist(nlp_out, "MAXSIZE", cha_str);

	if (!g_in_same_database) {
		Put_Nlist(nlp_out, "LOGIN_START", "<!--");
		Put_Nlist(nlp_out, "LOGIN_END", "-->");
	}
	chp_from_error = Get_Nlist(nlp_in, "from_top", 1);
	if (chp_from_error && *chp_from_error) {
		Build_Hidden(nlp_out, "HIDDEN", "from_top", "1");
	}
	/* at_profileの設定 */
	strcpy(cha_sql, "select T1.c_author_nickname");	/* 0 作者ニックネーム */
	strcat(cha_sql, ", T1.c_author_selfintro");		/* 1 作者自己紹介 */
	strcat(cha_sql, ", T1.c_author_mail");			/* 2 作者メールアドレス */
	strcat(cha_sql, ", T1.c_login");				/* 3 管理モードログインID */
	strcat(cha_sql, ", T1.c_password");				/* 4 管理モードログインPASSWORD */
	strcat(cha_sql, ", T1.b_send_mail");			/* 5 コメントやトラックバックがあったらメールを送信 */
	strcat(cha_sql, ", T1.n_remind");				/* 6 リマインダ質問 */
	strcat(cha_sql, ", T1.c_remind");				/* 7 リマインダ答え */
	strcat(cha_sql, ", T1.c_author_image");			/* 8 作者画像 */
	strcat(cha_sql, " from at_profile T1");
	sprintf(cha_sql+strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "プロフィールを得るクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		Put_Nlist(nlp_out, "ERROR", "プロフィールが設定されていません。<br>");
		Db_CloseDyna(dbres);
	}
	chp_from_error = Get_Nlist(nlp_in, "from_error", 1);
	if (chp_from_error && *chp_from_error) {
		chp_nickname = Get_Nlist(nlp_in, "author_nickname", 1);
		chp_selfintro = Get_Nlist(nlp_in, "author_notion", 1);
		chp_mail = Get_Nlist(nlp_in, "author_mail", 1);
		chp_login = Get_Nlist(nlp_in, "author_login", 1);
		chp_pass1 = Get_Nlist(nlp_in, "author_pass1", 1);
		chp_pass2 = Get_Nlist(nlp_in, "author_pass2", 1);
		chp_send = Get_Nlist(nlp_in, "author_send", 1);
		chp_question = Get_Nlist(nlp_in, "author_question", 1);
		chp_answer = Get_Nlist(nlp_in, "author_anser", 1);
	} else {
		chp_nickname = Db_GetValue(dbres, 0, 0);
		chp_selfintro = Db_GetValue(dbres, 0, 1);
		chp_mail = Db_GetValue(dbres, 0, 2);
		chp_login = Db_GetValue(dbres, 0, 3);
		chp_pass1 = Db_GetValue(dbres, 0, 4);
		chp_pass2 = Db_GetValue(dbres, 0, 4);
		chp_send = Db_GetValue(dbres, 0, 5);
		chp_question = Db_GetValue(dbres, 0, 6);
		chp_answer = Db_GetValue(dbres, 0, 7);
	}
	/* ニックネーム */
	if (chp_nickname && *chp_nickname) {
		chp_escape = Escape_HtmlString(chp_nickname);
		Put_Nlist(nlp_out, "AUTHOR_NICKNAME", chp_escape);
		free(chp_escape);
	}
	/* 自己紹介 */
	if (chp_selfintro && *chp_selfintro) {
		chp_escape = Escape_HtmlString(chp_selfintro);
		Put_Nlist(nlp_out, "AUTHOR_NOTION", chp_escape);
		free(chp_escape);
	}
	/* メールアドレス */
	if (chp_mail && *chp_mail) {
		chp_escape = Escape_HtmlString(chp_mail);
		Put_Nlist(nlp_out, "AUTHOR_MAIL", chp_escape);
		free(chp_escape);
	}
	if (g_in_same_database) {
		/* ログインID */
		if (chp_login && *chp_login) {
			chp_escape = Escape_HtmlString(chp_login);
			Put_Nlist(nlp_out, "AUTHOR_LOGIN", chp_escape);
			free(chp_escape);
		}
		/* パスワード１ */
		if (chp_pass1 && *chp_pass1) {
			chp_escape = Escape_HtmlString(chp_pass1);
			Put_Nlist(nlp_out, "AUTHOR_PASS1", chp_escape);
			free(chp_escape);
		}
		/* パスワード２ */
		if (chp_pass2 && *chp_pass2) {
			chp_escape = Escape_HtmlString(chp_pass2);
			Put_Nlist(nlp_out, "AUTHOR_PASS2", chp_escape);
			free(chp_escape);
		}
		/* リマインダ質問 */
		Build_ComboDb(OldDBase(db), nlp_out, "REMINDER", "select n_remind,c_remind from sy_remind order by n_remind", "author_question", chpa_top, NULL, chp_question ? chp_question : "--");
		/* リマインダ答え */
		if (chp_answer && *chp_answer) {
			chp_escape = Escape_HtmlString(chp_answer);
			Put_Nlist(nlp_out, "AUTHOR_ANSWER", chp_escape);
			free(chp_escape);
		}
	}
	/* メール受信 */
	if (chp_send && *chp_send && atoi(chp_send)) {
		Put_Nlist(nlp_out, "AUTHOR_SEND", "checked");
	}
	/* 作者画像 (エラー時でもファイルボックスの内容は引き継がない) */
	chp_tmp = Db_GetValue(dbres, 0, 8);
	if (chp_tmp && *chp_tmp) {
		Put_Format_Nlist(nlp_out, "AUTHOR_IMAGE", "<img src=\"%s%s%s/%s?BTN_DISP_PROFILE_IMAGE=1&blogid=%d\" height=\"45\" width=\"60\" align=\"left\">\n",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_SIDECONTENT, in_blog);
		if (Get_Nlist(nlp_in, "delete_image", 1)) {
			Build_Checkbox_With_Id(nlp_out, "AUTHOR_IMAGE", "delete_image", "1", 1, "delete_image", "画像を削除する<br>");
		} else {
			Build_Checkbox_With_Id(nlp_out, "AUTHOR_IMAGE", "delete_image", "1", 0, "delete_image", "画像を削除する<br>");
		}
	}
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_tmp && *chp_tmp) {
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%s?bid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
	} else {
		Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%s?blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
	}
	Page_Out(nlp_out, CO_SKEL_PROFILE);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            disp_page_access_view()
 * Description:
 *	アクセスカウンタの表示
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_access_view(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char cha_to[32];
	char cha_from[32];
	char *chp_tmp;
	int in_page_view;
	int in_unique_user;
	int in_today_all;
	int in_today_unique;

	Get_NowDate(cha_from);
	cha_from[4] = ':';
	cha_from[7] = ':';
	strcpy(cha_to, cha_from);

	strcat(cha_from, "00:00:00");
	strcat(cha_to, "23:59:59");

	sprintf(cha_sql,
		" select n_page_view"
		",n_unique_user"
		" from at_profile"
		" where n_blog_id = %d",
		in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "QUERY", "<br>");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_page_view = atoi(chp_tmp);
	} else {
		in_page_view = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_unique_user = atoi(chp_tmp);
	} else {
		in_unique_user = 0;
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		" select count(*)"
		" from at_view"
		" where n_blog_id = %d"
		" and d_view >= '%s'"
		" and d_view <= '%s'"
		, in_blog, cha_from, cha_to);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "QUERY", "<br>");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_today_all = atoi(chp_tmp);
	} else {
		in_today_all = 0;
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		" select count(*)"
		" from at_view"
		" where n_blog_id = %d"
		" and d_view >= '%s'"
		" and d_view <= '%s'"
		" and b_valid != 0"
		, in_blog, cha_from, cha_to);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "QUERY", "<br>");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_today_unique = atoi(chp_tmp);
	} else {
		in_today_unique = 0;
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "%d", in_page_view + in_today_all);
	Build_Hidden(nlp_out, "HIDDEN", "PV_ALL", cha_sql);
	sprintf(cha_sql, "%d", in_unique_user + in_today_unique);
	Build_Hidden(nlp_out, "HIDDEN", "UU_ALL", cha_sql);
	sprintf(cha_sql, "%d", in_today_all);
	Build_Hidden(nlp_out, "HIDDEN", "PV_TODAY", cha_sql);
	sprintf(cha_sql, "%d", in_today_unique);
	Build_Hidden(nlp_out, "HIDDEN", "UU_TODAY", cha_sql);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_access_edit()
 * Description:
 *	アクセスカウンタ編集画面表示
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_access_edit(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_tmp;
	int in_page_view;
	int in_unique_user;

	sprintf(cha_sql,
		" select n_page_view"
		",n_unique_user"
		" from at_profile"
		" where n_blog_id = %d",
		in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "QUERY", "<br>");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_page_view = atoi(chp_tmp);
	} else {
		in_page_view = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_unique_user = atoi(chp_tmp);
	} else {
		in_unique_user = 0;
	}
	Db_CloseDyna(dbres);

	Put_Format_Nlist(nlp_out, "PV_ALL", "%d", in_page_view);
	Put_Format_Nlist(nlp_out, "UU_ALL", "%d", in_unique_user);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_edditparts()
 * Description:
 *	ブログパーツ編集画面を表示する。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_editparts(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_tmp;
	char *chp_esc;

	chp_tmp = Get_Nlist(nlp_in, "partsid", 1);
	if (!chp_tmp) {
		return 0;
	}
	sprintf(cha_sql, "select c_parts_title,c_parts_body,b_parts_title from at_parts where n_blog_id = %d and n_parts_id = %s", in_blog, chp_tmp);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するRSS取り込み数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "partsid", chp_tmp);
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "PARTS_TITLE", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "PARTS_BODY", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "PARTS_DISP", "checked");
	}
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_sidecontent_list()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_sidecontent_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_esc;
	char *chp_fromuser;
	char *chp_selected;
	char cha_sql[512];
	int in_count;
	int i, j, k;

	sprintf(cha_sql, "select count(*) from at_sidecontent where n_blog_id = %d and n_sidecontent_type = %d", in_blog, CO_SIDECONTENT_ARCHIVE_LINKS);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツ数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_selected = Db_GetValue(dbres, 0, 0);
	if (!chp_selected) {
		Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツ数の取得に失敗しました。<br>");
		return 1;
	}
	if (atoi(chp_selected) >= CO_MAX_LINK_LISTS) {
		Put_Nlist(nlp_out, "LINKDISABLE", "disabled");
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select count(*) from at_sidecontent where n_blog_id = %d and n_sidecontent_type = %d", in_blog, CO_SIDECONTENT_ARCHIVE_RSS);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するRSS取り込み数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_selected = Db_GetValue(dbres, 0, 0);
	if (!chp_selected) {
		Put_Nlist(nlp_out, "ERROR", "表示するRSS取り込み数の取得に失敗しました。<br>");
		return 1;
	}
	if (atoi(chp_selected) > CO_MAX_RSS_IMPORTS) {
		Put_Nlist(nlp_out, "RSSDISABLE", "disabled");
	}
	Db_CloseDyna(dbres);

	/* 表示リスト */
	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	strcpy(cha_sql, "select concat_ws(':',T1.n_sidecontent_id, T1.n_sidecontent_type)");
	strcat(cha_sql, ", T1.c_sidecontent_title");
	strcat(cha_sql, " from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql),
		" where (T1.n_sidecontent_type = %d or T1.n_sidecontent_type = %d)",
		CO_SIDECONTENT_ARCHIVE_LINKS, CO_SIDECONTENT_ARCHIVE_RSS);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_sidecontent_order");
	/* 一回開いてコンテンツ数を得る */
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツ数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	for (i = 0, j = 0, k = 0; i < in_count; ++i) {
		int in_id;
		int in_type;
		char *chp_tmp;
		char *chp_esc;
		char *chp_name;
		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_name = Db_GetValue(dbres, i, 1);
		if (chp_tmp && chp_name) {
			chp_esc = Escape_HtmlString(chp_name);
			divide_sidecontent2(chp_tmp, &in_id, &in_type);
			if (in_type == CO_SIDECONTENT_ARCHIVE_LINKS) {
				Put_Format_Nlist(nlp_out, "LINK_LIST", "<tr%s><td width=\"60%%\">%s</td>", (j % 2) ? " bgcolor=\"#F0F5FD\"" : "", chp_esc);
				Put_Format_Nlist(nlp_out, "LINK_LIST", "<td align=\"center\"><a href=\"%s?BTN_DISP_LINKLIST_TITLE_EDIT=1&blogid=%d&&sidecontent=%s%s\"><img src=\"%s/title_henshu.gif\" border=\"0\"></a></td>",
					CO_CGI_SIDECONTENT, in_blog, chp_tmp, chp_fromuser ? "&from_user=1" : "", g_cha_admin_image);
				Put_Format_Nlist(nlp_out, "LINK_LIST", "<td align=\"center\"><a href=\"%s?BTN_DISP_EDIT_LINK=1&blogid=%d&sidecontent=%s%s\"><img src=\"%s/link_henshu.gif\" border=\"0\"></a></td>",
					CO_CGI_SIDECONTENT, in_blog, chp_tmp, chp_fromuser ? "&from_user=1" : "", g_cha_admin_image);
				Put_Format_Nlist(nlp_out, "LINK_LIST", "<td align=\"center\"><a href=\"%s?BTN_DISP_DELETE_LINKLIST_ASK=1&blogid=%d&sidecontent=%s%s\"><img src=\"%s/sakujo.gif\" border=\"0\"></a></td></tr>\n",
					CO_CGI_SIDECONTENT, in_blog, chp_tmp, chp_fromuser ? "&from_user=1" : "", g_cha_admin_image);
				++j;
			} else if (in_type == CO_SIDECONTENT_ARCHIVE_RSS) {
				Put_Format_Nlist(nlp_out, "RSS_LIST", "<tr%s><td width=\"80%%\">%s</td>", (k % 2) ? " bgcolor=\"#F0F5FD\"" : "", chp_esc);
				Put_Format_Nlist(nlp_out, "RSS_LIST", "<td align=\"center\"><a href=\"%s?BTN_DISP_EDIT_RSS=1&blogid=%d&sidecontent=%s%s\"><img src=\"%s/henshu_m.gif\" border=\"0\"></a></td>",
					CO_CGI_SIDECONTENT, in_blog, chp_tmp, chp_fromuser ? "&from_user=1" : "", g_cha_admin_image);
				Put_Format_Nlist(nlp_out, "RSS_LIST", "<td align=\"center\"><a href=\"%s?BTN_DELETE_RSS_ASK=1&blogid=%d&sidecontent=%s%s\"><img src=\"%s/sakujo.gif\" border=\"0\"></a></td></tr>\n",
					CO_CGI_SIDECONTENT, in_blog, chp_tmp, chp_fromuser ? "&from_user=1" : "", g_cha_admin_image);
				++k;
			}
		}
	}
	Db_CloseDyna(dbres);
	Put_Format_Nlist(nlp_out, "LINKCOUNT", "%d", j);
	Put_Format_Nlist(nlp_out, "RSSCOUNT", "%d", k);

	if (Get_Nlist(nlp_in, "from_error", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "parts_disp", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			Put_Nlist(nlp_out, "PARTS_DISP", "checked");
		}
		chp_tmp = Get_Nlist(nlp_in, "parts_title", 1);
		if (chp_tmp) {
			chp_esc = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "PARTS_TITLE", chp_esc);
			free(chp_esc);
		}
		chp_tmp = Get_Nlist(nlp_in, "parts_body", 1);
		if (chp_tmp) {
			chp_esc = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "PARTS_BODY", chp_esc);
			free(chp_esc);
		}
		if (Get_Nlist(nlp_in, "partsid", 1)) {
			Put_Nlist(nlp_out, "PARTS_EIDT", "編集");
			Put_Nlist(nlp_out, "PARTS_BTN", "henshu.gif");
			Put_Nlist(nlp_out, "PARTS_NAME", "BTN_EDIT_BLOGPARTS");
		} else {
			Put_Nlist(nlp_out, "PARTS_EIDT", "新規登録");
			Put_Nlist(nlp_out, "PARTS_BTN", "add.gif");
			Put_Nlist(nlp_out, "PARTS_NAME", "BTN_ADD_BLOGPARTS");
		}
	} else if (Get_Nlist(nlp_in, "BTN_DISP_EDIT_BLOGPARTS", 1)) {
		disp_page_editparts(db, nlp_in, nlp_out, in_blog);
		Put_Nlist(nlp_out, "PARTS_EIDT", "編集");
		Put_Nlist(nlp_out, "PARTS_BTN", "henshu.gif");
		Put_Nlist(nlp_out, "PARTS_NAME", "BTN_EDIT_BLOGPARTS");
	} else {
		Put_Nlist(nlp_out, "PARTS_EIDT", "新規登録");
		Put_Nlist(nlp_out, "PARTS_BTN", "add.gif");
		Put_Nlist(nlp_out, "PARTS_NAME", "BTN_ADD_BLOGPARTS");
	}
	strcpy(cha_sql, "select T1.n_sidecontent_id");
	strcat(cha_sql, ", T1.c_sidecontent_title");
	strcat(cha_sql, " from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_sidecontent_type = %d", CO_SIDECONTENT_BLOG_PARTS);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_sidecontent_order");
	/* 一回開いてコンテンツ数を得る */
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツ数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	for (i = 0; i < in_count; ++i) {
		int in_id;
		int in_type;
		char *chp_tmp;
		char *chp_esc;
		char *chp_name;
		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_name = Db_GetValue(dbres, i, 1);
		if (chp_tmp && chp_name) {
			chp_esc = Escape_HtmlString(chp_name);
			divide_sidecontent2(chp_tmp, &in_id, &in_type);
			Put_Format_Nlist(nlp_out, "PARTS_LIST", "<tr%s><td width=\"80%%\">%s</td>", (i % 2) ? " bgcolor=\"#F0F5FD\"" : "", chp_esc);
			Put_Format_Nlist(nlp_out, "PARTS_LIST", "<td align=\"center\"><a href=\"%s?BTN_DISP_EDIT_BLOGPARTS=1&blogid=%d&partsid=%s%s\"><img src=\"%s/henshu_m.gif\" border=\"0\"></a></td>",
				CO_CGI_SIDECONTENT, in_blog, chp_tmp, chp_fromuser ? "&from_user=1" : "", g_cha_admin_image);
			Put_Format_Nlist(nlp_out, "PARTS_LIST", "<td align=\"center\"><a href=\"%s?BTN_DELETE_PARTS_ASK=1&blogid=%d&partsid=%s%s\"><img src=\"%s/sakujo.gif\" border=\"0\"></a></td></tr>\n",
				CO_CGI_SIDECONTENT, in_blog, chp_tmp, chp_fromuser ? "&from_user=1" : "", g_cha_admin_image);
			++k;
		}
	}
	Db_CloseDyna(dbres);
	Put_Format_Nlist(nlp_out, "PARTSCOUNT", "%d", in_count);

	strcpy(cha_sql, "select concat_ws(':',T1.n_sidecontent_id, T1.n_sidecontent_type)");
	strcat(cha_sql, ", T1.c_sidecontent_title");
	strcat(cha_sql, " from at_sidecontent T1");
	strcat(cha_sql,	" where T1.b_display != 0");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_sidecontent_order");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツ数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	Db_CloseDyna(dbres);

	if (in_count) {
		chp_selected = Get_Nlist(nlp_in, "sidecontent", 1);
		if (chp_selected && *chp_selected) {
			Build_MultiComboDb_Blog(db, nlp_out, "SIDECONTENT", "sidecontent", cha_sql, 15, chp_selected);
		} else {
			Build_MultiComboDb_Blog(db, nlp_out, "SIDECONTENT", "sidecontent", cha_sql, 15, "");
		}
	}

	/* 追加リスト */
	strcpy(cha_sql, "select concat_ws(':',T1.n_sidecontent_id, T1.n_sidecontent_type)");
	strcat(cha_sql, ", T1.c_sidecontent_title");
	strcat(cha_sql, " from at_sidecontent T1");
	strcat(cha_sql, " where T1.b_display = 0");			/* 非表示のもの */
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_sidecontent_order");
	chp_selected = Get_Nlist(nlp_out, "add_sidecontent", 1);
	if (chp_selected && *chp_selected) {
		Build_MultiComboDb_Blog(db, nlp_out, "SIDECONTENT_TO_ADD", "add_sidecontent", cha_sql, 15, chp_selected);
	} else {
		Build_MultiComboDb_Blog(db, nlp_out, "SIDECONTENT_TO_ADD", "add_sidecontent", cha_sql, 15, "");
	}

	if (disp_page_calendar(db, nlp_in, nlp_out, in_blog)) {
		Put_Nlist(nlp_out, "ERROR", "アーカイブ設定画面の表示に失敗しました。<br>");
		return 1;
	}
	if (disp_page_archive(db, nlp_in, nlp_out, in_blog)) {
		Put_Nlist(nlp_out, "ERROR", "アーカイブ設定画面の表示に失敗しました。<br>");
		return 1;
	}
	if (g_in_dbb_mode) {
		if (disp_page_access_view(db, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERROR", "アクセス数の表示に失敗しました。<br>");
			return 1;
		}
	} else {
		if (disp_page_access_edit(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERROR", "アクセス数設定画面の表示に失敗しました。<br>");
			return 1;
		}
	}
	if (g_in_cart_mode == CO_CART_SHOPPER) {
		char cha_iteminfo[10] = "";
		char cha_itemnum[10] = "";
		Put_Nlist(nlp_out, "ITEMINFO_START", "-->");
		Put_Nlist(nlp_out, "ITEMINFO_END", "<!--");
		strcpy(cha_iteminfo, Get_Nlist(nlp_in, "item_info", 1) ? Get_Nlist(nlp_in, "item_info", 1) : "");
		strcpy(cha_itemnum, Get_Nlist(nlp_in, "items_number", 1) ? Get_Nlist(nlp_in, "items_number", 1) : "");
		if (!cha_iteminfo[0] || !cha_itemnum[0]) {
			sprintf(cha_sql, "select n_iteminfo_id,n_iteminfo_num from at_blog where n_blog_id=%d;", in_blog);
			dbres = Db_OpenDyna(db, cha_sql);
			if (!dbres) {
				Put_Nlist(nlp_out, "ERROR", "表示する商品情報を得るクエリに失敗しました。<br>");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				return 1;
			}
			strcpy(cha_iteminfo, Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "1");
			strcpy(cha_itemnum, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "3");
			Db_CloseDyna(dbres);
		}
		Put_Nlist(nlp_out, "ITEMS_NUMBER", cha_itemnum);
		Build_ComboDb(OldDBase(db), nlp_out, "ITEM_INFO", "select n_iteminfo_id,c_iteminfo_name from sy_iteminfo order by n_iteminfo_id", "item_info", NULL, NULL, cha_iteminfo);
	}
	Page_Out(nlp_out, CO_SKEL_SIDECONTENT);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	shift_sidecontent_order()
 * Description:
 *	非表示にしたときや削除したとき、順番を詰める。
 *	アップデートをかけるので、
 *	この関数を呼ぶ前にトランザクションをかける必要あり。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
static int shift_sidecontent_order_by_id(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_add_id, int in_add_type)
{
	DBRes *dbres;
	char cha_sql[512];
	int in_sidecontent_order;
	int in_skip;

	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		strcpy(cha_sql, "select T1.n_sidecontent_order");
		strcat(cha_sql, " from at_sidecontent T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_sidecontent_id = %d", in_add_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_add_type);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツのオーダーを得るクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		in_sidecontent_order = atoi(Db_GetValue(dbres, 0, 0));
		Db_CloseDyna(dbres);

		/* サブクエリ使用。注意 */
		strcpy(cha_sql, "update at_sidecontent");
		strcat(cha_sql, " set n_sidecontent_order = n_sidecontent_order - 1");
		strcat(cha_sql, " where b_display = 1");
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_order > %d", in_sidecontent_order);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_add_type);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "順番を並び替えるクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	shift_sidecontent_order()
 * Description:
 *	非表示にしたときや削除したとき、順番を詰める。
 *	アップデートをかけるので、
 *	この関数を呼ぶ前にトランザクションをかける必要あり。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
static int shift_sidecontent_order(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_sidecontent_id;
	int in_add_id;
	int in_add_type;

	chp_sidecontent_id = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_sidecontent_id || !*chp_sidecontent_id) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツIDの取得に失敗しました。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_add_id, &in_add_type);
	return shift_sidecontent_order_by_id(db, nlp_in, nlp_out, in_blog, in_add_id, in_add_type);
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_rss_new()
 * Description:
 *	新規RSS取り込み設定画面を表示する。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_rss_new(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_escape;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_selected;
	char cha_sql[512];
	int i;
	int in_row;

	strcpy(cha_sql, "select T1.n_rss_id");	/* 0 RSS ID */
	strcat(cha_sql, ", T1.c_rss_title");	/* 1 RSS タイトル */
	strcat(cha_sql, " from sy_rss T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "用意されたRSSを得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	/* エラーから復帰時には選択されたもの。 */
	chp_selected = Get_Nlist(nlp_in, "prepared_rss_id", 1);
	for(i = 0; i < in_row; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_tmp2 = Db_GetValue(dbres, i, 1);
		if (chp_tmp && *chp_tmp && chp_tmp2 && *chp_tmp2) {
			chp_escape = Escape_HtmlString(chp_tmp2);
			if (chp_selected && atoi(chp_tmp) == atoi(chp_selected)) {
				Build_Radio_With_Id(nlp_out, "PREPARED", "prepared_rss_id", chp_tmp, 1, chp_escape, chp_escape);
			} else {
				Build_Radio_With_Id(nlp_out, "PREPARED", "prepared_rss_id", chp_tmp, 0, chp_escape, chp_escape);
			}
			free(chp_escape);
			Put_Nlist(nlp_out, "PREPARED", "<br>");
		}
	}
	/* エラー復帰時引継ぎ */
	if (Get_Nlist(nlp_in, "from_error", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "manualselect", 1);
		if (chp_tmp && atoi(chp_tmp) == 0) {
			Put_Nlist(nlp_out, "MANUALSELECT0", CO_CHECKED);
		} else {
			Put_Nlist(nlp_out, "MANUALSELECT1", CO_CHECKED);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_title", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_TITLE", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_url", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_URL", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_article", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_ARTICLE", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_target1", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "RSS_TARGET1", CO_CHECKED);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_target0", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "RSS_TARGET0", CO_CHECKED);
		}
	} else {
		Put_Nlist(nlp_out, "RSS_URL", "http://");
		Put_Nlist(nlp_out, "MANUALSELECT1", CO_CHECKED);
		Put_Nlist(nlp_out, "RSS_TARGET1", CO_CHECKED);
		Put_Nlist(nlp_out, "RSS_TARGET0", CO_CHECKED);
	}
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_SETTING_RSSLIST=1&blogid=%d", CO_CGI_SIDECONTENT, in_blog);
	Page_Out(nlp_out, CO_SKEL_RSS_NEW);
	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_rss_edit()
 * Description:
 *	既存のRSS取り込み設定編集画面を表示する。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_rss_edit(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_escape;
	char *chp_tmp;
	char *chp_sidecontent_id;
	char cha_sql[512];
	int in_add_id;
	int in_add_type;

	chp_sidecontent_id = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_sidecontent_id || !*chp_sidecontent_id) {
		Put_Nlist(nlp_out, "ERROR", "リクエストされた値が不正です。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_add_id, &in_add_type);
	Put_Format_Nlist(nlp_out, "ID", "%d", in_add_id);
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_sidecontent_id);
	chp_tmp = Get_Nlist(nlp_in, "from_error", 1);
	/* エラー引継ぎ */
	if (chp_tmp && *chp_tmp) {
		chp_tmp = Get_Nlist(nlp_in, "rss_title", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_TITLE", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_url", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_URL", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_article", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_ARTICLE", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "rss_target", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "RSS_TARGET", CO_CHECKED);
		}
	/* エラー引継ぎでない */
	} else {
		strcpy(cha_sql, "select T1.c_rss_title");	/* 0 RSSタイトル */
		strcat(cha_sql, ", T1.c_rss_url");			/* 1 RSSURL */
		strcat(cha_sql, ", T1.n_rss_article");		/* 2 RSS記事数 */
		strcat(cha_sql, ", T1.b_rss_target");		/* 3 ターゲットウィンドウ */
		strcat(cha_sql, " from at_rss T1");
		sprintf(cha_sql+strlen(cha_sql), " where T1.n_rss_id = %d", in_add_id);
		sprintf(cha_sql+strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "RSS情報を得るクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		if (!Db_GetRowCount(dbres)) {
			Put_Nlist(nlp_out, "ERROR", "選択されたRSSは既に削除されています。<br>");
			Db_CloseDyna(dbres);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_TITLE", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Db_GetValue(dbres, 0, 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "RSS_URL", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Db_GetValue(dbres, 0, 2);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "RSS_ARTICLE", chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, 0, 3);
		if (chp_tmp && atoi(chp_tmp) == 1) {
			Put_Nlist(nlp_out, "RSS_TARGET", CO_CHECKED);
		}
		Db_CloseDyna(dbres);
	}
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_SETTING_RSSLIST=1&blogid=%d", CO_CGI_SIDECONTENT, in_blog);
	Page_Out(nlp_out, CO_SKEL_RSS);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_delete_rss_ask()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_delete_rss_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_sidecontent)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_add_id;
	int in_add_type;

	if (!chp_sidecontent) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツＩＤが取得できませんでした。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_sidecontent);
	divide_sidecontent2(chp_sidecontent, &in_add_id, &in_add_type);

	strcpy(cha_sql, "select T1.c_rss_title");
	strcat(cha_sql, " from at_rss T1");
	sprintf(cha_sql+strlen(cha_sql), " where n_rss_id = %d", in_add_id);
	sprintf(cha_sql+strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "RSSのタイトルを取得できませんでした。<br>");
		Db_CloseDyna(dbres);
		return 1;
	} else {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "DELETE_RSS_TITLE", chp_escape);
		free(chp_escape);
	}
	Db_CloseDyna(dbres);
	Page_Out(nlp_out, CO_SKEL_DELETE_RSS_ASK);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_delete_parts_ask()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_delete_parts_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_sidecontent)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_parts_id;

	if (!chp_sidecontent) {
		Put_Nlist(nlp_out, "ERROR", "ブログパーツＩＤが取得できませんでした。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "partsid", chp_sidecontent);
	in_parts_id = atoi(chp_sidecontent);

	strcpy(cha_sql, "select T1.c_parts_title");
	strcat(cha_sql, " from at_parts T1");
	sprintf(cha_sql+strlen(cha_sql), " where n_parts_id = %d", in_parts_id);
	sprintf(cha_sql+strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "ブログパーツのタイトルを取得できませんでした。<br>");
		Db_CloseDyna(dbres);
		return 1;
	} else {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "DELETE_PARTS_TITLE", chp_escape);
		free(chp_escape);
	}
	Db_CloseDyna(dbres);
	Page_Out(nlp_out, CO_SKEL_DELETE_PARTS_ASK);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            disp_profile_image()
 * Description:
 *      プロフィールの画像を表示する
%* ------------------------------------------------------------------------
 * Return:                      正常終了 0
 *      戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_profile_image(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[4096];

	sprintf(cha_sql, "select c_author_image,c_image_type,n_image_size from at_profile where n_blog_id=%d;", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		return 0;
	}
	if (!Db_FetchValue(dbres, 0) || !Db_FetchValue(dbres, 1) || !Db_FetchValue(dbres, 2)) {
		Db_CloseDyna(dbres);
		return 0;
	}

	fprintf(stdout, "Content-Type: %s\n\n", Db_FetchValue(dbres, 1));
	fwrite(Db_FetchValue(dbres, 0), atoi(Db_FetchValue(dbres, 2)), 1, stdout);
	Db_CloseDyna(dbres);
}

/*
+* ------------------------------------------------------------------------
 * Function:            update_profile()
 * Description:
 *      プロフィールを編集する
%* ------------------------------------------------------------------------
 * Return:                      正常終了 0
 *      戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int update_profile(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	struct stat sb;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char *chp_upload;
	char *chp_del_image;
	char *chp_new_image;
	char cha_sql[4096];
	char cha_error[512];
	int i;
	int in_error;
	int in_skip;
	unsigned long ul_len;

	/* エラーチェック */
	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "author_nickname", 1);
	if (!chp_tmp || !*chp_tmp) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Nlist(nlp_out, "ERROR", "店長のニックネームを入力してください。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "作者のニックネームを入力してください。<br>");
		}
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Nlist(nlp_out, "ERROR", "店長のニックネームは、空白以外の文字を含めて入力してください。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "作者のニックネームは、空白以外の文字を含めて入力してください。<br>");
		}
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_BLOG_AUTHOR_NICKNAME) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Format_Nlist(nlp_out, "ERROR", "店長のニックネームは、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_NICKNAME, CO_MAXLEN_BLOG_AUTHOR_NICKNAME / 2);
		} else {
			Put_Format_Nlist(nlp_out, "ERROR", "作者のニックネームは、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_NICKNAME, CO_MAXLEN_BLOG_AUTHOR_NICKNAME / 2);
		}
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "author_second_name", 1);
	if (chp_tmp && (strlen(chp_tmp) > CO_MAXLEN_BLOG_AUTHOR_SECONDNAME)) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Format_Nlist(nlp_out, "ERROR", "店長の姓は、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_SECONDNAME, CO_MAXLEN_BLOG_AUTHOR_SECONDNAME / 2);
		} else {
			Put_Format_Nlist(nlp_out, "ERROR", "作者の姓は、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_SECONDNAME, CO_MAXLEN_BLOG_AUTHOR_SECONDNAME / 2);
		}
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "author_first_name", 1);
	if (chp_tmp && (strlen(chp_tmp) > CO_MAXLEN_BLOG_AUTHOR_FIRSTNAME)) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Format_Nlist(nlp_out, "ERROR", "店長の名は、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_FIRSTNAME, CO_MAXLEN_BLOG_AUTHOR_FIRSTNAME / 2);
		} else {
			Put_Format_Nlist(nlp_out, "ERROR", "作者の名は、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_FIRSTNAME, CO_MAXLEN_BLOG_AUTHOR_FIRSTNAME / 2);
		}
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "author_notion", 1);
	if (chp_tmp && (strlen(chp_tmp) > CO_MAXLEN_BLOG_AUTHOR_SELFINTRO)) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Format_Nlist(nlp_out, "ERROR", "店長の説明は、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_SELFINTRO, CO_MAXLEN_BLOG_AUTHOR_SELFINTRO / 2);
		} else {
			Put_Format_Nlist(nlp_out, "ERROR", "作者の説明は、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_AUTHOR_SELFINTRO, CO_MAXLEN_BLOG_AUTHOR_SELFINTRO / 2);
		}
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "author_mail", 1);
	if (!chp_tmp || !*chp_tmp) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Nlist(nlp_out, "ERROR", "店長のメールアドレスを入力してください。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "作者のメールアドレスを入力してください。<br>");
		}
		in_error++;
	} else if (chp_tmp && (strlen(chp_tmp) > CO_MAXLEN_BLOG_AUTHOR_MAIL)){
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Format_Nlist(nlp_out, "ERROR", "店長のメールアドレスは、半角%d文字以内で入力してください。<br>", CO_MAXLEN_BLOG_AUTHOR_MAIL);
		} else {
			Put_Format_Nlist(nlp_out, "ERROR", "作者のメールアドレスは、半角%d文字以内で入力してください。<br>", CO_MAXLEN_BLOG_AUTHOR_MAIL);
		}
		in_error++;
	} else if (strlen(chp_tmp) > 0  && Check_Mail(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "メールアドレスに誤りがあります。<br>");
		in_error++;
	}
	if (g_in_same_database) {
		chp_tmp = Get_Nlist(nlp_in, "author_question", 1);
		if (!chp_tmp || !*chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "リマインダの質問を選択してください。<br>");
			in_error++;
		}
		chp_tmp = Get_Nlist(nlp_in, "author_answer", 1);
		if (!chp_tmp || !*chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "リマインダの答えを入力してください。<br>");
			in_error++;
		} else if (chp_tmp && (strlen(chp_tmp) > CO_MAXLEN_BLOG_REMIND_ANSWER)){
			sprintf(cha_error, "リマインダの答えは、半角%d文字、全角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_BLOG_REMIND_ANSWER, CO_MAXLEN_BLOG_REMIND_ANSWER / 2);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "author_image", 1);
	if (!chp_tmp) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			sprintf(cha_error, "店長画像のサイズは%dkB以下でお願い致します。<br>", CO_MAX_UPLOAD_SIZE);
		} else {
			sprintf(cha_error, "作者画像のサイズは%dkB以下でお願い致します。<br>", CO_MAX_UPLOAD_SIZE);
		}
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if (chp_tmp && *chp_tmp) {
		chp_tmp = Get_Nlist(nlp_in, "author_image", 3);
		if (chp_tmp && strlen(chp_tmp) == 0) {
			if (g_in_cart_mode == CO_CART_SHOPPER) {
				Put_Nlist(nlp_out, "ERROR", "店長画像に指定されたファイルが存在しません。<br>");
			} else {
				Put_Nlist(nlp_out, "ERROR", "作者画像に指定されたファイルが存在しません。<br>");
			}
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "author_image", 2);
	if (chp_tmp && !strstr(chp_tmp, "image")) {
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			Put_Nlist(nlp_out, "ERROR", "店長画像には画像ファイルを指定してください。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "作者画像には画像ファイルを指定してください。<br>");
		}
		in_error++;
	}
	if (g_in_same_database) {
		chp_tmp = Get_Nlist(nlp_in, "author_login", 1);
		if (!chp_tmp || !*chp_tmp) {
			strcpy(cha_error, "ログイン名を入力してください。<br>");
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		} else if (strlen(chp_tmp) > CO_MAXLEN_BLOG_LOGIN) {
			sprintf(cha_error, "ログイン名は、%d文字以内で入力してください。<br>", CO_MAXLEN_BLOG_LOGIN);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
		i = Duplicate_Login(db, nlp_out, in_blog, chp_tmp);
		if (i < 0) {
			in_error++;
		} else if (i) {
			sprintf(cha_error, "ログイン名「%s」はすでに使われています。他のものに変更してください。<br>", chp_tmp);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
		chp_tmp = Get_Nlist(nlp_in, "author_pass1", 1);
		chp_tmp2 = Get_Nlist(nlp_in, "author_pass1", 1);
		if (!chp_tmp || !*chp_tmp || !chp_tmp2 || !*chp_tmp2) {
			strcpy(cha_error, "パスワードを入力してください。<br>");
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		} else if (strlen(chp_tmp) > CO_MAXLEN_BLOG_PASSWORD) {
			sprintf(cha_error, "パスワードは、%d文字以内で入力してください。<br>", CO_MAXLEN_BLOG_PASSWORD);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		} else if (strcmp(chp_tmp, chp_tmp2) != 0) {
			strcpy(cha_error, "パスワードが一致しません。<br>");
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
	}
	if (in_error) {
		remove(Get_Nlist(nlp_in, "author_image", 1));
		return 1;
	}
	/* エラーチェックここまで */
	/* 登録する画像ファイル名を得る */
	ul_len = 0;
	chp_new_image = NULL;
	chp_upload = Get_Nlist(nlp_in, "author_image", 1);
	chp_del_image = Get_Nlist(nlp_in, "delete_image", 1);
	if (chp_upload && *chp_upload && !chp_del_image) {       /* 新規ファイルにする場合のみ */
		FILE* fp;
		if (stat(chp_upload, &sb) < 0) {
			Put_Nlist(nlp_out, "ERROR", "アップロードしたファイルの情報取得に失敗しました。<br>");
			remove(chp_upload);
			return 1;
		}
		fp = fopen(chp_upload, "r+b");
		if (!fp) {
			Put_Nlist(nlp_out, "ERROR", "アップロードしたファイルの情報取得に失敗しました。<br>");
			remove(chp_upload);
			return 1;
		}
		chp_tmp = malloc(sb.st_size);
		fread(chp_tmp, sb.st_size, 1, fp);
		fclose(fp);
		remove(chp_upload);
		chp_new_image = malloc(sb.st_size * 3);
		bzero(chp_new_image, sb.st_size * 3);
		ul_len = My_Db_EscapeString(db, chp_new_image, chp_tmp, sb.st_size);
		free(chp_tmp);
	}
	/* 登録する画像ファイル名ここまで */
	/* テーブルアップデート */
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		strcpy(cha_sql, "update at_profile");
		strcat(cha_sql, " set c_author_nickname = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "author_nickname", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', c_author_lastname = '");
		chp_tmp = Get_Nlist(nlp_in, "author_second_name", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "author_first_name", 1);
		strcat(cha_sql, "', c_author_firstname = '");
		if (chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
		strcat(cha_sql, "', c_author_selfintro = '");
		chp_tmp = Get_Nlist(nlp_in, "author_notion", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
		strcat(cha_sql, "', c_author_mail = '");
		chp_tmp = Get_Nlist(nlp_in, "author_mail", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
		if (g_in_same_database) {
			strcat(cha_sql, "', c_login = '");
			chp_tmp = Get_Nlist(nlp_in, "author_login", 1);
			if (chp_tmp && *chp_tmp) {
				chp_escape = My_Escape_SqlString(db, chp_tmp);
				strcat(cha_sql, chp_escape);
				free(chp_escape);
			}
			strcat(cha_sql, "', c_password = '");
			chp_tmp = Get_Nlist(nlp_in, "author_pass1", 1);
			if (chp_tmp && *chp_tmp) {
				chp_escape = My_Escape_SqlString(db, chp_tmp);
				strcat(cha_sql, chp_escape);
				free(chp_escape);
			}
			strcat(cha_sql, "', n_remind = ");
			chp_tmp = Get_Nlist(nlp_in, "author_question", 1);
			if (chp_tmp && *chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			strcat(cha_sql, ", c_remind = '");
			chp_tmp = Get_Nlist(nlp_in, "author_answer", 1);
			if (chp_tmp && *chp_tmp) {
				chp_escape = My_Escape_SqlString(db, chp_tmp);
				strcat(cha_sql, chp_escape);
				free(chp_escape);
			}
		}
		strcat(cha_sql, "', b_send_mail = ");
		chp_tmp = Get_Nlist(nlp_in, "author_send", 1);
		if (chp_tmp && *chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		/* 削除にチェックを入れたら消してしまう仕様 */
		if (chp_del_image && *chp_del_image) {
			strcat(cha_sql, ", c_author_image = ''");
		}
		sprintf(cha_sql+strlen(cha_sql), " where n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)){
			remove(chp_upload);
			Put_Nlist(nlp_out, "ERROR", "プロフィール登録するクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		chp_upload = Get_Nlist(nlp_in, "author_image", 1);
		if ((!chp_del_image || !*chp_del_image) && chp_upload && *chp_upload && chp_new_image) {
			chp_tmp = malloc(ul_len + 256);
			strcpy(chp_tmp, "update at_profile set c_author_image = '");
			strcat(chp_tmp, chp_new_image);
			sprintf(chp_tmp + strlen(chp_tmp), "' where n_blog_id = %d", in_blog);
			free(chp_new_image);
			if (Db_ExecSql(db, chp_tmp)) {
				free(chp_tmp);
				Put_Nlist(nlp_out, "ERROR", "プロフィール登録するクエリに失敗しました。<br>");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
			free(chp_tmp);
			chp_upload = Get_Nlist(nlp_in, "author_image", 2);
			sprintf(cha_sql, "update at_profile set c_image_type = '%s', n_image_size = %d where n_blog_id = %d", chp_upload ? chp_upload : "application/octed-stream", (int)sb.st_size, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				free(chp_tmp);
				Put_Nlist(nlp_out, "ERROR", "プロフィール登録するクエリに失敗しました。<br>");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
		/* 作者名が変わるのでRSS組直し */
		if (Create_Rss(db, nlp_out, in_blog)) {
			return 1;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "from_top", 1);
	chp_tmp2 = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_tmp2 && *chp_tmp2) {
		if (g_in_dbb_mode) {
			printf("Location: %s%s%s/%s/\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			printf("Location: %s%s%s/%08d/\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			printf("Location: %s%s%s/%s?bid=%d\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
	} else if (chp_tmp && *chp_tmp) {
		printf("Location: %s%s%s/%s?blogid=%d\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
	} else if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_archive()
 * Description:
 *	コメント･トラックバック･月間アーカイブの設定をする
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int update_archive(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char *chp_iteminfo;
	char *chp_itemnum;
	char cha_sql[512];
	char cha_error[256];
	char cha_comment_value[8];
	char cha_tb_value[8];
	char cha_month_value[8];
	char cha_new_value[8];
	int in_cnt;
	int in_error;
	int in_japanese;
	int in_skip;

	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "archive_comment", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "最新コメントの件数を入力してください。<br>");
		in_error++;
	} else if (Check_Numeric(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "最新コメントの件数には半角数字を入力してください。<br>");
		in_error++;
	} else if (atoi(chp_tmp) > CO_MAX_ARCHIVE_COMMENT || atoi(chp_tmp) < 1) {
		sprintf(cha_error, "最新コメントの件数は、1件以上%d件以下で入力してください。<br>"
			, CO_MAX_ARCHIVE_COMMENT);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else {
		strcpy(cha_comment_value, chp_tmp);
	}

	chp_tmp = Get_Nlist(nlp_in, "archive_trackback", 1);
	if (!chp_tmp || !*chp_tmp) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "最新引用記事の件数を入力してください。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "最新トラックバックの件数を入力してください。<br>");
		}
		in_error++;
	} else if (Check_Numeric(chp_tmp)) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "最新引用記事の件数には半角数字を入力してください。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "最新トラックバックの件数には半角数字を入力してください。<br>");
		}
		in_error++;
	} else if (atoi(chp_tmp) < 1 || atoi(chp_tmp) > CO_MAX_ARCHIVE_TRACKBACK) {
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "ERROR", "最新引用記事の件数は、1件以上%d件以下で入力してください。<br>", CO_MAX_ARCHIVE_TRACKBACK);
		} else {
			Put_Format_Nlist(nlp_out, "ERROR", "最新トラックバックの件数は、1件以上%d件以下で入力してください。<br>", CO_MAX_ARCHIVE_TRACKBACK);
		}
		in_error++;
	} else {
		strcpy(cha_tb_value, chp_tmp);
	}

	chp_tmp = Get_Nlist(nlp_in, "range_type", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "期間のラジオボタンを選択してください。<br>");
		in_error++;
	} else if (atoi(chp_tmp) == 1) {
		chp_tmp = Get_Nlist(nlp_in, "archive_month", 1);
		if (!chp_tmp || !*chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "表示する年数を入力してください。<br>");
			in_error++;
		} else if (Check_Numeric(chp_tmp)) {
			Put_Nlist(nlp_out, "ERROR", "表示する年数には半角数字を入力してください。<br>");
			in_error++;
		} else if (atoi(chp_tmp) < 1 || atoi(chp_tmp) > CO_MAX_ARCHIVE_MONTHLY){
			sprintf(cha_error, "表示する年数は、1年以上%d年以下で入力してください。<br>"
				, CO_MAX_ARCHIVE_MONTHLY);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		} else {
			strcpy(cha_month_value, chp_tmp);
		}
	} else {
		strcpy(cha_month_value, chp_tmp);
	}

	chp_tmp = Get_Nlist(nlp_in, "archive_newentry", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "最新記事の件数を入力してください。<br>");
		in_error++;
	} else if (Check_Numeric(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "最新記事の件数には半角数字を入力してください。<br>");
		in_error++;
	} else if (atoi(chp_tmp) < 1 || atoi(chp_tmp) > CO_MAX_ARCHIVE_NEWENTRY) {
		sprintf(cha_error, "最新記事の件数は、1件以上%d件以下で入力してください。<br>"
			  , CO_MAX_ARCHIVE_NEWENTRY);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else {
		strcpy(cha_new_value, chp_tmp);
	}

	chp_iteminfo = Get_Nlist(nlp_in, "item_info", 1);
	chp_itemnum = Get_Nlist(nlp_in, "items_number", 1);
	in_cnt = atoi(chp_itemnum ? chp_itemnum : "3");
	if (in_cnt < 1 || in_cnt > 9) {
		Put_Nlist(nlp_out, "ERROR", "表示する商品数は 1 〜 9 を指定してください。<br>");
		in_error++;
	}

	if (in_error) {
		return 1;
	}

	chp_tmp = Get_Nlist(nlp_in, "japanese_calendar", 1);
	if (!chp_tmp || !chp_tmp[0]) {
		in_japanese = 0;
	} else {
		in_japanese = atoi(chp_tmp);
	}
	if (Begin_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
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
		sprintf(cha_sql, "update at_profile set b_japanese_calendar = %d where n_blog_id = %d", in_japanese, in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}

		strcpy(cha_sql, "update at_archive");
		strcat(cha_sql, " set n_archive_value = ");
		strcat(cha_sql, cha_comment_value);
		sprintf(cha_sql + strlen(cha_sql), " where n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_COMMENT);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}

		strcpy(cha_sql, "update at_archive");
		strcat(cha_sql, " set n_archive_value = ");
		strcat(cha_sql, cha_tb_value);
		sprintf(cha_sql + strlen(cha_sql), " where n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_TRACKBACK);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}

		strcpy(cha_sql, "update at_archive");
		strcat(cha_sql, " set n_archive_value = ");
		strcat(cha_sql, cha_month_value);
		sprintf(cha_sql + strlen(cha_sql), " where n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_MONTHLY);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}

		strcpy(cha_sql, "update at_archive");
		strcat(cha_sql, " set n_archive_value = ");
		strcat(cha_sql, cha_new_value);
		sprintf(cha_sql + strlen(cha_sql), " where n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_NEWENTRY);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}

		if (g_in_cart_mode == CO_CART_SHOPPER) {
			sprintf(cha_sql,
				" update at_blog"
				" set n_iteminfo_id = %s"
				",n_iteminfo_num = %s"
				" where n_blog_id = %d",
				chp_iteminfo ? chp_iteminfo : "1",
				chp_itemnum ? chp_itemnum : "3",
				in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				return 1;
			}
		}
	}

	if (Commit_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションのコミットに失敗しました<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}

	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_tmp && *chp_tmp) {
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s?bid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
	} else {
		Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s?blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
	}
	Put_Nlist(nlp_out, "MESSAGE", "表示に関する詳細設定を変更しました。");
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

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            disp_page_linklist_title_edit()
 * Description:
 *      リンクリスト新規追加、編集画面を表示
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時(直前に戻る) 1
 *	エラー時(トップに戻る) 2
-* ------------------------------------------------------------------------*/
static int disp_page_linklist_title_edit(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_linkitem;
	char *chp_escape;
	char cha_sql[512];
	int in_group;
	int in_link_id;
	int in_link_type;

	in_group = 0;
	chp_tmp = Get_Nlist(nlp_in, "linkgroup_mode", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "linkgroup_mode", chp_tmp);
		in_group = 1;
	}
	chp_linkitem = Get_Nlist(nlp_in, "linkitem", 1);
	if (chp_linkitem) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "linkitem", chp_linkitem);
	}
	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_tmp);
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_link_id, &in_link_type);
	chp_tmp = Get_Nlist(nlp_in, "linklist_title", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "LINKLIST_TITLE", chp_escape);
		free(chp_escape);
	} else {
		strcpy(cha_sql, "select T1.c_sidecontent_title");
		strcat(cha_sql, " from at_sidecontent T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_sidecontent_id = %d", in_link_id);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_sidecontent_type = %d", in_link_type);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "リンク集の有無を確認するクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		if (Db_GetRowCount(dbres) == 0) {
			Put_Nlist(nlp_out, "ERROR", "選択されたリンク集は既に削除されています。<br>");
			Db_CloseDyna(dbres);
			return 2;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "LINKLIST_TITLE", chp_escape);
			free(chp_escape);
		}
		Db_CloseDyna(dbres);
	}
	Put_Nlist(nlp_out, "MODE_LINKLIST_TITLE", "リンク集のタイトルの編集");
	Put_Nlist(nlp_out, "BUTTON_LINKLIST_TITLE", "reload.gif");
	Put_Nlist(nlp_out, "BTN_NAME", "BTN_EDIT_LINKLIST_TITLE");
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "RESET_LOC", "%s?BTN_DISP_LINKLIST_TITLE_EDIT=1&blogid=%d&linkgroup_mode=%d&linkitem=%s&sidecontent=%d:%d%s",
		CO_CGI_SIDECONTENT, in_blog, in_group, chp_linkitem ? chp_linkitem : "0", in_link_id, in_link_type, chp_tmp ? "&from_user=1" : "");
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_SETTING_LINKLIST=1&blogid=%d", CO_CGI_SIDECONTENT, in_blog);
	Page_Out(nlp_out, CO_SKEL_LINKLIST_TITLE);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_linklist_new()
 * Description:
 *	新規リンクリスト編集画面を表示
 *	初回のみ。1度でも追加すると別関数になる。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_linklist_new(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_url;
	char *chp_escape;
	char cha_sql[512];
	int in_linkgroup_id;

	Put_Nlist(nlp_out, "LINKLIST_MODE", "新規追加");
	strcpy(cha_sql, "select max(n_sidecontent_id) from at_sidecontent");
	sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_type = %d", CO_SIDECONTENT_ARCHIVE_LINKS);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "IDを取るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "ID設定に失敗しました。<br>");
		return 1;
	}
	in_linkgroup_id = atoi(chp_tmp);
	sprintf(cha_sql, "%s:%d", chp_tmp, CO_SIDECONTENT_ARCHIVE_LINKS);
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", cha_sql);
	Db_CloseDyna(dbres);
	/* リンク集のタイトル。前画面で登録したのをそのまま出せばよい。 */
	chp_url = NULL;
	chp_tmp = Get_Nlist(nlp_in, "linklist_title", 1);
	if (chp_tmp && *chp_tmp) {
		Encode_Url(chp_tmp, &chp_url);
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "linklist_title", chp_escape);
		Put_Nlist(nlp_out, "LINKLIST_TITLE", chp_escape);
		free(chp_escape);
	}
	/* リンク集の内容は新規なのでまだない */
	Put_Nlist(nlp_out, "LINKLIST", "<select name=\"linkitem\" size=\"15\" disabled><option value=\"1\" selected>　　　　　　　　</select>\n");
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "RETURN", "%s?BTN_NEW_LINKITEM.x=1&blogid=%d&linklist_title=%s%s", CO_CGI_SIDECONTENT, in_blog, chp_url ? chp_url : "", chp_tmp ? "&from_user=1" : "");
	Page_Out(nlp_out, CO_SKEL_LINKLIST);
	if (chp_url)
		free(chp_url);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_linklist_edit()
 * Description:
 *	リンクリスト編集画面を表示。エラー復帰もこちら。
 *	新規リンクリストの場合、一度でもリンクの追加をした後はこちらの関数になる。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時(直後に戻る) 1
 *	(トップに戻る) 2
-* ------------------------------------------------------------------------*/
static int disp_page_linklist_edit(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char *chp_selected;
	char cha_sql[512];
	int in_linkgroup_id;
	int in_linkgroup_type;
	int in_count;

	/* 文言切り分け */
	if (Get_Nlist(nlp_in, "BTN_DISP_EDIT_LINK", 1) || Get_Nlist(nlp_in, "linkgroup_mode", 1)) {
		Put_Nlist(nlp_out, "LINKLIST_MODE", "編集");
		Build_HiddenEncode(nlp_out, "HIDDEN", "linkgroup_mode", "1");
	} else {
		Put_Nlist(nlp_out, "LINKLIST_MODE", "新規追加");
	}
	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツIDを取得できませんでした。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_tmp);
	divide_sidecontent(nlp_in, "sidecontent", &in_linkgroup_id, &in_linkgroup_type);
	/* リンク集のタイトル */
	strcpy(cha_sql, "select T1.c_sidecontent_title");
	strcat(cha_sql, " from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_sidecontent_id = %d", in_linkgroup_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", CO_SIDECONTENT_ARCHIVE_LINKS);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "リンク集のタイトルを得るクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	if (Db_GetRowCount(dbres) == 0) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "選択されたリンク集は既に削除されています。<br>");
		return 2;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "linklist_title", chp_escape);
		Put_Nlist(nlp_out, "LINKLIST_TITLE", chp_escape);
		free(chp_escape);
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select count(*) from at_linkitem where n_linkgroup_id = %d and n_blog_id = %d", in_linkgroup_id, in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "リンク集アイテムの個数を得るクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンク集アイテムの個数の取得に失敗しました。<br>");
		return 1;
	}
	if (atoi(chp_tmp) >= CO_MAX_LINK_ITEM) {
		Put_Nlist(nlp_out, "ADDDISABLE", "disabled");
	}
	Db_CloseDyna(dbres);

	/* リンク集の内容 */
	strcpy(cha_sql, "select T1.n_linkitem_id");
	strcat(cha_sql, ", T1.c_link_title");
	strcat(cha_sql, " from at_linkitem T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_linkgroup_id = %d", in_linkgroup_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_linkitem_order");
	chp_selected = Get_Nlist(nlp_in, "linkitem", 1);
	if (chp_selected && *chp_selected) {
		Build_MultiComboDb(OldDBase(db), nlp_out, "LINKLIST", "linkitem", cha_sql, 15, chp_selected);
	} else{
		Build_MultiComboDb(OldDBase(db), nlp_out, "LINKLIST", "linkitem", cha_sql, 15, "");
	}
	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "指定リンク集の取得に失敗しました。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_linkgroup_id, &in_linkgroup_type);
	in_count = Get_Count_Link_In_Linklist(db, nlp_in, nlp_out, in_linkgroup_id, in_blog);
	if (in_count == CO_ERROR) {
		Put_Nlist(nlp_out, "ERROR", "リンクの総数の取得に失敗しました。<br>");
		return 1;
	/* リンクが１つ以上ある場合 */
	} else if (in_count > 0) {
		Put_Nlist(nlp_out, "BUTTON_LINKLIST", "<tr>\n");
		Put_Format_Nlist(nlp_out, "BUTTON_LINKLIST", "<td style=\"padding:0 0 10 0;\"><input type=\"image\" src=\"%s/link_henshu_b.gif\" value=\"編集する\" name=\"BTN_DISP_EDIT_LINKITEM\"></td>\n", g_cha_admin_image);
		Put_Nlist(nlp_out, "BUTTON_LINKLIST", "</tr>\n");
		Put_Nlist(nlp_out, "BUTTON_LINKLIST", "<tr>\n");
		Put_Format_Nlist(nlp_out, "BUTTON_LINKLIST", "<td style=\"padding:0 0 10 0;\"><input type=\"image\" src=\"%s/link_sakujo_b.gif\" value=\"削除する\" name=\"BTN_DISP_DELETE_LINKITEM_ASK\"></td>\n", g_cha_admin_image);
		Put_Nlist(nlp_out, "BUTTON_LINKLIST", "</tr>\n");
	}
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "RETURN", "%s?BTN_DISP_SETTING_LINKLIST=1&blogid=%d%s", CO_CGI_SIDECONTENT, in_blog, chp_tmp ? "&from_user=1" : "");
	Page_Out(nlp_out, CO_SKEL_LINKLIST);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_linkitem_new()
 * Description:
 *	リンクの新規追加画面
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
 *	トップに戻るエラー 2
-* ------------------------------------------------------------------------*/
static int disp_page_linkitem_new(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_group;
	int in_link_id;
	int in_link_type;

	/*リンク集ページのmode引き継ぎ*/
	/* 埋まっているときはリンク集の編集。 */
	in_group = 0;
	if (Get_Nlist(nlp_in, "linkgroup_mode", 1)) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "linkgroup_mode", "1");
		in_group = 1;
	}
	/* リンク集ID(=サイドコンテンツID) */
	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンク集IDを取得できませんでした。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_link_id, &in_link_type);
	strcpy(cha_sql, "select T1.n_sidecontent_id");
	strcat(cha_sql, " from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_sidecontent_id = %d", in_link_id);
	sprintf(cha_sql+strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "リンク集の有無を確認するクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	if (Db_GetRowCount(dbres) == 0) {
		Put_Nlist(nlp_out, "ERROR", "選択されたリンク集は既に削除されています。<br>");
		Db_CloseDyna(dbres);
		return 2;
	}
	Db_CloseDyna(dbres);
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_tmp);
	/* エラー時復帰 */
	if (Get_Nlist(nlp_in, "from_error", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "linkitem_title", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "LINKITEM_TITLE", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "linkitem_url", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "LINKITEM_URL", chp_escape);
			free(chp_escape);
		}
		chp_tmp = Get_Nlist(nlp_in, "linkitem_target", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "LINKITEM_TARGET", CO_CHECKED);
		}
	/* エラー復帰でない */
	} else {
		Put_Nlist(nlp_out, "LINKITEM_URL", "http://");
		Put_Nlist(nlp_out, "LINKITEM_TARGET", CO_CHECKED);
	}
	/* 共通 */
	Put_Nlist(nlp_out, "BTN_NAME", "BTN_ADD_LINKITEM");
	Put_Nlist(nlp_out, "MODE_LINKITEM", "新規追加");
	Put_Nlist(nlp_out, "BUTTON_LINKITEM", "link_tuika_m.gif");
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "RESET_LOC", "%s?BTN_DISP_ADD_LINKITEM.x=1&blogid=%d&sidecontent=%d:%d&linkgroup_mode=%d%s",
		CO_CGI_SIDECONTENT, in_blog, in_link_id, in_link_type, in_group, chp_tmp ? "&from_user=1" : "");
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_EDIT_LINK=1&blogid=%d&sidecontent=%d:%d%s",
		CO_CGI_SIDECONTENT, in_blog, in_link_id, in_link_type, chp_tmp ? "&from_user=1" : "");
	Page_Out(nlp_out, CO_SKEL_LINKITEM);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_linkitem_edit()
 * Description:
 *	リンクの編集画面表示
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_linkitem_edit(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char *chp_sidecontent_id;
	char *chp_linkitem_id;
	char *chp_linkitem_title;
	char *chp_linkitem_url;
	char *chp_linkitem_target;
	char cha_sql[512];
	int in_group;
	int in_link_id;
	int in_link_type;

	/* リンクアイテム */
	chp_linkitem_id = Get_Nlist(nlp_in, "linkitem", 1);
	if (!chp_linkitem_id || !*chp_linkitem_id) {
		Put_Nlist(nlp_out, "ERROR", "編集するリンクを選択してください。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "linkitem", chp_linkitem_id);
	/* リンク集ID(=サイドコンテンツID) */
	chp_sidecontent_id = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_sidecontent_id || !*chp_sidecontent_id) {
		Put_Nlist(nlp_out, "ERROR", "リンク集IDを取得できませんでした。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_sidecontent_id);
	divide_sidecontent(nlp_in, "sidecontent", &in_link_id, &in_link_type);
	/*リンク集ページのmode引き継ぎ*/
	/* 埋まっているときはリンク集の編集。 */
	in_group = 0;
	if (Get_Nlist(nlp_in, "linkgroup_mode", 1)) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "linkgroup_mode", "1");
		in_group = 1;
	}
	Put_Nlist(nlp_out, "BTN_NAME", "BTN_EDIT_LINKITEM");
	Put_Nlist(nlp_out, "MODE_LINKITEM", "編集");
	Put_Nlist(nlp_out, "BUTTON_LINKITEM", "kettei.gif");
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "RESET_LOC", "%s?BTN_DISP_EDIT_LINKITEM=1&blogid=%d&linkitem=%s&sidecontent=%d:%d&linkgroup_mode=%d%s",
		CO_CGI_SIDECONTENT, in_blog, chp_linkitem_id, in_link_id, in_link_type, in_group, chp_tmp ? "&from_user=1" : "");
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_EDIT_LINK=1&blogid=%d&sidecontent=%d:%d%s",
		CO_CGI_SIDECONTENT, in_blog, in_link_id, in_link_type, chp_tmp ? "&from_user=1" : "");
	/* 表示 */
	strcpy(cha_sql, "select T1.c_link_title");	/* 0 リンクタイトル */
	strcat(cha_sql, ", T1.c_link_url");		/* 1 リンクURL */
	strcat(cha_sql, ", T1.b_link_target");		/* 2 ターゲット */
	strcat(cha_sql, " from at_linkitem T1");
	strcat(cha_sql, " where T1.n_linkitem_id = ");
	strcat(cha_sql, chp_linkitem_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_linkgroup_id = %d", in_link_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "編集するリンクの情報取得に失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		Put_Nlist(nlp_out, "ERROR", "選択されたリンクは既に削除されています。<br>");
		Db_CloseDyna(dbres);
		return 1;
	}
	if (Get_Nlist(nlp_in, "from_error", 1)) {
		chp_linkitem_title = Get_Nlist(nlp_in, "linkitem_title", 1);
		chp_linkitem_url = Get_Nlist(nlp_in, "linkitem_url", 1);
		chp_linkitem_target = Get_Nlist(nlp_in, "linkitem_target", 1);
	} else {
		chp_linkitem_title = Db_GetValue(dbres, 0, 0);
		chp_linkitem_url = Db_GetValue(dbres, 0, 1);
		chp_linkitem_target = Db_GetValue(dbres, 0, 2);
	}
	if (chp_linkitem_title && *chp_linkitem_title) {
		chp_escape = Escape_HtmlString(chp_linkitem_title);
		Put_Nlist(nlp_out, "LINKITEM_TITLE", chp_escape);
		free(chp_escape);
	}
	if (chp_linkitem_url && *chp_linkitem_url) {
		chp_escape = Escape_HtmlString(chp_linkitem_url);
		Put_Nlist(nlp_out, "LINKITEM_URL", chp_escape);
		free(chp_escape);
	}
	if (chp_linkitem_target && atoi(chp_linkitem_target) == 1) {
		Put_Nlist(nlp_out, "LINKITEM_TARGET", CO_CHECKED);
	}
	Db_CloseDyna(dbres);
	Page_Out(nlp_out, CO_SKEL_LINKITEM);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_access()
 * Description:
 *	アクセスカウンタの更新
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
static int update_access(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	int in_error;
	int in_skip;
	char cha_sql[512];
	char *chp_pv_all;
	char *chp_uu_all;
	char *chp_tmp;

	chp_pv_all = Get_Nlist(nlp_in, "pv_all", 1);
	chp_uu_all = Get_Nlist(nlp_in, "uu_all", 1);

	in_error = 0;
	if (!chp_pv_all || !chp_pv_all[0]) {
		Put_Nlist(nlp_out, "ERROR", "ページビュー(昨日までの累計)を入力してください。<br>");
		++in_error;
	} else if (Check_Numeric(chp_pv_all)) {
		Put_Nlist(nlp_out, "ERROR", "ページビュー(昨日までの累計)は数字を入力してください。<br>");
		++in_error;
	}
	if (!chp_uu_all || !chp_uu_all[0]) {
		Put_Nlist(nlp_out, "ERROR", "ユニークユーザー(昨日までの累計)を入力してください。<br>");
		++in_error;
	} else if (Check_Numeric(chp_uu_all)) {
		Put_Nlist(nlp_out, "ERROR", "ユニークユーザー(昨日までの累計)は数字を入力してください。<br>");
		++in_error;
	}
	if (in_error)
		return in_error;

	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		if (Begin_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			return 1;
		}
		sprintf(cha_sql,
			" update at_profile set"
			" n_page_view = %d"
			",n_unique_user = %d"
			" where n_blog_id = %d",
			atoi(chp_pv_all), atoi(chp_uu_all), in_blog);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "アクセス数をクリアすクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Put_Nlist(nlp_out, "QUERY", "<br>");
			Put_Nlist(nlp_out, "QUERY", cha_sql);
			return 1;
		}
		sprintf(cha_sql, "delete from at_view where n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "アクセス数をクリアすクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Put_Nlist(nlp_out, "QUERY", "<br>");
			Put_Nlist(nlp_out, "QUERY", cha_sql);
			return 1;
		}
		if (Commit_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "トランザクションのコミットに失敗しました<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}

	sprintf(cha_sql, "%s?BTN_DISP_SETTING_SIDECONTENT=1&blogid=%d&sidecontent=%d:%d",
		CO_CGI_SIDECONTENT, in_blog, CO_SIDECONTENT_ACCESS_LOG, CO_SIDECONTENT_ACCESS_LOG);
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_tmp) {
		sprintf(cha_sql + strlen(cha_sql), "&from_user=%s", chp_tmp);
	}
	Put_Nlist(nlp_out, "MESSAGE", "アクセス数をクリアしました。");
	Put_Nlist(nlp_out, "RETURN", cha_sql);
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

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	change_order_sidecontent()
 * Description:
 *	サイドコンテンツの順番を変更
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
static int exchange_sidecontent_order(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_where1[128];
	char cha_where2[128];
	char cha_num[32];
	char *chp_tmp;
	int in_error;
	int in_id;
	int in_type;
	int in_skip;

	in_error = 0;
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
		if (chp_tmp && *chp_tmp) {
			divide_sidecontent(nlp_in, "sidecontent", &in_id, &in_type);
			sprintf(cha_where1, CO_SIDECONTENT_WHERE_ETC1, in_blog, in_type);
			sprintf(cha_where2, CO_SIDECONTENT_WHERE_ETC2, in_blog);
			sprintf(cha_num, "%d", in_id);
			if (Get_Nlist(nlp_in, "BTN_UP_SIDECONTENT.x", 1)) {
				if (Change_Order_Db_Blog(OldDBase(db), cha_num, CO_SIDECONTENT_NAME_ID, CO_SIDECONTENT_NAME_SEQ, CO_SIDECONTENT_NAME_TABLE, cha_where1, cha_where2, CO_MODE_UP)) {
					in_error++;
				}
			} else if (Get_Nlist(nlp_in, "BTN_DOWN_SIDECONTENT.x", 1)) {
				if (Change_Order_Db_Blog(OldDBase(db), cha_num, CO_SIDECONTENT_NAME_ID, CO_SIDECONTENT_NAME_SEQ, CO_SIDECONTENT_NAME_TABLE, cha_where1, cha_where2, CO_MODE_DOWN)) {
					in_error++;
				}
			} else if (Get_Nlist(nlp_in, "BTN_TOP_SIDECONTENT.x", 1)) {
				if (Change_Order_Db_Blog(OldDBase(db), cha_num, CO_SIDECONTENT_NAME_ID, CO_SIDECONTENT_NAME_SEQ, CO_SIDECONTENT_NAME_TABLE, cha_where1, cha_where2, CO_MODE_TOP)) {
					in_error++;
				}
			} else if (Get_Nlist(nlp_in, "BTN_BOTTOM_SIDECONTENT.x", 1)) {
				if (Change_Order_Db_Blog(OldDBase(db), cha_num, CO_SIDECONTENT_NAME_ID, CO_SIDECONTENT_NAME_SEQ, CO_SIDECONTENT_NAME_TABLE, cha_where1, cha_where2, CO_MODE_BOTTOM)) {
					in_error++;
				}
			}
		}
		if (in_error) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツの順番の入れ替えに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
	}
	if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	un_dispaly_sidecontent()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int un_display_sidecontent(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char *chp_sidecontent;
	char cha_sql[512];
	int in_content_id;
	int in_content_type;
	int in_skip;

	chp_sidecontent = Get_Nlist(nlp_in, "sidecontent", 1);
	if (chp_sidecontent && *chp_sidecontent) {
		divide_sidecontent(nlp_in, "sidecontent", &in_content_id, &in_content_type);
		if (g_in_need_login && in_content_type == CO_SIDECONTENT_ADMIN_MENU) {
			Put_Nlist(nlp_out, "ERROR", "「管理者メニュー」は非表示にできません。<br>");
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
			if (Begin_Transact(db)) {
				Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				return 1;
			}
			if (shift_sidecontent_order(db, nlp_in, nlp_out, in_blog)) {
				Rollback_Transact(db);
				return 1;
			}
			chp_tmp = strchr(chp_sidecontent, ':');
			if (chp_tmp)
				*chp_tmp = '\0';
			strcpy(cha_sql, "update at_sidecontent");
			strcat(cha_sql, " set b_display = 0");
			sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_id = %d", in_content_id);
			sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_content_type);
			sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "非表示にするクエリーに失敗しました。<br>");
				Put_Nlist(nlp_out, "QUERY", cha_sql);
				Rollback_Transact(db);
				return 1;
			}
			if (Commit_Transact(db)) {
				Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				return 1;
			}
		}
	}
	/* 再表示 */
	if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)){
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ設定ページの表示に失敗しました。<br>");
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            rss_error_check()
 * Description:
 *	RSSアーカイブのエラーチェック
%* ------------------------------------------------------------------------
 * Return:
 *	正常 0
 *	エラー時 1以上
-* ------------------------------------------------------------------------*/
static int rss_error_check(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chp_tmp;
	char cha_error[512];
	int in_error;

	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "rss_title", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "RSSのタイトルを入力してください。<br>");
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "RSSのタイトルを入力してください。<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_RSS_TITLE) {
		sprintf(cha_error, "RSSのタイトルは半角%d文字(全角%d文字)以内で入力してください。<br>"
			, CO_MAXLEN_RSS_TITLE ,CO_MAXLEN_RSS_TITLE / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "rss_url", 1);
	if (!chp_tmp || !*chp_tmp || strcmp(chp_tmp, "http://") == 0) {
		Put_Nlist(nlp_out, "ERROR", "RSSのURLを入力してください。<br>");
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "RSSのURLを入力してください。<br>");
		in_error++;
	} else {
		if (strlen(chp_tmp) > CO_MAXLEN_RSS_URL) {
			sprintf(cha_error, "RSSのURLは半角%d文字以内で入力してください。<br>", CO_MAXLEN_RSS_URL);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
		if (Check_URL(chp_tmp)) {
			Put_Nlist(nlp_out, "ERROR", "RSSのURLに誤りがあります。確認してください。<br>");
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "rss_article", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "表示件数を入力してください。<br>");
		in_error++;
	} else if (Check_Numeric(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "表示件数は半角数字で入力してください。<br>");
		in_error++;
	} else if (atoi(chp_tmp) > CO_MAX_RSS_ARTICLE || atoi(chp_tmp) < 1) {
		sprintf(cha_error, "表示件数は1件以上%d件以下で入力してください。<br>", CO_MAX_RSS_ARTICLE);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	add_rss()
 * Description:
 *	サイドコンテンツへRSSを追加＆画面再描画
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int add_rss(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_escape;
	char *chp_tmp;
	char *chp_manual;
	char *chp_target;
	char cha_rss_title[CO_MAXLEN_RSS_TITLE + 1];
	char cha_rss_url[CO_MAXLEN_RSS_URL + 1];
	char cha_rss_article[4];	/* 数値のため */
	char cha_sql[512];
	int in_rss_id;
	int in_sidecontent_order;
	int in_skip;

	chp_manual = Get_Nlist(nlp_in, "manualselect", 1);
	if (!chp_manual || !*chp_manual) {
		Put_Nlist(nlp_out, "ERROR", "追加方法を選択してください。<br>");
		return 1;
	}
	if (atoi(chp_manual) == 1) {
		if (rss_error_check(nlp_in, nlp_out)) {
			return 1;
		}
		strcpy(cha_rss_title, Get_Nlist(nlp_in, "rss_title", 1));
		strcpy(cha_rss_url, Get_Nlist(nlp_in, "rss_url", 1));
		strcpy(cha_rss_article, Get_Nlist(nlp_in, "rss_article", 1));
		chp_target = Get_Nlist(nlp_in, "rss_target1", 1);
	} else {
		chp_tmp = Get_Nlist(nlp_in, "prepared_rss_id", 1);
		if (!chp_tmp || !*chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "RSSリンクを選択してください。<br>");
			return 1;
		}
		/* テーブルから選択 */
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "select T1.c_rss_title");	/* 0 RSSタイトル */
		strcat(cha_sql, ", T1.c_rss_url");		/* 1 RSSURL */
		strcat(cha_sql, " from sy_rss T1");
		strcat(cha_sql, " where T1.n_rss_id =");
		strcat(cha_sql, chp_tmp);
		dbres = Db_OpenDyna(db, cha_sql);
		sprintf(cha_sql+strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "用意されたRSSを得るクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		if (!Db_GetRowCount(dbres)) {
			Put_Nlist(nlp_out, "ERROR", "予期せぬ値が指定されました。<br>");
			Db_CloseDyna(dbres);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp && *chp_tmp) {
			strcpy(cha_rss_title, chp_tmp);
		} else {
			cha_rss_title[0] = '\0';
		}
		chp_tmp = Db_GetValue(dbres, 0, 1);
		if (chp_tmp && *chp_tmp) {
			strcpy(cha_rss_url, chp_tmp);
		} else {
			cha_rss_url[0] = '\0';
		}
		sprintf(cha_rss_article, "%d", CO_DEFAULT_RSS);
		chp_target = Get_Nlist(nlp_in, "rss_target0", 1);
	}

	sprintf(cha_sql, "select count(*) from at_sidecontent where n_blog_id = %d and n_sidecontent_type = %d", in_blog, CO_SIDECONTENT_ARCHIVE_RSS);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するRSS取り込み数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "表示するRSS取り込み数の取得に失敗しました。<br>");
		return 1;
	}
	if (atoi(chp_tmp) >= CO_MAX_RSS_IMPORTS) {
		Put_Format_Nlist(nlp_out, "ERROR", "RSS取り込みは全部で %d 件までしか登録できません。<br>", CO_MAX_RSS_IMPORTS);
		Db_CloseDyna(dbres);
		return 1;
	}
	Db_CloseDyna(dbres);

	if (Begin_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}

	sprintf(cha_sql,
		"select coalesce(max(T1.n_rss_id) + 1, 1)"
		" from at_rss T1"
		" where T1.n_blog_id = %d for update", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ登録クエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_rss_id = atoi(chp_tmp);
	} else {
		in_rss_id = 1;
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
		strcpy(cha_sql, "insert into at_rss");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_rss_id");
		strcat(cha_sql, ",c_rss_title");
		strcat(cha_sql, ",c_rss_url");
		strcat(cha_sql, ",n_rss_article");
		strcat(cha_sql, ",b_rss_target");
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d", in_blog, in_rss_id);
		strcat(cha_sql, ", '");
		chp_escape = My_Escape_SqlString(db, cha_rss_title);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', '");
		chp_escape = My_Escape_SqlString(db, cha_rss_url);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', ");
		strcat(cha_sql, cha_rss_article);
		strcat(cha_sql, ", ");
		if (chp_target && *chp_target) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ")");
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "RSS登録クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		sprintf(cha_sql,
			"select coalesce(max(T2.n_sidecontent_order), 0) + 1"
			" from at_sidecontent T2"
			" where T2.n_blog_id = %d for update", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ登録クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_sidecontent_order = atoi(chp_tmp);
		} else {
			in_sidecontent_order = 1;
		}
		Db_CloseDyna(dbres);

		/* こちらはサブクエリ使用。注意。 */
		strcpy(cha_sql, "insert into at_sidecontent");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_sidecontent_id");
		strcat(cha_sql, ",n_sidecontent_order");
		strcat(cha_sql, ",n_sidecontent_type");
		strcat(cha_sql, ",c_sidecontent_title");
		strcat(cha_sql, ",b_display");
		strcat(cha_sql, ",b_allow_outer");
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d, %d, %d", in_blog, in_rss_id, in_sidecontent_order, CO_SIDECONTENT_ARCHIVE_RSS);
		strcat(cha_sql, ", '");
		chp_escape = My_Escape_SqlString(db, cha_rss_title);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', 1, 1)");
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ登録クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	if (Commit_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	/* 改めてセレクトし、選択させておくために埋める */
	/* 取れなくても、選択されてない状態になるだけなので無視 */
	strcpy(cha_sql, "select max(T1.n_rss_id)");	/* 0 今登録したRSSのID */
	strcat(cha_sql, " from at_rss T1");
	sprintf(cha_sql + strlen(cha_sql), " where T2.n_blog_id = %d)", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp && *chp_tmp) {
			char cha_tmp[1024];
			sprintf(cha_tmp, "%s:%d", chp_tmp, CO_SIDECONTENT_ARCHIVE_RSS);
			Put_Nlist(nlp_in, "sidecontent", cha_tmp);
		}
		Db_CloseDyna(dbres);
	}
	/* 再表示 */
	if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ設定ページの表示に失敗しました。<br>");
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	edit_rss()
 * Description:		サイドコンテンツのRSS内容を変更＆画面再描画
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int edit_rss(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_escape;
	char *chp_tmp;
	char *chp_rss_id;
	char cha_sql[512];
	int in_rss_id;
	int in_rss_type;
	int in_skip;

	chp_rss_id = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_rss_id || !*chp_rss_id) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツIDの取得に失敗しました。<br>");
		return 1;
	}
	if (rss_error_check(nlp_in, nlp_out)) {
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_rss_id, &in_rss_type);
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		if (Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		strcpy(cha_sql, "update at_rss");
		strcat(cha_sql, " set c_rss_title = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "rss_title", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', c_rss_url = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "rss_url", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', n_rss_article = ");
		chp_tmp = Get_Nlist(nlp_in, "rss_article", 1);
		strcat(cha_sql, chp_tmp);
		strcat(cha_sql, ", b_rss_target = ");
		chp_tmp = Get_Nlist(nlp_in, "rss_target", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		sprintf(cha_sql + strlen(cha_sql), " where n_rss_id = %d", in_rss_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "RSSを更新するクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		strcpy(cha_sql, "update at_sidecontent");
		strcat(cha_sql, " set c_sidecontent_title = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "rss_title", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		sprintf(cha_sql + strlen(cha_sql), "' where n_sidecontent_id = %d", in_rss_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_rss_type);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツを更新するクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	/* 再表示 */
	if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ設定ページの表示に失敗しました。<br>");
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_rss
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値       	エラー   1
-* ------------------------------------------------------------------------*/
static int delete_rss(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char cha_sql[512];
	int in_rss_id;
	int in_rss_type;
	int in_skip;

	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "RSSIDの取得ができませんでした。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_rss_id, &in_rss_type);
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		if (Begin_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		if (shift_sidecontent_order(db, nlp_in, nlp_out, in_blog)) {
			Rollback_Transact(db);
			return 1;
		}
		strcpy(cha_sql, "delete from at_rss");
		sprintf(cha_sql + strlen(cha_sql), " where n_rss_id = %d", in_rss_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "RSS削除クエリに失敗しました<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		strcpy(cha_sql, "delete from at_sidecontent");
		sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_id = %d", in_rss_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_rss_type);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ削除クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		parts_error_check()
 * Description:
 *	ブログパーツの入力項目のエラーチェック
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int parts_error_check(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chp_tmp;
	char cha_error[512];
	int in_error;

	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "parts_title", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "ブログパーツのタイトルを入力してください。<br>");
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "ブログパーツのタイトルを入力してください。<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_PARTS_TITLE) {
		sprintf(cha_error, "ブログパーツのタイトルは半角%d文字(全角%d文字)以内で入力してください。<br>"
			, CO_MAXLEN_PARTS_TITLE ,CO_MAXLEN_PARTS_TITLE / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "parts_body", 1);
	if (!chp_tmp || !*chp_tmp || strcmp(chp_tmp, "http://") == 0) {
		Put_Nlist(nlp_out, "ERROR", "ブログパーツのソースを入力してください。<br>");
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "ブログパーツのソースを入力してください。<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_PARTS_BODY) {
		sprintf(cha_error, "ブログパーツのソースは半角%d文字(全角%d文字)以内で入力してください。<br>"
			, CO_MAXLEN_PARTS_BODY ,CO_MAXLEN_PARTS_BODY / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	add_parts()
 * Description:
 *	サイドコンテンツへブログパーツを追加
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int add_parts(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_valid;
	char *chp_escape;
	char *chp_tmp;
	char *chp_disp_title;
	char *chp_title;
	char *chp_body;
	char cha_sql[CO_MAXLEN_PARTS_BODY + 1024];
	int in_parts_id;
	int in_sidecontent_order;
	int in_skip;

	if (parts_error_check(nlp_in, nlp_out)) {
		return 1;
	}
	chp_disp_title = Get_Nlist(nlp_in, "parts_disp", 1);
	chp_title = Get_Nlist(nlp_in, "parts_title", 1);
	chp_body = Get_Nlist(nlp_in, "parts_body", 1);

	sprintf(cha_sql, "select count(*) from at_sidecontent where n_blog_id = %d and n_sidecontent_type = %d", in_blog, CO_SIDECONTENT_BLOG_PARTS);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するブログパーツ数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "表示するブログパーツ数の取得に失敗しました。<br>");
		return 1;
	}
	if (atoi(chp_tmp) >= CO_MAX_RSS_IMPORTS) {
		Put_Format_Nlist(nlp_out, "ERROR", "ブログパーツは全部で %d 件までしか登録できません。<br>", CO_MAX_BLOG_PARTS);
		Db_CloseDyna(dbres);
		return 1;
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
		if (Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		sprintf(cha_sql,
			"select coalesce(max(T1.n_parts_id) + 1, 1)"
			" from at_parts T1"
			" where T1.n_blog_id = %d for update", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ登録クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_parts_id = atoi(chp_tmp);
		} else {
			in_parts_id = 1;
		}
		Db_CloseDyna(dbres);

		strcpy(cha_sql, "insert into at_parts");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_parts_id");
		strcat(cha_sql, ",c_parts_title");
		strcat(cha_sql, ",c_parts_body");
		strcat(cha_sql, ",b_parts_title");
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d", in_blog, in_parts_id);
		strcat(cha_sql, ", '");
		chp_escape = My_Escape_SqlString(db, chp_title);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', '");
		chp_valid = Remove_Invalid_Tags(chp_body, nlp_out, 0);
		if (!chp_valid) {
			Rollback_Transact(db);
			return 1;
		}
		chp_escape = My_Escape_SqlString(db, chp_valid);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		free(chp_valid);
		strcat(cha_sql, "', ");
		if (chp_disp_title && *chp_disp_title) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ")");
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "RSS登録クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		sprintf(cha_sql,
			"select coalesce(max(T2.n_sidecontent_order), 0) + 1"
			" from at_sidecontent T2"
			" where T2.n_blog_id = %d for update", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ登録クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_sidecontent_order = atoi(chp_tmp);
		} else {
			in_sidecontent_order = 1;
		}
		Db_CloseDyna(dbres);

		/* こちらはサブクエリ使用。注意。 */
		strcpy(cha_sql, "insert into at_sidecontent");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_sidecontent_id");
		strcat(cha_sql, ",n_sidecontent_order");
		strcat(cha_sql, ",n_sidecontent_type");
		strcat(cha_sql, ",c_sidecontent_title");
		strcat(cha_sql, ",b_display");
		strcat(cha_sql, ",b_allow_outer");
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d, %d, %d", in_blog, in_parts_id, in_sidecontent_order, CO_SIDECONTENT_BLOG_PARTS);
		strcat(cha_sql, ", '");
		chp_escape = My_Escape_SqlString(db, chp_title);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', 1, 1)");
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ登録クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	edit_rss()
 * Description:		サイドコンテンツのRSS内容を変更＆画面再描画
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int edit_parts(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_escape;
	char *chp_valid;
	char *chp_tmp;
	char *chp_parts_id;
	char cha_sql[CO_MAXLEN_PARTS_BODY + 1024];
	int in_parts_id;
	int in_skip;

	chp_parts_id = Get_Nlist(nlp_in, "partsid", 1);
	if (!chp_parts_id || !*chp_parts_id) {
		Put_Nlist(nlp_out, "ERROR", "パーツIDの取得に失敗しました。<br>");
		return 1;
	}
	if (parts_error_check(nlp_in, nlp_out)) {
		return 1;
	}
	in_parts_id = atoi(chp_parts_id);
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		if (Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		strcpy(cha_sql, "update at_parts");
		strcat(cha_sql, " set c_parts_title = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "parts_title", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', c_parts_body = '");
		chp_valid = Remove_Invalid_Tags(Get_Nlist(nlp_in, "parts_body", 1), nlp_out, 0);
		if (!chp_valid) {
			Rollback_Transact(db);
			return 1;
		}
		chp_escape = My_Escape_SqlString(db, chp_valid);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		free(chp_valid);
		strcat(cha_sql, "', b_parts_title = ");
		chp_tmp = Get_Nlist(nlp_in, "parts_disp", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		sprintf(cha_sql + strlen(cha_sql), " where n_parts_id = %d", in_parts_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "RSSを更新するクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		strcpy(cha_sql, "update at_sidecontent");
		strcat(cha_sql, " set c_sidecontent_title = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "parts_title", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		sprintf(cha_sql + strlen(cha_sql), "' where n_sidecontent_id = %d", in_parts_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", CO_SIDECONTENT_BLOG_PARTS);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツを更新するクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_rss
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値       	エラー   1
-* ------------------------------------------------------------------------*/
static int delete_parts(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char cha_sql[512];
	int in_parts_id;
	int in_skip;

	chp_tmp = Get_Nlist(nlp_in, "partsid", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "パーツIDの取得ができませんでした。<br>");
		return 1;
	}
	in_parts_id = atoi(chp_tmp);
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		if (Begin_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		if (shift_sidecontent_order_by_id(db, nlp_in, nlp_out, in_blog, in_parts_id, CO_SIDECONTENT_BLOG_PARTS)) {
			Rollback_Transact(db);
			return 1;
		}
		strcpy(cha_sql, "delete from at_parts");
		sprintf(cha_sql + strlen(cha_sql), " where n_parts_id = %d", in_parts_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "RSS削除クエリに失敗しました<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		strcpy(cha_sql, "delete from at_sidecontent");
		sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_id = %d", in_parts_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", CO_SIDECONTENT_BLOG_PARTS);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ削除クエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	add_linkitem()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1(直前に戻る)
 *		 2(トップに戻る)
-* ------------------------------------------------------------------------*/
static int add_linkitem(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[2048];
	int in_ret;
	int in_linkitem_id;
	int in_linkitem_order;
	int in_grp_id;
	int in_grp_type;
	int in_skip;

	if (linkitem_error_check(nlp_in, nlp_out)) {
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_grp_id, &in_grp_type);

	sprintf(cha_sql, "select count(*) from at_linkitem where n_linkgroup_id = %d and n_blog_id = %d", in_grp_id, in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "リンク集アイテムの個数を得るクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンク集アイテムの個数の取得に失敗しました。<br>");
		return 1;
	}
	if (atoi(chp_tmp) >= CO_MAX_LINK_ITEM) {
		Put_Format_Nlist(nlp_out, "ERROR", "リンク集アイテムは %d 個以上登録できません。<br>", CO_MAX_LINK_ITEM + 1);
		Db_CloseDyna(dbres);
		return 1;
	}
	Db_CloseDyna(dbres);

	if (Begin_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
		return 1;
	}
	strcpy(cha_sql, " select coalesce(max(T1.n_linkitem_id), 0) + 1");	/* 0 リンクitemid最大値+1 */
	strcat(cha_sql, ",coalesce(max(T1.n_linkitem_order), 0) + 1");	/* 2 リンクアイテム順番最大値 + 1 */
	strcat(cha_sql, " from at_linkitem T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_linkgroup_id = %d", in_grp_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " for update");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "リンクを追加するクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_linkitem_id = atoi(chp_tmp);
	} else {
		in_linkitem_id = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_linkitem_order = atoi(chp_tmp);
	} else {
		in_linkitem_order = 0;
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
		strcpy(cha_sql, "insert into at_linkitem(");
		strcat(cha_sql, " n_blog_id");
		strcat(cha_sql, ",n_linkitem_id");		/* 0 */
		strcat(cha_sql, ",n_linkgroup_id");		/* 1 */
		strcat(cha_sql, ",n_linkitem_order");	/* 2 */
		strcat(cha_sql, ",c_link_title");		/* 3 */
		strcat(cha_sql, ",c_link_url");			/* 4 */
		strcat(cha_sql, ",b_link_target");		/* 5 */
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d, %d, %d", in_blog, in_linkitem_id, in_grp_id, in_linkitem_order);
		strcat(cha_sql, ", '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "linkitem_title", 1));
		strcat(cha_sql, chp_escape);	/* 3 */
		free(chp_escape);
		strcat(cha_sql, "', '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "linkitem_url", 1));	/* 5 */
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "' ,");
		chp_tmp = Get_Nlist(nlp_in, "linkitem_target", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ")");
		if (Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "リンクを追加するクエリーに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	if (Commit_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
		Rollback_Transact(db);
		return 1;
	}
	in_ret = disp_page_linklist_edit(db ,nlp_in, nlp_out, in_blog);
	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_linkitem()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1(直前に戻る)
 *		 2(トップに戻る)
-* ------------------------------------------------------------------------*/
static int update_linkitem(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_ret;
	int in_grp_id;
	int in_grp_type;
	int in_skip;

	if (linkitem_error_check(nlp_in, nlp_out)) {
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "linkitem", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンクIDが得られませんでした。<br>");
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンクグループが得られませんでした。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_grp_id, &in_grp_type);
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		strcpy(cha_sql, "update at_linkitem");
		strcat(cha_sql, " set c_link_title = '");
		chp_tmp = Get_Nlist(nlp_in, "linkitem_title", 1);
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', c_link_url = '");
		chp_tmp = Get_Nlist(nlp_in, "linkitem_url", 1);
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', b_link_target = ");
		chp_tmp = Get_Nlist(nlp_in, "linkitem_target", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, " where n_linkitem_id = ");
		strcat(cha_sql, Get_Nlist(nlp_in, "linkitem", 1));
		sprintf(cha_sql + strlen(cha_sql), " and n_linkgroup_id = %d", in_grp_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", cha_sql);
			return 1;
		}
	}
	in_ret = disp_page_linklist_edit(db ,nlp_in, nlp_out, in_blog);
	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_delete_linklist_ask()
 * Description:
 *	リンク集を削除するときの確認ページ表示
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
 *	トップに戻るエラー 2
-* ------------------------------------------------------------------------*/
static int disp_page_delete_linklist_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_sidecontent)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_grp_id;
	int in_grp_type;

	chp_tmp = Get_Nlist(nlp_in, "linkgroup_mode", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "linkgroup_mode", "1");
	}
	if (!chp_sidecontent || !*chp_sidecontent) {
		Put_Nlist(nlp_out, "ERROR", "削除するリンクを選択してください。<br>");
		return 1;
	}
	divide_sidecontent2(chp_sidecontent, &in_grp_id, &in_grp_type);
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_sidecontent);

	strcpy(cha_sql, "select T1.c_sidecontent_title");
	strcat(cha_sql, " from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_id = %d", in_grp_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", CO_SIDECONTENT_ARCHIVE_LINKS);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "削除するリンク集の情報を得るクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		Put_Nlist(nlp_out, "ERROR", "選択されたリンク集は既に削除されています。<br>");
		Db_CloseDyna(dbres);
		return 2;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "DELETE_LINKLIST_TITLE", chp_escape);
		free(chp_escape);
	}
	Db_CloseDyna(dbres);
	Page_Out(nlp_out, CO_SKEL_DELETE_LINKLIST_ASK);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_linklist()
 * Description:		リンク集を削除
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int delete_linklist(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char cha_sql[512];
	int in_grp_id;
	int in_grp_type;
	int in_skip;

	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "削除するリンク集が得られませんでした。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_grp_id, &in_grp_type);
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		if (Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		/* 順番詰める */
		if (shift_sidecontent_order(db, nlp_in, nlp_out, in_blog)) {
			Rollback_Transact(db);
			return 1;
		}
		/* グループに属するリンクの削除 */
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "delete from at_linkitem");
		sprintf(cha_sql + strlen(cha_sql), " where n_linkgroup_id = %d", in_grp_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db ,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "グループに属するリンクの削除に失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* グループ本体を削除 */
		strcpy(cha_sql, "delete from at_sidecontent");
		sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_id = %d", in_grp_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_grp_type);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db ,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "リンク集を削除するクエリーに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ設定ページの表示に失敗しました。<br>");
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_delete_linkitem_ask()
 * Description:		個々のリンクを削除するときの確認ページ表示
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_delete_linkitem_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_grp_id;
	int in_grp_type;

	/* リンクグループモード埋め込み */
	chp_tmp = Get_Nlist(nlp_in, "linkgroup_mode", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "linkgroup_mode", "1");
	}
	/* リンクグループ埋め込み */
	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンクグループが得られませんでした。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "sidecontent", chp_tmp);
	/* リンクアイテム埋め込み */
	chp_tmp = Get_Nlist(nlp_in, "linkitem", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "削除するリンクを選択してください。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "linkitem", chp_tmp);
	divide_sidecontent(nlp_in, "sidecontent", &in_grp_id, &in_grp_type);
	strcpy(cha_sql, "select T1.c_link_title");
	strcat(cha_sql, " from at_linkitem T1");
	strcat(cha_sql, " where T1.n_linkitem_id = ");
	strcat(cha_sql, Get_Nlist(nlp_in, "linkitem", 1));
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_linkgroup_id = %d", in_grp_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "リンク情報を得るクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		Put_Nlist(nlp_out, "ERROR", "選択されたリンクは既に削除されています。<br>");
		Db_CloseDyna(dbres);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "DELETE_LINKITEM_TITLE", chp_escape);
		free(chp_escape);
	}
	Db_CloseDyna(dbres);
	Page_Out(nlp_out, CO_SKEL_DELETE_LINKITEM_ASK);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	shift_linkitem_order()
 * Description:
 *	リンク削除に伴い、順番を詰める。
 *	サブクエリで一気につめるので注意。
 *	アップデートするので呼ぶ関数でトランザクションを忘れずに。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int shift_linkitem_order(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_linkitem_order;
	int in_linkitem_order;
	int in_grp_id;
	int in_grp_type;

	divide_sidecontent(nlp_in, "sidecontent", &in_grp_id, &in_grp_type);
	strcpy(cha_sql, " select T1.n_linkitem_order");	/* サブクエリ開始 */
	strcat(cha_sql, " from at_linkitem T1");
	strcat(cha_sql, " where n_linkitem_id = ");
	strcat(cha_sql, Get_Nlist(nlp_in, "linkitem", 1));
	sprintf(cha_sql + strlen(cha_sql), " and n_linkgroup_id = %d", in_grp_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "リンク情報を得るクエリーに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_linkitem_order = Db_GetValue(dbres, 0, 0);
	if (chp_linkitem_order) {
		in_linkitem_order = atoi(chp_linkitem_order);
	} else {
		in_linkitem_order = 0;
	}
	Db_CloseDyna(dbres);

	strcpy(cha_sql, "update at_linkitem");
	strcat(cha_sql, " set n_linkitem_order = n_linkitem_order - 1");
	sprintf(cha_sql + strlen(cha_sql), " where n_linkitem_order > %d", in_linkitem_order);
	sprintf(cha_sql + strlen(cha_sql), " and n_linkgroup_id = %d", in_grp_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "リンク削除に伴う調整に失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_linkitem()
 * Description:		個々のリンクを削除
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1 直前に戻る
 *		 2 トップに戻る
-* ------------------------------------------------------------------------*/
static int delete_linkitem(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char *chp_tmp2;
	char cha_sql[512];
	int in_ret;
	int in_grp_id;
	int in_grp_type;
	int in_skip;

	chp_tmp = Get_Nlist(nlp_in, "linkitem", 1);
	chp_tmp2 = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp || !chp_tmp2 || !*chp_tmp2) {
		Put_Nlist(nlp_out, "ERROR", "予期せぬリクエストです。<br>");
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
		if (Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		/* 順番詰める */
		if (shift_linkitem_order(db, nlp_in, nlp_out, in_blog)){
			Rollback_Transact(db);
			return 1;
		}
		divide_sidecontent(nlp_in, "sidecontent", &in_grp_id, &in_grp_type);
		strcpy(cha_sql, "delete from at_linkitem");
		strcat(cha_sql, " where n_linkitem_id = ");
		strcat(cha_sql, chp_tmp);
		sprintf(cha_sql + strlen(cha_sql), " and n_linkgroup_id = %d", in_grp_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db ,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "削除するクエリーに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	in_ret = disp_page_linklist_edit(db, nlp_in, nlp_out, in_blog);
	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	exchange_linkitem_order()
 * Description:
 *	リンク集の中のリンクの並び替えを行う。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1 (直前に戻る)
 *		 2 (トップに戻る)
-* ------------------------------------------------------------------------*/
static int exchange_linkitem_order(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char cha_where_etc[512];
	int in_type;
	int in_id;
	int in_error;

	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "所属リンク集が得られませんでした。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_id, &in_type);
	sprintf(cha_where_etc, CO_LINKITEM_WHERE_ETC, in_id, in_blog);
	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "linkitem", 1);
	if (chp_tmp && *chp_tmp) {
		if (Get_Nlist(nlp_in, "BTN_UP_LINKITEM.x", 1)) {
			if (Change_Order_Db(OldDBase(db), chp_tmp, CO_LINKITEM_NAME_ID, CO_LINKITEM_NAME_SEQ, CO_LINKITEM_NAME_TABLE, cha_where_etc, CO_MODE_UP)) {
				in_error++;
			}
		} else if (Get_Nlist(nlp_in, "BTN_DOWN_LINKITEM.x", 1)) {
			if (Change_Order_Db(OldDBase(db), chp_tmp, CO_LINKITEM_NAME_ID, CO_LINKITEM_NAME_SEQ, CO_LINKITEM_NAME_TABLE, cha_where_etc, CO_MODE_DOWN)) {
				in_error++;
			}
		}
	}
	if (in_error) {
		Put_Nlist(nlp_out, "ERROR", "リンクの順番の入れ替えに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_error = disp_page_linklist_edit(db, nlp_in, nlp_out, in_blog);
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_linklist_title()
 * Description:
 *	リンク集タイトルの編集
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1(直前に戻る)
 *		 2(トップに戻る)
-* ------------------------------------------------------------------------*/
static int update_linklist_title(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_escape;
	char *chp_tmp;
	char cha_sql[512];
	int in_ret;
	int in_grp_id;
	int in_grp_type;
	int in_skip;

	if (linklist_error_check(nlp_in, nlp_out)) {
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "リンク集のIDを取得できませんでした。<br>");
		return 1;
	}
	divide_sidecontent(nlp_in, "sidecontent", &in_grp_id, &in_grp_type);
	in_skip = 0;
	if (g_in_cart_mode == CO_CART_RESERVE) {
		in_skip = Get_Sample_Mode(db, nlp_out);
		if (in_skip < 0) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (!in_skip) {
		if (Begin_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
			return 1;
		}
		strcpy(cha_sql, "update at_sidecontent");
		strcat(cha_sql, " set c_sidecontent_title = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "linklist_title", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		sprintf(cha_sql + strlen(cha_sql), "' where n_sidecontent_id = %d", in_grp_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_grp_type);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
			Rollback_Transact(db);
			return 1;
		}
	}
	in_ret = disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog);
	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	add_linklist()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int add_linklist(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_sidecontent_id;
	int in_sidecontent_order;
	int in_skip;

	if (linklist_error_check(nlp_in, nlp_out)) {
		return 1;
	}

	sprintf(cha_sql, "select count(*) from at_sidecontent where n_blog_id = %d and n_sidecontent_type = %d", in_blog, CO_SIDECONTENT_ARCHIVE_LINKS);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツ数を得るクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "表示するサイドコンテンツ数の取得に失敗しました。<br>");
		return 1;
	}
	if (atoi(chp_tmp) >= CO_MAX_LINK_LISTS) {
		Put_Format_Nlist(nlp_out, "ERROR", "リンク集は %d 個以上登録できません。<br>", CO_MAX_LINK_LISTS + 1);
		Db_CloseDyna(dbres);
		return 1;
	}
	Db_CloseDyna(dbres);

	if (Begin_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
		return 1;
	}
	strcpy(cha_sql, "select coalesce(max(T1.n_sidecontent_id), 0) + 1 from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_sidecontent_type = %d", CO_SIDECONTENT_ARCHIVE_LINKS);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " for update");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツの登録に失敗しました。(リンク集)<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_escape = Db_GetValue(dbres, 0, 0);
	if (chp_escape) {
		in_sidecontent_id = atoi(chp_escape);
	} else {
		in_sidecontent_id = 1;
	}
	Db_CloseDyna(dbres);

	/* ここまでn_sidecontent_id */
	strcpy(cha_sql, "select coalesce(max(T2.n_sidecontent_order), 0) + 1 from at_sidecontent T2");
	sprintf(cha_sql + strlen(cha_sql), " where T2.n_blog_id = %d", in_blog);
	strcat(cha_sql, " for update");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツの登録に失敗しました。(リンク集)<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_escape = Db_GetValue(dbres, 0, 0);
	if (chp_escape) {
		in_sidecontent_order = atoi(chp_escape);
	} else {
		in_sidecontent_order = 1;
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
		/* サブクエリ使用。注意 */
		strcpy(cha_sql, "insert into at_sidecontent");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_sidecontent_id");
		strcat(cha_sql, ",n_sidecontent_order");
		strcat(cha_sql, ",n_sidecontent_type");
		strcat(cha_sql, ",c_sidecontent_title");
		strcat(cha_sql, ",b_display");
		strcat(cha_sql, ",b_allow_outer");
		strcat(cha_sql, ") values (");
		sprintf(cha_sql + strlen(cha_sql), "%d, %d, %d, %d", in_blog, in_sidecontent_id, in_sidecontent_order, CO_SIDECONTENT_ARCHIVE_LINKS);
		strcat(cha_sql, ", '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "linklist_title", 1));
		strcat(cha_sql, chp_escape);	/* c_sidecontent_title */
		free(chp_escape);
		strcat(cha_sql, "', 1, 1)");
		if (Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツの登録に失敗しました。(リンク集)<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* この場合のみ初回の関数 */
		if (disp_page_linklist_new(db, nlp_in, nlp_out, in_blog)) {
			Rollback_Transact(db);
			return 1;
		}
	} else {
		chp_tmp = Get_Nlist(nlp_in, "page1", 1);
		Put_Nlist(nlp_out, "PAGE1", chp_tmp ? chp_tmp : "");
		chp_tmp = Get_Nlist(nlp_in, "page2", 1);
		Put_Nlist(nlp_out, "PAGE2", chp_tmp ? chp_tmp : "none");
		chp_tmp = Get_Nlist(nlp_in, "page3", 1);
		Put_Nlist(nlp_out, "PAGE3", chp_tmp ? chp_tmp : "none");
		chp_tmp = Get_Nlist(nlp_in, "page4", 1);
		Put_Nlist(nlp_out, "PAGE4", chp_tmp ? chp_tmp : "none");
		chp_tmp = Get_Nlist(nlp_in, "page5", 1);
		Put_Nlist(nlp_out, "PAGE5", chp_tmp ? chp_tmp : "none");
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			Rollback_Transact(db);
			return 1;
		}
	}
	if (Commit_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	add_sidecontent()
 * Description:
 *	表示されるサイドコンテンツを足す。
 *	リンク集とRSSを新規追加するときは、設定画面に飛ばす。
 *	既存の項目およびユーザーが作成したリンク集・RSS集が、非表示になっている場合は、
 *	表示するようにアップデートし、画面を更新する。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int add_sidecontent(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_add;
	char *chp_tmp;
	char cha_sql[512];
	int in_add_type;
	int in_add_id;
	int in_order;
	int in_skip;

	chp_add = Get_Nlist(nlp_in, "add_sidecontent", 1);
	divide_sidecontent(nlp_in, "add_sidecontent", &in_add_id, &in_add_type);
	if (Begin_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました<br>");
		return 1;
	}
	strcpy(cha_sql, "select coalesce(max(T1.n_sidecontent_order), 0) + 1");	/* サブクエリ開始 */
	strcat(cha_sql, " from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " for update");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツを表示させるクエリに失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
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
		/* サブクエリ使用 */
		strcpy(cha_sql, "update at_sidecontent");
		strcat(cha_sql, " set b_display = 1");
		sprintf(cha_sql + strlen(cha_sql), ", n_sidecontent_order = %d", in_order);
		sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_id = %d", in_add_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_add_type);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツを表示させるクエリに失敗しました。<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	if (Commit_Transact(db)){
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
		Rollback_Transact(db);
		return 1;
	}
	Put_Format_Nlist(nlp_in, "sidecontent", "%s:%d", chp_add, in_add_type);	/* 選択させておくために押し戻す */
	/* 再表示 */
	if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ設定ページの表示に失敗しました。<br>");
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_linkitem()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
static int disp_page_linklist_title_new(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char *chp_escape;

	Put_Nlist(nlp_out, "MODE_LINKLIST_TITLE", "新規リンク集のタイトル作成");
	Put_Nlist(nlp_out, "BUTTON_LINKLIST_TITLE", "next.gif");
	Put_Nlist(nlp_out, "BTN_NAME", "BTN_ADD_NEW_LINKLIST");
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	Put_Format_Nlist(nlp_out, "RESET_LOC", "%s?BTN_ADD_NEW_LINKLIST=1%s", chp_tmp ? "&from_user=1" : "");
	chp_tmp = Get_Nlist(nlp_in, "linklist_title", 1);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Get_Nlist(nlp_in, "linklist_title", 1);
		Put_Nlist(nlp_out, "LINKLIST_TITLE", chp_escape);
		free(chp_escape);
	}
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_SETTING_LINKLIST=1&blogid=%d", CO_CGI_SIDECONTENT, in_blog);
	Page_Out(nlp_out, CO_SKEL_LINKLIST_TITLE);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	same_db()
 * Description:
 *
%* ------------------------------------------------------------------------
 * 戻り値:			同一でない 0
 *					同一である 1
 *					エラー時   -1
-* ------------------------------------------------------------------------*/
static int same_db(DBase *db, NLIST *nlp_out, char *chp_host, char *chp_dbase)
{
	DBRes *dbres;
	int in_same;
	char *chp_db_host;
	char *chp_db_dbase;

	dbres = Db_OpenDyna(db, "select T1.c_host,T1.c_db from sy_authinfo T1");
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "データベースの比較に失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "データベースの比較に失敗しました。<br>");
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_same = 1;
	chp_db_host = Db_FetchValue(dbres, 0);
	if (!chp_db_host || strcmp(chp_host, chp_db_host)) {
		in_same = 0;
	}
	chp_db_dbase = Db_FetchValue(dbres, 1);
	if (!chp_db_dbase || strcmp(chp_dbase, chp_db_dbase)) {
		in_same = 0;
	}
	Db_CloseDyna(dbres);
	return in_same;
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
	char cha_blog[32];
	char *chp_fromuser;
	char *chp_tmp;
	int in_blog;
	int in_error;
	int in_err;
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
	if (!db){
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
	g_in_same_database = same_db(db, nlp_out, cha_host, cha_db);
	if (g_in_same_database == CO_ERROR) {
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}

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
					Put_Nlist(nlp_out, "ACTION", CO_CGI_SIDECONTENT);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "認証情報を取得できません。<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_SIDECONTENT);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_SIDECONTENT);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_SIDECONTENT);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_SIDECONTENT);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_SIDECONTENT);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを操作する権限がありません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_SIDECONTENT);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを作成されていない方はご利用になれません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);
	g_in_need_login = Need_Login(db, in_blog);

	chp_fromuser = Get_Nlist(nlp_in, "from_top", 1);
	if (chp_fromuser && atoi(chp_fromuser)) {
		Put_Nlist(nlp_out, "FROM_TOP", "&from_top=1");
	}
	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_fromuser && atoi(chp_fromuser)) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_fromuser);
		Put_Nlist(nlp_out, "FROM_USER", "&from_user=1");
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

	/* サイドコンテンツの順序変更 */
	if (Get_Nlist(nlp_in, "BTN_UP_SIDECONTENT.x", 1) || Get_Nlist(nlp_in, "BTN_DOWN_SIDECONTENT.x", 1) ||
		Get_Nlist(nlp_in, "BTN_TOP_SIDECONTENT.x", 1) || Get_Nlist(nlp_in, "BTN_BOTTOM_SIDECONTENT.x", 1)) {
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (exchange_sidecontent_order(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* サイドコンテンツ一覧ページの表示 */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_SETTING_SIDECONTENT", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "page1", 1);
		Put_Nlist(nlp_out, "PAGE1", chp_tmp ? chp_tmp : "");
		chp_tmp = Get_Nlist(nlp_in, "page2", 1);
		Put_Nlist(nlp_out, "PAGE2", chp_tmp ? chp_tmp : "none");
		chp_tmp = Get_Nlist(nlp_in, "page3", 1);
		Put_Nlist(nlp_out, "PAGE3", chp_tmp ? chp_tmp : "none");
		chp_tmp = Get_Nlist(nlp_in, "page4", 1);
		Put_Nlist(nlp_out, "PAGE4", chp_tmp ? chp_tmp : "none");
		chp_tmp = Get_Nlist(nlp_in, "page5", 1);
		Put_Nlist(nlp_out, "PAGE5", chp_tmp ? chp_tmp : "none");
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* サイドコンテンツリンクリスト表示 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_SETTING_LINKLIST", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* サイドコンテンツRSSリスト表示 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_SETTING_RSSLIST", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* サイドコンテンツ詳細表示オプション表示 */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_SETTING_DISPLAY", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* サイドコンテンツを非表示に */
	} else if (Get_Nlist(nlp_in, "BTN_UNABLE_DISP_SIDECONTENT.x", 1)) {
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (un_display_sidecontent(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* サイドコンテンツを表示に */
	} else if (Get_Nlist(nlp_in, "BTN_ADD_SIDECONTENT.x", 1)) {
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (add_sidecontent(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* RSS取り込みRSS新規追加ページ */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_NEW_RSSITEM.x", 1)) {
		if (disp_page_rss_new(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* RSS新規追加ページからのRSS追加 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_ADD_RSS.x", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (add_rss(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_rss_new(db, nlp_in, nlp_out, in_blog)){
				in_error++;
			}
		}
	/* RSS編集ページ表示 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_EDIT_RSS", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (disp_page_rss_edit(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* RSS編集ページからの更新 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_EDIT_RSS.x", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (edit_rss(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_rss_edit(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* RSS編集ページから削除確認画面表示 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DELETE_RSS_ASK", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
		if (disp_page_delete_rss_ask(db, nlp_in, nlp_out, in_blog, chp_tmp)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* 削除確認画面からのRSS削除 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DELETE_RSS", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (delete_rss(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* アーカイブ設定ページの編集 */
	} else if (Get_Nlist(nlp_in, "BTN_SET_ARCHIVE.x", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (update_archive(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* ブログパーツ編集画面 */
	} else if (!g_in_dbb_mode && (Get_Nlist(nlp_in, "BTN_DISP_EDIT_BLOGPARTS", 1) || Get_Nlist(nlp_in, "BTN_DISP_ADD_BLOGPARTS", 1))) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* ブログパーツ登録処理 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_ADD_BLOGPARTS.x", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		if (add_parts(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
		}
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* ブログパーツ登録処理 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_EDIT_BLOGPARTS.x", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		if (edit_parts(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
		}
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* ブログパーツ削除処理 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DELETE_BLOGPARTS", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE2", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		if (delete_parts(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
		}
		if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
	/* ブログパーツ削除画面 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DELETE_PARTS_ASK", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "partsid", 1);
		if (disp_page_delete_parts_ask(db, nlp_in, nlp_out, in_blog, chp_tmp)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* プロフィール設定ページの表示 */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_PROFILE", 1)) {
		if (disp_page_profile(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			in_error++;
		}
	/* プロフィール設定ページの編集 */
	} else if (Get_Nlist(nlp_in, "BTN_EDIT_PROFILE.x", 1)) {
		if (update_profile(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_profile(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク集編集ページ表示 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_EDIT_LINK", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (disp_page_linklist_edit(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク集タイトル編集ページ表示(リンク集詳細ページ→タイトル編集ページ) */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_LINKLIST_TITLE_EDIT", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (disp_page_linklist_title_edit(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク集タイトル編集(タイトル編集→リンク集詳細) */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_EDIT_LINKLIST_TITLE.x", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (update_linklist_title(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_linklist_title_edit(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク集削除確認ページ */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_DELETE_LINKLIST_ASK", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		chp_tmp = Get_Nlist(nlp_in, "sidecontent", 1);
		if (disp_page_delete_linklist_ask(db ,nlp_in, nlp_out, in_blog, chp_tmp)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンクリスト削除 */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DELETE_LINKLIST", 1)) {
		Put_Nlist(nlp_out, "PAGE1", "none");
		Put_Nlist(nlp_out, "PAGE3", "none");
		Put_Nlist(nlp_out, "PAGE4", "none");
		Put_Nlist(nlp_out, "PAGE5", "none");
		if (delete_linklist(db ,nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "BUTTON_NAME", "BTN_EDIT_SIDECONTENT");
			in_error++;
		}
	/* 新規リンク集追加(タイトル決定→リンク集詳細ページへ) */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_ADD_NEW_LINKLIST.x", 1)) {
		if (add_linklist(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_linklist_title_new(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク追加ページ */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_NEW_LINKITEM.x", 1)) {
		if (disp_page_linklist_title_new(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク追加(リンク集詳細ページ→リンク追加) */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_ADD_LINKITEM.x", 1)) {
		in_err = disp_page_linkitem_new(db, nlp_in, nlp_out, in_blog);
		if (in_err) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク追加(リンク集詳細ページ→リンク追加) */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_EDIT_LINKITEM.x", 1)) {
		if (disp_page_linkitem_edit(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_linklist_edit(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク追加(リンク追加→リンク集詳細ページ */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_ADD_LINKITEM.x", 1)) {
		in_err = add_linkitem(db ,nlp_in, nlp_out, in_blog);
		if (in_err) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (in_err == 1) {
				if (disp_page_linkitem_new(db, nlp_in, nlp_out, in_blog)) {
					in_error++;
				}
			} else {
				if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
					in_error++;
				}
			}
		}
	/* リンク編集(リンク編集->リンク集詳細ページ */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_EDIT_LINKITEM.x", 1)) {
		in_err = update_linkitem(db ,nlp_in, nlp_out, in_blog);
		if (in_err) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_linkitem_edit(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク削除確認(リンク集詳細→リンク削除確認) */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DISP_DELETE_LINKITEM_ASK.x", 1)) {
		if (disp_page_delete_linkitem_ask(db ,nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_linklist_edit(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク削除(リンク削除確認→リンク集詳細) */
	} else if (!g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_DELETE_LINKITEM", 1)) {
		if (delete_linkitem(db ,nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_linklist_edit(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	/* リンク表示移動 (リンク集詳細ページの移動ボタン) */
	} else if (!g_in_dbb_mode && (Get_Nlist(nlp_in, "BTN_UP_LINKITEM.x", 1) || Get_Nlist(nlp_in, "BTN_DOWN_LINKITEM.x", 1))) {
		in_err = exchange_linkitem_order(db ,nlp_in, nlp_out, in_blog);
		if (in_err) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	} else if (Get_Nlist(nlp_in, "BTN_DISP_PROFILE_IMAGE", 1)) {
		if (disp_profile_image(db ,nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "BUTTON_NAME", "BTN_EDIT_SIDECONTENT");
			in_error++;
		}
	} else if (Get_Nlist(nlp_in, "BTN_UPDATE_ACCESS", 1)) {
		if (update_access(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "from_error", "1");
			if (disp_page_sidecontent_list(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
		}
	} else if (Get_Nlist(nlp_in, "BTN_DISP_BLOG_SETTING", 1)) {
		printf("Location: %s%s%s/%s?BTN_EDIT_ABOUT_BLOG=1&blogid=%d%s\n\n",
			   g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_CONFIGURE, in_blog, (chp_fromuser && atoi(chp_fromuser)) ? "&from_user=1" : "");
	} else {
		Put_Nlist(nlp_out, "ERROR", "直接CGIを実行できません。<br>");
		in_error++;
	}
	if (in_error) {
		put_error_page(nlp_in, nlp_out);
		Page_Out(nlp_out, CO_SKEL_ERROR);
		return 1;
	}

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
