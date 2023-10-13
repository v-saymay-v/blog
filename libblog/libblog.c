/*
+* ------------------------------------------------------------------------
 * Module-Name:         libblog.c
 * First-Created:       2004/08/16 ��¼ ����Ϻ
%* ------------------------------------------------------------------------
 * Module-Description:
 *	blog��Ϣ�Υ饤�֥�ꡣ
-* ------------------------------------------------------------------------
 * Change-Log:
 * 	2005/06/16 �غ�����
 *		�Ȥꤢ�����ե����ޥåȤ��ľ��
 *	2005/07/13 �غ�����
 *		�Ȥ��Ƥ��ʤ��ؿ���������
 *		ñ���CGI�Ǥ������Ѥ���Ƥ��餺���������������㤤��Τϥ����饤����
 *		����˱�äƴؿ�̾����ʸ������
 *		Remove_HtmlTags���ɲá�
 *	2005/07/14 �غ�����
 *		Get_Browser���ɲ�
 *	2005/08/18 �غ�����
 *		Remove_HtmlTags���르�ꥺ���ѹ���
 *	2005/08/22 �غ�����
 *		Get_Flag_Public �ɲá�
 *	2005/08/23 �غ�����
 *		Check_Url_Simple ������
 *		Check_Url_Simple_With_Crlf -> Check_URL_With_Crlf ���ѹ�
 *		Check_Alnum_Opt ���ɲá�
$* ------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h>
#include <netdb.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <libxml/parser.h>
#include <libcgi2.h>
#include <excart.h>
#include <libexcart.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "libnkf.h"
#include "libjcode.h"
#include "libcommon2.h"
#include "libnet.h"
#include "libhttp.h"
#include "libauth.h"
#include "libblog.h"

#define CO_COMMENT_INTERVAL	30	/* �����ȥ��󥿡��Х� */

int Is_Ball_Friend(DBase *db, NLIST *nlp_out, int in_login_blog, int in_blog);

int g_in_ssl_mode = 0;
int g_in_dbb_mode = 0;
int g_in_cart_mode = 0;
int g_in_hb_mode = 0;
int g_in_hosting_mode = 0;
int g_in_short_name = 0;
int g_in_html_editor = 0;
int g_in_admin_mode = 0;
int g_in_login_owner = 0;
char g_cha_protocol[256] = "";
char g_cha_asp_skeleton[256] = "";
char g_cha_admin_skeleton[256] = "";
char g_cha_user_skeleton[256] = "";
char g_cha_rss_path[256] = "";
char g_cha_css_path[256] = "";
char g_cha_upload_path[256] = "";
char g_cha_asp_cgi[256] = "";
char g_cha_admin_cgi[256] = "";
char g_cha_user_cgi[256] = "";
char g_cha_admin_image[256] = "";
char g_cha_user_image[256] = "";
char g_cha_theme_image[256] = "";
char g_cha_css_location[256] = "";
char g_cha_rss_location[256] = "";
char g_cha_script_location[256] = "";
char g_cha_base_location[256] = "";
char g_cha_host_name[256] = "";
char g_cha_blog_temp[256] = {0};

int g_in_need_login = 0;
char gcha_theme_first_icon[256] = {0};
char gcha_theme_last_icon[256] = {0};
char gcha_theme_prev_entry_icon[256] = {0};
char gcha_theme_next_entry_icon[256] = {0};
char gcha_theme_prev_page_icon[256] = {0};
char gcha_theme_next_page_icon[256] = {0};
char gcha_theme_plus_icon[256] = {0};
char gcha_theme_minus_icon[256] = {0};
char gcha_theme_back_top_icon[256] = {0};
char gcha_theme_back_blogtop_icon[256] = {0};

static char g_cha_blog_dir[1024] = "../..";
static char g_cha_blog_ini[1024] = "/data/dbb_blog.ini";
static char g_cha_blog_local[1024] = "/data/dbb_blog/dbb_blog.ini";
static char g_cha_blog_cart[1024] = "/data/as-blog/dbb_blog.ini";
static char g_cha_ini[1024] = "dbb_blog.ini";
static char g_cha_cnf[1024] = "dbb_blog.cnf";

int Is_Black_List(DBase *db, NLIST *nlp_out, const char *chp_access_key);
int Is_Caution_Owner(DBase *db, NLIST *nlp_out, const char *chp_access_key);

static void debug_log(char *chp_format, ...)
{
	FILE* fp;
	va_list ap;
	char szLogFile[1024];
	char* chp_buf;
	int userid;

	va_start(ap, chp_format);
	vasprintf(&chp_buf, chp_format, ap);
	va_end (ap);

	userid = geteuid();
	if (userid != -1) {
		struct passwd *pwd = getpwuid(userid);
		strcpy(szLogFile, pwd->pw_dir);
		strcat(szLogFile, "/debug.log");
	} else {
		strcpy(szLogFile, "/var/tmp/debug.log");
	}
	fp = fopen(szLogFile, "a+b");
	if (fp) {
		fwrite(chp_buf, strlen(chp_buf), 1, fp);
		fclose(fp);
	}
	free(chp_buf);
}

static int iseuc(const unsigned char* p)
{
	unsigned char b1 = *p++;
	if (b1 == 0x8e) {
		return 1;
	} else if (b1 >= 0xa1 && b1 <= 0xfe) {
		unsigned char b2 = *p;
		if (b2 >= 0xa1 && b2 <= 0xfe) {
			return 1;
		}
	}
	return 0;
}

/*
+* ========================================================================
 * Function:		�����ini�ɹ�
 * Include-file:
 * Description:
 *	���ե��ꥨ���ȥ����ƥ������ե�������ɤ߹��ߡ�
 *	�����Х�ǡ�������¸��
%* ========================================================================
 * Return:
 *	����� 0:������0�ʳ�:����
-* ======================================================================*/
int Read_Ini(const char* chp_ini, char* chp_host, char* chp_db, char* chp_username, char* chp_password, int *inp_ssl)
{
	FILE* fp;
	char cha_oneline[1024];

	fp = fopen(chp_ini, "r");
	if (!fp)
		return errno;

	*inp_ssl = 0;
	while (fgets(cha_oneline, sizeof(cha_oneline), fp)) {
		char* chp_name = strtok(cha_oneline, "=");
		if (chp_name) {
			char* chp_value = strtok(NULL, "=");
			if (chp_value) {
				char* chp_tmp;
				chp_tmp = strchr(chp_value, '\r');
				if (chp_tmp)
					*chp_tmp = '\0';
				chp_tmp = strchr(chp_value, '\n');
				if (chp_tmp)
					*chp_tmp = '\0';
				if (strcasecmp(chp_name, "username") == 0) {
					strcpy(chp_username, chp_value);
				} else if (strcasecmp(chp_name, "dbname") == 0) {
					strcpy(chp_db, chp_value);
				} else if (strcasecmp(chp_name, "hostname") == 0) {
					strcpy(chp_host, chp_value);
				} else if (strcasecmp(chp_name, "password") == 0) {
					strcpy(chp_password, chp_value);
				} else if (strcasecmp(chp_name, "ssl") == 0) {
					*inp_ssl = atoi(chp_value);
				}
			}
		}
	}
	fclose(fp);
	return 0;
}

/*
+* ========================================================================
 * Function:		asj_click.ini�ɹ�
 * Include-file:
 * Description:
 *	���ե��ꥨ���ȥ����ƥ������ե�����asj_click.ini���ɤ߹��ߡ�
 *	�����Х�ǡ�������¸��
%* ========================================================================
 * Return:
 *	����� 0:������0�ʳ�:����
-* ======================================================================*/
int Read_Blog_Ini(char* chp_host, char* chp_db, char* chp_username, char* chp_password, int *inp_ssl)
{
	FILE* fp;
	struct passwd *pwd;
	char cha_userdir[256];
	char cha_oneline[1024];
	int in_userid;
	int in_err;

	sprintf(cha_oneline, "./%s", g_cha_ini);
	in_err = Read_Ini(cha_oneline, chp_host, chp_db, chp_username, chp_password, inp_ssl);
	if (!in_err)
		goto next_step;
	sprintf(cha_oneline, "../%s", g_cha_ini);
	in_err = Read_Ini(cha_oneline, chp_host, chp_db, chp_username, chp_password, inp_ssl);
	if (!in_err)
		goto next_step;
	sprintf(cha_oneline, "../../%s", g_cha_ini);
	in_err = Read_Ini(cha_oneline, chp_host, chp_db, chp_username, chp_password, inp_ssl);
	if (!in_err)
		goto next_step;

	cha_userdir[0] = '\0';
	sprintf(cha_oneline, "./%s", g_cha_cnf);
	fp = fopen(cha_oneline, "r");
	if (fp) {
		while (fgets(cha_oneline, sizeof(cha_oneline), fp)) {
			char* chp_name = strtok(cha_oneline, "=");
			if (chp_name) {
				char* chp_value = strtok(NULL, "=");
				if (chp_value) {
					char* chp_tmp;
					chp_tmp = strchr(chp_value, '\r');
					if (chp_tmp)
						*chp_tmp = '\0';
					chp_tmp = strchr(chp_value, '\n');
					if (chp_tmp)
						*chp_tmp = '\0';
					if (strcmp(chp_name, "userdir") == 0) {
						strcpy(cha_userdir, chp_value);
						break;
					}
				}
			}
		}
		fclose(fp);
	}

	in_userid = getuid();
	if (in_userid != -1) {
		pwd = getpwuid(in_userid);
		if (pwd) {
			strcpy(g_cha_blog_dir, pwd->pw_dir);
		}
	}

	if (cha_userdir[0]) {
		strcpy(cha_oneline, g_cha_blog_dir);
		strcat(cha_oneline, "/data/");
		strcat(cha_oneline, cha_userdir);
		strcat(cha_oneline, "/");
		strcat(cha_oneline, g_cha_ini);
		in_err = Read_Ini(cha_oneline, chp_host, chp_db, chp_username, chp_password, inp_ssl);
		if (!in_err)
			goto next_step;
	} else {
		strcpy(cha_oneline, g_cha_blog_dir);
		strcat(cha_oneline, g_cha_blog_ini);
		in_err = Read_Ini(cha_oneline, chp_host, chp_db, chp_username, chp_password, inp_ssl);
		if (!in_err)
			goto next_step;
		strcpy(cha_oneline, g_cha_blog_dir);
		strcat(cha_oneline, g_cha_blog_local);
		in_err = Read_Ini(cha_oneline, chp_host, chp_db, chp_username, chp_password, inp_ssl);
		if (!in_err)
			goto next_step;
		strcpy(cha_oneline, g_cha_blog_dir);
		strcat(cha_oneline, g_cha_blog_cart);
		in_err = Read_Ini(cha_oneline, chp_host, chp_db, chp_username, chp_password, inp_ssl);
		if (!in_err)
			goto next_step;
	}
	fprintf(stderr, "%s - %s\n", cha_oneline, strerror(errno));
	return errno;

next_step:
	return 0;
}

/*
+* ========================================================================
 * Function:		replace_string
 * Description:
 *	ʸ������ִ�
%* ========================================================================
 * Return:
 * 0: ���θ�ν�����³����
 * 1: �����Ϥ����ǽ����
-* ======================================================================*/
void Replace_String(const char *chp_before, const char *chp_after, char *chp_string)
{
	char cha_temp[1024];
	char *chp_find;

	while (1) {
		chp_find = strstr(chp_string, chp_before);
		if (!chp_find)
			break;
		strncpy(cha_temp, chp_string, chp_find - chp_string);
		cha_temp[chp_find - chp_string] = '\0';
		strcat(cha_temp, chp_after);
		strcat(cha_temp, chp_find + strlen(chp_before));
		strcpy(chp_string, cha_temp);
	}
}

/*
+* ========================================================================
 * Function:		Read_Blog_Option
 * Description:
 *	�ǡ����١�������¸����Ƥ��륵���ӥ����ץ����򥰥��Х�ǡ������ɤ߹��ࡣ
%* ========================================================================
 * Return:
 * 0: ���θ�ν�����³����
 * 1: �����Ϥ����ǽ����
-* ======================================================================*/
int Read_Blog_Option(DBase *db)
{
	char* chp_val;
	struct passwd *stp_pwd;
	DBRes *res;

	res = Db_OpenDyna(db,
		"select "
			"c_adminskelpath,"		//  0
			"c_userskelpath,"		//  1
			"c_adminimagesloc,"		//  2
			"c_rsspath,"			//  3
			"c_uploadpath,"			//  4
			"c_cssloc,"				//  5
			"c_userimagesloc,"		//  6
			"c_themeimagesloc,"		//  7
			"c_aspcgiloc,"			//  8
			"c_admincgiloc,"		//  9
			"c_usercgiloc,"			// 10
			"c_rssloc,"				// 11
			"c_aspskelpath,"		// 12
			"c_csspath,"			// 13
			"b_shortname,"			// 14
			"c_baseloc,"			// 15
			"c_scriptsloc,"			// 16
			"b_useeditor,"			// 17
			"b_dbbmode,"			// 18
			"b_cartmode,"			// 19
			"b_hostingmode,"		// 20
			"b_hbmode,"				// 21
			"c_hostname "			// 22
		"from "
			 "sy_baseinfo;");
	if (!res) {
		fprintf(stderr, "Read service option failed.(%s)\n", Gcha_last_error);
		return 1;
	}
	if (!Db_GetRowCount(res)) {
		fprintf(stderr, "Service option no record.\n");
		Db_CloseDyna(res);
		return 1;
	}

	/* �����ԥ�����ȥ�Υǥ��쥯�ȥ� */
	chp_val = Db_GetValue(res, 0, 0);
	if (chp_val)
		strcpy(g_cha_admin_skeleton, chp_val);
	/* �桼����������ȥ�Υǥ��쥯�ȥ� */
	chp_val = Db_GetValue(res, 0, 1);
	if (chp_val)
		strcpy(g_cha_user_skeleton, chp_val);
	/* admin���᡼����������� */
	chp_val = Db_GetValue(res, 0, 2);
	if (chp_val)
		strcpy(g_cha_admin_image, chp_val);
	/* RSS��¸�ǥ��쥯�ȥ� */
	chp_val = Db_GetValue(res, 0, 3);
	if (chp_val)
		strcpy(g_cha_rss_path, chp_val);
	/* �ե����륢�åץ��ɥǥ��쥯�ȥ� */
	chp_val = Db_GetValue(res, 0, 4);
	if (chp_val)
		strcpy(g_cha_upload_path, chp_val);
	/* �������륷���ȥ�������� */
	chp_val = Db_GetValue(res, 0, 5);
	if (chp_val)
		strcpy(g_cha_css_location, chp_val);
	/* user���᡼����������� */
	chp_val = Db_GetValue(res, 0, 6);
	if (chp_val)
		strcpy(g_cha_user_image, chp_val);
	/* theme���᡼����������� */
	chp_val = Db_GetValue(res, 0, 7);
	if (chp_val)
		strcpy(g_cha_theme_image, chp_val);
	/* asp CGI��������� */
	chp_val = Db_GetValue(res, 0, 8);
	if (chp_val)
		strcpy(g_cha_asp_cgi, chp_val);
	/* admin CGI��������� */
	chp_val = Db_GetValue(res, 0, 9);
	if (chp_val)
		strcpy(g_cha_admin_cgi, chp_val);
	/* user CGI��������� */
	chp_val = Db_GetValue(res, 0, 10);
	if (chp_val)
		strcpy(g_cha_user_cgi, chp_val);
	/* RSS��������� */
	chp_val = Db_GetValue(res, 0, 11);
	if (chp_val)
		strcpy(g_cha_rss_location, chp_val);
	/* ASP������ȥ�Υǥ��쥯�ȥ� */
	chp_val = Db_GetValue(res, 0, 12);
	if (chp_val)
		strcpy(g_cha_asp_skeleton, chp_val);
	/* CSS��¸�ǥ��쥯�ȥ� */
	chp_val = Db_GetValue(res, 0, 13);
	if (chp_val)
		strcpy(g_cha_css_path, chp_val);
	/* URL��û�����뤫 */
	chp_val = Db_GetValue(res, 0, 14);
	if (chp_val)
		g_in_short_name = atoi(chp_val);
	/* ���硼��̾���ѻ��Υ١������������ */
	chp_val = Db_GetValue(res, 0, 15);
	if (chp_val)
		strcpy(g_cha_base_location, chp_val);
	/* ������ץȥ�������� */
	chp_val = Db_GetValue(res, 0, 16);
	if (chp_val)
		strcpy(g_cha_script_location, chp_val);
	/* HTML���ǥ���������Ѥ��뤫 */
	chp_val = Db_GetValue(res, 0, 17);
	if (chp_val)
		g_in_html_editor = atoi(chp_val);
	/* DBB�⡼�ɤ�ư��뤫 */
	chp_val = Db_GetValue(res, 0, 18);
	if (chp_val)
		g_in_dbb_mode = atoi(chp_val);
	/* �����ȥ⡼�ɤ�ư��뤫 */
	chp_val = Db_GetValue(res, 0, 19);
	if (chp_val)
		g_in_cart_mode = atoi(chp_val);
	/* �ۥ��ƥ��󥰷�����ǧ�ڤ� */
	chp_val = Db_GetValue(res, 0, 20);
	if (chp_val)
		g_in_hosting_mode = atoi(chp_val);
	/* HotBiz�⡼�ɤ�ư��뤫 */
	chp_val = Db_GetValue(res, 0, 21);
	if (chp_val)
		g_in_hb_mode = atoi(chp_val);
	/* �֥����֤���Ƥ���ۥ���̾ */
	chp_val = Db_GetValue(res, 0, 22);
	if (chp_val)
		strcpy(g_cha_host_name, chp_val);

	Db_CloseDyna(res);

	chp_val = getenv("HTTPS");
	if (chp_val && strcasecmp(chp_val, "on") == 0) {
		strcpy(g_cha_protocol, "https://");
	} else {
		chp_val = getenv("HTTP_REFERER");
		if (chp_val){
			if (strncmp(chp_val, "https", 5) == 0) {
				strcpy(g_cha_protocol, "https://");
			} else {
				strcpy(g_cha_protocol, "http://");
			}
		} else {
			strcpy(g_cha_protocol, "http://");
		}
	}

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		Replace_String("#USERDIR#", stp_pwd->pw_dir, g_cha_rss_path);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, g_cha_css_path);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, g_cha_upload_path);
		Replace_String("#USER#", stp_pwd->pw_name, g_cha_css_location);
		Replace_String("#USER#", stp_pwd->pw_name, g_cha_rss_location);
		Replace_String("#USER#", stp_pwd->pw_name, g_cha_script_location);
		Replace_String("#USER#", stp_pwd->pw_name, g_cha_base_location);
		Replace_String("#USER#", stp_pwd->pw_name, g_cha_asp_cgi);
		Replace_String("#USER#", stp_pwd->pw_name, g_cha_admin_cgi);
		Replace_String("#USER#", stp_pwd->pw_name, g_cha_user_cgi);
	}

	return 0;
}

/*
+* ========================================================================
 * Function:		Get_Browser
 * Include-file 	unistd.h
 * Description:
 *	�֥饦����Ƚ�̤��ƥ꥿����
%* ========================================================================
 * Return:
-* ======================================================================
*/
int Get_Browser(void)
{
	char *chp_tmp;

	chp_tmp = getenv("HTTP_USER_AGENT");
	if (!chp_tmp || !*chp_tmp) {
		return CO_BROWSER_OTHER;
	}
	/* "Opera"���ޤޤ줿��̵����Opera */
	if (strstr(chp_tmp, "Opera")) {
		return CO_BROWSER_OPERA;
	/* "Safari"���ޤޤ줿��̵����Safari */
	} else if (strstr(chp_tmp, "Safari")) {
		return CO_BROWSER_SAFARI;
	/* �嵭�ʳ��ǡ�MSIE���ޤޤ�Ƥ�����IE */
	} else if (strstr(chp_tmp, "MSIE")) {
		return CO_BROWSER_IE;
	/* �嵭�ʳ��ǡ�Gecko���ޤޤ�Ƥ�����Gecko�ʹߤ�NS��Mozzila */
	} else if (strstr(chp_tmp, "Gecko")) {
		return CO_BROWSER_NS6;
	/* �嵭�ʳ��ǡ�Mozilla���ޤޤ�Ƥ�����Gecko���� */
	} else if (strstr(chp_tmp, "Mozilla")) {
		return CO_BROWSER_NS47;
	}
	return CO_BROWSER_OTHER;
}

/*
+* ========================================================================
 * Function:		Get_OperatingSystem
 * Include-file
 * Description:
 *	���饤�����OS��Ƚ�̤��ƥ꥿����
%* ========================================================================
 * Return:
-* ======================================================================
*/
int Get_OperatingSystem(void)
{
	int in_os;
	char *chp_tmp;

	in_os = CO_OS_OTHER;
	chp_tmp = getenv("HTTP_USER_AGENT");
	if (!chp_tmp || !*chp_tmp) {
		return in_os;
	}
	if (strstr(chp_tmp, "Windows")) {
		in_os = CO_OS_WIN;
	} else if (strstr(chp_tmp, "Mac_")) {
		in_os = CO_OS_MAC;
	}
	return in_os;
}

/*
+* ========================================================================
 * Function:		Build_Radio_With_Id
 * Include-file:	libcgi2.h
 * Description:
 *	<input type="radio" name="chp_name" value="chp_value" id="chp_id">
 *	<label for="chp_id">chp_message</label>�κ���
%* ========================================================================
 * Return: ̵��
-* ======================================================================
*/
void Build_Radio_With_Id(
	  NLIST *nlp_out
	, char *chp_insert	/* ������ */
	, char *chp_name	/* name */
	, char *chp_value	/* value */
	, int in_flag		/* ��0�������å����� */
	, char *chp_id		/* ID */
	, char *chp_message	/* ��٥�ˤ����å����� */
)
{
	char *chp_str;
	char cha_checked[16];

	if (in_flag) {
		strcpy(cha_checked, "checked");
	} else {
		strcpy(cha_checked, "");
	}
	chp_str = (char *)calloc(strlen(chp_name) + strlen(chp_value) + strlen(chp_id) * 2
				 + strlen(cha_checked) + strlen(chp_message) + 512, sizeof(char));
	sprintf(chp_str, "<input type=\"radio\" id=\"%s\" name=\"%s\" value=\"%s\" %s><label for=\"%s\">%s</label>\n"
		, chp_id, chp_name, chp_value, cha_checked, chp_id, chp_message);
	Put_Nlist(nlp_out, chp_insert, chp_str);
	free(chp_str);
	return;
}

/*
+* ========================================================================
 * Function:		Build_Checkbox_With_Id
 * Include-file:	libcgi2.h
 * Description:
 *	<input type="radio" name="chp_name" value="chp_value" id="chp_id">
 *	<label for="chp_id">chp_message</label>�κ���
%* ========================================================================
 * Return: ̵��
-* ======================================================================
*/
void Build_Checkbox_With_Id(
	  NLIST *nlp_out
	, char *chp_insert	/* ������ */
	, char *chp_name	/* name */
	, char *chp_value	/* value */
	, int in_flag		/* ��0 checked 0 ̵�� */
	, char *chp_id		/* ID */
	, char *chp_message	/* ��٥�ˤ���ʸ�� */
)
{
	char *chp_str;
	char cha_checked[16];

	if (in_flag) {
		strcpy(cha_checked, "checked");
	} else {
		strcpy(cha_checked, "");
	}
	chp_str = (char *)calloc(strlen(chp_name) + strlen(chp_value) + strlen(chp_id) * 2
				 + strlen(cha_checked) + strlen(chp_message) + 512, sizeof(char));
	sprintf(chp_str, "<input type=\"checkbox\" id=\"%s\" name=\"%s\" value=\"%s\" %s><label for=\"%s\">%s</label>\n"
		 , chp_id, chp_name, chp_value, cha_checked, chp_id, chp_message);
	Put_Nlist(nlp_out, chp_insert, chp_str);
	free(chp_str);
	return;
}

/*
+* =======================================================================
 * Function:            Get_Create_Time_By_Entry_Id()
 * Description:
 *
%* ========================================================================
 * Return:
 *			���ｪλ 0
 *      �����          ���顼�� CO_ERROR
=* ========================================================================*/
int Get_Create_Time_By_Entry_Id(DBase *db, int in_entry_id, int in_blog, char *chp_create_time)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];

	strcpy(cha_sql, "select date_format(T1.d_entry_create_time, '%Y%m%d%H%i%s')");
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp && *chp_tmp) {
		strcpy(chp_create_time, chp_tmp);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ========================================================================
 * Function:		Get_Total_Entry
 * Description:
 *			����ȥ�����
%* ========================================================================
 * Return:
 *	����ȥ꡼�������顼����CO_ERROR
-* ======================================================================*/
int Get_Total_Entry(
	  DBase *db
	, NLIST *nlp_in
	, NLIST *nlp_out
	, int in_mode	/* 0,���� 1,������Τ� */
	, int in_blog
)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[128];
	int in_total_entry;

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from at_entry");
	if (in_mode != 0) {
		strcat(cha_sql, " where b_mode != 0 and b_mode is not null");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	} else {
		sprintf(cha_sql + strlen(cha_sql), " where n_blog_id = %d", in_blog);
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp){
		Db_CloseDyna(dbres);
		return 0;
	}
	in_total_entry = atoi(chp_tmp);
	Db_CloseDyna(dbres);

	return in_total_entry;
}

/*
+* ========================================================================
 * Function:		Get_Total_Comment
 * Description:
 *					�����Ȥ����
%* ========================================================================
 * Return:
 *	 �����Ȥ���������顼����CO_ERROR
-* ======================================================================*/
int Get_Total_Comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_all)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[128];
	int in_total_comment;

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from at_comment");
	if (in_all) {
		sprintf(cha_sql + strlen(cha_sql), " where n_blog_id = %d", in_blog);
	} else {
		strcat(cha_sql, " where b_comment_accept != 0");
		sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "��������������륯���꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp){
		Put_Nlist(nlp_out, "ERROR", "���������������Ǥ��ޤ���Ǥ�����");
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_total_comment = atoi(chp_tmp);
	Db_CloseDyna(dbres);

	return in_total_comment;
}

