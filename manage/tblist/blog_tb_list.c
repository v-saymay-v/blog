/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_tb_list.c
 * First-Created:	2004/08/31 ��¼ ����Ϻ
%* ------------------------------------------------------------------------
 * Module-Description:
 *	�ȥ�å��Хå��ΰ�������
-* ------------------------------------------------------------------------
 * Change-Log:
 *	2005/07/05 �غ�����
 *		�����˽񤭴����롣
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_tb_list.c,v 1.98 2009/02/24 04:37:18 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "libcgi2.h"
#include "libdb2.h"
#include "libcommon2.h"
#include "libnet.h"
#include "libauth.h"
#include "libblog.h"
#include "libblogcart.h"
#include "libblogreserve.h"

#define CO_PERPAGE 10
#define CO_SELECTBOX 0
#define CO_MULTISELECT 1
#define CO_BUFSIZE 10000

/*
+* ------------------------------------------------------------------------
 * Function:		put_error_data()
 * Description:
 * 	���顼����������
%* ------------------------------------------------------------------------
 * Return:
 *	(̵��)
-* ------------------------------------------------------------------------*/
void put_error_data(NLIST *nlp_in, NLIST *nlp_out, char *chp_action)
{
	char *chpa_esc[] = {
		"BTN_ACCEPT_TRACKBACK.x", "BTN_ACCEPT_TRACKBACK.y",
		"BTN_DISP_ACCEPT_TRACKBACK_ASK.x", "BTN_DISP_ACCEPT_TRACKBACK_ASK.y",
		"BTN_DISP_DELETE_TRACKBACK_ASK.x", "BTN_DISP_DELETE_TRACKBACK_ASK.y",
		"BTN_DELETE_TRACKBACK.x", "BTN_DELETE_TRACKBACK.y", "blogid", NULL
	};

	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	Put_Nlist(nlp_out, "ACTION", chp_action);
	Put_Nlist(nlp_out, "BUTTON", "BTN_DISP_TRACKBACKLIST");
	return ;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_trackbacklist()
 * Description:
 *	�ȥ�å��Хå��ꥹ�Ȥ�ɽ������
%* ------------------------------------------------------------------------
 * Return:
 *	���ｪλ 0
 *	���顼�� CO_ERROR
 *	�Ǹ��1�Ĥ���������κ����� 1
-* ------------------------------------------------------------------------*/
int disp_page_trackbacklist(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_entry_id;
	char *chp_tb_id;
	char *chp_escape;
	char cha_sql[512];
	char cha_str[8192];
	char cha_host[256];
	int in_count;
	int in_total_page;
	int in_total_trackback;
	int in_page;
	int in_checked_count;
	int in_checked_flg;
	int i;
	int j;

	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	if(!chp_entry_id || !*chp_entry_id) {
		in_total_trackback = Get_Total_Trackback(db, nlp_in, nlp_out, in_blog, 1);
	} else {
		in_total_trackback = Get_Total_Trackback_By_Entry_Id(db, nlp_in, nlp_out, atoi(chp_entry_id), in_blog, 1);
	}
	if(in_total_trackback == CO_ERROR) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ������������ޤ���Ǥ�����");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����������ޤ���Ǥ�����");
		}
		return CO_ERROR;
	}
	in_total_page = in_total_trackback / CO_PERPAGE_ARTICLE;
	if(in_total_trackback % CO_PERPAGE_ARTICLE) {
		in_total_page++;
	}
	if (in_total_trackback == CO_ERROR) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ������������ޤ���Ǥ�����");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����������ޤ���Ǥ�����");
		}
		return CO_ERROR;
	}
	if (in_total_trackback == 0) {
		Put_Nlist(nlp_out, "BUTTONTOPPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONPREVPAGE", "disabled");
		Put_Nlist(nlp_out, "MAE", "mae_bd.gif");
		Put_Nlist(nlp_out, "MAEMAE", "maemae_bd.gif");
		Put_Nlist(nlp_out, "BUTTONNEXTPAGE", "disabled");
		Put_Nlist(nlp_out, "BUTTONLASTPAGE", "disabled");
		Put_Nlist(nlp_out, "TUGI", "tugi_bd.gif");
		Put_Nlist(nlp_out, "TUGITUGI", "tugitugi_bd.gif");
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "LIST", "<tr class=\"a2\"><td align=\"center\" colspan=\"8\" bgcolor=\"#ffffff\">���ѵ������Ϥ���ޤ���</td></tr>\n");
		} else {
			Put_Nlist(nlp_out, "LIST", "<tr class=\"a2\"><td align=\"center\" colspan=\"8\" bgcolor=\"#ffffff\">�ȥ�å��Хå��Ϥ���ޤ���</td></tr>\n");
		}
		return 0;
	}
	/* �ܥ���Ĵ�� */
	if(!Get_Total_Comment(db, nlp_in, nlp_out, in_blog, 1)) {
		Put_Nlist(nlp_out, "COMMENTLIST", "disabled");
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
	Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_TRACKBACKLIST", "1");
	/* �ڡ������������� */
	Put_Format_Nlist(nlp_out, "PAGE", "��%d��(%d/%d)", in_total_trackback, in_page + 1, in_total_page);
	/* �������ȤΥȥ�å��Хå���ɽ�����Ƥ���Ȥ��ϡ����������ȥ��ɽ�� */
	if(chp_entry_id) {
		strcpy(cha_sql, "select T1.c_entry_title");
		strcat(cha_sql, " from at_entry T1");
		strcat(cha_sql, " where n_entry_id = ");
		strcat(cha_sql, chp_entry_id);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "������������륯����˼��Ԥ��ޤ�����");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return CO_ERROR;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "PAGE", "<br><i>������%s�פ���Ѥ��Ƥ��뵭��</i>", chp_escape);
			} else {
				Put_Format_Nlist(nlp_out, "PAGE", "<br><i>������%s�פؤΥȥ�å��Хå�</i>", chp_escape);
			}
			free(chp_escape);
		} else {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "PAGE", "<br><i>������(̵��)�פ���Ѥ��Ƥ��뵭��</i>");
			} else {
				Put_Nlist(nlp_out, "PAGE", "<br><i>������(̵��)�פؤΥȥ�å��Хå�</i>");
			}
		}
		Db_CloseDyna(dbres);
	}
	/* �ȥ�å��Хå� */
	strcpy(cha_sql, " select T1.n_tb_id");		/* 0 �ȥ�å��Хå�ID */
	strcat(cha_sql, ",T1.c_tb_title");			/* 1 �ȥ�å��Хå������ȥ� */
	strcat(cha_sql, ",T1.c_tb_url");			/* 2 �ȥ�å��Хå���URL */
	strcat(cha_sql, ",T1.c_tb_blog_name");		/* 3 �ȥ�å��Хå���������̾ */
	strcat(cha_sql, ",T2.n_entry_id");			/* 4 �Ƶ���ID */
	strcat(cha_sql, ",T2.b_mode");				/* 5 �Ƶ��������⡼�� */
	strcat(cha_sql, ",T2.c_entry_title");		/* 6 �Ƶ��������ȥ� */
	strcat(cha_sql, ",date_format(T1.d_tb_create_time, '%yǯ%m��%d��%H��%iʬ')");	/* 7 �ȥ�å��Хå����� */
	strcat(cha_sql, ",T1.c_tb_ip");				/* 8 �ȥ�å��Хå�IP */
	strcat(cha_sql, ",T1.b_tb_accept");			/* 9 ��ǧ�� */
	strcat(cha_sql, ",T3.c_filter_name");		/* 10 �ե��륿��̾ */
	strcat(cha_sql, " from at_trackback T1");
	strcat(cha_sql, " left join sy_filter T3 on T1.n_tb_filter = T3.n_filter_id");
	strcat(cha_sql, ",at_entry T2");
	strcat(cha_sql, " where T1.n_entry_id = T2.n_entry_id");
	if(chp_entry_id && *chp_entry_id) {
		strcat(cha_sql, " and T1.n_entry_id = ");
		strcat(cha_sql, chp_entry_id);
	}
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_tb_create_time desc");
	sprintf(cha_sql + strlen(cha_sql), " limit %d, %d", in_page * CO_PERPAGE_ARTICLE, CO_PERPAGE_ARTICLE);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ��������륯���꡼�˼��Ԥ��ޤ�����<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå������륯���꡼�˼��Ԥ��ޤ�����<br>");
		}
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	in_count = Db_GetRowCount(dbres);
	if(!in_count) {
		Db_CloseDyna(dbres);
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ���������ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå�������ޤ���");
		}
		return CO_ERROR;
	}
	in_checked_count = Get_NlistCount(nlp_in, "delete_trackback");
	for(i = 0; i < in_count; i++) {
		Put_Nlist(nlp_out, "LIST", "\n<tr class=\"a2\">");
		if (g_in_cart_mode == CO_CART_RESERVE) {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#B3E1E0\">");
		} else {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#c6e3ff\">");
		}
		in_checked_flg = 0;
		chp_tb_id = Db_GetValue(dbres, i, 0);
		chp_entry_id = Db_GetValue(dbres, i, 4);
		sprintf(cha_str, "%s:%s", chp_entry_id, chp_tb_id);
		for(j = 0; j < in_checked_count; j++) {
			chp_tmp2 = Get_Nlist(nlp_in, "delete_trackback", j + 1);
			if(chp_tb_id && chp_tmp2 && atoi(chp_tb_id) == atoi(chp_tmp2)) {
				in_checked_flg = 1;
			}
		}
		if(in_checked_flg) {
			Build_Checkbox(nlp_out, "LIST", "delete_trackback", cha_str, 1);
		} else {
			Build_Checkbox(nlp_out, "LIST", "delete_trackback", cha_str, 0);
		}
		Put_Nlist(nlp_out, "LIST", "</td>\n");

		if (g_in_cart_mode == CO_CART_RESERVE) {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#B3E1E0\">");
		} else {
			Put_Nlist(nlp_out, "LIST", "<td align=\"center\" width=\"50\" bgcolor=\"#c6e3ff\">");
		}
		in_checked_flg = 0;
		for(j = 0; j < in_checked_count; j++) {
			chp_tmp2 = Get_Nlist(nlp_in, "accept_trackback", j + 1);
			if(chp_tb_id && chp_tmp2 && atoi(chp_tb_id) == atoi(chp_tmp2)) {
				in_checked_flg = 1;
			}
		}
		chp_tmp = Db_GetValue(dbres, i, 9);
		if (chp_tmp && !atoi(chp_tmp)) {
			if(in_checked_flg) {
				Build_Checkbox(nlp_out, "LIST", "accept_trackback", cha_str, 1);
			} else {
				Build_Checkbox(nlp_out, "LIST", "accept_trackback", cha_str, 0);
			}
		} else {
			Put_Nlist(nlp_out, "LIST", "��ǧ��");
		}
		Put_Nlist(nlp_out, "LIST", "</td>\n");

		Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
		/* �����ȥ� */
		chp_tmp = Db_GetValue(dbres, i, 2);
		chp_tmp2 = Db_GetValue(dbres, i, 1);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s\" target=\"_blank\">", chp_tmp);
			free(chp_escape);
		}
		if(chp_tmp2 && *chp_tmp2) {
			chp_escape = Escape_HtmlString(chp_tmp2);
			Put_Nlist(nlp_out, "LIST", chp_escape);
			free(chp_escape);
		} else {
			Put_Nlist(nlp_out, "LIST", "(̵��)");
		}
		if(chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "LIST", "</a>");
		}
		Put_Nlist(nlp_out, "LIST", "</td>\n");
		Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
		/* �ȥ�å��Хå��֥�̾ */
		chp_tmp = Db_GetValue(dbres, i, 3);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "LIST", chp_escape);
			free(chp_escape);
		} else {
			Put_Nlist(nlp_out, "LIST", "(̵��)");
		}
		Put_Nlist(nlp_out, "LIST", "</td>");
		Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
		/* �ȥ�å��Хå�IP */
		chp_tmp = Db_GetValue(dbres, i, 8);
		if(chp_tmp && *chp_tmp) {
			if (Need_Login(db, in_blog)) {
				if (!Get_Nickname_From_Blog(db, nlp_out, atoi(chp_tmp), cha_host)) {
					strcpy(cha_host, "");
				}
				chp_tmp = Escape_HtmlString(cha_host);
				Put_Nlist(nlp_out, "LIST", chp_tmp);
				free(chp_tmp);
			} else {
				Put_Nlist(nlp_out, "LIST", chp_tmp);
				memset(cha_host, '\0', sizeof(cha_host));
				Get_Hostbyaddr(chp_tmp, cha_host);
				if(cha_host[0] != '\0') {
					Put_Format_Nlist(nlp_out, "LIST", "<br><small>(%s)</small>", cha_host);
				}
			}
		} else {
			Put_Nlist(nlp_out, "LIST", "&nbsp;");
		}
		Put_Nlist(nlp_out, "LIST", "</td>");
		/* �Ƶ��� */
		/* ��������Ƥ���Ȥ��ϥ��ɽ�� */
		Put_Nlist(nlp_out, "LIST", "<td align=\"left\" height=\"27\">");
		chp_tmp = Db_GetValue(dbres, i, 5);
		chp_tmp2 = Db_GetValue(dbres, i, 6);
		if(chp_tmp && atoi(chp_tmp) == 1) {
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s%s%s/%s/?eid=%s&FLG_FORCE_ALL=1#tb%s\" target=\"dbb_blog_main_wnd\">"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, Db_GetValue(dbres, i, 4), Db_GetValue(dbres, i, 0));
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s%s%s/%08d/?eid=%s&FLG_FORCE_ALL=1#tb%s\" target=\"dbb_blog_main_wnd\">"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, Db_GetValue(dbres, i, 4), Db_GetValue(dbres, i, 0));
			} else {
				Put_Format_Nlist(nlp_out, "LIST", "<a href=\"%s/%s?bid=%d&eid=%s&FLG_FORCE_ALL=1#tb%s\" target=\"dbb_blog_main_wnd\">"
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

		/* �ե��륿��̾ */
		Put_Nlist(nlp_out, "LIST", "<td align=\"center\" height=\"27\">");
		chp_tmp = Db_GetValue(dbres, i, 10);
		if(chp_tmp) {
			Put_Nlist(nlp_out, "LIST", chp_tmp);
		} else {
			Put_Nlist(nlp_out, "LIST", "<br>");
		}
		Put_Nlist(nlp_out, "LIST", "</td>");

		/* �ȥ�å��Хå����� */
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
 * Function:	 	delete_trackback()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int delete_trackback(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_sql[8192];
	int in_count;
	int in_skip;
	int i;

	in_count = Get_NlistCount(nlp_in, "delete_trackback");
	if(!in_count) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿���ѵ����ϴ��˺������Ƥ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿�ȥ�å��Хå��ϴ��˺������Ƥ��ޤ���");
		}
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
		strcpy(cha_sql, "delete from at_trackback");
		strcat(cha_sql, " where concat(n_entry_id,':',n_tb_id) in (");
		for(i = 0; i < in_count; i++) {
			if(i) {
				strcat(cha_sql, ", ");
			}
			strcat(cha_sql, "'");
			strcat(cha_sql, Get_Nlist(nlp_in, "delete_trackback", i + 1));
			strcat(cha_sql, "'");
		}
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if(Db_ExecSql(db, cha_sql)) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "���ѵ����κ���˼��Ԥ��ޤ�����");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��κ���˼��Ԥ��ޤ�����");
			}
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
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
int disp_page_delete_trackback_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
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
	in_checked_count = Get_NlistCount(nlp_in, "delete_trackback");
	if(!in_checked_count) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���������ѵ��������򤷤Ƥ���������");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�������ȥ�å��Хå������򤷤Ƥ���������");
		}
		return 1;
	}
	strcpy(cha_sql, "select concat(T1.n_entry_id,':',T1.n_tb_id)");	/* 0 �ȥ�å��Хå�ID */
	strcat(cha_sql, ", T1.c_tb_title");							/* 1 �ȥ�å��Хå������� */
	strcat(cha_sql, ", T2.c_entry_title");						/* 2 �Ƶ��������ȥ� */
	strcat(cha_sql, ", date_format(T1.d_tb_create_time,'%yǯ%m��%d��%H��%iʬ')");	/* 3 �ȥ�å��Хå����� */
	strcat(cha_sql, " from at_trackback T1");
	strcat(cha_sql, ", at_entry T2");
	strcat(cha_sql, " where T1.n_entry_id = T2.n_entry_id");
	strcat(cha_sql, " and concat(T1.n_entry_id,':',T1.n_tb_id) in (");
	for(i = 0; i < in_checked_count; i++) {
		if(i) {
			strcat(cha_sql, ", ");
		}
		strcat(cha_sql, "'");
		strcat(cha_sql, Get_Nlist(nlp_in, "delete_trackback", i + 1));
		strcat(cha_sql, "'");
	}
	strcat(cha_sql, ")");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_tb_create_time desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���������ѵ�����������륯���꡼�˼��Ԥ��ޤ�����");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�������ȥ�å��Хå���������륯���꡼�˼��Ԥ��ޤ�����");
		}
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if(!in_row) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿���ѵ����ϴ��˺������Ƥ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿�ȥ�å��Хå��ϴ��˺������Ƥ��ޤ���");
		}
		return 1;
	}
	if(in_row != in_checked_count) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "MSG", "���򤵤줿���ѵ����ϰ����������Ƥ��ޤ���<br>");
		} else {
			Put_Nlist(nlp_out, "MSG", "���򤵤줿�ȥ�å��Хå��ϰ����������Ƥ��ޤ���<br>");
		}
	}
	for(i = 0; i < in_row; i++) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "delete_trackback", Db_GetValue(dbres, i, 0));
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
		Put_Nlist(nlp_out, "DELETE_TITLE", cha_str);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		accept_trackback()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int accept_trackback(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_sql[8192];
	int in_count;
	int in_skip;
	int i;

	in_count = Get_NlistCount(nlp_in, "accept_trackback");
	if(!in_count) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿���ѵ����ϴ��˾�ǧ����Ƥ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿�ȥ�å��Хå��ϴ��˾�ǧ����Ƥ��ޤ���");
		}
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
		strcpy(cha_sql, "update at_trackback");
		strcat(cha_sql, " set b_tb_accept = 1");
		strcat(cha_sql, " where concat(n_entry_id,':',n_tb_id) in (");
		for(i = 0; i < in_count; i++) {
			if(i) {
				strcat(cha_sql, ", ");
			}
			strcat(cha_sql, "'");
			strcat(cha_sql, Get_Nlist(nlp_in, "accept_trackback", i + 1));
			strcat(cha_sql, "'");
		}
		strcat(cha_sql, ")");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if(Db_ExecSql(db, cha_sql)) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "�������Ѥξ�ǧ�˼��Ԥ��ޤ�����");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��ξ�ǧ�˼��Ԥ��ޤ�����");
			}
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
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
int disp_page_accept_trackback_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
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
	in_checked_count = Get_NlistCount(nlp_in, "accept_trackback");
	if(!in_checked_count) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "��ǧ������ѵ��������򤷤Ƥ���������");
		} else {
			Put_Nlist(nlp_out, "ERROR", "��ǧ����ȥ�å��Хå������򤷤Ƥ���������");
		}
		return 1;
	}
	strcpy(cha_sql, "select concat(T1.n_entry_id,':',T1.n_tb_id)");	/* 0 �ȥ�å��Хå�ID */
	strcat(cha_sql, ", T1.c_tb_title");							/* 1 �ȥ�å��Хå������� */
	strcat(cha_sql, ", T2.c_entry_title");						/* 2 �Ƶ��������ȥ� */
	strcat(cha_sql, ", date_format(T1.d_tb_create_time,'%yǯ%m��%d��%H��%iʬ')");	/* 3 �ȥ�å��Хå����� */
	strcat(cha_sql, " from at_trackback T1");
	strcat(cha_sql, ", at_entry T2");
	strcat(cha_sql, " where T1.n_entry_id = T2.n_entry_id");
	strcat(cha_sql, " and concat(T1.n_entry_id,':',T1.n_tb_id) in (");
	for(i = 0; i < in_checked_count; i++) {
		if(i) {
			strcat(cha_sql, ", ");
		}
		strcat(cha_sql, "'");
		strcat(cha_sql, Get_Nlist(nlp_in, "accept_trackback", i + 1));
		strcat(cha_sql, "'");
	}
	strcat(cha_sql, ")");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "��ǧ������ѵ�����������륯���꡼�˼��Ԥ��ޤ�����");
		} else {
			Put_Nlist(nlp_out, "ERROR", "��ǧ����ȥ�å��Хå���������륯���꡼�˼��Ԥ��ޤ�����");
		}
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if(!in_row) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿���ѵ����ϴ��˾�ǧ����Ƥ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿�ȥ�å��Хå��ϴ��˾�ǧ����Ƥ��ޤ���");
		}
		return 1;
	}
	if(in_row != in_checked_count) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "MSG", "���򤵤줿���ѵ����ϰ�����ǧ����Ƥ��ޤ���<br>");
		} else {
			Put_Nlist(nlp_out, "MSG", "���򤵤줿�ȥ�å��Хå��ϰ�����ǧ����Ƥ��ޤ���<br>");
		}
	}
	for(i = 0; i < in_row; i++) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "accept_trackback", Db_GetValue(dbres, i, 0));
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
	char *chp_tmp;
	char *chp_fromuser;
	char *chp_skel;
	char *chp_action;
	int in_blog;
	int in_error;
	int in_ret;
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
	chp_action = CO_CGI_TB_LIST;

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
					Put_Nlist(nlp_out, "ACTION", CO_CGI_TB_LIST);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "ǧ�ھ��������Ǥ��ޤ���<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_TB_LIST);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_TB_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�����Ԥˤ����������ӥ��Τ����Ѥ���ߤ��Ƥ���ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_TB_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_TB_LIST);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_TB_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥����Խ����븢�¤�����ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_TB_LIST);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥����������Ƥ��ʤ����Ϥ����Ѥˤʤ�ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);
	if (Need_Login(db, in_blog)) {
		Put_Nlist(nlp_out, "IP", "�˥å��͡���");
	} else {
		Put_Nlist(nlp_out, "IP", "IP");
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

	/* ����ȥꤴ�ȤΥȥ�å��Хå�������Ƥ�����ϡ����� */
	chp_tmp = Get_Nlist(nlp_in, "entry_id", 1);
	if(chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "entry_id", chp_tmp);
	}
	in_ret = 0;
	in_error = 0;
	chp_skel = 0;
	if(Get_Nlist(nlp_in, "BTN_DISP_DELETE_TRACKBACK_ASK.x", 1)) {
		if(disp_page_delete_trackback_ask(db, nlp_in, nlp_out, in_blog)) {
			in_ret = disp_page_trackbacklist(db, nlp_in, nlp_out, in_blog);
			if(in_ret == CO_ERROR) {
				in_error++;
			}
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = CO_SKEL_TRACKBACK_LIST;
		} else {
			chp_skel = CO_SKEL_DELETE_TRACKBACK_ASK;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DELETE_TRACKBACK.x", 1)) {
		if (delete_trackback(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = CO_SKEL_TRACKBACK_LIST;
		} else {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "MESSAGE", "���ѵ����������ޤ�����");
			} else {
				Put_Nlist(nlp_out, "MESSAGE", "�ȥ�å��Хå��������ޤ�����");
			}
			Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s?BTN_DISP_TRACKBACKLIST=1&blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_TB_LIST, in_blog);
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
	} else if (Get_Nlist(nlp_in, "BTN_DISP_ACCEPT_TRACKBACK_ASK.x", 1)) {
		if(disp_page_accept_trackback_ask(db, nlp_in, nlp_out, in_blog)) {
			in_ret = disp_page_trackbacklist(db, nlp_in, nlp_out, in_blog);
			if(in_ret == CO_ERROR) {
				in_error++;
			}
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = CO_SKEL_TRACKBACK_LIST;
		} else {
			chp_skel = CO_SKEL_ACCEPT_TRACKBACK_ASK;
		}
	} else if(Get_Nlist(nlp_in, "BTN_ACCEPT_TRACKBACK.x", 1)) {
		if (accept_trackback(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		in_ret = disp_page_trackbacklist(db, nlp_in, nlp_out, in_blog);
		if(in_ret == CO_ERROR) {
			in_error++;
		}
		chp_skel = CO_SKEL_TRACKBACK_LIST;
	} else if (Get_Nlist(nlp_in, "BTN_DISP_TRACKBACKLIST", 1) || Get_Nlist(nlp_in, "BTN_DISP_TRACKBACKLIST.x", 1)) {
		in_ret = disp_page_trackbacklist(db, nlp_in, nlp_out, in_blog);
		if(in_ret == CO_ERROR) {
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_ENTRYLIST", "1");
			chp_action = CO_CGI_ENTRY_LIST;
			in_error++;
		}
		chp_skel = CO_SKEL_TRACKBACK_LIST;
	} else {
		Put_Nlist(nlp_out, "ERROR", "ľ��CGI��¹ԤǤ��ޤ���<br>");
		in_error++;
	}
	if(in_error || in_ret == CO_ERROR || !chp_skel){
		put_error_data(nlp_in, nlp_out, chp_action);
		Page_Out(nlp_out, CO_SKEL_ERROR);
	} else {
		if(in_ret) {
			printf("Location: %s%s%s/%s?BTN_DISP_ENTRYLIST=1&blogid=%d\n\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY_LIST, in_blog);
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
