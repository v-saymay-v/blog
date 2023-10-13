/*
+* ------------------------------------------------------------------------
 * Module-Name:		tb.c
 * First-Created:	2004/08/20 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	トラックバック受信
-* ------------------------------------------------------------------------
 * Change-Log:
 *
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: seoreport.c,v 1.127 2011/10/04 05:32:09 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <netdb.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include "libcommon2.h"
#include "libnet.h"
#include "libnkf.h"
#include "libmysql.h"

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

typedef struct _SEARCHDATA {
	char cha_keyword[64];
	char cha_engine[32];
	int in_hit;
	int in_last;
} SEARCHDATA;

typedef struct _TABLES {
	char *chp_table;
	char *chp_sql;
} TABLES;

static TABLES g_sta_tables[] = {
	{
		"rt_seo_report",
		"create table rt_seo_report("
		"	d_report datetime,"
		"	c_keyword varchar(64),"
		"	n_engine integer,"
		"	c_engine varchar(100),"
		"	n_order integer,"
		"	n_number integer,"
		"	n_point integer,"
		"	c_rank varchar(8),"
		"	constraint seo_pkey primary key (d_report,c_keyword,c_engine)"
		") TYPE = InnoDB;"
	},
	{
		"rt_year_sales_report",
		"create table rt_year_sales_report("
		"	d_report datetime,"
		"	n_amount integer,"
		"	n_count integer,"
		"	n_unit_price integer,"
		"	n_amount_per_day integer,"
		"	n_count_per_day float,"
		"	constraint year_sales_pkey primary key (d_report)"
		") TYPE = InnoDB;"
	},
	{
		"rt_sales_ranking_report",
		"create table rt_sales_ranking_report("
		"	d_report datetime,"
		"	c_name varchar(128),"
		"	c_item_id varchar(64),"
		"	n_unit_price integer,"
		"	n_order integer,"
		"	n_amount integer,"
		"	n_percent float,"
		"	n_count integer,"
		"	n_last_order integer,"
		"	constraint sales_ranking_pkey primary key (d_report,n_order)"
		") TYPE = InnoDB;"
	},
	{
		"rt_top_sales_report",
		"create table rt_top_sales_report("
		"	n_order integer,"
		"	d_report datetime,"
		"	d_data datetime,"
		"	c_item_id varchar(64),"
		"	c_name varchar(128),"
		"	n_amount integer,"
		"	constraint top_sales_pkey primary key (d_report,d_data,c_item_id)"
		") TYPE = InnoDB;"
	},
	{
		"rt_top_daily_report",
		"create table rt_top_daily_report("
		"	n_order integer,"
		"	d_report datetime,"
		"	d_data datetime,"
		"	c_item_id varchar(64),"
		"	c_name varchar(128),"
		"	n_count integer,"
		"	constraint top_daily_pkey primary key (d_report,d_data,c_item_id)"
		") TYPE = InnoDB;"
	},
	{
		"rt_access_report",
		"create table rt_access_report("
		"	d_report datetime,"
		"	n_visit integer,"
		"	constraint access_pkey primary key (d_report)"
		") TYPE = InnoDB;"
	},
	{
		"rt_refer_report",
		"create table rt_refer_report("
		"	d_report datetime,"
		"	n_order integer,"
		"	c_url varchar(255),"
		"	n_visit integer,"
		"	constraint refer_pkey primary key (d_report,n_order)"
		") TYPE = InnoDB;"
	},
	{
		"rt_enter_report",
		"create table rt_enter_report("
		"	d_report datetime,"
		"	n_order integer,"
		"	c_url varchar(255),"
		"	n_visit integer,"
		"	constraint enter_pkey primary key (d_report,n_order)"
		") TYPE = InnoDB;"
	},
	{
		"rt_exit_report",
		"create table rt_exit_report("
		"	d_report datetime,"
		"	n_order integer,"
		"	c_url varchar(255),"
		"	n_visit integer,"
		"	constraint exit_pkey primary key (d_report,n_order)"
		") TYPE = InnoDB;"
	},
	{
		"rt_keyword_report",
		"create table rt_keyword_report("
		"	d_report datetime,"
		"	n_order integer,"
		"	c_keyword varchar(64),"
		"	c_engine varchar(32),"
		"	n_visit integer,"
		"	n_last integer,	"
		"	constraint exit_pkey primary key (d_report,n_order)"
		") TYPE = InnoDB;"
	}
};

static int g_in_current_year = 0;
static int g_in_current_month = 0;
static int g_in_current_day = 0;
static int g_in_search_year = 0;
static int g_in_search_month = 0;
static int g_in_search_day = 0;
static char g_cha_domain_ptn1[256] = {0};
static char g_cha_domain_ptn2[256] = {0};
static char g_cha_domain_ptn3[256] = {0};
static char g_cha_pass_dat[1024] = "/data/e-commex/pass.dat";
//static char g_cha_blog_ini[1024] = "/data/as-blog/dbb_blog.ini";
static char g_cha_vdomain_catalog[] = "/etc/vdomain.catalog";
static char g_cha_db_name[256] = "cart_report";
static char g_cha_db_pass[256] = "R2LQYT8XB5";
static char g_cha_db_user[256] = "root";
static char g_cha_db_host[256] = "localhost";
static char g_cha_domuser[256] = "";
static char g_cha_dompass[256] = "";
//static char g_cha_wget[] = "/usr/local/sbin/seowget -nv -q -U \"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 1.1.4322)\" ";
static char g_cha_wget[] = "/usr/local/bin/wget -nv -q -U \"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; .NET CLR 1.1.4322)\" ";

int Create_Table(DBase *db)
{
	DBRes *dbres;
	char *chp_sql;
	int i;

	for (i = 0; i < (int)(sizeof(g_sta_tables)/sizeof(TABLES)); ++i) {
		asprintf(&chp_sql, "show table status like '%s'", g_sta_tables[i].chp_table);
		dbres = Db_OpenDyna(db, chp_sql);
		if (!dbres) {
			syslog(LOG_NOTICE, "can not to exec query: %s(%s)", chp_sql, Gcha_last_error);
			free(chp_sql);
			return 1;
		}
		free(chp_sql);
		if (!Db_GetRowCount(dbres)) {
			if (Db_ExecSql(db, g_sta_tables[i].chp_sql) != CO_SQL_OK) {
				syslog(LOG_NOTICE, "can not to exec query: %s(%s)", chp_sql, Gcha_last_error);
				free(chp_sql);
				return 1;
			}
		}
		Db_CloseDyna(dbres);
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	Domain_To_User()
 * Description:
 *		-dで指定されたドメイン名をvdomain.catalogを使ってユーザー名に変換
%* ------------------------------------------------------------------------
 * Return:	正常終了 pszUserIDにユーザー名
 *			エラー時 pszUserIDに空文字
-* ------------------------------------------------------------------------*/
static void Domain_To_User(const char* pszDomain, char* pszUserID)
{
	FILE* fp;
	char szLine[1024];

	pszUserID[0] = '\0';
	fp = fopen(g_cha_vdomain_catalog, "r");
	if (!fp)
		return;

	while (fgets(szLine, sizeof(szLine), fp)) {
		char* pstr = strtok(szLine, ":");
		if (strcmp(pstr, pszDomain) == 0) {
			pstr = strtok(NULL, ":");
			if (pstr) {
				fclose(fp);
				strcpy(pszUserID, pstr);
				pstr = strchr(pszUserID, '\r');
				if (pstr)
					*pstr = '\0';
				pstr = strchr(pszUserID, '\n');
				if (pstr)
					*pstr = '\0';
				return;
			}
		}
	}
	fclose(fp);
}

/*
+* ------------------------------------------------------------------------
 * Function:		asj_click.ini読込
 * Include-file:
 * Description:
 *	アフィリエイトシステムの設定ファイルasj_click.iniを読み込み、
 *	グローバルデータに保存。
%* ------------------------------------------------------------------------
 * Return:
 *	戻り値 0:成功、0以外:失敗
-* ------------------------------------------------------------------------*/
static int read_pass_dat(const char* chp_user, char* chp_host, char* chp_db, char* chp_username, char* chp_password)
{
	struct passwd *pwd;
	FILE* fp;
	char cha_oneline[1024];

	pwd = getpwnam(chp_user);
	if (!pwd) {
		syslog(LOG_NOTICE, "user '%s' was not found.", chp_user);
		return 1;
	}
	strcpy(cha_oneline, pwd->pw_dir);
	strcat(cha_oneline, g_cha_pass_dat);
	fp = fopen(cha_oneline, "r");
	if (!fp) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		return errno;
	}

	strcpy(chp_host, g_cha_db_host);
	while (fgets(cha_oneline, sizeof(cha_oneline), fp)) {
		char* chp_name = strtok(cha_oneline, ":");
		if (chp_name) {
			char* chp_value = strtok(NULL, ":");
			if (chp_value) {
				char* chp_tmp;
				chp_tmp = strchr(chp_value, '\r');
				if (chp_tmp)
					*chp_tmp = '\0';
				chp_tmp = strchr(chp_value, '\n');
				if (chp_tmp)
					*chp_tmp = '\0';
				strcpy(chp_username, chp_name);
				strcpy(chp_password, chp_value);
				sprintf(chp_db, "pib_%s", chp_name);
			}
		}
	}
	fclose(fp);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		Conv_From_Euc()
 * Description:
 *		EUCコードを指定のコードに変換する
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static void Conv_From_Euc(const char *chp_src, const char *chp_code, char **chpp_dst)
{
	CP_NKF nkf;
	nkf = Create_Nkf();
	Set_Input_String(nkf, chp_src);
	Set_Input_Code(nkf, NKF_EUC);
	if (!chp_code) {
		Set_Output_Code(nkf, NKF_EUC);
	} else if (strcmp(chp_code, "UTF-8") == 0) {
		Set_Output_Code(nkf, NKF_UTF8);
	} else if (strcmp(chp_code, "Shift_JIS") == 0) {
		Set_Output_Code(nkf, NKF_SJIS);
	} else {
		Set_Output_Code(nkf, NKF_EUC);
	}
	Set_Assume_Hankaku_Kana(nkf, 1);
	Convert_To_String(nkf, chpp_dst);
	Destroy_Nkf_Engine(nkf);
}

