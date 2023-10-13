/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_entry.c
 * First-Created:	2004/08/ ��¼ ����Ϻ
%* ------------------------------------------------------------------------
 * Module-Description:
 *	��������ơ��Խ� ���ȥ�å��Хå�ping������ping����
-* ------------------------------------------------------------------------
 * Change-Log:
 *	2005/07/04 �غ�����
 *		ź�եե�����ν���������
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_entry.c,v 1.353 2010/03/19 09:30:30 n-wada Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <ctype.h>
#include <gd.h>
#include <libcgi2.h>
#include <excart.h>
#include <libexcart.h>
#include "libnkf.h"
#include "libnet.h"
#include "libhttp.h"
#include "libcgi2.h"
#include "libdb2.h"
#include "libcommon2.h"
#include "libjcode.h"
#include "libauth.h"
#include "libasjmovie.h"
#include "libblogcart.h"
#include "libblog.h"
#include "libblogcart.h"
#include "libblogreserve.h"

#define CO_EDITMODE_NEW		0
#define CO_EDITMODE_EDIT	1
#define CO_MAX_ATTACHED_FILE	5
#define CO_MAX_ATTACHED_MOVIE	1
#define CO_TEMPDIR_PATH "/data/reserve/images/"

/*
+* ------------------------------------------------------------------------
 * Function:		init_filetable
 * Description:
 *      ���åץ��ɤ��줿��������°���ʤ��ä��ե������ä�
 *	�Ĥޤꡢentry_id ������Τ�Τ�������
 *	�ޤ������ͽ�󤵤줿��������줺�˻ĤäƤ����Τ򸵤��᤹��
%* ------------------------------------------------------------------------
 * Return:
 *	����0������1
-* ------------------------------------------------------------------------*/
int init_filetable(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_sql[1024];

	/* �ե������ơ��֥뤫���� */
	strcpy(cha_sql, "delete from at_uploadfile");
	strcat(cha_sql, " where n_entry_id < 0");	/* ��ID */
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "�ե�����ơ��֥�ν����˼��Ԥ��ޤ�����(2)<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	/* ���ͽ�󤵤�Ƥ��ΤޤޤˤʤäƤ����Τ򸵤��᤹ */
	strcpy(cha_sql, "update at_uploadfile");
	strcat(cha_sql, " set b_delete_mode = 0");
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "�ե�����ơ��֥�ν����˼��Ԥ��ޤ�����(3)<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
		if(!Get_Nlist(nlp_in, "upload_movie", 1)) {
			/* ư��ե������ơ��֥뤫���� */
			strcpy(cha_sql, "delete from at_uploadmovie");
			strcat(cha_sql, " where n_entry_id < 0");	/* ��ID */
			sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "ư��ե�����ơ��֥�ν����˼��Ԥ��ޤ�����(2)<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				return 1;
			}
		}
		/* ���ͽ�󤵤�Ƥ��ΤޤޤˤʤäƤ���ư��ե�����򸵤��᤹ */
		strcpy(cha_sql, "update at_uploadmovie");
		strcat(cha_sql, " set b_delete_mode = 0");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "ư��ե�����ơ��֥�ν����˼��Ԥ��ޤ�����(3)<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	error_check
 * Description:
 *	���Ϲ��ܤΥ��顼�����å�
%* ------------------------------------------------------------------------
 * Return:
 *	0 ���� 1�ʾ� �۾�
