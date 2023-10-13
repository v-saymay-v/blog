#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmobile.h"
#include "libauth.h"
#include "libblog.h"
#include "blog_mobile_menu.h"
#include "blog_mobile_new_entry.h"
#include "blog_mobile_modify_entry.h"
#include "blog_mobile_entry_list.h"

typedef int PageProc(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog);
typedef struct _st_pageitem {
	char		*chp_name;
	PageProc	*stp_proc;
} st_pageitem;

st_pageitem sta_pageprocs[] = {
	{"mn", Mobile_Admin_Menu},
	{"nw", Mobile_Admin_New_Entry},
	{"ls", Mobile_Admin_Entry_List},	// ��������
	{"me", Mobile_Admin_Modify_Entry},		// �����Խ�
	{"wc", Mobile_Admin_New_Entry},		// ���ҎݎĽ񤭹���
	{"wt", Mobile_Admin_New_Entry},		// TB�񤭹���
};

/*
+* ------------------------------------------------------------------------
 * Function:	 	error_check
 * Description:
 *	���Ϲ��ܤΎ��ס���������
%* ------------------------------------------------------------------------
 * Return:
 *	0 ���� 1�ʾ� �۾�
-* ------------------------------------------------------------------------*/
int error_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out)
{
	char *chp_tmp;
	char cha_date[16];
	char cha_error[512];
	int in_error;

	in_error = 0;
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����Ď٤����Ϥ��Ƥ���������<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_ENTRY_TITLE_MOBILE) {
		sprintf(cha_error, "�����Ď٤�Ⱦ��%dʸ��������%dʸ���˰�������Ϥ��Ƥ���������<br>"
			, CO_MAXLEN_ENTRY_TITLE_MOBILE, CO_MAXLEN_ENTRY_TITLE_MOBILE / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "�����Ď٤ˤϡ�����ʳ���ʸ�������Ϥ��Ƥ���������<br>");
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_body", 1);
	if (!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "���Ƥ����Ϥ��Ƥ���������<br>");
		in_error++;
	} else if (strlen(chp_tmp) > CO_MAXLEN_ENTRY_BODY_MOBILE) {
		sprintf(cha_error, "���Ƥ�Ⱦ��%dʸ��������%dʸ���˰�������Ϥ��Ƥ���������<br>"
			, CO_MAXLEN_ENTRY_BODY_MOBILE, CO_MAXLEN_ENTRY_BODY_MOBILE / 2);
		Put_Nlist(nlp_out, "ERROR", cha_error);
		in_error++;
	} else if (Check_Space_Only(chp_tmp)) {
		Put_Nlist(nlp_out, "ERROR", "���Ƥˤϡ�����ʳ���ʸ�������Ϥ��Ƥ���������<br>");
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
	chp_tmp = Get_Nlist(nlp_in, "specify_date", 1);
	if (chp_tmp && atoi(chp_tmp)) {
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
	}
	chp_tmp = Get_Nlist(nlp_in, "trackback", 1);
	if (!chp_tmp){
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "�������Ѽ��ղ��ݤ����򤷤Ƥ���������<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�Ď׎����ʎގ������ղ��ݤ����򤷤Ƥ���������<br>");
		}
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (!chp_tmp){
		Put_Nlist(nlp_out, "ERROR", "���Ҏݎļ��ղ��ݤ����򤷤Ƥ���������<br>");
		in_error++;
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_mode", 1);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����ӡ��Ďޤ����򤷤Ƥ���������<br>");
		in_error++;
	}
	/*�Ď׎����ʎގ���PING�����Ύ�������*/
	chp_tmp = Get_Nlist(nlp_in, "trackback_url", 1);
	if (chp_tmp && *chp_tmp) {
		if (g_in_dbb_mode) {
			if (Check_URL_With_Crlf(nlp_out, chp_tmp, "����������", CO_MAX_TRACKBACK_URLS)) {
				in_error++;
			}
		} else {
			if (Check_URL_With_Crlf(nlp_out, chp_tmp, "�Ď׎����ʎގ�����", CO_MAX_TRACKBACK_URLS)) {
				in_error++;
			}
		}
	}
	return in_error;
}

