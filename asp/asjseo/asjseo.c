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
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: asjseo.c,v 1.30 2011/04/01 05:44:06 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <netdb.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
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

static char g_cha_domain_ptn0[256] = {0};
static char g_cha_domain_ptn1[256] = {0};
static char g_cha_domain_ptn2[256] = {0};
static char g_cha_db_host[256] = "192.168.100.157";
static char g_cha_db_name[256] = "cart_report";
static char g_cha_db_pass[256] = "R2LQYT8XB5";
static char g_cha_db_user[256] = "root";

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
	Convert_To_String(nkf, chpp_dst);
	Destroy_Nkf_Engine(nkf);
	if (!chpp_dst) {
		asprintf(chpp_dst, "%s", chp_src);
	}
}


void debug_log(char *chp_format, ...)
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

void Search_Proc(DBase *db_blog,
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
	struct stat sb;
	char cha_number[1024];
	char cha_out[256];
	char cha_cmd[4096];
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
	int in_number;
	int in_point;
	int in_found;
	int in_count;
	static char cha_wget[] = "/usr/local/bin/wget -nv -U \"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; GTB6.6; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\" ";

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
	if (chp_chp_head2) {
		Conv_From_Euc(chp_chp_head2, chp_code, &chp_head2);
	} else {
		chp_head2 = NULL;
	}
	Conv_From_Euc(chp_chp_tail, chp_code, &chp_tail);
	if (chp_chp_tail2) {
		Conv_From_Euc(chp_chp_tail2, chp_code, &chp_tail2);
	} else {
		chp_tail2 = NULL;
	}
	Conv_From_Euc(chp_chp_line, chp_code, &chp_line);
	Conv_From_Euc(chp_chp_url_head, chp_code, &chp_url_head);
	Conv_From_Euc(chp_chp_url_tail, chp_code, &chp_url_tail);

	sprintf(cha_out, "/var/tmp/wget_%d_XXXXX", getpid());
	fd = mkstemp(cha_out);
	close(fd);

	in_found = 0;
	in_count = 0;
	cha_number[0] = '\0';
	for (k = 0; k < in_max_num/in_max; ++k) {
		sprintf(cha_real_url, chp_url, chp_esc, k * in_max + in_begin);
		strcpy(cha_cmd, cha_wget);
		if (chp_cookie && chp_cookie[0]) {
			strcat(cha_cmd, "--cookies=off --header \"Cookie: ");
			strcat(cha_cmd, chp_cookie);
			strcat(cha_cmd, "\" ");
		}
		strcat(cha_cmd, "-O ");
		strcat(cha_cmd, cha_out);
		strcat(cha_cmd, " 2>&1 ");
		strcat(cha_cmd, cha_real_url);
		pp = popen(cha_cmd, "r");
		if (!pp) {
			printf("Content-Type: text/html\n\n");
			printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
			printf("<html><body>failed to exec wget(%s)<br></body></html>\n", strerror(errno));
			remove(cha_out);
			continue;
		}
		while (fgets(cha_cmd, sizeof(cha_cmd), pp))
			;
		pclose(pp);

		if (stat(cha_out, &sb) < 0) {
			debug_log("%s - stat error\n", chp_engine);
			continue;
		}

		pp = fopen(cha_out, "r");
		if (!pp) {
			printf("Content-Type: text/html\n\n");
			printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
			printf("<html><body>failed to open wget result(%s)<br></body></html>\n", strerror(errno));
			remove(cha_out);
			continue;
		}
		chp_buf = (char*)malloc(sb.st_size + 1);
		bzero(chp_buf, sb.st_size + 1);
//fprintf(stderr, "chp_buf = %x,  size = %ld\n", chp_buf, sb.st_size);
		fread(chp_buf, sb.st_size, 1, pp);
		fclose(pp);
		if (!chp_buf) {
			debug_log("%s - memory alloc error\n", chp_engine);
			remove(cha_out);
			continue;
		}

		// 検索された件数取得
		chp_cur = chp_buf;
		sprintf(cha_head, chp_head, chp_enc);
		chp_tmp = strstr(chp_cur, cha_head);
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
//fprintf(stderr, "cha_number = %s\n", cha_number);
		for (j = 0; *chp_cur && j < in_max; ++j) {
			// 行の頭を探す
			const char *chp_obj;
			chp_obj = (j > 0 || !chp_first || !chp_first[0]) ? chp_line : chp_first;
//fprintf(stderr, "chp_cur = %x,  chp_line = %x chp_obj = %x\n", chp_cur, chp_line, chp_obj);
//fprintf(stderr, "chp_cur length = %ld\n", strlen(chp_cur));
//fprintf(stderr, "chp_line length = %ld\n", strlen(chp_line));
			chp_tmp = strstr(chp_cur, chp_obj);
			if (chp_child && chp_child[0]) {
				chp_tmp2 = strstr(chp_cur, chp_child);
				if (chp_tmp2 && chp_tmp2 < chp_tmp) {
					chp_tmp = chp_tmp2;
					chp_obj = chp_child;
				}
			}
			if (!chp_tmp) {
				debug_log("%s(%s) - %d: line head not found!!\n", chp_engine, chp_obj, chp_cur - chp_buf);
				j = in_max;
				continue;
			}
			++in_count;
			chp_cur = chp_tmp + strlen(chp_obj);
			// URLの頭を探す
			chp_tmp = strstr(chp_cur, chp_url_head);
			if (!chp_tmp) {
				debug_log("%s(%s): url head not found!!\n", chp_engine, chp_url_head);
				j = in_max;
				continue;
			}
			chp_cur = chp_tmp + strlen(chp_url_head);
			// URLの最後を探す
			chp_tmp = strstr(chp_cur, chp_url_tail);
			if (!chp_tmp) {
				debug_log("%s(%s): url tail not found!!\n", chp_engine, chp_url_tail);
				j = in_max;
				continue;
			}
			strncpy(cha_cmd, chp_cur, chp_tmp - chp_cur);
			cha_cmd[chp_tmp - chp_cur] = '\0';
			Decode_Form(cha_cmd);
			if (!strncmp(cha_cmd, g_cha_domain_ptn0, strlen(g_cha_domain_ptn0))) {
				in_found = 1;
				break;
			} else if (strstr(cha_cmd, g_cha_domain_ptn1)) {
				in_found = 1;
				break;
			} else if (strstr(cha_cmd, g_cha_domain_ptn2)) {
				in_found = 1;
				break;
			}
			chp_cur = chp_tmp + strlen(chp_url_tail);
		}
		free(chp_buf);
		remove(cha_out);
		if (in_found) {
			break;
		}
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
	in_number = atoi(cha_number);

	in_point = 0;
	if (in_found && in_count <= 20) {
		sprintf(cha_cmd, "select n_point from sy_add_point where (n_min <= %d and n_max >= %d) or (n_min <= %d and n_max is null)", in_number, in_number, in_number);
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

	printf("<tr><td>%s</td><td>%s</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td>%s<br></td></tr>\n",
		chp_keyword, chp_engine, in_count, in_number, in_point, cha_rank);

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
static int Seo_Report(DBase *db_blog, NLIST *nlp_in)
{
	DBRes *blogres;
	char *chp_tmp;
	char *chp_url;
	char *chp_code;
	char *chp_first;
	char *chp_child;
	char *chp_cookie;
	int i;
	int l;
	int in_begin;
	int in_engine;
	int in_max;
	int in_max_num;
	float fl_mult;
//	pid_t pid;

	chp_tmp = Get_Nlist(nlp_in, "maxnum", 1);
	if (chp_tmp) {
		in_max_num = atoi(chp_tmp);
	} else {
		in_max_num = 100;
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
		printf("Content-Type: text/html\n\n");
		printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
		printf("<html><body>failed to exec query(%s)<br></body></html>\n", Gcha_last_error);
		return 1;
	}

	printf("Content-Type: text/html\n\n");
	printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
	printf("<html><body>\n");
	printf("<table border=\"1\">\n");
	printf("<tr><td>キーワード</td><td>エンジン</td><td>順位</td><td>検索数</td><td>ポイント</td><td>ランク</td></tr>\n");
	for (l = 0; l < Db_GetRowCount(blogres); ++l) {
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
		for (i = 1; i <= Get_NlistCount(nlp_in, "keyword"); ++i) {
			chp_tmp = Get_Nlist(nlp_in, "keyword", i);
			if (!chp_tmp || !chp_tmp[0]) {
				continue;
			}
			Search_Proc(db_blog,
				chp_tmp,
				My_Db_FixGetValue(blogres, l, 0),
				in_engine,
				in_max,
				in_max_num,
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
		}
	}
	printf("</table>\n");
	printf("</body></html>\n");

	Db_CloseDyna(blogres);

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
	DBase *db_blog;
	NLIST *nlp_in;
	char *chp_request;
//	char cha_host[256];
	char cha_db[256];

	chp_request = getenv("REQUEST_METHOD");
	if (!chp_request) {
		printf("Content-Type: text/html\n\n");
		printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
		printf("<html><body>can not get REQUEST_METHOD<br></body></html>\n");
		return 1;
	}
	nlp_in = Init_Cgi();
	if (strcasecmp(chp_request, "GET") == 0) {
		printf("Content-Type: text/html\n\n");
		printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
		printf("<html><body><form method=\"POST\" action=\"asjseo.cgi\">\n");
		chp_request = Get_Nlist(nlp_in, "host", 1);
		if (chp_request) {
			printf("<input type=\"hidden\" name=\"host\" value=\"%s\">\n", chp_request);
		}
		printf("DOMAIN: <input type=\"text\" name=\"domain\" size=\"50\" maxlength=\"256\"><br>\n");
		printf("MAXNUM: <input type=\"text\" name=\"maxnum\" size=\"50\" maxlength=\"256\" value=\"300\"><br>\n");
		printf("KEYWORD01: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD02: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD03: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD04: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD05: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD06: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD07: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD08: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD09: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("KEYWORD10: <input type=\"text\" size=\"50\" name=\"keyword\" maxlength=\"256\"><br>\n");
		printf("DOMAIN: <input type=\"submit\" name=\"submit\" value=\"検索\"><br>\n");
		printf("</form></body></html>\n");
		Finish_Nlist(nlp_in);
		return 0;
	} else if (strcasecmp(chp_request, "POST") != 0) {
		printf("Content-Type: text/html\n\n");
		printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
		printf("<html><body>unknown request %s<br></body></html>\n", chp_request);
		Finish_Nlist(nlp_in);
		return 1;
	}

	chp_request = Get_Nlist(nlp_in, "host", 1);
	if (chp_request) {
		strcpy(g_cha_db_host, chp_request);
	}
	db_blog = Db_ConnectWithParam(g_cha_db_host, g_cha_db_name, g_cha_db_user, g_cha_db_pass);
	if(!db_blog){
		printf("Content-Type: text/html\n\n");
		printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
		printf("<html><body>can not to connect to %s(%s)<br></body></html>\n", cha_db, Gcha_last_error);
		Finish_Nlist(nlp_in);
		return 1;
	}

	chp_request = Get_Nlist(nlp_in, "domain", 1);
	if (!chp_request || !chp_request[0]) {
		printf("Content-Type: text/html\n\n");
		printf("<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;CHARSET=EUC-JP\">\n");
		printf("<html><body>domain not specified.<br></body></html>\n");
		Finish_Nlist(nlp_in);
		return 1;
	}
	strcpy(g_cha_domain_ptn0, chp_request);
	strcpy(g_cha_domain_ptn1, ".");
	strcat(g_cha_domain_ptn1, chp_request);
	strcpy(g_cha_domain_ptn2, "/");
	strcat(g_cha_domain_ptn2, chp_request);
	Seo_Report(db_blog, nlp_in);
	Finish_Nlist(nlp_in);

	Db_Disconnect(db_blog);

	return 0;
}
