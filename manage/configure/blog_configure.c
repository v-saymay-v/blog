/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_configure.c
 * First-Created:	2004/08/12 ��¼ ����Ϻ
%* ------------------------------------------------------------------------
 * Module-Description:
 *
 *
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_configure.c,v 1.95 2008/07/14 07:55:18 hori Exp $";

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
 *	���顼���̤�������
%* ------------------------------------------------------------------------
 * Return:                      ���ｪλ 0
 *      �����          ���顼�� 1
-* ------------------------------------------------------------------------*/
void put_error_data(NLIST *nlp_in, NLIST *nlp_out)
{
	char *chpa_esc[] = {
		"blogid", "BTN_DISP_CONFIGURE", "BTN_EDIT_ABOUT_BLOG", "BTN_CONFIGURE.x", "BTN_CONFIGURE.y", "BTN_CONFIGURE_ABOUT_BLOG", NULL
	};

	Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
	Build_HiddenEncode(nlp_out, "HIDDEN", "from_error", "1");
	Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
	return ;
}
/*
+* ------------------------------------------------------------------------
 * Function:            disp_page_configure_from_error()
 * Description:
 *	���顼���̤��������
%* ------------------------------------------------------------------------
 * Return:                      ���ｪλ 0
 *      �����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int disp_page_configure_from_error(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_sql[1024];
	char cha_msg[256];
	int in_count;
	int in_checked_count;
	int in_checked_flg;
	int i;
	int j;

	/* �ԥ󥰥ꥹ�Ȥ��� */
	sprintf(cha_sql,
		" select"
			" T1.n_ping_id"		/* 0 �ԥ�ID */
			",T1.c_ping_site"	/* 1 ping������̾ */
			",T2.b_default"		/* 2 ����� */
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
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ԥ󥰽���ͤ����륯����˼��Ԥ��ޤ���");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	in_checked_count = Get_NlistCount(nlp_in, "default_prepared_ping");
	for(i = 0; i < in_count; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		sprintf(cha_msg, "default_ping%s", chp_tmp);
		chp_escape = Escape_HtmlString(Db_GetValue(dbres, i, 1));
		in_checked_flg = 0;
		/* ���顼�������ϡ������å�����Ƥ�����Τ�����å� */
		for(j = 0; j < in_checked_count; j++) {
			chp_tmp2 = Get_Nlist(nlp_in, "default_prepared_ping", j + 1);
			if(chp_tmp && chp_tmp2 && strcmp(chp_tmp, chp_tmp2) == 0) {
				in_checked_flg = 1;
				break;
			}
		}
		Build_Checkbox_With_Id(nlp_out, "DEFAULT_PREPARED_PING", "default_prepared_ping"
				       , chp_tmp, in_checked_flg, cha_msg, chp_escape);
		Put_Nlist(nlp_out, "DEFAULT_PREPARED_PING", "<br>\n");
		free(chp_escape);
	}
	Db_CloseDyna(dbres);
	/* ����ͥ��ƥ��꡼�������*/
	/* ���쥯�ȥܥå����Ȥ�Ω�� */
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select T1.n_category_id");	/* 0 ���ƥ���ID */
	strcat(cha_sql, ", T1.c_category_name");
	strcat(cha_sql, " from sy_category T1");
	/* ���顼��������nlp_in������*/
	if (!g_in_dbb_mode) {
		chp_tmp = Get_Nlist(nlp_in, "default_category", 1);
		if(chp_tmp && *chp_tmp) {
			Build_ComboDb(OldDBase(db), nlp_out, "DEFAULT_CATEGORY", cha_sql, "default_category", NULL, NULL, chp_tmp);
		} else {
			Build_ComboDb(OldDBase(db), nlp_out, "DEFAULT_CATEGORY", cha_sql, "default_category", NULL, NULL, "");
		}
	} else {
		Put_Nlist(nlp_out, "CATEGORY_START", "<!--");
		Put_Nlist(nlp_out, "CATEGORY_END", "-->");
		Build_Hidden(nlp_out, "HIDDEN", "default_category", "0");
	}
	/* �֥������ȥ� */
	chp_tmp = Get_Nlist(nlp_in, "blog_title", 1);
	if(chp_tmp){
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "BLOG_TITLE", chp_escape);
		free(chp_escape);
	}
	/* ���֥����ȥ� */
	chp_tmp = Get_Nlist(nlp_in, "blog_subtitle", 1);
	if(chp_tmp){
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "BLOG_SUBTITLE", chp_escape);
		free(chp_escape);
	}
	/* ���� */
	chp_tmp = Get_Nlist(nlp_in, "blog_description", 1);
	if(chp_tmp){
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "BLOG_DESCRIPTION", chp_escape);
		free(chp_escape);
	}
	/* DBB�Τ� ��ͧ�Τ߸��� */
	if (g_in_dbb_mode) {
		chp_tmp = Get_Nlist(nlp_in, "open_friend_only", 1);
		if(chp_tmp && atoi(chp_tmp)) {
			Put_Nlist(nlp_out, "OPEN_FRIEND_ONLY", CO_CHECKED);
		}
	}
	/* ɽ��������������� */
	chp_tmp = Get_Nlist(nlp_in, "toppage_disp_type", 1);
	if(chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "DISP_TYPE_SELECTED_DAY", CO_CHECKED);
	} else {
		Put_Nlist(nlp_out, "DISP_TYPE_SELECTED_INDEX", CO_CHECKED);
	}
	/* ɽ����� */
	chp_tmp = Get_Nlist(nlp_in, "latest_index", 1);
	if(chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "LATEST_INDEX", chp_escape);
		free(chp_escape);
	}
	/* ɽ������ */
	chp_tmp = Get_Nlist(nlp_in, "latest_day", 1);
	if(chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "LATEST_DAY", chp_escape);
		free(chp_escape);
	}
	/* ɽ��ʸ���� */
	chp_tmp = Get_Nlist(nlp_in, "cut_length", 1);
	if(chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "CUT_LENGTH", chp_escape);
		free(chp_escape);
	}
	/* �ȥ�å��Хå������ */
	chp_tmp = Get_Nlist(nlp_in, "default_trackback", 1);
	if(chp_tmp) {
		memset(cha_msg, '\0', sizeof(cha_msg));
		sprintf(cha_msg, "TRACKBACK%s", chp_tmp);
	} else {
		strcpy(cha_msg, "TRACKBACK1");
	}
	Put_Nlist(nlp_out, cha_msg, CO_CHECKED);
	/* �����Ƚ���� */
	chp_tmp = Get_Nlist(nlp_in, "default_comment", 1);
	if(chp_tmp) {
		memset(cha_msg, '\0', sizeof(cha_msg));
		sprintf(cha_msg, "COMMENT%s", chp_tmp);
	} else {
		strcpy(cha_msg, "COMMENT1");
	}
	Put_Nlist(nlp_out, cha_msg, CO_CHECKED);
	/* �����⡼�ɽ���� */
	chp_tmp = Get_Nlist(nlp_in, "default_mode", 1);
	if(chp_tmp) {
		memset(cha_msg, '\0', sizeof(cha_msg));
		sprintf(cha_msg, "MODE%s", chp_tmp);
	} else {
		strcpy(cha_msg, "MODE1");
	}
	Put_Nlist(nlp_out, cha_msg, CO_CHECKED);
	/* ping���������� */
	chp_tmp = Get_Nlist(nlp_in, "default_other_ping", 1);
	if(chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_Inputarea(nlp_out, "DEFAULT_OTHER_PING", "default_other_ping"
				, chp_escape, CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
		free(chp_escape);
	} else {
		Build_Inputarea(nlp_out, "DEFAULT_OTHER_PING", "default_other_ping"
				, "", CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
	}
	/* �����ȶػ�IP */
	if (g_in_need_login) {
		if (g_in_hb_mode) {
			Put_Nlist(nlp_out, "CMTHOST1", "���С�");
			Put_Nlist(nlp_out, "CMTHOST2", "���С�̾");
		} else {
			Put_Nlist(nlp_out, "CMTHOST1", "�桼����");
			Put_Nlist(nlp_out, "CMTHOST2", "�˥å��͡���");
		}
	} else {
		Put_Nlist(nlp_out, "CMTHOST1", "�ۥ���");
		Put_Nlist(nlp_out, "CMTHOST2", "IP���ɥ쥹�⤷���ϥۥ���̾");
	}
	chp_tmp = Get_Nlist(nlp_in, "denial_comment_host", 1);
	if(chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_Inputarea(nlp_out, "DENIAL_COMMENT_HOST", "denial_comment_host"
				, chp_escape, CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
		free(chp_escape);
	} else {
		Build_Inputarea(nlp_out, "DENIAL_COMMENT_HOST", "denial_comment_host"
				, "", CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
	}
	/* �ȥ�å��Хå��ػ�IP */
	chp_tmp = Get_Nlist(nlp_in, "denial_trackback_host", 1);
	if(chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_Inputarea(nlp_out, "DENIAL_TRACKBACK_HOST", "denial_trackback_host"
				, chp_escape, CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
		free(chp_escape);
	} else {
		Build_Inputarea(nlp_out, "DENIAL_TRACKBACK_HOST", "denial_trackback_host"
				, "", CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
	}

	chp_tmp = Get_Nlist(nlp_in, "use_url_filter", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "URLFILTER", "checked");
	}
	chp_tmp = Get_Nlist(nlp_in, "url_filter", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "URL_FILTER", chp_tmp);
	}

	chp_tmp = Get_Nlist(nlp_in, "use_blog_filter", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "BLOGFILTER", "checked");
	}
	chp_tmp = Get_Nlist(nlp_in, "blog_filter", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "BLOG_FILTER", chp_tmp);
	}

	chp_tmp = Get_Nlist(nlp_in, "use_contents_filter", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "CONTENTSFILTER", "checked");
	}
	chp_tmp = Get_Nlist(nlp_in, "contents_filter", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "CONTENTS_FILTER", chp_tmp);
	}

	chp_tmp = Get_Nlist(nlp_in, "use_entry_filter", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "ENTRYFILTER", "checked");
	}

	chp_tmp = Get_Nlist(nlp_in, "use_comment_filter", 1);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "COMMENTFILTER", "checked");
	}
	chp_tmp = Get_Nlist(nlp_in, "comment_filter", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "COMMENT_FILTER", chp_tmp);
	}

	if (g_in_cart_mode == CO_CART_SHOPPER) {
		Put_Nlist(nlp_out, "ITEMINFO_START", "-->");
		Put_Nlist(nlp_out, "ITEMINFO_END", "<!--");
	}
	chp_tmp = Get_Nlist(nlp_in, "mail_subject", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "MAIL_SUBJECT", chp_tmp);
	}
	chp_tmp = Get_Nlist(nlp_in, "mail_body", 1);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "MAIL_BODY", chp_tmp);
	}

	if(Get_Nlist(nlp_in, "BTN_EDIT_ABOUT_BLOG", 1)) {
		Build_Submit(nlp_out, "BUTTON_CONFIGURE", "BTN_CONFIGURE_ABOUT_BLOG", "��¸���ƥ����ɥС�������̤�");
	} else {
		Build_Submit(nlp_out, "BUTTON_CONFIGURE", "BTN_CONFIGURE", "���ߡ����ꡡ");
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	disp_page_configure()
 * Description:
 *	�������ɽ��
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int disp_page_configure(DBase *db, NLIST *nlp_in, NLIST* nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_sql[1024];
	char cha_msg[256];
	int in_filter;
	int in_valid;
	int in_count;
	int in_checked_flg;
	int i;

	/* �ԥ󥰥ꥹ�Ȥ��� */
	sprintf(cha_sql,
		" select"
			" T1.n_ping_id"		/* 0 �ԥ�ID */
			",T1.c_ping_site"	/* 1 ping������̾ */
			",T2.b_default"		/* 2 ����� */
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
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ԥ󥰽���ͤ����륯����˼��Ԥ��ޤ���");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	for(i = 0; i < in_count; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		sprintf(cha_msg, "default_ping%s", chp_tmp);
		chp_escape = Escape_HtmlString(Db_GetValue(dbres, i, 1));
		in_checked_flg = 0;
		chp_tmp2 = Db_GetValue(dbres, i, 2);
		if(chp_tmp2 && atoi(chp_tmp2)) {
			in_checked_flg = 1;
		}
		Build_Checkbox_With_Id(nlp_out, "DEFAULT_PREPARED_PING", "default_prepared_ping"
				       , chp_tmp, in_checked_flg, cha_msg, chp_escape);
		Put_Nlist(nlp_out, "DEFAULT_PREPARED_PING", "<br>\n");
		free(chp_escape);
	}
	Db_CloseDyna(dbres);

	if (!g_in_dbb_mode) {
		/* ����ͥ��ƥ��꡼�������*/
		sprintf(cha_sql,
			" select T1.n_blog_category"
			" from at_blog T1"
			" where T1.n_blog_id = %d",
			in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "���ƥ��꡼����ͤ����륯����˼��Ԥ��ޤ�����");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		if(Db_GetRowCount(dbres) && Db_GetValue(dbres, 0, 0)) {
			strcpy(cha_msg, Db_GetValue(dbres, 0, 0));
		} else {
			cha_msg[0] = '\0';
		}
		Db_CloseDyna(dbres);
		/* ���쥯�ȥܥå����Ȥ�Ω�� */
		strcpy(cha_sql, "select T1.n_category_id");	/* 0 ���ƥ���ID */
		strcat(cha_sql, ", T1.c_category_name");
		strcat(cha_sql, " from at_category T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		Build_ComboDb(OldDBase(db), nlp_out, "DEFAULT_CATEGORY", cha_sql, "default_category", NULL, NULL, cha_msg);
	} else {
		Put_Nlist(nlp_out, "CATEGORY_START", "<!--");
		Put_Nlist(nlp_out, "CATEGORY_END", "-->");
		Build_Hidden(nlp_out, "HIDDEN", "default_category", "0");
	}

	/* at_blog������ */
	strcpy(cha_sql, "select T1.c_blog_title");			/* 0 �֥������ȥ� */
	strcat(cha_sql, ", T1.c_blog_subtitle");			/* 1 �֥����֥����ȥ� */
	strcat(cha_sql, ", T1.c_blog_description");			/* 2 �֥����� */
	strcat(cha_sql, ", T1.b_setting_toppage_disptype");	/* 3 ������� or ������ */
	strcat(cha_sql, ", T1.n_setting_toppage_index");	/* 4 ɽ����� */
	strcat(cha_sql, ", T1.n_setting_toppage_day");		/* 5 ɽ������ */
	strcat(cha_sql, ", T1.b_default_trackback");		/* 6 �ȥ�å��Хå������ */
	strcat(cha_sql, ", T1.b_default_comment");			/* 7 �����Ƚ���� */
	strcat(cha_sql, ", T1.b_default_mode");				/* 8 �����⡼�ɽ���� */
	strcat(cha_sql, ", T1.n_setting_cut_length");		/* 9��...³�����ɤ�פ�ɽ������ʸ���� */
	strcat(cha_sql, ", T1.c_mail_subject");				/* 10 �����Ȥ�ȥ�å��Хå������ä��Ȥ���������᡼���̾ */
	strcat(cha_sql, ", T1.c_mail_body");				/* 11 �����Ȥ�ȥ�å��Хå������ä��Ȥ���������᡼����ʸ */
	if (g_in_dbb_mode) {
		strcat(cha_sql, ", T1.b_setting_friends_only");	/* ��ͧ�ˤΤ߸��� */
	}
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	/* �֥������ȥ� */
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp){
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "BLOG_TITLE", chp_escape);
		free(chp_escape);
	}
	/* ���֥����ȥ� */
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if(chp_tmp){
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "BLOG_SUBTITLE", chp_escape);
		free(chp_escape);
	}
	/* ���� */
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if(chp_tmp){
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "BLOG_DESCRIPTION", chp_escape);
		free(chp_escape);
	}
	/* ɽ��������������� */
	chp_tmp = Db_GetValue(dbres, 0, 3);
	if(chp_tmp) {
		/* ������ */
		if(atoi(chp_tmp) == 0) {
			Put_Nlist(nlp_out, "DISP_TYPE_SELECTED_INDEX", CO_CHECKED);
			chp_tmp = Db_GetValue(dbres, 0, 4);
			if(chp_tmp) {
				Put_Nlist(nlp_out, "LATEST_INDEX", chp_tmp);
			}
		/* ������� */
		} else {
			Put_Nlist(nlp_out, "DISP_TYPE_SELECTED_DAY", CO_CHECKED);
			chp_tmp = Db_GetValue(dbres, 0, 5);
			if(chp_tmp){
				Put_Nlist(nlp_out, "LATEST_DAY", chp_tmp);
			}
		}
	}
	/* �ȥ�å��Хå������ */
	chp_tmp = Db_GetValue(dbres, 0, 6);
	if(chp_tmp) {
		memset(cha_msg, '\0', sizeof(cha_msg));
		sprintf(cha_msg, "TRACKBACK%s", chp_tmp);
	} else {
		strcpy(cha_msg, "TRACKBACK1");
	}
	Put_Nlist(nlp_out, cha_msg, CO_CHECKED);
	/* �����Ƚ���� */
	chp_tmp = Db_GetValue(dbres, 0, 7);
	if(chp_tmp) {
		memset(cha_msg, '\0', sizeof(cha_msg));
		sprintf(cha_msg, "COMMENT%s", chp_tmp);
	} else {
		strcpy(cha_msg, "COMMENT1");
	}
	Put_Nlist(nlp_out, cha_msg, CO_CHECKED);
	/* �����⡼�ɽ���� */
	chp_tmp = Db_GetValue(dbres, 0, 8);
	if(chp_tmp) {
		memset(cha_msg, '\0', sizeof(cha_msg));
		sprintf(cha_msg, "MODE%s", chp_tmp);
	} else {
		strcpy(cha_msg, "MODE1");
	}
	Put_Nlist(nlp_out, cha_msg, CO_CHECKED);
	/* ��...³�����ɤ�פ�ɽ������ʸ���� */
	chp_tmp = Db_GetValue(dbres, 0, 9);
	if(chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "CUT_LENGTH", chp_tmp);
		free(chp_escape);
	}
	/* ��������ޤ����᡼�� */
	if (g_in_cart_mode == CO_CART_SHOPPER) {
		Put_Nlist(nlp_out, "ITEMINFO_START", "-->");
		Put_Nlist(nlp_out, "ITEMINFO_END", "<!--");
		Put_Nlist(nlp_out, "MAIL_SUBJECT", Db_GetValue(dbres, 0, 10) ? Db_GetValue(dbres, 0, 10) : "");
		Put_Nlist(nlp_out, "MAIL_BODY", Db_GetValue(dbres, 0, 11) ? Db_GetValue(dbres, 0, 11) : "");
	}
	/* ��ͧ�ˤΤ߸��� */
	chp_tmp = Db_GetValue(dbres, 0, 12);
	if (chp_tmp && atoi(chp_tmp)) {
		Put_Nlist(nlp_out, "OPEN_FRIEND_ONLY", CO_CHECKED);
	}
	Db_CloseDyna(dbres);

	/* ping���������� */
	sprintf(cha_sql, "select coalesce(max(T1.n_ping_id), 0) from sy_ping T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ping��������Ͽ�˼��Ԥ��ޤ���(4)");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "QUERY", "<br>");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
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
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
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
	Build_Inputarea(nlp_out, "DEFAULT_OTHER_PING", "default_other_ping", chp_escape, CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
	free(chp_escape);

	/* �����ȶػ�IP */
	if (g_in_need_login) {
		if (g_in_hb_mode) {
			Put_Nlist(nlp_out, "CMTHOST1", "���С�");
			Put_Nlist(nlp_out, "CMTHOST2", "���С�̾");
		} else {
			Put_Nlist(nlp_out, "CMTHOST1", "�桼����");
			Put_Nlist(nlp_out, "CMTHOST2", "�˥å��͡���");
		}
	} else {
		Put_Nlist(nlp_out, "CMTHOST1", "�ۥ���");
		Put_Nlist(nlp_out, "CMTHOST2", "IP���ɥ쥹�⤷���ϥۥ���̾");
	}
	sprintf(cha_sql,
		" select"
			" T1.c_host"
		" from"
			" at_denycomment T1"
		" where"
			" T1.n_blog_id = %d",
		in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
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
			if (g_in_need_login) {
				if (Get_Nickname_From_Owner(db, nlp_out, atoi(chp_tmp), cha_msg)) {
					return 1;
				}
				strcat(chp_escape, cha_msg);
			} else {
				chp_tmp2 = Escape_HtmlString(chp_tmp);
				strcat(chp_escape, chp_tmp2);
				free(chp_tmp2);
			}
			strcat(chp_escape, "\n");
		}
	}
	Build_Inputarea(nlp_out, "DENIAL_COMMENT_HOST", "denial_comment_host", chp_escape, CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
	free(chp_escape);

	/* �ȥ�å��Хå��ػ�IP */
	sprintf(cha_sql,
		" select"
			" T1.c_host"
		" from"
			" at_denytrackback T1"
		" where"
			" T1.n_blog_id = %d",
		in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
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
			if (g_in_need_login) {
				if (Get_Nickname_From_Owner(db, nlp_out, atoi(chp_tmp), cha_msg)) {
					return 1;
				}
				strcat(chp_escape, cha_msg);
			} else {
				chp_tmp2 = Escape_HtmlString(chp_tmp);
				strcat(chp_escape, chp_tmp2);
				free(chp_tmp2);
			}
			strcat(chp_escape, "\n");
		}
	}
	Build_Inputarea(nlp_out, "DENIAL_TRACKBACK_HOST", "denial_trackback_host", chp_escape, CO_PING_TEXTAREA_ROW, CO_PING_TEXTAREA_COL);
	free(chp_escape);
	Db_CloseDyna(dbres);

	/* �ȥ�å��Хå��ե��륿�� */
	sprintf(cha_sql, "select n_filter_id,c_filter,b_valid from at_trackback_filter where n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	for (i = 0; i < in_count; ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp) {
			in_filter = atoi(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 2);
			if (chp_tmp) {
				in_valid = atoi(chp_tmp);
			} else {
				in_valid = 0;
			}
			chp_tmp = Db_GetValue(dbres, i, 1);
			switch (in_filter) {
			case CO_URL_FILTER:
				if (in_valid)
					Put_Nlist(nlp_out, "URLFILTER", "checked");
				Put_Nlist(nlp_out, "URL_FILTER", chp_tmp ? chp_tmp : "");
				break;
			case CO_BLOG_FILTER:
				if (in_valid)
					Put_Nlist(nlp_out, "BLOGFILTER", "checked");
				Put_Nlist(nlp_out, "BLOG_FILTER", chp_tmp ? chp_tmp : "");
				break;
			case CO_CONTENTS_FILTER:
				if (in_valid)
					Put_Nlist(nlp_out, "CONTENTSFILTER", "checked");
				Put_Nlist(nlp_out, "CONTENTS_FILTER", chp_tmp ? chp_tmp : "");
				break;
			case CO_ENTRY_FILTER:
				if (in_valid)
					Put_Nlist(nlp_out, "ENTRYFILTER", "checked");
				break;
			}
		}
	}
	Db_CloseDyna(dbres);

	/* �����ȥե��륿�� */
	sprintf(cha_sql, "select c_filter,b_valid from at_comment_filter where n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_valid = atoi(chp_tmp);
	} else {
		in_valid = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (in_valid)
		Put_Nlist(nlp_out, "COMMENTFILTER", "checked");
	Put_Nlist(nlp_out, "COMMENT_FILTER", chp_tmp ? chp_tmp : "");
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_configure()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int update_configure(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_cr;
	char *chp_tmp;
	char *chp_lst;
	char *chp_val;
	char *chp_escape;
	char cha_sql[4096];
	char cha_error[512];
	int in_clear;
	int in_owner;
	int in_error;
	int in_count;
	int in_skip;
	int i;

	/* ���顼�����å� */
	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "blog_title", 1);
	if(!chp_tmp || !*chp_tmp) {
		strcpy(cha_error, "Blog�����ȥ�����Ϥ��Ƥ���������<br>");
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if(Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "Blog�Υ����ȥ�ϡ�����ʳ���ʸ����ޤ�����Ϥ��Ƥ���������<br>");
		in_error++;
	} else if(strlen(chp_tmp) > CO_MAXLEN_BLOG_TITLE) {
		sprintf(cha_error, "Blog�����ȥ�ϡ�Ⱦ��%dʸ��(����%dʸ��)��������Ϥ��Ƥ���������<br>"
			, CO_MAXLEN_BLOG_TITLE, CO_MAXLEN_BLOG_TITLE / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "blog_subtitle", 1);
	if(chp_tmp && (strlen(chp_tmp) > CO_MAXLEN_BLOG_SUBTITLE)) {
		sprintf(cha_error, "Blog�Υ��֥����ȥ�ϡ�%dʸ����������Ϥ��Ƥ���������<br>", CO_MAXLEN_BLOG_SUBTITLE);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "blog_description", 1);
	if(chp_tmp && (strlen(chp_tmp) > CO_MAXLEN_BLOG_DESCRIPTION)) {
		sprintf(cha_error, "Blog�������ϡ�%dʸ����������Ϥ��Ƥ���������<br>", CO_MAXLEN_BLOG_DESCRIPTION);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "toppage_disp_type", 1);
	if(!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�ȥåץڡ�����ɽ�����������򤷤Ƥ���������<br>");
		in_error++;
	} else if(atoi(chp_tmp) == 0) {
		chp_tmp = Get_Nlist(nlp_in, "latest_index", 1);
		if(!chp_tmp || !*chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "�ǿ�������ɽ����������Ϥ���Ƥ��ޤ���<br>");
			in_error++;
		} else if(Check_Numeric(chp_tmp)) {
			Put_Nlist(nlp_out, "ERROR", "�ǿ�������ɽ�������Ⱦ�ѿ��������Ϥ��Ƥ���������<br>");
			in_error++;
		} else if(atoi(chp_tmp) > CO_MAX_DISP_TOPPAGE_INDEX || atoi(chp_tmp) < 1) {
			sprintf(cha_error, "�ǿ�������ɽ������ϡ�1��ʾ�%d�����ǻ��ꤷ�Ƥ���������<br>"
				, CO_MAX_DISP_TOPPAGE_INDEX);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
	} else {
		chp_tmp = Get_Nlist(nlp_in, "latest_day", 1);
		if(!chp_tmp || !*chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "�ǿ�������ɽ�����������Ϥ���Ƥ��ޤ���<br>");
			in_error++;
		} else if(Check_Numeric(chp_tmp)) {
			Put_Nlist(nlp_out, "ERROR", "�ǿ�������ɽ��������Ⱦ�ѿ��������Ϥ��Ƥ���������<br>");
			in_error++;
		} else if(atoi(chp_tmp) > CO_MAX_DISP_TOPPAGE_DAY || atoi(chp_tmp) < 1) {
			sprintf(cha_error, "�ǿ�������ɽ�������ϡ�1���ʾ�%d������ǻ��ꤷ�Ƥ���������<br>"
				, CO_MAX_DISP_TOPPAGE_DAY);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "cut_length", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�ȥåץڡ�����ɽ��������Ƥ�ʸ���������Ϥ���Ƥ��ޤ���<br>");
		in_error++;
	} else if(Check_Numeric(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "�ȥåץڡ�����ɽ��������Ƥ�ʸ������Ⱦ�ѿ��������Ϥ��Ƥ���������<br>");
		in_error++;
	} else if (atoi(chp_tmp) <= 0) {
		sprintf(cha_error, "�ȥåץڡ�����ɽ��������Ƥ�ʸ�����λ��꤬�ְ�äƤ��ޤ���<br>");
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if (atoi(chp_tmp) > CO_MAXLEN_ENTRY_BODY) {
		sprintf(cha_error, "�ȥåץڡ�����ɽ��������Ƥ�ʸ�����ϡ�%dʸ������ǻ��ꤷ�Ƥ���������<br>", CO_MAXLEN_ENTRY_BODY);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	}
	/* �����ԥ����������� */
	chp_tmp = Get_Nlist(nlp_in, "default_other_ping", 1);
	if(chp_tmp && *chp_tmp) {
		if(strlen(chp_tmp) > CO_MAXLEN_UPDATE_PING) {
			sprintf(cha_error, "����ι���ping����ͤ�Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_MAXLEN_UPDATE_PING);
			Put_Nlist(nlp_out, "ERROR", cha_error);
			in_error++;
		}
		if(Check_URL_With_Crlf(nlp_out, chp_tmp, "����ping�����", CO_MAX_UPDATEPING_URLS)) {
			in_error++;
		}
	}
	/* �����ȶػߥۥ��� */
	chp_tmp = Get_Nlist(nlp_in, "denial_comment_host", 1);
	if(chp_tmp && *chp_tmp) {
		chp_val = malloc(strlen(chp_tmp) + 1);
		strcpy(chp_val, chp_tmp);
		chp_cr = strtok(chp_val, "\r\n");
		in_count = 0;
		while (chp_cr) {
			if (chp_cr[0]) {
				++in_count;
				if (in_count > CO_MAX_DENIAL_COMMENT_HOST) {
					if (g_in_need_login) {
						if (g_in_hb_mode) {
							Put_Format_Nlist(nlp_out, "ERROR", "�����ȵ��ݥ��С���%d���������Ϥ��Ƥ���������<br>", CO_MAX_DENIAL_COMMENT_HOST);
						} else {
							Put_Format_Nlist(nlp_out, "ERROR", "�����ȵ��ݥ˥å��͡����%d���������Ϥ��Ƥ���������<br>", CO_MAX_DENIAL_COMMENT_HOST);
						}
					} else {
						Put_Format_Nlist(nlp_out, "ERROR", "�����ȵ��ݥۥ��Ȥ�%d��������Ϥ��Ƥ���������<br>", CO_MAX_DENIAL_COMMENT_HOST);
					}
					in_error++;
					break;
				}
			}
			chp_cr = strtok(NULL, "\r\n");
		}
		free(chp_val);
		if(!g_in_need_login && Check_Alnum_Opt(chp_tmp, "\r\n.-")) {
			Put_Nlist(nlp_out, "ERROR", "�����ȵ��ݥۥ��Ȥ˻��ѤǤ��ʤ�ʸ��������ޤ���ʸ������ǧ���Ƥ���������<br>");
			in_error++;
		}
	}
	/* �ȥ�å��Хå��ػߥۥ��� */
	chp_tmp = Get_Nlist(nlp_in, "denial_trackback_host", 1);
	if(chp_tmp && *chp_tmp) {
		chp_val = malloc(strlen(chp_tmp) + 1);
		strcpy(chp_val, chp_tmp);
		chp_cr = strtok(chp_val, "\r\n");
		in_count = 0;
		while (chp_cr) {
			if (chp_cr[0]) {
				++in_count;
				if (in_count > CO_MAX_DENIAL_TRACKBACK_HOST) {
					if (g_in_need_login) {
						if (g_in_hb_mode) {
							Put_Format_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����ݥ��С���%d���������Ϥ��Ƥ���������<br>", CO_MAX_DENIAL_TRACKBACK_HOST);
						} else if (g_in_dbb_mode) {
							Put_Format_Nlist(nlp_out, "ERROR", "�������ѵ��ݥ˥å��͡����%d���������Ϥ��Ƥ���������<br>", CO_MAX_DENIAL_TRACKBACK_HOST);
						} else {
							Put_Format_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����ݥ˥å��͡����%d���������Ϥ��Ƥ���������<br>", CO_MAX_DENIAL_TRACKBACK_HOST);
						}
					} else {
						Put_Format_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����ݥۥ��Ȥ�%d��������Ϥ��Ƥ���������<br>", CO_MAX_DENIAL_TRACKBACK_HOST);
					}
					in_error++;
					break;
				}
			}
			chp_cr = strtok(NULL, "\r\n");
		}
		free(chp_val);
		if(!g_in_need_login && Check_Alnum_Opt(chp_tmp, "\r\n.-")) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "�������ѵ��ݥۥ��Ȥ˻��ѤǤ��ʤ�ʸ��������ޤ���ʸ������ǧ���Ƥ���������<br>");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����ݥۥ��Ȥ˻��ѤǤ��ʤ�ʸ��������ޤ���ʸ������ǧ���Ƥ���������<br>");
			}
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "url_filter", 1);
	if(chp_tmp && *chp_tmp) {
		if(strlen(chp_tmp) > CO_MAXLEN_FILTER_STRING) {
			Put_Format_Nlist(nlp_out, "ERROR", "URL�ե��륿����ʸ�����Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_MAXLEN_FILTER_STRING);
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "blog_filter", 1);
	if(chp_tmp && *chp_tmp) {
		if(strlen(chp_tmp) > CO_MAXLEN_FILTER_STRING) {
			Put_Format_Nlist(nlp_out, "ERROR", "�֥�̾�ե��륿����ʸ�����Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_MAXLEN_FILTER_STRING);
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "contents_filter", 1);
	if(chp_tmp && *chp_tmp) {
		if(strlen(chp_tmp) > CO_MAXLEN_FILTER_STRING) {
			Put_Format_Nlist(nlp_out, "ERROR", "���ץե��륿����ʸ�����Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_MAXLEN_FILTER_STRING);
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "comment_filter", 1);
	if(chp_tmp && *chp_tmp) {
		if(strlen(chp_tmp) > CO_MAXLEN_FILTER_STRING) {
			Put_Format_Nlist(nlp_out, "ERROR", "�����ȥե��륿����ʸ�����Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_MAXLEN_FILTER_STRING);
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "mail_subject", 1);
	if(chp_tmp && *chp_tmp) {
		if(strlen(chp_tmp) > CO_MAXLEN_FILTER_STRING) {
			Put_Format_Nlist(nlp_out, "ERROR", "�֥��������ΤΥ᡼��η�̾��Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", 256);
			in_error++;
		}
	}
	chp_tmp = Get_Nlist(nlp_in, "mail_body", 1);
	if(chp_tmp && *chp_tmp) {
		if(strlen(chp_tmp) > CO_MAXLEN_FILTER_STRING) {
			Put_Format_Nlist(nlp_out, "ERROR", "�֥��������ΤΥ᡼�����ʸ��Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", 1024);
			in_error++;
		}
	}
	if(in_error) {
		return 1;
	}

	in_clear = 0;
	if (g_in_dbb_mode) {
		chp_tmp = Get_Nlist(nlp_in, "open_friend_only", 1);
		if (chp_tmp && chp_tmp[0]) {
			in_clear = atoi(chp_tmp);
		}
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
		if(Begin_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥�����󳫻Ϥ˼��Ԥ��ޤ�����");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return 1;
		}
		strcpy(cha_sql, "update at_blog set");
		strcat(cha_sql, " c_blog_title = '");
		chp_escape = My_Escape_SqlString(db, Get_Nlist(nlp_in, "blog_title", 1));
		strcat(cha_sql, chp_escape);
		free(chp_escape);
		strcat(cha_sql, "', c_blog_subtitle = '");
		chp_tmp = Get_Nlist(nlp_in, "blog_subtitle", 1);
		if(chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
		strcat(cha_sql, "', c_blog_description = '");
		chp_tmp = Get_Nlist(nlp_in, "blog_description", 1);
		if(chp_tmp && *chp_tmp) {
			chp_escape = My_Escape_SqlString(db, chp_tmp);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
		strcat(cha_sql, "'");
		chp_tmp = Get_Nlist(nlp_in, "default_category", 1);
		strcat(cha_sql, ", n_blog_category = ");
		if(chp_tmp && *chp_tmp) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", b_setting_toppage_disptype = ");
		chp_tmp = Get_Nlist(nlp_in, "toppage_disp_type", 1);
		/* ���򤵤줿���Τ�DB������ */
		if(atoi(chp_tmp) == 0) {
			strcat(cha_sql, "0");
			strcat(cha_sql, ", n_setting_toppage_index = ");
			strcat(cha_sql, Get_Nlist(nlp_in, "latest_index", 1));
		} else {
			strcat(cha_sql,  "1");
			strcat(cha_sql, ", n_setting_toppage_day = ");
			strcat(cha_sql, Get_Nlist(nlp_in, "latest_day", 1));
		}
		strcat(cha_sql, ", b_default_trackback = ");
		chp_tmp = Get_Nlist(nlp_in, "default_trackback", 1);
		if (in_clear) {
			sprintf(cha_sql + strlen(cha_sql), "%d", CO_CMTRB_FRIEND);
		} else if (chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", b_default_comment = ");
		chp_tmp = Get_Nlist(nlp_in, "default_comment", 1);
		if (in_clear) {
			sprintf(cha_sql + strlen(cha_sql), "%d", CO_CMTRB_FRIEND);
		} else if (chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", b_default_mode = ");
		chp_tmp = Get_Nlist(nlp_in, "default_mode", 1);
		if(chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, "1");
		} else {
			strcat(cha_sql, "0");
		}
		strcat(cha_sql, ", n_setting_cut_length = ");
		chp_tmp = Get_Nlist(nlp_in, "cut_length", 1);
		if(chp_tmp && atoi(chp_tmp)) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "200");
		}
		strcat(cha_sql, ", c_mail_subject = '");
		chp_tmp = Get_Nlist(nlp_in, "mail_subject", 1);
		if(chp_tmp && chp_tmp[0]) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "");
		}
		strcat(cha_sql, "', c_mail_body = '");
		chp_tmp = Get_Nlist(nlp_in, "mail_body", 1);
		if(chp_tmp && chp_tmp[0]) {
			strcat(cha_sql, chp_tmp);
		} else {
			strcat(cha_sql, "");
		}
		strcat(cha_sql, "'");
		if (g_in_dbb_mode) {
			sprintf(cha_sql + strlen(cha_sql), ", b_setting_friends_only = %d", in_clear);
		}
		sprintf(cha_sql + strlen(cha_sql), " where n_blog_id = %d", in_blog);
		if(Db_ExecSql(db, cha_sql)){
			Put_Nlist(nlp_out, "ERROR", "���ƥ��꡼���Τ��������Ͽ���륯����˼��Ԥ��ޤ�����");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		if (!g_in_need_login) {
			/* �Ѱդ��줿PING������Υ����å��ܥå������� */
			sprintf(cha_sql, "delete from at_ping where n_blog_id = %d", in_blog);
			if(Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "ping��������Ͽ�˼��Ԥ��ޤ���(1)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
			sprintf(cha_sql,
				"insert into at_ping (n_blog_id,n_ping_id,c_ping_site,c_ping_url,b_default)"
				" select %d,n_ping_id,c_ping_site,c_ping_url,0 from sy_ping", in_blog);
			if(Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "ping��������Ͽ�˼��Ԥ��ޤ���(2)");
				Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
			in_count = Get_NlistCount(nlp_in, "default_prepared_ping");
			if(in_count) {
				for(i = 0; i < in_count; i++) {
					sprintf(cha_sql,
						"update at_ping set b_default = 1 where n_blog_id = %d and n_ping_id = %s",
						in_blog, Get_Nlist(nlp_in, "default_prepared_ping", i + 1));
					if(Db_ExecSql(db, cha_sql)) {
						Put_Nlist(nlp_out, "ERROR", "ping��������Ͽ�˼��Ԥ��ޤ���(3)");
						Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
						Rollback_Transact(db);
						return 1;
					}
				}
			}

			chp_lst = Get_Nlist(nlp_in, "default_other_ping", 1);
			if (chp_lst && *chp_lst) {
				sprintf(cha_sql, "select coalesce(max(T1.n_ping_id), 0) + 1 from at_ping T1 where n_blog_id = %d for update", in_blog);
				dbres = Db_OpenDyna(db, cha_sql);
				if(!dbres) {
					Put_Nlist(nlp_out, "ERROR", "ping��������Ͽ�˼��Ԥ��ޤ���(4)");
					Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
					Put_Nlist(nlp_out, "QUERY", "<br>");
					Put_Nlist(nlp_out, "QUERY", cha_sql);
					return 1;
				}
				chp_tmp = Db_GetValue(dbres, 0, 0);
				if (chp_tmp) {
					in_count = atoi(chp_tmp);
				} else {
					in_count = 1;
				}
				Db_CloseDyna(dbres);
				chp_val = strtok(chp_lst, "\n");
				while (chp_val) {
					chp_cr = strrchr(chp_val, '\r');
					if (chp_cr)
						*chp_cr = '\0';
					if (chp_val[0]) {
						chp_escape = My_Escape_SqlString(db, chp_val);
						sprintf(cha_sql,
							"insert into at_ping "
								"(n_blog_id,n_ping_id,c_ping_url,b_default) "
							"values "
								"(%d, %d, '%s', 0);",
								in_blog, in_count, chp_escape);
						if(Db_ExecSql(db, cha_sql)) {
							Put_Nlist(nlp_out, "ERROR", "ping��������Ͽ�˼��Ԥ��ޤ���(5)");
							Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
							Rollback_Transact(db);
							return 1;
						}
						free(chp_escape);
						++in_count;
					}
					chp_val = strtok(NULL, "\n");
				}
			}
		}

		sprintf(cha_sql, "delete from at_denycomment where n_blog_id = %d", in_blog);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "�����ȵ��ݥۥ�����Ͽ�˼��Ԥ��ޤ���(1)");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		chp_tmp = Get_Nlist(nlp_in, "denial_comment_host", 1);
		if(chp_tmp && *chp_tmp) {
			chp_val = strtok(chp_tmp, "\n");
			in_count = 1;
			while (chp_val) {
				chp_cr = strrchr(chp_val, '\r');
				if (chp_cr)
					*chp_cr = '\0';
				if (chp_val[0]) {
					if (g_in_need_login) {
						in_owner = Get_Owner_From_Nickname(db, nlp_out, chp_val);
						if (in_owner) {
							asprintf(&chp_escape, "%d", in_owner);
						} else {
							Put_Format_Nlist(nlp_out, "ERROR", "�˥å��͡����%s�פ���ĥ桼������¸�ߤ��ޤ���", chp_val);
							Rollback_Transact(db);
							return 1;
						}
					} else {
						chp_escape = My_Escape_SqlString(db, chp_val);
					}
					sprintf(cha_sql,
						"insert into at_denycomment "
							"(n_blog_id,n_host_id,c_host) "
						"values "
							"(%d, %d, '%s');",
							in_blog, in_count, chp_escape);
					if(Db_ExecSql(db, cha_sql)) {
						Put_Nlist(nlp_out, "ERROR", "�����ȵ��ݥۥ�����Ͽ�˼��Ԥ��ޤ���(2)");
						Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
						Rollback_Transact(db);
						return 1;
					}
					free(chp_escape);
					++in_count;
				}
				chp_val = strtok(NULL, "\n");
			}
		}

		sprintf(cha_sql, "delete from at_denytrackback where n_blog_id = %d", in_blog);
		if(Db_ExecSql(db, cha_sql)) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "�������ѵ��ݥۥ�����Ͽ�˼��Ԥ��ޤ���(1)");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����ݥۥ�����Ͽ�˼��Ԥ��ޤ���(1)");
			}
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		chp_tmp = Get_Nlist(nlp_in, "denial_trackback_host", 1);
		if(chp_tmp && *chp_tmp) {
			chp_val = strtok(chp_tmp, "\n");
			in_count = 1;
			while (chp_val) {
				chp_cr = strrchr(chp_val, '\r');
				if (chp_cr)
					*chp_cr = '\0';
				if (chp_val[0]) {
					if (g_in_need_login) {
						in_owner = Get_Owner_From_Nickname(db, nlp_out, chp_val);
						if (in_owner) {
							asprintf(&chp_escape, "%d", in_owner);
						} else {
							Put_Format_Nlist(nlp_out, "ERROR", "�˥å��͡����%s�פ���ĥ桼������¸�ߤ��ޤ���", chp_val);
							Rollback_Transact(db);
							return 1;
						}
					} else {
						chp_escape = My_Escape_SqlString(db, chp_val);
					}
					sprintf(cha_sql,
						"insert into at_denytrackback "
							"(n_blog_id,n_host_id,c_host) "
						"values "
							"(%d, %d, '%s');",
							in_blog, in_count, chp_escape);
					if(Db_ExecSql(db, cha_sql)) {
						if (g_in_dbb_mode) {
							Put_Nlist(nlp_out, "ERROR", "�������ѵ��ݥۥ�����Ͽ�˼��Ԥ��ޤ�����");
						} else {
							Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå����ݥۥ�����Ͽ�˼��Ԥ��ޤ�����");
						}
						Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
						Rollback_Transact(db);
						return 1;
					}
					free(chp_escape);
					++in_count;
				}
				chp_val = strtok(NULL, "\n");
			}
		}

		chp_tmp = Get_Nlist(nlp_in, "use_url_filter", 1);
		if (chp_tmp) {
			in_count = atoi(chp_tmp);
		} else {
			in_count = 0;
		}
		chp_tmp = Get_Nlist(nlp_in, "url_filter", 1);
		if (!chp_tmp)
			chp_tmp = "";
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		sprintf(cha_sql, "update at_trackback_filter set c_filter='%s',b_valid=%d where n_blog_id = %d and n_filter_id = %d", chp_escape, in_count, in_blog, CO_URL_FILTER);
		free(chp_escape);
		if(Db_ExecSql(db, cha_sql)) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "�������ѥե��륿����Ͽ�˼��Ԥ��ޤ�����");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��ե��륿����Ͽ�˼��Ԥ��ޤ�����");
			}
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		chp_tmp = Get_Nlist(nlp_in, "use_blog_filter", 1);
		if (chp_tmp) {
			in_count = atoi(chp_tmp);
		} else {
			in_count = 0;
		}
		chp_tmp = Get_Nlist(nlp_in, "blog_filter", 1);
		if (!chp_tmp)
			chp_tmp = "";
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		sprintf(cha_sql, "update at_trackback_filter set c_filter='%s',b_valid=%d where n_blog_id = %d and n_filter_id = %d", chp_escape, in_count, in_blog, CO_BLOG_FILTER);
		free(chp_escape);
		if(Db_ExecSql(db, cha_sql)) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "�������ѥե��륿����Ͽ�˼��Ԥ��ޤ�����");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��ե��륿����Ͽ�˼��Ԥ��ޤ�����");
			}
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		chp_tmp = Get_Nlist(nlp_in, "use_contents_filter", 1);
		if (chp_tmp) {
			in_count = atoi(chp_tmp);
		} else {
			in_count = 0;
		}
		chp_tmp = Get_Nlist(nlp_in, "contents_filter", 1);
		if (!chp_tmp)
			chp_tmp = "";
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		sprintf(cha_sql, "update at_trackback_filter set c_filter='%s',b_valid=%d where n_blog_id = %d and n_filter_id = %d", chp_escape, in_count, in_blog, CO_CONTENTS_FILTER);
		free(chp_escape);
		if(Db_ExecSql(db, cha_sql)) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "�������ѥե��륿����Ͽ�˼��Ԥ��ޤ�����");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��ե��륿����Ͽ�˼��Ԥ��ޤ�����");
			}
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		chp_tmp = Get_Nlist(nlp_in, "use_entry_filter", 1);
		if (chp_tmp) {
			in_count = atoi(chp_tmp);
		} else {
			in_count = 0;
		}
		sprintf(cha_sql, "update at_trackback_filter set c_filter='',b_valid=%d where n_blog_id = %d and n_filter_id = %d", in_count, in_blog, CO_ENTRY_FILTER);
		if(Db_ExecSql(db, cha_sql)) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "�������ѥե��륿����Ͽ�˼��Ԥ��ޤ�����");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��ե��륿����Ͽ�˼��Ԥ��ޤ�����");
			}
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}

		chp_tmp = Get_Nlist(nlp_in, "use_comment_filter", 1);
		if (chp_tmp) {
			in_count = atoi(chp_tmp);
		} else {
			in_count = 0;
		}
		chp_tmp = Get_Nlist(nlp_in, "comment_filter", 1);
		if (!chp_tmp)
			chp_tmp = "";
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		sprintf(cha_sql, "update at_comment_filter set c_filter='%s',b_valid=%d where n_blog_id = %d", chp_escape, in_count, in_blog);
		free(chp_escape);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "�����ȥե��륿����Ͽ�˼��Ԥ��ޤ�����");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if(Create_Rss(db, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERROR", "RSS�ե�����ι����˼��Ԥ��ޤ�����");
			Rollback_Transact(db);
			return 1;
		}
		if(Commit_Transact(db)) {
			Put_Nlist(nlp_out, "ERROR", "���ߥåȤ˼��Ԥ��ޤ�����");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
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
	char cha_blog[32];
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char *chp_fromuser;
	char *chp_skel;
	char *chp_tmp;
	int in_blog;
	int in_error;
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
					Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
					Put_Nlist(nlp_out, "ERROR", "ǧ�ھ��������Ǥ��ޤ���<br>");
					Page_Out(nlp_out, CO_SKEL_ERROR);
					goto clear_finish;
				}
			} else {
				Disp_Login_Page(nlp_in, nlp_out, CO_CGI_CONFIGURE);
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
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�����Ԥˤ����������ӥ��Τ����Ѥ���ߤ��Ƥ���ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_dbb_mode && Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp)) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
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
	if (g_in_hb_mode && in_orig == INT_MAX && Get_Blog_Auth(db, nlp_out, g_in_login_owner, in_blog) < CO_AUTH_DELETE) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥������븢�¤�����ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	if (in_blog == INT_MAX) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_CONFIGURE);
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "�֥����������Ƥ��ʤ����Ϥ����Ѥˤʤ�ޤ���<br>");
		Page_Out(nlp_out, CO_SKEL_ERROR);
		goto clear_finish;
	}
	sprintf(cha_blog, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "HIDDEN", "blogid", cha_blog);
	g_in_need_login = Need_Login(db, in_blog);
	if (g_in_need_login) {
		Put_Nlist(nlp_out, "PING_START", "<!--");
		Put_Nlist(nlp_out, "PING_END", "-->");
	}

	chp_fromuser = Get_Nlist(nlp_in, "from_user", 1);
	if (chp_fromuser) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_user", chp_fromuser);
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "NEXTCGI", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
			Put_Format_Nlist(nlp_out, "PREVCGI", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
		Put_Nlist(nlp_out, "TOP", "�֥�");
		Put_Nlist(nlp_out, "GIF", "go_blog.gif");
	} else {
		Put_Format_Nlist(nlp_out, "NEXTCGI", "%s?blogid=%d", CO_CGI_MENU, in_blog);
		Put_Format_Nlist(nlp_out, "PREVCGI", "%s?blogid=%d", CO_CGI_MENU, in_blog);
		Put_Nlist(nlp_out, "TOP", "������˥塼");
		Put_Nlist(nlp_out, "GIF", "go_kanri.gif");
	}

	in_error = 0;
	chp_skel = NULL;
	if(Get_Nlist(nlp_in, "from_error", 1)) {
		if(disp_page_configure_from_error(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		} else {
			chp_skel = CO_SKEL_CONFIGURE;
		}
	} else if(Get_Nlist(nlp_in, "BTN_DISP_CONFIGURE", 1)) {
		if(disp_page_configure(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		} else {
			chp_skel = CO_SKEL_CONFIGURE;
		}
	} else if (Get_Nlist(nlp_in, "BTN_EDIT_ABOUT_BLOG", 1)) {
		if(disp_page_configure(db, nlp_in, nlp_out, in_blog)) {
			in_error++;
		} else {
			chp_skel = CO_SKEL_CONFIGURE;
		}
	} else if(Get_Nlist(nlp_in, "BTN_CONFIGURE.x", 1)) {
		if(update_configure(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if(disp_page_configure_from_error(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			} else {
				chp_skel = CO_SKEL_CONFIGURE;
			}
		} else {
			char cha_jump[1024];
			if (Get_Nlist(nlp_in, "from_user", 1)) {
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
			Put_Nlist(nlp_out, "MESSAGE", "�֥�������򹹿����ޤ�����");
			Put_Nlist(nlp_out, "RETURN", cha_jump);
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
	} else if(Get_Nlist(nlp_in, "BTN_CONFIGURE_ABOUT_BLOG", 1)) {
		/* SIDECONTENT���꤫���褿��縵�β��̤���� */
		if(update_configure(db, nlp_in, nlp_out, in_blog)) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			if(disp_page_configure_from_error(db, nlp_in, nlp_out, in_blog)) {
				in_error++;
			} else {
				chp_skel = CO_SKEL_CONFIGURE;
			}
		} else {
			printf("Location:%s%s%s/%s?BTN_DISP_SETTING_SIDECONTENT=1&blogid=%d&sidecontent=%d\n\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_SIDECONTENT, in_blog, CO_SIDECONTENT_ABOUT_BLOG);
		}
	} else {
		Put_Nlist(nlp_out, "ERROR", "ľ��CGI��¹ԤǤ��ޤ���<br>");
		in_error++;
	}
	/* ���������ȯư���ϥڡ���������̵�� */
	if(in_error) {
		put_error_data(nlp_in, nlp_out);
		Page_Out(nlp_out, CO_SKEL_ERROR);
		return 1;
	} else if (chp_skel != NULL) {
		Page_Out(nlp_out, chp_skel);
	}

clear_finish:
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Db_Disconnect(db);

	return in_error;
}
