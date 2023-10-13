/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_comment_list.c
 * First-Created:	2004/08/31 ��¼ ����Ϻ
%* ------------------------------------------------------------------------
 * Module-Description:
 *	����Ƶ����ΰ�������
 *
-* ------------------------------------------------------------------------
 * Change-Log:
 *	2005/07/05 �غ�����
 *		�����˽񤭴����롣
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_comment_list.c,v 1.87 2009/09/14 09:54:59 hosi Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "libcgi2.h"
#include "libdb2.h"
#include "libnet.h"
#include "libcommon2.h"
#include "libauth.h"
#include "libblog.h"
#include "libblogcart.h"
#include "libblogreserve.h"

#define CO_PERPAGE		10
#define CO_SELECTBOX	0
#define CO_MULTISELECT	1
#define CO_BUFSIZE		10000

/*
+* ------------------------------------------------------------------------
 * Function:            put_error_data()
 * Description:
 *      ���顼����������
%* ------------------------------------------------------------------------
 * Return:
 *      (̵��)
-* ------------------------------------------------------------------------*/
void put_error_data(NLIST *nlp_in, NLIST *nlp_out, char* chp_action)
{
	char *chpa_esc[] = {
		"BTN_ACCEPT_COMMENT.x", "BTN_ACCEPT_COMMENT.y",
		"BTN_DISP_ACCEPT_COMMENT_ASK.x", "BTN_DISP_ACCEPT_COMMENT_ASK.y",
		"BTN_DISP_DELETE_COMMENT_ASK.x", "BTN_DISP_DELETE_COMMENT_ASK.y",
		"BTN_DELETE_COMMENT.x", "BTN_DELETE_COMMENT.y", NULL
	};

	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	Put_Nlist(nlp_out, "ACTION", chp_action);
	Put_Nlist(nlp_out, "BUTTON", "BTN_DISP_COMMENTLIST");
	return ;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_commentlist()
 * Description:
 *	�����ȥꥹ�Ȥ�ɽ������
%* ------------------------------------------------------------------------
 * Return:
 *	���ｪλ 0
 *	���顼�� CO_ERROR
 *	�Ǹ��1�Ĥ���������κ����� 1
-* ------------------------------------------------------------------------*/
int disp_page_commentlist(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_entry_id;
	char *chp_comment_id;
	char *chp_escape;
	char cha_sql[512];
	char cha_str[8192];
	char cha_host[256];
	int in_count;
	int in_total_page;
	int in_total_comment;
	int in_page;
	int in_checked_count;
	int in_checked_flg;
	int i;
	int j;

	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	if(!chp_entry_id || !*chp_entry_id) {
		in_total_comment = Get_Total_Comment(db, nlp_in, nlp_out, in_blog, 1);
	} else {
		in_total_comment = Get_Total_Comment_By_Entry_Id(db, nlp_in, nlp_out, atoi(chp_entry_id), in_blog, 1);
	}
	if(in_total_comment == CO_ERROR) {
		return CO_ERROR;
	}
	in_total_page = in_total_comment / CO_PERPAGE_ARTICLE;
	if(in_total_comment % CO_PERPAGE_ARTICLE) {
		in_total_page++;
	}
	if(in_total_comment == 0) {
		Put_Nlist(nlp_out, "BUTTONTOPPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONPREVPAGE", "disabled");
		Put_Nlist(nlp_out, "MAE", "mae_bd.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_bd.gif");
		Put_Nlist(nlp_out, "BUTTONNEXTPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONLASTPAGE", "disabled");
		Put_Nlist(nlp_out, "TUGI", "tugi_bd.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_bd.gif");
		Put_Nlist(nlp_out, "LIST", "<tr class=\"a2\"><td align=\"center\" colspan=\"7\" bgcolor=\"#ffffff\">�����ȤϤ���ޤ���</td></tr>\n");
		return 0;
	}
	/* �ܥ���Ĵ�� */
	if(!Get_Total_Trackback(db, nlp_in, nlp_out, in_blog, 1)) {
		Put_Nlist(nlp_out, "TRACKBACKLIST", "disabled");
	}
	if(Get_Nlist(nlp_in, "page", 1)) {
		in_page = atoi(Get_Nlist(nlp_in, "page", 1));
	} else {
		in_page = 0;
	}
	/* �ڡ�����ư�ܥ��󤬲����줿�Ȥ� */
	if(Get_Nlist(nlp_in, "BTN_TOPPAGE.x", 1)) {
		in_page = 0;
	} else if(Get_Nlist(nlp_in, "BTN_PREVPAGE.x", 1)) {
		in_page--;
	} else if(Get_Nlist(nlp_in, "BTN_NEXTPAGE.x", 1)) {
		in_page++;
	} else if(Get_Nlist(nlp_in, "BTN_LASTPAGE.x", 1)) {
		in_page = in_total_page - 1;
	}
	/* �ڡ���Ĵ�� */
	if(in_page < 0) {
		in_page = 0;
	} else if(in_page >= in_total_page) {
		in_page = in_total_page - 1;
	}
	/*�ڡ����᤯��ܥ���*/
	if(in_page == 0){
		Put_Nlist(nlp_out, "BUTTONTOPPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONPREVPAGE", "disabled");
		Put_Nlist(nlp_out, "MAE", "mae_bd.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_bd.gif");
	} else {
		Put_Nlist(nlp_out, "MAE", "mae_b.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_b.gif");
	}
	if(in_page == in_total_page - 1) {
		Put_Nlist(nlp_out, "BUTTONNEXTPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONLASTPAGE", "disabled");
		Put_Nlist(nlp_out, "TUGI", "tugi_bd.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_bd.gif");
	} else {
		Put_Nlist(nlp_out, "TUGI", "tugi_b.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_b.gif");
	}
	/* page������ */
	sprintf(cha_str, "%d", in_page);
	Build_HiddenEncode(nlp_out, "HIDDEN", "page", cha_str);
	/* ��������� */
	Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_COMMENTLIST", "1");
	/* �ڡ������������� */
	Put_Format_Nlist(nlp_out, "PAGE", "��%d��(%d/%d)", in_total_comment, in_page + 1, in_total_page);
	/* �������ȤΥ����Ȥ�ɽ�����Ƥ���Ȥ��ϡ����������ȥ��ɽ�� */
	if(chp_entry_id) {
		strcpy(cha_sql, "select T1.c_entry_title");
		strcat(cha_sql, " from at_entry T1");
		strcat(cha_sql, " where n_entry_id = ");
		strcat(cha_sql, chp_entry_id);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "������������륯����˼��Ԥ��ޤ�����");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Format_Nlist(nlp_out, "PAGE", "<br><i>������%s�פؤΥ�����</i>", chp_escape);
			free(chp_escape);
		} else {
			Put_Nlist(nlp_out, "PAGE", "<br><i>������(̵��)�פؤΥ�����</i>");
		}
		Db_CloseDyna(dbres);
	}
	/* ������ */
	strcpy(cha_sql, " select T1.n_comment_id");	/* 0 ������ID */
	strcat(cha_sql, ",T1.c_comment_author");	/* 1 �����Ȥ����� */
	strcat(cha_sql, ",T1.c_comment_mail");		/* 2 �����ȤΥ᡼�륢�ɥ쥹*/
	strcat(cha_sql, ",T1.c_comment_url");		/* 3 �����Ȥ�URL */
	strcat(cha_sql, ",T2.n_entry_id");			/* 4 �Ƶ���ID */
	strcat(cha_sql, ",T2.b_mode");				/* 5 �Ƶ��������⡼�� */
	strcat(cha_sql, ",T2.c_entry_title");		/* 6 �Ƶ��������ȥ� */
	strcat(cha_sql, ",date_format(T1.d_comment_create_time, '%yǯ%m��%d��%H��%iʬ')");	/* 7 �����Ȼ��� */
	strcat(cha_sql, ",T1.c_comment_ip");		/* 8 ������IP */
	strcat(cha_sql, ",T1.b_comment_accept");	/* 9 ��ǧ */
	strcat(cha_sql, ",T1.b_comment_filter");	/* 10 �ե��륿���ˤ����ä��� */
	strcat(cha_sql, " from at_comment T1");
	strcat(cha_sql, ",at_entry T2");
	strcat(cha_sql, " where T1.n_entry_id = T2.n_entry_id");
	if(chp_entry_id && *chp_entry_id) {
		strcat(cha_sql, " and T1.n_entry_id = ");
		strcat(cha_sql, chp_entry_id);
	}
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " and T2.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_comment_create_time desc");
	sprintf(cha_sql + strlen(cha_sql), " limit %d, %d", in_page * CO_PERPAGE_ARTICLE, CO_PERPAGE_ARTICLE);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�����Ȥ����륯���꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	in_count = Db_GetRowCount(dbres);
	if(!in_count) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "�����Ȥ�����ޤ���(2)");
		return 1;
	}
	in_checked_count = Get_NlistCount(nlp_in, "delete_comment");
	for(i = 0; i < in_count; i++) {
		Put_Nlist(nlp_out, "LIST", "\n<tr class=\"a2\">");
		if (g_in_cart_mode == CO_CART_RESERVE) {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#B3E1E0\">");
		} else {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#c6e3ff\">");
		}
		in_checked_flg = 0;
		chp_comment_id = Db_GetValue(dbres, i, 0);
		chp_entry_id = Db_GetValue(dbres, i, 4);
		sprintf(cha_str, "%s:%s", chp_entry_id ? chp_entry_id : "0", chp_comment_id ? chp_comment_id : "0");
		for(j = 0; j < in_checked_count; j++) {
			chp_tmp2 = Get_Nlist(nlp_in, "delete_comment", j + 1);
			if(chp_comment_id && chp_tmp2 && atoi(chp_comment_id) == atoi(chp_tmp2)) {
				in_checked_flg = 1;
			}
		}
		if(in_checked_flg) {
			Build_Checkbox(nlp_out, "LIST", "delete_comment", cha_str, 1);
		} else {
			Build_Checkbox(nlp_out, "LIST", "delete_comment", cha_str, 0);
		}
		Put_Nlist(nlp_out, "LIST", "</td>\n");
		if (g_in_cart_mode == CO_CART_RESERVE) {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#B3E1E0\">");
		} else {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#c6e3ff\">");
		}
		in_checked_flg = 0;
		for(j = 0; j < in_checked_count; j++) {
			chp_tmp2 = Get_Nlist(nlp_in, "accept_comment", j + 1);
			if(chp_comment_id && chp_tmp2 && atoi(chp_comment_id) == atoi(chp_tmp2)) {
				in_checked_flg = 1;
			}
		}
		chp_tmp = Db_GetValue(dbres, i, 9);
		if (chp_tmp && !atoi(chp_tmp)) {
			if(in_checked_flg) {
				Build_Checkbox(nlp_out, "LIST", "accept_comment", cha_str, 1);
			} else {
				Build_Checkbox(nlp_out, "LIST", "accept_comment", cha_str, 0);
			}
		} else {
			Put_Nlist(nlp_out, "LIST", "��ǧ��");
		}
		Put_Nlist(nlp_out, "LIST", "</td>\n");
		Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
		/* �����Ȥ����͡��᡼�륢�ɥ쥹 */
		chp_tmp = Db_GetValue(dbres, i, 1);
		chp_tmp2 = Db_GetValue(dbres, i, 2);
		if(chp_tmp && *chp_tmp && chp_tmp2 && *chp_tmp2) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Format_Nlist(nlp_out, "LIST", "<a href=\"mailto:%s\">%s</a>", chp_tmp2, chp_escape);
			free(chp_escape);
		} else if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "LIST", chp_escape);
			free(chp_escape);
		} else {
			Put_Nlist(nlp_out, "LIST", "(̵��̾)");
		}
		Put_Nlist(nlp_out, "LIST", "</td>");
		Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
		/* URL */
		chp_tmp = Db_GetValue(dbres, i, 3);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s\"><img src=\"%s/site.gif\" border=\"0\"></a>", chp_tmp, g_cha_admin_image);
			free(chp_escape);
		} else {
			Put_Nlist(nlp_out, "LIST", "&nbsp;");
		}
		Put_Nlist(nlp_out, "LIST", "</td>\n");
		/* IP */
		if (!g_in_need_login) {
			Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
			chp_tmp = Db_GetValue(dbres, i, 8);
			if(chp_tmp && *chp_tmp) {
				Put_Nlist(nlp_out, "LIST", chp_tmp);
				memset(cha_host, '\0', sizeof(cha_host));
				Get_Hostbyaddr(chp_tmp, cha_host);
				if(cha_host[0] != '\0') {
					Put_Format_Nlist(nlp_out, "LIST", "<br><small>(%s)</small>", cha_host);
				}
			} else {
				Put_Nlist(nlp_out, "LIST", "&nbsp;");
			}
			Put_Nlist(nlp_out, "LIST", "</td>\n");
		}
		/* �Ƶ��� */
		/* ��������Ƥ���Ȥ��ϥ��ɽ�� */
		Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
		chp_tmp = Db_GetValue(dbres, i, 5);
		chp_tmp2 = Db_GetValue(dbres, i, 6);
		if(chp_tmp && atoi(chp_tmp) == 1) {
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s%s%s/%s/?eid=%s&FLG_FORCE_ALL=1#c%s\" target=\"dbb_blog_main_wnd\">"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, Db_GetValue(dbres, i, 4), Db_GetValue(dbres, i, 0));
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s%s%s/%08d/?eid=%s&FLG_FORCE_ALL=1#c%s\" target=\"dbb_blog_main_wnd\">"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, Db_GetValue(dbres, i, 4), Db_GetValue(dbres, i, 0));
			} else {
				Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s/%s?bid=%d&eid=%s&FLG_FORCE_ALL=1#c%s\" target=\"dbb_blog_main_wnd\">"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, Db_GetValue(dbres, i, 4), Db_GetValue(dbres, i, 0));
			}
		}
		if(chp_tmp2 && *chp_tmp2) {
			chp_escape = Escape_HtmlString(chp_tmp2);
			Put_Nlist(nlp_out, "LIST", chp_escape);
			free(chp_escape);
		} else {
			Put_Nlist(nlp_out, "LIST", "(̵��)");
		}
		if(chp_tmp && atoi(chp_tmp) == 1) {
			Put_Nlist(nlp_out, "LIST", "</a>");
		}
		Put_Nlist(nlp_out, "LIST", "</td>");

		/* �ե��륿�� */
		Put_Nlist(nlp_out, "LIST", "<td nowrap align=\"center\" height=\"27\">");
		chp_tmp = Db_GetValue(dbres, i, 10);
		if(chp_tmp && atoi(chp_tmp)) {
			Put_Nlist(nlp_out, "LIST", "��");
		} else {
			Put_Nlist(nlp_out, "LIST", "<br>");
		}
		Put_Nlist(nlp_out, "LIST", "</td>");

		/* ���������� */
		Put_Nlist(nlp_out, "LIST", "<td nowrap align=\"center\" height=\"27\">");
		chp_tmp = Db_GetValue(dbres, i, 7);
		if(chp_tmp) {
			Put_Nlist(nlp_out, "LIST", chp_tmp);
		} else {
			Put_Nlist(nlp_out, "LIST", "--ǯ--��--��--��--ʬ");
		}
		Put_Nlist(nlp_out, "LIST", "</td>");
		Put_Nlist(nlp_out, "LIST", "</tr>\n\n");
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_comment()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int delete_comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_sql[8192];
	int in_count;
	int in_skip;
	int i;

	in_count = Get_NlistCount(nlp_in, "delete_comment");
	if(!in_count) {
		Put_Nlist(nlp_out, "ERROR", "���򤵤줿�����Ȥϴ��˺������Ƥ��ޤ���");
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
		strcpy(cha_sql, "delete from at_comment");
		strcat(cha_sql, " where concat(n_entry_id,':',n_comment_id) in (");
		for(i = 0; i < in_count; i++) {
			if(i) {
				strcat(cha_sql, ", ");
			}
			strcat(cha_sql, "'");
			strcat(cha_sql, Get_Nlist(nlp_in, "delete_comment", i + 1));
			strcat(cha_sql, "'");
		}
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "�����Ȥκ���˼��Ԥ��ޤ�����");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_delete_ask()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 * 	���ｪλ 0
 * 	���顼 1
