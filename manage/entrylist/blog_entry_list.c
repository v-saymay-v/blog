/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_entry_list.c
 * First-Created:	2004/08/12 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	既投稿記事の一覧画面
 *
-* ------------------------------------------------------------------------
 * Change-Log: $Id: blog_entry_list.c,v 1.99 2009/02/24 04:37:18 hori Exp $
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_entry_list.c,v 1.99 2009/02/24 04:37:18 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
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
void put_error_data(NLIST *nlp_in, NLIST *nlp_out, char *chp_action)
{
	char *chpa_esc[] = {
		"blogid", "BTN_DISP_DELETE_ENTRY_ASK.x", "BTN_DISP_DELETE_ENTRY_ASK.y", "BTN_DELETE_ENTRY.x", "BTN_DELETE_ENTRY.y", NULL
	};

	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	Put_Nlist(nlp_out, "ACTION", chp_action ? chp_action : CO_CGI_ENTRY_LIST);
	Put_Nlist(nlp_out, "BUTTON", "BTN_DISP_ENTRYLIST");
	return ;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_page_by_entryid()
 * Description:
 *	記事IDが含まれるページ数を調べる
 *	記事IDの記事がない場合は0を返す
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 ページ番号
 *	エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int get_page_by_entryid(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,int in_entry_id	/* 対象のエントリーID */
	,int in_perpage
	,int in_blog
)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_row;
	int in_page;
	int in_exist_flg;
	int i;

	strcpy(cha_sql, "select T1.n_entry_id");	/* エントリID */
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_entry_create_time desc, T1.n_entry_id desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "表示ページ数を得られませんでした。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	in_row = Db_GetRowCount(dbres);
	if (!in_row) {
		/* 最初のページ */
		return 0;
	}
	in_exist_flg = 0;
	for(i = 0, in_page = -1; i < in_row; i++) {
		if (i % in_perpage == 0) {
			in_page++;
		}
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp && atoi(chp_tmp) == in_entry_id) {
			in_exist_flg = 1;
			break;
		}
	}
	if (!in_exist_flg) {
		in_page = 0;
	}
	Db_CloseDyna(dbres);
	return in_page;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_entrylist()
 * Description:		既投稿リストを表示する
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	記事がない(トップページに飛ばす) 1
 *	エラー CO_ERROR