void Find_Team(char *chp_more, int *inp_year, int *inp_month, int *inp_day, char *chp_team)
{
	char cha_sql[1024];
	char *chp_tmp;
	char *chp_escape;

	chp_team[0] = '\0';
	*inp_year = 0;
	*inp_month = 0;
	*inp_day = 0;
	if (chp_more && *chp_more) {
		chp_escape = strtok(chp_more, "\n");
		while (chp_escape) {
			if (strstr(chp_escape, "colspan=\"7\"")) {
				char *chp_str;
				chp_str = strstr(chp_escape, "ǯ");
				if (chp_str) {
					chp_str -= 4;
					sscanf(chp_str, "%dǯ%d��%d��%s", inp_year, inp_month, inp_day, cha_sql);
				} else {
					chp_str = strstr(chp_escape, "<b>");
					if (chp_str) {
						chp_str += strlen("<b>");
						chp_tmp = strstr(chp_str, "</b>");
						if (chp_tmp) {
							strncpy(chp_team, chp_str, chp_tmp - chp_str);
							chp_team[chp_tmp - chp_str] = '\0';
						} else {
							strcpy(chp_team, chp_str);
						}
					}
				}
				if (chp_team[0] && *inp_year && *inp_month && *inp_day) {
					break;
				}
			}
			chp_escape = strtok(NULL, "\n");
		}
	}
}

int main()
{
	NLIST *nlp_in;
	NLIST *nlp_out;
	DBase *db;
	int i;
	int in_error;
	int in_owner;
	int in_admin;
	int in_login_blog;
	char cha_mess[1024];
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char *chp_blog;
	char *chp_page;

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	db = NULL;
	in_error = Read_Blog_Ini(cha_host,cha_db, cha_username, cha_password, &g_in_ssl_mode);
	if (in_error) {
		sprintf(cha_mess, "INI�̎����٤��ɤ߹��ߤ˼��Ԥ��ޤ�����(%s)", strerror(in_error));
		Put_Nlist(nlp_out, "ERROR", cha_mess);
		goto clear_finish;
	}
	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		sprintf(cha_mess, "�Îޡ����͎ޡ�����³�˼��Ԥ��ޤ�����(%s - %s - %s - %s)", cha_host, cha_db, cha_username, cha_password);
		Put_Nlist(nlp_out, "ERROR", cha_mess);
		++in_error;
		goto clear_finish;
	}
	if (Read_Blog_Option(db)) {
		Put_Nlist(nlp_out, "ERROR", "�̎ގێ��ގ��̎ߎ����ݤ��ɤ߹��ߤ˼��Ԥ��ޤ�����");
		++in_error;
		goto clear_finish;
	}
	Set_SkelPath(g_cha_admin_skeleton);

	chp_blog = Get_Nlist(nlp_in, "owner", 1);
	in_owner = chp_blog ? atoi(chp_blog) : 0;
	in_login_blog = Check_Mobile_Login(db, nlp_in, nlp_out, in_owner, &in_admin);
	if (in_login_blog < 0) {
		++in_error;
		goto clear_finish;
	}
	if (!in_login_blog) {
		if (Jump_To_Login_Mobile_Cgi(db, nlp_out)) {
			++in_error;
		}
		goto clear_finish;
	}
	g_in_admin_mode = in_admin;
	Blog_To_Temp(db, nlp_out, in_login_blog, g_cha_blog_temp);

	chp_page = Get_Nlist(nlp_in, "pg", 1);
	if (!chp_page) {
		chp_page = "mn";
	}
	for (i = 0; i < (int)(sizeof(sta_pageprocs)/sizeof(st_pageitem)); ++i) {
		if (!strcmp(sta_pageprocs[i].chp_name, chp_page)) {
			in_error = sta_pageprocs[i].stp_proc(db, nlp_in, nlp_out, in_login_blog);
			break;
		}
	}

clear_finish:
	if (db) {
		Db_Disconnect(db);
	}
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);

	return in_error;
}