/*
+* ========================================================================
 * Function:		Get_Total_Comment_By_Entry_Id
 * Description:
 *	����N���Ф��륳���Ȥ����
%* ========================================================================
 * Return:
 *	 �����Ȥ���� ���顼����CO_ERROR
-* ======================================================================*/
int Get_Total_Comment_By_Entry_Id(
	  DBase *db
	, NLIST *nlp_in
	, NLIST *nlp_out
	, int in_entry_id
	, int in_blog
	, int in_all
)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[128];
	int in_total_comment;

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from at_comment");
	if (in_all) {
		sprintf(cha_sql + strlen(cha_sql), " where n_entry_id = %d", in_entry_id);
	} else {
		strcat(cha_sql, " where b_comment_accept != 0");
		sprintf(cha_sql + strlen(cha_sql), " and n_entry_id = %d", in_entry_id);
	}
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "��������������륯���꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_total_comment = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_total_comment;
}

/*
+* ========================================================================
 * Function:		Get_Total_Trackback
 * Description:
 *	�ȥ�å��Хå������
%* ========================================================================
 * Return:
 *	�ȥ�å��Хå������
-* ======================================================================*/
int Get_Total_Trackback(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_all)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[128];
	int in_total_trackback;

	if (in_all) {
		strcpy(cha_sql, "select count(T1.n_tb_id)");
		strcat(cha_sql, " from at_trackback T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	} else {
		strcpy(cha_sql, "select count(T1.n_tb_id)");
		strcat(cha_sql, " from at_trackback T1");
		strcat(cha_sql, " where T1.b_tb_accept != 0");
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_total_trackback = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_total_trackback;
}

/*
+* ========================================================================
 * Function:		Get_Total_Trackback_By_Entry_Id
 * Description:
 *	����IDin_entry_id�Υȥ�å��Хå������
%* ========================================================================
 * Return:
 *	 �ȥ�å��Хå������
-* ======================================================================*/
int Get_Total_Trackback_By_Entry_Id(
	  DBase *db
	, NLIST *nlp_in
	, NLIST *nlp_out
	, int in_entry_id
	, int in_blog
	, int in_all
)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[256];
	int in_total_trackback;

	if (in_all) {
		strcpy(cha_sql, "select count(T1.n_tb_id)");
		strcat(cha_sql, " from at_trackback T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry_id);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	} else {
		strcpy(cha_sql, "select count(T1.n_tb_id)");
		strcat(cha_sql, " from at_trackback T1");
		strcat(cha_sql, " where T1.b_tb_accept != 0");
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_entry_id = %d", in_entry_id);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_total_trackback = atoi(chp_tmp);
	Db_CloseDyna(dbres);

	return in_total_trackback;
}

/*
+* ========================================================================
 * Function:	 	Get_Id_Latest_Entry_Current_Time()
 * Description:
 *	����������������߰����ǺǤ⿷��������������⡼�ɤε�����ID�����
%* ========================================================================
 * Return:
 *		���ｪλ ����ID���ʤ�����0
 *		���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Latest_Entry_Current_Time(DBase *db, int in_blog)
{
//	struct tm* stp_now;
//	time_t tm_now;
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
//	char cha_timestamp[32];
	int in_latest_entry_id;

//	tm_now = time(NULL);
//	stp_now = localtime(&tm_now);
//	memset(cha_timestamp, '\0', sizeof(cha_timestamp));
//	sprintf(cha_timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
//		stp_now->tm_year+1900, stp_now->tm_mon + 1, stp_now->tm_mday, stp_now->tm_hour, stp_now->tm_min, stp_now->tm_sec);
	strcpy(cha_sql, "select T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where b_mode = 1");
//	sprintf(cha_sql + strlen(cha_sql), " and T1.d_entry_create_time <= '%s'", cha_timestamp);
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_entry_create_time desc limit 1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp || !*chp_tmp){
		Db_CloseDyna(dbres);
		return 0;
	}
	in_latest_entry_id = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_latest_entry_id;
}

/*
+* ========================================================================
 * Function:	 	Get_Id_Latest_Entry()
 * Description:
 *	����ID�κ����ͤ���
%* ========================================================================
 * Return:
 *	����ID�κ����͡������ʤ��Ȥ���0��
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Latest_Entry(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_latest_entry_id;

	strcpy(cha_sql, "select max(T1.n_entry_id)");
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	/* ������1���ʤ� */
	if (!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		return 0;
	}
	if (!Db_GetValue(dbres, 0, 0)) {
		Db_CloseDyna(dbres);
		return 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	in_latest_entry_id = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_latest_entry_id;
}

/*
+* ========================================================================
 * Function:	Get_Id_Latest_Entry_Create_Time_Not_Draft()
 * Description:
 *	���񵭻�����ǵ������դ��Ǥ⿷������Τε���ID
%* ========================================================================
 * Return:
 *	���񵭻�����ǵ������դ��Ǥ⿷������Τε���ID
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Latest_Entry_Create_Time_Not_Draft(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_latest_entry_id;

	strcpy(cha_sql, "select T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	strcat(cha_sql, " order by T1.d_entry_create_time desc");
	strcat(cha_sql, ", T1.n_entry_id desc");
	strcat(cha_sql, " limit 1");
/*
	strcpy(cha_sql, "select T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_entry_create_time desc, T1.n_entry_id desc");
	strcat(cha_sql, " limit 1");
*/
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if (!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_latest_entry_id = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_latest_entry_id;
}

/*
+* ========================================================================
 * Function:		Get_Id_Oldest_Entry_Create_Time_Not_Draft()
 * Description:
 * 	���񵭻���ǵ������դ��Ǥ�Ť������ε���ID
%* ========================================================================
 * Return:
 *	���񵭻���ǺǸŤε���ID
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Oldest_Entry_Create_Time_Not_Draft(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_oldest_entry_id;

	strcpy(cha_sql, "select T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	strcat(cha_sql, " order by T1.d_entry_create_time asc");
	strcat(cha_sql, ", n_entry_id asc");
	strcat(cha_sql, " limit 1");
/*
	strcpy(cha_sql, "select T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_entry_id asc");
	strcat(cha_sql, " limit 1");
*/
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if (!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_oldest_entry_id = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_oldest_entry_id;
}

/*
+* ========================================================================
 * Function:		Get_Id_Prev_Entry_Create_Time()
 * Description:
 *	���λؼ���ƻ��֤ε���ID����
%* ========================================================================
 * Return:
 *	���ꤷ��ID�������Ρ�������ƻ��֤ˤ����뵭����ID
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Prev_Entry_Create_Time(DBase *db, int in_entry_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	char cha_create_time[32];
	int in_prev_eid;
	int in_ret;

	in_ret = Get_Create_Time_By_Entry_Id(db, in_entry_id, in_blog, cha_create_time);
	if (in_ret == CO_ERROR) {
		return CO_ERROR;
	}
	strcpy(cha_sql, "select	T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and (T1.d_entry_create_time < '%s' or (T1.d_entry_create_time = '%s' and T1.n_entry_id < %d))", cha_create_time, cha_create_time, in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	strcat(cha_sql, " order by T1.d_entry_create_time desc, T1.n_entry_id desc");
	strcat(cha_sql, " limit 1");
/*
	strcpy(cha_sql, "select	T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_entry_id < %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_entry_id desc");
	strcat(cha_sql, " limit 1");
*/
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if (!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_prev_eid = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_prev_eid;
}

/*
+* ========================================================================
 * Function:	 	Get_Id_Next_Entry_Create_Time()
 * Description:
 *	���λؼ���ƻ��֤ε���ID����
%* ========================================================================
 * Return:
 *	���ꤷ��ID�������Ρ�������ƻ��֤ˤ����뵭����ID
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Next_Entry_Create_Time(DBase *db, int in_entry_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	char cha_create_time[16];
	int in_next_eid;
	int in_ret;

	in_ret = Get_Create_Time_By_Entry_Id(db, in_entry_id, in_blog, cha_create_time);
	if (in_ret == CO_ERROR) {
		return CO_ERROR;
	}
	strcpy(cha_sql, "select	T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and (T1.d_entry_create_time > '%s' or (T1.d_entry_create_time = '%s' and T1.n_entry_id > %d))", cha_create_time, cha_create_time, in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	strcat(cha_sql, " order by T1.d_entry_create_time asc");
	strcat(cha_sql, ", T1.n_entry_id asc");
	strcat(cha_sql, " limit 1");
/*
	strcpy(cha_sql, "select	T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_entry_id > %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_entry_id asc");
	strcat(cha_sql, " limit 1");
*/
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_next_eid = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_next_eid;
}

/*
+* ========================================================================
 * Function:		Exist_Entry_Id()
 * Description:
 *	���ꤵ�줿����ID�ε�����¸�ߤ��뤫�ɤ���Ƚ�Ǥ���
%* ========================================================================
 * Return:
 *	0: �ʤ�
 *	1: ����
-* ========================================================================*/
int Exist_Entry_Id(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];

	strcpy(cha_sql, "select T1.n_entry_id");
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "������¸�ߤ��뤫�ɤ�����Ĵ�٤륯���꡼�˼��Ԥ��ޤ�����");
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp){
		Db_CloseDyna(dbres);
		return 0;
	}
	Db_CloseDyna(dbres);
	return 1;
}

/*
+* ========================================================================
 * Function:	 	Get_Count_Category()
 * Description:
 *	��Ͽ����Ƥ��륫�ƥ��꡼��������
%* ========================================================================
 * Return:
 *	���ƥ��꡼��
 *	���顼��CO_ERROR
-* ========================================================================*/
int Get_Count_Category(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[128];
	int in_total;

	strcpy(cha_sql, "select count(T1.n_category_id)");
	strcat(cha_sql, " from at_category T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_total = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_total;
}

/*
+* ========================================================================
 * Function:	 	Get_Latest_Entry_Year()
 * Description:
 *	��ɽ���Ƥ��ơ��Ǥ�̤��λؼ����ǯ������
%* ========================================================================
 * Return:
 *	�ǿ����ǯ
 *		���顼�� CO_ERROR
-* ========================================================================*/
int Get_Latest_Entry_Year(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_year;

	strcpy(cha_sql, "select max(T1.d_entry_create_time)");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where b_mode = 1");
	strcat(cha_sql, " and T1.d_entry_create_time < now()");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp || !*chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	/* ǯ�Ƕ��ڤ뤿�� */
	chp_tmp[4] = '\0';
	in_year = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_year;
}

/*
+* ========================================================================
 * Function:	 	Get_Oldest_Entry_Year()
 * Description:
 *	�������Ƥ��ơ��ǸŤλؼ����ǯ������
%* ========================================================================
 * Return:
 *	���ｪλ �ǸŤ����ǯ
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Oldest_Entry_Year(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[128];
	int in_year;

	strcpy(cha_sql, "select min(T1.d_entry_create_time)");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if (!chp_tmp || !*chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	/* ǯ�Τ�ɬ�� */
	chp_tmp[4] = '\0';
	in_year = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_year;
}

/*
+* ========================================================================
 * Function:	 	Get_Category_By_Cid()
 * Description:
 *	���ƥ��꡼ID���饫�ƥ��꡼̾���ꡢchp_category�˥��ԡ������᤹��
%* ========================================================================
 * Return:
 *	���� 0
 *	���� 1
-* ========================================================================*/
int Get_Category_By_Cid(
	  DBase *db
	, NLIST *nlp_out
	, int in_cid
	, int in_blog
	, char *chp_category
)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];

	strcpy(cha_sql, "select T1.c_category_name");
	strcat(cha_sql, " from at_category T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_category_id =%d", in_cid);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		chp_category = NULL;
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(chp_category, chp_tmp);
	Db_CloseDyna(dbres);
	return 0;

}

/*
+* ========================================================================
 * Function:	 	Get_Count_Link_In_Linklist()
 * Description:
 *	Ǥ�դΥ�󥯥��롼��in_linkgroup_id�˴ޤޤ���󥯿����֤�
%* ========================================================================
 * Return:
 *	���ｪλ ��󥯿�
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Count_Link_In_Linklist(
	  DBase *db
	, NLIST *nlp_in
	, NLIST *nlp_out
	, int in_linkgroup_id
	, int in_blog
)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_count;

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from at_linkitem T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_linkgroup_id = %d", in_linkgroup_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_count = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_count;
}

/*
+* ========================================================================
 * Function:	 	Get_Flag_Public()
 * Description:
 * 	����or��������
%* ========================================================================
 * Return:
 *	���ｪλ
 *		���� 1
 *		���� 0
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Flag_Public(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_flag;

	strcpy(cha_sql, "select T1.b_mode");
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", "�����⡼�ɤμ����˼��Ԥ��ޤ�����<br>");
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����⡼�ɤμ����˼��Ԥ��ޤ�����<br>");
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_flag = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_flag;
}

/*
+* ========================================================================
 * Function:	 	Get_Flag_Acceptable_Comment()
 * Description:
 * 	Ǥ�դε����Υ����ȼ��եե饰�ͤμ���
%* ========================================================================
 * Return:
 *	���ｪλ
 *		�����դ��� 1
 *		�����դ��ʤ� 0
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Flag_Acceptable_Comment(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_flag;
	int in_ret;

	strcpy(cha_sql, "select T1.b_comment");
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", "�����ȼ����դ����ݾ���μ����˼��Ԥ��ޤ�����<br>");
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����ȼ����դ����ݾ���μ����˼��Ԥ��ޤ�����<br>");
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_flag = atoi(chp_tmp);
	Db_CloseDyna(dbres);

	if (g_in_dbb_mode && in_flag == CO_CMTRB_FRIEND) {
		in_ret = Is_Ball_Friend(db, nlp_out, in_blog, in_login_blog);
		if (in_ret < 0) {
			return CO_ERROR;
		}
		if (!in_ret) {
			in_flag = CO_CMTRB_DENY;
		}
	}

	return in_flag;
}

/*
+* ========================================================================
 * Function:	 	Get_Flag_Acceptable_Trackback()
 * Description:
 * 	Ǥ�դε����Υȥ�å��Хå����եե饰�ͤμ���
%* ========================================================================
 * Return:
 *	���ｪλ
 *		�����դ��� 1
 *		�����դ��ʤ� 0
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Flag_Acceptable_Trackback(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_flag;

	strcpy(cha_sql, "select T1.b_trackback");
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "�������Ѽ����դ����ݾ���μ����˼��Ԥ��ޤ�����<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå������դ����ݾ���μ����˼��Ԥ��ޤ�����<br>");
		}
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp){
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "�������Ѽ����դ����ݾ���μ����˼��Ԥ��ޤ�����<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå������դ����ݾ���μ����˼��Ԥ��ޤ�����<br>");
		}
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_flag = atoi(chp_tmp);
	Db_CloseDyna(dbres);

	return in_flag;
}

/*
+* ========================================================================
 * Function:	 	Check_Space_Only()
 * Description:
 *	����ʸ��+���ѥ��ڡ����Τߤ����ϥ����å�
%* ========================================================================
 * Return:
 *	�����
 *		1 : ����ʸ�������ѥ��ڡ����Τ�
 *		0 : �嵭�ʳ�
-* ========================================================================*/
int Check_Space_Only(char *chp_str)
{
	unsigned char *uchp_source;
	int in_count;
	int i;
	int in_flag;

	in_count = strlen(chp_str);
	uchp_source = (unsigned char *)calloc(in_count + 1, sizeof(unsigned char));
	strcpy(uchp_source, chp_str);
	in_flag = 1;
	for(i = 0; i < in_count; i++) {
		/* ����ʸ���ΤȤ� */
		if (uchp_source[i] == 0x8e || (uchp_source[i] >= 0xa0 && uchp_source[i] <= 0xfe)) {
			/* ���ѥ��ڡ����ʳ������줿��break */
			if (strncmp(&uchp_source[i], "��", 2) != 0) {
				in_flag = 0;
				break;
			}
			/* ���;ʬ�˿ʤ�� */
			i++;
		/* Ⱦ��ʸ���ΤȤ� */
		} else {
			/* ����ʸ���ʳ������줿��֥졼�� */
			if (isspace(uchp_source[i]) == 0) {
				in_flag = 0;
				break;
			}
		}
	}
	free(uchp_source);
	return in_flag;
}

/*
+* ========================================================================
 * Function:		Check_URL_With_Crlf
 * Include-file:
 * Description:
 *	���ԤǶ��ڤ�줿URL������å���
 *	(2005.08.23�ɵ�)
 *	strtok�η�͹礤�Ǥɤ����褦��ʤ��ʤ뤿�ᡢ2��������˼�롣
%* ========================================================================
 * Return:
 *	���� 0
 * 	���Ϥ˸�� ���顼��
 *	����̵�� CO_ERROR
-* ========================================================================*/
int Check_URL_With_Crlf(NLIST *nlp_out, char *chp_source, const char *chp_name, int in_max)
{
	char **chpp_url;
	char **chpp_tmp;
	char *chp_copied;
	char *chp_buf;
	int in_error;
	int in_count;
	int i;

	/* strtok��Ȥ��ΤǤ��ä���¾�ΥХåե��ء� */
	chp_copied = (char *)calloc(strlen(chp_source) + 1, sizeof(char));
	if (chp_copied == NULL) {
		Put_Format_Nlist(nlp_out, "ERROR", "%sURL��ǧ��˥��꤬��­���ޤ�����<br>", chp_name);
		return CO_ERROR;
	}
	strcpy(chp_copied, chp_source);
	/* initialize */
	chpp_url = NULL;
	chpp_tmp = NULL;
	in_count = 0;
	in_error = 0;
	/* ���Ԥ��Ȥ�ʬ���ơ�2��������˼�� */
	chp_buf = strtok(chp_copied, "\r\n");
	while(chp_buf != NULL) {
		/* �Ǥ������realloc���򤱤����Τǡ�10���1��realloc����褦�ˤ��� */
		/* realloc���֤��ͤ򸵤�Ʊ���ѿ��˼��ȡ����Ԥ����Ȥ��˸���free�Ǥ��ʤ��ʤ뤿�����ѿ��˼�롣 */
		if (in_count % 10 == 0) {
			if (in_count == 0) {
				chpp_tmp = (char**)calloc(10, sizeof(char*));
			} else {
				chpp_tmp = (char**)realloc(chpp_url, sizeof(char*) * (in_count + 10));
			}
		}
		if (!chpp_tmp) {
			Put_Format_Nlist(nlp_out, "ERROR", "%sURL��ǧ��˥��꤬��­���ޤ�����<br>", chp_name);
			in_error++;
			break;
		}
		chpp_url = chpp_tmp;
		/* chpp�Τ�������ΰ���� */
		chpp_url[in_count] = (char*)calloc(strlen(chp_buf) + 1, sizeof(char));
		if (!chpp_url[in_count]) {
			Put_Format_Nlist(nlp_out, "ERROR", "%sURL��ǧ��˥��꤬��­���ޤ�����<br>", chp_name);
			in_error++;
			break;
		}
		/* ���ԡ����� */
		strcpy(chpp_url[in_count], chp_buf);
		/* ����õ�� */
		chp_buf = strtok(NULL, "\r\n");
		in_count++;
		if (in_count > in_max) {
			Put_Format_Nlist(nlp_out, "ERROR", "%s�� %d URL ����ˤ��Ƥ���������<br>", chp_name, in_max);
			in_error++;
			break;
		}
	}
	/* ���顼�����ä���ե꡼���ƥ��顼���֤� */
	if (in_error) {
		for(; in_count > 0; in_count--) {
			free(chpp_url[in_count - 1]);
		}
		if (chpp_url) {
			free(chpp_url);
		}
		free(chp_copied);
		return CO_ERROR;
	}
	/* �ΰ���ݤ��Ƽ�ä��ͤ��줾���Check_URL */
	for(i = 0; i < in_count; i++) {
		if (Check_URL(chpp_url[i])) {
			Put_Format_Nlist(nlp_out, "ERROR", "%sURL�˸�꤬����ޤ����������ǧ���Ƥ���������<br>", chp_name);
			in_error++;
			break;
		}
	}
	/* �ΰ賫�� */
	for(i = 0; i < in_count; i++) {
		free(chpp_url[i]);
	}
	/* chpp_url�ϡ�in_count��0�ʤ�NULL */
	if (in_count > 0) {
		free(chpp_url);
	}
	free(chp_copied);
	return in_error;
}

#if 0
/*
+* ========================================================================
 * Function:		Check_Alnum_Opt
 * Include-file:
 * Description:
 *	Check_Alnum���ĥ���ơ��㳰ʸ��������դ�����褦�ˤ����Τߡ�
%* ========================================================================
 * Return:
 *	���� 0
 * 	���顼 1
-* ========================================================================*/
int Check_Alnum_Opt(char *chp_source, char *chp_reject)
{
	int in_length;
	int i;

	in_length = strlen(chp_source);
	for(i = 0; i < in_length; i++) {
		if (isalnum(chp_source[i]) == 0) {
			if (strchr(chp_reject, chp_source[i]) == NULL) {
				return 1;
			}
		}
	}
	return 0;
}

/*
+* =========================================================================
 * Function:            Remove_HtmlTags()
 * Description:
 *	HTML�������������
 *	��������������������������Ƥ��ʤ��ä��ꤹ���
 *	�Ѥ�ʸ�����֤롣
 *	�֤��ͤϥե꡼����ɬ�פ����롣
%* =========================================================================
 * Return:
 *		�������������char*
-* ========================================================================*/
char *Remove_HtmlTags(char *chp_source)
{
	char *chp_dest;
	int in_source_length;
	int in_dest_length;
	int in_comment_flg;
	int in_tag_flg;
	int i;

	/* �Ȥꤢ��������¼�� */
	in_source_length = strlen(chp_source);
	chp_dest = (char *)calloc(in_source_length + 1, sizeof(char));
	if (!chp_dest) {
		free(chp_source);
		return NULL;
	}
	for(i = 0, in_tag_flg = 0, in_comment_flg = 0, in_dest_length = 0; i < in_source_length; i++) {
		/* �����ե饰�����äƤ���Ȥ��ϡ�">"�򸫤Ĥ����Ȥ��˥ե饰�򤪤��� */
		if (in_tag_flg) {
			if (chp_source[i] == '>') {
				in_tag_flg = 0;
			}
		/* �����ȥե饰��Ω�äƤ���Ȥ��ϡ�"-->"�򸫤Ĥ����Ȥ��˥ե饰�򤪤��� */
		} else if (in_comment_flg) {
			if (strncmp(&chp_source[i], "-->", strlen("-->")) == 0) {
				in_comment_flg = 0;
				i += (strlen("-->") - 1);	/* "->"���ɤ߹��ޤʤ��褦�����Ф���forʸ��+1���θ */
			}
		/* Ω�äƤ��ʤ��Ȥ��ϡ��������ϤޤäƤ��ʤ�����Ƚ�ǡ� */
		} else {
			/* <!--�ǻϤޤꡢ�ʹߤ�-->�������Τϥ����Ȥȼ�롣 */
			if (strncmp(&chp_source[i], "<!--", strlen("<!--")) == 0 && strstr(&chp_source[i], "-->") != NULL) {
				in_comment_flg = 1;
			/* �����ϡ�'<'�ȡ�ľ��˱ѻ�,'!','/'�Τ����줫��³���� */
			/* ���ġ�'>'���ʹߤˤ����Τ򥿥����Ϥȸ��Ƥ���褦���� */
			/* ���ץ֥饦���Ǥ⡢�ʾ�ξ������ƤϤޤ��ΤΤߤ򥿥����Ϥȸ��Ƥ���(�褦��)�� */
			} else if (chp_source[i] == '<' && (isalpha(chp_source[i + 1]) != 0 || chp_source[i + 1] == '!' || chp_source[i + 1] == '/') && strchr(&chp_source[i], '>') != NULL) {
				in_tag_flg = 1;
			} else {
				chp_dest[in_dest_length] = chp_source[i];
				in_dest_length++;
			}
		}
	}
	/* �Ĥ��� */
	chp_dest[in_dest_length] = '\0';
	return chp_dest;
}
#endif