-* ------------------------------------------------------------------------*/
int disp_page_entrylist(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_entry_id;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char *chp_fromuser;
	char cha_sql[512];
	char cha_str[8192];
	int in_total_page;
	int in_total_entry;
	int in_page;
	int in_row;
	int in_checked_count;
	int in_checked_flg;
	int i;
	int j;

	in_total_entry = Get_Total_Entry(db, nlp_in, nlp_out, 0, in_blog);
	if (in_total_entry == 0) {
		Put_Nlist(nlp_out, "BUTTONTOPPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONPREVPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONNEXTPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONLASTPAGE", "disabled");
		Put_Nlist(nlp_out, "MAE", "mae_bd.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_bd.gif");
		Put_Nlist(nlp_out, "TUGI", "tugi_bd.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_bd.gif");
		Put_Nlist(nlp_out, "LIST", "<tr class=\"a2\"><td colspan=\"7\" align=\"center\" bgcolor=\"#ffffff\">記事がありません。</td></tr>\n");
		return 0;
//		Put_Nlist(nlp_out, "ERROR", "記事がありません。");
//		return 1;
	}
	/* トータルページ数 */
	in_total_page = in_total_entry / CO_PERPAGE_ARTICLE;
	if (in_total_entry % CO_PERPAGE_ARTICLE) {
		in_total_page++;
	}
	/* ページ引継ぎ */
	if (Get_Nlist(nlp_in, "page", 1)) {
		in_page = atoi(Get_Nlist(nlp_in, "page", 1));
	} else if (Get_Nlist(nlp_in, "entry_id", 1)) {
		in_page = get_page_by_entryid(db, nlp_in, nlp_out, atoi(Get_Nlist(nlp_in, "entry_id", 1)), CO_PERPAGE_ARTICLE, in_blog);
	} else {
		in_page = 0;
	}
	if (in_page == CO_ERROR) {
		return CO_ERROR;
	}
	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	/* ボタンが押されたことによる移動 */
	if (Get_Nlist(nlp_in, "BTN_TOPPAGE.x", 1)) {
		in_page = 0;
	} else if (Get_Nlist(nlp_in, "BTN_PREVPAGE.x", 1)) {
		in_page--;
	} else if (Get_Nlist(nlp_in, "BTN_NEXTPAGE.x", 1)) {
		in_page++;
	} else if (Get_Nlist(nlp_in, "BTN_LASTPAGE.x", 1)) {
		in_page = in_total_page - 1;
	}
	/* ページ数調整 */
	if (in_page < 0) {
		in_page = 0;
	} else if (in_page >= in_total_page) {
		in_page = in_total_page - 1;
	}
	/* ボタン無効化 */
	if (in_page == 0) {
		Put_Nlist(nlp_out, "BUTTONTOPPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONPREVPAGE", "disabled");
		Put_Nlist(nlp_out, "MAE", "mae_bd.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_bd.gif");
	} else {
		Put_Nlist(nlp_out, "MAE", "mae_b.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_b.gif");
	}
	if (in_page == in_total_page - 1) {
		Put_Nlist(nlp_out, "BUTTONNEXTPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONLASTPAGE", "disabled");
		Put_Nlist(nlp_out, "TUGI", "tugi_bd.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_bd.gif");
	} else {
		Put_Nlist(nlp_out, "TUGI", "tugi_b.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_b.gif");
	}
	/* page埋め込み */
	sprintf(cha_str, "%d", in_page);
	Build_HiddenEncode(nlp_out, "HIDDEN", "page", cha_str);
	/* 常に埋める */
	Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_ENTRYLIST", "1");
	/* ページ数情報 */
	sprintf(cha_str, "全%d件(%d/%d)", in_total_entry, in_page + 1, in_total_page);
	Put_Nlist(nlp_out, "PAGE", cha_str);
	if (Make_Tmptable_For_Count(db, nlp_out, in_blog, 1)) {
		return CO_ERROR;
	}
	/* ページ内容 */
	strcpy(cha_sql, " select T1.n_entry_id");				/* 0 エントリID */
	strcat(cha_sql, ",T1.c_entry_title");					/* 1 エントリタイトル */
	strcat(cha_sql, ",T1.b_mode");							/* 2 公開モード */
	strcat(cha_sql, ",T2.c_category_name");					/* 3 カテゴリ名 */
	strcat(cha_sql, ",coalesce(T3.n_comment_count, 0)");	/* 4 コメント数 */
	strcat(cha_sql, ",coalesce(T4.n_tb_count, 0)");			/* 5 トラックバック数 */
	strcat(cha_sql, ",date_format(T1.d_entry_create_time, '%y年%m月%d日%H時%i分')");	/* 6 投稿時間 */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " left join tmp_comment T3");
	strcat(cha_sql, " on T1.n_entry_id = T3.n_entry_id");
	strcat(cha_sql, " left join tmp_trackback T4");
	strcat(cha_sql, " on T1.n_entry_id = T4.n_entry_id");
	strcat(cha_sql, ",at_category T2");
	strcat(cha_sql, " where T1.n_category_id = T2.n_category_id");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	sprintf(cha_sql + strlen(cha_sql), " and T2.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_entry_create_time desc, T1.n_entry_id desc");
	sprintf(cha_sql + strlen(cha_sql), " limit %d, %d", in_page * CO_PERPAGE_ARTICLE, CO_PERPAGE_ARTICLE);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "記事情報を得るクエリに失敗しました。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	in_row = Db_GetRowCount(dbres);
	if (!in_row) {
		Put_Nlist(nlp_out, "BUTTONTOPPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONPREVPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONNEXTPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONLASTPAGE", "disabled");
		Put_Nlist(nlp_out, "MAE", "mae_bd.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_bd.gif");
		Put_Nlist(nlp_out, "TUGI", "tugi_bd.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_bd.gif");
		Put_Nlist(nlp_out, "LIST", "<tr class=\"a2\"><td colspan=\"7\" align=\"center\" bgcolor=\"#B3E1E0\">記事がありません。</td></tr>\n");
		Db_CloseDyna(dbres);
		return 0;
	}
	in_checked_count = Get_NlistCount(nlp_in, "delete_entry");
	for(i = 0; i < in_row; i++) {
		Put_Nlist(nlp_out, "LIST", "<tr class=\"a2\">");
		if (g_in_cart_mode == CO_CART_RESERVE) {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" bgcolor=\"#B3E1E0\">");
		} else {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" bgcolor=\"#c6e3ff\">");
		}
		in_checked_flg = 0;
		/* 削除チェックボックス */
		chp_entry_id = Db_GetValue(dbres, i, 0);
		for(j = 0; j < in_checked_count; j++) {
			chp_tmp = Get_Nlist(nlp_in, "delete_entry", j + 1);
			if (chp_entry_id && chp_tmp && atoi(chp_entry_id) == atoi(chp_tmp)) {
				in_checked_flg = 1;
			}
		}
		if (in_checked_flg) {
			Build_Checkbox(nlp_out, "LIST", "delete_entry", chp_entry_id, 1);
		} else {
			Build_Checkbox(nlp_out, "LIST", "delete_entry", chp_entry_id, 0);
		}
		memset(cha_str, '\0', sizeof(cha_str));
		strcpy(cha_str, "</td>\n");
		/* タイトル(下書きはリンクしない) */
		strcat(cha_str, "<td align=\"left\" height=\"27\">");
		chp_tmp = Db_GetValue(dbres, i, 1);
		chp_tmp2 = Db_GetValue(dbres, i, 2);
		if (chp_tmp2 && atoi(chp_tmp2) == 1) {
			if (g_in_dbb_mode) {
				sprintf(cha_str + strlen(cha_str), "<a href=\"%s%s%s/%s/?eid=%s#entry\" target=\"dbb_blog_main_wnd\">"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_entry_id);
			} else if (g_in_short_name) {
				sprintf(cha_str + strlen(cha_str), "<a href=\"%s%s%s/%08d/?eid=%s#entry\" target=\"dbb_blog_main_wnd\">"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_entry_id);
			} else {
				sprintf(cha_str + strlen(cha_str), "<a href=\"%s/%s?eid=%s&bid=%d#entry\" target=\"dbb_blog_main_wnd\">"
					,g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_entry_id, in_blog);
			}
		}
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		} else {
			strcat(cha_str, "(無題)");
		}
		if (chp_tmp2 && atoi(chp_tmp2) == 1) {
			strcat(cha_str, "</a>");
		}
		strcat(cha_str, "</td>\n");
		/* 編集リンク */
		strcat(cha_str, "<td align=\"center\" width=\"50\" height=\"27\">");
		sprintf(cha_str + strlen(cha_str)
			, "<a href=\"%s%s%s/%s?BTN_DISP_MODIFYENTRY=1&blogid=%d&entry_id=%s&page=%d&from_user=%d\"><img src=\"%s/henshu_m.gif\" border=\"0\"></a></td>\n"
			, g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY, in_blog, chp_entry_id,
			in_page, chp_fromuser ? atoi(chp_fromuser) : 0, g_cha_admin_image);
		/* 編集モード */
		strcat(cha_str, "<td align=\"center\" width=\"50\" height=\"27\">");
		chp_tmp = Db_GetValue(dbres, i, 2);
		if (chp_tmp && atoi(chp_tmp) == 1) {
			strcat(cha_str, "公開");
		} else {
			strcat(cha_str, "下書き");
		}
		strcat(cha_str, "</td>\n");
		/* カテゴリー */
		if (!g_in_dbb_mode) {
			strcat(cha_str, "<td align=\"center\" height=\"27\">");
			chp_tmp = Db_GetValue(dbres, i, 3);
			if (chp_tmp && *chp_tmp) {
				chp_escape = Escape_HtmlString(chp_tmp);
				strcat(cha_str, chp_escape);
				free(chp_escape);
			} else {
				strcat(cha_str, "&nbsp;");
			}
			strcat(cha_str, "</td>\n");
		}
		strcat(cha_str, "<td align=\"center\" width=\"50\" height=\"27\">");
		/* コメント数 */
		chp_tmp = Db_GetValue(dbres, i, 4);
		if (chp_tmp && atoi(chp_tmp)) {
			sprintf(cha_str + strlen(cha_str)
				, "<a href=\"%s%s%s/%s?BTN_DISP_COMMENTLIST=1&blogid=%d&entry_id=%s&from_user=%d\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_COMMENT_LIST, in_blog, chp_entry_id, chp_fromuser ? atoi(chp_fromuser) : 0, chp_tmp);
		} else {
			strcat(cha_str, "0");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "<td align=\"center\" width=\"50\" height=\"27\">");
		/*トラックバック*/
		chp_tmp = Db_GetValue(dbres, i, 5);
		if (chp_tmp && atoi(chp_tmp)) {
			sprintf(cha_str + strlen(cha_str)
				, "<a href=\"%s%s%s/%s?BTN_DISP_TRACKBACKLIST=1&blogid=%d&entry_id=%s&from_user=%d\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_TB_LIST, in_blog, chp_entry_id, chp_fromuser ? atoi(chp_fromuser) : 0, chp_tmp);
		} else {
			strcat(cha_str, "0");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "<td align=\"center\" height=\"27\">");
		/* 時間 */
		chp_tmp = Db_GetValue(dbres, i, 6);
		if (chp_tmp) {
			strcat(cha_str, chp_tmp);
		} else {
			strcat(cha_str, "--年--月--日--時--分");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "</tr>\n\n");
		Put_Nlist(nlp_out, "LIST", cha_str);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_entry()
 * Description:
 *	エントリーを削除する。エントリーに付随してトラックバック、コメント、
 *	添付ファイルも処理。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int delete_entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char cha_sql[8192];
	char cha_where[1024];
	char cha_temp[20];
	char cha_str[2048];
	int in_checked_count;
	int in_skip;
	int i;

	in_checked_count = Get_NlistCount(nlp_in, "delete_entry");
	if (!in_checked_count) {
		Put_Nlist(nlp_out, "ERROR", "削除するファイルを取得できませんでした。");
		return 1;
	}
	/* entry_idの羅列を作る */
	memset(cha_where, '\0', sizeof(cha_where));
	for(i = 0; i < in_checked_count; i++) {
		if (i) {
			strcat(cha_where, ", ");
		}
		/* 一応 */
		chp_tmp = My_Escape_SqlString(db, Get_Nlist(nlp_in, "delete_entry", i + 1));
		strcat(cha_where, chp_tmp);
		free(chp_tmp);
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
			Put_Nlist(nlp_out, "ERROR", "トランザクションの開始に失敗しました");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		/* エントリ本体 */
		if (g_in_dbb_mode && g_in_admin_mode) {
			sprintf(cha_sql, "insert into at_entry_deleted select * from at_entry where n_entry_id in (%s) and n_blog_id = %d", cha_where, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
		strcpy(cha_sql, "delete from at_entry");
		strcat(cha_sql, " where n_entry_id in (");
		strcat(cha_sql, cha_where);
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* コメント */
		if (g_in_dbb_mode && g_in_admin_mode) {
			sprintf(cha_sql, "insert into at_comment_deleted select * from at_comment where n_entry_id in (%s) and n_blog_id = %d", cha_where, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
		strcpy(cha_sql, "delete from at_comment");
		strcat(cha_sql, " where n_entry_id in (");
		strcat(cha_sql, cha_where);
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(2)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* トラックバック受信 */
		if (g_in_dbb_mode && g_in_admin_mode) {
			sprintf(cha_sql, "insert into at_trackback_deleted select * from at_trackback where n_entry_id in (%s) and n_blog_id = %d", cha_where, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
		strcpy(cha_sql, "delete from at_trackback");
		strcat(cha_sql, " where n_entry_id in (");
		strcat(cha_sql, cha_where);
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(3)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* トラックバック送信 */
		if (g_in_dbb_mode && g_in_admin_mode) {
			sprintf(cha_sql, "insert into at_sendtb_deleted select * from at_sendtb where n_entry_id in (%s) and n_blog_id = %d", cha_where, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
		strcpy(cha_sql, "delete from at_sendtb");
		strcat(cha_sql, " where n_entry_id in (");
		strcat(cha_sql, cha_where);
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(3)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* 添付ファイルテーブル */
		strcpy(cha_sql, "delete from at_uploadfile");
		strcat(cha_sql, " where n_entry_id in (");
		strcat(cha_sql, cha_where);
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(4)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* 送信リンクテーブル */
		if (g_in_dbb_mode && g_in_admin_mode) {
			sprintf(cha_sql, "insert into at_sendlink_deleted select * from at_sendlink where n_entry_id in (%s) and n_blog_id = %d", cha_where, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
		strcpy(cha_sql, "delete from at_sendlink");
		strcat(cha_sql, " where n_entry_id in (");
		strcat(cha_sql, cha_where);
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(4)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		/* 受信リンクテーブル */
		if (g_in_dbb_mode && g_in_admin_mode) {
			sprintf(cha_sql, "insert into at_recvlink_deleted select * from at_recvlink where n_entry_id in (%s) and n_blog_id = %d", cha_where, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
			sprintf(cha_sql, "insert into at_recvlink_deleted select * from at_recvlink where n_from_entry in (%s) and n_blog_id = %d", cha_where, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
		strcpy(cha_sql, "delete from at_recvlink");
		strcat(cha_sql, " where n_entry_id in (");
		strcat(cha_sql, cha_where);
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db,cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(4)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if (g_in_dbb_mode) {
			chp_tmp = strtok(cha_where, ",");
			while (chp_tmp) {
				Blog_To_Temp(db, nlp_out, in_blog, cha_temp);
				sprintf(cha_str, "%s%s%s/%s?eid=%s&bid=%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_tmp, cha_temp);
				sprintf(cha_sql, "delete from at_sendlink where c_link_url = '%s'", cha_str);
				if (Db_ExecSql(db,cha_sql)) {
					Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(4)");
					Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
					Rollback_Transact(db);
					return 1;
				}
				sprintf(cha_str, "%s%s%s/%s/?eid=%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, cha_temp, chp_tmp);
				sprintf(cha_sql, "delete from at_trackback where c_tb_url = '%s'", cha_str);
				if (Db_ExecSql(db,cha_sql)) {
					Put_Nlist(nlp_out, "ERROR", "エントリの削除に失敗しました。(4)");
					Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
					Rollback_Transact(db);
					return 1;
				}
				chp_tmp = strtok(NULL, ",");
			}
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_delete_entry_ask()
 * Description:		記事の削除確認画面の表示
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int disp_page_delete_entry_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	char cha_str[CO_PERPAGE_ARTICLE * 1024];
	int in_checked_count;
	int in_row;
	int i;

	in_checked_count = Get_NlistCount(nlp_in, "delete_entry");
	if (!in_checked_count) {
		Put_Nlist(nlp_out, "ERROR", "削除する記事を選択してください。");
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "page", 1);
	if (chp_tmp){
		Build_HiddenEncode(nlp_out, "HIDDEN", "page", chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_tmp){
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_tmp);
	}
	strcpy(cha_sql, "select T1.n_entry_id");	/* 0 記事ID */
	strcat(cha_sql, ", T1.c_entry_title");		/* 1 記事タイトル */
	strcat(cha_sql, ", date_format(T1.d_entry_create_time,'%y年%m月%d日%H時%i分')");	/* 2 投稿時間 */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql,  " where T1.n_entry_id in (");
	for(i = 0; i < in_checked_count; i++) {
		if (i) {
			strcat(cha_sql, ", ");
		}
		strcat(cha_sql, Get_Nlist(nlp_in, "delete_entry", i + 1));
	}
	strcat(cha_sql, ")");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_entry_create_time desc, T1.n_entry_id desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "削除する記事の情報取得に失敗しました。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if (!in_row) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "選択したファイルは既に削除されています。");
		return 1;
	}
	if (in_row != in_checked_count) {
		Put_Nlist(nlp_out, "MSG", "(選択したファイルのうち一部が既に削除されています。)<br>");
	}
	for(i = 0; i < in_row; i++) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "delete_entry", Db_GetValue(dbres, i, 0));
		strcpy(cha_str, "<tr style=\"background-color:#F0F5FD;\">");
		strcat(cha_str, "<td nowrap>");
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		} else {
			strcat(cha_str, "(無題)");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "<td nowrap>\n");
		chp_tmp = Db_GetValue(dbres, i, 2);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_str, chp_tmp);
		} else {
			strcat(cha_str, "--年--月--日--時--分");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "</tr>\n");
		Put_Nlist(nlp_out, "DELETE_TITLE", cha_str);
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
	char *chp_fromuser;
	char *chp_skel;
	char *chp_tmp;
	char *chp_action;
	char cha_blog[32];
	char cha_jump[1024];
	int in_blog;
	int in_error;
	int in_ret;
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
	Set_SkelPath(g_cha_admin_skeleton);

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	chp_action = NULL;
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
					Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY_LIST);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "認証情報を取得できません。<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_ENTRY_LIST);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY_LIST);
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
	if (g_in_hb_mode && in_orig == INT_MAX && Get_Blog_Auth(db, nlp_out, g_in_login_owner, in_blog) < CO_AUTH_WRITE) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを編集する権限がありません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログを作成されていない方はご利用になれません。<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);

	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_fromuser && atoi(chp_fromuser)) {
		Build_HiddenEncode(nlp_out, "FROMUSER", "from_user", chp_fromuser);
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_fromuser);
		Put_Format_Nlist(nlp_out, "FROM_USER", "&from_user=%s", chp_fromuser);
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
	Put_Format_Nlist(nlp_out, "FROM_USER", "&blogid=%d", in_blog);

	in_ret = 0;
	in_error = 0;
	chp_skel = NULL;
	if (Get_Nlist(nlp_in, "BTN_DISP_DELETE_ENTRY_ASK.x", 1)) {
		if (disp_page_delete_entry_ask(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			in_ret = disp_page_entrylist(db, nlp_in, nlp_out, in_blog);
			if (in_ret) {
				chp_action = CO_CGI_MENU;
				in_error++;
			}
			chp_skel = CO_SKEL_ENTRY_LIST;
		} else {
			chp_skel = CO_SKEL_DELETE_ENTRY_ASK;
		}
	} else if (Get_Nlist(nlp_in, "BTN_DELETE_ENTRY.x", 1)) {
		if (delete_entry(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = CO_SKEL_ENTRY_LIST;
		} else if (Create_Rss(db, nlp_out, in_blog)) {
			chp_skel = CO_SKEL_ENTRY_LIST;
			in_error++;
		} else {
			sprintf(cha_jump, "%s%s%s/%s?BTN_DISP_ENTRYLIST=1&blogid=%d&from_user=%s",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY_LIST,
				in_blog, chp_fromuser ? chp_fromuser : "0");
			Put_Nlist(nlp_out, "MESSAGE", "記事を削除しました。");
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
			chp_skel = CO_SKEL_CONFIRM;
		}
	/* 初期表示、ページ移動、および記事更新から戻ってきたとき */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_ENTRYLIST", 1) || Get_Nlist(nlp_in, "BTN_DISP_ENTRYLIST.x", 1) || Get_Nlist(nlp_in, "entry_id", 1)) {
		in_ret = disp_page_entrylist(db, nlp_in, nlp_out, in_blog);
		if (in_ret) {
			chp_action = CO_CGI_MENU;
			in_error++;
		}
		chp_skel = CO_SKEL_ENTRY_LIST;
	} else {
		Put_Nlist(nlp_out, "ERROR", "直接CGIを実行できません。<br>");
		chp_action = CO_CGI_MENU;
		in_error++;
	}
	if (in_error || in_ret == CO_ERROR || !chp_skel) {
		put_error_data(nlp_in, nlp_out, chp_action);
		Page_Out(nlp_out, CO_SKEL_ERROR);
	} else if (in_ret == 1) {
		if (chp_fromuser && atoi(chp_fromuser)) {
			if (g_in_short_name) {
				printf("Location: %s%s%s/%08d/\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
			} else {
				printf("Location: %s/%s/%d\n\n", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
			}
		} else {
			printf("Location: %s%s%s/%s?blogid=%d\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
		}
	} else {
		Page_Out(nlp_out, chp_skel);
	}

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