/*
+* ------------------------------------------------------------------------
 * Function:		Conv_From_Sjis()
 * Description:
 *		Shift_JISコードをEUCに変換する
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static void Conv_From_Sjis(const char *chp_src, char **chpp_dst)
{
	CP_NKF nkf;
	nkf = Create_Nkf();
	Set_Input_String(nkf, chp_src);
	Set_Input_Code(nkf, NKF_SJIS);
	Set_Output_Code(nkf, NKF_EUC);
	Convert_To_String(nkf, chpp_dst);
	Destroy_Nkf_Engine(nkf);
}

/*
+* ========================================================================
 * Function:		Http_Post_Get_Result
 * Include-file:	libnet.h
 * Description:
 *	指定のホストに指定のhttpリクエストを送信し、戻りの文字列をバッファに返す
%* ========================================================================
 * Return:
 *	0: 正常終了
 *	1: エラー
-* ========================================================================*/
/*
int Http_Cmd_Get_With_Auth(
	const char* chp_request,
	const char* chp_host,
	const char* chp_path,
	const char* chp_params,
	const char* chp_login,
	const char* chp_password,
	const char* chp_cookie,
	const char* chp_referer,
	char** chpp_result)
{
	int err;
	int sd;
	int ret;
	char cha_buff[1025];
	char* chp_header;
	char* chp_last;
	char* chp_str;
	char* chp_auth;
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
	if (service){
		// 成功したらポート番号をコピー
		sa.sin_port = service->s_port;
	} else {
		// 失敗したら80番に決め打ち
		sa.sin_port = htons(80);
	}
	memcpy(&sa.sin_addr, he->h_addr_list[0], he->h_length);
	ret = connect(sd, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0){
		err = errno;
		close(sd);
		return err;
	}

	chp_str = (char*)malloc(strlen(chp_login) + strlen(":") + strlen(chp_password) + 1);
	sprintf(chp_str, "%s:%s", chp_login, chp_password);
	chp_auth = (char*)malloc(strlen(chp_str) * 2);
	Base64_Encode((const unsigned char *)chp_str, strlen(chp_str), (unsigned char *)chp_auth);

	if (chp_cookie && chp_cookie[0]) {
		asprintf(&chp_header,
			"%s %s HTTP/1.0\r\n"
			"%s"
			"Host: %s\r\n"
			"Referer: %s\r\n"
			"Authorization: Basic %s\r\n"
			"Content-Type: text/plain; charset=EUC-JP\r\n"
			"Content-length: %d\r\n\r\n"
			"%s\r\n\r\n",
			chp_request, chp_path, chp_cookie, chp_host, chp_referer, chp_auth, strlen(chp_params), chp_params);
	} else {
		asprintf(&chp_header,
			"%s %s HTTP/1.0\r\n"
			"Host: %s\r\n"
			"Referer: %s\r\n"
			"Authorization: Basic %s\r\n"
			"Content-Type: text/plain; charset=EUC-JP\r\n"
			"Content-length: %d\r\n\r\n"
			"%s\r\n\r\n",
			chp_request, chp_path, chp_host, chp_referer, chp_auth, strlen(chp_params), chp_params);
	}
	free(chp_auth);
	free(chp_str);

	ret = write(sd, chp_header, strlen(chp_header));
	if (ret < strlen(chp_header)){
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
*/

typedef struct _REFERDATA {
	char	cha_url[1024];
	int		in_visit;
} REFERDATA;

static void Build_Refer_Data(char **chpp_cur, const char *chp_mark, const char *chp_line, const char *chp_begin, const char *chp_end, const char *chp_html1, const char *chp_html2, int in_url, int in_visit, REFERDATA *stp_refdata)
{
	char cha_tmp[1024];
	char *chp_wrk;
	char *chp_tmp;
	char *chp_esc;
	char *chp_cur;
	int i, j;

	j = 0;
	chp_cur = *chpp_cur;
	chp_tmp = strstr(chp_cur, chp_mark);
	if (!chp_tmp)
		return;
	chp_cur = chp_tmp + strlen(chp_mark);
	while (1) {
		chp_tmp = strstr(chp_cur, chp_line);
		if (!chp_tmp)
			break;
		chp_cur = chp_tmp + strlen(chp_line);
		i = 0;
		while (1) {
			if (i == in_url) {
				chp_tmp = strstr(chp_cur, chp_html1);
				if (!chp_tmp)
					break;
				chp_cur = chp_tmp + strlen(chp_html1);
				chp_tmp = strstr(chp_cur, chp_html2);
				if (!chp_tmp)
					break;
				strncpy(cha_tmp, chp_cur, chp_tmp - chp_cur);
				cha_tmp[chp_tmp - chp_cur] = '\0';
				if (strncmp(cha_tmp, "<A HREF=\"", 9) == 0) {
					chp_wrk = cha_tmp + 9;
				} else {
					chp_wrk = cha_tmp;
				}
				chp_esc = strstr(chp_wrk, "\">");
				if (chp_esc) {
					*chp_esc = '\0';
				}
				strcpy(stp_refdata[j].cha_url, chp_wrk);
				chp_cur = chp_tmp + strlen(chp_html2);
			} else {
				chp_tmp = strstr(chp_cur, chp_begin);
				if (!chp_tmp)
					break;
				chp_cur = chp_tmp + strlen(chp_begin);
				chp_tmp = strstr(chp_cur, chp_end);
				if (!chp_tmp)
					break;
				strncpy(cha_tmp, chp_cur, chp_tmp - chp_cur);
				cha_tmp[chp_tmp - chp_cur] = '\0';
				if (i == in_visit) {
					chp_wrk = cha_tmp;
					while (*chp_wrk) {
						if (*chp_wrk < '0' || *chp_wrk > '9') {
							chp_esc = chp_wrk;
							while (*chp_esc) {
								*chp_esc = *(chp_esc + 1);
								++chp_esc;
							}
						} else {
							++chp_wrk;
						}
					}
					stp_refdata[j].in_visit = atoi(cha_tmp);
				}
			}
			chp_cur = chp_tmp + strlen(chp_end);
			++i;
			if (i > in_url)
				break;
		}
		++j;
		if (j >= 5)
			break;
	}
	*chpp_cur = chp_cur;
}

static void Build_Search_Data(char *chp_result, SEARCHDATA *stp_searchdata)
{
	char *chp_sql;
	char *chp_tmp;
	char *chp_buf;
	char *chp_dst;
	char *chpa_data[6];
	int in_cnt;
	int in_clms;

	in_cnt = 0;
	chp_tmp = strtok(chp_result, "\r\n");
	while (chp_tmp && in_cnt < 30) {
		chp_buf = (char*)malloc(strlen(chp_tmp) + 1);
		strcpy(chp_buf, chp_tmp);
		in_clms = Get_CsvList(chp_buf, ',', chpa_data, 6);
		if (in_clms == 6 && !Check_Numeric(chpa_data[4])) {
			stp_searchdata[in_cnt].in_hit = atoi(chpa_data[4]);
			Conv_From_Sjis(chpa_data[3], &chp_dst);
			chp_sql = Escape_SqlString(chp_dst);
			strcpy(stp_searchdata[in_cnt].cha_keyword, chp_sql);
			free(chp_sql);
			free(chp_dst);
			Conv_From_Sjis(chpa_data[2], &chp_dst);
			strcpy(stp_searchdata[in_cnt].cha_engine, chp_dst);
			free(chp_dst);
			stp_searchdata[in_cnt].in_last = 0;
			++in_cnt;
		}
		free(chp_buf);
		chp_tmp = strtok(NULL, "\r\n");
	}
}

static char * Exec_Wget(char *chp_cmd, char *chp_out)
{
	FILE *pp;
	char *chp_tmp;
	char *chp_result;
	char cha_cmd[1024];

	pp = popen(chp_cmd, "r");
	if (!pp) {
		syslog(LOG_NOTICE, "failed to exec wget(%s)", strerror(errno));
		return NULL;
	}
	while (fgets(cha_cmd, sizeof(cha_cmd), pp))
		;
	pclose(pp);
	pp = fopen(chp_out, "r");
	if (!pp) {
		syslog(LOG_NOTICE, "failed to open wget result(%s)", strerror(errno));
		return NULL;
	}
	chp_result = NULL;
	while (fgets(cha_cmd, sizeof(cha_cmd), pp)) {
		if (!chp_result) {
			chp_result = (char*)malloc(strlen(cha_cmd) + 1);
			strcpy(chp_result, cha_cmd);
		} else {
			chp_tmp = realloc(chp_result, strlen(chp_result) + strlen(cha_cmd) + 1);
			if (chp_tmp) {
				chp_result = chp_tmp;
				strcat(chp_result, cha_cmd);
			}
		}
	}
	fclose(pp);
	if (!chp_result) {
		syslog(LOG_NOTICE, "no wget result");
		return NULL;
	}
	return chp_result;
}