-* ------------------------------------------------------------------------*/
int error_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_date[16];
	char cha_sql[512];
	char cha_error[512];
	int in_error;

	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����ȥ�����Ϥ��Ƥ���������<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_ENTRY_TITLE) {
		sprintf(cha_error, "�����ȥ��Ⱦ��%dʸ��������%dʸ���˰�������Ϥ��Ƥ���������<br>"
			, CO_MAXLEN_ENTRY_TITLE, CO_MAXLEN_ENTRY_TITLE / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "�����ȥ�ˤϡ�����ʳ���ʸ�������Ϥ��Ƥ���������<br>");
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_body", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "���Ƥ����Ϥ��Ƥ���������<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_ENTRY_BODY) {
		sprintf(cha_error, "���Ƥ�Ⱦ��%dʸ��������%dʸ���˰�������Ϥ��Ƥ���������<br>"
			, CO_MAXLEN_ENTRY_BODY, CO_MAXLEN_ENTRY_BODY/2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "���Ƥˤϡ�����ʳ���ʸ�������Ϥ��Ƥ���������<br>");
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_more", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "����(³��)������Ǥ��ޤ���Ǥ�����<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_ENTRY_MORE) {
		sprintf(cha_error, "����(³��)��Ⱦ��%dʸ��������%dʸ���˰�������Ϥ��Ƥ���������<br>"
			, CO_MAXLEN_ENTRY_MORE, CO_MAXLEN_ENTRY_MORE/2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
	if (!chp_tmp){
		Put_Nlist(nlp_out, "ERROR", "���פμ����˼��Ԥ��ޤ�����<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_ENTRY_SUMMARY) {
		sprintf(cha_error, "���פ�Ⱦ��%dʸ��������%dʸ���˰�������Ϥ��Ƥ���������<br>"
			, CO_MAXLEN_ENTRY_SUMMARY, CO_MAXLEN_ENTRY_SUMMARY/2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if (strlen(chp_tmp) > 0 && Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "���פˤϡ�����ʳ���ʸ�������Ϥ��Ƥ���������<br>");
		in_error++;
	}
	sprintf(cha_date, "%s/%s/%s", Get_Nlist(nlp_in, "year", 1), Get_Nlist(nlp_in, "month", 1), Get_Nlist(nlp_in, "day", 1));
	if (Check_Date(cha_date)){
		Put_Nlist(nlp_out, "ERROR", "���դλ���˸�꤬����ޤ���<br>");
		in_error++;
	}
	sprintf(cha_date, "%s:%s:00", Get_Nlist(nlp_in, "hour", 1), Get_Nlist(nlp_in, "minute", 1));
	if (Check_Time(cha_date)) {
		Put_Nlist(nlp_out, "ERROR", "���֤λ���˸�꤬����ޤ���<br>");
		in_error++;
	}
	if(g_in_cart_mode == CO_CART_SHOPPER) {
		chp_tmp = Get_Nlist(nlp_in, "item_id", 1);
		if(chp_tmp && *chp_tmp) {
			if(strlen(chp_tmp) > CO_ITEM_ID_LENGTH) {
				memset(cha_error, '\0', sizeof(cha_error));
				sprintf(cha_error, "���ʥ����ɤ�Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_ITEM_ID_LENGTH);
				Put_Nlist(nlp_out, "ERROR", cha_error);
				in_error++;
			} else if(Check_Code(chp_tmp)) {
				Put_Nlist(nlp_out, "ERROR", "���ʥ����ɤ�̵����ʸ��������ޤ���ʸ����Ƴ�ǧ���Ƥ���������<br>");
				in_error++;
			} else {
				/* ���ʥ����ɽ�ʣ�����å� */
				memset(cha_sql, '\0', sizeof(cha_sql));
				strcpy(cha_sql, "select count(*)");
				strcat(cha_sql, " from sm_item T1");
				sprintf(cha_sql + strlen(cha_sql), " where T1.c_item_id = '%s'", chp_tmp);
				dbres = Db_OpenDyna(db, cha_sql);
				if(!dbres) {
					memset(cha_sql, '\0', sizeof(cha_sql));
					sprintf(cha_sql, "���ʾ���μ����˼��Ԥ��ޤ�����(%s)", Gcha_last_error);
					Put_Nlist(nlp_out, "ERROR", cha_sql);
					return 1;
				}
				chp_tmp = Db_GetValue(dbres, 0, 0);
				if(!chp_tmp) {
					memset(cha_sql, '\0', sizeof(cha_sql));
					sprintf(cha_sql, "���ʾ���μ����˼��Ԥ��ޤ�����(%s)", Gcha_last_error);
					Put_Nlist(nlp_out, "ERROR", cha_sql);
					Db_CloseDyna(dbres);
					return 1;
				}
				if(atoi(chp_tmp) == 0) {
					Put_Nlist(nlp_out, "ERROR", "���ꤵ�줿���ʥ����ɤξ��ʤ�¸�ߤ��ޤ���<br>\n");
					in_error++;
				}
				Db_CloseDyna(dbres);
			}
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
	if (!chp_tmp){
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "�������Ѽ��ղ��ݤ����򤷤Ƥ���������<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����ղ��ݤ����򤷤Ƥ���������<br>");
		}
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (!chp_tmp){
		Put_Nlist(nlp_out, "ERROR", "�����ȼ��ղ��ݤ����򤷤Ƥ���������<br>");
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����⡼�ɤ����򤷤Ƥ���������<br>");
		in_error++;
	}
	/*�ȥ�å��Хå�PING�����Υ����å�*/
	chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
	if (chp_tmp && *chp_tmp) {
		if (g_in_dbb_mode) {
			if (Check_URL_With_Crlf(nlp_out, chp_tmp, "����������", CO_MAX_TRACKBACK_URLS)) {
				in_error++;
			}
		} else {
			if (Check_URL_With_Crlf(nlp_out, chp_tmp, "�ȥ�å��Хå���", CO_MAX_TRACKBACK_URLS)) {
				in_error++;
			}
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "related_url", 1);
	if (chp_tmp && *chp_tmp) {
		if (Check_URL_With_Crlf(nlp_out, chp_tmp, "��Ϣ���", CO_MAX_TRACKBACK_URLS)) {
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "updateping_text", 1);
	if (chp_tmp && *chp_tmp) {
		if (Check_URL_With_Crlf(nlp_out, chp_tmp, "����ping������", CO_MAX_UPDATEPING_URLS)) {
			in_error++;
		}
	}
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:		put_related_link_list
 * Description:
 *	�����ȥ�å��Хå��ꥹ�Ȥ����
%* ------------------------------------------------------------------------
 * Return:			�ʤ�
 *
-* ------------------------------------------------------------------------*/
int put_related_link_list(DBase *db, NLIST *nlp_out, int in_blog, int in_entry)
{
	DBRes *dbres;
	char cha_sql[512];
	int in_count;
	int i;

	/* �ȥ�å��Хå��ꥹ�Ȥ��� */
	sprintf(cha_sql,
		" select T1.c_link_url"			/* 1 ������URL */
		" from at_sendlink T1"
		" where T1.n_entry_id = %d"
		" and T1.n_blog_id = %d"
		" and T1.b_link_tb = 0"
		" order by T1.n_link_id",
		in_entry, in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ����ꥹ�Ȥ����륯����˼��Ԥ��ޤ�����<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��ꥹ�Ȥ����륯����˼��Ԥ��ޤ�����<br>");
		}
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	if (!in_count) {
		Db_CloseDyna(dbres);
		return 0;
	}
	for (i = 0; i < in_count; ++i) {
		Put_Format_Nlist(nlp_out, "RERATEDURL", "%s\r\n", Db_GetValue(dbres, i, 0) ? Db_GetValue(dbres, i, 0) : "");
	}
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	put_send_trackback_list
 * Description:
 *	�����ȥ�å��Хå��ꥹ�Ȥ����
%* ------------------------------------------------------------------------
 * Return:			�ʤ�
 *
-* ------------------------------------------------------------------------*/
int put_send_trackback_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry)
{
	DBRes *dbres;
	char cha_sql[512];
	int in_count;
	int i;

	/* �ȥ�å��Хå��ꥹ�Ȥ��� */
	sprintf(cha_sql,
		" select"
			" T1.n_tb_id"			/* 0 �ȥ�å��Хå�ID */
			",T1.c_tb_url"			/* 1 ������URL */
			",date_format(T1.d_tb_create_time,'%%Y/%%m/%%d %%H:%%i')"	/* 2 �������� */
			",T1.b_success"			/* 3 ������� */
		" from"
			" at_sendtb T1"
		" where"
			" T1.n_entry_id=%d"
		" and"
			" T1.n_blog_id=%d"
		" order by"
			" T1.n_tb_id desc",
		in_entry, in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ����ꥹ�Ȥ����륯����˼��Ԥ��ޤ�����<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��ꥹ�Ȥ����륯����˼��Ԥ��ޤ�����<br>");
		}
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	if (!in_count) {
		Db_CloseDyna(dbres);
		return 0;
	}

	Put_Nlist(nlp_out, "TRACKBACKRT", "<tr class=\"a1\">\n");
	if (g_in_dbb_mode) {
		Put_Nlist(nlp_out, "TRACKBACKRT", "\t<td nowrap valign=\"top\" bgcolor=\"#c2e2ff\">�������������������</td>\n");
	} else if (g_in_cart_mode == CO_CART_RESERVE) {
		Put_Nlist(nlp_out, "TRACKBACKRT", "\t<td nowrap valign=\"top\" bgcolor=\"#B3E1E0\">�ȥ�å��Хå��������</td>\n");
	} else {
		Put_Nlist(nlp_out, "TRACKBACKRT", "\t<td nowrap valign=\"top\" bgcolor=\"#c2e2ff\">�ȥ�å��Хå��������</td>\n");
	}
	Put_Nlist(nlp_out, "TRACKBACKRT", "\t<td>\n");
	Put_Nlist(nlp_out, "TRACKBACKRT", "\t\t<table border=\"1\" cellspacing=\"0\" cellpadding=\"2\" cellpadding=\"2\" style=\"border:solid #8ba6da 1px;\" class=\"a1\">\n");
	Put_Nlist(nlp_out, "TRACKBACKRT", "\t\t<tr style=\"font-size:12; background-color:#F0F5FD;\"><td>������</td><td>��������</td><td nowrap>�������</td></tr>\n");

	for (i = 0; i < in_count; ++i) {
		Put_Nlist(nlp_out, "TRACKBACKRT", "\t\t<tr>\n");
		Put_Format_Nlist(nlp_out, "TRACKBACKRT", "\t\t\t<td>%s</td>\n", Db_GetValue(dbres, i, 1) ? Db_GetValue(dbres, i, 1) : "");
		Put_Format_Nlist(nlp_out, "TRACKBACKRT", "\t\t\t<td>%s</td>\n", Db_GetValue(dbres, i, 2) ? Db_GetValue(dbres, i, 2) : "");
		if (Db_GetValue(dbres, i, 3) && atoi(Db_GetValue(dbres, i, 3))) {
			Put_Nlist(nlp_out, "TRACKBACKRT", "\t\t\t<td>����</td>\n");
		} else {
			Put_Format_Nlist(nlp_out, "TRACKBACKRT",
				"\t\t\t<td><input type=\"button\" value=\"������\" onclick=\"location.href='%s%s%s/%s?BTN_RESEND_TRACKBACK=1&blogid=%d&entry_id=%d&tbid=%s'\"></td>\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY, in_blog, in_entry, Db_GetValue(dbres, i, 0) ? Db_GetValue(dbres, i, 0) : "0");
		}
		Put_Nlist(nlp_out, "TRACKBACKRT", "\t\t</tr>\n");
	}

	Put_Nlist(nlp_out, "TRACKBACKRT", "\t\t</table>\n");
	Put_Nlist(nlp_out, "TRACKBACKRT", "\t</td>\n");
	Put_Nlist(nlp_out, "TRACKBACKRT", "</tr>\n");
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	put_update_ping_list
 * Description:
 *	�ԥ󥰥ꥹ�Ȥ����
%* ------------------------------------------------------------------------
 * Return:			�ʤ�
 *
-* ------------------------------------------------------------------------*/
int put_update_ping_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_sql[512];
	char cha_msg[64];
	int in_count;
	int in_checked_count;
	int in_handover_flg;	/* ���Ѥ��ե饰 */
	int in_checked_flg;
	int i;
	int j;

	/* ���˲��̤�ɽ������Ƥ���Ȥ��ϡ����β��̤�ɽ������Ƥ����Τ�����Ѥ� */
	if (Get_Nlist(nlp_in, "BTN_UPLOAD_FILE", 1) || Get_Nlist(nlp_in, "BTN_UPLOAD_MOVIE", 1) || Get_Nlist(nlp_in, "BTN_BACK_FROM_ERROR.x", 1) || Get_Nlist(nlp_in, "BTN_DELETE_FILE", 1) || Get_Nlist(nlp_in, "BTN_DELETE_MOVIE", 1)) {
		in_handover_flg = 1;
		in_checked_count = Get_NlistCount(nlp_in, "updateping");
	} else {
		in_handover_flg = 0;
		in_checked_count = 0;
	}

	/* �ԥ󥰥ꥹ�Ȥ��� */
	sprintf(cha_sql,
		" select"
			" T1.n_ping_id"		/* 0 �ԥ�ID */
			",T1.c_ping_site"	/* 1 ping������̾ */
			",T2.b_default"		/* 2 ����� */
			",T2.c_ping_url"	/* 3 ping������URL */
		" from"
			" sy_ping T1"
			",at_ping T2"
		" where"
			" T1.n_ping_id=T2.n_ping_id"
		" and"
			" T2.n_blog_id=%d"
		" order by"
			" T1.n_ping_id",
		in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ԥ󥰥ꥹ�Ȥ����륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	/* �Ѱդ��줿�ԥ󥰥ꥹ�Ⱥ��� */
	in_count = Db_GetRowCount(dbres);
	for(i = 0; i < in_count; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		sprintf(cha_msg, "updateping%s", chp_tmp);
		in_checked_flg = 0;
		if (in_handover_flg) {
			for(j = 0; j < in_checked_count; j++) {
				chp_tmp = Db_GetValue(dbres, i, 3);
				chp_tmp2 = Get_Nlist(nlp_in, "updateping", j + 1);
				if (chp_tmp && chp_tmp2 && strcmp(chp_tmp, chp_tmp2) == 0) {
					in_checked_flg = 1;
					break;
				}
			}
		} else {
			chp_tmp2 = Db_GetValue(dbres, i, 2);
			if (chp_tmp2 && atoi(chp_tmp2)) {
				in_checked_flg = 1;
			}
		}
		chp_escape = Escape_HtmlString(Db_GetValue(dbres, i, 1));
		chp_tmp2 = Escape_HtmlString(Db_GetValue(dbres, i, 3));
		Build_Checkbox_With_Id(nlp_out, "UPDATEPING", "updateping", chp_tmp2, in_checked_flg, cha_msg, chp_escape);
		Put_Nlist(nlp_out, "UPDATEPING", "<br>\n");
		free(chp_tmp2);
		free(chp_escape);
	}
	Db_CloseDyna(dbres);
	/* �ƥ����Ȥ���� */
	Put_Nlist(nlp_out, "UPDATEPING", "������ɲä�����ϲ����ˣ�������򣱹ԤȤ��Ƶ������Ƥ���������<br>");
	if (in_handover_flg) {
		chp_tmp = Get_Nlist(nlp_in, "updateping_text", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
		} else {
			chp_escape = malloc(1);
			*chp_escape = '\0';
		}
	} else {
		if (Begin_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥������γ��Ϥ˼��Ԥ��ޤ���<br>");
			return 1;
		}
		sprintf(cha_sql, "select coalesce(max(T1.n_ping_id), 0) from sy_ping T1 for update");
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "ping��������Ͽ�˼��Ԥ��ޤ���(4)");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_count = atoi(chp_tmp);
		} else {
			in_count = 0;
		}
		Db_CloseDyna(dbres);
		sprintf(cha_sql,
			" select"
				" T1.c_ping_url"
			" from"
				" at_ping T1"
			" where"
				"     T1.n_ping_id > %d"
				" and T1.n_blog_id = %d",
			in_count, in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres){
			Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		if (Commit_Transact(db)){
			Put_Nlist(nlp_out, "ERROR", "���ߥåȤ˼��Ԥ��ޤ���<br>");
			Rollback_Transact(db);
			return 1;
		}
		in_count = Db_GetRowCount(dbres);
		if (in_count) {
			chp_escape = (char*)malloc(in_count * 258);
		} else {
			chp_escape = (char*)malloc(1);
		}
		*chp_escape = '\0';
		for (i = 0; i < in_count; ++i) {
			chp_tmp = Db_GetValue(dbres, i, 0);
			if (chp_tmp) {
				chp_tmp2 = Escape_HtmlString(chp_tmp);
				strcat(chp_escape, chp_tmp2);
				strcat(chp_escape, "\n");
				free(chp_tmp2);
			}
		}
	}
	Build_Inputarea(nlp_out, "UPDATEPING", "updateping_text", chp_escape, CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
	free(chp_escape);
	if (!in_handover_flg) {
		Db_CloseDyna(dbres);
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_newentry()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int disp_page_newentry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	struct tm* stp_now;
	time_t tm_now;
	char *chp_tmp;
	char *chp_tmp2;
	char cha_num[32];
	char cha_sql[512];
	char cha_str[1024];
	char cha_content[4096];	/* ��ñ��� */
	char cha_path[256];		/* ��ñ��� */
	char cha_title[256];	/* ��ñ��� */
	char *chp_content;
	char *chp_title;
	char *chp_path;
	int in_browser;

	/* ź�եե�����β���Ͽ���Ѥ���ե�����ID������ */
	/* �Ǥ���¤��դˤ��롣 */
	tm_now = time(NULL);
	sprintf(cha_str, "%d", -((int)tm_now % INT_MAX) - 1);	/* ID��-1����-(INT_MAX)�ˤʤ� */
	Build_HiddenEncode(nlp_out, "HIDDEN", "tmpfile_id", cha_str);
	/* �⡼�ɤ����Ƥ��� */
	sprintf(cha_str, "%d", CO_EDITMODE_NEW);
	Build_HiddenEncode(nlp_out, "HIDDEN", "editmode", cha_str);
	if (init_filetable(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	/* �ǥե���Ȥ��� */
	strcpy(cha_sql, "select T1.n_blog_category");	/* ���ƥ��꡼ID */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "���ƥ��꡼����ͤ����륯����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUREY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		strcpy(cha_str, chp_tmp);
	} else {
		strcpy(cha_str, "0");
	}
	Db_CloseDyna(dbres);

	if (g_in_dbb_mode) {
		if (Build_Team_Combo(db, nlp_in, nlp_out, "TEAM", in_blog, "team", NULL, NULL)) {
			return 1;
		}
		Build_Hidden(nlp_out, "HIDDEN", "entry_category", "0");
		if (Get_Nlist(nlp_in, "BTN_EASY_TRACKBACK", 1)) {
			Put_Nlist(nlp_out, "EASY_TRACKBACK", "1");
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_EASY_TRACKBACK", "1");
		} else {
			Put_Nlist(nlp_out, "EASY_TRACKBACK", "0");
		}
	} else {
		/* ���ƥ���Υ��쥯�ȥܥå��� */
		strcpy(cha_sql, "select T1.n_category_id");	/* 0 ���ƥ���ID */
		strcat(cha_sql, " , T1.c_category_name");	/* 1 ���ƥ���̾ */
		strcat(cha_sql, " from at_category T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		strcat(cha_sql, " order by T1.n_category_order");
		Build_Combobox(OldDBase(db), nlp_out, "CATEGORY", cha_sql, "entry_category", cha_str);
	}
	/* ��������Υ��쥯�ȥܥå����ȡ������ȼ���饸���ܥ����Ф��� */
	Build_Radio_With_Id(nlp_out, "DATE", "specify_date", "0", 1, "specify0", "��¸������������������Ȥ���");
	Put_Nlist(nlp_out, "DATE", "<br>");
	Build_Radio_With_Id(nlp_out, "DATE", "specify_date", "1", 0, "specify1", "��������ꤹ��");
	stp_now = localtime(&tm_now);
	sprintf(cha_str, "%d/%02d/%02d", stp_now->tm_year + 1900, stp_now->tm_mon + 1, stp_now->tm_mday);
	Build_Date(nlp_out, "DATE", "year", "month", "day", stp_now->tm_year + 1850, stp_now->tm_year + 1950, cha_str);
	sprintf(cha_str, "%02d:%02d", stp_now->tm_hour, stp_now->tm_min);
	Build_ComboTime(nlp_out, "DATE", "hour","minute", 1, cha_str);
	if (g_in_hb_mode) {
		Build_Checkbox(nlp_out, "DELDATE", "del_auto", "1", 0);
		sprintf(cha_str, "%d/%02d/%02d", stp_now->tm_year + 1900, stp_now->tm_mon + 1, stp_now->tm_mday);
		Build_Date(nlp_out, "DELDATE", "del_year", "del_month", "del_day", stp_now->tm_year + 1850, stp_now->tm_year + 1950, cha_str);
		Put_Nlist(nlp_out, "DELDATE", "�ˤʤä���������");
	}
	Put_Nlist(nlp_out, "TITLE", "�� �� �� ��");
	/* �ե����륢�åץ����� */
	Build_Input_File(nlp_out, "FILEBUTTON", "file", "", 30, 255);
	Build_Submit(nlp_out, "FILEBUTTON", "BTN_UPLOAD_FILE", "�ե�����Υ��åץ���");
	if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
		/* ư�襢�åץ����� */
		Build_Input_File(nlp_out, "MOVIEBUTTON", "movie", "", 30, 255);
		Build_Submit(nlp_out, "MOVIEBUTTON", "BTN_UPLOAD_MOVIE", "ư��Υ��åץ���");
	}
	/* ����ͤ˱�äƥ饸���ܥ���Υ����å� */
	strcpy(cha_sql, " select T1.b_default_trackback");	/* 0 �ȥ�å��Хå������ */
	strcat(cha_sql, ",T1.b_default_comment");			/* 1 �����Ƚ���� */
	strcat(cha_sql, ",T1.b_default_mode");				/* 2 ��ƥ⡼�ɽ���� */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�饸���ܥ������ͤ����륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		switch (atoi(chp_tmp)) {
		case 1:
			Put_Nlist(nlp_out, "TRACKBACK1", CO_CHECKED);
			break;
		case 2:
			Put_Nlist(nlp_out, "TRACKBACK2", CO_CHECKED);
			break;
		case 3:
			Put_Nlist(nlp_out, "TRACKBACK3", CO_CHECKED);
			break;
		default:
			Put_Nlist(nlp_out, "TRACKBACK0", CO_CHECKED);
		}
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		switch (atoi(chp_tmp)) {
		case 1:
			Put_Nlist(nlp_out, "COMMENT1", CO_CHECKED);
			break;
		case 2:
			Put_Nlist(nlp_out, "COMMENT2", CO_CHECKED);
			break;
		case 3:
			Put_Nlist(nlp_out, "COMMENT3", CO_CHECKED);
			break;
		default:
			Put_Nlist(nlp_out, "COMMENT0", CO_CHECKED);
		}
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "MODE1", CO_CHECKED);
	} else {
		Put_Nlist(nlp_out, "MODE0", CO_CHECKED);
	}
	Db_CloseDyna(dbres);

	/* ��ñ���(�֥å��ޡ�����å�) */
	if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
		Put_Format_Nlist(nlp_out, "DBB", "%d", g_in_dbb_mode);
		if (g_in_dbb_mode) {
			chp_tmp = Get_Nlist(nlp_in, "f", 1);
			if (chp_tmp && (strcasecmp(chp_tmp, "C") == 0 || strcasecmp(chp_tmp, "L") == 0 || strcasecmp(chp_tmp, "B") == 0)) {
				strcpy(cha_title, chp_tmp);
				chp_tmp = Get_Nlist(nlp_in, "l", 1);
				if (chp_tmp) {
					strcpy(cha_path, chp_tmp);
				} else {
					strcpy(cha_path, "");
				}
				chp_tmp = Get_Nlist(nlp_in, "b", 1);
				if (chp_tmp) {
					strcpy(cha_content, chp_tmp);
				} else {
					strcpy(cha_content, "");
				}
				chp_tmp = Get_Nlist(nlp_in, "o", 1);
				if (chp_tmp) {
					strcpy(cha_sql, chp_tmp);
				} else {
					strcpy(cha_sql, "");
				}
			}
		} else {
			if (Get_Nlist(nlp_in, "from_preview", 1)) {
				chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
				if (chp_tmp) {
					Put_Nlist(nlp_out, "ENTRYTITLE", chp_tmp);
				}
			}
			chp_tmp = Get_Nlist(nlp_in, "t", 1);
			if (chp_tmp) {
				strcpy(cha_title, chp_tmp);
			} else {
				strcpy(cha_title, "");
			}
			chp_tmp = Get_Nlist(nlp_in, "l", 1);
			if (chp_tmp) {
				strcpy(cha_path, chp_tmp);
			} else {
				strcpy(cha_path, "");
			}
			chp_tmp = Get_Nlist(nlp_in, "b", 1);
			if (chp_tmp) {
				strcpy(cha_content, chp_tmp);
			} else {
				strcpy(cha_content, "");
			}
			if (Detect_Code((unsigned char *)cha_path, strlen(cha_path)) == CO_ASCII && strcasestr(cha_path, "%u")) {
				chp_path = Copy_Str(cha_path);
			} else {
				chp_path = Encode_Blog_Url(cha_path);
			}
			if (Detect_Code((unsigned char *)cha_title, strlen(cha_title)) == CO_ASCII && strcasestr(cha_title, "%u")) {
				chp_title = Copy_Str(cha_title);
			} else {
				chp_title = Encode_Blog_Url(cha_title);
			}
			if (Detect_Code((unsigned char *)cha_content, strlen(cha_content)) == CO_ASCII && strcasestr(cha_content, "%u")) {
				chp_content = Copy_Str(cha_content);
			} else {
				chp_content = Encode_Blog_Url(cha_content);
			}
			Put_Nlist(nlp_out, "HIDDEN_ENTRYBODY1", chp_path);
			Put_Nlist(nlp_out, "HIDDEN_ENTRYBODY2", chp_title);
			Put_Nlist(nlp_out, "HIDDEN_ENTRYBODY3", chp_content);
			free(chp_path);
			free(chp_title);
			free(chp_content);

			in_browser = Get_Browser();
			if (in_browser == CO_BROWSER_NS6) {
				Put_Nlist(nlp_out, "DECODE1", "decodeURIComponent(document.myEscape.after1.value)");
				Put_Nlist(nlp_out, "DECODE2", "decodeURIComponent(document.myEscape.after2.value)");
				Put_Nlist(nlp_out, "DECODE3", "decodeURIComponent(document.myEscape.after3.value)");
			} else if (in_browser == CO_BROWSER_SAFARI) {
				Put_Nlist(nlp_out, "DECODE1", "unescape(unescape(document.myEscape.after1.value))");
				Put_Nlist(nlp_out, "DECODE2", "unescape(unescape(document.myEscape.after2.value))");
				Put_Nlist(nlp_out, "DECODE3", "unescape(unescape(document.myEscape.after3.value))");
			} else {
				Put_Nlist(nlp_out, "DECODE1", "unescape(document.myEscape.after1.value)");
				Put_Nlist(nlp_out, "DECODE2", "unescape(document.myEscape.after2.value)");
				Put_Nlist(nlp_out, "DECODE3", "unescape(document.myEscape.after3.value)");
			}
		}
	}
	if (g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_EASY_TRACKBACK", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "bid", 1);
		chp_tmp2 = Get_Nlist(nlp_in, "eid", 1);
		if (chp_tmp && chp_tmp2) {
			Put_Format_Nlist(nlp_out, "TRACKBACKURL", "%s%s%s/%s/%s-%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_TB, chp_tmp, chp_tmp2);
		}
	}
	if (put_update_ping_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	in_browser = chp_tmp ? atoi(chp_tmp) : 0;
	if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
		Put_Format_Nlist(nlp_out, "RESETBTN",
			"<input type=\"image\" src=\"%s/reset.gif\" name=\"BTN_DISP_NEWENTRY\">", g_cha_admin_image);
	} else if (g_in_dbb_mode && Get_Nlist(nlp_in, "BTN_EASY_TRACKBACK", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "bid", 1);
		chp_tmp2 = Get_Nlist(nlp_in, "eid", 1);
		Put_Format_Nlist(nlp_out, "RESETBTN",
			"<a href=\"%s%s%s/%s?BTN_DISP_NEWENTRY=1&blogid=%d&from_user=%d&BTN_EASY_TRACKBACK=1&bid=%s&eid=%s\"><img src=\"%s/reset.gif\" border=\"0\" alt=\"�ꥻ�å�\"></a>",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi,  CO_CGI_ENTRY, in_blog, in_browser,
			chp_tmp ? chp_tmp : "0", chp_tmp2 ? chp_tmp2 : "0", g_cha_admin_image);
	} else {
		Put_Format_Nlist(nlp_out, "RESETBTN",
			"<a href=\"%s%s%s/%s?BTN_DISP_NEWENTRY=1&blogid=%d&from_user=%d\"><img src=\"%s/reset.gif\" border=\"0\" alt=\"�ꥻ�å�\"></a>",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi,  CO_CGI_ENTRY, in_blog, in_browser, g_cha_admin_image);
	}
	Put_Nlist(nlp_out, "BUTTON", "toukou.gif");
	if (in_browser) {
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
	} else {
		Put_Format_Nlist(nlp_out, "PREVCGI", "%s/%s?blogid=%d", g_cha_admin_cgi, CO_CGI_MENU, in_blog);
	}

	if (g_in_dbb_mode) {
		cha_num[0] = '\0';
		chp_tmp = Get_Nlist(nlp_in, "o", 1);
		if (chp_tmp) {
			strcpy(cha_num, chp_tmp);
			chp_tmp = Get_Nlist(nlp_in, "f", 1);
			if (chp_tmp && (chp_tmp[0] == 'c' || chp_tmp[0] == 'C')) {
				int in_tmp;
				chp_tmp = Get_Nlist(nlp_in, "o", 1);
				in_tmp = Temp_To_Owner(db, nlp_out, chp_tmp);
				if (in_tmp >= 0) {
					sprintf(cha_num, "%d", in_tmp);
				}
			}
		}
		chp_tmp = Get_DBB_Topic(db, nlp_out, in_blog, Get_Nlist(nlp_in, "f", 1), Get_Nlist(nlp_in, "b", 1), cha_num, Get_Nlist(nlp_in, "l", 1));
		if (chp_tmp) {
			Put_Nlist(nlp_out, "DBBTOPIC", chp_tmp);
			free(chp_tmp);
			/*�����å��ܥå���*/
			if(Get_Nlist(nlp_in, "b", 1) && In_Battle_Owner(db, nlp_out, g_in_login_owner, Get_Nlist(nlp_in, "b", 1))) {
				memset(cha_str, '\0', sizeof(cha_str));
				sprintf(cha_str, "&nbsp;&nbsp;<p style=\"font-size:12px;\"><input type=\"checkbox\" value=\"1\" name=\"INSERT_BATTLE_ORDER\"%s>��������Υ����������������롣</p>", Get_Nlist(nlp_in, "INSERT_BATTLE_ORDER", 1) ? " checked" : "");
				Put_Nlist(nlp_out, "DBBTOPIC", cha_str);
			}
		}
	}

	if (g_in_cart_mode == CO_CART_RESERVE) {
		if (Build_Reserve_Combo(db, nlp_out, "ITEM", "item_id", NULL, in_blog, g_in_login_owner)) {
			return 1;
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	put_attached_file_list()
 * Description:
 *	ź�եե����������ɽ�����롣
 *	���˵�����°���Ƥ����Τ�entry_id����
 *	�ޤ�����Τ�Τ�entry_id = chp_tmpfile_id�ȤʤäƤ���
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int put_attached_file_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_entry_id;	/* ����ID */
	char *chp_tmpfile_id;	/* ����Ͽ��ID */
	char *chp_filename;
	char *chp_filename_org;
	char *chp_filetype;
	char *chp_uploadfile_id;
	char cha_sql[1024];
	char cha_msg[1024];
	int in_row;
	int in_max_attach;
	int i;

	in_max_attach = CO_MAX_ATTACHED_FILE;
	dbres = Db_OpenDyna(db, "select n_max_attach from sy_baseinfo");
	if (dbres) {
		chp_filename = Db_GetValue(dbres, 0, 0);
		if (chp_filename) {
			in_max_attach = atoi(chp_filename);
		}
		Db_CloseDyna(dbres);
	}

	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	strcpy(cha_sql, "select T1.c_fileimage");	/* 0 �����о�ե�����̾ */
	strcat(cha_sql, ", T1.c_filetype");			/* 1 �ե����륿���� */
	strcat(cha_sql, ", T1.c_filename");			/* 2 �ե�����̾ */
	strcat(cha_sql, ", T1.n_uploadfile_id");	/* 3 ���åץ��ɥե�����ID */
	strcat(cha_sql, ", T1.n_entry_id");			/* 4 ����ID */
	strcat(cha_sql, " from at_uploadfile T1");
	strcat(cha_sql, " where T1.b_delete_mode = 0");	/* ���ͽ�󤵤�Ƥ��ʤ� */
	/* ������°���Ƥ����Τȡ����ե�����ȤʤäƤ����Τ򥻥쥯�� */
	if (chp_entry_id || chp_tmpfile_id) {
		strcat(cha_sql, " and T1.n_entry_id in (");
		if (chp_entry_id) {
			strcat(cha_sql, chp_entry_id);
		}
		if (chp_entry_id && chp_tmpfile_id) {
			strcat(cha_sql, ", ");
		}
		if (chp_tmpfile_id) {
			strcat(cha_sql, chp_tmpfile_id);
		}
		strcat(cha_sql, ")");
	}
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������°���Ƥ�������ե���������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	/* �ե�����ꥹ�� */
	if (in_row != 0) {
		Put_Nlist(nlp_out, "ATTACHED_FILE", "<table>\n");
		for(i = 0; i < in_row; i++){
			chp_filename = Db_GetValue(dbres, i, 0);
			chp_filetype = Db_GetValue(dbres, i, 1);
			chp_filename_org = Db_GetValue(dbres, i, 2);
			chp_uploadfile_id = Db_GetValue(dbres, i, 3);
			chp_entry_id = Db_GetValue(dbres, i, 4);
			if (chp_filename && chp_filetype && chp_filename_org && chp_uploadfile_id){
				Put_Nlist(nlp_out, "ATTACHED_FILE", "<tr><td>\n");
				sprintf(cha_msg, "%s:%s", chp_entry_id, chp_uploadfile_id);
				Build_Checkbox(nlp_out, "ATTACHED_FILE", "delete_file", cha_msg, 0);
				Put_Format_Nlist(nlp_out
					, "ATTACHED_FILE", "<a href=\"%s%s%s/%s?BTN_DISP_ATTACH_FILE=1&blogid=%d&entry_id=%s&uploadid=%s\" target=\"_blank\">%s</a>(%s)"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY, in_blog, chp_entry_id, chp_uploadfile_id, chp_filename_org, chp_filetype);
				Put_Nlist(nlp_out, "ATTACHED_FILE", "</td></tr>\n");
			}
		}
		Put_Nlist(nlp_out, "ATTACHED_FILE", "</table>\n");
		Build_Submit(nlp_out, "ATTACHED_FILE", "BTN_DELETE_FILE_ASK", "�����å����������ե��������");
		if (g_in_html_editor) {
			Put_Nlist(nlp_out, "ATTACHED_FILE", "&nbsp;\n");
			Build_Submit(nlp_out, "ATTACHED_FILE", "BTN_DISPLAY_FILE", "�����å����������ե������ɽ��");
		}
	}
	if (in_row < in_max_attach/*CO_MAX_ATTACHED_FILE*/) {
		/* �ե����륢�åץ����� */
		Build_Input_File(nlp_out, "FILEBUTTON", "file", "", 30, 255);
		Build_Submit(nlp_out, "FILEBUTTON", "BTN_UPLOAD_FILE", "�ե�����Υ��åץ���");
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	put_attached_movie_list()
 * Description:
 *	ź��ư�������ɽ�����롣
 *	���˵�����°���Ƥ����Τ�entry_id����
 *	�ޤ�����Τ�Τ�entry_id = chp_tmpfile_id�ȤʤäƤ���
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int put_attached_movie_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_entry_id;	/* ����ID */
	char *chp_tmpfile_id;	/* ����Ͽ��ID */
	char *chp_filename;
	char cha_sql[1024];
	int in_row;
	int in_max_attach;
	int i;

	in_max_attach = CO_MAX_ATTACHED_MOVIE;
	dbres = Db_OpenDyna(db, "select n_max_attach from sy_baseinfo");
	if (dbres) {
		chp_filename = Db_GetValue(dbres, 0, 0);
		if (chp_filename) {
			in_max_attach = atoi(chp_filename);
		}
		Db_CloseDyna(dbres);
	}
	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	strcpy(cha_sql, "select T1.c_filename");	/* 0 */
	strcat(cha_sql, ", T1.n_entry_id");			/* 1 */
	strcat(cha_sql, " from at_uploadmovie T1");
	strcat(cha_sql, " where T1.b_delete_mode = 0");	/* ���ͽ�󤵤�Ƥ��ʤ� */
	/* ������°���Ƥ����Τȡ����ե�����ȤʤäƤ����Τ򥻥쥯�� */
	if (chp_entry_id || chp_tmpfile_id) {
		strcat(cha_sql, " and T1.n_entry_id in (");
		if (chp_entry_id) {
			strcat(cha_sql, chp_entry_id);
		}
		if (chp_entry_id && chp_tmpfile_id) {
			strcat(cha_sql, ", ");
		}
		if (chp_tmpfile_id) {
			strcat(cha_sql, chp_tmpfile_id);
		}
		strcat(cha_sql, ")");
	}
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������°���Ƥ���ư��ե���������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	/* �ե�����ꥹ�� */
	if (in_row != 0) {
		Put_Nlist(nlp_out, "ATTACHED_MOVIE", "<table>\n");
		for(i = 0; i < in_row; i++){
			chp_filename = Db_GetValue(dbres, i, 0);
			chp_entry_id = Db_GetValue(dbres, i, 1);
			if (chp_filename){
				Put_Nlist(nlp_out, "ATTACHED_MOVIE", "<tr><td>\n");
				//Build_Checkbox(nlp_out, "ATTACHED_MOVIE", "delete_movie", chp_entry_id, 0);
				Put_Nlist(nlp_out, "ATTACHED_MOVIE", "<object width=\"320\" height=\"240\">\n");
				Put_Nlist(nlp_out, "ATTACHED_MOVIE", "<param name=\"allowScriptAccess\" value=\"sameDomain\" />\n");
				if (g_in_cart_mode == CO_CART_RESERVE) {
					Put_Nlist(nlp_out, "ATTACHED_MOVIE", "<param name=\"movie\" value=\"/reserve/cgi-bin/asjplayer.swf\" />\n");
					Put_Format_Nlist(nlp_out, "ATTACHED_MOVIE", "<param name=\"FlashVars\" value=\"flvfile=/reserve/images/%s\" />\n", chp_filename);
				} else {
					Put_Nlist(nlp_out, "ATTACHED_MOVIE", "<param name=\"movie\" value=\"/e-commex/cgi-bin/asjplayer.swf\" />\n");
					Put_Format_Nlist(nlp_out, "ATTACHED_MOVIE", "<param name=\"FlashVars\" value=\"flvfile=/e-commex/images/upload/%s\" />\n", chp_filename);
				}
				Put_Nlist(nlp_out, "ATTACHED_MOVIE", "<param name=\"quality\" value=\"high\" />\n");
				if (g_in_cart_mode == CO_CART_RESERVE) {
					Put_Format_Nlist(nlp_out, "ATTACHED_MOVIE", "<embed src=\"/reserve/cgi-bin/asjplayer.swf\" FlashVars=\"flvfile=/reserve/images/%s\" quality=\"high\" width=\"300\" height=\"250\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" />\n", chp_filename);
				} else {
					Put_Format_Nlist(nlp_out, "ATTACHED_MOVIE", "<embed src=\"/e-commex/cgi-bin/asjplayer.swf\" FlashVars=\"flvfile=/e-commex/images/%s\" quality=\"high\" width=\"300\" height=\"250\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" />\n", chp_filename);
				}
				Put_Nlist(nlp_out, "ATTACHED_MOVIE", "</object>\n");
				Put_Nlist(nlp_out, "ATTACHED_MOVIE", "</td></tr>\n");
			}
		}
		Put_Nlist(nlp_out, "ATTACHED_MOVIE", "</table>\n");
		Build_Submit(nlp_out, "ATTACHED_MOVIE", "BTN_DELETE_MOVIE_ASK", "ư��ե��������");
		if (g_in_html_editor) {
			Put_Nlist(nlp_out, "ATTACHED_FILE", "&nbsp;\n");
			Build_Submit(nlp_out, "ATTACHED_FILE", "BTN_DISPLAY_MOVIE", "ư��ե������ɽ��");
		}
	}
	if (in_row < in_max_attach/*CO_MAX_ATTACHED_MOVIE*/) {
		/* ư�襢�åץ����� */
		Build_Input_File(nlp_out, "MOVIEBUTTON", "movie", "", 30, 255);
		Build_Submit(nlp_out, "MOVIEBUTTON", "BTN_UPLOAD_MOVIE", "ư��Υ��åץ���");
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_editentry()
 * Description:
 *	���������ڡ���������
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int disp_page_editentry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	struct tm *stp_now;
	DBRes *dbres;
	time_t tm_now;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char *chp_entry_id;
	char cha_sql[1024];
	char cha_team[128];
	char cha_battle[128];
	char cha_str[64];
	char cha_category_id[4];
	char cha_day[10];	/*yyyy/mm/dd*/
	char cha_time[5];	/*hh:mm*/
	int in_fromuser;
	int in_year;
	int in_month;
	int in_day;
	int in_mode;

	if(Get_Nlist(nlp_in, "ERROR", 1)) {
		Put_Nlist(nlp_out, "ERROR", Get_Nlist(nlp_in, "ERROR", 1));
	}
	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	if (chp_entry_id && *chp_entry_id) {
		if (Exist_Entry_Id(db, nlp_out, atoi(chp_entry_id), in_blog) == 0) {
			Put_Nlist(nlp_out, "ERROR", "̵���ʥ���ȥ꡼ID�Ǥ���");
			return 1;
		}
		/* ����ID������ */
		Build_HiddenEncode(nlp_out, "ENTRYID", "entry_id", chp_entry_id);
	}
	/* �����Խ���Ǥ��뤳�Ȥ򼨤�HIDDEN */
	sprintf(cha_str, "%d", CO_EDITMODE_EDIT);
	Build_HiddenEncode(nlp_out, "HIDDEN", "editmode", cha_str);
	/* ź�եե�����β���Ͽ���Ѥ���ե�����ID������ */
	/* �Ǥ���¤��դˤ��롣 */
	if(!Get_Nlist(nlp_in, "tmpfile_id", 1)) {
		tm_now = time(NULL);
		sprintf(cha_str, "%d", -((int)tm_now % INT_MAX) - 1);	/* ID��-1����-(INT_MAX)�ˤʤ� */
		Build_HiddenEncode(nlp_out, "HIDDEN", "tmpfile_id", cha_str);
	} else {
		Build_HiddenEncode(nlp_out, "HIDDEN", "tmpfile_id", Get_Nlist(nlp_in, "tmpfile_id", 1));
	}
	if (init_filetable(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	/* ź�եե�����ꥹ�� */
	if (put_attached_file_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
		/* ź��ư��ꥹ�� */
		if (put_attached_movie_list(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	}
	strcpy(cha_sql, "select T1.n_category_id");	/* 0 ���ƥ���ID*/
	strcat(cha_sql, ", T1.b_comment");			/* 1 �����Ȳ��� */
	strcat(cha_sql, ", T1.b_trackback");		/* 2 �ȥ�å��Хå����� */
	strcat(cha_sql, ", T1.b_mode");				/* 3 �����⡼�� */
	strcat(cha_sql, ", T1.c_entry_title");		/* 4 �����ȥ� */
	strcat(cha_sql, ", T1.c_entry_summary");	/* 5 ���� */
	strcat(cha_sql, ", T1.c_entry_body");		/* 6 ��ʸ */
	strcat(cha_sql, ", date_format(T1.d_entry_create_time,'%Y/%m/%d %H:%i')");	/* 7 �������� */
	strcat(cha_sql, ", T1.c_entry_more");		/* 8 ³�� */
	strcat(cha_sql, ", T1.c_item_id");			/* 9 ��Ϣ����ID */
	strcat(cha_sql, ", T1.c_item_kind");		/* 10 ��Ϣ���ʼ��� */
	strcat(cha_sql, ", T1.c_item_url");			/* 11 ��Ϣ����URL */
	strcat(cha_sql, ", T1.c_item_owner");		/* 12 ��Ϣ���ʥ����ʡ� */
	if (g_in_hb_mode) {
		strcat(cha_sql, ", date_format(T1.d_entry_delete_time,'%Y')");	/* 13 �����-ǯ */
		strcat(cha_sql, ", date_format(T1.d_entry_delete_time,'%m')");	/* 14 �����-�� */
		strcat(cha_sql, ", date_format(T1.d_entry_delete_time,'%d')");	/* 15 �����-�� */
	}
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where n_entry_id =");
	strcat(cha_sql, chp_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "��������ȥ꡼�����륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp && *chp_tmp) {
		strcpy(cha_category_id, chp_tmp);
	} else {
		cha_category_id[0] = '\0';
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		switch (atoi(chp_tmp)) {
		case 1:
			Put_Nlist(nlp_out, "COMMENT1", CO_CHECKED);
			break;
		case 2:
			Put_Nlist(nlp_out, "COMMENT2", CO_CHECKED);
			break;
		case 3:
			Put_Nlist(nlp_out, "COMMENT3", CO_CHECKED);
			break;
		default:
			Put_Nlist(nlp_out, "COMMENT0", CO_CHECKED);
		}
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		switch (atoi(chp_tmp)) {
		case 1:
			Put_Nlist(nlp_out, "TRACKBACK1", CO_CHECKED);
			break;
		case 2:
			Put_Nlist(nlp_out, "TRACKBACK2", CO_CHECKED);
			break;
		case 3:
			Put_Nlist(nlp_out, "TRACKBACK3", CO_CHECKED);
			break;
		default:
			Put_Nlist(nlp_out, "TRACKBACK0", CO_CHECKED);
		}
	}
	chp_tmp = Db_GetValue(dbres, 0, 3);
	if (chp_tmp && atoi(chp_tmp) == 1) {
		Put_Nlist(nlp_out, "MODE1", CO_CHECKED);
		in_mode = 1;
	} else {
		Put_Nlist(nlp_out, "MODE0", CO_CHECKED);
		in_mode = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 4);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "ENTRYTITLE", chp_escape);
		free(chp_escape);
	}
	chp_tmp = Db_GetValue(dbres, 0, 5);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "ENTRYSUMMARY", chp_escape);
		free(chp_escape);
	}
	chp_tmp = Db_GetValue(dbres, 0, 6);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "ENTRYBODY", chp_escape);
		free(chp_escape);
	}
	chp_tmp = Db_GetValue(dbres, 0, 7);
	if (chp_tmp && *chp_tmp) {
		chp_escape = strchr(chp_tmp, ' ');
		if (chp_escape) {
			++chp_escape;
			strcpy(cha_day, chp_tmp);
			strcpy(cha_time, chp_escape);
		} else {
			strcpy(cha_day, chp_tmp);
			cha_time[0] = '\0';
		}
	} else {
		cha_day[0] = '\0';
		cha_time[0] = '\0';
	}
	if (g_in_dbb_mode) {
		cha_team[0] = '\0';
		in_year = 0;
		in_month = 0;
		in_day = 0;
		chp_tmp = Db_GetValue(dbres, 0, 8);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "PREVIEW", chp_tmp);
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "ENTRYMORE", chp_escape);
			free(chp_escape);
			chp_escape = strtok(chp_tmp, "\n");
			while (chp_escape) {
				if (strstr(chp_escape, "colspan=\"7\"")) {
					char *chp_str;
					chp_str = strstr(chp_escape, "ǯ");
					if (chp_str) {
						chp_str -= 4;
						sscanf(chp_str, "%dǯ%d��%d��%s", &in_year, &in_month, &in_day, cha_sql);
					} else {
						chp_str = strstr(chp_escape, "<b>");
						if (chp_str) {
							chp_str += strlen("<b>");
							chp_tmp2 = strstr(chp_str, "</b>");
							if (chp_tmp2) {
								strncpy(cha_team, chp_str, chp_tmp2 - chp_str);
								cha_team[chp_tmp2 - chp_str] = '\0';
							} else {
								strcpy(cha_team, chp_str);
							}
						}
					}
					if (cha_team[0] && in_year && in_month && in_day) {
						break;
					}
				}
				chp_escape = strtok(NULL, "\n");
			}
		}
		/* ��ͭ��������� */
		cha_sql[0] = '\0';
		if (in_year && in_month && in_day) {
			sprintf(cha_sql, "%04d/%02d/%02d", in_year, in_month, in_day);
		}
		if (Build_Team_Combo(db, nlp_in, nlp_out, "TEAM", in_blog, "team", cha_team, cha_sql)) {
			return 1;
		}
		memset(cha_battle, '\0', sizeof(cha_battle));
		chp_tmp = Db_GetValue(dbres, 0, 9);
		if (chp_tmp && *chp_tmp) {
			Build_HiddenEncode(nlp_out, "HIDDEN", "b", chp_tmp);
			strcpy(cha_battle, chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, 0, 10);
		if (chp_tmp && *chp_tmp) {
			Build_HiddenEncode(nlp_out, "HIDDEN", "f", chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, 0, 11);
		if (chp_tmp && *chp_tmp) {
			Build_HiddenEncode(nlp_out, "HIDDEN", "l", chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, 0, 12);
		if (chp_tmp && *chp_tmp) {
			char *chp_fnc = Db_GetValue(dbres, 0, 10);
			if (chp_tmp && (chp_fnc[0] == 'c' || chp_fnc[0] == 'C')) {
				char cha_id[32];
				if (!Owner_To_Temp(db, nlp_out, atoi(chp_tmp), cha_id)) {
					Build_HiddenEncode(nlp_out, "HIDDEN", "o", cha_id);
				}
			} else {
				Build_HiddenEncode(nlp_out, "HIDDEN", "o", chp_tmp);
			}
		}
		chp_tmp = Get_DBB_Topic(db, nlp_out, in_blog, Db_GetValue(dbres, 0, 10), Db_GetValue(dbres, 0, 9), Db_GetValue(dbres, 0, 12), chp_tmp = Db_GetValue(dbres, 0, 11));
		if (chp_tmp) {
			Put_Nlist(nlp_out, "DBBTOPIC", chp_tmp);
			free(chp_tmp);
			/*�����å��ܥå���*/
			if(strlen(cha_battle) && In_Battle_Owner(db, nlp_out, g_in_login_owner, cha_battle)) {
				memset(cha_str, '\0', sizeof(cha_str));
				sprintf(cha_str, "&nbsp;&nbsp;<p style=\"font-size:12px;\"><input type=\"checkbox\" value=\"1\" name=\"INSERT_BATTLE_ORDER\"%s>��������Υ����������������롣</p>", Get_Nlist(nlp_in, "INSERT_BATTLE_ORDER", 1) ? " checked" : "");
				Put_Nlist(nlp_out, "DBBTOPIC", cha_str);
			}
		}
	} else if (g_in_cart_mode == CO_CART_RESERVE) {
		chp_tmp = Db_GetValue(dbres, 0, 9);
		if (Build_Reserve_Combo(db, nlp_out, "ITEM", "item_id", chp_tmp, in_blog, g_in_login_owner)) {
			return 1;
		}
	} else {
		chp_tmp = Db_GetValue(dbres, 0, 9);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "ITEM", chp_escape);
			free(chp_escape);
		}
	}
	stp_now = localtime(&tm_now);
	if (g_in_hb_mode) {
		sprintf(cha_str, "%04d/%02d/%02d", stp_now->tm_year + 1900, stp_now->tm_mon + 1, stp_now->tm_mday);
		in_day = Db_GetValue(dbres, 0, 13) && Db_GetValue(dbres, 0, 14) && Db_GetValue(dbres, 0, 15);
		Build_Checkbox(nlp_out, "DELDATE", "del_auto", "1", in_day);
		if (in_day) {
			sprintf(cha_str, "%04d/%02d/%02d", atoi(Db_GetValue(dbres, 0, 13)), atoi(Db_GetValue(dbres, 0, 14)), atoi(Db_GetValue(dbres, 0, 15)));
		}
		Build_Date(nlp_out, "DELDATE", "del_year", "del_month", "del_day", stp_now->tm_year + 1850, stp_now->tm_year + 1950, cha_str);
		Put_Nlist(nlp_out, "DELDATE", "�ˤʤä���������");
	}
	Db_CloseDyna(dbres);
	if (g_in_dbb_mode) {
		Build_Hidden(nlp_out, "HIDDEN", "entry_category", "0");
		if (Get_Nlist(nlp_in, "BTN_EASY_TRACKBACK", 1)) {
			Put_Nlist(nlp_out, "EASY_TRACKBACK", "1");
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_EASY_TRACKBACK", "1");
		} else {
			Put_Nlist(nlp_out, "EASY_TRACKBACK", "0");
		}
	} else {
		/* ���ƥ��ꥻ�쥯�ȥܥå��� */
		strcpy(cha_sql, "select T1.n_category_id");	/* ���ƥ���ID */
		strcat(cha_sql, ", T1.c_category_name");	/* ���ƥ���̾ */
		strcat(cha_sql, " from at_category T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		strcat(cha_sql, " order by T1.n_category_order");
		Build_Combobox(OldDBase(db), nlp_out, "CATEGORY", cha_sql, "entry_category", cha_category_id);
	}
	/* Date���� */
	if (g_in_dbb_mode && in_mode) {
		Put_Nlist(nlp_out, "DATE_START", "<!--");
		Put_Nlist(nlp_out, "DATE_END", "-->");
		Put_Nlist(nlp_out, "MODE_START", "<!--");
		Put_Nlist(nlp_out, "MODE_END", "-->");
		cha_day[4] = '\0';
		cha_day[7] = '\0';
		cha_day[10] = '\0';
		Build_Hidden(nlp_out, "HIDDEN", "year", cha_day);
		Build_Hidden(nlp_out, "HIDDEN", "month", cha_day + 5);
		Build_Hidden(nlp_out, "HIDDEN", "day", cha_day + 8);
		cha_time[2] = '\0';
		cha_time[5] = '\0';
		Build_Hidden(nlp_out, "HIDDEN", "hour", cha_time);
		Build_Hidden(nlp_out, "HIDDEN", "minute", cha_time + 3);
		Build_Hidden(nlp_out, "HIDDEN", "entry_mode", "1");
	} else {
		Build_Date(nlp_out, "DATE", "year", "month", "day", stp_now->tm_year + 1850, stp_now->tm_year + 1950, cha_day);
		Build_ComboTime(nlp_out, "DATE", "hour", "minute", 1, cha_time);
	}
	Put_Nlist(nlp_out, "TITLE", "��ƺѤߵ������Խ�");

	if (put_update_ping_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	if (put_related_link_list(db, nlp_out, in_blog, atoi(chp_entry_id))) {
		return 1;
	}
	if (put_send_trackback_list(db, nlp_in, nlp_out, in_blog, atoi(chp_entry_id))) {
		return 1;
	}
	Put_Format_Nlist(nlp_out, "COPYBTN",
		"<input type=\"image\" src=\"%s/copy.gif\" name=\"BTN_COPY_ENTRY\">��", g_cha_admin_image);
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	in_fromuser = chp_tmp ? atoi(chp_tmp) : 0;
	if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
		Put_Format_Nlist(nlp_out, "RESETBTN",
			"<input type=\"image\" src=\"%s/reset.gif\" name=\"BTN_DISP_NEWENTRY\">", g_cha_admin_image);
	} else {
		Put_Format_Nlist(nlp_out, "RESETBTN",
			"<a href=\"%s%s%s/%s?BTN_DISP_MODIFYENTRY=1&blogid=%d&entry_id=%s&from_user=%d\"><img src=\"%s/reset.gif\" border=\"0\" alt=\"�ꥻ�å�\"></a>",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi,  CO_CGI_ENTRY, in_blog, chp_entry_id, in_fromuser, g_cha_admin_image);
	}
	Put_Nlist(nlp_out, "BUTTON", "kettei.gif");
	in_fromuser = 0;
	chp_tmp = Get_Nlist(nlp_in, "page", 1);
	if (chp_tmp) {
		in_fromuser = atoi(chp_tmp);
	}
	Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_ENTRYLIST=1&blogid=%d&page=%d", CO_CGI_ENTRY_LIST, in_blog, in_fromuser);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		my_Http_Post_Get_Result_for_update_ping
 * Description:
 *		����Υۥ��Ȥ�http��POST����������ʹ���ping��
%* ------------------------------------------------------------------------
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
-* ------------------------------------------------------------------------*/
int my_Http_Post_Get_Result_for_update_ping(
	 const char* chp_host
	,const char* chp_path
	,const char* chp_params
	,char** chpp_result
)
{
	int err;
	int sd;
	int ret;
	char cha_buff[1025];
	char* chp_header;
	char* chp_last;
	char* chp_str;
	struct sockaddr_in sa;
	struct hostent *he;
	struct servent *service;
	FILE* fp;

	he = gethostbyname(chp_host);
	if (!he) {
		return errno;
	}
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		return errno;
	}
	memset(&sa, sizeof(sa), 0);
	sa.sin_family = AF_INET;
	service = getservbyname("http","tcp");
	if (service) {
		/* ����������ݡ����ֹ�򥳥ԡ� */
		sa.sin_port = service->s_port;
	} else {
		/* ���Ԥ�����80�֤˷���Ǥ� */
		sa.sin_port = htons(80);
	}
	memcpy(&sa.sin_addr, he->h_addr_list[0], he->h_length);
	ret = connect(sd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0) {
		err = errno;
		close(sd);
		return err;
	}
	/* ���̤�Http_Post_Get_Result�Ȱ㤦�ΤϤ������� */
	asprintf(&chp_header,
		 "POST %s HTTP/1.0\r\n"
		 "User-Agent: blog_system\r\n"
		 "Host: %s\r\n"
		 "Content-Type: text/xml\r\n"
		 "Content-Length: %d\r\n\r\n"
		 "%s\r\n"
		 , chp_path, chp_host, strlen(chp_params), chp_params);
	ret = write(sd, chp_header, strlen(chp_header));
	if (ret < strlen(chp_header)) {
		err = errno;
		free(chp_header);
		close(sd);
		return err;
	}
	free(chp_header);
	fp = fdopen(sd, "r");
	if (!fp) {
		err = errno;
		close(sd);
		return err;
	}
	chp_str = (char*)malloc(1);
	*chp_str = '\0';
	fgets(cha_buff, sizeof(cha_buff), fp);
	while(fgets(cha_buff, sizeof(cha_buff), fp)) {
		chp_last = chp_str;
		asprintf(&chp_str, "%s%s", chp_last, cha_buff);
		free(chp_last);
	}
	*chpp_result = chp_str;
	fclose(fp);
	close(sd);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_ping()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼��
-* ------------------------------------------------------------------------*/
int update_ping(
	 NLIST *nlp_in
	,NLIST *nlp_out
	,int in_new_entry_id
	,char *chp_url
	,int in_blog
)
{
	CP_NKF nkf;
	NKF_CODE cd;
	char *chp_tmp;
	char *chp_escape;
	char *chp_result;
	char *chp_params_out;
	char cha_host[2048];
	char cha_domain[2048];
	char cha_path[2048];
	char cha_params[8192];
	char cha_http[10];
	int in_ret;

	chp_result = NULL;
	memset(cha_host, '\0', sizeof(cha_host));
	memset(cha_path, '\0', sizeof(cha_path));
	if (strncmp(chp_url, "http://", strlen("http://")) == 0){
		sscanf(chp_url, "http://%[^/]%s", cha_host, cha_path);
	} else if (strncmp(chp_url, "https://", strlen("https://")) == 0) {
		sscanf(chp_url, "https://%[^/]%s", cha_host, cha_path);
	} else {
		/* ���⤷�ʤ� */
		return 0;
	}
	if (cha_host[0] == '\0' || cha_path[0] == '\0') {
		/* ���⤷�ʤ� */
		return 0;
	}
	/*�ɥᥤ��̾*/
	chp_tmp = getenv("SERVER_NAME");
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "������̾�μ����˼��Ԥ��ޤ�����");
		return 1;
	}
	strcpy(cha_domain, chp_tmp);
	/* ��ĥ����weblogUpdates.extendedPing */
	if (strcmp(cha_host, "ping.blo.gs") == 0){
		memset(cha_params, '\0', sizeof(cha_params));
		strcpy(cha_params, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		strcat(cha_params, "<methodCall>\n");
		strcat(cha_params, "<methodName>weblogUpdates.extendedPing</methodName>\n");
		strcat(cha_params, "<params>\n");
		chp_tmp = Get_Nlist(nlp_in, "blog_name", 1);
		/* �֥�̾��hidden����ޤäƤ��� */
		if (chp_tmp) {
			chp_escape = Remove_HtmlTags(chp_tmp);
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s</string></value></param>\n"
				, chp_escape);
			free(chp_escape);
		}
		if (g_in_dbb_mode) {
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%s/</string></value></param>\n"
				, g_cha_protocol, cha_domain, g_cha_base_location, g_cha_blog_temp);
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%s/?eid=%d</string></value></param>\n"
				, g_cha_protocol, cha_domain, g_cha_base_location, g_cha_blog_temp, in_new_entry_id);
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%s.rdf</string></value></param>\n"
				, cha_http, cha_domain, g_cha_rss_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%08d/</string></value></param>\n"
				, g_cha_protocol, cha_domain, g_cha_base_location, in_blog);
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%08d/?eid=%d</string></value></param>\n"
				, g_cha_protocol, cha_domain, g_cha_base_location, in_blog, in_new_entry_id);
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%08d.rdf</string></value></param>\n"
				, cha_http, cha_domain, g_cha_rss_location, in_blog);
		} else {
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%s?bid=%d</string></value></param>\n"
				, cha_http, cha_domain, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%s?bid=%d&eid=%d</string></value></param>\n"
				, cha_http, cha_domain, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, in_new_entry_id);
			sprintf(cha_params + strlen(cha_params), "<param><value><string>%s%s%s/%08d.rdf</string></value></param>\n"
				, cha_http, cha_domain, g_cha_rss_location, in_blog);
		}
		strcat(cha_params, "</params>\n");
		strcat(cha_params, "</methodCall>\n");
	/* �̾����weblogUpdates.ping */
	} else {
		memset(cha_params, '\0', sizeof(cha_params));
		strcpy(cha_params, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
		strcat(cha_params, "<methodCall>\n");
		strcat(cha_params, "<methodName>weblogUpdates.ping</methodName>\n");
		strcat(cha_params, "<params>\n");
		chp_tmp = Get_Nlist(nlp_in, "blog_name", 1);
		if (chp_tmp){
			chp_escape = Remove_HtmlTags(chp_tmp);
			sprintf(&cha_params[strlen(cha_params)], "<param><value><string>%s</string></value></param>\n", chp_escape);
			free(chp_escape);
		}
		if (g_in_dbb_mode) {
			sprintf(cha_params + strlen(cha_params), "<param><value>%s%s%s/%s/</value></param>\n"
				, g_cha_protocol, cha_domain, g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			sprintf(cha_params + strlen(cha_params), "<param><value>%s%s%s/%08d/</value></param>\n"
				, g_cha_protocol, cha_domain, g_cha_base_location, in_blog);
		} else {
			sprintf(cha_params + strlen(cha_params), "<param><value>%s%s%s/%s?bid=%d</value></param>\n"
				, g_cha_protocol, cha_domain, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
		strcat(cha_params, "</params>\n");
		strcat(cha_params, "</methodCall>\n");
	}

	errno = 0;
	/* �������Ѵ����ơ��ܤ��Ѥ� */
	nkf = Create_Nkf();
	Set_Input_String(nkf, cha_params);
	Set_Input_Code(nkf, NKF_EUC);
	Set_Output_Code(nkf, NKF_UTF8);
	cd = Convert_To_String(nkf, &chp_params_out);
	Destroy_Nkf_Engine(nkf);

	in_ret = my_Http_Post_Get_Result_for_update_ping(cha_host, cha_path, chp_params_out, &chp_result);
	if (in_ret){
		Put_Nlist(nlp_out, "ERROR", "����ping�ۥ��ȡ�");
		Put_Nlist(nlp_out, "ERROR", cha_host);
		Put_Nlist(nlp_out, "ERROR", "�פˤ������Ǥ��ޤ���Ǥ�����<br>");
	}
	free(chp_params_out);
	free(chp_result);
	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	register_uploadfile()
 * Description:
 *	����������Ͽ�������Խ�����ź�դ���ե�����ξ���ι�����
 *	���ͽ�󤵤�Ƥ���ե�����������롣
 *	���θ塢��ID����Ͽ����Ƥ���ե��������ID���ѹ����롣
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼��
-* ------------------------------------------------------------------------*/
int register_uploadfile(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,int in_entry_id
	,int in_blog
)
{
	char *chp_tmpfile_id;
	char cha_sql[2048];

	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	if (!chp_tmpfile_id || !*chp_tmpfile_id) {
		Put_Nlist(nlp_out, "ERROR", "�����ե�����ID������Ǥ��ޤ���Ǥ�����");
		return 1;
	}
	/* ���ͽ�󤵤�Ƥ���ե������������ */
	/* DB�夫���� */
	strcpy(cha_sql, "delete from at_uploadfile");
	strcat(cha_sql, " where b_delete_mode = 1");		/* ���ͽ�� */
	strcat(cha_sql, " and (n_entry_id = ");
	strcat(cha_sql, chp_tmpfile_id);		/* ź�եե�����ID */
	sprintf(cha_sql + strlen(cha_sql), " or n_entry_id = %d)", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "�����ե�����ν����˼��Ԥ��ޤ�����(1)<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	/* ��ID�򥨥�ȥ꡼ID�˿���ʤ���������������˵����˽�°���롣 */
	strcpy(cha_sql, "update at_uploadfile");
	sprintf(cha_sql + strlen(cha_sql), " set n_entry_id = %d", in_entry_id);
	strcat(cha_sql, ", b_delete_mode = 0");
	strcat(cha_sql, " where n_entry_id =");
	strcat(cha_sql, chp_tmpfile_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "�����ե��������Ͽ�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	register_uploadmovie()
 * Description:
 *	����������Ͽ�������Խ�����ź�դ���ư��ե�����ξ���ι�����
 *	���ͽ�󤵤�Ƥ���ư��ե�����������롣
 *	���θ塢��ID����Ͽ����Ƥ���ư��ե��������ID���ѹ����롣
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼��
-* ------------------------------------------------------------------------*/
int register_uploadmovie(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,int in_entry_id
	,int in_blog
)
{
	char *chp_tmpfile_id;
	char cha_sql[2048];

	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	if (!chp_tmpfile_id || !*chp_tmpfile_id) {
		Put_Nlist(nlp_out, "ERROR", "�����ե�����ID������Ǥ��ޤ���Ǥ�����");
		return 1;
	}
	/* ���ͽ�󤵤�Ƥ���ե������������ */
	/* DB�夫���� */
	strcpy(cha_sql, "delete from at_uploadmovie");
	strcat(cha_sql, " where b_delete_mode = 1");		/* ���ͽ�� */
	strcat(cha_sql, " and (n_entry_id = ");
	strcat(cha_sql, chp_tmpfile_id);		/* ź�եե�����ID */
	sprintf(cha_sql + strlen(cha_sql), " or n_entry_id = %d)", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "ư��ե�����ν����˼��Ԥ��ޤ�����(1)<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	/* ��ID�򥨥�ȥ꡼ID�˿���ʤ���������������˵����˽�°���롣 */
	strcpy(cha_sql, "update at_uploadmovie");
	sprintf(cha_sql + strlen(cha_sql), " set n_entry_id = %d", in_entry_id);
	strcat(cha_sql, ", b_delete_mode = 0");
	strcat(cha_sql, " where n_entry_id =");
	strcat(cha_sql, chp_tmpfile_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "ư��ե��������Ͽ�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		insert_new_entry()
 * Description:
 *	��������ȥ꡼�򥤥󥵡���
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼��
-* ------------------------------------------------------------------------*/
int insert_new_entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	char *chp_order;
	char *chp_replace;
	char *chp_escape;
	char cha_num[256];
	char cha_url[1024];
	char cha_sql[16384];
	bool b_order;
	int in_new_entry_id;
	int in_checked_ping;
	int in_rt;
	int in_mode;
	int in_point;
	int in_skip;
	int i;

	if (error_check(db, nlp_in, nlp_out)) {
		return 1;
	}
	if (g_in_dbb_mode) {
		chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
		if ((!chp_tmp || atoi(chp_tmp) != 0) && check_future(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	}

	sprintf(cha_sql, "select n_next_entry from at_profile where n_blog_id = %d for update", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_new_entry_id = atoi(chp_tmp);
	} else {
		in_new_entry_id = 1;
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
	in_rt = 0;
	in_mode = 0;
	if (!in_skip) {
		if (Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥�����󳫻Ϥ˼��Ԥ��ޤ�����<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return CO_ERROR;
		}
		strcpy(cha_sql, "insert into at_entry");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_entry_id");
		strcat(cha_sql, ",n_category_id");
		strcat(cha_sql, ",b_comment");
		strcat(cha_sql, ",b_trackback");
		strcat(cha_sql, ",b_mode");
		strcat(cha_sql, ",c_entry_title");
		strcat(cha_sql, ",c_entry_summary");
		strcat(cha_sql, ",c_entry_body");
		strcat(cha_sql, ",c_entry_more");
		strcat(cha_sql, ",d_entry_create_time");
		strcat(cha_sql, ",d_entry_real_time");
		strcat(cha_sql, ",d_entry_modify_time");
		strcat(cha_sql, ",c_item_id");
		if (g_in_dbb_mode) {
			strcat(cha_sql, ",c_item_kind");
			strcat(cha_sql, ",c_item_owner");
			strcat(cha_sql, ",c_item_url");
			strcat(cha_sql, ",n_point");
		} else if (g_in_hb_mode) {
			strcat(cha_sql, ",d_entry_delete_time");
		}
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d", in_blog, in_new_entry_id);	/* n_entry_id */
		strcat(cha_sql, ", ");
		chp_tmp = Get_Nlist(nlp_in, "entry_category", 1);
		if (chp_tmp && *chp_tmp) {
			sprintf(cha_sql + strlen(cha_sql), "%d", atoi(chp_tmp));	/* n_category */
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", ");
		chp_tmp = Get_Nlist(nlp_in, "comment", 1);
		sprintf(cha_sql + strlen(cha_sql), "%d", atoi(chp_tmp));	/* b_comment */
		strcat(cha_sql, ", ");
		chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
		sprintf(cha_sql + strlen(cha_sql), "%d", atoi(chp_tmp));	/* b_trackback */
		strcat(cha_sql, ", ");
		chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
		in_mode = chp_tmp ? atoi(chp_tmp) : 1;
		sprintf(cha_sql + strlen(cha_sql), "%d", in_mode);	/* b_mode */
		strcat(cha_sql, ", '");
		chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);	/* c_entry_title */
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', '");
		chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);	/* c_entry_summary */
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
		strcat(cha_sql, "', '");
		sprintf(cha_num, "%d", in_new_entry_id);
		chp_tmp3 = Get_Nlist(nlp_in, "entry_body", 1);	/* c_entry_body */
		chp_tmp2 = Remove_Invalid_Tags(chp_tmp3, nlp_out, 0);
		if (!chp_tmp2) {
			Rollback_Transact(db);
			return CO_ERROR;
		}
		in_point = 0;
		if (g_in_dbb_mode) {
#ifdef DEBUGTEAUX
			char *chp_string = malloc(strlen(chp_tmp3) + 1);
			strcpy(chp_string, chp_tmp3);
#else
			char *chp_string = malloc(strlen(chp_tmp2) + 1);
			strcpy(chp_string, chp_tmp2);
#endif
			in_point = Calc_Body_Point(db, nlp_out, in_blog, in_new_entry_id, 1, chp_string, NULL);
			free(chp_string);
			if (in_point == INT_MAX) {
				Rollback_Transact(db);
				return CO_ERROR;
			}
		}
		chp_tmp = Get_Nlist(nlp_in, "tmpfile_id", 1);
		if (chp_tmp) {
#ifdef DEBUGTEAUX
			chp_replace = Replace_Str(chp_tmp3, chp_tmp, cha_num);
#else
			chp_replace = Replace_Str(chp_tmp2, chp_tmp, cha_num);
#endif
			chp_escape = My_Escape_SqlString(db, chp_replace);
			free(chp_replace);
		} else {
			chp_escape = My_Escape_SqlString(db, chp_tmp2);
		}
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		free(chp_tmp2);
		strcat(cha_sql, "', '");
		b_order = 0;
		if (Get_Nlist(nlp_in, "INSERT_ORDER", 1)) {
			NLIST *nlp_tmp;
			chp_order = NULL;
			chp_escape = NULL;
			nlp_tmp = Init_Nlist();
			chp_tmp = Get_Nlist(nlp_in, "entry_more", 1);
			if (!chp_tmp || !chp_tmp[0]) {
				if (Disp_Team_Player(db, nlp_in, nlp_tmp, nlp_out, "TEMP", "team")) {
					Rollback_Transact(db);
					Finish_Nlist(nlp_tmp);
					return CO_ERROR;
				}
				chp_order = Page_Out_Mem(nlp_tmp, "blog_temp.skl");
			}
			if (chp_tmp && chp_tmp[0]) {
				chp_escape = My_Escape_SqlString(db, chp_tmp);	/* c_entry_more */
			} else if (chp_order) {
				chp_escape = My_Escape_SqlString(db, chp_order);	/* c_entry_more */
				free(chp_order);
			}
			if (chp_escape) {
				strcat(cha_sql, chp_escape);
				free(chp_escape);
				b_order = 1;
			}
			Finish_Nlist(nlp_tmp);
		}
		if (Get_Nlist(nlp_in, "INSERT_BATTLE_ORDER", 1)) {
			NLIST *nlp_tmp;
			chp_order = NULL;
			chp_escape = NULL;
			nlp_tmp = Init_Nlist();
//			chp_tmp = Get_Nlist(nlp_in, "entry_more", 1);
//			if (!chp_tmp || !chp_tmp[0]) {
				if (Disp_Battle_Player(db, nlp_in, nlp_tmp, nlp_out, "TEMP", "b", in_blog)) {
					Rollback_Transact(db);
					Finish_Nlist(nlp_tmp);
					return CO_ERROR;
				}
				chp_order = Page_Out_Mem(nlp_tmp, "blog_temp.skl");
//			}
//			if (chp_tmp && chp_tmp[0]) {
//				chp_escape = My_Escape_SqlString(db, chp_tmp);	/* c_entry_more */
//			} else if (chp_order) {
			if (chp_order) {
				chp_escape = My_Escape_SqlString(db, chp_order);	/* c_entry_more */
				free(chp_order);
			}
			if (chp_escape) {
				if(b_order) {
					strcat(cha_sql, "<br clear=\"all\"><br>");
				}
				strcat(cha_sql, chp_escape);
				free(chp_escape);
			}
			Finish_Nlist(nlp_tmp);
		}
		strcat(cha_sql, "'");
		/* �������ˤϡ�specify_date���ʤ� */
		chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
		if (!chp_tmp || atoi(chp_tmp) != 0) {
			/* d_entry_create_time */
			strcat(cha_sql, ", '");
			strcat(cha_sql, Get_Nlist(nlp_in, "year", 1));
			strcat(cha_sql, "-");
			strcat(cha_sql, Get_Nlist(nlp_in, "month", 1));
			strcat(cha_sql, "-");
			strcat(cha_sql, Get_Nlist(nlp_in, "day", 1));
			strcat(cha_sql, " ");
			strcat(cha_sql, Get_Nlist(nlp_in, "hour", 1));
			strcat(cha_sql, ":");
			strcat(cha_sql, Get_Nlist(nlp_in, "minute", 1));
			strcat(cha_sql, ":00'");
		} else {
			/* d_entry_create_time */
			strcat(cha_sql, ", now()");
		}
		/* d_entry_real_time */
		strcat(cha_sql, ", now()");
		/* d_entry_modify_time */
		strcat(cha_sql, ", now()");
		strcat(cha_sql, ", '");
		if (g_in_dbb_mode) {
			chp_tmp = Get_Nlist(nlp_in, "b", 1);
			if (chp_tmp && *chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			strcat(cha_sql, "','");
			chp_tmp = Get_Nlist(nlp_in, "f", 1);
			if (chp_tmp && *chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			strcat(cha_sql, "',");
			chp_tmp = Get_Nlist(nlp_in, "o", 1);
			if (chp_tmp && *chp_tmp) {
				in_rt = Temp_To_Owner(db, nlp_out, chp_tmp);
				if (in_rt > 0) {
					sprintf(cha_sql + strlen(cha_sql), "%d", in_rt);
				} else {
					strcat(cha_sql, "null");
				}
			} else {
				strcat(cha_sql, "null");
			}
			strcat(cha_sql, ",'");
			chp_tmp = Get_Nlist(nlp_in, "l", 1);
			if (chp_tmp && *chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			sprintf(cha_sql + strlen(cha_sql), "', %d)", in_point);
		} else if (g_in_hb_mode) {
			if (!Get_Nickname(db, nlp_out, cha_url, cha_num, NULL)) {
				Rollback_Transact(db);
				return CO_ERROR;
			}
			chp_escape = My_Escape_SqlString(db, cha_url);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
			chp_tmp = Get_Nlist(nlp_in, "del_auto", 1);
			if (chp_tmp && atoi(chp_tmp) && Get_Nlist(nlp_in, "del_year", 1) && Get_Nlist(nlp_in, "del_month", 1) && Get_Nlist(nlp_in, "del_day", 1)) {
				strcat(cha_sql, "','");
				sprintf(cha_sql + strlen(cha_sql), "%04d-%02d-%02d", atoi(Get_Nlist(nlp_in, "del_year", 1)), atoi(Get_Nlist(nlp_in, "del_month", 1)), atoi(Get_Nlist(nlp_in, "del_day", 1)));
				strcat(cha_sql, "')");
			} else {
				strcat(cha_sql, "',null)");
			}
		} else {
			chp_tmp = Get_Nlist(nlp_in, "item_id", 1);
			if (chp_tmp && *chp_tmp) {
				chp_escape = My_Escape_SqlString(db, chp_tmp);	/* c_entry_summary */
				strcat(cha_sql, chp_escape);
				free(chp_escape);
			}
			strcat(cha_sql, "')");
		}
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return CO_ERROR;
		}

		sprintf(cha_sql, "update at_profile set n_next_entry=n_next_entry+1 where n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return CO_ERROR;
		}

		/* uploadfile��ID������Ͽ */
		if (register_uploadfile(db, nlp_in, nlp_out,in_new_entry_id, in_blog)) {
			Rollback_Transact(db);
			return CO_ERROR;
		}
		/* uploadmovie��ID������Ͽ */
		if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
			if (register_uploadmovie(db, nlp_in, nlp_out,in_new_entry_id, in_blog)) {
				Rollback_Transact(db);
				return CO_ERROR;
			}
		}
		/* ���� */
		in_rt = 0;
		chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			/*�ȥ�å��Хå�PING����*/
			chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
			if (chp_tmp && strlen(chp_tmp) != 0){
				chp_tmp2 = strtok(chp_tmp, "\n");
				while (chp_tmp2){
					chp_escape = strchr(chp_tmp2, '\r');
					if (chp_escape)
						*chp_escape = '\0';
					if (trackback(db, nlp_in, nlp_out, in_new_entry_id, chp_tmp2, in_blog, 0, NULL)){
						++in_rt;
					}
					chp_tmp2 = strtok(NULL, "\n");
				}
			}
			/*��ϢURL��Ͽ*/
			chp_tmp = Get_Nlist(nlp_in, "related_url", 1);
			if (chp_tmp && strlen(chp_tmp) != 0){
				chp_tmp2 = strpbrk(chp_tmp, "\n");
				while (1) {
					if (!chp_tmp2) {
						if (strlen(chp_tmp)) {
							chp_tmp2 = chp_tmp + strlen(chp_tmp);
						} else {
							break;
						}
					}
					if (chp_tmp2 - chp_tmp < sizeof(cha_url)) {
						strncpy(cha_url, chp_tmp, chp_tmp2 - chp_tmp);
						cha_url[chp_tmp2 - chp_tmp] = '\0';
						chp_escape = strchr(cha_url, '\r');
						if (chp_escape)
							*chp_escape = '\0';
						if (insert_link(db, nlp_out, in_blog, in_new_entry_id, cha_url, 0, 0, 0)) {
							Rollback_Transact(db);
							return CO_ERROR;
						}
					}
					chp_tmp = chp_tmp2;
					++chp_tmp;
					if (*chp_tmp == '\r')
						++chp_tmp;
					chp_tmp2 = strpbrk(chp_tmp, "\n");
				}
			}
			/* ����PING����(�Ѱդ��줿���) */
			in_checked_ping = Get_NlistCount(nlp_in, "updateping");
			for(i = 0; i < in_checked_ping; i++) {
				chp_tmp = Get_Nlist(nlp_in, "updateping", i + 1);
				if (chp_tmp && *chp_tmp && update_ping(nlp_in, nlp_out, in_new_entry_id, chp_tmp, in_blog)) {
					++in_rt;
				}
			}
			/* ����PING(�ƥ����ȥ��ꥢ) */
			chp_tmp = Get_Nlist(nlp_in, "updateping_text", 1);
			if (chp_tmp && strlen(chp_tmp) != 0){
				chp_tmp2 = strtok(chp_tmp, "\r\n");
				while (chp_tmp2) {
					if (update_ping(nlp_in, nlp_out, in_new_entry_id, chp_tmp2, in_blog)) {
						++in_rt;
					}
					chp_tmp2 = strtok(NULL, "\n");
				}
			}
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "���ߥåȤ˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return CO_ERROR;
		}
	}

	if (g_in_cart_mode == CO_CART_SHOPPER && in_mode) {
		in_rt = Send_Blog_Update(db, nlp_in, nlp_out, in_blog);
	}

	return in_rt;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	modify_posted_entry()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
int modify_posted_entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	NLIST *nlp_tmp;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	char *chp_escape;
	char *chp_replace;
	char *chp_order;
	char cha_num[32];
	char cha_url[1024];
	char cha_sql[16384];
	bool b_order;
	int in_entry_id;
	int in_checked_ping;
	int in_point;
	int in_skip;
	int in_rt;
	int i;

	chp_tmp = Get_Nlist(nlp_in, "entry_id", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "����ID������Ǥ��ޤ���Ǥ�����<br>");
		return 1;
	}
	in_entry_id = atoi(chp_tmp);
	if (Exist_Entry_Id(db, nlp_out, in_entry_id, in_blog) == 0) {
		Put_Nlist(nlp_out, "ERROR", "���ε����Ϥ���ޤ���");
		return 1;
	}
	if (error_check(db, nlp_in, nlp_out)) {
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
			Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥������γ��Ϥ˼��Ԥ��ޤ���<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}

		strcpy(cha_sql, "update at_entry");
		strcat(cha_sql, " set n_category_id = ");
		chp_tmp = Get_Nlist(nlp_in, "entry_category", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", b_comment = ");
		chp_tmp = Get_Nlist(nlp_in, "comment", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", b_trackback = ");
		chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", b_mode = ");
		chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", c_entry_title = '");
		chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		strcat(cha_sql, chp_escape);
		free(chp_escape);

		strcat(cha_sql, "', c_entry_summary = '");
		chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
		if (chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}

		strcat(cha_sql, "', c_entry_body = '");
		sprintf(cha_num, "%d", in_entry_id);
		chp_tmp3 = Get_Nlist(nlp_in, "entry_body", 1);
		chp_tmp2 = Remove_Invalid_Tags(chp_tmp3, nlp_out, 0);
		if (!chp_tmp2) {
			Rollback_Transact(db);
			return 1;
		}
		in_point = 0;
		if (g_in_dbb_mode) {
#ifdef DEBUGTEAUX
			in_point = Calc_Body_Point(db, nlp_out, in_blog, in_entry_id, 0, chp_tmp3, NULL);
#else
			in_point = Calc_Body_Point(db, nlp_out, in_blog, in_entry_id, 0, chp_tmp2, NULL);
#endif
			if (in_point == INT_MAX) {
				Rollback_Transact(db);
				return 1;
			}
		}
		chp_tmp = Get_Nlist(nlp_in, "tmpfile_id", 1);
		if (chp_tmp) {
#ifdef DEBUGTEAUX
			chp_replace = Replace_Str(chp_tmp3, chp_tmp, cha_num);
#else
			chp_replace = Replace_Str(chp_tmp2, chp_tmp, cha_num);
#endif
			chp_escape = My_Escape_SqlString(db, chp_replace);
			free(chp_replace);
		} else {
			chp_escape = My_Escape_SqlString(db, chp_tmp2);
		}
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		free(chp_tmp2);

		b_order = 0;
		strcat(cha_sql, "', c_entry_more = '");
		chp_tmp = Get_Nlist(nlp_in, "entry_more", 1);
		chp_tmp2 = Get_Nlist(nlp_in, "tmpfile_id", 1);
		if (Get_Nlist(nlp_in, "INSERT_ORDER", 1) && chp_tmp) {
			if (chp_tmp2) {
				chp_replace = Replace_Str(chp_tmp, chp_tmp2, cha_num);
				chp_escape = My_Escape_SqlString(db, chp_replace);
				free(chp_replace);
			} else {
				chp_escape = My_Escape_SqlString(db, chp_tmp);
			}
			strcat(cha_sql, chp_escape);
			free(chp_escape);
			b_order = 1;
		}

		if (Get_Nlist(nlp_in, "INSERT_BATTLE_ORDER", 1)) {
			nlp_tmp = Init_Nlist();
			if (Disp_Battle_Player(db, nlp_in, nlp_tmp, nlp_out, "TEMP", "b", in_blog)) {
				Rollback_Transact(db);
				Finish_Nlist(nlp_tmp);
				return 1;
			}
			chp_order = Page_Out_Mem(nlp_tmp, "blog_temp.skl");
			if (chp_order) {
				chp_escape = My_Escape_SqlString(db, chp_order);
				free(chp_order);
				if(b_order) {
					strcat(cha_sql, "<br clear=\"all\"><br>");
				}
				strcat(cha_sql, chp_escape);
				free(chp_escape);
			}
			Finish_Nlist(nlp_tmp);
		}

		strcat(cha_sql, "', d_entry_create_time = '");
		strcat(cha_sql, Get_Nlist(nlp_in, "year", 1));
		strcat(cha_sql, "-");
		strcat(cha_sql, Get_Nlist(nlp_in, "month", 1));
		strcat(cha_sql, "-");
		strcat(cha_sql, Get_Nlist(nlp_in, "day", 1));
		strcat(cha_sql, " ");
		strcat(cha_sql, Get_Nlist(nlp_in, "hour", 1));
		strcat(cha_sql, ":");
		strcat(cha_sql, Get_Nlist(nlp_in, "minute", 1));
		strcat(cha_sql, ":00'");

		strcat(cha_sql, ", c_item_id = '");
		if (g_in_dbb_mode) {
			chp_tmp = Get_Nlist(nlp_in, "b", 1);
			if (chp_tmp && *chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			strcat(cha_sql, "', c_item_kind = '");
			chp_tmp = Get_Nlist(nlp_in, "f", 1);
			if (chp_tmp && *chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			strcat(cha_sql, "', c_item_owner = ");
			chp_tmp = Get_Nlist(nlp_in, "o", 1);
			if (chp_tmp && *chp_tmp) {
				in_rt = Temp_To_Owner(db, nlp_out, chp_tmp);
				if (in_rt > 0) {
					sprintf(cha_sql + strlen(cha_sql), "%d", in_rt);
				} else {
					strcat(cha_sql, "null");
				}
			} else {
				strcat(cha_sql, "null");
			}
			strcat(cha_sql, ", c_item_url = '");
			chp_tmp = Get_Nlist(nlp_in, "l", 1);
			if (chp_tmp && *chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			strcat(cha_sql, "'");
			sprintf(cha_sql + strlen(cha_sql), ", n_point = %d", in_point);
		} else if (g_in_hb_mode) {
			if (!Get_Nickname(db, nlp_out, cha_url, cha_num, NULL)) {
				Rollback_Transact(db);
				return CO_ERROR;
			}
			chp_escape = My_Escape_SqlString(db, cha_url);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
			chp_tmp = Get_Nlist(nlp_in, "del_auto", 1);
			if (chp_tmp && atoi(chp_tmp) && Get_Nlist(nlp_in, "del_year", 1) && Get_Nlist(nlp_in, "del_month", 1) && Get_Nlist(nlp_in, "del_day", 1)) {
				strcat(cha_sql, "', d_entry_delete_time='");
				sprintf(cha_sql + strlen(cha_sql), "%04d-%02d-%02d", atoi(Get_Nlist(nlp_in, "del_year", 1)), atoi(Get_Nlist(nlp_in, "del_month", 1)), atoi(Get_Nlist(nlp_in, "del_day", 1)));
				strcat(cha_sql, "'");
			} else {
				strcat(cha_sql, "', d_entry_delete_time=null");
			}
		} else {
			chp_tmp = Get_Nlist(nlp_in, "item_id", 1);
			if (chp_tmp && *chp_tmp) {
				chp_escape = My_Escape_SqlString(db, chp_tmp);
				strcat(cha_sql, chp_escape);
				free(chp_escape);
			}
			strcat(cha_sql, "'");
		}

		sprintf(cha_sql + strlen(cha_sql), " where n_entry_id = %d", in_entry_id);
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
		if (Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "���������꡼�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		/* uploadfile��ID������Ͽ */
		if (register_uploadfile(db, nlp_in, nlp_out,in_entry_id, in_blog)) {
			Rollback_Transact(db);
			return 1;
		}
		/* uploadmovie��ID������Ͽ */
		if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
			if (register_uploadmovie(db, nlp_in, nlp_out,in_entry_id, in_blog)) {
				Rollback_Transact(db);
				return CO_ERROR;
			}
		}
		/* �ȥ�å��Хå�PING������PING����(�������Τ�) */
		chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			/* �ȥ�å��Хå� */
			chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
			if (chp_tmp && strlen(chp_tmp) != 0) {
				chp_tmp2 = strtok(chp_tmp, "\n");
				while(chp_tmp2) {
					in_rt = trackback(db, nlp_in, nlp_out, in_entry_id, chp_tmp2, in_blog, 0, NULL);
					if (in_rt == 1){
						return 1;
					}
					chp_tmp2 = strtok(NULL, "\n");
				}
			}
			/*��ϢURL��Ͽ*/
			sprintf(cha_sql, "delete from at_sendlink where n_blog_id = %d and n_entry_id = %d and b_link_tb = 0", in_blog, in_entry_id);
			if (Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "���������꡼�˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				Rollback_Transact(db);
				return 1;
			}
			chp_tmp = Get_Nlist(nlp_in, "related_url", 1);
			if (chp_tmp && strlen(chp_tmp) != 0) {
				chp_tmp2 = strpbrk(chp_tmp, "\n");
				while (1) {
					if (!chp_tmp2) {
						if (strlen(chp_tmp)) {
							chp_tmp2 = chp_tmp + strlen(chp_tmp);
						} else {
							break;
						}
					}
					if (chp_tmp2 - chp_tmp < sizeof(cha_url)) {
						strncpy(cha_url, chp_tmp, chp_tmp2 - chp_tmp);
						cha_url[chp_tmp2 - chp_tmp] = '\0';
						chp_escape = strchr(cha_url, '\r');
						if (chp_escape)
							*chp_escape = '\0';
						if (insert_link(db, nlp_out, in_blog, in_entry_id, cha_url, 0, 0, 0)) {
							Rollback_Transact(db);
							return CO_ERROR;
						}
					}
					chp_tmp = chp_tmp2;
					++chp_tmp;
					if (*chp_tmp == '\r')
						++chp_tmp;
					chp_tmp2 = strpbrk(chp_tmp, "\n");
				}
			}
			/* ����PING(�����å��ܥå���) */
			in_checked_ping = Get_NlistCount(nlp_in, "updateping");
			for(i = 0; i < in_checked_ping; i++) {
				chp_tmp = Get_Nlist(nlp_in, "updateping", i + 1);
				if (chp_tmp && *chp_tmp) {
					if (update_ping(nlp_in, nlp_out, in_entry_id, chp_tmp, in_blog)) {
						Put_Nlist(nlp_out, "ERROR", "����PING�������˼��Ԥ��ޤ�����");
						Rollback_Transact(db);
						return 1;
					}
				}
			}
			/* ����PING(�ƥ����ȥ��ꥢ) */
			chp_tmp = Get_Nlist(nlp_in, "updateping_text", 1);
			if (chp_tmp && strlen(chp_tmp) != 0){
				chp_tmp2 = strtok(chp_tmp, "\r\n");
				while(chp_tmp2 && (strlen(chp_tmp2) > 5)){
					if (update_ping(nlp_in, nlp_out, in_entry_id, chp_tmp2, in_blog)){
						Put_Nlist(nlp_out, "ERROR", "����PING�������˼��Ԥ��ޤ�����");
						Rollback_Transact(db);
						return 1;
					}
					chp_tmp2 = strtok(NULL, "\n");
				}
			}
		}
		if (Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "���ߥåȤ˼��Ԥ��ޤ���<br>");
			Rollback_Transact(db);
			return 1;
		}
	}

	in_rt = 0;
//	if (g_in_cart_mode) {
//		in_rt = Send_Blog_Update(db, nlp_in, nlp_out, in_blog);
//	}
	return in_rt;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	re_disp_page_entry()
 * Description:
 *	�Ƽ���̤�����äƤ����Ȥ������衣��Ȥ����ޤäƤ����Τ���Ф���
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int re_disp_page_entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_more)
{
	struct tm *stp_now;
	char *chp_tmp;
	char *chp_entry_id;
	char *chp_escape;
	char cha_day[16];
	char cha_time[8];
	char cha_num[32];
	char cha_sql[1024];
	int in_fromuser;
	int in_editmode;
	int in_entrymode;
	time_t tm_now;

	if(Get_Nlist(nlp_in, "ERROR", 1)) {
		Put_Nlist(nlp_out, "ERR_START", "-->");
		Put_Nlist(nlp_out, "ERR_END", "<!--");
		Put_Nlist(nlp_out, "ERROR", Get_Nlist(nlp_in, "ERROR", 1));
	}
	/* PING������ꥹ�� */
	if (put_update_ping_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	/* ź�եե�����ꥹ�� */
	if (put_attached_file_list(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
		/* ź��ư��ꥹ�� */
		if (put_attached_movie_list(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	}
	in_editmode = CO_EDITMODE_NEW;
	chp_tmp = Get_Nlist(nlp_in, "editmode", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "editmode", chp_tmp);
		in_editmode = atoi(chp_tmp);
	}
	/* �Ƽ�ID���Ѥ� */
	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	if (chp_entry_id && *chp_entry_id) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "entry_id", chp_entry_id);
	}
	chp_tmp = Get_Nlist(nlp_in, "tmpfile_id", 1);
	if (chp_tmp && *chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "tmpfile_id", chp_tmp);
	}
	/* ���ƥ��ꥻ�쥯�ȥܥå��� */
	if (g_in_dbb_mode) {
		Build_Hidden(nlp_out, "HIDDEN", "CATEGORY", "0");
	} else {
		strcpy(cha_sql, "select T1.n_category_id");	/* 0 ���ƥ���Id */
		strcat(cha_sql, ", T1.c_category_name");	/* 1 ���ƥ���̾ */
		strcat(cha_sql, " from at_category T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		strcat(cha_sql, " order by T1.n_category_order");
		chp_tmp = Get_Nlist(nlp_in, "entry_category", 1);
		if (chp_tmp && *chp_tmp) {
			Build_Combobox(OldDBase(db), nlp_out, "CATEGORY", cha_sql, "entry_category", chp_tmp);
		} else {
			Build_Combobox(OldDBase(db), nlp_out, "CATEGORY", cha_sql, "entry_category", "");
		}
	}
	/* �ȥ�å��Хå����ղ��� */
	chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
	if (chp_tmp) {
		switch (atoi(chp_tmp)) {
		case 3: Put_Nlist(nlp_out, "TRACKBACK3", CO_CHECKED); break;
		case 2: Put_Nlist(nlp_out, "TRACKBACK2", CO_CHECKED); break;
		case 1: Put_Nlist(nlp_out, "TRACKBACK1", CO_CHECKED); break;
		default:
			Put_Nlist(nlp_out, "TRACKBACK0", CO_CHECKED);
		}
	}
	/* �����ȼ��ղ��� */
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (chp_tmp) {
		switch (atoi(chp_tmp)) {
		case 3: Put_Nlist(nlp_out, "COMMENT3", CO_CHECKED); break;
		case 2: Put_Nlist(nlp_out, "COMMENT2", CO_CHECKED); break;
		case 1: Put_Nlist(nlp_out, "COMMENT1", CO_CHECKED); break;
		default:
			Put_Nlist(nlp_out, "COMMENT0", CO_CHECKED);
		}
	}
	/* ��ƥ⡼�� */
	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	if (chp_tmp && atoi(chp_tmp) == 1) {
		Put_Nlist(nlp_out, "MODE1", CO_CHECKED);
		in_entrymode = 1;
	} else {
		Put_Nlist(nlp_out, "MODE0", CO_CHECKED);
		in_entrymode = 0;
	}
	/* �����ȥ� */
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "ENTRYTITLE", chp_escape);
		free(chp_escape);
	}
	/* ���� */
	chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "ENTRYSUMMARY", chp_escape);
		free(chp_escape);
	}
	/* ��ʸ���ɲ� */
	chp_tmp = Get_Nlist(nlp_in, "entry_body", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "ENTRYBODY", chp_escape);
		free(chp_escape);
	}
	/* ³�� */
	if (in_more) {
		chp_tmp = Get_Nlist(nlp_in, "entry_more", 1);
		if (chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "ENTRYMORE", chp_escape);
			free(chp_escape);
		}
	}
	/* ��Ϣ���� */
	if (g_in_cart_mode == CO_CART_SHOPPER) {
		chp_tmp = Get_Nlist(nlp_in, "item_id", 1);
		if (chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "ITEM", chp_escape);
			free(chp_escape);
		}
	} else if (g_in_cart_mode == CO_CART_RESERVE) {
		chp_tmp = Get_Nlist(nlp_in, "item_id", 1);
		if (Build_Reserve_Combo(db, nlp_out, "ITEM", "item_id", chp_tmp, in_blog, g_in_login_owner)) {
			return 1;
		}
	}
	/* ��ͭ��������� */
	if (g_in_dbb_mode) {
		char *chp_y;
		char *chp_m;
		char *chp_d;
		chp_y = Get_Nlist(nlp_in, "ORDYEAR", 1);
		chp_m = Get_Nlist(nlp_in, "ORDMONTH", 1);
		chp_d = Get_Nlist(nlp_in, "ORDDAY", 1);
		if (chp_y && chp_y[0] && chp_m && chp_m[0] && chp_d && chp_d[0]) {
			sprintf(cha_sql, "%s/%s/%s", chp_y, chp_m, chp_d);
		} else {
			cha_sql[0] = '\0';
		}
		if (Build_Team_Combo(db, nlp_in, nlp_out, "TEAM", in_blog, "team", NULL, cha_sql)) {
			return 1;
		}
	}
	/* ������ƻ�����ƻ��֤���ꤹ��饸���ܥ����Ф� */
	if (in_editmode == CO_EDITMODE_NEW) {
		/* ��������Υ��쥯�ȥܥå����ȡ������ȼ���饸���ܥ����Ф��� */
		chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
		if (chp_tmp && atoi(chp_tmp)) {
			Build_Radio_With_Id(nlp_out, "DATE", "specify_date", "0", 0, "specify0", "��¸������������������Ȥ���");
			Put_Nlist(nlp_out, "DATE", "<br>");
			Build_Radio_With_Id(nlp_out, "DATE", "specify_date", "1", 1, "specify1", "��������ꤹ��");
		} else {
			Build_Radio_With_Id(nlp_out, "DATE", "specify_date", "0", 1, "specify0", "��¸������������������Ȥ���");
			Put_Nlist(nlp_out, "DATE", "<br>");
			Build_Radio_With_Id(nlp_out, "DATE", "specify_date", "1", 0, "specify1", "��������ꤹ��");
		}
		Put_Nlist(nlp_out, "BUTTON", "toukou.gif");
	} else {
		Put_Nlist(nlp_out, "BUTTON", "kettei.gif");
	}
	time(&tm_now);
	stp_now = localtime(&tm_now);
	if (in_editmode == CO_EDITMODE_NEW || !in_entrymode || !g_in_dbb_mode) {
		/* ǯ */
		chp_tmp = Get_Nlist(nlp_in, "year", 1);
		if (chp_tmp && *chp_tmp) {
			strcpy(cha_day, chp_tmp);
		} else {
			sprintf(cha_day, "%d", stp_now->tm_year + 1900);
		}
		strcat(cha_day, "/");
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "month", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_day, chp_tmp);
		} else {
			sprintf(cha_day + strlen(cha_day), "%02d", stp_now->tm_mon + 1);
		}
		strcat(cha_day, "/");
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "day", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_day, chp_tmp);
		} else {
			sprintf(cha_day + strlen(cha_day), "%02d", stp_now->tm_mday);
		}
		Build_Date(nlp_out, "DATE", "year", "month", "day", stp_now->tm_year + 1850, stp_now->tm_year + 1950, cha_day);
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "hour", 1);
		if (chp_tmp) {
			strcpy(cha_time, chp_tmp);
		} else {
			sprintf(cha_time, "%02d", stp_now->tm_hour);
		}
		strcat(cha_time, ":");
		/* ʬ */
		chp_tmp = Get_Nlist(nlp_in, "minute", 1);
		if (chp_tmp) {
			strcat(cha_time, chp_tmp);
		} else {
			sprintf(cha_time + strlen(cha_time), "%02d", stp_now->tm_min);
		}
		Build_ComboTime(nlp_out, "DATE", "hour", "minute", 1, cha_time);
	} else {
		Put_Nlist(nlp_out, "DATE_START", "<!--");
		Put_Nlist(nlp_out, "DATE_END", "-->");
		Put_Nlist(nlp_out, "MODE_START", "<!--");
		Put_Nlist(nlp_out, "MODE_END", "-->");
		Build_Hidden(nlp_out, "HIDDEN", "entry_mode", "1");
		/* ǯ */
		chp_tmp = Get_Nlist(nlp_in, "year", 1);
		if (chp_tmp && *chp_tmp) {
			strcpy(cha_day, chp_tmp);
		} else {
			sprintf(cha_day, "%d", stp_now->tm_year + 1900);
		}
		Build_HiddenEncode(nlp_out, "HIDDEN", "year", cha_day);
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "month", 1);
		if (chp_tmp && *chp_tmp) {
			strcpy(cha_day, chp_tmp);
		} else {
			sprintf(cha_day, "%02d", stp_now->tm_mon + 1);
		}
		Build_HiddenEncode(nlp_out, "HIDDEN", "month", cha_day);
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "day", 1);
		if (chp_tmp && *chp_tmp) {
			strcpy(cha_day, chp_tmp);
		} else {
			sprintf(cha_day, "%02d", stp_now->tm_mday);
		}
		Build_HiddenEncode(nlp_out, "HIDDEN", "day", cha_day);
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "hour", 1);
		if (chp_tmp) {
			strcpy(cha_time, chp_tmp);
		} else {
			sprintf(cha_time, "%02d", stp_now->tm_hour);
		}
		Build_HiddenEncode(nlp_out, "HIDDEN", "hour", cha_time);
		/* ʬ */
		chp_tmp = Get_Nlist(nlp_in, "minute", 1);
		if (chp_tmp) {
			strcpy(cha_time, chp_tmp);
		} else {
			sprintf(cha_time, "%02d", stp_now->tm_min);
		}
		Build_HiddenEncode(nlp_out, "HIDDEN", "minute", cha_time);
	}

	if (g_in_hb_mode) {
		/* ǯ */
		chp_tmp = Get_Nlist(nlp_in, "del_year", 1);
		if (chp_tmp && *chp_tmp) {
			strcpy(cha_day, chp_tmp);
		} else {
			sprintf(cha_day, "%d", stp_now->tm_year + 1900);
		}
		strcat(cha_day, "/");
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "month", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_day, chp_tmp);
		} else {
			sprintf(cha_day + strlen(cha_day), "%02d", stp_now->tm_mon + 1);
		}
		strcat(cha_day, "/");
		/* �� */
		chp_tmp = Get_Nlist(nlp_in, "day", 1);
		if (chp_tmp && *chp_tmp) {
			strcat(cha_day, chp_tmp);
		} else {
			sprintf(cha_day + strlen(cha_day), "%02d", stp_now->tm_mday);
		}
		chp_tmp = Get_Nlist(nlp_in, "del_auto", 1);
		Build_Checkbox(nlp_out, "DELDATE", "del_auto", "1", chp_tmp && atoi(chp_tmp) > 0);
		Build_Date(nlp_out, "DELDATE", "del_year", "del_month", "del_day", stp_now->tm_year + 1850, stp_now->tm_year + 1950, cha_day);
		Put_Nlist(nlp_out, "DELDATE", "�ˤʤä���������");
	}

	/* �ȥ�å��Хå��򤫤���URL */
	chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "TRACKBACKURL", chp_tmp);
	}
	/* ��ϢURL */
	chp_tmp = Get_Nlist(nlp_in, "related_url", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "RERATEDURL", chp_tmp);
	}
	/* DBB �ȥԥå� */
	if (g_in_dbb_mode) {
		cha_num[0] = '\0';
		chp_tmp = Get_Nlist(nlp_in, "o", 1);
		if (chp_tmp) {
			strcpy(cha_num, chp_tmp);
			chp_tmp = Get_Nlist(nlp_in, "f", 1);
			if (chp_tmp[0] == 'c' || chp_tmp[0] == 'C') {
				int in_tmp;
				chp_tmp = Get_Nlist(nlp_in, "o", 1);
				in_tmp = Temp_To_Owner(db, nlp_out, chp_tmp);
				if (in_tmp >= 0) {
					sprintf(cha_num, "%d", in_tmp);
				}
			}
		}
		chp_tmp = Get_Nlist(nlp_in, "b", 1);
		if(chp_tmp) {
			Build_Hidden(nlp_out, "HIDDEN", "b", chp_tmp);
		}
		chp_tmp = Get_Nlist(nlp_in, "f", 1);
		if(chp_tmp && strcmp(chp_tmp, "B") == 0) {
			Build_Hidden(nlp_out, "HIDDEN", "f", chp_tmp);
		}
		chp_tmp = Get_DBB_Topic(db, nlp_out, in_blog, Get_Nlist(nlp_in, "f", 1), Get_Nlist(nlp_in, "b", 1), cha_num, Get_Nlist(nlp_in, "l", 1));
		if (chp_tmp) {
			Put_Nlist(nlp_out, "DBBTOPIC", chp_tmp);
			free(chp_tmp);
			/*�����å��ܥå���*/
			if(Get_Nlist(nlp_in, "b", 1) && In_Battle_Owner(db, nlp_out, g_in_login_owner, Get_Nlist(nlp_in, "b", 1))) {
				memset(cha_sql, '\0', sizeof(cha_sql));
				sprintf(cha_sql, "&nbsp;&nbsp;<p style=\"font-size:12px;\"><input type=\"checkbox\" value=\"1\" name=\"INSERT_BATTLE_ORDER\"%s>��������Υ����������������롣</p>", Get_Nlist(nlp_in, "INSERT_BATTLE_ORDER", 1) ? " checked" : "");
				Put_Nlist(nlp_out, "DBBTOPIC", cha_sql);
			}
		}
		if (Get_Nlist(nlp_in, "BTN_EASY_TRACKBACK", 1)) {
			Put_Nlist(nlp_out, "EASY_TRACKBACK", "1");
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_EASY_TRACKBACK", "1");
		} else {
			Put_Nlist(nlp_out, "EASY_TRACKBACK", "0");
		}
	}
	if (in_editmode == CO_EDITMODE_EDIT) {
		Put_Nlist(nlp_out, "TITLE", "��ƺѤߵ������Խ�");
	} else {
		Put_Nlist(nlp_out, "TITLE", "�������");
	}
	if (chp_entry_id && put_send_trackback_list(db, nlp_in, nlp_out, in_blog, atoi(chp_entry_id))) {
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
	in_fromuser = chp_tmp ? atoi(chp_tmp) : 0;
	if(in_editmode == CO_EDITMODE_NEW) {
		if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
			Put_Format_Nlist(nlp_out, "RESETBTN",
							 "<input type=\"image\" src=\"%s/reset.gif\" name=\"BTN_DISP_NEWENTRY\">", g_cha_admin_image);
		} else {
			Put_Format_Nlist(nlp_out, "RESETBTN",
							 "<a href=\"%s%s%s/%s?BTN_DISP_NEWENTRY=1&blogid=%d&from_user=%d\"><img src=\"%s/reset.gif\" border=\"0\" alt=\"�ꥻ�å�\"></a>",
							 g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi,  CO_CGI_ENTRY, in_blog, in_fromuser, g_cha_admin_image);
		}
	} else {
		if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
			Put_Format_Nlist(nlp_out, "RESETBTN",
							 "<input type=\"image\" src=\"%s/reset.gif\" name=\"BTN_DISP_NEWENTRY\">", g_cha_admin_image);
		} else {
			Put_Format_Nlist(nlp_out, "RESETBTN",
							 "<a href=\"%s%s%s/%s?BTN_DISP_MODIFYENTRY=1&blogid=%d&entry_id=%s&from_user=%d\"><img src=\"%s/reset.gif\" border=\"0\" alt=\"�ꥻ�å�\"></a>",
							 g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi,  CO_CGI_ENTRY, in_blog, chp_entry_id, in_fromuser, g_cha_admin_image);
		}
	}
	if (in_editmode == CO_EDITMODE_NEW) {
		if (in_fromuser) {
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
			} else {
				Put_Format_Nlist(nlp_out, "PREVCGI", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
			}
		} else {
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s?blogid=%d", CO_CGI_MENU, in_blog);
		}
	} else {
		int in_page = 0;
		Put_Format_Nlist(nlp_out, "COPYBTN",
			"<input type=\"image\" src=\"%s/copy.gif\" name=\"BTN_COPY_ENTRY\">��", g_cha_admin_image);
		chp_tmp = Get_Nlist(nlp_in, "page", 1);
		if (chp_tmp) {
			in_page = atoi(chp_tmp);
		}
		Put_Format_Nlist(nlp_out, "PREVCGI", "%s?BTN_DISP_ENTRYLIST=1&blogid=%d", CO_CGI_ENTRY_LIST, in_blog, in_page);
	}
	return 0;
}