/*
+* =========================================================================
 * Function:	 	Create_Rss()
 * Description:
 *	���ۤ���RSS�ե������������롣
 *	����Ū�ʻ��ͤ˹�碌��UTF-8�Υե�����ˤ���ɬ�פ����롣
%* =========================================================================
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ========================================================================*/
int Create_Rss(DBase *db, NLIST *nlp_out, int in_blog)
{
	CP_NKF nkf;
	DBRes *dbres;
	FILE *fp;
	FILE *fp_out;
	char *chp_tmp;
	char *chp_escape;
	char cha_blog_title[512];
	char cha_blog_description[512];
	char cha_blog_author[512];
	char cha_sql[512];
	char cha_date[30];
	char cha_temp[1024];
	char cha_temprdf[512];
	int in_count;
	int in_fd;
	int i;

	memset(cha_blog_title, '\0', sizeof(cha_blog_title));
	memset(cha_blog_description, '\0', sizeof(cha_blog_description));
	memset(cha_blog_author, '\0', sizeof(cha_blog_author));
	strcpy(cha_sql, " select T1.c_blog_title");		/* 0 �֥������ȥ� */
	strcat(cha_sql, ",T1.c_blog_description");		/* 1 �֥����� */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�֥���������륯����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		strcpy(cha_blog_title, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		strcpy(cha_blog_description, chp_tmp);
	}
	Db_CloseDyna(dbres);
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select T1.c_author_nickname");	/* 0 �˥å��͡��� */
	strcat(cha_sql, " from at_profile T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "���Ծ�������륯����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		strcpy(cha_blog_author, chp_tmp);
	}
	Db_CloseDyna(dbres);

	sprintf(cha_temprdf, "%s/rdf-%d-tmpXXXXXXXX", g_cha_rss_path, in_blog);
	in_fd = mkstemp(cha_temprdf);
	if (!in_fd) {
		Put_Nlist(nlp_out, "ERROR", "�ƥ�ݥ��ե�����κ����˼��Ԥ��ޤ�����");
		return 1;
	}

	fp = fdopen(in_fd, "r+");
	if (!fp) {
		close(in_fd);
		remove(cha_temprdf);
		Put_Nlist(nlp_out, "ERROR", "�ե�����Υ����ץ�˼��Ԥ��ޤ�����");
		return 1;
	}
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n");	/* ���Ȥ�iconv��EUC����ɸ��UTF-8���Ѵ� */
	fprintf(fp, "<rdf:RDF\n");
	fprintf(fp, " xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n");
	fprintf(fp, " xmlns=\"http://purl.org/rss/1.0/\"\n");
	fprintf(fp, " xmlns:content=\"http://purl.org/rss/1.0/modules/content/\"\n");
	fprintf(fp, " xmlns:taxo=\"http://purl.org/rss/1.0/modules/taxonomy/\"\n");
	fprintf(fp, " xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n");
	fprintf(fp, " xmlns:syn=\"http://purl.org/rss/1.0/modules/syndication/\"\n");
	fprintf(fp, " xmlns:admin=\"http://webns.net/mvcb/\">\n");
	chp_tmp = getenv("SERVER_NAME");
	if (!chp_tmp) {
		pclose(fp);
		Put_Nlist(nlp_out, "ERROR", "�ѥ��μ����˼��Ԥ��ޤ�����");
		return 1;
	}
	fprintf(fp, "<channel rdf:about=\"%s%s%s/%08d.rdf\">\n", g_cha_protocol, chp_tmp, g_cha_rss_location, in_blog);
	chp_escape = Remove_HtmlTags(cha_blog_title);
	chp_tmp = Escape_HtmlString(chp_escape);
	fprintf(fp, "<title>%s</title>\n", chp_tmp);
	free(chp_tmp);
	free(chp_escape);
	if (g_in_short_name) {
		fprintf(fp, "<link>%s%s%s/%08d/</link>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
	} else {
		fprintf(fp, "<link>%s%s%s/%s?bid=%d</link>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
	}
	chp_escape = Remove_HtmlTags(cha_blog_description);
	chp_tmp = Escape_HtmlString(chp_escape);
	fprintf(fp, "<description>%s</description>\n", chp_escape);
	free(chp_tmp);
	free(chp_escape);
	fprintf(fp, "<dc:language>ja</dc:language>\n");
	fprintf(fp, "<items>\n");
	fprintf(fp, "<rdf:Seq>\n");
	strcpy(cha_sql, " select T1.n_entry_id");	/* 0 ����ID */
	strcat(cha_sql, ",T1.c_entry_title");		/* 1 ���������ȥ� */
	strcat(cha_sql, ",T1.c_entry_summary");		/* 2 �������� */
	strcat(cha_sql, ",T1.c_entry_body");		/* 3 �������� */
	strcat(cha_sql, ",T1.d_entry_create_time");	/* 4 ������ƻ��� */
	strcat(cha_sql, ",T2.c_category_name");		/* 5 ���ƥ��꡼̾ */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, ",at_category T2");
	strcat(cha_sql, " where T1.b_mode = 1");		/* ������ */
	strcat(cha_sql, " and T1.n_category_id = T2.n_category_id");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id=%d", in_blog);
	sprintf(cha_sql + strlen(cha_sql), " order by T1.d_entry_create_time desc, T1.n_entry_id desc limit %d", CO_RSS_MAX);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		fclose(fp);
		remove(cha_temprdf);
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "������������륯���꡼�˼��Ԥ��ޤ�����<br>(%s)", Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	for (i = 0; i < in_count; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp) {
			if (g_in_short_name) {
				sprintf(cha_temp, "%s%s%s/%08d/?eid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, atoi(chp_tmp));
			} else {
				sprintf(cha_temp, "%s%s%s/%s?eid=%d&bid=%d"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML
					, atoi(chp_tmp), in_blog);
			}
			chp_escape = Escape_HtmlString(cha_temp);
			fprintf(fp, "<rdf:li rdf:resource=\"%s\" />\n", chp_escape);
			free(chp_escape);
		} else {
			fclose(fp);
			remove(cha_temprdf);
			Db_CloseDyna(dbres);
			Put_Nlist(nlp_out, "ERROR", "RSS�������˼��Ԥ��ޤ�����");
			return 1;
		}
	}
	fprintf(fp, "</rdf:Seq>\n");
	fprintf(fp, "</items>\n");
	fprintf(fp, "</channel>\n\n");
	for (i = 0; i < in_count; i++) {	/* <item rdf:about=""> */
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp){
			if (g_in_short_name) {
				sprintf(cha_temp, "%s%s%s/%08d/?eid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, atoi(chp_tmp));
			} else {
				sprintf(cha_temp, "%s%s%s/%s?eid=%d&bid=%d"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML
					, atoi(chp_tmp), in_blog);
			}
			chp_escape = Escape_HtmlString(cha_temp);
			fprintf(fp, "<item rdf:about=\"%s\" >\n", chp_escape);
			free(chp_escape);
			if (g_in_short_name) {
				sprintf(cha_temp, "%s%s%s/%08d/?eid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, atoi(chp_tmp));
			} else {
				sprintf(cha_temp, "%s%s%s/%s?eid=%d&bid=%d"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML
					, atoi(chp_tmp), in_blog);
			}
			chp_escape = Escape_HtmlString(cha_temp);
			fprintf(fp, "<link>%s</link>\n", chp_escape);
			free(chp_escape);
		} else {
			fclose(fp);
			remove(cha_temprdf);
			Db_CloseDyna(dbres);
			Put_Nlist(nlp_out, "ERROR", "RSS�������˼��Ԥ��ޤ�����");
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (chp_tmp){
			strcpy(cha_blog_title, chp_tmp);
		}
		/* <title> �����Υ����ȥ� */
		chp_escape = Remove_HtmlTags(cha_blog_title);
		chp_tmp = Escape_HtmlString(chp_escape);
		fprintf(fp, "<title>%s</title>\n", chp_escape);
		free(chp_tmp);
		free(chp_escape);
		/* <description> ���פ򼨤��� */
		if (Db_GetValue(dbres, i, 2) && *Db_GetValue(dbres, i, 2)) {
			/* ���פ�CO_MAXLEN_ENTRY_SUMMARY��Ķ���ʤ��Ϥ� */
			chp_escape = Remove_HtmlTags(Db_GetValue(dbres, i, 2));
			chp_tmp = Escape_HtmlString(chp_escape);
			fprintf(fp, "<description>%s</description>\n", chp_escape);
			free(chp_tmp);
			free(chp_escape);
		} else if (Db_GetValue(dbres, i, 3) && *Db_GetValue(dbres, i, 3)) {
			/* ��ʸ���פȤ��ƻ��Ѥ������������ڤ� */
			chp_tmp = Remove_HtmlTags(Db_GetValue(dbres, i, 3));
			chp_escape = Chop_String(chp_tmp, CO_MAXLEN_ENTRY_SUMMARY);
			free(chp_tmp);
			chp_tmp = Escape_HtmlString(chp_escape);
			fprintf(fp, "<description>%s</description>\n", chp_tmp);
			free(chp_tmp);
			free(chp_escape);
		} else {
			fprintf(fp, "<description></description>\n");
		}
		/* <dc:creator> ���� */
		chp_escape = Escape_HtmlString(cha_blog_author);
		fprintf(fp, "<dc:creator>%s</dc:creator>\n", chp_escape);
		free(chp_escape);
		/* <dc:subject> ��˴ޤޤ�륫�ƥ���ʤɤ򼨤� */
		chp_tmp = Db_GetValue(dbres, i, 5);
		if (chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			fprintf(fp, "<dc:subject>%s</dc:subject>\n", chp_escape);
			free(chp_escape);
		}
		/* <content:encoded> �����򤽤Τޤ�ȯ�Ԥ��롣<![CDATA[]]>����Ѥ��Ƥ���Τǥ����Ϥ��Τޤ� */
		chp_tmp = Db_GetValue(dbres, i, 3);
		if (chp_tmp) {
			fprintf(fp, "<content:encoded><![CDATA[%s\n]]></content:encoded>\n", chp_tmp);
		}
		chp_tmp = Db_GetValue(dbres, i, 4);
		if (chp_tmp){
			sprintf(cha_date, chp_tmp);
			memset(cha_temp, '\0', sizeof(cha_temp));
			sprintf(cha_temp, "%.4s-%.2s-%.2sT%.2s:%.2s:%.2s+09:00"
				, cha_date, cha_date + 5, cha_date + 8, cha_date + 11, cha_date + 14, cha_date + 17);
		}
		fprintf(fp, "<dc:date>%s</dc:date>\n", cha_temp);
		fprintf(fp, "</item>\n\n");
	}
	fprintf(fp, "</rdf:RDF>\n");
	Db_CloseDyna(dbres);
	rewind(fp);

	sprintf(cha_temp, "%s/%08d.rdf", g_cha_rss_path, in_blog);
	remove(cha_temp);
	fp_out = fopen(cha_temp, "w+");
	if (!fp_out) {
		fclose(fp);
		remove(cha_temprdf);
		Put_Nlist(nlp_out, "ERROR", "�ե�����Υ����ץ�˼��Ԥ��ޤ�����");
		return 1;
	}

	nkf = Create_Nkf_Stream(fp);
	Set_Input_Code(nkf, NKF_EUC);
	Set_Output_Code(nkf, NKF_UTF8);
	Convert_To_Stream(nkf, fp_out);
	Destroy_Nkf_Engine(nkf);

	fclose(fp);
	fclose(fp_out);
	remove(cha_temprdf);

	return 0;
}

/*
+* =========================================================================
 * Function:	 	Write_Comment_To_Xml()
 * Description:
 *	����Υ֥��Υ�����XML�����ǻ���Υ��ȥ꡼��˽��Ϥ��롣
 *	����Ū�ʻ��ͤ˹�碌��UTF-8�Υե�����ˤ���ɬ�פ����롣
%* =========================================================================
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ========================================================================*/
int Write_Comment_To_Xml(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry, FILE* fp_out)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_tmp;
	char *chp_esc;
	int i;

	strcpy(cha_sql, " select");
	strcat(cha_sql, " T1.c_comment_body");			/* 0 ���������� */
	strcat(cha_sql, ",T1.c_comment_author");		/* 1 �����ȵ��Ҽ� */
	strcat(cha_sql, ",T1.c_comment_mail");			/* 2 ���Ҽԥ᡼�� */
	strcat(cha_sql, ",T1.c_comment_url");			/* 3 ���Ҽ�URL */
	strcat(cha_sql, ",T1.d_comment_create_time");	/* 4 �������� */
	strcat(cha_sql, ",T1.c_comment_ip");			/* 5 IP */
	strcat(cha_sql, " from at_comment T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id=%d", in_blog);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_entry_id=%d", in_entry);
	strcat(cha_sql, " order by T1.d_comment_create_time desc, T1.n_entry_id desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������������륯���꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	fprintf(fp_out, "<comments>\n");
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		fprintf(fp_out, "<comment>\n");
		fprintf(fp_out, "<title />\n");

		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<description>\n");
			fprintf(fp_out, "<![CDATA[");
			fprintf(fp_out, "%s", chp_esc);
			fprintf(fp_out, "]]>\n");
			fprintf(fp_out, "</description>\n");
		} else {
			fprintf(fp_out, "<description />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 1);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<name>%s</name>\n", chp_esc);
		} else {
			fprintf(fp_out, "<name />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 5);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<ip>%s</ip>\n", chp_esc);
		} else {
			fprintf(fp_out, "<ip />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 2);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<email>%s</email>\n", chp_esc);
		} else {
			fprintf(fp_out, "<email />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 3);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<url>%s</url>\n", chp_esc);
		} else {
			fprintf(fp_out, "<url />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 4);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<date>%s</date>\n", chp_esc);
		} else {
			fprintf(fp_out, "<date />\n");
		}
		free(chp_esc);

		fprintf(fp_out, "</comment>\n");
	}
	fprintf(fp_out, "</comments>\n");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* =========================================================================
 * Function:	 	Write_Trackback_To_Xml()
 * Description:
 *	����Υ֥��Υ�����XML�����ǻ���Υ��ȥ꡼��˽��Ϥ��롣
 *	����Ū�ʻ��ͤ˹�碌��UTF-8�Υե�����ˤ���ɬ�פ����롣
%* =========================================================================
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ========================================================================*/
int Write_Trackback_To_Xml(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry, FILE* fp_out)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_tmp;
	char *chp_esc;
	int i;

	strcpy(cha_sql, " select");
	strcat(cha_sql, " T1.c_tb_title");			/* 0 �����ȥ� */
	strcat(cha_sql, ",T1.c_tb_excerpt");		/* 1 ���� */
	strcat(cha_sql, ",T1.c_tb_blog_name");		/* 2 �֥�̾ */
	strcat(cha_sql, ",T1.c_tb_url");			/* 3 ���Ҽ�URL */
	strcat(cha_sql, ",T1.d_tb_create_time");	/* 4 �������� */
	strcat(cha_sql, ",T1.c_tb_ip");				/* 5 IP */
	strcat(cha_sql, " from at_trackback T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id=%d", in_blog);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_entry_id=%d", in_entry);
	strcat(cha_sql, " order by T1.d_tb_create_time desc, T1.n_entry_id desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������������륯���꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	fprintf(fp_out, "<trackbacks>\n");
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		fprintf(fp_out, "<trackback>\n");

		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<title>%s</title>\n", chp_esc);
		} else {
			fprintf(fp_out, "<title />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 1);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<excerpt>\n");
			fprintf(fp_out, "<![CDATA[");
			fprintf(fp_out, "%s", chp_esc);
			fprintf(fp_out, "]]>\n");
			fprintf(fp_out, "</excerpt>\n");
		} else {
			fprintf(fp_out, "<excerpt />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 2);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<blog_name>%s</blog_name>\n", chp_esc);
		} else {
			fprintf(fp_out, "<blog_name />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 3);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<url>%s</url>\n", chp_esc);
		} else {
			fprintf(fp_out, "<url />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 4);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<date>%s</date>\n", chp_esc);
		} else {
			fprintf(fp_out, "<date />\n");
		}
		free(chp_esc);

		chp_tmp = Db_GetValue(dbres, i, 5);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp_out, "<ip>%s</ip>\n", chp_esc);
		} else {
			fprintf(fp_out, "<ip />\n");
		}
		free(chp_esc);

		fprintf(fp_out, "</trackback>\n");
	}
	fprintf(fp_out, "</trackbacks>\n");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* =========================================================================
 * Function:	 	Write_To_Xml()
 * Description:
 *	����Υ֥������Τ�XML�����ǻ���Υ��ȥ꡼��˽��Ϥ��롣
 *	����Ū�ʻ��ͤ˹�碌��UTF-8�Υե�����ˤ���ɬ�פ����롣
%* =========================================================================
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ========================================================================*/
int Write_To_Xml(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, FILE* fp_out)
{
	CP_NKF nkf;
	DBRes *dbres;
	FILE *fp;
	char cha_sql[512];
	char cha_temprdf[512];
	char cha_nickname[100];
	char cha_fullname[200];
	char *chp_tmp;
	char *chp_esc;
	int in_fd;
	int i;

	sprintf(cha_temprdf, "%s/rdf-%d-tmpXXXXXXXX", g_cha_rss_path, in_blog);
	in_fd = mkstemp(cha_temprdf);
	if (!in_fd) {
		Put_Nlist(nlp_out, "ERROR", "�ƥ�ݥ��ե�����κ����˼��Ԥ��ޤ�����");
		return 1;
	}

	fp = fdopen(in_fd, "r+");
	if (!fp) {
		close(in_fd);
		remove(cha_temprdf);
		Put_Nlist(nlp_out, "ERROR", "�ե�����Υ����ץ�˼��Ԥ��ޤ�����");
		return 1;
	}

	fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n");
	fprintf(fp, "<blog>\n");

	strcpy(cha_sql, " select T1.c_blog_title");		/* 0 �֥������ȥ� */
	strcat(cha_sql, ",T1.c_blog_description");		/* 1 �֥����� */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�֥���������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		fclose(fp);
		remove(cha_temprdf);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
	if (chp_esc[0]) {
		fprintf(fp, "<name>%s</name>\n", chp_esc);
	} else {
		fprintf(fp, "<name />\n");
	}
	free(chp_esc);
	chp_tmp = Db_GetValue(dbres, 0, 1);
	chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
	if (chp_esc[0]) {
		fprintf(fp, "<description>\n");
		fprintf(fp, "<![CDATA[");
		fprintf(fp, "%s", chp_esc);
		fprintf(fp, "]]>\n");
		fprintf(fp, "</description>\n");
	} else {
		fprintf(fp, "<description />\n");
	}
	free(chp_esc);
	Db_CloseDyna(dbres);

	fprintf(fp, "<users>\n");
	fprintf(fp, "<user>\n");
	strcpy(cha_sql, "select T1.c_author_nickname");	/* 0 �˥å��͡��� */
	strcat(cha_sql, ",c_author_firstname");
	strcat(cha_sql, ",c_author_lastname");
	strcat(cha_sql, ",c_author_selfintro");
	strcat(cha_sql, " from at_profile T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id=%d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "���Ծ�������륯����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		fclose(fp);
		remove(cha_temprdf);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
	if (chp_esc[0]) {
		fprintf(fp, "<name>%s</name>\n", chp_esc);
	} else {
		fprintf(fp, "<name />\n");
	}
	strcpy(cha_nickname, chp_esc);
	free(chp_esc);

	chp_tmp = Db_GetValue(dbres, 0, 2);
	strcpy(cha_fullname, chp_tmp ? chp_tmp : "");
	if (cha_fullname[0])
		strcat(cha_fullname, " ");
	chp_tmp = Db_GetValue(dbres, 0, 1);
	strcat(cha_fullname, chp_tmp ? chp_tmp : "");
	chp_esc = Escape_HtmlString(cha_fullname);
	if (chp_esc[0]) {
		fprintf(fp, "<full_name>%s</full_name>\n", chp_esc);
	} else {
		fprintf(fp, "<full_name />\n");
	}
	free(chp_esc);
	chp_tmp = Db_GetValue(dbres, 0, 3);
	chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
	if (chp_esc[0]) {
		fprintf(fp, "<description>\n");
		fprintf(fp, "<![CDATA[");
		fprintf(fp, "%s", chp_esc);
		fprintf(fp, "]]>\n");
		fprintf(fp, "</description>\n");
	} else {
		fprintf(fp, "<description />\n");
	}
	free(chp_esc);
	Db_CloseDyna(dbres);
	fprintf(fp, "</user>\n");
	fprintf(fp, "</users>\n");

	strcpy(cha_sql, " select T1.n_entry_id");	/* 0 ����ID */
	strcat(cha_sql, ",T1.c_entry_title");		/* 1 ���������ȥ� */
	strcat(cha_sql, ",T1.c_entry_more");		/* 2 ����³�� */
	strcat(cha_sql, ",T1.c_entry_body");		/* 3 �������� */
	strcat(cha_sql, ",T1.d_entry_create_time");	/* 4 ������ƻ��� */
	strcat(cha_sql, ",T2.c_category_name");		/* 5 ���ƥ��꡼̾ */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " left join at_category T2");
	strcat(cha_sql, " on T1.n_category_id = T2.n_category_id");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id=%d", in_blog);
	strcat(cha_sql, " order by T1.d_entry_create_time desc, T1.n_entry_id desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		fclose(fp);
		remove(cha_temprdf);
		Put_Nlist(nlp_out, "ERROR", "������������륯���꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}

	fprintf(fp, "<entries>\n");
	for (i = 0; i < Db_GetRowCount(dbres); i++) {
		/* �������� */
		fprintf(fp, "<entry>\n");
		/* �����ȥ� */
		chp_tmp = Db_GetValue(dbres, i, 1);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp, "<title>%s</title>\n", chp_esc);
		} else {
			fprintf(fp, "<title />\n");
		}
		free(chp_esc);
		/* ɮ�� */
		if (cha_nickname[0]) {
			fprintf(fp, "<author>%s</author>\n", cha_nickname);
		} else {
			fprintf(fp, "<author />\n");
		}
		/* ���ƥ��� */
		chp_tmp = Db_GetValue(dbres, i, 5);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp, "<category>%s</category>\n", chp_esc);
		} else {
			fprintf(fp, "<category />\n");
		}
		/* ���� */
		chp_tmp = Db_GetValue(dbres, i, 4);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp, "<date>%s</date>\n", chp_esc);
		} else {
			fprintf(fp, "<date />\n");
		}
		free(chp_esc);
		/* ��ʸ */
		chp_tmp = Db_GetValue(dbres, i, 3);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp, "<description>\n");
			fprintf(fp, "<![CDATA[");
			fprintf(fp, "%s", chp_esc);
			fprintf(fp, "]]>\n");
			fprintf(fp, "</description>\n");
		} else {
			fprintf(fp, "<description />\n");
		}
		free(chp_esc);
		/* ³�� */
		chp_tmp = Db_GetValue(dbres, i, 2);
		chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (chp_esc[0]) {
			fprintf(fp, "<sequel>\n");
			fprintf(fp, "<![CDATA[");
			fprintf(fp, "%s", chp_esc);
			fprintf(fp, "]]>\n");
			fprintf(fp, "</sequel>\n");
		} else {
			fprintf(fp, "<sequel />\n");
		}
		free(chp_esc);
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (chp_tmp) {
			/* ������ */
			if (Write_Comment_To_Xml(db, nlp_in, nlp_out, in_blog, atoi(chp_tmp), fp)) {
				fclose(fp);
				remove(cha_temprdf);
				return 1;
			}
			/* �ȥ�å��Хå� */
			if (Write_Trackback_To_Xml(db, nlp_in, nlp_out, in_blog, atoi(chp_tmp), fp)) {
				fclose(fp);
				remove(cha_temprdf);
				return 1;
			}
		}
		/* ������λ */
		fprintf(fp, "</entry>\n");
	}
	Db_CloseDyna(dbres);
	fprintf(fp, "</entries>\n");
	fprintf(fp, "</blog>\n");
	rewind(fp);

	nkf = Create_Nkf_Stream(fp);
	Set_Input_Code(nkf, NKF_EUC);
	Set_Output_Code(nkf, NKF_UTF8);
	Convert_To_String(nkf, &chp_tmp);
	Destroy_Nkf_Engine(nkf);
	fclose(fp);
	remove(cha_temprdf);

	fprintf(fp_out, "Content-Disposition: attachment; filename=\"blog_export.xml\"\n");
	fprintf(fp_out, "Content-type: application/xml; charset=UTF-8\n\n");
	fprintf(fp_out, "%s", chp_tmp);
	fflush(fp_out);
	free(chp_tmp);

	return 0;
}


/*
+* =========================================================================
 * Function:	 	utf8_to_euc()
 * Description:
 *	����ʸ�������ɤ�EUC���Ѵ����롣
%* =========================================================================
 * Return:
 *		�Ѵ����ʸ��(freeɬ��)
-* ========================================================================*/
char* utf8_to_euc(char *chp_encode, char *chp_src)
{
	CP_NKF nkf;
	char *chp_tmp;
	int in_code;
	if (!chp_src) {
		chp_tmp = (char*)malloc(1);
		*chp_tmp = '\0';
		return chp_tmp;
	} else if (strcasecmp(chp_encode, "UTF-8") == 0) {
		in_code = NKF_UTF8;
	} else if (strcasecmp(chp_encode, "SHIFT_JIS") == 0) {
		in_code = NKF_SJIS;
	} else {
		chp_tmp = (char*)malloc(strlen(chp_src) + 1);
		strcpy(chp_tmp, chp_src);
		return chp_tmp;
	}
	nkf = Create_Nkf_String(chp_src);
	Set_Input_Code(nkf, in_code);
	Set_Output_Code(nkf, NKF_EUC);
	Convert_To_String(nkf, &chp_tmp);
	Destroy_Nkf_Engine(nkf);
	return chp_tmp;
}

/*
+* =========================================================================
 * Function:	 	name_to_category_id()
 * Description:
 *	���ƥ���̾�򥫥ƥ���ID���Ѵ����롣
%* =========================================================================
 * Return:
 *		0: ���ƥ���̵��
 *		>0 ���ƥ���ID
 *		<0 ���פ��ʤ��ä�
-* ========================================================================*/
int name_to_category_id(DBase *db, char *chp_name, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_sql;
	int in_cat;

	asprintf(&chp_sql, "select n_category_id from at_category where c_category_name = '%s' and n_blog_id = %d", chp_name, in_blog);
	dbres = Db_OpenDyna(db, chp_sql);
	free(chp_sql);
	if (!dbres)
		return -1;
	if (!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		return -1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_cat = atoi(chp_tmp);
	} else {
		in_cat = -1;
	}
	Db_CloseDyna(dbres);
	return in_cat;
}

/*
+* =========================================================================
 * Function:	 	add_new_category()
 * Description:
 *	���������ƥ�����ɲä������ƥ���ID���֤���
%* =========================================================================
 * Return:
 *		0: ����
 *		>0 ���ƥ���ID
-* ========================================================================*/
int add_new_category(DBase *db, char *chp_name, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_sql;
	char *chp_escape;
	int in_category;
	int in_order;
	int in_rt;

	asprintf(&chp_sql, "select coalesce(max(T1.n_category_id) + 1, 1), coalesce(max(T1.n_category_order) + 1, 1) from at_category T1 where T1.n_blog_id = %d for update", in_blog);
	dbres = Db_OpenDyna(db, chp_sql);
	free(chp_sql);
	if (!dbres) {
		Rollback_Transact(db);
		return 0;
	}
	if (!Db_GetRowCount(dbres)) {
		Rollback_Transact(db);
		Db_CloseDyna(dbres);
		return 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_category = atoi(chp_tmp);
	} else {
		in_category = 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_order = atoi(chp_tmp);
	} else {
		in_order = 1;
	}
	Db_CloseDyna(dbres);

	chp_escape = My_Escape_SqlString(db, chp_name);
	asprintf(&chp_sql, "insert into at_category (n_blog_id,n_category_id,n_category_order,c_category_name) values (%d, %d, %d, '%s')", in_blog, in_category, in_order, chp_escape);
	in_rt = Db_ExecSql(db, chp_sql);
	free(chp_escape);
	free(chp_sql);
	if (in_rt != CO_SQL_OK) {
		Rollback_Transact(db);
		return 0;
	}
	return in_category;
}