/*
+* ------------------------------------------------------------------------
 * Function:		Access_Report()
 * Description:
 *		アクセスログを解析しレポート作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int Access_Report(DBase *db, DBase *db_blog)
{
	typedef struct _ACCESSDATA {
		int	in_year;
		int	in_month;
		int	in_visit;
	} ACCESSDATA;
	ACCESSDATA sta_accessdata[12];
	REFERDATA sta_enterdata[5];
	REFERDATA sta_exitdata[5];
	REFERDATA sta_referdata[5];
	SEARCHDATA sta_searchthis[30];
	SEARCHDATA sta_searchlast[30];
	char cha_sql[1024];
	char cha_user[33];
	char cha_pass[33];
	char cha_url[256];
	char cha_out[256];
	char cha_cookie[256];
	char cha_data[256];
	char cha_refer[256];
	char cha_line[33];
	char cha_refer_line[33];
	char cha_refer_begin[33];
	char cha_refer_end[33];
	char cha_refer_html1[33];
	char cha_refer_html2[33];
	char cha_value_begin[33];
	char cha_value_end[33];
	char cha_enter_mark[33];
	char cha_exit_mark[33];
	char cha_refer_mark[33];
	char cha_tmp[1024];
	char cha_cmd[1024];
	char *chp_tmp;
	char *chp_cur;
	char *chp_esc;
	char *chp_result;
	int fd;
	int in_year;
	int in_month;
	int in_visit;
	int in_columns;
	int in_enter_visit;
	int in_enter_url;
	int in_exit_visit;
	int in_exit_url;
	int in_refer_visit;
	int in_refer_url;
	int i, j;
	time_t n_time;
	DBRes *dbres;

	dbres = Db_OpenDyna(db_blog,
		" select"
		" c_user"			//  0
		",c_pass"			//  1
		",c_line"			//  2
		",c_value_begin"	//  3
		",c_value_end"		//  4
		",n_month"			//  5
		",n_visit"			//  6
		",n_columns"		//  7
		",c_refer_line"		//  8
		",c_refer_begin"	//  9
		",c_refer_end"		// 10
		",c_refer_html1"	// 11
		",c_refer_html2"	// 12
		",c_enter_mark"		// 13
		",n_enter_visit"	// 14
		",n_enter_url"		// 15
		",c_exit_mark"		// 16
		",n_exit_visit"		// 17
		",n_exit_url"		// 18
		",c_refer_mark"		// 19
		",n_refer_visit"	// 20
		",n_refer_url"		// 21
		" from sy_auth_info");
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		syslog(LOG_NOTICE, "failed to get auth info.");
		Db_CloseDyna(dbres);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	strcpy(cha_user, chp_tmp ? chp_tmp : "");
	if (g_cha_domuser[0]) {
		strcpy(cha_user, g_cha_domuser);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	strcpy(cha_pass, chp_tmp ? chp_tmp : "");
	if (g_cha_dompass[0]) {
		strcpy(cha_pass, g_cha_dompass);
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	strcpy(cha_line, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 3);
	strcpy(cha_value_begin, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 4);
	strcpy(cha_value_end, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 5);
	in_month = chp_tmp ? atoi(chp_tmp) : 0;
	chp_tmp = Db_GetValue(dbres, 0, 6);
	in_visit = chp_tmp ? atoi(chp_tmp) : 7;
	chp_tmp = Db_GetValue(dbres, 0, 7);
	in_columns = chp_tmp ? atoi(chp_tmp) : 11;

	chp_tmp = Db_GetValue(dbres, 0, 8);
	strcpy(cha_refer_line, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 9);
	strcpy(cha_refer_begin, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 10);
	strcpy(cha_refer_end, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 11);
	strcpy(cha_refer_html1, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 12);
	strcpy(cha_refer_html2, chp_tmp ? chp_tmp : "");

	chp_tmp = Db_GetValue(dbres, 0, 13);
	strcpy(cha_enter_mark, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 14);
	in_enter_visit = chp_tmp ? atoi(chp_tmp) : 0;
	chp_tmp = Db_GetValue(dbres, 0, 15);
	in_enter_url = chp_tmp ? atoi(chp_tmp) : 0;

	chp_tmp = Db_GetValue(dbres, 0, 16);
	strcpy(cha_exit_mark, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 17);
	in_exit_visit = chp_tmp ? atoi(chp_tmp) : 0;
	chp_tmp = Db_GetValue(dbres, 0, 18);
	in_exit_url = chp_tmp ? atoi(chp_tmp) : 0;

	chp_tmp = Db_GetValue(dbres, 0, 19);
	strcpy(cha_refer_mark, chp_tmp ? chp_tmp : "");
	chp_tmp = Db_GetValue(dbres, 0, 20);
	in_refer_visit = chp_tmp ? atoi(chp_tmp) : 0;
	chp_tmp = Db_GetValue(dbres, 0, 21);
	in_refer_url = chp_tmp ? atoi(chp_tmp) : 0;
	Db_CloseDyna(dbres);

	memset(&sta_accessdata, 0, sizeof(sta_accessdata));
	memset(&sta_enterdata, 0, sizeof(sta_enterdata));
	memset(&sta_exitdata, 0, sizeof(sta_exitdata));
	memset(&sta_referdata, 0, sizeof(sta_referdata));
	memset(&sta_searchthis, 0, sizeof(sta_searchthis));
	memset(&sta_searchlast, 0, sizeof(sta_searchlast));

	sprintf(cha_refer, "http://%s/index.html", g_cha_domain_ptn3);

	n_time = time(NULL);
	sprintf(cha_out, "/var/tmp/out_%lld_XXXXX", (long long)n_time);
	fd = mkstemp(cha_out);
	close(fd);

	sprintf(cha_cookie, "/var/tmp/cookie_%lld_XXXXX", (long long)n_time);
	fd = mkstemp(cha_cookie);
	close(fd);

	sprintf(cha_data, "go_auth=go\\&id=%s\\&pass=%s", cha_user, cha_pass);
	sprintf(cha_url, "http://%s/web-setup/asusage/", g_cha_domain_ptn3);
	sprintf(cha_cmd, "%s -O %s --save-cookies %s --keep-session-cookies --referer=%s --post-data=%s %s",
			g_cha_wget, cha_out, cha_cookie, cha_refer, cha_data, cha_url);
	chp_result = Exec_Wget(cha_cmd, cha_out);
	if (!chp_result) {
		syslog(LOG_NOTICE, "failed to exec wget");
		remove(cha_out);
		remove(cha_cookie);
		return 1;
	}

	j = 0;
	chp_cur = chp_result;
	while (1) {
		chp_tmp = strstr(chp_cur, cha_line);
		if (!chp_tmp)
			break;
		chp_cur = chp_tmp + strlen(cha_line);
		for (i = 0; i < in_columns; ++i) {
			chp_tmp = strstr(chp_cur, cha_value_begin);
			if (!chp_tmp)
				break;
			chp_cur = chp_tmp + strlen(cha_value_begin);
			chp_tmp = strstr(chp_cur, cha_value_end);
			if (!chp_tmp)
				break;
			strncpy(cha_tmp, chp_cur, chp_tmp - chp_cur);
			cha_tmp[chp_tmp - chp_cur] = '\0';
			if (i == in_month) {
				sscanf(cha_tmp, "%d年 %d月", &sta_accessdata[j].in_year, &sta_accessdata[j].in_month);
			} else if (i == in_visit) {
				char *chp_wrk = cha_tmp;
				while (*chp_wrk) {
					if (*chp_wrk < '0' || *chp_wrk > '9') {
						chp_esc = chp_wrk;
						while (*chp_esc) {
							*chp_esc = *(chp_esc + 1);
							++chp_esc;
						}
					} else {
						++chp_wrk;
					}
				}
				sta_accessdata[j].in_visit = atoi(cha_tmp);
			}
			chp_cur = chp_tmp + strlen(cha_value_end);
		}
		++j;
		if (j >= 12)
			break;
	}
	free(chp_result);

	sprintf(cha_url, "http://%s/web-setup/asusage/usage_%04d%02d.html", g_cha_domain_ptn3, g_in_search_year, g_in_search_month);
	sprintf(cha_cmd, "%s -O %s --load-cookies %s --keep-session-cookies --referer=%s %s",
			g_cha_wget, cha_out, cha_cookie, cha_refer, cha_url);
	chp_result = Exec_Wget(cha_cmd, cha_out);
	if (!chp_result) {
		syslog(LOG_NOTICE, "failed to exec wget");
		remove(cha_out);
		remove(cha_cookie);
		return 1;
	}

	chp_cur = chp_result;
	Build_Refer_Data(&chp_cur, cha_enter_mark, cha_refer_line, cha_refer_begin, cha_refer_end, cha_refer_html1, cha_refer_html2, in_enter_url, in_enter_visit, sta_enterdata);
	Build_Refer_Data(&chp_cur, cha_exit_mark, cha_refer_line, cha_refer_begin, cha_refer_end, cha_refer_html1, cha_refer_html2, in_exit_url, in_exit_visit, sta_exitdata);
	Build_Refer_Data(&chp_cur, cha_refer_mark, cha_refer_line, cha_refer_begin, cha_refer_end, cha_refer_html1, cha_refer_html2, in_refer_url, in_refer_visit, sta_referdata);
	free(chp_result);

	sprintf(cha_url, "http://%s/web-setup/elogcsv.cgi?method=monthly\\&range=%04d/%02d\\&format=csv", g_cha_domain_ptn3, g_in_search_year, g_in_search_month);
	sprintf(cha_cmd, "%s -O %s --load-cookies %s --keep-session-cookies --referer=%s %s",
			g_cha_wget, cha_out, cha_cookie, cha_refer, cha_url);
	chp_result = Exec_Wget(cha_cmd, cha_out);
	if (!chp_result) {
		syslog(LOG_NOTICE, "failed to exec wget");
		remove(cha_out);
		remove(cha_cookie);
		return 1;
	}

	Build_Search_Data(chp_result, sta_searchthis);
	free(chp_result);

	in_year = g_in_search_year;
	in_month = g_in_search_month;
	--in_month;
	if (!in_month) {
		in_month = 12;
		--in_year;
	}

	sprintf(cha_url, "http://%s/web-setup/elogcsv.cgi?method=monthly\\&range=%04d/%02d\\&format=csv", g_cha_domain_ptn3, in_year, in_month);
	sprintf(cha_cmd, "%s -O %s --load-cookies %s --keep-session-cookies --referer=%s %s",
			g_cha_wget, cha_out, cha_cookie, cha_refer, cha_url);
	chp_result = Exec_Wget(cha_cmd, cha_out);
	if (!chp_result) {
		syslog(LOG_NOTICE, "failed to exec wget");
		remove(cha_out);
		remove(cha_cookie);
		return 1;
	}

	Build_Search_Data(chp_result, sta_searchlast);
	free(chp_result);

	for (i = 0; i < 12; ++i) {
		sprintf(cha_sql, "delete from rt_access_report where d_report = '%04d-%02d-01'", sta_accessdata[i].in_year, sta_accessdata[i].in_month);
		Db_ExecSql(db, cha_sql);
		if (sta_accessdata[i].in_year && sta_accessdata[i].in_month) {
			sprintf(cha_sql,
				"insert into rt_access_report "
					"(d_report,n_visit) "
				"values "
					"('%04d-%02d-01', %d)",
				sta_accessdata[i].in_year, sta_accessdata[i].in_month, sta_accessdata[i].in_visit);
			if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				return 1;
			}
		}
	}

	for (i = 0; i < 5; ++i) {
		sprintf(cha_sql, "delete from rt_refer_report where d_report = '%04d-%02d-01' and n_order = %d", g_in_search_year, g_in_search_month, i + 1);
		Db_ExecSql(db, cha_sql);
		if (sta_referdata[i].cha_url[0]) {
			sprintf(cha_sql,
				"insert into rt_refer_report "
					"(d_report,n_order,c_url,n_visit) "
				"values "
					"('%04d-%02d-01', %d, '%s', %d)",
				g_in_search_year, g_in_search_month, i + 1, sta_referdata[i].cha_url, sta_referdata[i].in_visit);
			if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				return 1;
			}
		}
	}

	for (i = 0; i < 5; ++i) {
		sprintf(cha_sql, "delete from rt_enter_report where d_report = '%04d-%02d-01' and n_order = %d", g_in_search_year, g_in_search_month, i + 1);
		Db_ExecSql(db, cha_sql);
		if (sta_enterdata[i].cha_url[0]) {
			sprintf(cha_sql,
				"insert into rt_enter_report "
					"(d_report,n_order,c_url,n_visit) "
				"values "
					"('%04d-%02d-01', %d, '%s', %d)",
				g_in_search_year, g_in_search_month, i + 1, sta_enterdata[i].cha_url, sta_enterdata[i].in_visit);
			if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				return 1;
			}
		}
	}

	for (i = 0; i < 5; ++i) {
		sprintf(cha_sql, "delete from rt_exit_report where d_report = '%04d-%02d-01' and n_order = %d", g_in_search_year, g_in_search_month, i + 1);
		Db_ExecSql(db, cha_sql);
		if (sta_exitdata[i].cha_url[0]) {
			sprintf(cha_sql,
				"insert into rt_exit_report "
					"(d_report,n_order,c_url,n_visit) "
				"values "
					"('%04d-%02d-01', %d, '%s', %d)",
				g_in_search_year, g_in_search_month, i + 1, sta_exitdata[i].cha_url, sta_exitdata[i].in_visit);
			if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				return 1;
			}
		}
	}

	for (i = 0; i < 30; ++i) {
		for (j = 0; j < 30; ++j) {
			if (!sta_searchthis[i].in_last &&
				strcmp(sta_searchthis[i].cha_keyword, sta_searchlast[j].cha_keyword) == 0 &&
				strcmp(sta_searchthis[i].cha_engine, sta_searchlast[j].cha_engine) == 0) {
				sta_searchthis[i].in_last = j + 1;
			}
		}
		sprintf(cha_sql, "delete from rt_keyword_report where d_report = '%04d-%02d-01' and n_order = %d", g_in_search_year, g_in_search_month, i + 1);
		Db_ExecSql(db, cha_sql);
		if (sta_searchthis[i].cha_keyword[0] && sta_searchthis[i].cha_engine[0]) {
			sprintf(cha_sql,
				"insert into rt_keyword_report "
					"(d_report,n_order,c_keyword,c_engine,n_visit,n_last) "
				"values "
					"('%04d-%02d-01', %d, '%s', '%s', %d, %d)",
				g_in_search_year, g_in_search_month, i + 1, sta_searchthis[i].cha_keyword, sta_searchthis[i].cha_engine,
				sta_searchthis[i].in_hit, sta_searchthis[i].in_last);
			if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				return 1;
			}
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		Big_Five_Report()
 * Description:
 *		売り上げ金額上位５つのレポート作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int Big_Five_Report(DBase *db)
{
	DBRes *dbres;
	char cha_sql[1024];
	char *chp_tmp;
	int in_prev_year;
	int in_prev_month;
	int in_next_year;
	int in_next_month;
	int in_year;
	int in_month;
	int i;

	in_prev_year = g_in_search_year;
	in_prev_month = g_in_search_month;
	--in_prev_year;

	in_next_year = g_in_search_year;
	in_next_month = g_in_search_month;
	++in_next_month;
	if (in_next_month > 12) {
		++in_next_year;
		in_next_month = 1;
	}

	strcpy(cha_sql, " create temporary table temp_report1 as");
	strcat(cha_sql, " select T3.c_item_id as c_item_id");	/* 0 商品id */
	strcat(cha_sql, ",T3.c_name as c_item_name");			/* 1 商品名 */
	strcat(cha_sql, ",sum(T2.n_amount*coalesce(T2.n_count, 0)) as n_amount");	/* 2 売上 */
	strcat(cha_sql, ",T3.n_order as n_order");				/* 3 順位 */
	strcat(cha_sql, " from st_charge T1");
	strcat(cha_sql, ",st_charge_detail T2");
	strcat(cha_sql, ",rt_sales_ranking_report T3");
	strcat(cha_sql, " where T3.c_item_id = T2.c_item_id");
	strcat(cha_sql, " and T2.n_charge_no = T1.n_charge_no");
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt >= '%04d-%02d-01'", in_prev_year, in_prev_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", in_next_year, in_next_month);
	sprintf(cha_sql+strlen(cha_sql), " and T3.d_report = '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	strcat(cha_sql, " group by c_item_id");
	strcat(cha_sql, " order by T3.n_amount desc, T3.c_name");
	strcat(cha_sql, " limit 5");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}

	strcpy(cha_sql, " create temporary table temp_report2 as");
	strcat(cha_sql, " select");
	strcat(cha_sql, " T3.c_item_id as c_item_id");					/* 0 商品id */
	strcat(cha_sql, ",T3.c_item_name as c_item_name");				/* 1 商品名 */
	strcat(cha_sql, ",sum(T2.n_amount*coalesce(T2.n_count, 0)) as n_amount");	/* 2 売上*/
	strcat(cha_sql, ",sum(coalesce(T2.n_count, 0)) as n_count");	/* 3 販売数*/
	strcat(cha_sql, ",T1.d_receipt as d_receipt");					/* 4 日付 */
	strcat(cha_sql, ",T3.n_order as n_order");						/* 5 順位 */
	strcat(cha_sql, " from st_charge T1");
	strcat(cha_sql, ",st_charge_detail T2");
	strcat(cha_sql, ",temp_report1 T3");
	strcat(cha_sql, " where T3.c_item_id = T2.c_item_id");
	strcat(cha_sql, " and T2.n_charge_no = T1.n_charge_no");
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt >= '%04d-%02d-01'", in_prev_year, in_prev_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	strcat(cha_sql, " group by T3.c_item_id, date_format(T1.d_receipt, '%Y-%m-%d')");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}

	sprintf(cha_sql, "delete from rt_top_sales_report where d_report = '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	Db_ExecSql(db, cha_sql);
	strcpy(cha_sql, " insert into rt_top_sales_report (d_report,d_data,c_item_id,c_name,n_amount,n_order)");
	strcat(cha_sql, " select");
	sprintf(cha_sql + strlen(cha_sql), " '%d-%d-01'", g_in_search_year, g_in_search_month);		/* 0 レポート日付 */
	strcat(cha_sql, ",date_format(T1.d_receipt,'%Y-%m-01')");	/* 1 データ日付 */
	strcat(cha_sql, ",T1.c_item_id");							/* 2 商品id */
	strcat(cha_sql, ",T1.c_item_name");							/* 3 商品名 */
	strcat(cha_sql, ",sum(T1.n_amount)");						/* 4 売上*/
	strcat(cha_sql, ",T1.n_order");								/* 5 順位 */
	strcat(cha_sql, " from temp_report2 T1");
	sprintf(cha_sql+strlen(cha_sql), " where T1.d_receipt >= '%04d-%02d-01'", in_prev_year, in_prev_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " group by T1.c_item_id, date_format(T1.d_receipt, '%Y-%m')");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
	}

	sprintf(cha_sql, "delete from rt_top_daily_report where d_report = '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	Db_ExecSql(db, cha_sql);
	strcpy(cha_sql, " insert into rt_top_daily_report (d_report,d_data,c_item_id,c_name,n_count,n_order)");
	strcat(cha_sql, " select");
	sprintf(cha_sql + strlen(cha_sql), " '%d-%d-01'", g_in_search_year, g_in_search_month);		/* 0 レポート日付 */
	strcat(cha_sql, ",T1.d_receipt");			/* 1 日付 */
	strcat(cha_sql, ",T1.c_item_id");			/* 2 商品id */
	strcat(cha_sql, ",T1.c_item_name");			/* 3 商品名 */
	strcat(cha_sql, ",sum(T1.n_count)");		/* 4 売上*/
	strcat(cha_sql, ",T1.n_order");				/* 5 順位 */
	strcat(cha_sql, " from temp_report2 T1");
	sprintf(cha_sql+strlen(cha_sql), " where T1.d_receipt >= '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " group by T1.c_item_id, date_format(T1.d_receipt, '%Y-%m-%d')");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
	}

	dbres = Db_OpenDyna(db, "select c_item_id,c_item_name,n_order from temp_report1");
	if (dbres) {
		for (i = 0; i < Db_GetRowCount(dbres); ++i) {
			for (in_year = in_prev_year, in_month = in_prev_month; in_year * 12 + in_month < in_next_year * 12 + in_next_month; ++in_month) {
				DBRes *dbles;
				if (in_month > 12) {
					in_month = 1;
					in_year++;
				}
				sprintf(cha_sql, "select count(*) from rt_top_sales_report where d_report = '%04d-%02d-01' and d_data = '%04d-%02d-01' and c_item_id = '%s'",
					g_in_search_year, g_in_search_month, in_year, in_month, Db_GetValue(dbres, i, 0));
				dbles = Db_OpenDyna(db, cha_sql);
				if (dbles) {
					chp_tmp = Db_GetValue(dbles, 0, 0);
					if (!chp_tmp || !atoi(chp_tmp)) {
						chp_tmp = Escape_SqlString(Db_GetValue(dbres, i, 1));
						sprintf(cha_sql,
							" insert into rt_top_sales_report"
							" (d_report,d_data,c_item_id,c_name,n_amount,n_order)"
							" values"
							" ('%04d-%02d-01','%04d-%02d-01','%s','%s',0,%s)",
							g_in_search_year, g_in_search_month, in_year, in_month,
							Db_GetValue(dbres, i, 0), chp_tmp, Db_GetValue(dbres, i, 2));
						free(chp_tmp);
						if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
							syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
						}
					}
					Db_CloseDyna(dbles);
				} else {
					syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				}
			}
		}
		Db_CloseDyna(dbres);
	} else {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		Sales_Ranking_Report()
 * Description:
 *		売り上げランキングレポート作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int Sales_Ranking_Report(DBase *db)
{
	typedef struct _RANKDATA {
		char	cha_id[65];
		char	cha_item[129];
		int		in_sales;
		float	in_percent;
		int		in_count;
		int		in_price;
		int		in_last_rank;
	} RANKDATA;
	RANKDATA sta_rankdata[11];
	DBRes *dbres;
	char cha_sql[1024];
	char *chp_tmp;
	int in_prev_year;
	int in_prev_month;
	int in_next_year;
	int in_next_month;
	int in_count;
	int in_amount;
	int in_all_count;
	int in_all_amount;
	int i, j;

	in_next_year = g_in_search_year;
	in_next_month = g_in_search_month;
	++in_next_month;
	if (in_next_month > 12) {
		++in_next_year;
		in_next_month = 1;
	}

	in_prev_year = g_in_search_year;
	in_prev_month = g_in_search_month;
	--in_prev_month;
	if (in_prev_month == 0) {
		--in_prev_year;
		in_prev_month = 12;
	}

	for (i = 0; i < 11; ++i) {
		sta_rankdata[i].cha_id[0] = '\0';
		sta_rankdata[i].cha_item[0] = '\0';
		sta_rankdata[i].in_sales = 0;
		sta_rankdata[i].in_percent = 0.0;
		sta_rankdata[i].in_count = 0;
		sta_rankdata[i].in_price = 0;
		sta_rankdata[i].in_last_rank = 0;
	}
	strcpy(sta_rankdata[10].cha_item, "その他");

	strcpy(cha_sql, " select");
	strcat(cha_sql, " sum(T2.n_amount*coalesce(T2.n_count, 0))");	/* 0 売上 */
	strcat(cha_sql, ",sum(coalesce(T2.n_count, 0))");				/* 2 個数 */
	strcat(cha_sql, " from st_charge T1");
	strcat(cha_sql, ",st_charge_detail T2");
	strcat(cha_sql, " where T2.n_charge_no = T1.n_charge_no");
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt >= '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	in_all_amount = (chp_tmp ? atoi(chp_tmp) : 0);
	chp_tmp = Db_GetValue(dbres, 0, 1);
	in_all_count = (chp_tmp ? atoi(chp_tmp) : 0);
	Db_CloseDyna(dbres);

	strcpy(cha_sql, " create temporary table tmp_item_rank(");
	strcat(cha_sql, " primary key (c_item_id, c_sub1_id, c_sub2_id)");
	strcat(cha_sql, ")");
	strcat(cha_sql, " select T1.c_item_id");
	strcat(cha_sql, ",T1.c_sub1_id");
	strcat(cha_sql, ",T1.c_sub2_id");
	strcat(cha_sql, ",min(T1.n_rank) as n_rank");
	strcat(cha_sql, " from st_item_price_rank T1");
	strcat(cha_sql, " where T1.n_selling_price is not null");
	strcat(cha_sql, " group by T1.c_item_id, T1.c_sub1_id, T1.c_sub2_id");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}

	strcpy(cha_sql, " create temporary table temp_ranking as");
	strcat(cha_sql, " select");
	strcat(cha_sql, " T3.c_item_id");
	strcat(cha_sql, ",T3.c_name");
	strcat(cha_sql, ",sum(T2.n_amount*coalesce(T2.n_count, 0)) as all_amount");
	strcat(cha_sql, ",sum(coalesce(T2.n_count, 0)) as all_count");
	strcat(cha_sql, ",T4.n_selling_price");
	strcat(cha_sql, ",T1.n_consumption_tax");
	strcat(cha_sql, " from st_charge T1");
	strcat(cha_sql, ",st_charge_detail T2");
	strcat(cha_sql, ",sm_item T3");
	strcat(cha_sql, ",st_item_price_rank T4");
	strcat(cha_sql, ",tmp_item_rank T5");
	strcat(cha_sql, " where T3.c_item_id = T2.c_item_id");
	strcat(cha_sql, " and T4.c_item_id = T2.c_item_id");
	strcat(cha_sql, " and T4.c_sub1_id = T2.c_sub1_id");
	strcat(cha_sql, " and T4.c_sub2_id = T2.c_sub2_id");
	strcat(cha_sql, " and T4.c_item_id = T5.c_item_id");
	strcat(cha_sql, " and T4.c_sub1_id = T5.c_sub1_id");
	strcat(cha_sql, " and T4.c_sub2_id = T5.c_sub2_id");
	strcat(cha_sql, " and T4.n_rank = T5.n_rank");
	strcat(cha_sql, " and T2.n_charge_no = T1.n_charge_no");
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt >= '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	strcat(cha_sql, " group by T3.c_item_id");
	strcat(cha_sql, " order by all_amount desc, T3.c_name");
	strcat(cha_sql, " limit 10");
/*
	strcpy(cha_sql, " create temporary table temp_ranking as");
	strcat(cha_sql, " select");
	strcat(cha_sql, " T3.c_item_id");			// 0 商品id
	strcat(cha_sql, ",T3.c_name");				// 1 商品名
	strcat(cha_sql, ",sum(T2.n_amount*coalesce(T2.n_count, 0)) as all_amount");	// 2 売上
	strcat(cha_sql, ",sum(coalesce(T2.n_count, 0)) as all_count");				// 3 販売数合計
	strcat(cha_sql, ",T4.n_selling_price");	// 4 単価
	strcat(cha_sql, ",T1.n_consumption_tax");
	strcat(cha_sql, " from st_charge T1");
	strcat(cha_sql, ",st_charge_detail T2");
	strcat(cha_sql, ",sm_item T3");
	strcat(cha_sql, ",sm_subitem T4");
	strcat(cha_sql, " where T3.c_item_id = T2.c_item_id");
	strcat(cha_sql, " and T4.c_item_id = T2.c_item_id");
	strcat(cha_sql, " and T4.c_sub_id = T2.c_sub_id");
	strcat(cha_sql, " and T2.n_charge_no = T1.n_charge_no");
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt >= '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	strcat(cha_sql, " group by T3.c_item_id");
	strcat(cha_sql, " order by all_amount desc, T3.c_name");
	strcat(cha_sql, " limit 10");
*/
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	strcpy(cha_sql, " create temporary table temp_ranking2 as");
	strcat(cha_sql,
		" select T2.c_item_id"
		",T1.n_selling_price + FLOOR(T1.n_selling_price * T1.n_consumption_tax / 100.0) as n_selling_price"
		" from temp_ranking T1"
		",sm_subitem T2"
		" where T1.c_item_id = T2.c_item_id"
//		" and T2.n_selling_price is not null"
//		" and T2.n_selling_price != 0"
		" and T2.c_name is not null"
		" and T2.c_name != ''"
		" group by T2.c_item_id"
		" order by T2.c_sub_id");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}

	if (Db_ExecSql(db, "update temp_ranking T1,temp_ranking2 T2 set T1.n_selling_price = T2.n_selling_price where T1.c_item_id = T2.c_item_id") != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}

	strcpy(cha_sql, " select");
	strcat(cha_sql, " c_item_id");			/* 0 商品id */
	strcat(cha_sql, ",c_name");				/* 1 商品名 */
	strcat(cha_sql, ",all_amount");			/* 2 売上*/
	strcat(cha_sql, ",all_count");			/* 3 販売数合計*/
	strcat(cha_sql, ",n_selling_price");	/* 4 単価 */
	strcat(cha_sql, " from temp_ranking");
	strcat(cha_sql, " order by all_amount desc, c_name");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	in_count = 0;
	in_amount = 0;
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		strcpy(sta_rankdata[i].cha_id, chp_tmp ? chp_tmp : "");
		chp_tmp = Db_GetValue(dbres, i, 1);
		strcpy(sta_rankdata[i].cha_item, chp_tmp ? chp_tmp : "");
		chp_tmp = Db_GetValue(dbres, i, 2);
		sta_rankdata[i].in_sales = chp_tmp ? atoi(chp_tmp) : 0;
		in_amount += sta_rankdata[i].in_sales;
		chp_tmp = Db_GetValue(dbres, i, 3);
		sta_rankdata[i].in_count = chp_tmp ? atoi(chp_tmp) : 0;
		chp_tmp = Db_GetValue(dbres, i, 4);
		sta_rankdata[i].in_price = chp_tmp ? atoi(chp_tmp) : 0;
		in_count += sta_rankdata[i].in_count;
	}
	Db_CloseDyna(dbres);

	strcpy(cha_sql, " select");
	strcat(cha_sql, " T3.c_item_id");					/* 0 商品id */
	strcat(cha_sql, ",sum(T2.n_amount*coalesce(T2.n_count, 0)) as all_amount");	/* 1 売上*/
	strcat(cha_sql, " from st_charge T1");
	strcat(cha_sql, ",st_charge_detail T2");
	strcat(cha_sql, ",sm_item T3");
	strcat(cha_sql, " where T3.c_item_id = T2.c_item_id");
	strcat(cha_sql, " and T2.n_charge_no = T1.n_charge_no");
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt >= '%04d-%02d-01'", in_prev_year, in_prev_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt <  '%04d-%02d-01'", g_in_search_year, g_in_search_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	strcat(cha_sql, " group by T3.c_item_id");
	strcat(cha_sql, " order by all_amount desc, T3.c_name");
	strcat(cha_sql, " limit 100");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		for (j = 0; j < 10; ++j) {
			if (chp_tmp && strcmp(chp_tmp, sta_rankdata[j].cha_id) == 0) {
				sta_rankdata[j].in_last_rank =  i + 1;
				break;
			}
		}
	}
	Db_CloseDyna(dbres);

	sta_rankdata[10].in_count = in_all_count - in_count;
	sta_rankdata[10].in_sales = in_all_amount - in_amount;
	sta_rankdata[10].in_percent = in_all_amount ? ((float)sta_rankdata[10].in_sales / (float)in_all_amount) * 100.0 : 0.0;
	for (i = 0; i < 10; ++i) {
		sta_rankdata[i].in_percent = in_all_amount ? ((float)sta_rankdata[i].in_sales / (float)in_all_amount) * 100.0 : 0.0;
	}

	for (i = 0; i < 11; ++i) {
		sprintf(cha_sql, "delete from rt_sales_ranking_report where d_report = '%04d-%02d-01' and n_order = %d",
			g_in_search_year, g_in_search_month, i + 1);
		Db_ExecSql(db, cha_sql);
		chp_tmp = Escape_SqlString(sta_rankdata[i].cha_item);
		sprintf(cha_sql,
			"insert into rt_sales_ranking_report "
				"(d_report,c_name,c_item_id,n_order,n_amount,n_percent,n_count,n_unit_price,n_last_order) "
			"values "
				"('%04d-%02d-01', '%s', '%s', %d, %d, %f, %d, %d, %d)",
			g_in_search_year, g_in_search_month,
			chp_tmp, sta_rankdata[i].cha_id, i + 1,
			sta_rankdata[i].in_sales, sta_rankdata[i].in_percent,
			sta_rankdata[i].in_count, sta_rankdata[i].in_price,
			sta_rankdata[i].in_last_rank);
		free(chp_tmp);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
			return 1;
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		Whole_Sales_Report()
 * Description:
 *		売り上げ全体のレポート作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int Whole_Sales_Report(DBase *db)
{
	typedef struct _MONTHDATA {
		char	cha_date[20];
		int		in_sales;
		int		in_count;
		int		in_unit;
		int		in_days;
	} MONTHDATA;
	MONTHDATA sta_monthdata[13];
	DBRes *dbres;
	char cha_sql[1024];
	char cha_unit[128];
	char cha_date[16];
	char *chp_tmp;
	char *chp_tmp2;
	int in_from_year;
	int in_from_month;
	int in_next_year;
	int in_next_month;
	int in_year;
	int in_month;
	int in_cnt;
	int i, j;

	in_next_year = g_in_search_year;
	in_next_month = g_in_search_month;
	++in_next_month;
	if (in_next_month > 12) {
		++in_next_year;
		in_next_month = 1;
	}

	in_from_year = g_in_search_year;
	in_from_month = g_in_search_month;
	--in_from_year;

	in_year = in_from_year;
	in_month = in_from_month;
	for (i = 0; i < 13; ++i) {
		sprintf(sta_monthdata[i].cha_date, "%04d-%02d-01", in_year, in_month);
		sta_monthdata[i].in_sales = 0;
		sta_monthdata[i].in_count = 0;
		sta_monthdata[i].in_unit = 0;
		sta_monthdata[i].in_days = Get_MonthDay(in_year, in_month);
		++in_month;
		if (in_month > 12) {
			++in_year;
			in_month = 1;
		}
	}

	strcpy(cha_sql, " select ");
	strcat(cha_sql, " date_format(T1.d_receipt, '%Y-%m-01')");		/* 0 日付   */
	strcat(cha_sql, ",sum(T2.n_amount*coalesce(T2.n_count, 0))");	/* 1 売上   */
	strcat(cha_sql, ",count(T1.n_charge_no)");						/* 2 伝票数 */
	strcat(cha_sql, " from st_charge T1");
	strcat(cha_sql, ",st_charge_detail T2");
	strcat(cha_sql, " where");
	strcat(cha_sql, " T2.n_charge_no = T1.n_charge_no");
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt >='%04d-%02d-01'", in_from_year, in_from_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt < '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	strcat(cha_sql, " group by date_format(T1.d_receipt, '%Y年%m月')");
	strcat(cha_sql, " order by T1.d_receipt, T1.n_charge_no");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		for (j = 0; j < 13; ++j) {
			if (strcmp(chp_tmp, sta_monthdata[j].cha_date) == 0) {
				chp_tmp = Db_GetValue(dbres, i, 1);
				sta_monthdata[j].in_sales = (chp_tmp ? atoi(chp_tmp) : 0);
				chp_tmp = Db_GetValue(dbres, i, 2);
				sta_monthdata[j].in_count = (chp_tmp ? atoi(chp_tmp) : 0);
				sta_monthdata[j].in_unit = (sta_monthdata[j].in_count ? sta_monthdata[j].in_sales/sta_monthdata[j].in_count : 0);
				break;
			}
		}
	}
	Db_CloseDyna(dbres);

	for (i = 0; i < 13; ++i) {
		sprintf(cha_sql, "delete from rt_year_sales_report where d_report = '%s'", sta_monthdata[i].cha_date);
		Db_ExecSql(db, cha_sql);
		sprintf(cha_sql,
			"insert into rt_year_sales_report "
				"(d_report,n_amount,n_count,n_unit_price,n_amount_per_day,n_count_per_day) "
			"values "
				"('%s', %d, %d, %d, %d, %f)",
			sta_monthdata[i].cha_date, sta_monthdata[i].in_sales,
			sta_monthdata[i].in_count, sta_monthdata[i].in_unit,
			sta_monthdata[i].in_sales/sta_monthdata[i].in_days,
			(float)sta_monthdata[i].in_count/(float)sta_monthdata[i].in_days);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
			return 1;
		}
	}

	strcpy(cha_sql, " select ");
	strcat(cha_sql, " date_format(T1.d_receipt, '%Y-%m-01')");		/* 0 日付   */
	strcat(cha_sql, ",count(T1.n_charge_no)");						/* 1 伝票数 */
	strcat(cha_sql, " from st_charge T1");
	sprintf(cha_sql+strlen(cha_sql), " where T1.d_receipt >='%04d-%02d-01'", in_from_year, in_from_month);
	sprintf(cha_sql+strlen(cha_sql), " and T1.d_receipt < '%04d-%02d-01'", in_next_year, in_next_month);
	strcat(cha_sql, " and (T1.b_cancel is null or T1.b_cancel=0)");
	strcat(cha_sql, " group by date_format(T1.d_receipt, '%Y年%m月')");
	strcat(cha_sql, " order by T1.d_receipt, T1.n_charge_no");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_tmp2 = Db_GetValue(dbres, i, 1);
		if (chp_tmp && chp_tmp2) {
			in_cnt = atoi(chp_tmp2);
			snprintf(cha_unit, sizeof(cha_unit), "n_amount / %d", in_cnt);
			strcpy(cha_date, chp_tmp);
			cha_date[4] = '\0';
			cha_date[7] = '\0';
			sprintf(cha_sql, "update rt_year_sales_report set n_count = %d, n_unit_price = %s, n_count_per_day = %f where d_report = '%s'",
				in_cnt, in_cnt ? cha_unit : "0",
				(float)atoi(chp_tmp2)/(float)Get_MonthDay(atoi(cha_date), atoi(cha_date + 5)), chp_tmp);
			if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				return 1;
			}
		}
	}
	Db_CloseDyna(dbres);

	return 0;
}