//�ؿ�����
int ImageInfoPNG(FILE* fp, int *Width, int *Height)
{
	unsigned char buf[26];
	unsigned char sig[9]={0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A,0};//�УΣǤμ��̥إå�

	bzero(buf, sizeof(buf));
	fread(buf, 25, 1, fp);
	if(strncmp((char*)buf,(char *)sig,8) != 0) {
		return 0;
	}

	//���������֤�������Ǥ��ʤΤǤ��ʤꤤ���ø�������ư���Ƥ�
	*Width=buf[16]*0x1000000+buf[17]*0x10000+buf[18]*0x100+buf[19];
	*Height=buf[20]*0x1000000+buf[21]*0x10000+buf[22]*0x100+buf[23];

	return 1;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	temporary_register_uploadfile()
 * Description:
 *	�ե�����Υ��åץ��ɻ��ν�����
 *	�ե�����򥢥åץ��ɸ塢�ե�������͡��ࡣ
 *	���θ塢ź�եե�����ơ��֥�˲���Ͽ��
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
int temporary_register_uploadfile(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	FILE* fp;
	gdImagePtr gd;
	struct stat sb;
	char *chp_tmp;
	char *chp_tmpfile_id;
	char *chp_escape;
	char *chp_fileimage;
	char cha_fullpath[1024];
	char cha_error[512];
	char cha_sql[512];
	int in_x, in_y;
	double dbl_x, dbl_y, dbl_ratio;
	int in_uploadfile_id;
	int in_fromuser;
	int in_max_attach;
	int in_skip;

	/* ź�եե�����ID�����뤳�Ȥ��ǧ */
	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	if (!chp_tmpfile_id || !*chp_tmpfile_id) {
		Put_Nlist(nlp_out, "ERROR", "�����ե�����ID�������Ǥ��ޤ���");
	}
	/* �ե�����Υ��顼�����å� */
	chp_tmp = Get_Nlist(nlp_in, "file", 1);
	if (!chp_tmp) {
		sprintf(cha_error, "�ե����륵������%dkB�ʲ��Ǥ��ꤤ�פ��ޤ���\n", CO_MAX_UPLOAD_SIZE);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		return 1;
	} else if (strlen(chp_tmp) == 0) {
		Put_Nlist(nlp_out, "ERROR", "�ե����뤬���Ϥ���Ƥ��ޤ���");
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "file", 2);
	if (!chp_tmp || (chp_tmp && strlen(chp_tmp) == 0)){
		Put_Nlist(nlp_out, "ERROR", "�ե����륿���פ�̵���Ǥ���");
		remove(Get_Nlist(nlp_in, "file", 1));
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "file", 3);
	if (!chp_tmp || (chp_tmp && strlen(chp_tmp) == 0)){
		remove(Get_Nlist(nlp_in, "file", 1));
		Put_Nlist(nlp_out, "ERROR", "�ե����뤬���Ĥ���ʤ������⤷���϶��Υե����뤬���ꤵ��Ƥ��ޤ���");
		return 1;
	}
	in_max_attach = CO_MAX_ATTACHED_FILE;
	dbres = Db_OpenDyna(db, "select n_max_attach from sy_baseinfo");
	if (dbres) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_max_attach = atoi(chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	/* ź�եե������CO_MAX_ATTACHED_FILE�ޤ� */
	strcpy(cha_sql, "select count(*)");	/* 0 ź�եե�����Ŀ� */
	strcat(cha_sql, " from at_uploadfile T1");
	strcat(cha_sql, " where T1.n_entry_id in (");
	strcat(cha_sql, chp_tmpfile_id);
	chp_tmp = Get_Nlist(nlp_in, "entry_id", 1);
	if (chp_tmp && *chp_tmp) {
		strcat(cha_sql, ", ");
		strcat(cha_sql, chp_tmp);
	}
	strcat(cha_sql, ")");
	strcat(cha_sql, " and T1.b_delete_mode = 0");	/* ���ͽ��Ǥʤ� */
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ե�����������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp && *chp_tmp && atoi(chp_tmp) >= in_max_attach/*CO_MAX_ATTACHED_FILE*/) {
		sprintf(cha_error, "��Ĥε�����ź�դǤ�������ե������ %d�� �ޤǤǤ���", in_max_attach/*CO_MAX_ATTACHED_FILE*/);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		Db_CloseDyna(dbres);
		return 1;
	}
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "file", 1);
	if (stat(chp_tmp, &sb) < 0) {
		remove(chp_tmp);
		Put_Nlist(nlp_out, "ERROR", "���åץ��ɤ����ե�����ξ�������˼��Ԥ��ޤ�����<br>");
		return 1;
	}
	chp_escape = Get_Nlist(nlp_in, "file", 2);
	if (!chp_escape || (!g_in_hb_mode && strncasecmp(chp_escape, "image/", 6) != 0)) {
		remove(chp_tmp);
		Put_Nlist(nlp_out, "ERROR", "���åץ��ɤǤ���ե�����ϲ����ե���������Ǥ���<br>");
		return 1;
	}
	fp = fopen(chp_tmp, "r");
	if (!fp) {
		Put_Format_Nlist(nlp_out, "ERROR", "���åץ��ɤ����ե�����Υ����ץ�˼��Ԥ��ޤ�����<br>%s<br>%s<br>", strerror(errno), chp_tmp);
		remove(chp_tmp);
		return 1;
	}
	in_x = 160;
	in_y = 120;
	gd = gdImageCreateFromGif(fp);
	if (!gd) {
		fseek(fp, 0, SEEK_SET);
		gd = gdImageCreateFromJpeg(fp);
//		if (!gd) {
//			fseek(fp, 0, SEEK_SET);
//			gd = gdImageCreateFromPng(fp);
//		}
	}
	if (gd) {
		in_x = gdImageSX(gd);
		in_y = gdImageSY(gd);
		gdImageDestroy(gd);
	} else {
		fseek(fp, 0, SEEK_SET);
		ImageInfoPNG(fp, &in_x, &in_y);
	}
	chp_tmp = malloc(sb.st_size);
	fseek(fp, 0, SEEK_SET);
	fread(chp_tmp, sb.st_size, 1, fp);
	fclose(fp);
	remove(Get_Nlist(nlp_in, "file", 1));
	chp_fileimage = malloc(sb.st_size * 3);
	bzero(chp_fileimage, sb.st_size * 3);
	My_Db_EscapeString(db, chp_fileimage, chp_tmp, sb.st_size);
	free(chp_tmp);

	if (Begin_Transact(db)) {
		free(chp_fileimage);
		Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥�����󳫻Ϥ˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}

	sprintf(cha_sql, "select coalesce(max(T1.n_uploadfile_id), 0) + 1 from at_uploadfile T1 where T1.n_blog_id=%d and T1.n_entry_id=%d for update", in_blog, atoi(chp_tmpfile_id));
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		free(chp_fileimage);
		Put_Nlist(nlp_out, "ERROR", "�ե������ֹ���ꥯ����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_uploadfile_id = atoi(chp_tmp);
	} else {
		in_uploadfile_id = 1;
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
		chp_escape = Escape_HtmlString(Get_Nlist(nlp_in, "file", 3));
		strcpy(cha_sql, "insert into at_uploadfile");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_uploadfile_id");
		strcat(cha_sql, ",c_filename");
		strcat(cha_sql, ",c_filetype");
		strcat(cha_sql, ",n_filesize");
		strcat(cha_sql, ",n_entry_id");
		strcat(cha_sql, ",b_delete_mode");
		strcat(cha_sql, ",n_imagesize_x");
		strcat(cha_sql, ",n_imagesize_y");
		sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d", in_blog, in_uploadfile_id);
		strcat(cha_sql, ", '");
		strcat(cha_sql, chp_escape);
		strcat(cha_sql, "', '");
		strcat(cha_sql, Get_Nlist(nlp_in, "file", 2));
		strcat(cha_sql, "', ");
		sprintf(cha_sql + strlen(cha_sql), "%d", (int)sb.st_size);
		strcat(cha_sql, ", ");
		strcat(cha_sql, chp_tmpfile_id);	/* ��ID */
		strcat(cha_sql, ", 0");				/* ����⡼�ɤ�0 */
		sprintf(cha_sql + strlen(cha_sql), ", %d, %d", in_x, in_y);
		strcat(cha_sql, ")");
		free(chp_escape);
		if (Db_ExecSql(db, cha_sql)){
			free(chp_fileimage);
			remove(Get_Nlist(nlp_in, "file", 1));
			remove(cha_fullpath);
			Put_Nlist(nlp_out, "ERROR", "�ե����륢�åץ��ɲ���Ͽ�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}

		asprintf(&chp_tmp,
			"update at_uploadfile"
			" set c_fileimage = '%s'"
			" where n_uploadfile_id = %d"
			" and n_entry_id = %s"
			" and n_blog_id = %d",
			chp_fileimage, in_uploadfile_id, chp_tmpfile_id, in_blog);
		if (Db_ExecSql(db, chp_tmp)){
			free(chp_fileimage);
			free(chp_tmp);
			Put_Nlist(nlp_out, "ERROR", "�ե����륢�åץ��ɲ���Ͽ�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		free(chp_tmp);
		free(chp_fileimage);
	}
	if (Commit_Transact(db)) {
		remove(Get_Nlist(nlp_in, "file", 1));
		remove(cha_fullpath);
		Put_Nlist(nlp_out, "ERROR", "�ե����륢�åץ��ɲ���Ͽ�Υ��ߥåȤ˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}

	if (g_in_html_editor) {
		if (strstr(Get_Nlist(nlp_in, "file", 2), "image/")) {
			dbl_x = (double)in_x;
			dbl_y = (double)in_y;
			dbl_ratio = dbl_y / dbl_x;
			while (dbl_x * dbl_y > 160.0 * 120.0) {
				dbl_x -= 1.0;
				dbl_y = dbl_ratio * dbl_x;
			}
			chp_tmp = Get_Nlist(nlp_in, "from_user", 1);
			in_fromuser = chp_tmp ? atoi(chp_tmp) : 0;
			if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
				Put_Format_Nlist(nlp_out, "RESETBTN",
					"<input type=\"image\" src=\"%s/reset.gif\" name=\"BTN_DISP_NEWENTRY\">", g_cha_admin_image);
			} else {
				Put_Format_Nlist(nlp_out, "RESETBTN",
					"<a href=\"%s%s%s/%s?BTN_DISP_NEWENTRY=1&blogid=%d&from_user=%d%s\"><img src=\"%s/reset.gif\" border=\"0\" alt=\"�ꥻ�å�\"></a>",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi,  CO_CGI_ENTRY, in_blog, in_fromuser,
					Get_Nlist(nlp_in, "BTN_EASY_TRACKBACK", 1) ? "&BTN_EASY_TRACKBACK=1" : "", g_cha_admin_image);
			}
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "INSERTIMG",
					"<a href=\"%s%s%s/%s/%s-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">"
					"<img src=\"%s%s%s/%s/%s-%d-%d?BTN_DISP_ATTACH_FILE=1\" width=\"%d\" height=\"%d\" border=\"0\">"
					"</a><br />",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, atoi(chp_tmpfile_id), in_uploadfile_id,
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, atoi(chp_tmpfile_id), in_uploadfile_id,
					(int)dbl_x, (int)dbl_y);
			} else {
				Put_Format_Nlist(nlp_out, "INSERTIMG",
					"<a href=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">"
					"<img src=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" width=\"%d\" height=\"%d\" border=\"0\">"
					"</a><br />",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, atoi(chp_tmpfile_id), in_uploadfile_id,
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, atoi(chp_tmpfile_id), in_uploadfile_id,
					(int)dbl_x, (int)dbl_y);
			}
		} else if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "INSERTIMG",
				"<a href=\"%s%s%s/%s/%s-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">%s</a><br />",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, atoi(chp_tmpfile_id), in_uploadfile_id, chp_escape);
		} else {
			Put_Format_Nlist(nlp_out, "INSERTIMG",
				"<a href=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">%s</a><br />",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, atoi(chp_tmpfile_id), in_uploadfile_id, chp_escape);
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_hidden_all_print
 * Description:
 *		
%* ------------------------------------------------------------------------
 * Return:
 *		0: ���ｪλ
 *		1: ���顼
-* ----------------------------------------------------------------------*/
void build_hidden_all_print(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chp_value;
	NLIST *nlp_cur;
	NLIST *nlp_cur2;
	int i;

	for(i = 0; i < CO_MAX_SLOT; i++) {
		for(nlp_cur = nlp_in + i; nlp_cur->nlp_next; ) {
			nlp_cur = nlp_cur->nlp_next;
			if(strcasecmp("movie", nlp_cur->chp_name) && strcasecmp("BTN_UPLOAD_MOVIE", nlp_cur->chp_name)) {
				chp_value = Escape_HtmlString(nlp_cur->chp_value);
				printf("<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", nlp_cur->chp_name, chp_value);
				free(chp_value);
			}
			for(nlp_cur2 = nlp_cur; nlp_cur2->nlp_name_next; ) {
				nlp_cur2 = nlp_cur2->nlp_name_next;
				if(strcasecmp("movie", nlp_cur2->chp_name) && strcasecmp("BTN_UPLOAD_MOVIE", nlp_cur2->chp_name)) {
					chp_value = Escape_HtmlString(nlp_cur2->chp_value);
					printf("<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", nlp_cur2->chp_name, chp_value);
					free(chp_value);
				}
			}
		}
	}
	return;
}

/*
+* ------------------------------------------------------------------------
 * Function:		check_movie_type
 * Description:
 *			ư��ե�����μ��������å�
%* ------------------------------------------------------------------------
 * Return:
 *			0: ����
 *			1: ���顼
-* ----------------------------------------------------------------------*/
int check_movie_type(NLIST *nlp_in)
{
	char *chp_val;
	char cha_type[16];
	char cha_file[512];

	memset(cha_file, '\0', sizeof(cha_file));
	if(!Get_Nlist(nlp_in, "movie", 3) || *Get_Nlist(nlp_in, "movie", 3) == '\0') {
		return 1;
	}
	strcpy(cha_file, Get_Nlist(nlp_in, "movie", 3));
	chp_val = strrchr(cha_file, '.');
	memset(cha_type, '\0', sizeof(cha_type));
	strcat(cha_type, chp_val + 1);
	if(strcmp(cha_type, "3gp")
	   && strcmp(cha_type, "avi")
	   && strcmp(cha_type, "mp4")
	   && strcmp(cha_type, "mpg")
	   && strcmp(cha_type, "wmv")) {
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	temporary_register_uploadmovie()
 * Description:
 *	ư��ե�����Υ��åץ��ɻ��ν�����
 *	ư��ե�����򥢥åץ��ɸ塢�ե�������͡��ࡣ
 *	���θ塢ź�եե�����ơ��֥�˲���Ͽ��
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
int temporary_register_uploadmovie(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	struct stat sb;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmpfile_id;
	char *chp_escape;
	char cha_command[5120];
	char cha_filename[1024];
	char cha_oldfile[1024];
	char cha_buf[1024];
	char cha_error[512];
	char cha_sql[1024];
	int in_pid;
	int in_status;
	int in_max_attach;
	int in_skip;
	int in_ret;

	/* ź�եե�����ID�����뤳�Ȥ��ǧ */
	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	if (!chp_tmpfile_id || !*chp_tmpfile_id) {
		Put_Nlist(nlp_out, "ERROR", "�����ե�����ID�������Ǥ��ޤ���");
	}
	/* �ե�����Υ��顼�����å� */
	chp_tmp = Get_Nlist(nlp_in, "movie", 1);	/* ��¸�ե�����ե�ѥ� */
	if (!chp_tmp) {
//		sprintf(cha_error, "�ե����륵������%dkB�ʲ��Ǥ��ꤤ�פ��ޤ���\n", CO_MAX_UPLOAD_SIZE);
//		Put_Nlist(nlp_out, "ERROR", cha_error);
//		return 1;
	} else if (strlen(chp_tmp) == 0) {
		Put_Nlist(nlp_out, "ERROR", "�ե����뤬���Ϥ���Ƥ��ޤ���");
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "movie", 2);	/* �ե����륿���� */
	if (!chp_tmp || (chp_tmp && strlen(chp_tmp) == 0)){
		Put_Nlist(nlp_out, "ERROR", "�ե����륿���פ�̵���Ǥ���");
		remove(Get_Nlist(nlp_in, "movie", 1));
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "movie", 3);	/* ���åץ��ɥե�����̾ */
	if (!chp_tmp || (chp_tmp && strlen(chp_tmp) == 0)){
		remove(Get_Nlist(nlp_in, "movie", 1));
		Put_Nlist(nlp_out, "ERROR", "�ե����뤬���Ĥ���ʤ������⤷���϶��Υե����뤬���ꤵ��Ƥ��ޤ���");
		return 1;
	}
	in_max_attach = CO_MAX_ATTACHED_MOVIE;
	dbres = Db_OpenDyna(db, "select n_max_attach from sy_baseinfo");
	if (dbres) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_max_attach = atoi(chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	/* ź�եե������CO_MAX_ATTACHED_MOVIE�ޤ� */
	strcpy(cha_sql, "select count(*)");	/* 0 ź��ư��ե�����Ŀ� */
	strcat(cha_sql, " from at_uploadmovie T1");
	strcat(cha_sql, " where T1.n_entry_id in (");
	strcat(cha_sql, chp_tmpfile_id);
	chp_tmp = Get_Nlist(nlp_in, "entry_id", 1);
	if (chp_tmp && *chp_tmp) {
		strcat(cha_sql, ", ");
		strcat(cha_sql, chp_tmp);
	}
	strcat(cha_sql, ")");
	strcat(cha_sql, " and T1.b_delete_mode = 0");	/* ���ͽ��Ǥʤ� */
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ե�����������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp && *chp_tmp && atoi(chp_tmp) >= in_max_attach/*CO_MAX_ATTACHED_FILE*/) {
		sprintf(cha_error, "��Ĥε�����ź�դǤ���ư��ե������ %d�� �ޤǤǤ���", in_max_attach/*CO_MAX_ATTACHED_MOVIE*/);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		Db_CloseDyna(dbres);
		return 1;
	}
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "movie", 1);
	if (stat(chp_tmp, &sb) < 0) {
		remove(chp_tmp);
		Put_Nlist(nlp_out, "ERROR", "���åץ��ɤ����ե�����ξ�������˼��Ԥ��ޤ�����<br>");
		return 1;
	}
	//chp_escape = Get_Nlist(nlp_in, "movie", 2);
	//if (!chp_escape || (!g_in_hb_mode && strncasecmp(chp_escape, "image/", 6) != 0)) {
	//	remove(chp_tmp);
	//	Put_Nlist(nlp_out, "ERROR", "���åץ��ɤǤ���ե�����ϲ����ե���������Ǥ���<br>");
	//	return 1;
	//}
	if(check_movie_type(nlp_in)) {
		remove(chp_tmp);
		Put_Nlist(nlp_out, "ERROR", "ư��ե������3gp��avi��mp4��mpg��wmv�Τ��Ť줫����ꤷ�Ƥ���������");
		return 1;
	}

	if (Begin_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥�����󳫻Ϥ˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
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
		/* ffmeg�Ǥ��Ѵ����ѹ� */
		memset(cha_buf, '\0', sizeof(cha_buf));
		strcpy(cha_buf, Get_Nlist(nlp_in, "movie", 3));
		chp_tmp = strrchr(cha_buf, '.');
		if(chp_tmp) {
			chp_tmp++;
		} else {
			chp_tmp = cha_buf;
		}
		memset(cha_oldfile, '\0', sizeof(cha_oldfile));
		sprintf(cha_oldfile, "%s.%s", Get_Nlist(nlp_in, "movie", 1), chp_tmp);
		rename(Get_Nlist(nlp_in, "movie", 1), cha_oldfile);
		memset(cha_buf, '\0', sizeof(cha_buf));
		strcpy(cha_buf, Get_Nlist(nlp_in, "movie", 1));
		chp_tmp = strrchr(cha_buf, '.');
		if(chp_tmp) {
			chp_tmp++;
		} else {
			chp_tmp = cha_buf;
		}
		memset(cha_filename, '\0', sizeof(cha_filename));
		if (g_in_cart_mode == CO_CART_RESERVE) {
			sprintf(cha_filename, "%s%s%s.flv", Get_HomeDirectory(), CO_TEMPDIR_PATH, chp_tmp);
		} else if (g_in_cart_mode == CO_CART_SHOPPER) {
			chp_tmp2 = GetCartImageDirectory();
			if(!chp_tmp2) {
				return 1;
			}
			sprintf(cha_filename, "%s/%s.flv", chp_tmp2, chp_tmp);
		}

		Page_Out(nlp_out, CO_SKEL_UPLOAD_MOVIE);
		fflush(stdout);
		in_pid = fork();
		if(in_pid == 0) {
			in_ret = Convert_To_FlvFile(cha_oldfile, cha_filename, CO_DEFAULT_VALUE, CO_DEFAULT_VALUE, 320, CO_DEFAULT_VALUE);
			if(in_ret > 10) {
				remove(cha_oldfile);
				printf("</body>\n");
				printf("<body onload=\"document.jump.submit();\">\n");
				if (g_in_cart_mode == CO_CART_RESERVE) {
					printf("<form action=\"/reserve/cgi-bin/admin/%s\" method=\"post\" name=\"jump\">\n", CO_CGI_ENTRY);
				} else {
					printf("<form action=\"/e-commex/cgi-bin/admin/%s\" method=\"post\" name=\"jump\">\n", CO_CGI_ENTRY);
				}
				build_hidden_all_print(nlp_in, nlp_out);
				printf("<input type=\"hidden\" name=\"BTN_RE_DISP\" value=\"1\">\n");
				printf("<input type=\"hidden\" name=\"page\" value=\"0\">\n");
				printf("<input type=\"hidden\" name=\"from_user\" value=\"0\">\n");
				printf("<input type=\"hidden\" name=\"upload_movie\" value=\"1\">\n");
				printf("<input type=\"hidden\" name=\"ERROR\" value=\"ư��ե�����Υ��åץ��ɤ˼��Ԥ��ޤ�����<br>\">\n");
				printf("</form>\n");
				printf("</body>\n");
				fflush(stdout);
				Rollback_Transact(db);
				exit(1);
			}
			memset(cha_command, '\0', sizeof(cha_command));
			sprintf(cha_command, "/bin/chmod 644 %s", cha_filename);
			system(cha_command);
			remove(cha_oldfile);
			printf("</body>\n");
			printf("<body onload=\"document.jump.submit();\">\n");
			if (g_in_cart_mode == CO_CART_RESERVE) {
				printf("<form action=\"/reserve/cgi-bin/admin/%s\" method=\"post\" name=\"jump\">\n", CO_CGI_ENTRY);
			} else {
				printf("<form action=\"/e-commex/cgi-bin/admin/%s\" method=\"post\" name=\"jump\">\n", CO_CGI_ENTRY);
			}
			build_hidden_all_print(nlp_in, nlp_out);
			printf("<input type=\"hidden\" name=\"BTN_RE_DISP\" value=\"1\">\n");
			printf("<input type=\"hidden\" name=\"page\" value=\"0\">\n");
			printf("<input type=\"hidden\" name=\"from_user\" value=\"0\">\n");
			printf("<input type=\"hidden\" name=\"upload_movie\" value=\"1\">\n");
			printf("</form>\n");
			printf("</body>\n");
			fflush(stdout);
		} else if(in_pid == -1) {
			remove(cha_oldfile);
			printf("</body>\n");
			printf("<body onload=\"document.jump.submit();\">\n");
			if (g_in_cart_mode == CO_CART_RESERVE) {
				printf("<form action=\"/reserve/cgi-bin/admin/%s\" method=\"post\" name=\"jump\">\n", CO_CGI_ENTRY);
			} else {
				printf("<form action=\"/e-commex/cgi-bin/admin/%s\" method=\"post\" name=\"jump\">\n", CO_CGI_ENTRY);
			}
			build_hidden_all_print(nlp_in, nlp_out);
			printf("<input type=\"hidden\" name=\"BTN_RE_DISP\" value=\"1\">\n");
			printf("<input type=\"hidden\" name=\"page\" value=\"0\">\n");
			printf("<input type=\"hidden\" name=\"from_user\" value=\"0\">\n");
			printf("<input type=\"hidden\" name=\"upload_movie\" value=\"1\">\n");
			printf("<input type=\"hidden\" name=\"ERROR\" value=\"ư��ե�����Υ��åץ��ɤ˼��Ԥ��ޤ�����<br>\">\n");
			printf("</form>\n");
			printf("</body>\n");
			fflush(stdout);
			Rollback_Transact(db);
			exit(1);
		} else {
			while(waitpid(-1, &in_status, WNOHANG) <= 0) {
				printf("<!-- -->\n");
				fflush(stdout);
				sleep(5);
			}
			exit(0);
		}

		memset(cha_filename, '\0', sizeof(cha_filename));
		sprintf(cha_filename, "%s.flv", chp_tmp);
		chp_escape = Escape_HtmlString(Get_Nlist(nlp_in, "movie", 3));
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "insert into at_uploadmovie");
		strcat(cha_sql, "(n_blog_id");
		strcat(cha_sql, ",n_entry_id");
		strcat(cha_sql, ",c_filename");
		strcat(cha_sql, ",c_origname");
		strcat(cha_sql, ",b_delete_mode");
		strcat(cha_sql, ") values(");
		sprintf(cha_sql + strlen(cha_sql), "%d", in_blog);
		sprintf(cha_sql + strlen(cha_sql), ", %s", chp_tmpfile_id);
		sprintf(cha_sql + strlen(cha_sql), ", '%s'", cha_filename);
		sprintf(cha_sql + strlen(cha_sql), ", '%s'", chp_escape);
		strcat(cha_sql, ", 0");				/* ����⡼�ɤ�0 */
		strcat(cha_sql, ")");
		free(chp_escape);
		if (Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "�ե����륢�åץ��ɲ���Ͽ�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
	}
	if (Commit_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "�ե����륢�åץ��ɲ���Ͽ�Υ��ߥåȤ˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	put_blog_name()
 * Description:
 *	���åץǡ��Ȥ���ӥȥ�å��Хå��Ѥ˥֥�̾��HIDDEN�������ࡣ
%* ------------------------------------------------------------------------
 * Return:
 * 	���ｪλ 0
 *	���顼�� 1
-* ------------------------------------------------------------------------*/
int put_blog_name(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];

	strcpy(cha_sql, "select T1.c_blog_title");
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "�֥������ȥ�����륯���꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "blog_name", chp_escape);
		free(chp_escape);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_article_attach()
 * Description:
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int build_article_attach(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,int in_blog
)
{
	DBRes *dbres;
	int i, c;
	int in_uploadfile_id;
	char cha_sql[8192];
	char *chp_tmp;
	char *chp_eid;
	char *chp_escape;
	char *chp_filename;
	int in_row;

	chp_tmp = Get_Nlist(nlp_in, "tmpfile_id", 1);
	chp_eid = Get_Nlist(nlp_in, "entry_id", 1);
	if (!chp_tmp && !chp_eid) {
		Put_Nlist(nlp_out, "ERROR", "����ID������Ǥ��ޤ���<br>");
		return 1;
	}
	strcpy(cha_sql, "select T1.n_uploadfile_id");	/* 0 �ե�����ID */
	strcat(cha_sql, ",T1.c_fileimage");				/* 1 �ե����륤�᡼�� */
	strcat(cha_sql, ",T1.c_filename");				/* 2 �ե�����̾ɽ��̾ */
	strcat(cha_sql, ",T1.c_filetype");				/* 3 �ե����륿����*/
	strcat(cha_sql, ",T1.n_imagesize_x");			/* 4 X���������� */
	strcat(cha_sql, ",T1.n_imagesize_y");			/* 5 Y���������� */
	strcat(cha_sql, ",T1.n_entry_id");				/* 6 ����ID */
	strcat(cha_sql, " from at_uploadfile T1");
	if (chp_eid) {
		if (chp_tmp) {
			sprintf(cha_sql + strlen(cha_sql), " where (T1.n_entry_id = %s or T1.n_entry_id = %s)", chp_tmp, chp_eid);
		} else {
			sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %s", chp_eid);
		}
	} else {
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %s", chp_tmp);
	}
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " and b_delete_mode = 0");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "���åץ��ɥե�����ξ�������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	/* ���᡼���ѥ����������ʤ��顢SQLʸ���� */
	c = Db_GetRowCount(dbres);
	if (c)
		Put_Nlist(nlp_out, "ARTICLE", "\n<div class=\"entry_body\">");
	for(i = 0; i < c; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (!chp_tmp)
			continue;
		in_uploadfile_id = atoi(chp_tmp);
		if (Db_GetValue(dbres, i, 3) && strstr(Db_GetValue(dbres, i, 3), "image/")) {
			double dbl_x, dbl_y, dbl_ratio;
			dbl_x = 160.0;
			chp_tmp = Db_GetValue(dbres, i, 4);
			if (chp_tmp)
				dbl_x = (double)atoi(chp_tmp);
			dbl_y = 120.0;
			chp_tmp = Db_GetValue(dbres, i, 5);
			if (chp_tmp)
				dbl_y = (double)atoi(chp_tmp);
			dbl_ratio = dbl_y / dbl_x;
			while (dbl_x * dbl_y > 160.0 * 120.0) {
				dbl_x -= 1.0;
				dbl_y = dbl_ratio * dbl_x;
			}
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "ARTICLE",
					"<a href=\"%s%s%s/%s/%s-%s-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">"
					"<img src=\"%s%s%s/%s/%s-%s-%d?BTN_DISP_ATTACH_FILE=1\" width=\"%d\" height=\"%d\" border=\"0\">"
					"</a>\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, Db_GetValue(dbres, i, 6), in_uploadfile_id,
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, Db_GetValue(dbres, i, 6), in_uploadfile_id,
					(int)dbl_x, (int)dbl_y);
			} else {
				Put_Format_Nlist(nlp_out, "ARTICLE",
					"<a href=\"%s%s%s/%s/%d-%s-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">"
					"<img src=\"%s%s%s/%s/%d-%s-%d?BTN_DISP_ATTACH_FILE=1\" width=\"%d\" height=\"%d\" border=\"0\">"
					"</a>\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, Db_GetValue(dbres, i, 6), in_uploadfile_id,
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, Db_GetValue(dbres, i, 6), in_uploadfile_id,
					(int)dbl_x, (int)dbl_y);
			}
		} else {
			chp_tmp = Db_GetValue(dbres, i, 2);
			chp_escape = Escape_HtmlString(chp_tmp);
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "ARTICLE",
					"<a href=\"%s%s%s/%s/%s-%s-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">%s</a>\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, Db_GetValue(dbres, i, 6), in_uploadfile_id, chp_escape);
			} else {
				Put_Format_Nlist(nlp_out, "ARTICLE",
					"<a href=\"%s%s%s/%s/%d-%s-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">%s</a>\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, Db_GetValue(dbres, i, 6), in_uploadfile_id, chp_escape);
			}
			free(chp_escape);
		}
	}
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "tmpfile_id", 1);
	/* ư�� */
	if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "select T1.c_filename");	/* 0 */
		strcat(cha_sql, ", T1.n_entry_id");			/* 1 */
		strcat(cha_sql, " from at_uploadmovie T1");
		strcat(cha_sql, " where T1.b_delete_mode = 0");	/* ���ͽ�󤵤�Ƥ��ʤ� */
		/* ������°���Ƥ����Τȡ����ե�����ȤʤäƤ����Τ򥻥쥯�� */
		if (chp_eid || chp_tmp) {
			strcat(cha_sql, " and T1.n_entry_id in (");
			if (chp_eid) {
				strcat(cha_sql, chp_eid);
			}
			if (chp_eid && chp_tmp) {
				strcat(cha_sql, ", ");
			}
			if (chp_tmp) {
				strcat(cha_sql, chp_tmp);
			}
			strcat(cha_sql, ")");
		}
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "������°���Ƥ���ư��ե���������륯����˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		in_row = Db_GetRowCount(dbres);
		/* �ե�����ꥹ�� */
		if (in_row != 0) {
			for(i = 0; i < in_row; i++){
				chp_filename = Db_GetValue(dbres, i, 0);
				if (chp_filename){
					Put_Nlist(nlp_out, "ARTICLE", "<object width=\"320\" height=\"240\">\n");
					Put_Nlist(nlp_out, "ARTICLE", "<param name=\"allowScriptAccess\" value=\"sameDomain\" />\n");
					if (g_in_cart_mode == CO_CART_RESERVE) {
						Put_Nlist(nlp_out, "ARTICLE", "<param name=\"movie\" value=\"/reserve/cgi-bin/asjplayer.swf\" />\n");
						Put_Format_Nlist(nlp_out, "ARTICLE", "<param name=\"FlashVars\" value=\"flvfile=/reserve/images/%s&thumb=../../../image/b1-ti_img.jpg\" />\n", chp_filename);
					} else {
						Put_Nlist(nlp_out, "ARTICLE", "<param name=\"movie\" value=\"/e-commex/cgi-bin/asjplayer.swf\" />\n");
						Put_Format_Nlist(nlp_out, "ARTICLE", "<param name=\"FlashVars\" value=\"flvfile=/e-commex/images/upload/%s&thumb=../../../image/b1-ti_img.jpg\" />\n", chp_filename);
					}
					Put_Nlist(nlp_out, "ARTICLE", "<param name=\"quality\" value=\"high\" />\n");
					if (g_in_cart_mode == CO_CART_RESERVE) {
						Put_Format_Nlist(nlp_out, "ARTICLE", "<embed src=\"/reserve/cgi-bin/asjplayer.swf\" FlashVars=\"flvfile=/reserve/images/%s&../../../image/b1-ti_img.jpg\" quality=\"high\" width=\"300\" height=\"250\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" />\n", chp_filename);
					} else {
						Put_Format_Nlist(nlp_out, "ARTICLE", "<embed src=\"/e-commex/cgi-bin/asjplayer.swf\" FlashVars=\"flvfile=/e-commex/images/%s&../../../image/b1-ti_img.jpg\" quality=\"high\" width=\"300\" height=\"250\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" />\n", chp_filename);
					}
					Put_Nlist(nlp_out, "ARTICLE", "</object>\n");
				}
			}
		}
		Db_CloseDyna(dbres);
	}
	if (c) {
		Put_Nlist(nlp_out, "ARTICLE", "<br></div>\n");
	}
	return 0;
}

/*
+* =========================================================================
 * Function:            insert_link()
 * Description:
 *	��Ϣ��󥯤��ɲä��롣
%* =========================================================================
 * Return:
 *		�ɲ÷��
-* ========================================================================*/
int display_link(DBase *db, NLIST *nlp_out, int in_blog, int in_entry, char *chp_url)
{
	NLIST *nlp_cur;
	NLIST *nlp_cur2;
	NLIST *nlp_cookie;
	CP_NKF nkf;
	NKF_CODE cd;
	int i;
	char cha_sql[1024];
	char cha_cookie[16384];
	char cha_host[1024];
	char cha_param[1024];
	char cha_path[1024];
	char *chp_title;
	char *chp_value;
	char *chp_host;
	char *chp_param;
	char *chp_str;
	char *chp_tmp;

	chp_title = NULL;
	cha_host[0] = '\0';
	cha_path[0] = '\0';
	cha_param[0] = '\0';
	chp_tmp = strstr(chp_url, "://");
	if (chp_tmp) {
		chp_tmp += strlen("://");
		strncpy(cha_sql, chp_url, chp_tmp - chp_url);
		cha_sql[chp_tmp - chp_url] = '\0';
	} else {
		strcpy(cha_sql, "http://");
	}
	strcat(cha_sql, "%[^/]%[^?]%s");
	sscanf(chp_url, cha_sql, cha_host, cha_path, cha_param);
	if (!cha_host[0]) {
		return 0;
	}
	if (!cha_path[0]) {
		strcpy(cha_path, "/");
	}

	cha_cookie[0] = '\0';
	nlp_cookie = Get_Cookie();
	for(i = 0; i < CO_MAX_SLOT; i++) {
		for(nlp_cur = nlp_cookie + i; nlp_cur->nlp_next; ) {
			nlp_cur = nlp_cur->nlp_next;
			chp_value = Escape_HtmlString(nlp_cur->chp_value);
			if (!cha_cookie[0])
				strcpy(cha_cookie, "Cookie:");
			sprintf(cha_cookie + strlen(cha_cookie), " %s=%s;", nlp_cur->chp_name, chp_value);
			free(chp_value);
			for(nlp_cur2 = nlp_cur; nlp_cur2->nlp_name_next; ) {
				nlp_cur2 = nlp_cur2->nlp_name_next;
				chp_value = Escape_HtmlString(nlp_cur2->chp_value);
				if (!cha_cookie[0])
					strcpy(cha_cookie, "Cookie:");
				sprintf(cha_cookie + strlen(cha_cookie), " %s=%s;", nlp_cur2->chp_name, chp_value);
				free(chp_value);
			}
		}
	}
	if (cha_cookie[0])
		strcat(cha_cookie, "\r\n");
	Finish_Nlist(nlp_cookie);

	strcat(cha_path, cha_param);
	chp_tmp = cha_param;
	if (chp_tmp[0] == '?') {
		++chp_tmp;
	}
	if (Http_Get_Get_Result(cha_host, cha_path, chp_tmp, cha_cookie, &chp_str)) {
		Put_Nlist(nlp_out, "ERROR", "���ꤵ�줿URL�˥��������Ǥ��ޤ���<br>");
		return CO_ERROR;
	}

	nkf = Create_Nkf();
	Set_Input_String(nkf, chp_str);
	Set_Output_Code(nkf, NKF_EUC);
	cd = Convert_To_String(nkf, &chp_tmp);
	Destroy_Nkf_Engine(nkf);
	free(chp_str);

	chp_host = strcasestr(chp_tmp, "<TITLE");
	if (chp_host) {
		chp_host += strlen("<TITLE");
		chp_param = strchr(chp_host, '>');
		if (chp_param) {
			++chp_param;
			chp_str = strchr(chp_param, '<');
			if (chp_str) {
				*chp_str = '\0';
				chp_title = (char*)malloc(strlen(chp_param) + 1);
				strcpy(chp_title, chp_param);
			}
		}
	}
	free(chp_tmp);
	if (chp_title) {
		Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"%s\">%s</a><br>\n", chp_url, chp_title);
		free(chp_title);
	} else {
		Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"%s\">%s</a><br>\n", chp_url, chp_url);
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	preview_draft()
 * Description:		��ƤΥץ�ӥ塼
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int preview_draft(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	struct tm *stp_now;
	DBRes *dbres;
	char *chp_br;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_ascii;
	char *chp_escape;
	char cha_css[1024];
	char cha_sql[2048];
	char cha_str[4096];
	char cha_url[256];
	time_t tm_now;
	int in_skin_no;
	int in_cate_no;
	int in_dispcart;
	int in_comment;
	int in_trackback;
	int in_entry;
	int j;

	if (error_check(db, nlp_in, nlp_out)) {
		return 1;
	}
	/* �������륷���ȡʥ桼������ν�����ޤ��*/
	strcpy(cha_sql, " select T1.n_theme_id");	/* 0 ������ID */
	strcat(cha_sql, ",T1.c_user_css");			/* 1 �桼�����CSS */
	strcat(cha_sql, ",T1.n_category_id");		/* 2 ������ */
	strcat(cha_sql, " from at_looks T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�������륷������������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_skin_no = atoi(chp_tmp);
	} else {
		in_skin_no = 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		strcpy(cha_str, chp_tmp);
		in_skin_no = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		in_cate_no = atoi(chp_tmp);
	} else {
		in_cate_no = 1;
	}
	Db_CloseDyna(dbres);
	if (in_skin_no == 0) {
		strcpy(cha_css, cha_str);
	} else {
		strcpy(cha_sql, "select T1.c_theme_file");
		strcat(cha_sql, " from sy_theme T1");
		sprintf(cha_sql + strlen(cha_sql), " where n_theme_id = %d", in_skin_no);
		sprintf(cha_sql + strlen(cha_sql), " and n_category_id = %d", in_cate_no);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "�ơ��޾ܺ٤����륯���꡼�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp){
			sprintf(cha_css, "%s/%s", g_cha_css_location, chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	Put_Nlist(nlp_out, "PATHCSS", cha_css);
	/* entry */
	/* entry_date */
	Put_Nlist(nlp_out, "ARTICLE", "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"entry-day-title\"><tr><td height=\"30\">");
	tm_now = time(NULL);
	stp_now = localtime(&tm_now);
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	/* �������ˤϡ�specify_date���ʤ� */
	if (!chp_tmp || atoi(chp_tmp) == 1) {
		chp_tmp = Get_Nlist(nlp_in, "year", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
		} else {
			sprintf(cha_str, "%d", stp_now->tm_year + 1900);
			Put_Nlist(nlp_out, "ARTICLE", cha_str);
		}
		Put_Nlist(nlp_out, "ARTICLE", "ǯ");
		chp_tmp = Get_Nlist(nlp_in, "month", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
		} else {
			sprintf(cha_str, "%d", stp_now->tm_mon);
			Put_Nlist(nlp_out, "ARTICLE", cha_str);
		}
		Put_Nlist(nlp_out, "ARTICLE", "��");
		chp_tmp = Get_Nlist(nlp_in, "day", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
		} else {
			sprintf(cha_str, "%d", stp_now->tm_mday);
			Put_Nlist(nlp_out, "ARTICLE", cha_str);
		}
		Put_Nlist(nlp_out, "ARTICLE", "��");
	} else {
		strftime(cha_str, strlen("YYYYǯMM��DD��") + 1, "%Yǯ%m��%d��", stp_now);
		Put_Nlist(nlp_out, "ARTICLE", cha_str);
	}
	Put_Nlist(nlp_out, "ARTICLE", "</td></tr></table>\n");
	/* entry_title */
	Put_Nlist(nlp_out, "ARTICLE", "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"entrytable\">\n");
	Put_Nlist(nlp_out, "ARTICLE", "<tr>\n");
	Put_Nlist(nlp_out, "ARTICLE", "<td align=\"left\">\n");
	Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_title\">");
	Put_Nlist(nlp_out, "ARTICLE", "<a href=\"#entry\">");
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		chp_ascii = Conv_Long_Ascii(chp_escape, 40);
		chp_br = Conv_Blog_Br(chp_ascii);
		Put_Nlist(nlp_out, "ARTICLE", chp_br);
		free(chp_br);
		free(chp_ascii);
		free(chp_escape);
	}
	Put_Nlist(nlp_out, "ARTICLE", "</a>\n");
	Put_Nlist(nlp_out, "ARTICLE", "</div>\n");
	/* ź�եե����� */
	chp_tmp = Get_Nlist(nlp_in, "tmpfile_id", 1);
	if (chp_tmp) {
		build_article_attach(db, nlp_in, nlp_out, in_blog);
	}
	/* entry_body */
	Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_body\">");
	if (g_in_dbb_mode && Get_Nlist(nlp_in, "INSERT_ORDER", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "entry_more", 1);
		if (chp_tmp && chp_tmp[0]) {
			Put_Nlist(nlp_out, "ARTICLE", "<br>");
			Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
			Put_Nlist(nlp_out, "ARTICLE", "\n<br clear=\"all\">");
		} else {
			NLIST *nlp_tmp;
			char *chp_order;
			nlp_tmp = Init_Nlist();
			if (Disp_Team_Player(db, nlp_in, nlp_tmp, nlp_out, "TEMP", "team")) {
				Finish_Nlist(nlp_tmp);
				return 1;
			}
			chp_order = Page_Out_Mem(nlp_tmp, "blog_temp.skl");
			Put_Nlist(nlp_out, "ARTICLE", "<br>");
			Put_Nlist(nlp_out, "ARTICLE", chp_order);
			Put_Nlist(nlp_out, "ARTICLE", "\n<br clear=\"all\">");
			free(chp_order);
		}
	}
	if (g_in_dbb_mode && Get_Nlist(nlp_in, "INSERT_BATTLE_ORDER", 1)) {
		/*battle order*/
#if 0
		chp_tmp = Get_Nlist(nlp_in, "entry_more", 1);
		if (chp_tmp && chp_tmp[0]) {
			Put_Nlist(nlp_out, "ARTICLE", "<br>");
			Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
			Put_Nlist(nlp_out, "ARTICLE", "\n<br clear=\"all\">");
		} else {
#endif
			NLIST *nlp_tmp;
			char *chp_order;
			nlp_tmp = Init_Nlist();
			if (Disp_Battle_Player(db, nlp_in, nlp_tmp, nlp_out, "TEMP", "b", in_blog)) {
				Finish_Nlist(nlp_tmp);
				return 1;
			}
			chp_order = Page_Out_Mem(nlp_tmp, "blog_temp.skl");
			Put_Nlist(nlp_out, "ARTICLE", "<br>");
			Put_Nlist(nlp_out, "ARTICLE", chp_order);
			Put_Nlist(nlp_out, "ARTICLE", "\n<br clear=\"all\">");
			free(chp_order);
//		}
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_body", 1);
/*
	chp_escape = Remove_Invalid_Tags(chp_tmp);
	if (g_in_html_editor) {
		Put_Nlist(nlp_out, "ARTICLE", chp_escape);
	} else {
		chp_tmp = Conv_Blog_Br(chp_escape);
		Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
		free(chp_tmp);
	}
	free(chp_escape);
*/
	chp_tmp2 = Remove_Invalid_Tags(chp_tmp, nlp_out, 0);
	if (!chp_tmp2) {
		return 1;
	}
	if (g_in_html_editor) {
		Put_Nlist(nlp_out, "ARTICLE", chp_tmp2);
	} else {
		chp_ascii = Conv_Long_Ascii(chp_tmp2, 60);
		chp_escape = Conv_Blog_Br(chp_ascii);
		Put_Nlist(nlp_out, "ARTICLE", chp_escape);
		free(chp_escape);
		free(chp_ascii);
	}
	free(chp_tmp2);
	Put_Nlist(nlp_out, "ARTICLE", "</div>\n");
	in_entry = 0;
	chp_tmp = Get_Nlist(nlp_in, "entry_id", 1);
	if (chp_tmp) {
		in_entry = atoi(chp_tmp);
	}
	/* ��Ϣ��󥯥ꥹ�Ȥ��� */
	if (g_in_dbb_mode) {
		int in_tmp;
		char cha_num[32];
		cha_num[0] = '\0';
		chp_tmp = Get_Nlist(nlp_in, "o", 1);
		if (chp_tmp) {
			strcpy(cha_num, chp_tmp);
			chp_tmp = Get_Nlist(nlp_in, "f", 1);
			if (chp_tmp[0] == 'c' || chp_tmp[0] == 'C') {
				chp_tmp = Get_Nlist(nlp_in, "o", 1);
				in_tmp = Temp_To_Owner(db, nlp_out, chp_tmp);
				if (in_tmp >= 0) {
					sprintf(cha_num, "%d", in_tmp);
				}
			}
		}
		chp_tmp = Get_DBB_Topic(db, nlp_out, in_blog, Get_Nlist(nlp_in, "f", 1), Get_Nlist(nlp_in, "b", 1), cha_num, Get_Nlist(nlp_in, "l", 1));
		if (chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			chp_ascii = Conv_Long_Ascii(chp_escape, 60);
			chp_br = Conv_Blog_Br(chp_ascii);
			Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_associatelink\">��Ϣ��󥯡�<br>\n");
			Put_Nlist(nlp_out, "ARTICLE", chp_br);
			Put_Nlist(nlp_out, "ARTICLE","</div>\n");
			free(chp_br);
			free(chp_ascii);
			free(chp_escape);
			free(chp_tmp);
		} else {
			int in_tb_blog;
			int in_tb_entry;
			chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
			chp_tmp2 = strrchr(chp_tmp, '/');
			if (chp_tmp && chp_tmp2 && sscanf(chp_tmp2, "/%d-%d", &in_tb_blog, &in_tb_entry) == 2) {
				Get_Nickname_From_Blog(db, nlp_out, in_tb_blog, cha_str);
				sprintf(cha_sql, "select c_entry_title from at_entry where n_blog_id = %d and n_entry_id = %d", in_tb_blog, in_tb_entry);
				dbres = Db_OpenDyna(db, cha_sql);
				if (!dbres) {
					Put_Nlist(nlp_out, "ERROR", "���������ȥ�����륯����˼��Ԥ��ޤ�����<br>");
					Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
					return 1;
				}
				chp_tmp = Db_GetValue(dbres, 0, 0);
				if (chp_tmp) {
					cha_sql[0] = '\0';
					chp_escape = Escape_HtmlString(chp_tmp);
					chp_ascii = Conv_Long_Ascii(chp_escape, 60);
					chp_br = Conv_Blog_Br(chp_ascii);
					chp_tmp = Escape_HtmlString(cha_str);
					Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_associatelink\">��Ϣ��󥯡�<br>\n");
					Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"%s\">%s(%s)</a>\n", Get_Nlist(nlp_in, "trackback_url", 1), chp_br, chp_tmp);
					Put_Nlist(nlp_out, "ARTICLE","</div>\n");
					free(chp_tmp);
					free(chp_br);
					free(chp_ascii);
					free(chp_escape);
				}
				Db_CloseDyna(dbres);
			}
		}
	} else {
		chp_tmp = Get_Nlist(nlp_in, "related_url", 1);
		if (chp_tmp && strlen(chp_tmp) != 0){
			j = 0;
			chp_tmp2 = strpbrk(chp_tmp, "\n");
			while (1) {
				if (!chp_tmp2) {
					if (strlen(chp_tmp)) {
						chp_tmp2 = chp_tmp + strlen(chp_tmp);
					} else {
						break;
					}
				}
				if (chp_tmp2 - chp_tmp < sizeof(cha_url)) {
					strncpy(cha_url, chp_tmp, chp_tmp2 - chp_tmp);
					cha_url[chp_tmp2 - chp_tmp] = '\0';
					chp_escape = strchr(cha_url, '\r');
					if (chp_escape)
						*chp_escape = '\0';
					if (!j) {
						Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_associatelink\">��Ϣ��󥯡�<br>\n");
					}
					if (display_link(db, nlp_out, in_blog, in_entry, cha_url)) {
						return CO_ERROR;
					}
					++j;
				}
				chp_tmp = chp_tmp2;
				++chp_tmp;
				if (*chp_tmp == '\r')
					++chp_tmp;
				chp_tmp2 = strpbrk(chp_tmp, "\n");
			}
			if (j) {
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
			}
		}
	}
	/* �����Ȥδ�Ϣ���� */
	chp_tmp = Get_Nlist(nlp_in, "item_id", 1);
	if (g_in_cart_mode == CO_CART_SHOPPER && chp_tmp && chp_tmp[0]) {
		Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_body\">\n");
		if (build_article_cart(db, nlp_in, nlp_out, in_blog, chp_tmp, 0, &in_dispcart)) {
			return 1;
		}
		Put_Nlist(nlp_out, "ARTICLE","</div>\n");
	}
	Put_Nlist(nlp_out, "ARTICLE", "</td></tr>\n");
	Put_Nlist(nlp_out, "ARTICLE", "<tr><td width=\"100%\">\n");
	/* entry_state */
	Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_state\">");
	if (!g_in_dbb_mode) {
		chp_tmp = Get_Nlist(nlp_in, "entry_category", 1);
		if (chp_tmp && *chp_tmp) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			strcpy(cha_sql, "select T1.c_category_name");	/* 0 ���ƥ���̾ */
			strcat(cha_sql, " from at_category T1");
			sprintf(cha_sql + strlen(cha_sql), " where T1.n_category_id = %s", chp_tmp);
			sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
			dbres = Db_OpenDyna(db, cha_sql);
			if (!dbres) {
				Put_Nlist(nlp_out, "ERROR", "���ƥ��꡼̾�����륯����˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				return 1;
			}
			Put_Nlist(nlp_out, "ARTICLE", "<a href=\"#category\">");
			chp_tmp = Db_GetValue(dbres, 0, 0);
			if (chp_tmp && *chp_tmp) {
				chp_escape = Escape_HtmlString(chp_tmp);
				Put_Nlist(nlp_out, "ARTICLE", chp_escape);
				free(chp_escape);
			} else {
				Put_Nlist(nlp_out, "ARTICLE", "category");
			}
			Put_Nlist(nlp_out, "ARTICLE", "</a>\n");
			Db_CloseDyna(dbres);
		} else {
			Put_Nlist(nlp_out, "ARTICLE", "<a href=\"#category\">category</a>\n");
		}
		Put_Nlist(nlp_out, "ARTICLE", " | ");
	}
	// �����ȿ��ȥȥ�å��Хå���
	in_comment = 0;
	in_trackback = 0;
	if (in_entry) {
		if(Make_Tmptable_For_Count(db, nlp_out, in_blog, 0)) {
			return 1;
		}
		sprintf(cha_sql, "select n_comment_count from tmp_comment where n_entry_id = %d", in_entry);	/* 8 �����ȿ� */
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "�ơ��޾ܺ٤����륯���꡼�˼��Ԥ��ޤ�����<br>%s<br>%s", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp){
			in_comment = atoi(chp_tmp);
		}
		Db_CloseDyna(dbres);
		sprintf(cha_sql, "select n_tb_count from tmp_trackback where n_entry_id = %d", in_entry);		/* 9 �ȥ�å��Хå��� */
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "�ơ��޾ܺ٤����륯���꡼�˼��Ԥ��ޤ�����<br>%s<br>%s", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp){
			in_trackback = atoi(chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	tm_now = time(NULL);
	stp_now = localtime(&tm_now);
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	/* �������ˤϡ�specify_date���ʤ� */
	if (!chp_tmp || atoi(chp_tmp) == 1) {
		chp_tmp = Get_Nlist(nlp_in, "hour", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
		} else {
			sprintf(cha_str, "%d", stp_now->tm_hour);
			Put_Nlist(nlp_out, "ARTICLE", cha_str);
		}
		Put_Nlist(nlp_out, "ARTICLE", "��");
		chp_tmp = Get_Nlist(nlp_in, "minute", 1);
		if (chp_tmp && *chp_tmp) {
			Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
		} else {
			sprintf(cha_str, "%d", stp_now->tm_min);
			Put_Nlist(nlp_out, "ARTICLE", cha_str);
		}
		Put_Nlist(nlp_out, "ARTICLE", "ʬ");
	} else {
		strftime(cha_str, strlen("HH��MMʬ") + 1, "%H��%Mʬ", stp_now);
		Put_Nlist(nlp_out, "ARTICLE", cha_str);
	}
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (chp_tmp && atoi(chp_tmp) == 1) {
		Put_Nlist(nlp_out, "ARTICLE", " | ");
		Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"#comment\">������(%d)</a>\n", in_comment);
	}
	chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
	if (chp_tmp && atoi(chp_tmp) == 1) {
		Put_Nlist(nlp_out, "ARTICLE", " | ");
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"#trackback\">���ѵ���(%d)</a>\n", in_trackback);
		} else {
			Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"#trackback\">�ȥ�å��Хå�(%d)</a>\n", in_trackback);
		}
	}
	Put_Nlist(nlp_out, "ARTICLE", "</div>\n");
	Put_Nlist(nlp_out, "ARTICLE", "</td>\n");
	Put_Nlist(nlp_out, "ARTICLE", "</tr>\n");
	Put_Nlist(nlp_out, "ARTICLE", "</table>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	put_data_error_page()
 * Description:		���顼���̤���������Υǡ�����������
 *
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
void put_data_error_page(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_button, char *chp_action)
{
	char *chpa_esc[] = {
		  "BTN_DELETE_FILE", "BTN_DELETE_FILE_ASK", "BTN_DISP_NEWENTRY", "BTN_INSERT_ORDER"
		, "BTN_EASY_ENTRY", "BTN_UPLOAD_FILE", "BTN_PREVIEW_DRAFT.x", "BTN_PREVIEW_DRAFT.y"
		, "BTN_BACK_FROM_ERROR.x", "BTN_BACK_FROM_ERROR.y", "BTN_EDIT_ENTRY.x", "BTN_EDIT_ENTRY.y"
		, "BTN_RESEND_TRACKBACK"
		, "btn_pass", "btn_id", "btn_login.x", "btn_login.y"
		, "file", "b", "l", "f", "t", "after1", "after2", "after3", "from_preview"
		, "related_url", NULL
	};
	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	Put_Nlist(nlp_out, "ACTION", chp_action);
	Put_Nlist(nlp_out, "BUTTON_NAME", chp_button);
	return;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_delete_attached_file_ask()
 * Description:
 *		ź�եե�����κ����ǧ����ɽ��
%* ------------------------------------------------------------------------
 * Return:
 *	���� 0
 *	���顼�� 1
-* ------------------------------------------------------------------------*/
int disp_delete_attached_file_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[1024];
	char cha_str[1024];
	int in_row;
	int in_checked_count;
	int i;

	/* �����å��ܥå����Υ����å� */
	in_checked_count = Get_NlistCount(nlp_in, "delete_file");
	if (!in_checked_count) {
		Put_Nlist(nlp_out, "ERROR", "�������ե���������򤷤Ƥ���������");
		return 1;
	}
	/* ����ե�����ξ���򥻥쥯�� */
	strcpy(cha_sql, "select T1.c_filename");	/* 0 �ե�����̾ */
	strcat(cha_sql, ", T1.c_filetype");			/* 1 �ե����륿���� */
	strcat(cha_sql, ", T1.n_uploadfile_id");	/* 2 �ե�����ID */
	strcat(cha_sql, ", T1.n_entry_id");			/* 3 ����ID */
	strcat(cha_sql, " from at_uploadfile T1");
	strcat(cha_sql, " where concat(T1.n_entry_id,':',T1.n_uploadfile_id) in (");
	for(i = 0; i < in_checked_count; i++) {
		if (i) {
			strcat(cha_sql, ", ");
		}
		strcat(cha_sql, "'");
		strcat(cha_sql, Get_Nlist(nlp_in, "delete_file", i + 1));
		strcat(cha_sql, "'");
	}
	strcat(cha_sql, ")");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�����ե���������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if (!in_row) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "���򤵤줿�ե�����ϴ��˺������Ƥ��ޤ���");
		return 1;
	}
	if (in_row < in_checked_count) {
		Put_Nlist(nlp_out, "MSG", "���򤵤줿�ե�����ΰ����ϴ��˺������Ƥ��ޤ���<br>");
	}
	Put_Nlist(nlp_out, "FILELIST", "<ul>\n");
	for(i = 0; i < in_row; i++) {
		memset(cha_str, '\0', sizeof(cha_str));
		strcpy(cha_str, "<li>");
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		}
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (chp_tmp && *chp_tmp) {
			sprintf(cha_str + strlen(cha_str), "(%s)", chp_tmp);
		}
		strcat(cha_str, "</li>\n");
		Put_Nlist(nlp_out, "FILELIST", cha_str);
		sprintf(cha_str, "%s:%s", Db_GetValue(dbres, i, 3), Db_GetValue(dbres, i, 2));
		Build_HiddenEncode(nlp_out, "HIDDEN", "delete_file", cha_str);
	}
	Put_Nlist(nlp_out, "FILELIST", "</ul>");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_delete_attached_movie_ask()
 * Description:
 *		ź�եե�����κ����ǧ����ɽ��
%* ------------------------------------------------------------------------
 * Return:
 *	���� 0
 *	���顼�� 1
-* ------------------------------------------------------------------------*/
int disp_delete_attached_movie_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_entry_id;	/* ����ID */
	char *chp_tmpfile_id;	/* ����Ͽ��ID */
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[1024];
	char cha_str[1024];
	int in_row;
	int i;

	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	/* ����ե�����ξ���򥻥쥯�� */
	strcpy(cha_sql, "select T1.c_filename");	/* 0 �ե�����̾ */
	strcat(cha_sql, ", T1.n_entry_id");			/* 1 ����ID */
	strcat(cha_sql, ", T1.c_origname");			/* 2 ���ꥸ�ʥ�ե�����̾ */
	strcat(cha_sql, " from at_uploadmovie T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	if (chp_entry_id || chp_tmpfile_id) {
		strcat(cha_sql, " and T1.n_entry_id in (");
		if (chp_entry_id) {
			strcat(cha_sql, chp_entry_id);
		}
		if (chp_entry_id && chp_tmpfile_id) {
			strcat(cha_sql, ", ");
		}
		if (chp_tmpfile_id) {
			strcat(cha_sql, chp_tmpfile_id);
		}
		strcat(cha_sql, ")");
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ư��ե���������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if (!in_row) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "���򤵤줿�ե�����ϴ��˺������Ƥ��ޤ���");
		return 1;
	}
	Put_Nlist(nlp_out, "FILELIST", "<ul>\n");
	for(i = 0; i < in_row; i++) {
		memset(cha_str, '\0', sizeof(cha_str));
		strcpy(cha_str, "<li>");
		chp_tmp = Db_GetValue(dbres, i, 2);
		if (chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		}
		strcat(cha_str, "</li>\n");
		Put_Nlist(nlp_out, "FILELIST", cha_str);
	}
	Put_Nlist(nlp_out, "FILELIST", "</ul>");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_attached_image()
 * Description:
 *	��ʸ���ź�եե�����ɽ���������롣
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
void delete_attached_image(char *chp_content, int in_blog, int in_entry, int in_upload)
{
	char cha_str[1024];
	char* chp_tag1;
	char* chp_tag2;
	char* chp_tmp;
	char* chp_copy;

	if (g_in_dbb_mode) {
		sprintf(cha_str, "%s/%s-%d-%d", CO_CGI_BUILD_HTML, g_cha_blog_temp, in_entry, in_upload);
	} else {
		sprintf(cha_str, "%s/%d-%d-%d", CO_CGI_BUILD_HTML, in_blog, in_entry, in_upload);
	}
	chp_copy = (char*)malloc(strlen(chp_content) + 1);

	chp_copy[0] = '\0';
	chp_tmp = chp_content;
	while (1) {
		int l;
		int n;
		chp_tag1 = strcasestr(chp_tmp, "<a ");
		if (!chp_tag1)
			break;
		l = chp_tag1 - chp_tmp;
		n = strlen(chp_copy);
		strncat(chp_copy, chp_tmp, l);
		chp_copy[n + l] = '\0';
		chp_tag2 = strcasestr(chp_tmp, "</a>");
		if (!chp_tag2) {
			chp_tmp = chp_tag1;
			break;
		}
		chp_tag2 += strlen("</a>");
		chp_tmp = strcasestr(chp_tag1, cha_str);
		if (!chp_tmp || chp_tmp >= chp_tag2) {
			l = chp_tag2 - chp_tag1;
			n = strlen(chp_copy);
			strncat(chp_copy, chp_tag1, l);
			chp_copy[n + l] = '\0';
		}
		chp_tmp = chp_tag2;
	}
	strcat(chp_copy, chp_tmp);
	strcpy(chp_content, chp_copy);
	free(chp_copy);

	chp_copy[0] = '\0';
	chp_tmp = chp_content;
	while (1) {
		int l;
		int n;
		chp_tag1 = strcasestr(chp_tmp, "<img ");
		if (!chp_tag1)
			break;
		l = chp_tag1 - chp_tmp;
		n = strlen(chp_copy);
		strncat(chp_copy, chp_tmp, l);
		chp_copy[n + l] = '\0';
		chp_tag2 = strcasestr(chp_tmp, ">");
		if (!chp_tag2) {
			chp_tmp = chp_tag1;
			break;
		}
		chp_tag2 += strlen(">");
		chp_tmp = strcasestr(chp_tag1, cha_str);
		if (!chp_tmp || chp_tmp >= chp_tag2) {
			l = chp_tag2 - chp_tag1;
			n = strlen(chp_copy);
			strncat(chp_copy, chp_tag1, l);
			chp_copy[n + l] = '\0';
		}
		chp_tmp = chp_tag2;
	}
	strcat(chp_copy, chp_tmp);
	strcpy(chp_content, chp_copy);
	free(chp_copy);
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_attached_file()
 * Description:
 *	ź�եե������ֺ��ͽ��פˤ��롣
 *	�ºݤ˺������Τϡ��������/�Խ�����
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
int delete_attached_file(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_sql[512];
	char *chp_tmp;
	char *chp_content;
	int in_count;
	int i;

	in_count = Get_NlistCount(nlp_in, "delete_file");
	if (!in_count) {
		Put_Nlist(nlp_out, "ERROR", "�ե����뤬����Ǥ��ޤ���");
		return 1;
	}
	strcpy(cha_sql, "update at_uploadfile");
	strcat(cha_sql, " set b_delete_mode = 1");
	strcat(cha_sql, " where concat(n_entry_id,':',n_uploadfile_id) in (");
	for(i = 0; i < in_count; i++) {
		if (i) {
			strcat(cha_sql, ", ");
		}
		strcat(cha_sql, "'");
		strcat(cha_sql, Get_Nlist(nlp_in, "delete_file", i + 1));
		strcat(cha_sql, "'");
	}
	strcat(cha_sql, ")");
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "���ͽ��˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_content = Get_Nlist(nlp_in, "entry_body", 1);
	if (chp_content) {
		for(i = 0; i < in_count; i++) {
			strcpy(cha_sql, Get_Nlist(nlp_in, "delete_file", i + 1));
			strtok(cha_sql, ":");
			chp_tmp = strtok(NULL, ":");
			delete_attached_image(chp_content, in_blog, atoi(cha_sql), atoi(chp_tmp));
		}
	}
	chp_content = Get_Nlist(nlp_in, "entry_more", 1);
	if (chp_content) {
		for(i = 0; i < in_count; i++) {
			strcpy(cha_sql, Get_Nlist(nlp_in, "delete_file", i + 1));
			strtok(cha_sql, ":");
			chp_tmp = strtok(NULL, ":");
			delete_attached_image(chp_content, in_blog, atoi(cha_sql), atoi(chp_tmp));
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_attached_movie()
 * Description:
 *	ư��ե������ֺ��ͽ��פˤ��롣
 *	�ºݤ˺������Τϡ��������/�Խ�����
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
int delete_attached_movie(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_entry_id;	/* ����ID */
	char *chp_tmpfile_id;	/* ����Ͽ��ID */
	char cha_sql[512];

	chp_entry_id = Get_Nlist(nlp_in, "entry_id", 1);
	chp_tmpfile_id = Get_Nlist(nlp_in, "tmpfile_id", 1);
	if (!chp_entry_id && !chp_tmpfile_id) {
		return 0;
	}
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "delete from at_uploadmovie");
	if (chp_entry_id || chp_tmpfile_id) {
		strcat(cha_sql, " where n_entry_id in (");
		if (chp_entry_id) {
			strcat(cha_sql, chp_entry_id);
		}
		if (chp_entry_id && chp_tmpfile_id) {
			strcat(cha_sql, ", ");
		}
		if (chp_tmpfile_id) {
			strcat(cha_sql, chp_tmpfile_id);
		}
		strcat(cha_sql, ")");
	}
#if 0
	strcpy(cha_sql, "update at_uploadmovie");
	strcat(cha_sql, " set b_delete_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " where n_entry_id = %s", Get_Nlist(nlp_in, "tmpfile_id", 1));
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
#endif
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "���ͽ��˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	delete_attached_file()
 * Description:
 *	ź�եե������ֺ��ͽ��פˤ��롣
 *	�ºݤ˺������Τϡ��������/�Խ�����
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
int display_attached_file(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char cha_num[512];
	char *chp_tmp;
	char *chp_content;
	char *chp_escape;
	int in_count;
	int i;

	in_count = Get_NlistCount(nlp_in, "delete_file");
	if (!in_count) {
		Put_Nlist(nlp_out, "ERROR", "�ե����뤬����Ǥ��ޤ���");
		return 1;
	}
	chp_content = Get_Nlist(nlp_in, "entry_body", 1);
	if (chp_content) {
		for(i = 0; i < in_count; i++) {
			strcpy(cha_sql, Get_Nlist(nlp_in, "delete_file", i + 1));
			strtok(cha_sql, ":");
			chp_tmp = strtok(NULL, ":");
			delete_attached_image(chp_content, in_blog, atoi(cha_sql), atoi(chp_tmp));
		}
	}
	chp_content = Get_Nlist(nlp_in, "entry_more", 1);
	if (chp_content) {
		for(i = 0; i < in_count; i++) {
			strcpy(cha_sql, Get_Nlist(nlp_in, "delete_file", i + 1));
			strtok(cha_sql, ":");
			chp_tmp = strtok(NULL, ":");
			delete_attached_image(chp_content, in_blog, atoi(cha_sql), atoi(chp_tmp));
		}
	}

	for(i = 0; i < in_count; i++) {
		strcpy(cha_num, Get_Nlist(nlp_in, "delete_file", i + 1));
		strtok(cha_num, ":");
		chp_tmp = strtok(NULL, ":");

		sprintf(cha_sql, "select c_filename,c_filetype from at_uploadfile where n_blog_id = %d and n_entry_id = %s and n_uploadfile_id = %s;", in_blog, cha_num, chp_tmp);
		dbres = Db_OpenDyna(db, cha_sql);
		if (dbres) {
			if (Db_FetchNext(dbres) == CO_SQL_OK) {
				if (g_in_dbb_mode) {
					if (Db_FetchValue(dbres, 1) && strstr(Db_FetchValue(dbres, 1), "image/")) {
						Put_Format_Nlist(nlp_out, "ENTRYBODY",
							"<a href=\"%s%s%s/%s/%s-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">"
							"<img src=\"%s%s%s/%s/%s-%d-%d?BTN_DISP_ATTACH_FILE=1\" width=\"160\" height=\"120\" border=\"0\">"
							"</a><br />",
							g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, atoi(cha_num), atoi(chp_tmp),
							g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, atoi(cha_num), atoi(chp_tmp));
					} else {
						chp_escape = Escape_HtmlString(Db_FetchValue(dbres, 0));
						Put_Format_Nlist(nlp_out, "ENTRYBODY",
							"<a href=\"%s%s%s/%s/%s-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">%s</a><br />",
							g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, atoi(cha_num), atoi(chp_tmp), chp_escape);
						free(chp_escape);
					}
				} else {
					if (Db_FetchValue(dbres, 1) && strstr(Db_FetchValue(dbres, 1), "image/")) {
						Put_Format_Nlist(nlp_out, "ENTRYBODY",
							"<a href=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">"
							"<img src=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" width=\"160\" height=\"120\" border=\"0\">"
							"</a><br />",
							g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, atoi(cha_num), atoi(chp_tmp),
							g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, atoi(cha_num), atoi(chp_tmp));
					} else {
						chp_escape = Escape_HtmlString(Db_FetchValue(dbres, 0));
						Put_Format_Nlist(nlp_out, "ENTRYBODY",
							"<a href=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">%s</a><br />",
							g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, atoi(cha_num), atoi(chp_tmp), chp_escape);
						free(chp_escape);
					}
				}
			}
			Db_CloseDyna(dbres);
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            disp_page_attachfile()
 * Description:
 *      �ץ�ե�����β�����ɽ������
%* ------------------------------------------------------------------------
 * Return:                      ���ｪλ 0
 *      �����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int disp_page_attachfile(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[4096];
	char* chp_upload;
	int in_upload;
	int in_entry;

	in_entry = 0;
	in_upload = 0;
	chp_upload = getenv("PATH_INFO");
	if (!chp_upload || sscanf(chp_upload, "/%d-%d", &in_entry, &in_upload) < 2) {
		in_entry = 0;
		in_upload = 0;
	}
	if (!in_upload) {
		chp_upload = Get_Nlist(nlp_in, "uploadid", 1);
		if (!chp_upload) {
			return 0;
		} else {
			in_upload = atoi(chp_upload);
		}
		chp_upload = Get_Nlist(nlp_in, "entry_id", 1);
		if (!chp_upload) {
			return 0;
		} else {
			in_entry = atoi(chp_upload);
		}
	}
	sprintf(cha_sql, "select c_fileimage,c_filetype,n_filesize from at_uploadfile where n_blog_id = %d and n_entry_id = %d and n_uploadfile_id = %d;", in_blog, in_entry, in_upload);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		return 0;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		return 0;
	}
	if (!Db_FetchValue(dbres, 0) || !Db_FetchValue(dbres, 1) || !Db_FetchValue(dbres, 2)) {
		return 0;
	}
	fprintf(stdout, "Content-Type: %s\n\n", Db_FetchValue(dbres, 1));
	fwrite(Db_FetchValue(dbres, 0), atoi(Db_FetchValue(dbres, 2)), 1, stdout);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            resend_trackback()
 * Description:
 *      �����˼��Ԥ����ȥ�å��Хå������������
%* ------------------------------------------------------------------------
 * Return:
 *      ���ｪλ 0
 *      ���顼�� 1
-* ------------------------------------------------------------------------*/
int resend_trackback(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_upload;
	int in_err;
	int in_entry;
	int in_tb;

	in_err = 0;
	in_tb = 0;
	in_entry = 0;
	chp_upload = Get_Nlist(nlp_in, "entry_id", 1);
	if (!chp_upload) {
		Put_Nlist(nlp_out, "ERROR", "����������Ǥ��ޤ���");
		++in_err;
	} else {
		in_entry = atoi(chp_upload);
	}
	chp_upload = Get_Nlist(nlp_in, "tbid", 1);
	if (!chp_upload) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ���������Ǥ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå�������Ǥ��ޤ���");
		}
		++in_err;
	} else {
		in_tb = atoi(chp_upload);
	}
	if (in_err)
		return in_err;

	sprintf(cha_sql,
		" select"
		" c_tb_url"
		" from"
		" at_sendtb"
		" where n_blog_id = %d"
		" and n_entry_id = %d"
		" and n_tb_id = %d"
		, in_blog, in_entry, in_tb);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ��������륯����˼��Ԥ��ޤ�����<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå������륯����˼��Ԥ��ޤ�����<br>");
		}
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s", Gcha_last_error, cha_sql);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿���ѵ�����¸�ߤ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR", "���򤵤줿�ȥ�å��Хå���¸�ߤ��ޤ���");
		}
		return 1;
	}
	in_err = trackback(db, nlp_in, nlp_out, in_entry, Db_GetValue(dbres, 0, 0), in_blog, in_tb, NULL);
	Db_CloseDyna(dbres);

	return in_err;
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
int main(void)
{
	NLIST *nlp_in;
	NLIST *nlp_out;
	DBase *db;
	char *chp_skel;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_button;
	char *chp_fromuser;
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char cha_location[256];
	char cha_blog[32];
	int in_blog;
	int in_orig;
	int in_error;
	int in_black_list = 0;

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
	if (!db){
		printf("\n\n�ǡ����١�����³�˼��Ԥ��ޤ�����");
		return 1;
	}
	if (Read_Blog_Option(db)) {
		printf("\n\n�֥����ץ������ɤ߹��ߤ˼��Ԥ��ޤ�����");
		Db_Disconnect(db);
		return 1;
	}
	/* �����ե��������¸��� */
	Set_SavePath(g_cha_upload_path);
	Set_SkelPath(g_cha_admin_skeleton);
	/* �ե����륵�������� kbñ�� */
	Set_MaxFileSize(CO_MAX_UPLOAD_SIZE * 1024 * 200);

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	Put_Nlist(nlp_out, "CSS", g_cha_css_location);
	Put_Nlist(nlp_out, "SCRIPTS", g_cha_script_location);
	Put_Nlist(nlp_out, "IMAGES", g_cha_admin_image);
	if (g_in_html_editor) {
		Put_Nlist(nlp_out, "ONLOAD", "OnLoad();");
	}
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
		if (Get_Nlist(nlp_in, CO_DISP_REMIND, 1)) {
			Disp_Remind_Page(db, nlp_in, nlp_out, CO_CGI_ENTRY);
			Page_Out(nlp_out, CO_SKEL_REMIND);
			goto clear_finish;
		} else if (Get_Nlist(nlp_in, CO_LOGIN_ID, 1) && Get_Nlist(nlp_in, CO_PASSWORD, 1)) {
			in_blog = Auth_Login(db, nlp_in, nlp_out, CO_LOGIN_ID, CO_PASSWORD, &g_in_login_owner);
			if (in_blog < 0) {
				Put_Nlist(nlp_out, "ERR_START", "-->");
				Put_Nlist(nlp_out, "ERR_END", "<!--");
				Page_Out(nlp_out, CO_SKEL_LOGIN);
				goto clear_finish;
			} else if (!in_blog) {
				put_data_error_page(db, nlp_in, nlp_out, "", CO_CGI_ENTRY);
				chp_tmp = Get_Nlist(nlp_in, "BTN_DISP_NEWENTRY", 1);
				if (chp_tmp)
					Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_NEWENTRY", chp_tmp);
				chp_tmp = Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1);
				if (chp_tmp)
					Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_EASY_ENTRY", chp_tmp);
				chp_tmp = Get_Nlist(nlp_in, "b", 1);
				if (chp_tmp)
					Build_Hidden(nlp_out, "HIDDEN", "b", chp_tmp);
				chp_tmp = Get_Nlist(nlp_in, "l", 1);
				if (chp_tmp)
					Build_HiddenEncode(nlp_out, "HIDDEN", "l", chp_tmp);
				chp_tmp = Get_Nlist(nlp_in, "f", 1);
				if (chp_tmp)
					Build_HiddenEncode(nlp_out, "HIDDEN", "f", chp_tmp);
				chp_tmp = Get_Nlist(nlp_in, "t", 1);
				if (chp_tmp)
					Build_HiddenEncode(nlp_out, "HIDDEN", "t", chp_tmp);
				chp_tmp = Get_Nlist(nlp_in, "o", 1);
				if (chp_tmp)
					Build_HiddenEncode(nlp_out, "HIDDEN", "o", chp_tmp);
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
					Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "ǧ�ھ��������Ǥ��ޤ���<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_ENTRY);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�����Ԥˤ����������ӥ��Τ����Ѥ���ߤ��Ƥ���ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_ENTRY);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥����Խ����븢�¤�����ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", "/cgi-bin/dbb_blog_list.cgi");
		Put_Nlist(nlp_out, "ERROR", "�֥����������Ƥ��ʤ����Ϥ����Ѥˤʤ�ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);
	in_black_list = (g_in_dbb_mode && In_Black_List(db, nlp_out, g_in_login_owner, NULL));
	if (Need_Login(db, in_blog)) {
		Put_Nlist(nlp_out, "PING_START", "<!--");
		Put_Nlist(nlp_out, "PING_END", "-->");
	}
	if (!g_in_dbb_mode) {
		Put_Nlist(nlp_out, "ORDER_START", "<!--");
		Put_Nlist(nlp_out, "ORDER_END", "-->");
	} else {
		Put_Nlist(nlp_out, "LINK_START", "<!--");
		Put_Nlist(nlp_out, "LINK_END", "-->");
	}
	if (!g_in_cart_mode) {
		Put_Nlist(nlp_out, "ITEM_START", "<!--");
		Put_Nlist(nlp_out, "ITEM_END", "-->");
	}

	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_fromuser && atoi(chp_fromuser)) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_fromuser);
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
		Put_Format_Nlist(nlp_out, "NEXTCGI", "%s?blogid=%d", CO_CGI_MENU, in_blog);
		Put_Nlist(nlp_out, "TOP", "������˥塼");
		Put_Nlist(nlp_out, "GIF", "go_kanri.gif");
	}

	if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
		chp_tmp = Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1);
		if (chp_tmp)
			Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_EASY_ENTRY", chp_tmp);
		chp_tmp = Get_Nlist(nlp_in, "b", 1);
		if (chp_tmp)
			Build_Hidden(nlp_out, "HIDDEN", "b", chp_tmp);
		chp_tmp = Get_Nlist(nlp_in, "l", 1);
		if (chp_tmp) {
			Build_HiddenEncode(nlp_out, "HIDDEN", "l", chp_tmp);
//			if (g_in_dbb_mode) {
//				Build_HiddenEncode(nlp_out, "HIDDEN", "related_url", chp_tmp);
//			}
		}
		chp_tmp = Get_Nlist(nlp_in, "f", 1);
		if (chp_tmp)
			Build_HiddenEncode(nlp_out, "HIDDEN", "f", chp_tmp);
		chp_tmp = Get_Nlist(nlp_in, "t", 1);
		if (chp_tmp)
			Build_HiddenEncode(nlp_out, "HIDDEN", "t", chp_tmp);
		chp_tmp = Get_Nlist(nlp_in, "o", 1);
		if (chp_tmp)
			Build_HiddenEncode(nlp_out, "HIDDEN", "o", chp_tmp);
	}

	/* BlogName�����ߡʥ��åץǡ��ȡ��ȥ�å��Хå���) */
	put_blog_name(db, nlp_out, in_blog);	/* BLOGNAME */
	/* ����� */
	in_error = 0;
	chp_skel = NULL;
	chp_button = "BTN_BACK_FROM_ERROR.x";
	/* ź�եե������� */
	if (Get_Nlist(nlp_in, "BTN_DELETE_FILE", 1)){
		if (delete_attached_file(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		/* �����κ����� */
		if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	/* ź�եե���������ǧ */
	} else if (Get_Nlist(nlp_in, "BTN_DELETE_FILE_ASK", 1)) {
		if (disp_delete_attached_file_ask(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
				in_error++;
			}
			chp_skel = CO_SKEL_EDITENTRY;
		} else {
			/* ���ܰ��Ѥ� */
			put_data_error_page(db, nlp_in, nlp_out, chp_button, CO_CGI_ENTRY);
			Put_Nlist(nlp_out, "BTN_NAME", "BTN_DELETE_FILE");
			chp_skel = CO_SKEL_DELETE_FILE_ASK;
		}
	/* ư��ե������� */
	} else if (Get_Nlist(nlp_in, "BTN_DELETE_MOVIE", 1)){
		if (delete_attached_movie(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		/* �����κ����� */
		if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	/* ư��ե���������ǧ */
	} else if (Get_Nlist(nlp_in, "BTN_DELETE_MOVIE_ASK", 1)) {
		if (disp_delete_attached_movie_ask(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
				in_error++;
			}
			chp_skel = CO_SKEL_EDITENTRY;
		} else {
			/* ���ܰ��Ѥ� */
			put_data_error_page(db, nlp_in, nlp_out, chp_button, CO_CGI_ENTRY);
			Put_Nlist(nlp_out, "BTN_NAME", "BTN_DELETE_MOVIE");
			chp_skel = CO_SKEL_DELETE_FILE_ASK;
		}
	/* ź�եե�����ɽ�� */
	} else if (Get_Nlist(nlp_in, "BTN_DISPLAY_FILE", 1)){
		if (display_attached_file(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		/* ɽ����κ����� */
		if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	} else if (Get_Nlist(nlp_in, "BTN_INSERT_ORDER", 1)) {
		NLIST *nlp_tmp = Init_Nlist();
		if (Disp_Team_Player(db, nlp_in, nlp_tmp, nlp_out, "TEMP", "team")) {
			in_error++;
		} else {
			/* ���åץ��ɸ�κ����� */
			chp_tmp = Page_Out_Mem(nlp_tmp, "blog_temp.skl");
			Put_Nlist(nlp_out, "PREVIEW", chp_tmp);
			chp_tmp2 = Escape_HtmlString(chp_tmp);
			Put_Nlist(nlp_out, "ENTRYMORE", chp_tmp2);
			if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 0)) {
				in_error++;
			}
			free(chp_tmp);
			free(chp_tmp2);
			chp_skel = CO_SKEL_EDITENTRY;
		}
		Finish_Nlist(nlp_tmp);
	} else if (Get_Nlist(nlp_in, "BTN_PREVIEW_DRAFT.x", 1)) {
		if (preview_draft(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
				in_error++;
			}
			chp_skel = CO_SKEL_EDITENTRY;
		} else {
			/* ���顼���̤�Ʊ���ܤ�ץ�ӥ塼������ */
			put_data_error_page(db, nlp_in, nlp_out, chp_button, CO_CGI_ENTRY);
			Put_Nlist(nlp_out, "BUTTON", "BTN_BACK_FROM_ERROR");
			chp_skel = CO_SKEL_PREVIEW;
		}
	/* ������Ͽ/���� */
	} else if (Get_Nlist(nlp_in, "BTN_EDIT_ENTRY.x", 1)) {
		int in_editmode = CO_EDITMODE_NEW;
		char *chp_mess = "";
		chp_tmp = Get_Nlist(nlp_in, "editmode", 1);
		if (chp_tmp) {
			in_editmode = atoi(chp_tmp);
		}
		chp_tmp2 = Get_Nlist(nlp_in, "entry_id", 1);
		if (in_black_list) {
			Put_Nlist(nlp_out, "ERROR", "���ߡ��񤭹��ߤ��ػߤ���Ƥ��ޤ���");
			in_error++;
		} else if (in_editmode == CO_EDITMODE_NEW) {
			if (insert_new_entry(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
			if (chp_fromuser) {
				if (g_in_dbb_mode) {
					sprintf(cha_location, "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
				} else if (g_in_short_name) {
					sprintf(cha_location, "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
				} else {
					sprintf(cha_location, "%s%s%s/%s/%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
				}
			} else {
				sprintf(cha_location, "%s%s%s/%s?blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
			}
			chp_mess = "��������Ƥ��ޤ�����";
		} else if (in_editmode == CO_EDITMODE_EDIT && chp_tmp2) {
			if (modify_posted_entry(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			}
			if (chp_fromuser && atoi(chp_fromuser)) {
				sprintf(cha_location, "%s%s%s/%s?blogid=%d&entry_id=%s&from_user=1", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY_LIST, in_blog, chp_tmp2);
			} else {
				sprintf(cha_location, "%s%s%s/%s?blogid=%d&entry_id=%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY_LIST, in_blog, chp_tmp2);
			}
			chp_mess = "�������Խ����ޤ�����";
		} else {
			Put_Nlist(nlp_out, "ERROR", "�Խ��⡼�ɤ���äƤ��ޤ���");
			in_error++;
		}
		if (!in_error) {
			if (Create_Rss(db, nlp_out, in_blog)) {
				in_error++;
			} else {
				Put_Nlist(nlp_out, "MESSAGE", chp_mess);
				Put_Nlist(nlp_out, "RETURN", cha_location);
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
		} else {
			re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1);
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			chp_skel = CO_SKEL_EDITENTRY;
			in_error = 0;
		}
	} else if (Get_Nlist(nlp_in, "BTN_BACK_FROM_ERROR.x", 1)){
		/* ���顼���̤⤷���ϥץ�ӥ塼��������� */
		re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1);
		chp_skel = CO_SKEL_EDITENTRY;
	/* ������Ͽ����ɽ�� */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_NEWENTRY", 1) || Get_Nlist(nlp_in, "BTN_DISP_NEWENTRY.x", 1) || Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
		if (disp_page_newentry(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
		if (Get_Nlist(nlp_in, "BTN_EASY_ENTRY", 1)) {
			chp_skel = CO_SKEL_EDITENTRY_BUFFER;
		} else {
			chp_skel = CO_SKEL_EDITENTRY;
		}
	} else if (Get_Nlist(nlp_in, "BTN_COPY_ENTRY.x", 1)) {
		Remove_Nlist(nlp_in, "editmode", 1);
		if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	} else if (Get_Nlist(nlp_in, "BTN_UPLOAD_FILE", 1)) {
		if (temporary_register_uploadfile(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		/* ���åץ��ɸ�κ����� */
		if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	} else if (Get_Nlist(nlp_in, "BTN_UPLOAD_MOVIE", 1)) {
		if (temporary_register_uploadmovie(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			/* ���åץ��ɸ�κ����� */
			if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
				in_error++;
			}
			chp_skel = CO_SKEL_EDITENTRY;
		} else {
			chp_skel = NULL;
		}
	} else if (Get_Nlist(nlp_in, "BTN_RE_DISP", 1)) {
		/* ���åץ��ɸ�κ����� */
		if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	} else if (Get_Nlist(nlp_in, "BTN_DISP_MODIFYENTRY", 1)) {
		if (disp_page_editentry(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	} else if (Get_Nlist(nlp_in, "BTN_DISP_ATTACH_FILE", 1)) {
		disp_page_attachfile(db, nlp_in, nlp_out, in_blog);
	} else if (Get_Nlist(nlp_in, "BTN_RESEND_TRACKBACK", 1)) {
		if (resend_trackback(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
		/* �����κ����� */
		if (re_disp_page_entry(db, nlp_in, nlp_out, in_blog, 1)) {
			in_error++;
		}
		chp_skel = CO_SKEL_EDITENTRY;
	} else {
		put_data_error_page(db, nlp_in, nlp_out, chp_button, CO_CGI_MENU);
		Put_Nlist(nlp_out, "ERROR", "ľ��CGI��¹ԤǤ��ޤ���");
		chp_skel =  CO_SKEL_ERROR;
	}
	/* ���顼������Ȥ��ϡ����顼������ȥ�˽��ϡ� */
	/* ���顼���ʤ������ĥ�����ȥ����ꤵ��Ƥ��ʤ����ϡ���������������Ǥ���Τǲ��⤷�ʤ��� */
	if (in_error) {
		put_data_error_page(db, nlp_in, nlp_out, chp_button, CO_CGI_ENTRY);
		Page_Out(nlp_out, CO_SKEL_ERROR);
	} else if (chp_skel) {
		Page_Out(nlp_out, chp_skel);
	}
	fflush(stdout);

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