/*
+* =========================================================================
 * Function:	 	Read_Comments_From_File()
 * Description:
 *	�����Ȥ��ɤ߹���
%* =========================================================================
 * Return:
 *		>0: ����
 *		0 : ����
-* ========================================================================*/
int Read_Trackbacks(DBase *db, NLIST *nlp_out, int in_blog, char *chp_encoding, xmlNodePtr stp_entries)
{
	xmlNodePtr stp_entry;
	xmlNodePtr stp_comment;
	DBRes *dbres;
	char cha_sql[512];
	char *chp_sql;
	char *chp_tmp;

	if (!stp_entries)
		return 0;
	stp_entry = stp_entries;
	while (1) {
		if (!stp_entry || !stp_entry->name)
			break;
		if (strcasecmp((const char*)stp_entry->name, "trackback") == 0) {
			char *chp_title_euc = NULL;
			char *chp_excerpt_euc = NULL;
			char *chp_name_euc = NULL;
			char *chp_date_euc = NULL;
			char *chp_url_euc = NULL;
			char *chp_ip_euc = NULL;
			stp_comment = stp_entry->children;
			while (1) {
				if (strcasecmp((const char*)stp_comment->name, "title") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_title_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "excerpt") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_excerpt_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "blog_name") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_name_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "date") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_date_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "url") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_url_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "ip") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_ip_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				}
				if (stp_comment == stp_entry->last) {
					int in_comment;
					char *chp_title_esc = NULL;
					char *chp_excerpt_esc = NULL;
					char *chp_name_esc = NULL;
					sprintf(cha_sql, "select coalesce(max(T1.n_tb_id), 0) + 1 from at_trackback T1 where T1.n_blog_id = %d for update", in_blog);
					dbres = Db_OpenDyna(db, cha_sql);
					if(!dbres) {
						Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
						Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
						return 1;
					}
					chp_tmp = Db_GetValue(dbres, 0, 0);
					if (chp_tmp) {
						in_comment = atoi(chp_tmp);
					} else {
						in_comment = 1;
					}
					Db_CloseDyna(dbres);
					Descape_HtmlString(chp_title_euc);
					chp_title_esc = My_Escape_SqlString(db, chp_title_euc);
					Descape_HtmlString(chp_excerpt_euc);
					chp_excerpt_esc = My_Escape_SqlString(db, chp_excerpt_euc);
					Descape_HtmlString(chp_name_euc);
					chp_name_esc = My_Escape_SqlString(db, chp_name_euc);
					asprintf(&chp_sql,
						"insert into at_trackback "
						"(n_blog_id,"
						" n_entry_id,"
						" n_tb_id,"
						" c_tb_title,"
						" c_tb_excerpt,"
						" c_tb_blog_name,"
						" c_tb_url,"
						" c_tb_ip,"
						" d_tb_create_time)"
						" values "
						"(%d, -1, %d, '%s', '%s', '%s', '%s', '%s', '%s')",
						in_blog, in_comment, chp_title_esc, chp_excerpt_esc, chp_name_esc, chp_url_euc, chp_ip_euc, chp_date_euc);
					free(chp_name_esc);
					free(chp_title_esc);
					free(chp_excerpt_esc);
					if (Db_ExecSql(db, chp_sql)) {
						free(chp_sql);
						if (g_in_dbb_mode) {
							Put_Nlist(nlp_out, "ERROR", "�������ѤΥ����꡼�˼��Ԥ��ޤ�����<br>");
						} else {
							Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��Υ����꡼�˼��Ԥ��ޤ�����<br>");
						}
						Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, chp_sql);
						return 1;
					}
					free(chp_sql);
					break;
				}
				stp_comment = stp_comment->next;
			}
			if (chp_title_euc)
				free(chp_title_euc);
			if (chp_name_euc)
				free(chp_name_euc);
			if (chp_excerpt_euc)
				free(chp_excerpt_euc);
			if (chp_date_euc)
				free(chp_date_euc);
			if (chp_url_euc)
				free(chp_url_euc);
			if (chp_ip_euc)
				free(chp_ip_euc);
		}
		if (stp_entry == stp_entries->last) {
			break;
		}
		stp_entry = stp_entry->next;
	}
	return 0;
}

/*
+* =========================================================================
 * Function:	 	Read_Comments_From_File()
 * Description:
 *	�����Ȥ��ɤ߹���
%* =========================================================================
 * Return:
 *		>0: ����
 *		0 : ����
-* ========================================================================*/
int Read_Comments(DBase *db, NLIST *nlp_out, int in_blog, char *chp_encoding, xmlNodePtr stp_entries)
{
	xmlNodePtr stp_entry;
	xmlNodePtr stp_comment;
	DBRes *dbres;
	char cha_sql[512];
	char *chp_sql;
	char *chp_tmp;

	if (!stp_entries)
		return 0;
	stp_entry = stp_entries;
	while (1) {
		if (!stp_entry || !stp_entry->name)
			break;
		if (strcasecmp((const char*)stp_entry->name, "comment") == 0) {
			char *chp_description_euc = NULL;
			char *chp_name_euc = NULL;
			char *chp_mail_euc = NULL;
			char *chp_date_euc = NULL;
			char *chp_url_euc = NULL;
			char *chp_ip_euc = NULL;
			stp_comment = stp_entry->children;
			while (1) {
				if (strcasecmp((const char*)stp_comment->name, "description") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_description_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "name") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_name_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "email") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_mail_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "date") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_date_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "url") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_url_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				} else if (strcasecmp((const char*)stp_comment->name, "ip") == 0) {
					chp_tmp = (char*)xmlNodeGetContent(stp_comment);
					chp_ip_euc = utf8_to_euc(chp_encoding, chp_tmp ? chp_tmp : "");
				}
				if (stp_comment == stp_entry->last) {
					int in_comment;
					char *chp_description_esc = NULL;
					char *chp_name_esc = NULL;
					sprintf(cha_sql, "select coalesce(max(T1.n_comment_id), 0) + 1 from at_comment T1 where T1.n_blog_id = %d for update", in_blog);
					dbres = Db_OpenDyna(db, cha_sql);
					if(!dbres) {
						Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
						Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
						return 1;
					}
					chp_tmp = Db_GetValue(dbres, 0, 0);
					if (chp_tmp) {
						in_comment = atoi(chp_tmp);
					} else {
						in_comment = 1;
					}
					Db_CloseDyna(dbres);
					Descape_HtmlString(chp_description_euc);
					chp_description_esc = My_Escape_SqlString(db, chp_description_euc);
					Descape_HtmlString(chp_name_euc);
					chp_name_esc = My_Escape_SqlString(db, chp_name_euc);
					asprintf(&chp_sql,
						"insert into at_comment"
						"(n_blog_id,"
						" n_entry_id,"
						" n_comment_id,"
						" c_comment_author,"
						" c_comment_ip,"
						" c_comment_mail,"
						" c_comment_url,"
						" c_comment_body,"
						" d_comment_create_time)"
						" values "
						"(%d, -1, %d, '%s', '%s', '%s', '%s', '%s', '%s')",
						in_blog, in_comment, chp_name_esc, chp_ip_euc, chp_mail_euc, chp_url_euc, chp_description_esc, chp_date_euc);
					free(chp_name_esc);
					free(chp_description_esc);
					if (Db_ExecSql(db, chp_sql)) {
						free(chp_sql);
						Put_Nlist(nlp_out, "ERROR", "�����ȥ���ݡ��ȤΥ����꡼�˼��Ԥ��ޤ�����<br>");
						Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, chp_sql);
						return 1;
					}
					free(chp_sql);
					break;
				}
				stp_comment = stp_comment->next;
			}
			if (chp_description_euc)
				free(chp_description_euc);
			if (chp_name_euc)
				free(chp_name_euc);
			if (chp_mail_euc)
				free(chp_mail_euc);
			if (chp_date_euc)
				free(chp_date_euc);
			if (chp_url_euc)
				free(chp_url_euc);
			if (chp_ip_euc)
				free(chp_ip_euc);
		}
		if (stp_entry == stp_entries->last) {
			break;
		}
		stp_entry = stp_entry->next;
	}
	return 0;
}

/*
+* =========================================================================
 * Function:	 	Read_Xml_From_File()
 * Description:
 *	���åץ��ɤ��줿XML�ե�������ɤ߹��ߡ��������ɲä��롣
%* =========================================================================
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ========================================================================*/
int Read_Xml_From_File(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char* chp_filename)
{
	xmlDocPtr stp_doc;
	xmlNodePtr stp_root;
	xmlNodePtr stp_entries;
	xmlNodePtr stp_entry;
	xmlNodePtr stp_childs;
	DBRes *dbres;
	char cha_sql[512];
	char *chp_sql;
	char *chp_tmp;

	stp_doc = xmlParseEntity(chp_filename);
	if (!stp_doc) {
		Put_Nlist(nlp_out, "ERROR", "XML�ե�����Υѡ����˼��Ԥ��ޤ�����<br>");
		return 1;
	}
	stp_root = xmlDocGetRootElement(stp_doc);
	if (!stp_root) {
		Put_Nlist(nlp_out, "ERROR", "�롼�ȤΥΡ��ɤ�����Ǥ��ޤ���<br>");
		xmlFreeDoc(stp_doc);
		return 1;
	}
	if (strcasecmp((const char*)stp_root->name, "blog") != 0) {
		Put_Nlist(nlp_out, "ERROR", "XML�ե�����η�������äƤ��ޤ���<br>");
		xmlFreeDoc(stp_doc);
		return 1;
	}

	stp_entries = stp_root->children;
	while (1) {
		if (strcasecmp((const char*)stp_entries->name, "entries") == 0) {
			int in_new_entry_id = 0;
			stp_entry = stp_entries->children;
			while (1) {
				if (strcasecmp((const char*)stp_entry->name, "entry") == 0) {
					char *chp_title_euc = NULL;
					char *chp_author_euc = NULL;
					char *chp_category_euc = NULL;
					char *chp_date_euc = NULL;
					char *chp_description_euc = NULL;
					char *chp_sequel_euc = NULL;
					stp_childs = stp_entry->children;
					if (Begin_Transact(db)) {
						free(chp_category_euc);
						xmlFreeDoc(stp_doc);
						Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥�����󳫻Ϥ˼��Ԥ��ޤ�����<br>");
						Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
						return 1;
					}
					while (1) {
						if (strcasecmp((const char*)stp_childs->name, "title") == 0) {
							chp_tmp = (char*)xmlNodeGetContent(stp_childs);
							chp_title_euc = utf8_to_euc((char*)stp_doc->encoding, chp_tmp ? chp_tmp : "");
						} else if (strcasecmp((const char*)stp_childs->name, "author") == 0) {
							chp_tmp = (char*)xmlNodeGetContent(stp_childs);
							chp_author_euc = utf8_to_euc((char*)stp_doc->encoding, chp_tmp ? chp_tmp : "");
						} else if (strcasecmp((const char*)stp_childs->name, "category") == 0) {
							chp_tmp = (char*)xmlNodeGetContent(stp_childs);
							chp_category_euc = utf8_to_euc((char*)stp_doc->encoding, chp_tmp ? chp_tmp : "");
						} else if (strcasecmp((const char*)stp_childs->name, "date") == 0) {
							chp_tmp = (char*)xmlNodeGetContent(stp_childs);
							chp_date_euc = utf8_to_euc((char*)stp_doc->encoding, chp_tmp ? chp_tmp : "");
						} else if (strcasecmp((const char*)stp_childs->name, "description") == 0) {
							chp_tmp = (char*)xmlNodeGetContent(stp_childs);
							chp_description_euc = utf8_to_euc((char*)stp_doc->encoding, chp_tmp ? chp_tmp : "");
						} else if (strcasecmp((const char*)stp_childs->name, "sequel") == 0) {
							chp_tmp = (char*)xmlNodeGetContent(stp_childs);
							chp_sequel_euc = utf8_to_euc((char*)stp_doc->encoding, chp_tmp ? chp_tmp : "");
						} else if (strcasecmp((const char*)stp_childs->name, "comments") == 0) {
							Read_Comments(db, nlp_out, in_blog, (char*)stp_doc->encoding, stp_childs->children);
						} else if (strcasecmp((const char*)stp_childs->name, "trackbacks") == 0) {
							Read_Trackbacks(db, nlp_out, in_blog, (char*)stp_doc->encoding, stp_childs->children);
						}
						if (stp_childs == stp_entry->last) {
							char *chp_title_esc;
							char *chp_author_esc;
							char *chp_description_esc;
							char *chp_sequel_esc;
							int in_category = name_to_category_id(db, chp_category_euc, in_blog);
							if (in_category < 0) {
								if (chp_category_euc[0]) {
									in_category = add_new_category(db, chp_category_euc, in_blog);
									if (!in_category) {
										free(chp_category_euc);
										Put_Nlist(nlp_out, "ERROR", "���ƥ��꡼�����Ǥ��ޤ���Ǥ�����<br>");
										xmlFreeDoc(stp_doc);
										return 1;
									}
								} else {
									in_category = 0;
								}
							}
							free(chp_category_euc);
							sprintf(cha_sql, "select n_next_entry from at_profile where n_blog_id = %d for update", in_blog);
							dbres = Db_OpenDyna(db, cha_sql);
							if (!dbres) {
								xmlFreeDoc(stp_doc);
								Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
								Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
								return 1;
							}
							chp_tmp = Db_GetValue(dbres, 0, 0);
							if (chp_tmp) {
								in_new_entry_id = atoi(chp_tmp);
							} else {
								in_new_entry_id = 1;
							}
							Db_CloseDyna(dbres);

							Descape_HtmlString(chp_description_euc);
							Descape_HtmlString(chp_sequel_euc);
							chp_title_esc =  My_Escape_SqlString(db, chp_title_euc);
							chp_author_esc = My_Escape_SqlString(db, chp_author_euc);
							chp_description_esc = My_Escape_SqlString(db, chp_description_euc);
							chp_sequel_esc = My_Escape_SqlString(db, chp_sequel_euc);
							asprintf(&chp_sql,
								"insert into at_entry"
								"(n_blog_id"
								",n_entry_id"
								",n_category_id"
								",b_comment"
								",b_trackback"
								",b_mode"
								",c_entry_title"
								",c_entry_summary"
								",c_entry_body"
								",c_entry_more"
								",d_entry_create_time"
								",d_entry_modify_time)"
								" values"
								"(%d,%d,%d,1,1,1,'%s','','%s','%s','%s',now())"
								 , in_blog, in_new_entry_id, in_category, chp_title_esc, chp_description_esc, chp_sequel_esc, chp_date_euc);
							free(chp_title_esc);
							free(chp_author_esc);
							free(chp_description_esc);
							free(chp_sequel_esc);
							if (Db_ExecSql(db, chp_sql)) {
								free(chp_sql);
								xmlFreeDoc(stp_doc);
								Rollback_Transact(db);
								Put_Nlist(nlp_out, "ERROR", "��ƥ���ݡ��ȤΥ����꡼�˼��Ԥ��ޤ�����<br>");
								Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, chp_sql);
								return 1;
							}
							free(chp_sql);
							sprintf(cha_sql, "update at_profile set n_next_entry=n_next_entry+1 where n_blog_id = %d", in_blog);
							if (Db_ExecSql(db, cha_sql)) {
								xmlFreeDoc(stp_doc);
								Put_Nlist(nlp_out, "ERROR", "��ƥ���ݡ��ȤΥ����꡼�˼��Ԥ��ޤ�����<br>");
								Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
								Rollback_Transact(db);
								return 1;
							}
							sprintf(cha_sql, "update at_comment set n_entry_id = %d where n_blog_id = %d and n_entry_id = -1", in_new_entry_id, in_blog);
							if (Db_ExecSql(db, cha_sql)) {
								xmlFreeDoc(stp_doc);
								Put_Nlist(nlp_out, "ERROR", "��ƥ���ݡ��ȤΥ����꡼�˼��Ԥ��ޤ�����<br>");
								Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
								Rollback_Transact(db);
								return 1;
							}
							sprintf(cha_sql, "update at_trackback set n_entry_id = %d where n_blog_id = %d and n_entry_id = -1", in_new_entry_id, in_blog);
							if (Db_ExecSql(db, cha_sql)) {
								xmlFreeDoc(stp_doc);
								Put_Nlist(nlp_out, "ERROR", "��ƥ���ݡ��ȤΥ����꡼�˼��Ԥ��ޤ�����<br>");
								Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
								Rollback_Transact(db);
								return 1;
							}
							break;
						}
						stp_childs = stp_childs->next;
					}
					if (chp_title_euc)
						free(chp_title_euc);
					if (chp_author_euc)
						free(chp_author_euc);
					if (chp_date_euc)
						free(chp_date_euc);
					if (chp_description_euc)
						free(chp_description_euc);
					if (chp_sequel_euc)
						free(chp_sequel_euc);
					if (Commit_Transact(db)) {
						Put_Nlist(nlp_out, "ERROR", "���ߥåȤ˼��Ԥ��ޤ�����<br>");
						Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
						Rollback_Transact(db);
						return 1;
					}
				}
				if (stp_entry == stp_entries->last) {
					break;
				}
				stp_entry = stp_entry->next;
			}
		}
		if (stp_entries == stp_root->last) {
			break;
		}
		stp_entries = stp_entries->next;
	}
	xmlFreeDoc(stp_doc);

	return 0;
}

/*
+* ========================================================================
 * Function:            Reformat_Date()
 * Description:
 *	yyyymmdd ---> yyyy/mm/dd
%* ========================================================================
 * Return:
 *      �ʤ�
-* =======================================================================*/
void Reformat_Date(char *chp_dest, char *chp_src)
{
	chp_dest[0] = '\0';
	sprintf(chp_dest, "%.4s/%.2s/%.2s", chp_src + 0, chp_src + 4, chp_src + 6);
	return;
}

/*
+* ========================================================================
 * Function:            Make_Tmptable_For_Count
 * Include-file:        libblog.h
 * Description:
 *	�����Ȥ��ε�����°���륳���ȿ�������tmp_comment��
 *	�ȥ�å��Хå���������tmp_trackback��2�ĤΥƥ�ݥ�꡼�ơ��֥��������롣
%* ========================================================================
 * Return:
 *	0 ����
 *	CO_ERROR������
-* ======================================================================
 */
int Make_Tmptable_For_Count(DBase *db, NLIST *nlp_out, int in_blog, int in_all)
{
	char cha_sql[1024];

	strcpy(cha_sql, "create temporary table tmp_comment as");
	strcat(cha_sql, " select T1.n_entry_id as n_entry_id");
	strcat(cha_sql, ", coalesce(count(T1.n_comment_id), 0) as n_comment_count");    /* �����ȿ� */
	strcat(cha_sql, " from at_comment T1");
	if (in_all) {
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	} else {
		strcat(cha_sql, " where T1.b_comment_accept != 0");
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	}
	strcat(cha_sql, " group by T1.n_entry_id");
	if (Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "�����ȿ������륯����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	strcpy(cha_sql, "create temporary table tmp_trackback as");
	strcat(cha_sql, " select T1.n_entry_id as n_entry_id");
	strcat(cha_sql, ", coalesce(count(T1.n_tb_id), 0) as n_tb_count");      /* �����ȿ� */
	strcat(cha_sql, " from at_trackback T1");
	if (in_all) {
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	} else {
		strcat(cha_sql, " where T1.b_tb_accept != 0");
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	}
	strcat(cha_sql, " group by T1.n_entry_id");
	if (Db_ExecSql(db, cha_sql)) {
		if(g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ����������륯����˼��Ԥ��ޤ�����");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå��������륯����˼��Ԥ��ޤ�����");
		}
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return CO_ERROR;
	}
	return 0;
}

/*
+* ========================================================================
 * Function:	Duplicate_Login
 * Description:
 *
%* ========================================================================
 * Return:	<0	���顼
 * 			=0	����
 *			>0	���֤ꤢ��
-* ======================================================================
 */
int Duplicate_Login(DBase *db, NLIST *nlp_out, int in_blog, char *chp_login)
{
	DBRes *dbres;
	char *chp_num;
	char cha_sql[255];
	int in_num;

	sprintf(cha_sql, "select count(*) from at_profile where n_blog_id != %d and c_login = '%s'", in_blog, chp_login);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�������������ξȹ�˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		return 0;
	}
	chp_num = Db_FetchValue(dbres, 0);
	if (!chp_num) {
		Db_CloseDyna(dbres);
		return 0;
	}
	in_num = atoi(chp_num);
	Db_CloseDyna(dbres);

	return (in_num > 0);
}

/*
+* ========================================================================
 * Function:	Put_Format_Nlist
 * Description:
 * �ե����ޥåȤ˽��ä�Ÿ��������Τ�Put_Nlist����
%* ========================================================================
 * Return:
 * ���ｪλ:	0
 * ���顼��λ:	1
-* ======================================================================*/
void Put_Format_Nlist(NLIST *nlp_out, char *chp_insert, char *chp_format, ...)
{
	char *chp_str;
	va_list ap;

	va_start(ap, chp_format);
	vasprintf(&chp_str, chp_format, ap);
	Put_Nlist(nlp_out, chp_insert, chp_str);
	free(chp_str);
	va_end (ap);
}