void Search_Proc(DBase *db, DBase *db_blog,
	const char *chp_keyword,
	const char *chp_engine,
	int in_engine,
	int in_max,
	int in_max_num,
	int in_begin,
	float fl_mult,
	const char *chp_code,
	const char *chp_cookie,
	const char *chp_url,
	const char *chp_chp_head,
	const char *chp_chp_head2,
	const char *chp_chp_tail,
	const char *chp_chp_tail2,
	const char *chp_first,
	const char *chp_chp_line,
	const char *chp_child,
	const char *chp_chp_url_head,
	const char *chp_chp_url_tail)
{
	FILE *pp;
	DBRes *ptres;
	char cha_number[1024];
	char cha_out[256];
	char cha_cmd[1024];
	char cha_head[512];
	char cha_rank[8];
	char cha_real_url[1024];
	char *chp_tmp;
	char *chp_tmp2;
//	char *chp_tmp3;
	char *chp_buf;
	char *chp_cur;
	char *chp_enc;
	char *chp_esc;
	char *chp_head;
	char *chp_head2;
	char *chp_tail;
	char *chp_tail2;
	char *chp_line;
//	char *chp_yaku;
	char *chp_url_head;
	char *chp_url_tail;
	int j, k;
	int fd;
//	int in_number;
	int in_point;
	int in_found;
	int in_count;

	if (!chp_keyword || !chp_keyword[0] ||
		!chp_url || !chp_url[0] ||
		!chp_chp_head || !chp_chp_head[0] ||
		!chp_chp_tail || !chp_chp_tail[0] ||
		!chp_chp_line || !chp_chp_line[0] ||
		!chp_chp_url_head || !chp_chp_url_head[0] ||
		!chp_chp_url_tail || !chp_chp_url_tail[0]) {
		return;
	}

	Conv_From_Euc(chp_keyword, chp_code, &chp_enc);
	Encode_Url(chp_enc, &chp_esc);
	Conv_From_Euc(chp_chp_head, chp_code, &chp_head);
	Conv_From_Euc(chp_chp_tail, chp_code, &chp_tail);
	Conv_From_Euc(chp_chp_line, chp_code, &chp_line);
	Conv_From_Euc(chp_chp_url_head, chp_code, &chp_url_head);
	if (chp_chp_head2) {
		Conv_From_Euc(chp_chp_head2, chp_code, &chp_head2);
	} else {
		chp_head2 = NULL;
	}
	if (chp_chp_tail2) {
		Conv_From_Euc(chp_chp_tail2, chp_code, &chp_tail2);
	} else {
		chp_tail2 = NULL;
	}
	Conv_From_Euc(chp_chp_url_tail, chp_code, &chp_url_tail);

	sprintf(cha_out, "/var/tmp/wget_%d_XXXXX", getpid());
	fd = mkstemp(cha_out);
	close(fd);

	in_found = 0;
	in_count = 0;
	cha_number[0] = '\0';
	for (k = 0; k < in_max_num/in_max && !in_found; ++k) {
		sprintf(cha_real_url, chp_url, chp_esc, k * in_max + in_begin);
		strcpy(cha_cmd, g_cha_wget);
		if (chp_cookie && chp_cookie[0]) {
			strcat(cha_cmd, "--cookies=off --header \"Cookie: ");
			strcat(cha_cmd, chp_cookie);
			strcat(cha_cmd, "\" ");
		}
		strcat(cha_cmd, "-O ");
		strcat(cha_cmd, cha_out);
		strcat(cha_cmd, " ");
		strcat(cha_cmd, cha_real_url);
		strcat(cha_cmd, " 2>&1");
		pp = popen(cha_cmd, "r");
		if (!pp) {
			syslog(LOG_NOTICE, "failed to exec wget(%s)", strerror(errno));
			remove(cha_out);
			continue;
		}
		while (fgets(cha_cmd, sizeof(cha_cmd), pp))
			;
		pclose(pp);
		pp = fopen(cha_out, "r");
		if (!pp) {
			syslog(LOG_NOTICE, "failed to open wget result(%s)", strerror(errno));
			remove(cha_out);
			continue;
		}
		chp_buf = NULL;
		while (fgets(cha_cmd, sizeof(cha_cmd), pp)) {
			if (!chp_buf) {
				chp_buf = (char*)malloc(strlen(cha_cmd) + 1);
				strcpy(chp_buf, cha_cmd);
			} else {
				chp_tmp = realloc(chp_buf, strlen(chp_buf) + strlen(cha_cmd) + 1);
				if (chp_tmp) {
					chp_buf = chp_tmp;
					strcat(chp_buf, cha_cmd);
				}
			}
		}
		fclose(pp);
		if (!chp_buf) {
			continue;
		}

		// 検索された件数取得
		chp_cur = chp_buf;
		sprintf(cha_head, chp_head, chp_enc);
		chp_tmp = strstr(chp_cur, cha_head);
/*
		if (chp_tmp) {
			chp_tmp2 = strstr(chp_tmp + strlen(cha_head), chp_tail);
			if (chp_tmp2) {
				if (chp_tmp2 - chp_tmp > sizeof(cha_number)) {
					chp_tmp = NULL;
				}
			} else {
				chp_tmp = NULL;
			}
		}
*/
		if (!chp_tmp && chp_head2) {
			sprintf(cha_head, chp_head2, chp_enc);
			chp_tmp = strstr(chp_cur, cha_head);
		}
		if (chp_tmp) {
			int n;
			chp_tmp += strlen(cha_head);
			while (*chp_tmp < '0' || *chp_tmp > '9') {
				++chp_tmp;
			}
			n = 0;
			while (n < sizeof(cha_number) - 1) {
				char c = *chp_tmp;
				if (c >= '0' && c <= '9') {
					cha_number[n] = c;
					++n;
				} else if (c == ',') {
				} else {
					break;
				}
				++chp_tmp;
			}
			cha_number[n] = '\0';
			chp_cur = chp_tmp;
		}
/*
		if (chp_tmp) {
			while (*chp_tmp < '0' || *chp_tmp > '9') {
				++chp_tmp;
			}
			chp_tmp2 = strstr(chp_tmp, chp_tail);
			if (chp_tmp2) {
				if (!cha_number[0] && chp_tmp2 - chp_tmp < sizeof(cha_number)) {
					strncpy(cha_number, chp_tmp, chp_tmp2 - chp_tmp);
					cha_number[chp_tmp2 - chp_tmp] = '\0';
				}
				chp_cur = chp_tmp2 + strlen(chp_tail);
			} else if (chp_tail2) {
				chp_tmp2 = strstr(chp_tmp, chp_tail2);
				if (chp_tmp2) {
					if (!cha_number[0] && chp_tmp2 - chp_tmp < sizeof(cha_number)) {
						strncpy(cha_number, chp_tmp, chp_tmp2 - chp_tmp);
						cha_number[chp_tmp2 - chp_tmp] = '\0';
					}
					chp_cur = chp_tmp2 + strlen(chp_tail2);
				} else {
					chp_cur = chp_tmp;
				}
			} else {
				chp_cur = chp_tmp;
			}
		}
*/
		for (j = 0; j < in_max; ++j) {
			// 行の頭を探す
			const char *chp_obj;
			chp_tmp = strstr(chp_cur, chp_line);
			chp_obj = chp_line;
//			chp_tmp = strstr(chp_cur, (j > 0 || !chp_first || !chp_first[0]) ? chp_line : chp_first);
//			chp_obj = (j > 0 || !chp_first || !chp_first[0]) ? chp_line : chp_first;
//			chp_tmp = strstr(chp_cur, chp_obj);
			if (chp_child && chp_child[0]) {
				chp_tmp2 = strstr(chp_cur, chp_child);
				if (chp_tmp2 && chp_tmp2 < chp_tmp) {
					chp_tmp = chp_tmp2;
					chp_obj = chp_child;
				}
			}
			if (!chp_tmp) {
//				break;
				continue;
			}
			++in_count;
			chp_cur = chp_tmp + strlen(chp_obj);
			// URLの頭を探す
			chp_tmp = strstr(chp_cur, chp_url_head);
			if (!chp_tmp) {
				continue;
			}
			chp_cur = chp_tmp + strlen(chp_url_head);
			// URLの最後を探す
			chp_tmp = strstr(chp_cur, chp_url_tail);
			if (!chp_tmp) {
				continue;
			}
			strncpy(cha_cmd, chp_cur, chp_tmp - chp_cur);
			cha_cmd[chp_tmp - chp_cur] = '\0';
			Decode_Form(cha_cmd);
			if (strstr(cha_cmd, g_cha_domain_ptn1) || strstr(cha_cmd, g_cha_domain_ptn2)) {
				in_found = 1;
				break;
			}
			chp_cur = chp_tmp + strlen(chp_url_tail);
		}
		free(chp_buf);
	}
	free(chp_esc);

	chp_tmp = cha_number;
	while (*chp_tmp) {
		if (*chp_tmp < '0' || *chp_tmp > '9') {
			chp_esc = chp_tmp;
			while (*chp_esc) {
				*chp_esc = *(chp_esc + 1);
				++chp_esc;
			}
		} else {
			++chp_tmp;
		}
	}
//	in_number = atoi(cha_number);

	in_point = 0;
	if (in_found && in_count <= 20) {
		sprintf(cha_cmd, "select n_point from sy_add_point where (n_min <= %s and n_max >= %s) or (n_min <= %s and n_max is null)", cha_number, cha_number, cha_number);
		ptres = Db_OpenDyna(db_blog, cha_cmd);
		if (ptres) {
			chp_tmp = My_Db_FixGetValue(ptres, 0, 0);
			if (chp_tmp) {
				in_point += atoi(chp_tmp);
			}
			Db_CloseDyna(ptres);
		}
	}
	if (in_found) {
		sprintf(cha_cmd, "select n_point from sy_search_point where n_min <= %d and n_max >= %d", in_count, in_count);
		ptres = Db_OpenDyna(db_blog, cha_cmd);
		if (ptres) {
			chp_tmp = My_Db_FixGetValue(ptres, 0, 0);
			if (chp_tmp) {
				in_point += atoi(chp_tmp);
			}
			Db_CloseDyna(ptres);
		}
	} else {
		in_count = 0;
	}
	in_point = (int)((float)in_point * fl_mult);
	if (in_point > 100)
		in_point = 100;

	cha_rank[0] = '\0';
	sprintf(cha_cmd, "select c_rank from sy_search_rank where n_min <= %d and n_max >= %d", in_point, in_point);
	ptres = Db_OpenDyna(db_blog, cha_cmd);
	if (ptres) {
		chp_tmp = My_Db_FixGetValue(ptres, 0, 0);
		if (chp_tmp) {
			strcpy(cha_rank, chp_tmp);
		}
		Db_CloseDyna(ptres);
	}

	sprintf(cha_cmd,
		"insert into rt_seo_report (d_report,c_keyword,c_engine,n_order,n_number,n_point,c_rank,n_engine) "
		"values ('%04d-%02d-%02d', '%s', '%s', %d, %s, %d, '%s', %d)",
		g_in_current_year, g_in_current_month, g_in_current_day, chp_keyword, chp_engine,
		in_count, cha_number, in_point, cha_rank, in_engine);
	if (Db_ExecSql(db, cha_cmd) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(--)", Gcha_last_error);
	}

	free(chp_enc);
	remove(cha_out);
}