-* ------------------------------------------------------------------------*/
int disp_page_delete_comment_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	char cha_str[CO_PERPAGE * 1024];
	int in_checked_count;
	int in_row;
	int i;

	/* �ڡ��������� */
	chp_tmp = Get_Nlist(nlp_in, "page", 1);
	if(chp_tmp && *chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "page", chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_tmp){
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_tmp);
	}
	in_checked_count = Get_NlistCount(nlp_in, "delete_comment");
	if(!in_checked_count) {
		Put_Nlist(nlp_out, "ERROR", "������륳���Ȥ����򤷤Ƥ���������");
		return 1;
	}
	strcpy(cha_sql, "select concat(T1.n_entry_id,':',T1.n_comment_id)");	/* 0 ������ID */
	strcat(cha_sql, ", T1.c_comment_author");	/* 1 �����Ȥ����� */
	strcat(cha_sql, ", T2.c_entry_title");		/* 2 �Ƶ��������ȥ� */
	strcat(cha_sql, ", date_format(T1.d_comment_create_time,'%yǯ%m��%d��%H��%iʬ')");	/* 3 �����Ȼ��� */
	strcat(cha_sql, " from at_comment T1");
	strcat(cha_sql, ", at_entry T2");
	strcat(cha_sql, " where T1.n_entry_id = T2.n_entry_id");
	strcat(cha_sql, " and concat(T1.n_entry_id,':',T1.n_comment_id) in (");
	for(i = 0; i < in_checked_count; i++) {
		if(i) {
			strcat(cha_sql, ", ");
		}
		strcat(cha_sql, "'");
		strcat(cha_sql, Get_Nlist(nlp_in, "delete_comment", i + 1));
		strcat(cha_sql, "'");
	}
	strcat(cha_sql, ")");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_comment_create_time desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������륳���Ⱦ�������륯���꡼�˼��Ԥ��ޤ�����");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if(!in_row) {
		Put_Nlist(nlp_out, "ERROR", "���򤵤줿�����Ȥϴ��˺������Ƥ��ޤ���");
		return 1;
	}
	if(in_row != in_checked_count) {
		Put_Nlist(nlp_out, "MSG", "���򤵤줿�����Ȥϰ����������Ƥ��ޤ���<br>");
	}
	for(i = 0; i < in_row; i++) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "delete_comment", Db_GetValue(dbres, i, 0));
		strcpy(cha_str, "<tr style=\"background-color:#F0F5FD;\">\n");
		strcat(cha_str, "<td nowrap>");
		chp_tmp = Db_GetValue(dbres, i, 1);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		} else {
			strcat(cha_str, "(̵��̾)");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "<td nowrap>");
		chp_tmp = Db_GetValue(dbres, i, 2);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		} else {
			strcat(cha_str, "(̵��)");
		}
		strcat(cha_str, "</td>");
		strcat(cha_str, "<td nowrap>");
		chp_tmp = Db_GetValue(dbres, i, 3);
		if(chp_tmp) {
			strcat(cha_str, chp_tmp);
		} else {
			strcat(cha_str, "--ǯ--��--��--��--ʬ");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "</tr>\n");
		Put_Nlist(nlp_out, "DELETE_TITLE", cha_str);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_comment()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int accept_comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_sql[8192];
	int in_count;
	int in_skip;
	int i;

	in_count = Get_NlistCount(nlp_in, "accept_comment");
	if(!in_count) {
		Put_Nlist(nlp_out, "ERROR", "���򤵤줿�����Ȥϴ��˾�ǧ����Ƥ��ޤ���");
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
		strcpy(cha_sql, " update at_comment");
		strcat(cha_sql, " set b_comment_accept = 1");
		strcat(cha_sql, " where concat(n_entry_id,':',n_comment_id) in (");
		for(i = 0; i < in_count; i++) {
			if(i) {
				strcat(cha_sql, ", ");
			}
			strcat(cha_sql, "'");
			strcat(cha_sql, Get_Nlist(nlp_in, "accept_comment", i + 1));
			strcat(cha_sql, "'");
		}
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "�����Ȥκ���˼��Ԥ��ޤ�����");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_accept_comment_ask()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 * 	���ｪλ 0
 * 	���顼 1