/*
+* =============================================================================
 * Function:		Change_Order_Db_Blog
 * Include-file:	libcgi2.h
 * Description:
 * Sequence��primarykey���������������Ƥ���table��
 * chp_mod���б�����Sequence���¤��ؤ���Ԥ�
 * ��MySQL4.0�ʤɤ�subquery��ͭ���Ǥʤ�database�ΰ٤�
 * select���ټ¹Ԥ��Ƥ���Τ��ѻ��ˤʤäƤ��롣
%* =============================================================================
 * Return:
 * -1: ����Хå����顼
 * 0: ���ｪλ
 * 1�ʾ�: �����꡼���顼
-* =============================================================================
*/
int Change_Order_Db_Blog(
	DBase *db
	,char *chp_id			/* �¤����Ѥ����оݤ�ID	*/
	,char *chp_name_id		/* Primary Key �Υ����͡��� */
	,char *chp_name_seq		/* Order Sequence�Υ����͡��� */
	,char *chp_name_table	/* �ǡ��������äƤ���table �͡��� */
	,char *chp_where1		/* where��ǻ��ꤹ�� Primary Key�ξ�� */
	,char *chp_where2		/* where��ǻ��ꤹ�� Order�ξ�� */
	,char *chp_mod)			/* t(top), u(up), d(down), b(bottom) �Τɤ줫 */
{
	DBRes *dbres;
	char *chp_sql;
	char *chp_buf;
	int in_min_seq; 	/* chp_table �κǾ���seq */
	int in_max_seq;		/* chp_table �κ����seq */
	int in_seq;			/* chp_id �� seq */
	int in_side_seq;	/* orderby seq �����٤��seq */
	int in_side_id;		/* orderby seq �����٤��id */
	int in_ret;

	if (chp_id == NULL || *chp_id == '\0') { /*���⤷�ʤ� */
		return 0;
	}
	in_ret = Begin_Transact(db);
	if (in_ret == CO_SQL_ERR) {
		return 1;
	}
	if (*chp_where1 == '\0') {
		asprintf(&chp_sql, "select %s from %s where %s = %s", chp_name_seq, chp_name_table, chp_name_id, chp_id);
	} else {
		asprintf(&chp_sql, "select %s from %s where %s = %s and %s", chp_name_seq, chp_name_table, chp_name_id, chp_id, chp_where1);
	}
	dbres = Db_OpenDyna(db, chp_sql);
	free(chp_sql);
	if (dbres == NULL) {
		return 1;
	} else if (!(Db_GetRowCount(dbres))) { 	/*���⤷�ʤ� */
		Db_CloseDyna(dbres);
		return 0;
	}
	chp_buf = Db_GetValue(dbres, 0, 0);
	if (chp_buf == NULL) {
		Db_CloseDyna(dbres);
		return 1;
	}
	in_seq = atoi(chp_buf);
	Db_CloseDyna(dbres);
	if (*chp_mod == 't' || *chp_mod == 'T') {
		/* seq �κǾ��ͤ�������� */
		if (*chp_where2 == '\0') {
			asprintf(&chp_sql, "select MIN(%s) from %s", chp_name_seq, chp_name_table);
		} else {
			asprintf(&chp_sql, "select MIN(%s) from %s where %s", chp_name_seq, chp_name_table, chp_where2);
		}
		dbres = Db_OpenDyna(db, chp_sql);
		free(chp_sql);
		if (dbres == NULL) {
			return 2;
		} else if (!(Db_GetRowCount(dbres))) { /*���⤷�ʤ� */
			Db_CloseDyna(dbres);
			return 0;
		}
		chp_buf = Db_GetValue(dbres, 0, 0);
		if (chp_buf == NULL) {
			Db_CloseDyna(dbres);
			return 0;
		}
		in_min_seq = atoi(chp_buf);
		if (chp_buf == NULL) {
			Db_CloseDyna(dbres);
			return 3;
		}
		Db_CloseDyna(dbres);
		if (in_min_seq == in_seq) {
			return 0;
		}
		/*���־�ΤȤ��ϲ��⤷�ʤ�*/
		if (in_min_seq == 1) {
			/* �Ȥꤢ�������Ƥˣ���­���Ƥ��ޤ� */
			if (*chp_where2 == '\0') {
				asprintf(&chp_sql, "update %s set %s = %s + 1", chp_name_table, chp_name_seq, chp_name_seq);
			} else {
				asprintf(&chp_sql, "update %s set %s = %s + 1 where %s", chp_name_table, chp_name_seq, chp_name_seq, chp_where2);
			}
			in_ret = Db_ExecSql(db, chp_sql);
			free(chp_sql);
			if (in_ret == CO_SQL_ERR) {
				return 2;
			}
			/* ��Ƭ�˻��äƤ��� */
			if (*chp_where1 == '\0') {
				asprintf(&chp_sql, "update %s set %s = 1 where %s = %s", chp_name_table, chp_name_seq, chp_name_id, chp_id);
			} else {
				asprintf(&chp_sql, "update %s set %s = 1 where %s = %s and %s", chp_name_table, chp_name_seq, chp_name_id, chp_id, chp_where1);
			}
			in_ret = Db_ExecSql(db, chp_sql);
			free(chp_sql);
			if (in_ret == CO_SQL_ERR) {
				return 3;
			}
		} else if (in_min_seq > 1){
			/* seq �κǾ��� - 1 �˥��åȤ��� */
			if (*chp_where1 == '\0') {
				asprintf(&chp_sql, "update %s set %s = %d where %s = %s", chp_name_table, chp_name_seq, in_min_seq - 1, chp_name_id, chp_id);
			} else {
				asprintf(&chp_sql, "update %s set %s = %d where %s = %s and %s", chp_name_table, chp_name_seq, in_min_seq - 1, chp_name_id, chp_id, chp_where1);
			}
			in_ret = Db_ExecSql(db, chp_sql);
			free(chp_sql);
			if (in_ret == CO_SQL_ERR) {
				return 4;
			}
//			return 0;
		}
		/* ��֤ο�������־�˻��äƤ���ȷ��֤������Ƥ��ޤ��Τǡ����֤����� */
		if (*chp_where2 == '\0') {
			asprintf(&chp_sql, "update %s set %s = %s - 1 where %s > %d", chp_name_table, chp_name_seq, chp_name_seq, chp_name_seq, in_seq);
		} else {
			asprintf(&chp_sql, "update %s set %s = %s - 1 where %s > %d and %s", chp_name_table, chp_name_seq, chp_name_seq, chp_name_seq, in_seq , chp_where2);
		}
		in_ret = Db_ExecSql(db, chp_sql);
		free(chp_sql);
		if (in_ret == CO_SQL_ERR) {
			return 13;
		}
	} else if (*chp_mod == 'u' || *chp_mod == 'U' || *chp_mod == 'd' || *chp_mod == 'D') {
		if (*chp_mod == 'u' || *chp_mod == 'U') {
			/* chp_id �� seq ��꾮���������seq ��������� */
			if (*chp_where2 == '\0') {
				asprintf(&chp_sql, "select MAX(%s) from %s where %s < %d", chp_name_seq, chp_name_table, chp_name_seq, in_seq);
			} else {
				asprintf(&chp_sql, "select MAX(%s) from %s where %s < %d and %s", chp_name_seq, chp_name_table, chp_name_seq, in_seq, chp_where2);
			}
			dbres = Db_OpenDyna(db, chp_sql);
			free(chp_sql);
			if (dbres == NULL) {
				return 5;
			} else if (Db_GetValue(dbres, 0, 0) == NULL||*Db_GetValue(dbres, 0, 0) =='\0') { /*��Ȥ�Ȱ��־����� */
				Db_CloseDyna(dbres);
				return 0;
			}
			in_side_seq = atoi(Db_GetValue(dbres, 0, 0));
			Db_CloseDyna(dbres);
			/* in_side_seq �� in_side_id ��������� */
 			if (*chp_where2 == '\0') {
				asprintf(&chp_sql, "select %s from %s where %s = %d", chp_name_id, chp_name_table, chp_name_seq, in_side_seq);
			} else {
				asprintf(&chp_sql, "select %s from %s where %s = %d and %s", chp_name_id, chp_name_table, chp_name_seq, in_side_seq, chp_where2);
			}
			dbres = Db_OpenDyna(db, chp_sql);
			free(chp_sql);
			if (dbres == NULL) {
				return 6;
			}
			if (Db_GetValue(dbres, 0, 0) == NULL) {
				Db_CloseDyna(dbres);
				return 7;
			}
			in_side_id = atoi(Db_GetValue(dbres, 0, 0));
			Db_CloseDyna(dbres);
		} else { /* down*/
			/* chp_id �� seq ����礭���Ǿ���seq ��������� */
			if (*chp_where2 == '\0') {
				asprintf(&chp_sql, "select MIN(%s) from %s where %s > %d", chp_name_seq, chp_name_table, chp_name_seq, in_seq);
			} else {
				asprintf(&chp_sql, "select MIN(%s) from %s where %s > %d and %s", chp_name_seq, chp_name_table, chp_name_seq, in_seq, chp_where2);
			}
			dbres = Db_OpenDyna(db, chp_sql);
			free(chp_sql);
			if (dbres == NULL) {
				return 7;
			} else if (Db_GetValue(dbres, 0, 0) == NULL||*Db_GetValue(dbres, 0, 0) =='\0') { /*��Ȥ�Ȱ����礭�� */
				Db_CloseDyna(dbres);
				return 0;
			}
			in_side_seq = atoi(Db_GetValue(dbres, 0, 0));
			Db_CloseDyna(dbres);
			/* in_side_seq �� in_side_id ��������� */
			if (*chp_where2 == '\0') {
				asprintf(&chp_sql, "select %s from %s where %s = %d", chp_name_id, chp_name_table, chp_name_seq, in_side_seq);
			} else {
				asprintf(&chp_sql, "select %s from %s where %s = %d and %s", chp_name_id, chp_name_table, chp_name_seq, in_side_seq, chp_where2);
			}
			dbres = Db_OpenDyna(db, chp_sql);
			free(chp_sql);
			if (dbres == NULL) {
				return 8;
			}
			if (Db_GetValue(dbres, 0, 0) == NULL) {
				Db_CloseDyna(dbres);
				return 9;
			}
			in_side_id = atoi(Db_GetValue(dbres, 0, 0));
			Db_CloseDyna(dbres);
		}
		/* �ʲ��٤�礦seq �θ򴹽��� */
		/* in_side_seq = in_seq ������*/
		if (*chp_where2 == '\0') {
			asprintf(&chp_sql, "update %s set %s = %d where %s = %d", chp_name_table, chp_name_seq, in_seq, chp_name_seq, in_side_seq);
		} else {
			asprintf(&chp_sql, "update %s set %s = %d where %s = %d and %s", chp_name_table, chp_name_seq, in_seq, chp_name_seq, in_side_seq, chp_where2);
		}
		in_ret = Db_ExecSql(db, chp_sql);
		free(chp_sql);
		if (in_ret == CO_SQL_ERR) {
			if (CO_SQL_ERR == Rollback_Transact(db)) {
				return -1;
			} else {
				return 10;
			}
		}
		/* in_seq = in_side_seq ������*/
		if (*chp_where1 == '\0') {
			asprintf(&chp_sql, "update %s set %s = %d where %s = %s", chp_name_table, chp_name_seq, in_side_seq, chp_name_id, chp_id);
		} else {
			asprintf(&chp_sql, "update %s set %s = %d where %s = %s and %s", chp_name_table, chp_name_seq, in_side_seq, chp_name_id, chp_id, chp_where1);
		}
		in_ret = Db_ExecSql(db, chp_sql);
		free(chp_sql);
		if (in_ret == CO_SQL_ERR) {
			if (CO_SQL_ERR == Rollback_Transact(db)) {
				return -1;
			} else {
				return 9;
			}
		}
	} else if (*chp_mod == 'b' || *chp_mod == 'B') {
		/* seq �κ����ͤ�������� */
		if (*chp_where2 == '\0') {
			asprintf(&chp_sql, "select MAX(%s) from %s where %s > %d", chp_name_seq, chp_name_table, chp_name_seq, in_seq);
		} else {
			asprintf(&chp_sql, "select MAX(%s) from %s where %s > %d and %s", chp_name_seq, chp_name_table, chp_name_seq, in_seq, chp_where2);
		}
		dbres = Db_OpenDyna(db, chp_sql);
		free(chp_sql);
		if (dbres == NULL) {
			return 11;
		} else if (Db_GetValue(dbres, 0, 0) == NULL || *Db_GetValue(dbres, 0, 0) == '\0' ||!Db_GetRowCount(dbres)) { /* ���������礭�� */
			Db_CloseDyna(dbres);
			return 0;
		}
		in_max_seq = atoi(Db_GetValue(dbres, 0, 0));
		Db_CloseDyna(dbres);
		/* in_seq �� in_max_seq + 1 �򥻥åȤ��� */
		if (*chp_where1 == '\0') {
			asprintf(&chp_sql, "update %s set %s = %d where %s = %s", chp_name_table, chp_name_seq, in_max_seq + 1, chp_name_id, chp_id);
		} else {
			asprintf(&chp_sql, "update %s set %s = %d where %s = %s and %s", chp_name_table, chp_name_seq, in_max_seq + 1, chp_name_id, chp_id, chp_where1);
		}
		in_ret = Db_ExecSql(db, chp_sql);
		free(chp_sql);
		if (in_ret == CO_SQL_ERR) {
			return 12;
		}
		/* �����ޤǤǡ��¤��ؤ��Ͻ�λ */

		/* ��֤ο�������ֲ��˻��äƤ���ȷ��֤������Ƥ��ޤ��Τǡ����֤����� */
		if (*chp_where2 == '\0') {
			asprintf(&chp_sql, "update %s set %s = %s - 1 where %s > %d", chp_name_table, chp_name_seq, chp_name_seq, chp_name_seq, in_seq);
		} else {
			asprintf(&chp_sql, "update %s set %s = %s - 1 where %s > %d and %s", chp_name_table, chp_name_seq, chp_name_seq, chp_name_seq, in_seq, chp_where2);
		}
		in_ret = Db_ExecSql(db, chp_sql);
		free(chp_sql);
		if (in_ret == CO_SQL_ERR) {
			return 13;
		}
	}
	/* �¤��ؤ���λ��ˡ�SEQ�γ����ֹ椬1����Ϥޤ��ͤˤ��餹 */
	/* ���ߤκǾ� SEQ �μ��� */
	if (*chp_where2 == '\0') {
		asprintf(&chp_sql, "select MIN(%s) from %s", chp_name_seq, chp_name_table);
	} else {
		asprintf(&chp_sql, "select MIN(%s) from %s where %s", chp_name_seq, chp_name_table, chp_where2);
	}
	dbres = Db_OpenDyna(db, chp_sql);
	free(chp_sql);
	if (in_ret == CO_SQL_ERR) {
		return 14;
	}
	chp_buf = Db_GetValue(dbres, 0 , 0);
	if (chp_buf == NULL) {
		return 15;
	}
	if (atoi(chp_buf) != 1) {		/* 1 ����Ϥޤ��ͤ����Τ򤺤餹 */
		if (*chp_where2 == '\0') {
			asprintf(&chp_sql, "update %s set %s = %s - (%s) + 1", chp_name_table, chp_name_seq, chp_name_seq, chp_buf);
		} else {
			asprintf(&chp_sql, "update %s set %s = %s -(%s) + 1 where %s", chp_name_table, chp_name_seq, chp_name_seq, chp_buf, chp_where2);
		}
		in_ret = Db_ExecSql(db, chp_sql);
		free(chp_sql);
		if (in_ret == CO_SQL_ERR) {
			return 16;
		}
	}
	Commit_Transact(db);
	return 0;
}

/*
+* =============================================================================
 * Function:	Conv_Blog_Wbr
 * Description: \r\n�����\n��<br>���Ѵ���������ֳ֤�<wbr>�������
 *
%* =============================================================================
 * Return:
 *	<br>��<wbr>���Ѵ����줿ʸ�����char�ݥ���
-* =============================================================================
*/
char* Conv_Blog_Br(char *chp_string)
{
	unsigned char *chp_buf;
	char *chp_conv;
	int in_total;
	int in_count;
	int in_line;
	int in_tag;
	int i, j;

	if (!chp_string) {
		return NULL;
	}
	in_total = strlen(chp_string);
	for (i = 0, in_count = 0; i < in_total; i++) {
		if (i + 1 < in_total &&
			*(chp_string + i) == '\r' &&
			*(chp_string + i + 1) =='\n') {
			++in_count;
		} else if (*(chp_string + i) == '\n') {
			in_count++;
		}
	}
	chp_conv = (char *)malloc(in_total + in_count * strlen("<br>&nbsp;") + 1);
	if (!chp_conv) {
		return NULL;
	}

	j = 0;
	i = 0;
	in_tag = 0;
	in_line = 0;
	while (*(chp_string + i) != '\0') {
		chp_buf = chp_string + i;
		if (*chp_buf == '<') {
			*(chp_conv + j) = *(chp_string + i);
			j++;
			i++;
			++in_tag;
		} else if (in_tag && *chp_buf == '>') {
			*(chp_conv + j) = *(chp_string + i);
			j++;
			i++;
			--in_tag;
			if (!in_tag) {
				in_line = 0;
			}
		} else if (*chp_buf == '\r' && *(chp_buf + 1) == '\n' && *(chp_buf + 2) == '\r' && *(chp_buf + 3) == '\n') {
			if (j > 0 && *(chp_conv + j - 1) == '>') {
				strcpy(chp_conv + j, "\r\n\r\n");
				j += strlen("\r\n\r\n");
			} else {
				strcpy(chp_conv + j, "<p>\r\n");
				j += strlen("<p>\r\n");
			}
			i += strlen("\r\n\r\n");
			in_line = 0;
		} else if (*chp_buf == '\n' && *(chp_buf + 1) == '\n') {
			if (j > 0 && *(chp_conv + j - 1) == '>') {
				strcpy(chp_conv + j, "\n\n");
				j += strlen("\n\n");
			} else {
				strcpy(chp_conv + j, "<p>\n");
				j += strlen("<p>\n");
			}
			i += strlen("\n\n");
			in_line = 0;
		} else if (*chp_buf == '\r' && *(chp_buf + 1) == '\n') {
			if (j > 0 && *(chp_conv + j - 1) == '>') {
				strcpy(chp_conv + j, "\r\n");
				j += strlen("\r\n");
			} else {
				strcpy(chp_conv + j, "<br>\r\n");
				j += strlen("<br>\r\n");
			}
			i += strlen("\r\n");
			in_line = 0;
		} else if (*chp_buf == '\n') {
			if (j > 0 && *(chp_conv + j - 1) == '>') {
				strcpy(chp_conv + j, "\n");
				j += strlen("\n");
			} else {
				strcpy(chp_conv + j, "<br>\n");
				j += strlen("<br>\n");
			}
			++i;
			in_line = 0;
		} else if (*chp_buf >= 0xA1 && *chp_buf <= 0xFE) {
			if (i + 2 > in_total) {
				break;
			}
			strncpy(chp_conv + j, chp_string + i, 2);
			i += 2;
			j += 2;
			in_line += 2;
		} else if (*chp_buf == 0x8E) {
			if (i + 2 > in_total) {
				break;
			}
			strncpy(chp_conv + j, chp_string + i, 2);
			i += 2;
			j += 2;
			in_line += 2;
		} else if (*chp_buf == 0x8F) {
			if (i + 3 > in_total) {
				break;
			}
			strncpy(chp_conv + j, chp_string + i, 3);
			i += 3;
			j += 3;
			in_line += 3;
		} else if (*chp_buf == 0x26) {		/*&*/
			if (!strncmp(chp_string + i, "&lt;", strlen("&lt;"))
			|| !strncmp(chp_string + i, "&gt;", strlen("&gt;"))) {
				strncpy(chp_conv + j, chp_string + i, 4);
				i += 4;
				j += 4;
				in_line += 4;
			} else if (!strncmp(chp_string + i, "&amp;", strlen("&amp;"))) {
				strncpy(chp_conv + j, chp_string + i, 5);
				i += 5;
				j += 5;
				in_line += 5;
			} else if (!strncmp(chp_string + i, "&quot;", strlen("&quot;"))) {
				strncpy(chp_conv + j, chp_string + i, 6);
				i += 6;
				j += 6;
				in_line += 6;
			} else {
				*(chp_conv + j) = *(chp_string + i);
				j++;
				i++;
				++in_line;
			}
		} else {
			*(chp_conv + j) = *(chp_string + i);
			j++;
			i++;
			if (!in_tag)
				++in_line;
		}
	}
	*(chp_conv + j) = '\0';
	return chp_conv;
}

/*
+* =============================================================================
 * Function:	Conv_Long_Ascii
 * Description: Ⱦ�Ѥ�Ĺ��³���Ƥ���ʸ�����\n�򤤤��
 *
%* =============================================================================
 * Return:
 *	\n���Ѵ����줿ʸ�����char�ݥ���
-* =============================================================================
*/
char* Conv_Long_Ascii(char *chp_string, int in_len)
{
	int in_cnt;
	int in_br;
	int in_tag;
	char *chp_cur;
	char *chp_dst;
	char *chp_conv;

	if (!chp_string) {
		return NULL;
	}

	in_br = 0;
	in_cnt = 0;
	chp_cur = chp_string;
	while (*chp_cur) {
		if (iseuc((unsigned char*)chp_cur)) {
			++chp_cur;
			in_cnt = 0;
		} else {
			++in_cnt;
			if (in_cnt >= in_len) {
				++in_br;
				in_cnt = 0;
			}
		}
		++chp_cur;
	}
	chp_conv = (char *)malloc(strlen(chp_string) + in_br * strlen("\n") + 1);

	in_cnt = 0;
	in_tag = 0;
	chp_cur = chp_string;
	chp_dst = chp_conv;
	while (*chp_cur) {
		if (iseuc((unsigned char*)chp_cur)) {
			*chp_dst++ = *chp_cur++;
			in_cnt = 0;
		} else if (in_tag && *chp_cur == '>') {
			in_tag = 0;
		} else if (!in_tag) {
			if (*chp_cur == '<') {
				in_tag = 1;
			} else {
				++in_cnt;
				if (in_cnt >= in_len) {
					*chp_dst++ = '\n';
					++in_br;
					in_cnt = 0;
				}
			}
		}
		*chp_dst++ = *chp_cur++;
	}
	*chp_dst = '\0';

	return chp_conv;
}

/*
+* ========================================================================
 * Function:		ʸ�����ʣ��
 * Include-file:
 * Description:
 *	chp_str�ǻ��ꤵ�줿ʸ�����Ʊ��Ĺ���Υ������ݤ���������ʣ�̤��롣
%* ========================================================================
 * Return:
 *	ʣ�̤���ʸ���󡣻Ȥ�����ä���free�ǲ�����
-* ======================================================================*/
char* Copy_Str(const char* chp_str)
{
	char* chp_new;
	if (chp_str) {
		chp_new = malloc(strlen(chp_str) + 1);
		strcpy(chp_new, chp_str);
	} else {
		chp_new = malloc(1);
		*chp_new = '\0';
	}
	return chp_new;
}

/*
+* ========================================================================
 * Function:		ʸ�����Ϣ��
 * Include-file:
 * Description:
 *	chp_str1��chp_str2��Ĥʤ���ʸ����򿷤��˳��ݤ���������˺������롣
%* ========================================================================
 * Return:
 *	�Ĥʤ��ä�ʸ���󡣻Ȥ�����ä���free�ǲ�����
-* ======================================================================*/
char* Concat_Str(const char* chp_str1, const char* chp_str2)
{
	char* chp_new;

	if (!chp_str1 || !chp_str1[0]) {
		if (!chp_str2 || !chp_str2[0]) {
			chp_new = malloc(1);
			*chp_new = '\0';
			return chp_new;
		} else {
			return Copy_Str(chp_str2);
		}
	} else if (!chp_str2 || !chp_str2[0]) {
		return Copy_Str(chp_str1);
	}

	chp_new = malloc(strlen(chp_str1) + strlen(chp_str2) + 1);
	strcpy(chp_new, chp_str1);
	strcat(chp_new, chp_str2);
	return chp_new;
}

/*
+* ========================================================================
 * Function:		ʸ�����Ϣ��
 * Include-file:
 * Description:
 *	chpp_org��chp_add��Ĥʤ���ʸ�����chpp_add���֤���
%* ========================================================================
 * Return:
 *	�ʤ�
-* ======================================================================*/
void Add_Str(char** chpp_org, const char* chp_add)
{
	char* chp_new;
	chp_new = Concat_Str(*chpp_org, chp_add);
	free(*chpp_org);
	*chpp_org = chp_new;
}

/*
+* ========================================================================
 * Function:		ʸ������ִ�
 * Include-file:
 * Description:
 *	chp_org���chp_before��chp_after���Ѵ�����ʸ�����
 *	�����˳��ݤ���������˺������롣
%* ========================================================================
 * Return:
 *	�Ѵ����ʸ���󡣻Ȥ�����ä���free�ǲ�����
-* ======================================================================*/
char* Replace_Str(const char* chp_org, const char* chp_before, const char* chp_after)
{
	char* chp_src;
	char* chp_ret;

	if (!chp_org || !chp_before || !chp_after)
		return (char*)chp_org;
	asprintf(&chp_ret, "%s", chp_org);
	if (!chp_before || !chp_before[0] || !chp_after)
		return chp_ret;

	chp_src = strstr(chp_ret, chp_before);
	while (chp_src) {
		int l;
		char* chp_new1;
		char* chp_new2;
		*chp_src = '\0';
		chp_new1 = Concat_Str(chp_ret, chp_after);
		chp_src += strlen(chp_before);
		chp_new2 = Concat_Str(chp_new1, chp_src);
		l = strlen(chp_new1);
		free(chp_ret);
		free(chp_new1);
		chp_ret = chp_new2;
		chp_src = strstr(chp_ret+l, chp_before);
	}

	return chp_ret;
}

/*
+* ========================================================================
 * Function:		�ڡ����Ѥ�ʸ����
 * Include-file:
 * Description:
 *	chp_sql�򸵤ˡ��ڡ���������׻�����ʸ����������
%* ========================================================================
 * Return:
 *	�Ѵ���ο�����
-* ======================================================================*/
char* Page_Control_Str(const char* chp_myname, const char* chp_param, int in_rows, int in_items_per_page, int in_start, int in_pagestart)
{
#define CO_LINE 11
	int i;
	int in_st;
	int in_max;
	int in_new;
	char* chp_out;
	char* chp_tmp;
	char* chp_str;
	char cha_and[3];

	if (in_rows <= in_items_per_page)
		return NULL;

	chp_out = malloc(1);
	*chp_out = '\0';

	/* chp_parm ����ƬȽ�� */
	if (chp_param[0] && chp_param[0] != '&') {
		strcpy(cha_and, "&");
	} else {
		strcpy(cha_and, "");
	}

	/* in_pagestart ���顼�����å� */
	if (in_pagestart > in_rows / in_items_per_page || in_pagestart < 0) {
		in_pagestart = 0;
	}

	/* in_start ���顼�����å� */
	if (in_start < 0) {
		in_start = 0;
	}
	if (in_start > (in_rows - 1)) {
		in_start = in_rows - in_items_per_page;
	}

	in_max = (in_rows + in_items_per_page - 1) / in_items_per_page;
	if (in_start / in_items_per_page) {
		in_new = in_pagestart;
		in_st = (in_start - in_items_per_page) / in_items_per_page;
		if (in_st - (CO_LINE / 2) <= 0) {
			in_new = 0;
		} else if (in_st + (CO_LINE / 2) >= in_max) {
		} else {
			--in_new;
		}
		asprintf(&chp_str, "<a href=\"%s?start=%d&pagestart=%d%s%s\">&lt;����</a>&nbsp;\n"
			 , chp_myname, in_start - in_items_per_page, in_new, cha_and, chp_param);
		chp_tmp = Concat_Str(chp_out, chp_str);
		free(chp_out);
		free(chp_str);
		chp_out = chp_tmp;
	} else {
		chp_tmp = Concat_Str(chp_out, "&lt;����&nbsp;\n");
		free(chp_out);
		chp_out = chp_tmp;
	}

	for (i = in_pagestart; i < in_pagestart + CO_LINE && i < in_max; ++i) {
		if (i < (CO_LINE / 2)) {
			in_st = 0;
		} else if (i - (CO_LINE / 2) + CO_LINE >= in_max) {
			if (in_max >= CO_LINE) {
				in_st = in_max - CO_LINE;
			} else {
				in_st = 0;
			}
		} else {
			in_st = i - (CO_LINE / 2);
		}
		if (i != in_start / in_items_per_page) {
			asprintf(&chp_str, "<a href=\"%s?start=%d&pagestart=%d%s%s\">",
				chp_myname, in_items_per_page * i, in_st, cha_and, chp_param);
			chp_tmp = Concat_Str(chp_out, chp_str);
			free(chp_out);
			free(chp_str);
			chp_out = chp_tmp;
		}
		asprintf(&chp_str, "%d", i + 1);
		chp_tmp = Concat_Str(chp_out, chp_str);
		free(chp_out);
		free(chp_str);
		chp_out = chp_tmp;
		if (i != in_start / in_items_per_page) {
			chp_tmp = Concat_Str(chp_out, "</a>");
			free(chp_out);
			chp_out = chp_tmp;
		}
		chp_tmp = Concat_Str(chp_out, "&nbsp;\n");
		free(chp_out);
		chp_out = chp_tmp;
	}

	if ((in_start / in_items_per_page < (in_rows - 1)/ in_items_per_page) || in_start + in_items_per_page < in_rows) {
		in_st = (in_start + in_items_per_page) / in_items_per_page;
		if (in_st - (CO_LINE / 2) <= 0) {
			in_pagestart = 0;
		} else if (in_st + (CO_LINE / 2) >= in_max) {
		} else {
			++in_pagestart;
		}
		asprintf(&chp_str, "<a href=\"%s?start=%d&pagestart=%d%s%s\">����&gt;</a>\n", chp_myname
			 , in_start + in_items_per_page, in_pagestart, cha_and, chp_param);
		chp_tmp = Concat_Str(chp_out, chp_str);
		free(chp_out);
		free(chp_str);
		chp_out = chp_tmp;
	} else {
		chp_tmp = Concat_Str(chp_out, "����&gt;\n");
		free(chp_out);
		chp_out = chp_tmp;
	}
	return chp_out;
}

/*
+* ========================================================================
 * Function:		�ڡ����Ѥ�ʸ����
 * Include-file:
 * Description:
 *	chp_sql�򸵤ˡ��ڡ���������׻�����ʸ����������
%* ========================================================================
 * Return:
 *	�Ѵ���ο�����
-* ======================================================================*/
char* Page_Control_Str_by_Sql(DBase *db, const char* chp_sql, const char* chp_myname, const char* chp_param, int in_items_per_page, int in_start, int in_pagestart, int* inp_rows)
{
	DBRes *res;
	char* chp_new;

	*inp_rows = 0;
	chp_new = NULL;
	res = Db_OpenDyna(db, (char*)chp_sql);
	if (res) {
		*inp_rows = Db_GetRowCount(res);
		Db_CloseDyna(res);
		chp_new = Page_Control_Str(chp_myname, chp_param, *inp_rows, in_items_per_page, in_start, in_pagestart);
	}
	return chp_new;
}

/*
+* ========================================================================
 * Function:		Match_Filter
 * Include-file:
 * Description:
 *	�������ν񤭹�����˥��ڡ����Ƕ��ڤ�줿ʸ����¸�ߤ��뤫��ǧ
%* ========================================================================
 * Return:
 *	==0: ����ޤ���
 *  !=0: ����ޤ�����
-* ======================================================================*/
int Match_Filter(const char* chp_entry, const char* chp_filter)
{
	char *chp_buff;
	char *chp_token;

	chp_buff = (char*)malloc(strlen(chp_filter) + 1);
	if (!chp_buff)
		return 0;

	strcpy(chp_buff, chp_filter);
	chp_token = strtok(chp_buff, "\n");
	while (chp_token) {
		if (chp_token[0]) {
			char *chp_fnd = strchr(chp_token, '\r');
			if (chp_fnd)
				*chp_fnd = '\0';
			if (strcasestr(chp_entry, chp_token)) {
				free(chp_buff);
				return 1;
			}
		}
		chp_token = strtok(NULL, "\n");
	}
	free(chp_buff);
	return 0;
}

