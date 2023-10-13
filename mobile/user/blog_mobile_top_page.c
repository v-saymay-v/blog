#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libmysql.h"
#include "libauth.h"
#include "libmobile.h"
#include "libblog.h"
#include "blog_mobile_top_page.h"

int Mobile_User_Top_Page(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	DBRes *dbles;
	NLIST *nlp_tmp;
	int j;
	int in_tbk;
	int in_cmt;
	int in_owner;
	int in_black;
	int in_entry;
	int in_links;
	int in_friend;
	char cha_sql[512];
	char cha_path[1024];
	char cha_temp[512];
	char cha_item_id[256];
	char cha_item_kind[256];
	char cha_item_url[256];
	char cha_item_owner[256];
	char cha_nickname[100];
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_esc;
	char *chp_bdy;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	in_entry = chp_tmp ? atoi(chp_tmp) : 0;

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
	Put_Nlist(nlp_out, "AUTHOR", chp_esc);
	free(chp_esc);

	in_owner = Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_sql);
	in_black = In_Black_List(db, nlp_out, in_owner, NULL);
	if (!in_black) {
		NLIST *nlp_tmp;
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_new_entry.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "ENTWRT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_edit_entry.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "ENTEDT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "ENTWRT", "記事を書く");
		Put_Nlist(nlp_out, "ENTEDT", "記事を編集");
	}

	if (in_entry) {
		sprintf(cha_sql,
			" select T1.n_entry_id"
			",date_format(T1.d_entry_create_time, '%%Y年%%m月%%d日')"
			",T1.c_entry_title"
			",T1.c_entry_body"
			",T1.c_item_id"
			",T1.c_item_kind"
			",T1.c_item_url"
			",T1.c_item_owner"
			",T1.b_comment"
			",T1.b_trackback"
			" from at_entry T1"
			" where T1.n_blog_id = %d"
			" and T1.n_entry_id = %d"
			" and T1.d_entry_create_time < now()"
			" and T1.b_mode != 0",
			in_blog, in_entry);
	} else {
		sprintf(cha_sql,
			" select T1.n_entry_id"
			",date_format(T1.d_entry_create_time, '%%Y年%%m月%%d日')"
			",T1.c_entry_title"
			",T1.c_entry_body"
			",T1.c_item_id"
			",T1.c_item_kind"
			",T1.c_item_url"
			",T1.c_item_owner"
			",T1.b_comment"
			",T1.b_trackback"
			" from at_entry T1"
			" where T1.n_blog_id = %d"
			" and T1.d_entry_create_time < now()"
			" and T1.b_mode != 0"
			" order by T1.d_entry_create_time desc"
			" limit 1",
			in_blog);
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}

	in_tbk = 0;
	in_cmt = 0;
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		in_entry = 0;
		Put_Nlist(nlp_out, "EID", "0");
	} else {
		in_entry = atoi(chp_tmp);
		in_tbk = Get_Total_Trackback_By_Entry_Id(db, nlp_in, nlp_out, in_entry, in_blog, 0);
		in_cmt = Get_Total_Comment_By_Entry_Id(db, nlp_in, nlp_out, in_entry, in_blog, 0);
		Put_Nlist(nlp_out, "EID", chp_tmp);
	}
	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_blog, NULL);

	if (in_entry)
		in_tbk = Get_Total_Trackback_By_Entry_Id(db, nlp_in, nlp_out, in_entry, in_blog, 0);
	if (in_tbk) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_entry);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		sprintf(cha_sql, "%d", in_tbk);
		Put_Nlist(nlp_tmp, "TRB", cha_sql);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_trackback_list.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "TRB", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "TRB", "引用された記事を見る (0)");
	}

	if (in_entry)
		in_cmt = Get_Total_Comment_By_Entry_Id(db, nlp_in, nlp_out, in_entry, in_blog, 0);
	if (in_cmt) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_entry);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		sprintf(cha_sql, "%d", in_cmt);
		Put_Nlist(nlp_tmp, "CMT", cha_sql);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_comment_list.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "CMT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "CMT", "ｺﾒﾝﾄを見る (0)");
	}

	in_friend = 0;
	in_cmt = 0;
	chp_tmp = Db_GetValue(dbres, 0, 9);
	if (in_entry && chp_tmp) {
		in_cmt = atoi(chp_tmp);
		if (in_cmt == CO_CMTRB_FRIEND) {
			in_friend = Is_Ball_Friend(db, nlp_out, in_blog, in_login_blog);
			in_cmt = (in_friend > 0);
		}
	}
	if (!in_black && in_cmt) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_entry);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		Put_Format_Nlist(nlp_tmp, "BID", "%d", in_blog);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_trackback_write.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "TRBWRT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "TRBWRT", "引用記事を書く");
	}

	in_cmt = 0;
	chp_tmp = Db_GetValue(dbres, 0, 8);
	if (in_entry && chp_tmp) {
		in_cmt = atoi(chp_tmp);
		if (in_cmt == CO_CMTRB_FRIEND) {
			in_friend = Is_Ball_Friend(db, nlp_out, in_blog, in_login_blog);
			in_cmt = (in_friend > 0);
		}
	}
	if (!in_black && in_cmt) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_entry);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_comment_write.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "CMTWRT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "CMTWRT", "ｺﾒﾝﾄをする");
	}

	if (in_blog != in_login_blog) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_entry);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		in_cmt = Is_Favorite_Blog(db, nlp_out, in_blog, in_login_blog);
		if (in_cmt) {
			Put_Nlist(nlp_tmp, "PAGE", "df");
			Put_Nlist(nlp_tmp, "FAVORITE", "お気に入りから削除");
		} else {
			Put_Nlist(nlp_tmp, "PAGE", "af");
			Put_Nlist(nlp_tmp, "FAVORITE", "お気に入りに登録");
		}
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_favorite.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "FAVORITE", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "FAVORITE", "お気に入りに登録");
	}

	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "DATETIME", chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "ENTRY_TITLE", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Db_GetValue(dbres, 0, 3);
	if (chp_tmp) {
		chp_bdy = Conv_Long_Ascii(chp_tmp, 56);
		chp_tmp = Conv_Blog_Br(chp_bdy);
		Put_Nlist(nlp_out, "ENTRY_BODY", chp_tmp);
		free(chp_tmp);
		free(chp_bdy);
	}
	cha_item_id[0] = '\0';
	cha_item_kind[0] = '\0';
	cha_item_url[0] = '\0';
	cha_item_owner[0] = '\0';
	chp_tmp = Db_GetValue(dbres, 0, 4);
	if (chp_tmp) {
		strcpy(cha_item_id, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 5);
	if (chp_tmp) {
		strcpy(cha_item_kind, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 6);
	if (chp_tmp) {
		strcpy(cha_item_url, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 7);
	if (chp_tmp) {
		strcpy(cha_item_owner, chp_tmp);
	}
	Db_CloseDyna(dbres);

	/* 関連リンクリストを取る */
	sprintf(cha_sql,
		" select"
			" T1.c_link_title"			/* 0 リンクタイトルL */
			",T1.c_link_url"			/* 1 リンクURL */
			",T1.c_link_ip"				/* 2 リンクIP */
			",T1.b_link_tb"				/* 3 */
		" from"
			" at_sendlink T1"
		" where"
			" T1.n_entry_id=%d"
		" and"
			" T1.n_blog_id=%d"
		" order by"
			" T1.n_link_id",
		in_entry, in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	in_links = Db_GetRowCount(dbres);
	if (in_links) {
		for (j = 0; j < in_links; ++j) {
			chp_tmp = Db_GetValue(dbres, j, 0);
			chp_esc = Db_GetValue(dbres, j, 1);
			if (chp_esc && chp_esc[0]) {
				chp_tmp2 = Db_GetValue(dbres, j, 2);
				if (chp_tmp2) {
//					int in_sub_blog = 0;
					int in_sub_entry = 0;
					if (Blog_To_Temp(db, nlp_out, atoi(chp_tmp2), cha_temp)) {
						Db_CloseDyna(dbres);
						return 1;
					}
					sprintf(cha_sql, "%s%s%s/%s?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML);
					strcat(cha_sql, "eid=%d&bid=%s");
					sscanf(chp_esc, cha_sql, &in_sub_entry, cha_path);
					chp_tmp2 = Escape_HtmlString(chp_tmp);
					if (in_sub_entry) {
						sprintf(cha_sql, "select b_tb_accept from at_trackback where c_tb_ip = '%d' and n_entry_id = %d and c_tb_url like '%%?eid=%d'", in_blog, in_sub_entry, in_entry);
						dbles = Db_OpenDyna(db, cha_sql);
						if (!dbles) {
							sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
							Put_Nlist(nlp_out, "ERROR", cha_sql);
							return 1;
						}
						chp_tmp = Db_GetValue(dbles, 0, 0);
						if (chp_tmp && atoi(chp_tmp) > 0) {
							if (j == 0) {
								Put_Nlist(nlp_out, "ENTRY_BODY", "<br /><br />関連リンク：<br>\n");
							}
							nlp_tmp = Init_Nlist();
							Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
							sprintf(cha_sql, "%d", in_sub_entry);
							Put_Nlist(nlp_tmp, "TBEID", cha_sql);
							Put_Nlist(nlp_tmp, "TBBLOG", cha_path);
							Conv_Depend_String(nlp_tmp);
							Put_Nlist(nlp_tmp, "NAME", chp_tmp2);
							chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_article.skl");
							if (chp_tmp) {
								Put_Nlist(nlp_out, "ENTRY_BODY", chp_tmp);
								free(chp_tmp);
							}
							Finish_Nlist(nlp_tmp);
						}
						Db_CloseDyna(dbles);
					} else if (chp_tmp) {
						Put_Format_Nlist(nlp_out, "ENTRY_BODY", "%s<br>\n", chp_tmp2);
					}
					free(chp_tmp2);
				} else if (chp_tmp) {
					Put_Format_Nlist(nlp_out, "ENTRY_BODY", "%s<br>\n", chp_tmp);
				}
			} else if (chp_tmp) {
				Put_Format_Nlist(nlp_out, "ENTRY_BODY", "%s<br>\n", chp_tmp);
			}
		}
	}
//	if (cha_item_kind[0] == 'L' || cha_item_kind[0] == 'l' || cha_item_kind[0] == 'C' || cha_item_kind[0] == 'c') {
//		chp_tmp = Get_DBB_Topic(db, nlp_out, in_blog, cha_item_kind, cha_item_id, cha_item_owner, cha_item_url);
//		if (chp_tmp) {
//			Put_Nlist(nlp_out, "ENTRY_BODY", "<br />DBBトピック：<br>\n");
//			Put_Nlist(nlp_out, "ENTRY_BODY", chp_tmp);
//		}
//	}
	Db_CloseDyna(dbres);

	if (in_entry)
		in_tbk = Get_Id_Prev_Entry_Create_Time(db, in_entry, in_blog);
	if (in_entry && in_tbk != CO_ERROR) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_tbk);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_prev_article.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "PREV", chp_tmp);
			free(chp_tmp);
		} else {
			Put_Nlist(nlp_out, "PREV", "<< 前へ");
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "PREV", "<< 前へ");
	}

	if (in_entry)
		in_cmt = Get_Id_Next_Entry_Create_Time(db, in_entry, in_blog);
	if (in_entry && in_cmt != CO_ERROR) {
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_blog, NULL);
		sprintf(cha_sql, "%d", in_cmt);
		Put_Nlist(nlp_tmp, "EID", cha_sql);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_next_article.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "NEXT", chp_tmp);
			free(chp_tmp);
		} else {
			Put_Nlist(nlp_out, "NEXT", "次へ >>");
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "NEXT", "次へ >>");
	}

	if (in_blog != in_login_blog) {
		Put_Nlist(nlp_out, "MENU_START", "<!--");
		Put_Nlist(nlp_out, "MENU_END", "-->");
	}

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_user_top.skl");

	return 0;
}