-* ------------------------------------------------------------------------*/
int disp_page_accept_comment_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	char cha_str[CO_PERPAGE * 1024];
	int in_checked_count;
	int in_row;
	int i;

	/* �ڡ��������� */
	chp_tmp = Get_Nlist(nlp_in, "page", 1);
	if(chp_tmp && *chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "page", chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_tmp){
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_tmp);
	}
	in_checked_count = Get_NlistCount(nlp_in, "accept_comment");
	if(!in_checked_count) {
		Put_Nlist(nlp_out, "ERROR", "��ǧ���륳���Ȥ����򤷤Ƥ���������");
		return 1;
	}
	strcpy(cha_sql, "select concat(T1.n_entry_id,':',T1.n_comment_id)");	/* 0 ������ID */
	strcat(cha_sql, ", T1.c_comment_author");	/* 1 �����Ȥ����� */
	strcat(cha_sql, ", T2.c_entry_title");		/* 2 �Ƶ��������ȥ� */
	strcat(cha_sql, ", date_format(T1.d_comment_create_time,'%yǯ%m��%d��%H��%iʬ')");	/* 3 �����Ȼ��� */
	strcat(cha_sql, " from at_comment T1");
	strcat(cha_sql, ", at_entry T2");
	strcat(cha_sql, " where T1.n_entry_id = T2.n_entry_id");
	strcat(cha_sql, " and concat(T1.n_entry_id,':',T1.n_comment_id) in (");
	for(i = 0; i < in_checked_count; i++) {
		if(i) {
			strcat(cha_sql, ", ");
		}
		strcat(cha_sql, "'");
		strcat(cha_sql, Get_Nlist(nlp_in, "accept_comment", i + 1));
		strcat(cha_sql, "'");
	}
	strcat(cha_sql, ")");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "��ǧ���륳���Ⱦ�������륯���꡼�˼��Ԥ��ޤ�����");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if(!in_row) {
		Put_Nlist(nlp_out, "ERROR", "���򤵤줿�����Ȥϴ��˾�ǧ����Ƥ��ޤ���");
		return 1;
	}
	if(in_row != in_checked_count) {
		Put_Nlist(nlp_out, "MSG", "���򤵤줿�����Ȥϰ�����ǧ����Ƥ��ޤ���<br>");
	}
	for(i = 0; i < in_row; i++) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "accept_comment", Db_GetValue(dbres, i, 0));
		memset(cha_str, '\0', sizeof(cha_str));
		strcpy(cha_str, "<tr style=\"background-color:#F0F5FD;\">\n");
		strcat(cha_str, "<td nowrap>");
		chp_tmp = Db_GetValue(dbres, i, 1);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		} else {
			strcat(cha_str, "(̵��̾)");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "<td nowrap>");
		chp_tmp = Db_GetValue(dbres, i, 2);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		} else {
			strcat(cha_str, "(̵��)");
		}
		strcat(cha_str, "</td>");
		strcat(cha_str, "<td nowrap>");
		chp_tmp = Db_GetValue(dbres, i, 3);
		if(chp_tmp) {
			strcat(cha_str, chp_tmp);
		} else {
			strcat(cha_str, "--ǯ--��--��--��--ʬ");
		}
		strcat(cha_str, "</td>\n");
		strcat(cha_str, "</tr>\n");
		Put_Nlist(nlp_out, "ACCEPT_TITLE", cha_str);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	main()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
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
	char *chp_skel;
	char *chp_action;
	int in_blog;
	int in_error;
	int in_ret;
	//int g_in_login_owner;
	int in_orig;

	in_error = Read_Blog_Ini(cha_host,cha_db, cha_username, cha_password, &g_in_ssl_mode);
	if (in_error) {
		printf("\n\nINI�ե�������ɤ߹��ߤ˼��Ԥ��ޤ�����(%s)", strerror(in_error));
		return in_error;
	}
	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		printf("\n\n�ǡ����١�����³�˼��Ԥ��ޤ�����");
		return 1;
	}
	if (Read_Blog_Option(db)) {
		printf("\n\n�֥����ץ������ɤ߹��ߤ˼��Ԥ��ޤ�����");
		Db_Disconnect(db);
		return 1;
	}
	Set_SkelPath(g_cha_admin_skeleton);

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	Put_Nlist(nlp_out, "CSS", g_cha_css_location);
	Put_Nlist(nlp_out, "SCRIPTS", g_cha_script_location);
	Put_Nlist(nlp_out, "IMAGES", g_cha_admin_image);
	chp_action = CO_CGI_COMMENT_LIST;

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
				Put_Nlist(nlp_out, "ERROR", "������ID�ޤ��ϥѥ���ɤ��ְ�äƤ��ޤ���<br>");
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
					Put_Nlist(nlp_out, "ACTION", CO_CGI_COMMENT_LIST);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "ǧ�ھ��������Ǥ��ޤ���<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_COMMENT_LIST);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_COMMENT_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�����Ԥˤ����������ӥ��Τ����Ѥ���ߤ��Ƥ���ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_COMMENT_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_COMMENT_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥��Ϥ��Ȥ��ˤʤ�ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_RESERVE && Get_Blog_Priv(db, nlp_out, g_in_login_owner) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_BOOKMARKLET);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥������븢�¤�����ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_hb_mode && in_orig == INT_MAX && Get_Blog_Auth(db, nlp_out, g_in_login_owner, in_blog) < CO_AUTH_WRITE) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_COMMENT_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥����Խ����븢�¤�����ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_COMMENT_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥����������Ƥ��ʤ����Ϥ����Ѥˤʤ�ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);
	g_in_need_login = Need_Login(db, in_blog);
	if (g_in_need_login) {
		Put_Nlist(nlp_out, "IP_START", "<!--");
		Put_Nlist(nlp_out, "IP_END", "-->");
	}

	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_fromuser && atoi(chp_fromuser)) {
		Build_HiddenEncode(nlp_out, "FROMUSER", "from_user", chp_fromuser);
		Put_Nlist(nlp_out, "FROM_USER", "&from_user=1");
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
		Put_Nlist(nlp_out, "TOP", "�֥�");
		Put_Nlist(nlp_out, "GIF", "go_blog.gif");
	} else {
		Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%s?blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
		Put_Nlist(nlp_out, "TOP", "������˥塼");
		Put_Nlist(nlp_out, "GIF", "go_kanri.gif");
	}
	Put_Format_Nlist(nlp_out, "FROM_USER", "&blogid=%d", in_blog);

	/* ����ȥꤴ�ȤΥ����Ȥ�����Ƥ�����ϡ����� */
	chp_tmp = Get_Nlist(nlp_in, "entry_id", 1);
	if(chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "entry_id", chp_tmp);
	}
	in_ret = 0;
	in_error = 0;
	chp_skel = 0;
	if(Get_Nlist(nlp_in, "BTN_DISP_DELETE_COMMENT_ASK.x", 1)) {
		if(disp_page_delete_comment_ask(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			in_ret = disp_page_commentlist(db, nlp_in, nlp_out, in_blog);
			if(in_ret == CO_ERROR) {
				in_error++;
			}
			chp_skel = CO_SKEL_COMMENT_LIST;
		} else {
			chp_skel = CO_SKEL_DELETE_COMMENT_ASK;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DELETE_COMMENT.x", 1)) {
		if (delete_comment(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = CO_SKEL_COMMENT_LIST;
		} else {
			Put_Nlist(nlp_out, "MESSAGE", "�����Ȥ������ޤ�����");
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s?blogid=%d&BTN_DISP_COMMENTLIST=1", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_COMMENT_LIST, in_blog);
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "TITLE", "DBB_blog �����ԥ�˥塼");
			} else if (g_in_cart_mode == CO_CART_SHOPPER) {
				Put_Nlist(nlp_out, "TITLE", "ŹĹ�֥�������˥塼");
			} else if (g_in_cart_mode == CO_CART_RESERVE) {
				Put_Nlist(nlp_out, "TITLE", "e�ꥶ���֥֥�������˥塼");
			} else {
				Put_Nlist(nlp_out, "TITLE", "AS-BLOG �����ԥ�˥塼");
			}
			chp_skel = CO_SKEL_CONFIRM;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DISP_ACCEPT_COMMENT_ASK.x", 1)) {
		if(disp_page_accept_comment_ask(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			in_ret = disp_page_commentlist(db, nlp_in, nlp_out, in_blog);
			if(in_ret == CO_ERROR) {
				in_error++;
			}
			chp_skel = CO_SKEL_COMMENT_LIST;
		} else {
			chp_skel = CO_SKEL_ACCEPT_COMMENT_ASK;
		}
	} else if(Get_Nlist(nlp_in, "BTN_ACCEPT_COMMENT.x", 1)) {
		if (accept_comment(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = CO_SKEL_COMMENT_LIST;
		} else {
			Put_Nlist(nlp_out, "MESSAGE", "�����Ȥ�ǧ���ޤ�����");
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s?blogid=%d&BTN_DISP_COMMENTLIST=1", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_COMMENT_LIST, in_blog);
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "TITLE", "DBB_blog �����ԥ�˥塼");
			} else if (g_in_cart_mode == CO_CART_SHOPPER) {
				Put_Nlist(nlp_out, "TITLE", "ŹĹ�֥�������˥塼");
			} else if (g_in_cart_mode == CO_CART_RESERVE) {
				Put_Nlist(nlp_out, "TITLE", "e�ꥶ���֥֥�������˥塼");
			} else {
				Put_Nlist(nlp_out, "TITLE", "AS-BLOG �����ԥ�˥塼");
			}
			chp_skel = CO_SKEL_CONFIRM;
		}
	} else if (Get_Nlist(nlp_in, "BTN_DISP_COMMENTLIST", 1) || Get_Nlist(nlp_in, "BTN_DISP_COMMENTLIST.x", 1)) {
		in_ret = disp_page_commentlist(db, nlp_in, nlp_out, in_blog);
		if(in_ret == CO_ERROR) {
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_ENTRYLIST", "1");
			chp_action = CO_CGI_ENTRY_LIST;
			in_error++;
		}
		chp_skel = CO_SKEL_COMMENT_LIST;
	} else {
		Put_Nlist(nlp_out, "ERROR", "ľ��CGI��¹ԤǤ��ޤ���<br>");
		in_error++;
	}
	if(in_error || in_ret/* == CO_ERROR*/ || !chp_skel){
		put_error_data(nlp_in, nlp_out, chp_action);
		Page_Out(nlp_out, CO_SKEL_ERROR);
	} else {
		if(in_ret) {
			printf("Location: %s%s%s/%s?blogid=%d&BTN_DISP_ENTRYLIST=1\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY_LIST, in_blog);
		} else {
			Page_Out(nlp_out, chp_skel);
		}
	}

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