/*
+* ========================================================================
 * Function:		Check_RBL
 * Include-file:
 * Description:
 *	����������Ȥ�IP��RBL����Ͽ����Ƥ��ʤ�����ǧ��
%* ========================================================================
 * Return:
 *	==0: ����ޤ���
 *  !=0: ����ޤ�����
-* ======================================================================*/
int Check_RBL()
{
	struct hostent *host;
	struct in_addr *ptr;
	int i;
	char hb[100];
	char cha_ip_host[256];
	char *p;
	char *chp_tmp;
	char *s, *sb, *sc;
	static char *chpa_rbl_domain[] = {
		"web.dnsbl.sorbs.net.",
		"spam.dnsbl.sorbs.net.",
//		"list.dsbl.org.",
//		"multihop.dsbl.org.",
		"all.rbl.jp.",
		NULL
	};

	/* REMOTE_ADDR�������Ǥ��ʤ��ΤϤ��䤷�� */
	chp_tmp = getenv("REMOTE_ADDR");
	if (!chp_tmp) {
		fprintf(stderr, "%s(%d): No REMOTE_ADDR\n", __FILE__, __LINE__);
		return 1;
	}

	s = chp_tmp + strlen(chp_tmp);
	sb = hb;
	while (--s != chp_tmp) {
		if (*s == '.') {
			sc = s;
			while (*++sc != '.' && *sc)
				*sb++ = *sc;
			*sb++ = '.';
		}
	}
	sc = s;
	while (*sc != '.' && *sc)
		*sb++ = *sc++;
	*sb++ = '.';

	for (i = 0; chpa_rbl_domain[i]; ++i) {
		strcpy(cha_ip_host, hb);
		strcat(cha_ip_host, chpa_rbl_domain[i]);
		host = gethostbyname(cha_ip_host);
		if (host || (h_errno != HOST_NOT_FOUND && h_errno != TRY_AGAIN)) {
			p = strcasestr(hb, ".dnsbl.sorbs.net.");
			if (host != NULL && p != NULL && (strcasecmp(p, ".dnsbl.sorbs.net.") == 0)) {
				if (host->h_addrtype == AF_INET) {
					while ((ptr = (struct in_addr *)*host->h_addr_list++) != NULL) {
						if (strcmp(inet_ntoa(*ptr), "127.0.0.10") == 0) {
							return 0;
						}
					}
				}
			}
			fprintf(stderr, "%s(%d): RBL Check - Black(%s)\n", __FILE__, __LINE__, hb);
			return 1;
		}
	}
	return 0;
}

/*
+* ========================================================================
 * Function:            need_login()
 * Description:
 *      �֥�����ɽ������ǧ�ڤ�ɬ�פ�
%* ========================================================================
 * Return:              ���� 0
 *      �����          ɬ�� 1
-* ========================================================================*/
int Need_Login(DBase *db, int in_blog)
{
	DBRes *dbres;
	char cha_sql[1024];
	char *chp_tmp;
	int in_ret;

	sprintf(cha_sql, "select b_needlogin from at_blog where n_blog_id = %d;", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		return 1;
	}
	chp_tmp = Db_FetchValue(dbres, 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		return 1;
	}
	in_ret = atoi(chp_tmp);
	Db_CloseDyna(dbres);

	return in_ret;
}

/*
+* =============================================================================
 * Function:		Build_MultiComboDb
 * Include-file:	libcgi2.h
 * Description:
 * ʣ����ɽ����Combobox�򥯥��꡼(chp_query)�����������
%* =============================================================================
 * Return:
 * 0: ���ｪλ
 * 1�ʾ�: �����꡼���顼
-* =============================================================================
*/
int Build_MultiComboDb_Blog(
	DBase *db
	,NLIST *nlp_out
	,char *chp_insert
	,char *chp_name			/* ComboBOX ��̾��  */
	,char *chp_query		/* �ץ饤�ޥꥭ����ɽ���ͤ�������륯���꡼*/
	,int in_row				/* ComboBOX �ιԿ� */
	,char *chp_selected)
{
	DBRes *dbres;

	dbres = Db_OpenDyna(db, chp_query);
	if (!dbres) {
		goto empty_list;
	}
	if (!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		goto empty_list;
	}
	Db_CloseDyna(dbres);
	return Build_MultiComboDb(OldDBase(db), nlp_out, chp_insert, chp_name, chp_query, in_row, chp_selected);

empty_list:
	Put_Format_Nlist(nlp_out, chp_insert, "<select name=\"%s\" size=%d>\n", chp_name, in_row);
	Put_Nlist(nlp_out, chp_insert, "\t<option value=\"\">��������������������</option>\n");
	Put_Nlist(nlp_out, chp_insert, "</select>\n");

	return 0;
}

/*
+* =============================================================================
 * Function:		Dialog_Message
 * Include-file:
 * Description:
 * ���������ܥå����˼¹Է�����Υ�å���������Ϥ���
%* =============================================================================
 * Return:
-* =============================================================================
*/
void Dialog_Message(NLIST *nlp_out, char *chp_insert, char *chp_message)
{
	Put_Nlist(nlp_out, chp_insert, "<script language=\"JavaScript\">\n");
	Put_Nlist(nlp_out, chp_insert, "<!--\n");
	Put_Format_Nlist(nlp_out, chp_insert, "alert(\"%s\");\n", chp_message);
	Put_Nlist(nlp_out, chp_insert, "//-->\n");
	Put_Nlist(nlp_out, chp_insert, "</script>\n");
}

/*
+* =============================================================================
 * Function:		Dialog_Message_Query
 * Include-file:
 * Description:
 * ���������ܥå����˼¹Է�����Υ�å���������Ϥ���
%* =============================================================================
 * Return:
-* =============================================================================
*/
void Dialog_Message_Query(NLIST *nlp_out, char *chp_insert, char *chp_message, char *chp_query)
{
	Put_Nlist(nlp_out, chp_insert, "<script language=\"JavaScript\">\n");
	Put_Nlist(nlp_out, chp_insert, "<!--\n");
	Put_Nlist(nlp_out, chp_insert, "args = new Array();\n");
	Put_Format_Nlist(nlp_out, chp_insert, "args[0] = \"%s\";\n", chp_message);
	Put_Format_Nlist(nlp_out, chp_insert, "args[1] = \"%s\";\n", chp_query);
	Put_Format_Nlist(nlp_out, chp_insert, "window.showModalDialog(\"%s/blog_error.html\", args, \"dialogHeight:400px;dialogWidth:640px\");\n", g_cha_script_location);
	Put_Nlist(nlp_out, chp_insert, "//-->\n");
	Put_Nlist(nlp_out, chp_insert, "</script>\n");
}

/*
+* =============================================================================
 * Function:		Dialog_Message
 * Include-file:
 * Description:
 * ���������ܥå����˼¹Է�����Υ�å���������Ϥ�������Υڡ����إ�����
%* =============================================================================
 * Return:
-* =============================================================================
*/
void Dialog_Message_Jump(char *chp_message, char *chp_jump)
{
	printf("Content-Type: text/html\n\n");
	printf("<HTML>\n");
	printf("<HEAD>\n");
	printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
	printf("</HEAD>\n");
	printf("<script language=\"JavaScript\">\n");
	printf("<!--\n");
	printf("alert(\"%s\");\n", chp_message);
	printf("location.href=\"%s\"", chp_jump);
	printf("//-->\n");
	printf("</script>\n");
	printf("</html>\n");
}

/*
int is_open_tag(char *chp_tag)
{
	int i;
	static char *chpp_tags[] = {
		"area","base","basefont","bgsound","br","col",
		"colgroup","dd","dt","embed","frame","head",
		"hr","html","img","input","isindex","keygen",
		"li","link","meta","nextid","option","p",
		"param","plaintext","rb","rp","rt","spacer",
		"tbody","td","tfoot","th","thead","tr","wbr",
		NULL};
	for (i = 0; chpp_tags[i]; ++i) {
		if (strcasecmp(chp_tag, chpp_tags[i]) == 0)
			return 1;
	}
	return 0;
}

*/
/*
+* =============================================================================
 * Function:		Get_String_After
 * Description:
 * 				�����ʸ�����in_lenʸ�����ʸ������֤���
 *				��������Tag�Ϸ׻�������ʤ�
%* =============================================================================
 * Return:
 *			0: ���ｪλ
 *			1: ���顼
-* =============================================================================*/
/*
char* Get_String_After(char *chp_str, int in_len)
{
	int in_tag;
	int in_cnt;
	int in_out;
	int in_l;
	int i;
	char cha_tag[1024];
	char *chp_src;

	in_tag = 0;
	in_cnt = 0;
	in_out = 0;
	cha_tag[0] = '\0';
	chp_src = chp_str;
	while (*chp_src && (in_cnt < in_len || cha_tag[0])) {
		if (in_tag) {
			if (*chp_src == '>') {
				in_tag = 0;
			}
			++chp_src;
		} else {
			if (*chp_src == '<') {
				in_tag = 1;
				if (chp_src[1] == '/') {
					if (cha_tag[0]) {
						in_l = strlen(cha_tag);
						if (strncasecmp(cha_tag, chp_src + 2, in_l) == 0 && (chp_src[in_l + 2] <= ' ' || chp_src[in_l + 2] == '>' )) {
							cha_tag[0] = '\0';
							chp_src += in_l + 2;
							if (chp_src[0] == '>')
								in_tag = 0;
						}
					}
					++chp_src;
				} else if (!cha_tag[0]) {
					++chp_src;
					for (i = 0; *chp_src > ' ' && *chp_src != '>'; ++i) {
						cha_tag[i] = *chp_src;
						++chp_src;
					}
					cha_tag[i] = '\0';
					if (is_open_tag(cha_tag)) {
						cha_tag[0] = '\0';
					}
				} else {
					++chp_src;
				}
			} else if (!in_tag) {
				if (iseuc((unsigned char*)chp_src)) {
					++in_cnt;
					++chp_src;
				}
				++in_cnt;
				++chp_src;
			}
		}
	}
	return chp_src;
}
*/
int is_open_tag(char *chp_tag)
{
	int i;
	static char *chpp_tags[] = {"br","p",NULL};
	for (i = 0; chpp_tags[i]; ++i) {
		if (strcasecmp(chp_tag, chpp_tags[i]) == 0)
			return 1;
	}
	return 0;
}

void search_next_tag(const char *chp_str, const char *chp_tag, int in_len, int *inp_current_pos, int *inp_chars, int *inp_depth)
{
	int in_cnt;
	int in_out;
	int in_chr;
	int in_l;
	int i;
	char cha_tag[1024];
	const char *chp_src;

	in_cnt = *inp_current_pos;
	in_chr = *inp_chars;
	in_out = 0;
	chp_src = chp_str + *inp_current_pos;
	while (*chp_src) {
		if (*chp_src == '<') {
			if (chp_src[1] == '/') {
				in_l = strlen(chp_tag);
				if (strncasecmp(chp_tag, chp_src + 2, in_l) == 0 && (chp_src[in_l + 2] <= ' ' || chp_src[in_l + 2] == '>' )) {
					chp_src += in_l + 2;
					in_cnt += in_l + 2;
					while (*chp_src && *chp_src != '>') {
						++chp_src;
						++in_cnt;
					}
					if (*chp_src == '>') {
						++chp_src;
						++in_cnt;
					}
					*inp_chars = in_chr;
					*inp_current_pos = in_cnt;
					--*inp_depth;
					return;
				}
				while (*chp_src && *chp_src != '>') {
					++chp_src;
					++in_cnt;
				}
				++chp_src;
				++in_cnt;
			} else {
				if (!*inp_depth && in_chr >= in_len) {
					break;
				}
				++chp_src;
				++in_cnt;
				for (i = 0; *chp_src > ' ' && *chp_src != '>'; ++i) {
					cha_tag[i] = *chp_src;
					++chp_src;
					++in_cnt;
				}
				cha_tag[i] = '\0';
				while (*chp_src && *chp_src != '>') {
					++chp_src;
					++in_cnt;
				}
				if (*chp_src == '>') {
					++chp_src;
					++in_cnt;
				}
				if (!is_open_tag(cha_tag)) {
					++*inp_depth;
					search_next_tag(chp_str, cha_tag, in_len, &in_cnt, &in_chr, inp_depth);
					chp_src = chp_str + in_cnt;
				}
			}
		} else {
			if (!*inp_depth && in_chr >= in_len) {
				break;
			}
			if (iseuc((unsigned char*)chp_src)) {
				++in_cnt;
				++in_chr;
				++chp_src;
			}
			++in_chr;
			++in_cnt;
			++chp_src;
		}
	}
	*inp_chars = in_chr;
	*inp_current_pos = in_cnt;
}

char* Get_String_After(char *chp_str, int in_len)
{
	int in_cur_pos;
	int in_chars;
	int in_depth;

	in_chars = 0;
	in_cur_pos = 0;
	in_depth = 0;
	search_next_tag(chp_str, "", in_len, &in_cur_pos, &in_chars, &in_depth);
	return chp_str + in_cur_pos;
}

/*
+* =============================================================================
 * Function:		Decode_Blog_Form
 * Description:
 * 				�����URLʸ�����%XX��%uXXXX��ʸ�������ɤ��Ѵ����롣
%* =============================================================================
 * Return:
 *			 0: ���ｪλ
 *			-1: ���顼
-* =============================================================================*/
int Decode_Blog_Form(char* chp_value)
{
	int i, j;
	char *chp_temp;
	char ch_buf;
	char ch_buf2;
	int in_len;

	in_len = strlen(chp_value);
	if (in_len == 0)
		return 0;
	chp_temp = (char*)malloc(in_len);
	if (!chp_temp)
		return -1;
	for(i = 0, j = 0; i < in_len; i++, j++) {
		if (chp_value[i] == '+') {
			chp_temp[j]=' ';
			continue;
		}
		if (chp_value[i] != '%') {
			chp_temp[j] = chp_value[i];
			continue;
		}
		if (chp_value[i+1] == 'u' || chp_value[i+1] == 'U') {
			++i;
			ch_buf = ((chp_value[++i] >= 'A') ? (chp_value[i] & 0xdf) - 'A' + 10 : chp_value[i] - '0');
			ch_buf *= 16;
			ch_buf += ((chp_value[++i] >= 'A') ? (chp_value[i] & 0xdf) - 'A' + 10 : chp_value[i] - '0');
			ch_buf2 = ((chp_value[++i] >= 'A') ? (chp_value[i] & 0xdf) - 'A' + 10 : chp_value[i] - '0');
			ch_buf2 *= 16;
			ch_buf2 += ((chp_value[++i] >= 'A') ? (chp_value[i] & 0xdf) - 'A' + 10 : chp_value[i] - '0');
			chp_temp[j++] = ch_buf2;
			chp_temp[j] = ch_buf;
		} else {
			ch_buf = ((chp_value[++i] >= 'A') ? (chp_value[i] & 0xdf) - 'A' + 10 : chp_value[i] - '0');
			ch_buf *= 16;
			ch_buf += ((chp_value[++i] >= 'A') ? (chp_value[i] & 0xdf) - 'A' + 10 : chp_value[i] - '0');
			chp_temp[j] = ch_buf;
		}
	}
	for (i = 0; i < j; i++)
		chp_value[i] = chp_temp[i];
	chp_value[i] = '\0';
	free(chp_temp);

	return(0);
}

/*
+* =============================================================================
 * Function:		Decode_Blog_Form
 * Description:
 * 				�����URLʸ�����%XX��%uXXXX��ʸ�������ɤ��Ѵ����롣
%* =============================================================================
 * Return:
 *			 0: ���ｪλ
 *			-1: ���顼
-* =============================================================================*/
char* Encode_Blog_Url(const char* chp_url)
{
	unsigned char c;
	const char* chp_str;
	char* chp_dst;
	char* chp_out;

	chp_out = (char*)malloc(strlen(chp_url)*3 + 1);
	chp_dst = chp_out;
	chp_str = chp_url;
	while (*chp_str) {
		c = (unsigned char)*chp_str;
		if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xEF)) {
			sprintf(chp_dst, "%%%02X", c);
			chp_dst += 3;
			++chp_str;
			if(*chp_str == '\0')
				break;
			c = (unsigned char)*chp_str;
			sprintf(chp_dst, "%%%02X", c);
			chp_dst += 3;
			++chp_str;
		} else if ((c >= 0x40 && c <= 0x5A) ||
				(c >= 0x61 && c <= 0x7A) ||
				(c >= 0x30 && c <= 0x39) ||
				c == 0x2A || c == 0x2D || c == 0x2E || c == 0x5F)
		{
			*chp_dst++ = *chp_str++;
		} else {
			sprintf(chp_dst, "%%%02X", c);
			chp_dst += 3;
			++chp_str;
		}
	}
	*chp_dst = '\0';
	return chp_out;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	article_table_image_head()
 * Description:
 *	�����ơ��֥�ΰϤ߲����������إå�
%* ------------------------------------------------------------------------
 * Return:
 *	�ʤ�
-* ------------------------------------------------------------------------*/
void article_table_image_head(NLIST *nlp_in, NLIST *nlp_out)
{
	/* �ȥåץڡ����ʳ� */
	if (Get_Nlist(nlp_in, "eid", 1) || Get_Nlist(nlp_in, "cid", 1) || Get_Nlist(nlp_in, "kid", 1) || Get_Nlist(nlp_in, "date", 1) || Get_Nlist(nlp_in, "month", 1)) {
		Put_Nlist(nlp_out, "ARTICLE", "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"560\" align=\"center\">\n");
	/* �ȥåץڡ��� */
	} else {
		Put_Nlist(nlp_out, "ARTICLE", "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"top-main\">\n");
	}
	if (g_in_dbb_mode) {
		Put_Nlist(nlp_out, "ARTICLE", "<tr>\n");
		Put_Nlist(nlp_out, "ARTICLE", "<td class=\"frame-top-left\"></td>\n");
		Put_Nlist(nlp_out, "ARTICLE", "<td class=\"frame-top-middle\"></td>\n");
		Put_Nlist(nlp_out, "ARTICLE", "<td class=\"frame-top-right\"></td>\n");
		Put_Nlist(nlp_out, "ARTICLE", "</tr>\n");
	}
	Put_Nlist(nlp_out, "ARTICLE", "<tr>\n");
	if (g_in_dbb_mode) {
		Put_Nlist(nlp_out, "ARTICLE", "<td class=\"frame-middle-left\"></td>\n");
	}
	Put_Nlist(nlp_out, "ARTICLE", "<td align=\"left\">\n");
	return;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	article_table_image_tail()
 * Description:
 *		(�ȥåץڡ���)�����ơ��֥�ΰϤ߲����������եå�
%* ------------------------------------------------------------------------
 * Return:
 *	�ʤ�
-* ------------------------------------------------------------------------*/
void article_table_image_tail(NLIST *nlp_in, NLIST *nlp_out)
{
	char cha_html[8192];

	strcpy(cha_html,"</td>\n");
	if (g_in_dbb_mode) {
		strcat(cha_html, "<td class=\"frame-middle-right\"></td>\n");
	}
	strcat(cha_html, "</tr>\n");
	if (g_in_dbb_mode) {
		strcat(cha_html, "<tr>\n");
		strcat(cha_html, "<td class=\"frame-bottom-left\"></td>\n");
		strcat(cha_html, "<td class=\"frame-bottom-middle\"></td>\n");
		strcat(cha_html, "<td class=\"frame-bottom-right\"></td>\n");
		strcat(cha_html, "</tr>\n");
	}
	strcat(cha_html, "</table>\n");
	if(Get_Nlist(nlp_in, "eid", 1)) {
		Put_Nlist(nlp_out, "IMAGEFOOTER", cha_html);
	} else {
		Put_Nlist(nlp_out, "ARTICLE", cha_html);
	}
	return;
}

/*
+* =============================================================================
 * Function:		Create_Blog
 * Description:
 * 		�����DB��ˣ��ĥ֥�����������ǡ��������
%* =============================================================================
 * Return:
 *			!=0: ��������blogid
 *			==0: ���顼
-* =============================================================================*/
int Create_Blog(DBase *db)
{
	char cha_sql[1024];
	int in_blog;

	if(Db_ExecSql(db, "insert into at_blog (n_blog_id,d_create_time) values (NULL,now());")){
		printf("failed to exec query(%s)\n", Gcha_last_error);
		Db_Disconnect(db);
		return 0;
	}
	in_blog = My_Db_GetInsertId(db);

	strcpy(cha_sql, "update at_blog set");
	strcat(cha_sql, " c_blog_title = ''");
	strcat(cha_sql, ",c_blog_subtitle = ''");
	strcat(cha_sql, ",c_blog_description = ''");
	strcat(cha_sql, ",n_blog_category = 0");
	strcat(cha_sql, ",b_setting_toppage_disptype =0");
	strcat(cha_sql, ",n_setting_toppage_index = 10");
	strcat(cha_sql, ",b_default_trackback = 1");
	strcat(cha_sql, ",b_default_comment = 1");
	strcat(cha_sql, ",b_default_mode = 1");
	strcat(cha_sql, ",b_needlogin = 1");
	strcat(cha_sql, ",c_mail_subject = '%b����������'");
	strcat(cha_sql, ",c_mail_body = '%c����\r\n�֥��򹹿����ޤ����ΤǤ��Τ餻���ޤ�������URL��ꥢ���������Ƥ���������\r\n%u\r\n'");
	sprintf(cha_sql+strlen(cha_sql), " where n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 0;
	}
	/* �Ѱդ��줿PING������Υ����å��ܥå������� */
	sprintf(cha_sql,
		"insert into at_ping (n_blog_id,n_ping_id,c_ping_site,c_ping_url,b_default)"
		" select %d,n_ping_id,c_ping_site,c_ping_url,0 from sy_ping", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// �ץ�ե��������ͺ���
	sprintf(cha_sql, "insert into at_profile (n_blog_id,c_author_nickname,c_login,c_password) values (%d,'','','');", in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// �ơ��޽���ͺ���
	sprintf(cha_sql, "insert into at_looks (n_blog_id,n_theme_id,n_category_id) values (%d,1,1);", in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// �����ɥ���ƥ�Ľ����
	sprintf(cha_sql,
		"insert into at_sidecontent "
			"(n_blog_id,n_sidecontent_id,n_sidecontent_order,n_sidecontent_type,c_sidecontent_title,b_display,b_allow_outer) "
		"select "
			"%d,n_sidecontent_id,n_sidecontent_order,n_sidecontent_type,c_sidecontent_title,1,b_allow_outer "
		"from "
			"sy_sidecontent;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// �Ѱդ��줿������RSS�����
	sprintf(cha_sql,
		"insert into at_rss "
			"(n_blog_id,n_rss_id,c_rss_title,c_rss_url,n_rss_article,b_rss_target,b_default) "
		"select "
			"%d,n_rss_id,c_rss_title,c_rss_url,0,0,1 "
		"from "
			"sy_rss;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// ���������ֽ����
	sprintf(cha_sql,
		"insert into at_archive "
			"(n_blog_id,n_archive_id,n_archive_value) "
		"select "
			"%d,n_archive_id,n_archive_value "
		"from "
			"sy_archive;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// ���ƥ�������
	sprintf(cha_sql,
		"insert into at_category "
			"(n_blog_id,n_category_id,n_category_order,c_category_name) "
		"select "
			"%d,n_category_id,n_category_order,c_category_name "
		"from "
			"sy_category;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// �����ȥե��륿�������
	sprintf(cha_sql,
		"insert into at_comment_filter "
			"(n_blog_id,c_filter,b_valid) "
		"values "
			"(%d,'',0) "
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}
	// �ȥ�å��Хå��ե��륿�������
	sprintf(cha_sql,
		"insert into at_trackback_filter "
			"(n_blog_id,n_filter_id,c_filter,b_valid) "
		"select "
			"%d,n_filter_id,'',0 "
		"from "
			"sy_filter;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		printf("failed to exec query(%s)(%s)\n", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 0;
	}

	return in_blog;
}

/*
+* =============================================================================
 * Function:		Remove_Invalid_Tags
 * Description:
 * 		�ȤäƤϤ����ʤ����������롣
%* =============================================================================
 * Return:
 *		!=NULL: ������ʸ����
 *		==NULL: ���顼
-* =============================================================================*/
char *Remove_Invalid_Tags(char *chp_source, NLIST *nlp_out, int in_mobile)
{
	FILE *fp;
	int fd;
	int in_len;
	int in_rt;
//	int in_error;
	uid_t in_uid;
	struct passwd *stp_pwd;
	char cha_cmd[2048];
	char cha_temp[1024];
//	char *chp_tmp;
	char *chp_dest;

	in_len = strlen(chp_source);
	chp_dest = (char *)calloc(in_len * 4 + 1, sizeof(char));
	if (!chp_dest) {
		Put_Nlist(nlp_out, "ERROR", "����γ��ݤ˼��Ԥ��ޤ�����");
		return NULL;
	}

	in_uid = getuid();
	stp_pwd = getpwuid(in_uid);
	if (!stp_pwd) {
		Put_Nlist(nlp_out, "ERROR", "�ѥ���ɾ���μ����˼��Ԥ��ޤ�����");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", strerror(errno));
		return NULL;
	}
	strcpy(cha_temp, stp_pwd->pw_dir);
	strcat(cha_temp, "/temp.XXXXXX");
	fd = mkstemp(cha_temp);
	if (!fd) {
		Put_Nlist(nlp_out, "ERROR", "�ƥ�ݥ��ե�����κ����˼��Ԥ��ޤ�����");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", strerror(errno));
		return NULL;
	}
	if (write(fd, chp_source, strlen(chp_source)) < 0) {
		Put_Nlist(nlp_out, "ERROR", "�ƥ�ݥ��ե�����κ����˼��Ԥ��ޤ�����");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", strerror(errno));
		close(fd);
		remove(cha_temp);
		return NULL;
	}
	close(fd);
/*
	in_error = 0;
	sprintf(cha_cmd, "/usr/local/bin/htmllint -x ie55 %s", cha_temp);
	fp = popen(cha_cmd, "r+");
	if (!fp) {
		Put_Nlist(nlp_out, "ERROR", "HTML�����å����ε�ư�˼��Ԥ��ޤ�����");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", strerror(errno));
		return NULL;
	}
	while (fgets(cha_cmd, sizeof(cha_cmd), fp)) {
		char *chp_line;
		chp_line = strchr(cha_cmd, '(');
		if (chp_line) {
			++chp_line;
			chp_tmp = strchr(chp_line, ')');
			if (chp_tmp) {
				*chp_tmp = '\0';
				++chp_tmp;
				if (*chp_tmp == ':') {
					char *chp_err;
					++chp_tmp;
					chp_err = chp_tmp;
					chp_tmp = strchr(chp_err, ':');
					if (chp_tmp) {
						int in_line;
						int in_err;
						*chp_tmp = '\0';
						in_line = atoi(chp_line);
						in_err = atoi(chp_err);
						if (in_line > 1 && in_err >= 8) {
							++in_error;
							++chp_tmp;
							chp_err = Escape_HtmlString(chp_tmp);
							Put_Format_Nlist(nlp_out, "QUERY", "(%d����)��%s<BR>\n", in_line, chp_err);
							free(chp_err);
						}
					}
				}
			}
		}
	}
	pclose(fp);
	if (in_error) {
		Put_Nlist(nlp_out, "ERROR", "HTML�ε��Ҥ˸�꤬����ޤ���");
		return NULL;
	}
*/
	if (in_mobile) {
		sprintf(cha_cmd, "/usr/local/bin/safehtml.mobile < %s", cha_temp);
	} else {
		sprintf(cha_cmd, "/usr/local/bin/safehtml < %s", cha_temp);
	}
	fp = popen(cha_cmd, "r+");
	if (!fp) {
		Put_Nlist(nlp_out, "ERROR", "HTML�����å����ε�ư�˼��Ԥ��ޤ�����");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", strerror(errno));
		remove(cha_temp);
		return NULL;
	}
	in_rt = fread(chp_dest, 1, in_len * 4, fp);
	chp_dest[in_rt] = '\0';
	pclose(fp);
	remove(cha_temp);

	return chp_dest;
}

/*
+* =============================================================================
 * Function:		Calc_Body_Point
 * Description:
 * 		��ʸ�����Ƥ�������Ĥ��롣
%* =============================================================================
 * Return:
 *		>=0: ����
 *		< 0: ���顼
-* =============================================================================*/
int Calc_Body_Point(DBase *db, NLIST *nlp_out, int in_blog, int in_entry, int in_new, const char *chp_source, const char *chp_access_key)
{
#define MAXKIND 100
	typedef struct _T {
		char cha_kind[20];
		int in_point;
		int in_max;
	} KINDPOIND;
	FILE *fp;
	KINDPOIND sta_kind_point[MAXKIND];
	DBRes *dbres;
	NLIST *nlp_temp;
	int i;
	int fd;
	int in_cup;
	int in_point;
	int in_find;
	int in_black;
	int in_caution;
	int in_special;
	int in_black_minus;
	int in_caution_minus;
	uid_t in_uid;
	struct passwd *stp_pwd;
	char cha_cmd[2048];
	char cha_sql[1024];
	char cha_temp[1024];
	char *chp_tmp;
	char *chp_word;
	char *chp_hinsi;
	char *chp_point;
	char *chp_kind;

	in_uid = getuid();
	if (in_uid < 0) {
		Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", strerror(errno));
		return INT_MAX;
	}
	stp_pwd = getpwuid(in_uid);
	if (!stp_pwd) {
		Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", strerror(errno));
		return INT_MAX;
	}
	strcpy(cha_temp, stp_pwd->pw_dir);
	strcat(cha_temp, "/temp.XXXXXX");
	fd = mkstemp(cha_temp);
	if (!fd) {
		Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", strerror(errno));
		return INT_MAX;
	}
	if (write(fd, chp_source, strlen(chp_source)) < 0) {
		Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", strerror(errno));
		close(fd);
		remove(cha_temp);
		return INT_MAX;
	}
	close(fd);

	sprintf(cha_cmd, "/usr/local/bin/chasen < %s | /usr/bin/grep '̾��-���'", cha_temp);
	fp = popen(cha_cmd, "r+");
	if (!fp) {
		Put_Nlist(nlp_out, "ERROR", "�ץ�����ư�˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", strerror(errno));
		remove(cha_temp);
		return INT_MAX;
	}
	memset(&sta_kind_point, 0, sizeof(sta_kind_point));
	in_point = 0;
	nlp_temp = Init_Nlist();
	while (fgets(cha_cmd, sizeof(cha_cmd), fp)) {
		chp_word = strtok(cha_cmd, "\t");
		strtok(NULL, "\t");
		strtok(NULL, "\t");
		chp_hinsi = strtok(NULL, "\t");
		if (chp_word && chp_hinsi) {
			strtok(chp_hinsi, "-");
			strtok(NULL, "-");
			chp_kind = strtok(NULL, "-");
			chp_point = strtok(NULL, "-");
			if (chp_point) {
				if (chp_point[0] == 'M') {
					chp_point[0] = '-';
				}
				in_cup = atoi(chp_point);
				if (!Get_Nlist(nlp_temp, chp_word, 1)) {
					Put_Nlist(nlp_temp, chp_word, chp_point);
					if (chp_kind) {
						in_find = 0;
						for (i = 0; i < MAXKIND && sta_kind_point[i].cha_kind[0]; ++i) {
							if (strcmp(sta_kind_point[i].cha_kind, chp_kind) == 0) {
								in_find = 1;
								if (sta_kind_point[i].in_max && sta_kind_point[i].in_point + in_cup > sta_kind_point[i].in_max) {
									in_cup = (sta_kind_point[i].in_max - sta_kind_point[i].in_point);
									sta_kind_point[i].in_point = sta_kind_point[i].in_max;
								} else {
									sta_kind_point[i].in_point += in_cup;
								}
								break;
							}
						}
						if (!in_find) {
							sprintf(cha_sql, "select T1.n_max from sy_point_kind T1 where T1.c_name = '%s'", chp_kind);
							dbres = Db_OpenDyna(db, cha_sql);
							if (!dbres) {
								Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
								Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
								return INT_MAX;
							}
							chp_tmp = Db_GetValue(dbres, 0, 0);
							sta_kind_point[i].in_max = chp_tmp ? atoi(chp_tmp) : 0;
							strcpy(sta_kind_point[i].cha_kind, chp_kind);
							sta_kind_point[i].in_point = in_cup;
							Db_CloseDyna(dbres);
						}
					}
					in_point += in_cup;
				}
				if (in_new && in_cup > 0) {
					sprintf(cha_sql, "update at_keyword_appear set n_count = n_count + 1 where c_keyword = '%s' and d_appear = CURDATE()", chp_word);
					if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
						Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
						Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
						return INT_MAX;
					}
					if (!My_Db_GetAffectedRows(db)) {
						sprintf(cha_sql, "insert into at_keyword_appear (c_keyword, d_appear, n_count) values ('%s', CURDATE(), 1)", chp_word);
						if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
							Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
							Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
							return INT_MAX;
						}
					}
				}
			}
		}
	}
	pclose(fp);
	remove(cha_temp);
	Finish_Nlist(nlp_temp);

	in_black = Is_Black_List(db, nlp_out, chp_access_key);
	if (in_black < 0) {
		return INT_MAX;
	}
	in_caution = Is_Caution_Owner(db, nlp_out, chp_access_key);
	if (in_caution < 0) {
		return INT_MAX;
	}
	in_black_minus = 0;
	in_caution_minus = 0;
	if (in_black || in_caution) {
		sprintf(cha_sql, "select T1.n_max from sy_point_kind T1 where T1.n_kind = %d or T1.n_kind = %d order by T1.n_kind", CO_POINT_BLACKLIST, CO_POINT_CAUTION);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return INT_MAX;
		}
		if (in_black) {
			chp_tmp = Db_GetValue(dbres, 0, 0);
			in_black_minus = chp_tmp ? atoi(chp_tmp) : 0;
			in_point -= in_black_minus;
		}
		if (in_caution) {
			chp_tmp = Db_GetValue(dbres, 1, 0);
			in_caution_minus = chp_tmp ? atoi(chp_tmp) : 0;
			in_point -= in_caution_minus;
		}
		Db_CloseDyna(dbres);
	}

	in_special = 0;
	if (!in_new) {
		sprintf(cha_sql, "select n_point from at_point_detail where n_blog_id = %d and n_entry_id = %d and n_kind = %d", in_blog, in_entry, CO_POINT_SPECIAL);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return INT_MAX;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_special = atoi(chp_tmp);
			in_point += in_special;
		}
		Db_CloseDyna(dbres);
	}

	sprintf(cha_cmd, "delete from at_point_detail where n_blog_id = %d and n_entry_id = %d", in_blog, in_entry);
	if (Db_ExecSql(db, cha_cmd) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return INT_MAX;
	}
	for (i = 0; i < MAXKIND && sta_kind_point[i].cha_kind[0]; ++i) {
		sprintf(cha_cmd, "insert into at_point_detail select %d,%d,T1.n_kind,%d from sy_point_kind T1 where T1.c_name = '%s'",
			in_blog, in_entry, sta_kind_point[i].in_point, sta_kind_point[i].cha_kind);
		if (Db_ExecSql(db, cha_cmd) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return INT_MAX;
		}
	}
	if (in_black_minus) {
		sprintf(cha_cmd, "insert into at_point_detail (n_blog_id,n_entry_id,n_kind,n_point) values (%d,%d,%d,-%d)",
			in_blog, in_entry, CO_POINT_BLACKLIST, in_black_minus);
		if (Db_ExecSql(db, cha_cmd) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return INT_MAX;
		}
	}
	if (in_caution_minus) {
		sprintf(cha_cmd, "insert into at_point_detail (n_blog_id,n_entry_id,n_kind,n_point) values (%d,%d,%d,-%d)",
			in_blog, in_entry, CO_POINT_CAUTION, in_caution_minus);
		if (Db_ExecSql(db, cha_cmd) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
			Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
			return INT_MAX;
		}
	}
	sprintf(cha_cmd, "insert into at_point_detail (n_blog_id,n_entry_id,n_kind,n_point) values (%d,%d,%d,%d)",
		in_blog, in_entry, CO_POINT_SPECIAL, in_special);
	if (Db_ExecSql(db, cha_cmd) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return INT_MAX;
	}

	return in_point;
}

/*
+* =============================================================================
 * Function:		Delete_Old_Entry
 * Description:
 * 		HotBiz�⡼�ɻ���������¤�ۤ��������������롣
%* =============================================================================
 * Return:
 *		!=NULL: ������ʸ����
 *		==NULL: ���顼
-* =============================================================================*/
void Delete_Old_Entry(DBase *db)
{
	DBRes *dbres;
	int i;
	int in_blog;
	int in_entry;
	char cha_sql[1024];
	char *chp_tmp;

	dbres = Db_OpenDyna(db, "select n_blog_id,n_entry_id from at_entry where d_entry_delete_time is not null and d_entry_delete_time <= now()");
	if (!dbres) {
		return;
	}
	Begin_Transact(db);
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (!chp_tmp)
			break;
		in_blog = atoi(chp_tmp);
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (!chp_tmp)
			break;
		in_entry = atoi(chp_tmp);
		sprintf(cha_sql, "delete from at_entry where n_blog_id = %d and n_entry_id = %d", in_blog, in_entry);
		Db_ExecSql(db, cha_sql);
	}
	Commit_Transact(db);
	Db_CloseDyna(dbres);
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	check_future()
 * Description:
 *
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼�� 1
-* ------------------------------------------------------------------------*/
int check_future(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[1024];
	char cha_date[20];
	char cha_post[20];
	char *chp_tmp;

	Get_DateTimeNow(cha_date);
	sprintf(cha_post, "%s/%s/%s %s:%s",
		Get_Nlist(nlp_in, "year", 1), Get_Nlist(nlp_in, "month", 1), Get_Nlist(nlp_in, "day", 1),
		Get_Nlist(nlp_in, "hour", 1), Get_Nlist(nlp_in, "minute", 1));
	if (strcmp(cha_post, cha_date) <= 0) {
		return 0;
	}

	sprintf(cha_sql, "select count(*) from at_entry where d_entry_create_time > now() and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Db_CloseDyna(dbres);
		return 0;
	}
	if (atoi(chp_tmp) >= CO_MAX_FUTURE_ENTRY) {
		Db_CloseDyna(dbres);
		Put_Format_Nlist(nlp_out, "ERROR", "���ߤ��������դε�����%d�����Ǥ��ꤤ���ޤ���<br>", CO_MAX_FUTURE_ENTRY);
		return 1;
	}
	Db_CloseDyna(dbres);
	return 0;
}


/*
+* ------------------------------------------------------------------------
 * Function:		my_Http_Post_Get_Result_for_trackback
 * Description:
 *		����Υۥ��Ȥ�http��POST��������������ʸ�����Хåե����֤�
 *		�ȥ�å��Хå����ѥ��塼��
 *		���å����ϻ��Ѥ��ʤ��ΤǤȤꤢ������������Ϥ����Ƥ���
%* ------------------------------------------------------------------------
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
-* ------------------------------------------------------------------------*/
int my_Http_Post_Get_Result_for_trackback(
	 const char* chp_host		/* �ۥ��� */
	,const char* chp_path		/* �ѥ� */
	,const char* chp_params		/* �ѥ�᡼�� */
	,char** chpp_result			/* ��� */
)
{
	int i;
	int err;
	int sd;
	int ret;
	char cha_buff[1025];
	char cha_cookie[16384];
	char* chp_header;
	char* chp_last;
	char* chp_str;
	char *chp_value;
	struct sockaddr_in sa;
	struct hostent *he;
	struct servent *service;
	FILE* fp;
	NLIST *nlp_cur;
	NLIST *nlp_cur2;
	NLIST *nlp_cookie;

	he = gethostbyname(chp_host);
	if (!he) {
		return 1;
	}
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd < 0) {
		return 1;
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

	/* ���̤�post�Ȱ㤦�ΤϤ������� */
	if (cha_cookie && cha_cookie[0]) {
		asprintf(&chp_header,
			 "POST %s HTTP/1.0\r\n"
			 "Host: %s\r\n"
			 "%s"
			 "Content-Type: application/x-www-form-urlencoded\r\n"
			 "Content-length: %d\r\n\r\n"
			 "%s\r\n\r\n"
			 , chp_path, chp_host, cha_cookie, strlen(chp_params), chp_params);
	} else {
		asprintf(&chp_header,
			 "POST %s HTTP/1.0\r\n"
			 "Host: %s\r\n"
			 "Content-Type: application/x-www-form-urlencoded\r\n"
			 "Content-length: %d\r\n\r\n"
			 "%s\r\n\r\n"
			 , chp_path, chp_host, strlen(chp_params), chp_params);
	}
	ret = write(sd, chp_header, strlen(chp_header));
	if (ret < strlen(chp_header)){
		free(chp_header);
		err = errno;
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
	while (fgets(cha_buff, sizeof(cha_buff), fp)) {
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
+* =========================================================================
 * Function:            insert_link()
 * Description:
 *	��Ϣ��󥯤��ɲä��롣
%* =========================================================================
 * Return:
 *		�ɲ÷��
-* ========================================================================*/
int insert_link(DBase *db, NLIST *nlp_out, int in_blog, int in_entry, char *chp_url, int in_trackback, int in_tb_blog, int in_tb_entry)
{
	DBRes *dbres;
	NLIST *nlp_cur;
	NLIST *nlp_cur2;
	NLIST *nlp_cookie;
	CP_NKF nkf;
	NKF_CODE cd;
	int i;
	int in_link;
	int in_link_blog;
	int in_link_entry;
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

	sprintf(cha_sql, "select coalesce(max(T1.n_link_id), 0) + 1 from at_sendlink T1 where T1.n_blog_id=%d and T1.n_entry_id=%d for update", in_blog, in_entry);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "���ѵ���ID�����Υ�����˼��Ԥ��ޤ�����<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå�ID�����Υ�����˼��Ԥ��ޤ�����<br>");
		}
		Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return CO_ERROR;
	}
	chp_str = Db_GetValue(dbres, 0, 0);
	if (chp_str) {
		in_link = atoi(chp_str);
	} else {
		in_link = 1;
	}
	Db_CloseDyna(dbres);

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
	if (!cha_path[0]) {
		strcpy(cha_path, "/");
	}

	if (g_in_dbb_mode) {
		chp_tmp = strchr(g_cha_host_name, '.');
		if (!chp_tmp) {
			Put_Nlist(nlp_out, "ERROR", "�ɲäǤ����󥯤ϥɥ꡼��١����ܡ������URL�����Ǥ���<br>");
			return CO_ERROR;
		}
		++chp_tmp;
		strcpy(cha_sql, chp_tmp);
		if (!strcasestr(cha_host, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "�ɲäǤ����󥯤ϥɥ꡼��١����ܡ������URL�����Ǥ���<br>");
			return CO_ERROR;
		}
		if (!Get_Nickname_From_Blog(db, nlp_out, in_tb_blog, cha_param)) {
			Rollback_Transact(db);
			return CO_ERROR;
		}
		sprintf(cha_sql, "select c_entry_title from at_entry where n_blog_id = %d and n_entry_id = %d", in_tb_blog, in_tb_entry);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "���ѵ���ID�����Υ�����˼��Ԥ��ޤ�����<br>");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå�ID�����Υ�����˼��Ԥ��ޤ�����<br>");
			}
			Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return CO_ERROR;
		}
		chp_str = Db_GetValue(dbres, 0, 0);
		if (chp_str) {
			asprintf(&chp_title, "%s(%s)", Copy_Str(chp_str), cha_param);
		} else {
			asprintf(&chp_title, "%s(%s)", Copy_Str(chp_url), cha_param);
		}
		Db_CloseDyna(dbres);
		sprintf(cha_sql,
			"insert into at_sendlink "
				"(n_blog_id,n_entry_id,n_link_id,c_link_title,c_link_url,c_link_ip,b_link_tb) "
			"values "
				"(%d, %d, %d, '%s', '%s', '%d', %d)",
				in_blog, in_entry, in_link, chp_title ? chp_title : "", chp_url, in_tb_blog, in_trackback);
	} else {
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
		sprintf(cha_sql, "delete from at_sendlink where n_blog_id = %d and n_entry_id = %d and c_link_url = '%s'", in_blog, in_entry, chp_url);
		if (Db_ExecSql(db, cha_sql)) {
			free(chp_str);
			Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
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
		sprintf(cha_sql,
			"insert into at_sendlink "
				"(n_blog_id,n_entry_id,n_link_id,c_link_title,c_link_url,b_link_tb) "
			"values "
				"(%d, %d, %d, '%s', '%s', %d)",
				in_blog, in_entry, in_link, chp_title ? chp_title : "", chp_url, in_trackback);
	}
	if (Db_ExecSql(db, cha_sql)) {
		if (chp_title)
			free(chp_title);
		Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	if (chp_title)
		free(chp_title);

	if (strcasecmp(g_cha_host_name, cha_host) == 0) {
		in_link_blog = 0;
		in_link_entry = 0;
		if (g_in_dbb_mode) {
			cha_cookie[0] = '\0';
			sprintf(cha_sql, "%s%s%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location);
			strcat(cha_sql, "%s/?eid=%d");
			sscanf(chp_url, cha_sql, cha_cookie, &in_link_entry);
			if (!cha_cookie[0] || !in_link_entry) {
				sprintf(cha_sql, "%s%s%s/%s?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML);
				strcat(cha_sql, "bid=%s&eid=%d%");
				sscanf(chp_url, cha_sql, cha_cookie, &in_link_entry);
				if (!cha_cookie[0] || !in_link_entry) {
					sprintf(cha_sql, "%s%s%s/%s?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML);
					strcat(cha_sql, "eid=%d&bid=%s%");
					sscanf(chp_url, cha_sql, &in_link_entry, cha_cookie);
				}
			}
			if (cha_cookie[0]) {
				in_link_blog = Temp_To_Blog(db, nlp_out, cha_cookie);
				if (in_link_blog < 0) {
					return CO_ERROR;
				}
			}
		} else {
			sprintf(cha_sql, "%s%s%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location);
			strcat(cha_sql, "%08d/?eid=%d");
			sscanf(chp_url, cha_sql, &in_link_blog, &in_link_entry);
			if (!in_link_blog || !in_link_entry) {
				sprintf(cha_sql, "%s%s%s/%s?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML);
				strcat(cha_sql, "bid=%d&eid=%d%");
				sscanf(chp_url, cha_sql, &in_link_blog, &in_link_entry);
				if (!in_link_blog || !in_link_entry) {
					sprintf(cha_sql, "%s%s%s/%s?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML);
					strcat(cha_sql, "eid=%d&bid=%d%");
					sscanf(chp_url, cha_sql, &in_link_entry, &in_link_blog);
				}
			}
		}
		if (in_link_blog && in_link_entry) {
			sprintf(cha_sql, "select coalesce(max(T1.n_link_id), 0) + 1 from at_recvlink T1 where T1.n_blog_id=%d and T1.n_entry_id=%d for update", in_blog, in_entry);
			dbres = Db_OpenDyna(db, cha_sql);
			if (!dbres) {
				if (g_in_dbb_mode) {
					Put_Nlist(nlp_out, "ERROR", "���ѵ���ID�����Υ�����˼��Ԥ��ޤ�����<br>");
				} else {
					Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå�ID�����Υ�����˼��Ԥ��ޤ�����<br>");
				}
				Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				Rollback_Transact(db);
				return CO_ERROR;
			}
			chp_str = Db_GetValue(dbres, 0, 0);
			if (chp_str) {
				in_link = atoi(chp_str);
			} else {
				in_link = 1;
			}
			Db_CloseDyna(dbres);
			sprintf(cha_sql, "delete from at_recvlink where n_blog_id = %d and n_entry_id = %d and n_from_blog = %d and n_from_entry = %d",
				in_blog, in_entry, in_link_blog, in_link_entry);
			if (Db_ExecSql(db, cha_sql)) {
				free(chp_str);
				Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				return CO_ERROR;
			}
			sprintf(cha_sql,
				"insert into at_recvlink "
					"(n_blog_id,n_entry_id,n_link_id,n_from_blog,n_from_entry) "
				"values "
					"(%d, %d, %d, %d, %d)",
					in_blog, in_entry, in_link, in_link_blog, in_link_entry);
			if (Db_ExecSql(db, cha_sql)) {
				if (chp_title)
					free(chp_title);
				Put_Nlist(nlp_out, "ERROR", "������ƥ����꡼�˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
				return CO_ERROR;
			}
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	trackback()
 * Description:
 * 	�ȥ�å��Хå�ping�����Ф�
%* ------------------------------------------------------------------------
 * Return:
 *		���ｪλ 0
 *		���顼 0�ʳ�
-* ------------------------------------------------------------------------*/
int trackback(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,int in_new_entry_id	/* ����ȥ�ID */
	,char *chp_tburl
	,int in_blog
	,int in_tbid
	,char *chp_key
)
{
	CP_NKF nkf;
	NKF_CODE cd;
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_cmd;
	char *chp_escape;
	char *chp_params;
	char *chp_params2;
	char *chp_result;
	char cha_protocol[10];
	char cha_title[128];
	char cha_str[2048];
	char cha_host[2048];
	char cha_sql[16384];
	int in_rt;
	int in_ins;
	int in_tb_blog;
	int in_tb_entry;

	chp_params = NULL;
	chp_params2 = NULL;
	chp_result = NULL;
	memset(cha_host, '\0', sizeof(cha_host));
	memset(cha_str, '\0', sizeof(cha_str));
	strcpy(cha_str, chp_tburl);
	if (strncmp(cha_str, "http://", strlen("http://")) == 0){
		sscanf(cha_str, "http://%[^/]%*s", cha_host);
		strcpy(cha_protocol, "http://");
	} else if (strncmp(cha_str, "https://", strlen("https://")) == 0){
		sscanf(cha_str, "https://%[^/]%*s", cha_host);
		strcpy(cha_protocol, "https://");
	} else {
		/* ���⤷�ʤ� */
		return 0;
	}
	if (cha_host[0] == '\0') {
		/* ���⤷�ʤ� */
		return 0;
	}
	chp_tmp = getenv("SERVER_NAME");
	if (chp_tmp) {
		strcpy(cha_str, chp_tmp);
	} else {
		gethostname(cha_str, sizeof(cha_str));
	}

	if (g_in_dbb_mode) {
		in_tb_blog = 0;
		in_tb_entry = 0;
		if (strcasecmp(cha_host, g_cha_host_name) != 0) {
			Put_Nlist(nlp_out, "ERROR", "���ꤵ�줿URL��DBB�֥���URL�ǤϤ���ޤ���<br>");
			return CO_ERROR;
		}
		sprintf(cha_str, "%s%s%s/%s/%%d-%%d", cha_protocol, cha_host, g_cha_user_cgi, CO_CGI_TB);
		if (sscanf(chp_tburl, cha_str, &in_tb_blog, &in_tb_entry) != 2) {
			Put_Nlist(nlp_out, "ERROR", "���ꤵ�줿URL��DBB�֥���URL�ǤϤ���ޤ���<br>");
			return CO_ERROR;
		}
		Blog_To_Temp(db, nlp_out, in_tb_blog, cha_sql);
		sprintf(cha_str, "%s%s%s/%s?eid=%d&bid=%s", cha_protocol, cha_host, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_tb_entry, cha_sql);
		if (insert_link(db, nlp_out, in_blog, in_new_entry_id, cha_str, 1, in_tb_blog, in_tb_entry)) {
			return CO_ERROR;
		}
	} else if (g_in_hb_mode) {
		in_tb_blog = 0;
		in_tb_entry = 0;
		sprintf(cha_str, "%s%s%s/%s/%%d-%%d", cha_protocol, cha_host, g_cha_user_cgi, CO_CGI_TB);
		if (sscanf(chp_tburl, cha_str, &in_tb_blog, &in_tb_entry) == 2) {
			sprintf(cha_str, "%s%s%s/%s?eid=%d&bid=%d", cha_protocol, cha_host, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_tb_entry, in_tb_blog);
			if (insert_link(db, nlp_out, in_blog, in_new_entry_id, cha_str, 1, in_tb_blog, in_tb_entry)) {
				return CO_ERROR;
			}
		}
	}

	cha_title[0] = '\0';
	sprintf(cha_sql, "select c_blog_title from at_blog where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		strcpy(cha_title, chp_tmp);
	}
	Db_CloseDyna(dbres);

	if (!in_tbid) {
		in_ins = 1;
		sprintf(cha_sql, "select coalesce(max(T1.n_tb_id), 0) + 1 from at_sendtb T1 where T1.n_blog_id=%d and T1.n_entry_id=%d for update", in_blog, in_new_entry_id);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "���ѵ���ID�����Υ�����˼��Ԥ��ޤ�����<br>");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå�ID�����Υ�����˼��Ԥ��ޤ�����<br>");
			}
			Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_tbid = atoi(chp_tmp);
		} else {
			in_tbid = 1;
		}
		Db_CloseDyna(dbres);
		sprintf(cha_sql,
			"insert into at_sendtb "
				"(n_blog_id,n_entry_id,n_tb_id,c_tb_url,d_tb_create_time,c_tb_title,c_tb_excerpt,b_success) "
			"values "
				"(%d, %d, %d, '%s', now()",
			in_blog, in_new_entry_id, in_tbid, chp_tburl);
	} else {
		in_ins = 0;
		strcpy(cha_sql, "update at_sendtb set d_tb_create_time=now()");
	}

	if (g_in_dbb_mode) {
		sprintf(cha_str, "%s%s%s/%s/?eid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_new_entry_id);
	} else if (g_in_short_name) {
		sprintf(cha_str, "%s%s%s/%08d/?eid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_new_entry_id);
	} else {
		sprintf(cha_str, "%s%s%s/%s?eid=%d&bid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_new_entry_id, in_blog);
	}
	Add_PostValue(&chp_params, "url", cha_str);
	chp_tmp = Get_Nlist(nlp_in, "entry_title", 1);
	if (chp_tmp) {
		chp_escape = Remove_HtmlTags(chp_tmp);
		Add_PostValue(&chp_params, "title", chp_escape);
		free(chp_escape);
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		if (in_ins) {
			sprintf(cha_sql + strlen(cha_sql), ", '%s'", chp_escape);
		} else {
			sprintf(cha_sql + strlen(cha_sql), ", c_tb_title='%s'", chp_escape);
		}
		free(chp_escape);
	} else {
		if (in_ins) {
			strcat(cha_sql, ", ''");
		} else {
			strcat(cha_sql, ", c_tb_title=''");
		}
	}
/*
	chp_tmp = Get_Nlist(nlp_in, "blog_name", 1);
	if (chp_tmp) {
		chp_escape = Remove_HtmlTags(chp_tmp);
		Add_PostValue(&chp_params, "blog_name", chp_escape);
		free(chp_escape);
	}
*/
	if (cha_title[0]) {
		chp_escape = Remove_HtmlTags(cha_title);
		Add_PostValue(&chp_params, "blog_name", chp_escape);
		free(chp_escape);
	}
	chp_tmp = Get_Nlist(nlp_in, "entry_summary", 1);
	if (chp_tmp && *chp_tmp) {
		chp_escape = Remove_HtmlTags(chp_tmp);
		Add_PostValue(&chp_params, "excerpt", chp_escape);
		free(chp_escape);
		chp_escape = My_Escape_SqlString(db, chp_tmp);
		if (in_ins) {
			sprintf(cha_sql + strlen(cha_sql), ", '%s'", chp_escape);
		} else {
			sprintf(cha_sql + strlen(cha_sql), ", c_tb_excerpt='%s'", chp_escape);
		}
		free(chp_escape);
	} else {
		chp_tmp = Get_Nlist(nlp_in, "entry_body", 1);
		if (chp_tmp && *chp_tmp) {
			chp_tmp2 = Chop_String(chp_tmp, CO_MAXLEN_ENTRY_SUMMARY);
			chp_escape = Remove_HtmlTags(chp_tmp2);
			Add_PostValue(&chp_params, "excerpt", chp_escape);
			free(chp_escape);
			chp_escape = My_Escape_SqlString(db, chp_tmp2);
			if (in_ins) {
				sprintf(cha_sql + strlen(cha_sql), ", '%s'", chp_escape);
			} else {
				sprintf(cha_sql + strlen(cha_sql), ", c_tb_excerpt='%s'", chp_escape);
			}
			free(chp_escape);
			free(chp_tmp2);
		} else {
			if (in_ins) {
				strcat(cha_sql, ", ''");
			} else {
				strcat(cha_sql, ", c_tb_excerpt=''");
			}
		}
	}

	nkf = Create_Nkf();
	Set_Input_String(nkf, chp_params);
	Set_Input_Code(nkf, NKF_EUC);
	Set_Output_Code(nkf, NKF_UTF8);
	cd = Convert_To_String(nkf, &chp_tmp);
	Destroy_Nkf_Engine(nkf);
	chp_params2 = Make_DigestParams(CO_SECURE_CGI_ID, 60, chp_tmp);
	free(chp_tmp);

	strcpy(cha_str, cha_host);
	strcat(cha_str, "/");
	chp_tmp = strstr(chp_tburl, cha_str);
	if (chp_tmp) {
		chp_tmp += strlen(cha_host);
	} else {
		chp_tmp = chp_tburl;
	}
	chp_cmd = NULL;
	if (g_in_dbb_mode && chp_key) {
		asprintf(&chp_cmd, "%s-%s", chp_tmp, chp_key);
	}
	in_rt = my_Http_Post_Get_Result_for_trackback(cha_host, chp_cmd ? chp_cmd : chp_tmp, chp_params2, &chp_result);
	if (chp_cmd) {
		free(chp_cmd);
	}
	if (in_rt) {
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "ERROR", "�ۥ��ȡ�%s�פؤε����������������˼��Ԥ��ޤ�����<br>", cha_host);
		} else {
			Put_Format_Nlist(nlp_out, "ERROR", "�ۥ��ȡ�%s�פؤΥȥ�å��Хå������˼��Ԥ��ޤ�����<br>", cha_host);
		}
	}
	free(chp_params2);
	free(chp_params);
//	if (g_in_dbb_mode) {
//		fprintf(stderr, "%s\n", chp_result);
//	}
	free(chp_result);

	if (!in_rt) {
		if (in_ins) {
			sprintf(cha_sql + strlen(cha_sql), ", %d)", (in_rt == 0));
		} else {
			sprintf(cha_sql + strlen(cha_sql), ", b_success = %d where n_blog_id = %d and n_entry_id = %d and n_tb_id = %d", (in_rt == 0), in_blog, in_new_entry_id, in_tbid);
		}
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "������������������̹����Υ�����˼��Ԥ��ޤ�����<br>");
			} else {
				Put_Nlist(nlp_out, "ERROR", "�ȥ�å��Хå�������̹����Υ�����˼��Ԥ��ޤ�����<br>");
			}
			Put_Format_Nlist(nlp_out, "ERROR", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			++in_rt;
		}
	}

	return in_rt;
}

/*
+* ------------------------------------------------------------------------
 * Function:            trackback_ip_check()
 * Description:
 *      �ȥ�å��Хå����Ƥ���桼������IP��Ĵ�١�Ϣ³�Ǥ��Ƥ��ʤ��������
 *      �ػߥۥ��Ȥ˴ޤޤ�Ƥ��ʤ�����Ĵ�٤롣
 *	�����ȤȰۤʤꡢ����λ�����(CO_TRACKBACK_INTERVAL)��
 *	����β��(CO_TRACKBACK_COUNT)�ʾ�Υȥ�å��Хå������ä����ϵ��ݡ�
%* ------------------------------------------------------------------------
 * Return:
 *      ���� 0
 *      ���� 1 (�ػ�IP�Τ���)
 *           2 (Ϣ³��ƤΤ���)
 *      ���顼�� CO_ERROR
-* ------------------------------------------------------------------------*/
int trackback_ip_check(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog, int in_login_owner)
{
#define CO_TRACKBACK_INTERVAL 60
#define CO_TRACKBACK_COUNT 2
	DBRes *dbres;
	char *chp_tmp;
	char *chp_ip;
	char cha_sql[1024];
	char cha_host[512];
	int in_denial_flg;

	chp_ip = getenv("REMOTE_ADDR");
	if(!chp_ip || !*chp_ip) {
		Put_Nlist(nlp_out, "MSG", "Illegal access.");
		return CO_ERROR;
	}
	memset(cha_host, '\0', sizeof(cha_host));
	/* �ۥ��Ȥϼ��ʤ��Ƥ�Ȥꤢ�������ˤ��ʤ� */
	Get_Hostbyaddr(chp_ip, cha_host);
	/* �ػ�IP�˻��ꤵ��Ƥ��뤫? */
	strcpy(cha_sql, "select T1.c_host");     /* 0 �ȥ�å��Хå��ػߥۥ��� */
	strcat(cha_sql, " from at_denytrackback T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "MSG", "Query failed to get denial hosts.");
		Put_Nlist(nlp_out, "MSG", Gcha_last_error);
		fprintf(stderr, "Query failed to get denial hosts.\n%s(%d)\n", Gcha_last_error, __LINE__);
		return CO_ERROR;
	}
	in_denial_flg = 0;
	while (Db_FetchNext(dbres) == CO_SQL_OK) {
		chp_tmp = Db_FetchValue(dbres, 0);
		if(chp_tmp && *chp_tmp) {
			if (g_in_need_login) {
				if (atoi(chp_tmp) == in_login_owner) {
					in_denial_flg = 1;
					break;
				}
			} else {
				if (strcmp(chp_ip, chp_tmp) == 0 || strstr(cha_host, chp_tmp)) {
					in_denial_flg = 1;
					break;
				}
			}
		}
	}
	Db_CloseDyna(dbres);
	if(in_denial_flg) {
		Put_Nlist(nlp_out, "MSG", "Your request is refused. Please inquire to administrator.");
		fprintf(stderr, "Your request is refused. Please inquire to administrator.(%d)\n", __LINE__);
		return 1;
	}
	/* Ϣ³�ȥ�å��Хå������å� */
	/* ����ε����Ρ������IP����Υȥ�å��Хå������롣 */
	/* �������ۤ�����CO_TRACKBACK_COUNT���ܤκ������֤��� */
	/* CO_TRACKBACK_INTERVAL�ð���ˤ��ä���硢���ݡ� */
	strcpy(cha_sql, "select max(UNIX_TIMESTAMP(T1.d_tb_create_time))");  /* 0 �����Ⱥ������� */
	strcat(cha_sql, " from at_trackback T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.c_tb_ip = '%s'", chp_ip);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "MSG", "Failed query to get information of recent entry. ");
		Put_Nlist(nlp_out, "MSG", Gcha_last_error);
		fprintf(stderr, "Failed query to get information of recent entry.\n%s(%d)\n", Gcha_last_error, __LINE__);
		return CO_ERROR;
	}
	if(Db_GetRowCount(dbres)) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(chp_tmp) {
			/* ǯ�ȷ��Ĵ������mktime�ǰ쵤���ä�ľ�� */
			time_t in_time = (time_t)atol(chp_tmp);
			if(time(NULL) - in_time <= (time_t)CO_TRACKBACK_INTERVAL) {
				in_denial_flg = 1;
			}
		}
	}
	Db_CloseDyna(dbres);
	if(in_denial_flg) {
		Put_Format_Nlist(nlp_out, "MSG", "You can't send only %d trackbacks in %d seconds. Please try later.", CO_TRACKBACK_COUNT, CO_TRACKBACK_INTERVAL);
		fprintf(stderr, "You can't send only %d trackbacks in %d seconds. Please try later.(%d)\n", CO_TRACKBACK_COUNT, CO_TRACKBACK_INTERVAL, __LINE__);
		return 2;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		comment_owner_check()
 * Description:
 *	�����Ȥ��Ƥ���桼�����Υ����ʡ�ID��Ĵ�١�Ϣ³��Ƥ��Ƥ��ʤ��������
 *	�ػߥ����ʡ��˴ޤޤ�Ƥ��ʤ�����Ĵ�٤롣
%* ------------------------------------------------------------------------
 * Return:
 *	���� 0
 *	���� 1 (�ػ�IP�Τ���)
 *	     2 (Ϣ³��ƤΤ���)
 *	���顼�� CO_ERROR
-* ------------------------------------------------------------------------*/
int comment_owner_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[1024];
	char cha_num[32];
	char *chp_eid;
	char *chp_tmp;
	char *chp_author_name;
	int in_denial_flg;
	int i;

	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if(!chp_eid || !*chp_eid) {
		chp_eid = "0";
	}
	chp_author_name = Get_Nlist(nlp_in, "comment_author_name", 1);
	if(!chp_author_name) {
		Put_Nlist(nlp_out, "ERROR", "̾�������Ϥ��Ƥ���������<br>");
		return CO_ERROR;
	}
	chp_tmp = strchr(chp_author_name, ':');
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����������󤵤�Ƥ��ޤ���<br>");
		return CO_ERROR;
	}
	if (chp_tmp - chp_author_name >= sizeof(cha_num)) {
		Put_Nlist(nlp_out, "ERROR", "�����������󤵤�Ƥ��ޤ���<br>");
		return CO_ERROR;
	}
	strncpy(cha_num, chp_author_name, chp_tmp - chp_author_name);
	cha_num[chp_tmp - chp_author_name] = '\0';

	strcpy(cha_sql, "select T1.c_host");	/* 0 �����ȶػߥۥ��� */
	strcat(cha_sql, " from at_denycomment T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ػ�IP�ꥹ�Ȥμ����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	in_denial_flg = 0;
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		/* �ػߥۥ��Ȥϲ��Զ��ڤ�ǳ�Ǽ����Ƥ��롣��Ĥ�����٤ư��פ����Τ�����е��ݡ� */
		chp_tmp = Db_GetValue(dbres, i, 0);
		if(chp_tmp && *chp_tmp && strcmp(cha_num, chp_tmp) == 0) {
			in_denial_flg = 1;
			break;
		}
	}
	Db_CloseDyna(dbres);
	if(in_denial_flg) {
		Put_Nlist(nlp_out, "ERROR", "�����Ȥ����ݤ���ޤ����������Ԥˤ��䤤��碌����������");
		return 1;
	}
	/* Ϣ³��ƥ����å� */
	/* ���뵭��ID�κǶ�Υ����Ȥλ��֤����롣 */
	strcpy(cha_sql, "select max(UNIX_TIMESTAMP(T1.d_comment_create_time))");	/* 0 �����Ⱥ������� */
	strcat(cha_sql, " from at_comment T1");
	strcat(cha_sql, " where T1.c_comment_ip = '");
	strcat(cha_sql, cha_num);
	strcat(cha_sql, "' and T1.n_entry_id = ");
	strcat(cha_sql, chp_eid);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ǿ���Ƥξ���μ����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	if(Db_GetRowCount(dbres)) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(chp_tmp) {
			/* ǯ�ȷ��Ĵ������mktime�ǰ쵤���ä�ľ�� */
			time_t in_time = (time_t)atol(chp_tmp);
			if(time(NULL) - in_time <= (time_t)CO_COMMENT_INTERVAL) {
				in_denial_flg = 1;
			}
		}
	}
	Db_CloseDyna(dbres);
	if(in_denial_flg) {
		Put_Nlist(nlp_out, "ERROR", "Ϣ³��ƤϤǤ��ޤ���<br>���Ф餯�ԤäƤ�����ľ���Ƥ���������");
		return 2;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		comment_ip_check()
 * Description:
 *	�����Ȥ��Ƥ���桼������IP��Ĵ�١�Ϣ³��Ƥ��Ƥ��ʤ��������
 *	�ػߥۥ��Ȥ˴ޤޤ�Ƥ��ʤ�����Ĵ�٤롣
%* ------------------------------------------------------------------------
 * Return:
 *	���� 0
 *	���� 1 (�ػ�IP�Τ���)
 *	     2 (Ϣ³��ƤΤ���)
 *	���顼�� CO_ERROR
-* ------------------------------------------------------------------------*/
int comment_ip_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_eid;
	char *chp_ip;
	char cha_sql[1024];
	char cha_host[512];
	int in_denial_flg;
	int i;

	chp_ip = getenv("REMOTE_ADDR");
	if(!chp_ip || !*chp_ip) {
		Put_Nlist(nlp_out, "ERROR", "�������������Ǥ���");
		return CO_ERROR;
	}
	if (Check_RBL()) {
		Put_Nlist(nlp_out, "ERROR", "�����Ȥ����ݤ���ޤ����������Ԥˤ��䤤��碌����������");
		return 1;
	}
	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if(!chp_eid || !*chp_eid) {
		chp_eid = "0";
	}
	memset(cha_host, '\0', sizeof(cha_host));
	/* �ۥ��Ȥϼ��ʤ��Ƥ�Ȥꤢ�������ˤ��ʤ� */
	Get_Hostbyaddr(chp_ip, cha_host);
	/* �ػ�IP�˻��ꤵ��Ƥ��뤫? */
	strcpy(cha_sql, "select T1.c_host");	/* 0 �����ȶػߥۥ��� */
	strcat(cha_sql, " from at_denycomment T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ػ�IP�ꥹ�Ȥμ����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	in_denial_flg = 0;
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		/* �ػߥۥ��Ȥϲ��Զ��ڤ�ǳ�Ǽ����Ƥ��롣��Ĥ�����٤ư��פ����Τ�����е��ݡ� */
		chp_tmp = Db_GetValue(dbres, i, 0);
		if(chp_tmp && *chp_tmp && (strcmp(chp_ip, chp_tmp) == 0 || strstr(cha_host, chp_tmp))) {
			in_denial_flg = 1;
			break;
		}
	}
	Db_CloseDyna(dbres);
	if(in_denial_flg) {
		Put_Nlist(nlp_out, "ERROR", "�����Ȥ����ݤ���ޤ����������Ԥˤ��䤤��碌����������");
		return 1;
	}
	/* Ϣ³��ƥ����å� */
	/* ���뵭��ID�κǶ�Υ����Ȥλ��֤����롣 */
	strcpy(cha_sql, "select max(UNIX_TIMESTAMP(T1.d_comment_create_time))");	/* 0 �����Ⱥ������� */
	strcat(cha_sql, " from at_comment T1");
	strcat(cha_sql, " where T1.c_comment_ip = '");
	strcat(cha_sql, chp_ip);
	strcat(cha_sql, "' and T1.n_entry_id = ");
	strcat(cha_sql, chp_eid);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ǿ���Ƥξ���μ����˼��Ԥ��ޤ�����<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	if(Db_GetRowCount(dbres)) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(chp_tmp) {
			/* ǯ�ȷ��Ĵ������mktime�ǰ쵤���ä�ľ�� */
			time_t in_time = (time_t)atol(chp_tmp);
			if(time(NULL) - in_time <= (time_t)CO_COMMENT_INTERVAL) {
				in_denial_flg = 1;
			}
		}
	}
	Db_CloseDyna(dbres);
	if(in_denial_flg) {
		Put_Nlist(nlp_out, "ERROR", "Ϣ³��ƤϤǤ��ޤ���<br>���Ф餯�ԤäƤ�����ľ���Ƥ���������");
		return 2;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_count_article_by_day()
 * Description:		������ε����ο��������
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ �����η��
 *	�����          ���顼�� CO_ERROR
-* ------------------------------------------------------------------------*/
int get_count_article_by_day(
	 DBase *db
	,char *chp_day_top	/* ���� yyyymmdd */
	,char *chp_day_end	/* ��λ yyyymmdd */
	,int in_blog
)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_count;

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	strcat(cha_sql, " and T1.d_entry_create_time < now()");
	sprintf(&cha_sql[strlen(cha_sql)], " and T1.d_entry_create_time >= '%s000000'", chp_day_top);	/* ������ */
	sprintf(&cha_sql[strlen(cha_sql)], " and T1.d_entry_create_time <= '%s235900'", chp_day_end);	/* ��λ�� */
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if(!chp_tmp){
		in_count = CO_ERROR;
	} else {
		in_count = atoi(chp_tmp);
	}
	Db_CloseDyna(dbres);
	return in_count;
}

/*
+* ------------------------------------------------------------------------
 * Function:            update_visitor_list()
 * Description:
 *      ˬ��ԥꥹ�Ȥ򹹿�����
%* ------------------------------------------------------------------------
 * Return:                      ���ｪλ 0
 *      �����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int update_visitor_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
#define MAX_VISIT 30
	DBase *dbb_db;
	DBRes *dbres;
	char cha_sql[4096];
	char cha_nickname[255];
	char cha_mailaddr[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char *chp_tmp;
	int in_visit;
	int in_count;

	if (in_login_blog <= 0 || in_login_blog == in_blog) {
		return 0;
	}
	sprintf(cha_sql,
		"select T1.c_host,"
		" T1.c_db,"
		" T1.c_user,"
		" T1.c_password"
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "������˼��Ԥ��ޤ�����(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "���Х��μ����˼��Ԥ��ޤ�����<br>");
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	Db_CloseDyna(dbres);
	if (g_in_ssl_mode) {
		dbb_db = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		dbb_db = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!dbb_db){
		Put_Format_Nlist(nlp_out, "ERROR", "�ǡ����١�������³�˼��Ԥ��ޤ�����(%s:%d)<br>", __FILE__, __LINE__);
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>", Gcha_last_error);
		return 1;
	}
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select T1.b_stealth");	/* 0 */
	strcat(cha_sql, " from dt_owner T1");
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(dbb_db, cha_sql);
	if(!dbres) {
		Db_Disconnect(dbb_db);
		return 1;
	}
	if(atoi(Db_GetValue(dbres, 0, 0)) == 1) {
		Db_CloseDyna(dbres);
		Db_Disconnect(dbb_db);
		return 0;
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(dbb_db);

	if(Begin_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "�ȥ�󥶥�����󳫻Ϥ˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		fprintf(stderr, "%d: %s\n", __LINE__, Gcha_last_error);
		return 1;
	}
	if (!g_in_admin_mode) {
		if (g_in_dbb_mode || g_in_hb_mode) {
			cha_mailaddr[0] = '\0';
			in_visit = Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_nickname);
//			in_visit = Get_Nickname(db, nlp_out, cha_nickname, cha_mailaddr, NULL);
			if (!in_visit) {
				fprintf(stderr, "%d: skip updating visitor list (%d - %d - %d)\n", __LINE__, in_blog, in_login_blog, in_visit);
				return g_in_need_login;
			}
			sprintf(cha_sql, "delete from at_visitor where n_blog_id = %d and n_owner_id = %d and d_visit_time >= CURDATE() and d_visit_time < DATE_ADD(CURDATE(), INTERVAL 1 DAY)", in_blog, in_visit);
			if(Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
				fprintf(stderr, "%d: %s\n", __LINE__, Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
			sprintf(cha_sql,
				"insert into at_visitor "
					"(n_blog_id,n_visitor_id,n_owner_id,c_nickname,c_mailaddr) "
				"values "
					"(%d, %d, %d, '%s', '%s')",
				in_blog, in_login_blog, in_visit, cha_nickname, cha_mailaddr);
			if(Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
				fprintf(stderr, "%d: %s\n", __LINE__, Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		} else {
			sprintf(cha_sql, "delete from at_visitor where n_blog_id = %d and n_visitor_id = %d and d_visit_time >= CURDATE() and d_visit_time < DATE_ADD(CURDATE(), INTERVAL 1 DAY)", in_blog, in_login_blog);
			if(Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
				fprintf(stderr, "%d: %s\n", __LINE__, Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
			sprintf(cha_sql,
				"insert into at_visitor "
					"(n_blog_id,n_visitor_id,c_nickname,c_mailaddr) "
				"select "
					"%d,%d,c_author_nickname,c_author_mail "
				"from "
					"at_profile "
				"where "
					"n_blog_id=%d",
				in_blog, in_login_blog, in_login_blog);
			if(Db_ExecSql(db, cha_sql)) {
				Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
				fprintf(stderr, "%d: %s\n", __LINE__, Gcha_last_error);
				Rollback_Transact(db);
				return 1;
			}
		}
	}
	in_count = 0;
	sprintf(cha_sql, "select count(*) from at_visitor where n_blog_id = %d;", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (dbres) {
		if (Db_FetchNext(dbres) == CO_SQL_OK) {
			chp_tmp = Db_FetchValue(dbres, 0);
			if (chp_tmp) {
				in_count = atoi(chp_tmp);
			}
		}
		Db_CloseDyna(dbres);
	}
	if (in_count > MAX_VISIT) {
		sprintf(cha_sql, "delete from at_visitor where n_blog_id = %d order by d_visit_time limit %d", in_blog, in_count - MAX_VISIT);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			fprintf(stderr, "%d: %s\n", __LINE__, Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}
	if(Commit_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "���ߥåȤ˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		fprintf(stderr, "%d: %s\n", __LINE__, Gcha_last_error);
		Rollback_Transact(db);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            uupdate_access_conter()
 * Description:
 *      �������������󥿤򹹿�����
%* ------------------------------------------------------------------------
 * Return:                      ���ｪλ 0
 *      �����          ���顼�� 1
-* ------------------------------------------------------------------------*/
void update_access_conter(DBase *db, int in_blog, int in_login_blog)
{
#define REFER_INTERVAL (24 * 60)
	DBase *dbb_db;
	char *chp_ip;
	char *chp_ua;
	char *chp_tmp;
	DBRes *dbres;
	char cha_sql[4096];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	int b_vld;
	int in_cnt;

	chp_ip = getenv("REMOTE_ADDR");
	chp_ua = getenv("HTTP_USER_AGENT");
	if (!chp_ip || !chp_ua) {
		return;
	}
	sprintf(cha_sql,
		"select T1.c_host,"
		" T1.c_db,"
		" T1.c_user,"
		" T1.c_password"
		" from sy_authinfo T1");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		return;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		return;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "localhost");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "dbb_blog");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "dbb_blog");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "dbb_blog");
	Db_CloseDyna(dbres);
	if (g_in_ssl_mode) {
		dbb_db = Db_ConnectSSL(cha_host, cha_db, cha_user, cha_password);
	} else {
		dbb_db = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	}
	if (!dbb_db){
		return;
	}
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select T1.b_stealth");	/* 0 */
	strcat(cha_sql, " from dt_owner T1");
	sprintf(&cha_sql[strlen(cha_sql)], " where T1.n_blog_id = %d", in_login_blog);
	dbres = Db_OpenDyna(dbb_db, cha_sql);
	if(!dbres) {
		Db_Disconnect(dbb_db);
		return;
	}
	if(atoi(Db_GetValue(dbres, 0, 0)) == 1) {
		Db_CloseDyna(dbres);
		Db_Disconnect(dbb_db);
		return;
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(dbb_db);

	if(Begin_Transact(db)) {
		fprintf(stderr, "%s(%d): Begin_Transact failed(%s)\n", __FILE__, __LINE__, Gcha_last_error);
		return;
	}
	b_vld = 1;
	in_cnt = 0;
	sprintf(cha_sql,
		"select "
			"UNIX_TIMESTAMP(d_view) "
		"from "
			"at_view "
		"where "
			"n_blog_id=%d and "
			"c_ip_addr='%s' "
		"order by "
			"d_view desc "
		"limit 1;",
		in_blog, chp_ip);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		fprintf(stderr, "%s(%d): Db_OpenDyna failed(%s)\n", __FILE__, __LINE__, Gcha_last_error);
		Rollback_Transact(db);
		return;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_cnt = atoi(chp_tmp);
		if (in_cnt + REFER_INTERVAL * 60 >= time(NULL))
			b_vld = 0;
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		"insert into at_view "
			"(n_blog_id,c_ip_addr,c_user_agent,b_valid) "
		"values "
			"(%d, '%s', '%s', %d);",
		in_blog, chp_ip, chp_ua, b_vld);
	if (Db_ExecSql(db, cha_sql)) {
		fprintf(stderr, "%s(%d): Db_ExecSql failed(%s)\n", __FILE__, __LINE__, Gcha_last_error);
		Rollback_Transact(db);
	} else {
		Commit_Transact(db);
	}
}