/*
+* ------------------------------------------------------------------------
 * Function:		Seo_Report()
 * Description:
 *		SEO対策用のレポート作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
static int Seo_Report(DBase *db, DBase *db_blog)
{
	DBRes *dbres;
	DBRes *blogres;
	char cha_cmd[1024];
	char *chp_tmp;
	char *chp_url;
	char *chp_code;
	char *chp_first;
	char *chp_child;
	char *chp_cookie;
	int i;
	int l;
	int in_engine;
	int in_begin;
	int in_max;
	float fl_mult;
//	pid_t pid;

	in_max = Get_MonthDay(g_in_current_year, g_in_current_month);
	sprintf(cha_cmd, "delete from rt_seo_report where d_report>='%04d-%02d-01 00:00:00' and d_report<='%04d-%02d-%02d 23:59:59'",
		g_in_current_year, g_in_current_month, g_in_current_year, g_in_current_month, in_max);
	if (Db_ExecSql(db, cha_cmd) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
	}

	blogres = Db_OpenDyna(db_blog,
		" select"
		" c_engine"
		",c_code"
		",c_cookie"
		",c_url"
		",c_whole_head"
		",c_whole_tail"
		",c_first_head"
		",c_line_head"
		",c_url_head"
		",c_url_tail"
		",c_child_head"
		",n_max"
		",f_multiple"
		",n_engine"
		",n_begin"
		",c_whole_head2"
		",c_whole_tail2"
		" from sy_search_engine"
		" order by n_engine");
	if (!blogres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		return 1;
	}
	dbres = Db_OpenDyna(db, "select c_keyword from sm_create_html_keyword order by n_keyword_id");
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		Db_CloseDyna(blogres);
		return 1;
	}

	for (l = 0; l < Db_GetRowCount(blogres); ++l) {
//printf("%s\n", My_Db_FixGetValue(blogres, l, 0));
		chp_code = My_Db_FixGetValue(blogres, l, 1);
		chp_cookie = My_Db_FixGetValue(blogres, l, 2);
		chp_url = My_Db_FixGetValue(blogres, l, 3);
		if (!chp_url || !chp_url[0]) {
			continue;
		}
		chp_tmp = My_Db_FixGetValue(blogres, l, 6);
		if (chp_tmp && chp_tmp[0]) {
			Conv_From_Euc(chp_tmp, chp_code, &chp_first);
		} else {
			chp_first = NULL;
		}
//printf("first1 = 0x%x\n", chp_first);
		chp_tmp = My_Db_FixGetValue(blogres, l, 10);
		if (chp_tmp && chp_tmp[0]) {
			Conv_From_Euc(chp_tmp, chp_code, &chp_child);
		} else {
			chp_child = NULL;
		}
		chp_tmp = My_Db_FixGetValue(blogres, l, 11);
		if (chp_tmp) {
			in_max = atoi(chp_tmp);
		} else {
			in_max = 100;
		}
		chp_tmp = My_Db_FixGetValue(blogres, l, 12);
		if (chp_tmp) {
			fl_mult = atof(chp_tmp);
		} else {
			fl_mult = 1.0;
		}
		chp_tmp = My_Db_FixGetValue(blogres, l, 13);
		if (chp_tmp) {
			in_engine = atoi(chp_tmp);
		} else {
			in_engine = 0;
		}
		chp_tmp = My_Db_FixGetValue(blogres, l, 14);
		if (chp_tmp) {
			in_begin = atoi(chp_tmp);
		} else {
			in_begin = 0;
		}
		for (i = 0; i < Db_GetRowCount(dbres); ++i) {
//printf("0x%x - %s\n", chp_first, My_Db_FixGetValue(dbres, i, 0));
			chp_tmp = My_Db_FixGetValue(dbres, i, 0);
			if (!chp_tmp || !chp_tmp[0]) {
				continue;
			}
//			if (!fork()) {
				Search_Proc(db, db_blog,
					chp_tmp,
					My_Db_FixGetValue(blogres, l, 0),
					in_engine,
					in_max,
					100,
					in_begin,
					fl_mult,
					chp_code,
					chp_cookie,
					chp_url,
					My_Db_FixGetValue(blogres, l, 4),
					My_Db_FixGetValue(blogres, l, 15),
					My_Db_FixGetValue(blogres, l, 5),
					My_Db_FixGetValue(blogres, l, 16),
					chp_first,
					My_Db_FixGetValue(blogres, l, 7),
					chp_child,
					My_Db_FixGetValue(blogres, l, 8),
					My_Db_FixGetValue(blogres, l, 9));
//				exit (0);
//			}
		}
	}

//	while ((pid = waitpid(-1, NULL, WNOHANG) > 0))

	Db_CloseDyna(blogres);
	Db_CloseDyna(dbres);

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
int main(int argc, char**argv)
{
	DBase *db;
	DBase *db_blog;
	time_t tm_cur;
	struct tm *ptm_cur;
	char cha_user[128];
	char cha_domain[128];
	char cha_year[5];
	char cha_month[5];
	char cha_day[5];
	char cha_host[256];
	char cha_db[256];
	char cha_target[256];
	char cha_username[256];
	char cha_password[256];
	char ch;
	int in_error;

	cha_domain[0] = '\0';
	cha_user[0] = '\0';
	cha_target[0] = '\0';
	cha_year[0] = '\0';
	cha_month[0] = '\0';
	cha_day[0] = '\0';
	while ((ch = getopt(argc, argv, "b:d:h:m:o:p:r:s:t:u:w:y:?")) != (char)-1) {
		switch (ch) {
		case 'b':
			if (strlen(optarg) < sizeof(g_cha_db_name)) {
				strcpy(g_cha_db_name, optarg);
			}
			break;
		case 'd':
			in_error = atoi(optarg);
			if (in_error >= 1 && in_error <= 31) {
				sprintf(cha_day, "%02d", in_error);
			}
			break;
		case 'h':
			if (strlen(optarg) < sizeof(g_cha_db_host)) {
				strcpy(g_cha_db_host, optarg);
			}
			break;
		case 'm':
			in_error = atoi(optarg);
			if (in_error >= 1 && in_error <= 12) {
				sprintf(cha_month, "%02d", in_error);
			}
			break;
		case 'o':
			if (strlen(optarg) < sizeof(cha_domain)) {
				strcpy(cha_domain, optarg);
			}
			break;
		case 'p':
			if (strlen(optarg) < sizeof(g_cha_db_pass)) {
				strcpy(g_cha_db_pass, optarg);
			}
			break;
		case 'r':
			if (strlen(optarg) < sizeof(g_cha_domuser)) {
				strcpy(g_cha_domuser, optarg);
			}
			break;
		case 's':
			if (strlen(optarg) < sizeof(cha_user)) {
				strcpy(cha_user, optarg);
			}
			break;
		case 't':
			if (strlen(optarg) < sizeof(cha_target)) {
				strcpy(cha_target, optarg);
			}
			break;
		case 'u':
			if (strlen(optarg) < sizeof(g_cha_db_user)) {
				strcpy(g_cha_db_user, optarg);
			}
			break;
		case 'w':
			if (strlen(optarg) < sizeof(g_cha_dompass)) {
				strcpy(g_cha_dompass, optarg);
			}
			break;
		case 'y':
			in_error = atoi(optarg);
			if (in_error >= 1900 && in_error <= 3000) {
				sprintf(cha_year, "%04d", in_error);
			}
			break;
		case '?':
			printf("Usage: seoreport -o DOMAIN -r USER -w PASS [-y YYYY] [-m MM] [-d dd] [-s TARGETDB] [-h DBHOST] [-u DBUSER] [-p DBPASS] [-b SOURCEDB] [-t TARGET]\n");
			return 0;
		}
	}

	openlog("seoreport", LOG_PID, LOG_LOCAL7);

	tm_cur = time(NULL);
	ptm_cur = localtime(&tm_cur);
	g_in_current_year = ptm_cur->tm_year + 1900;
	g_in_current_month = ptm_cur->tm_mon + 1;
	g_in_current_day = ptm_cur->tm_mday;

	in_error = 0;
	if (!cha_domain[0]) {
		syslog(LOG_NOTICE, "domain was not specified.");
		++in_error;
	}
	if (!cha_year[0]) {
		sprintf(cha_year, "%d", ptm_cur->tm_year + 1900);
	}
	if (!cha_month[0]) {
		sprintf(cha_month, "%d", ptm_cur->tm_mon + 1);
	}
	if (!cha_month[0]) {
		sprintf(cha_day, "%d", ptm_cur->tm_mday);
	}
	if (in_error)
		return in_error;

	if (!cha_target[0]) {
		strcpy(cha_target, cha_domain);
	}

	g_in_search_year = atoi(cha_year);
	g_in_search_month = atoi(cha_month);
	g_in_search_day = atoi(cha_day);
	strcpy(g_cha_domain_ptn1, ".");
	strcat(g_cha_domain_ptn1, cha_target);
	strcpy(g_cha_domain_ptn2, "/");
	strcat(g_cha_domain_ptn2, cha_target);
	strcpy(g_cha_domain_ptn3, "www.");
	strcat(g_cha_domain_ptn3, cha_target);

	if (!cha_user[0]) {
		Domain_To_User(cha_domain, cha_user);
		if (!cha_user[0]) {
			syslog(LOG_NOTICE, "domain '%s' was not found.", cha_domain);
			++in_error;
			return in_error;
		}
		in_error = read_pass_dat(cha_user, cha_host, cha_db, cha_username, cha_password);
		if (in_error) {
			closelog();
			return in_error;
		}
	} else {
		strcpy(cha_db, cha_user);
		strcpy(cha_host, g_cha_db_host);
		strcpy(cha_username, g_cha_db_user);
		strcpy(cha_password, g_cha_db_pass);
	}
	db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	if(!db){
		syslog(LOG_NOTICE, "can not to connect to %s(%s)", cha_db, Gcha_last_error);
		closelog();
		return 1;
	}
	if (Create_Table(db)) {
		return 1;
	}
	db_blog = Db_ConnectWithParam(cha_host, g_cha_db_name, g_cha_db_user, g_cha_db_pass);
	if(!db_blog){
		syslog(LOG_NOTICE, "can not to connect to %s(%s)", cha_db, Gcha_last_error);
		Db_Disconnect(db);
		closelog();
		return 1;
	}

	if (Begin_Transact(db)) {
		syslog(LOG_NOTICE, "failed to begin transaction(%s)", Gcha_last_error);
		Db_Disconnect(db);
		Db_Disconnect(db_blog);
		closelog();
		return 1;
	}

	Sales_Ranking_Report(db);
	Whole_Sales_Report(db);
	Big_Five_Report(db);
	Access_Report(db, db_blog);
	Seo_Report(db, db_blog);

	if (Commit_Transact(db)) {
		syslog(LOG_NOTICE, "failed to commit transaction(%s)", Gcha_last_error);
	}

	Db_Disconnect(db);
	Db_Disconnect(db_blog);
	closelog();

	return 0;
}
