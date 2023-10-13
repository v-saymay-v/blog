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
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: tb.c,v 1.64 2009/02/24 04:37:19 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <libcgi2.h>
#include <excart.h>
#include <libexcart.h>
#include "libnkf.h"
#include "libcgi2.h"
#include "libnet.h"
#include "libcommon2.h"
#include "libhttp.h"
#include "libauth.h"
#include "libblog.h"

//#define CO_TRACKBACK_INTERVAL 60
//#define CO_TRACKBACK_COUNT 2

/*
+* ------------------------------------------------------------------------
 * Function:	 	page_out_xml()
 * Description:
 *	xmlとしてページアウト
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int page_out_xml(NLIST *nlp_name, char *chp_skeleton)
{
	char *chp_exp;

	printf("Content-type: text/xml;charset=euc-jp\n\n");
	chp_exp = Page_Out_Mem(nlp_name, chp_skeleton);
	if (chp_exp) {
		printf("%s", chp_exp);
		free(chp_exp);
	} else {
		chp_exp = Get_Nlist(nlp_name, "MSG", 1);
		if (chp_exp) {
			printf("%s", chp_exp);
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		make_tblist_for_xml()
 * Description:
 *	受信したトラックバックの一覧を発行
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int make_tblist_for_xml(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,int in_entry_id       /* エントリID */
	,int in_blog
)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_sql[1024];
	char cha_path[1024];
	char cha_str[16384];
	int in_count;
	int i;

	chp_tmp = getenv("SERVER_NAME");
	if(!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "MSG", "Can't get SERVER_NAME.");
		fprintf(stderr, "Can't get SERVER_NAME.(%d)\n", __LINE__);
		return 1;
	}
	if (g_in_short_name) {
		sprintf(cha_path + strlen(cha_path), "%s%s%s/%08d/?eid=%d", g_cha_protocol, chp_tmp, g_cha_base_location, in_blog, in_entry_id);
	} else {
		sprintf(cha_path + strlen(cha_path), "%s%s%s/%s?eid=%d&bid=%d", g_cha_protocol, chp_tmp, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_entry_id, in_blog);
	}
	Put_Nlist(nlp_out, "LINK", cha_path);
	/* エントリー情報を得る */
	strcpy(cha_sql, "select T1.c_entry_title");	/* 0 タイトル */
	strcat(cha_sql, ", T1.c_entry_summary");	/* 1 概要 */
	strcat(cha_sql, ", T1.c_entry_body");		/* 2 本文 */
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "MSG", "Failed query to get information of requested entry. ");
		Put_Nlist(nlp_out, "MSG", Gcha_last_error);
		fprintf(stderr, "Failed query to get information of requested entry.\n%s(%d)\n", Gcha_last_error, __LINE__);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp && *chp_tmp) {
		chp_escape = Remove_HtmlTags(chp_tmp);
		Put_Nlist(nlp_out, "TITLE", chp_escape);
		free(chp_escape);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if(chp_tmp && *chp_tmp) {
		chp_escape = Remove_HtmlTags(chp_tmp);
		Put_Nlist(nlp_out, "DESCRIPTION", chp_escape);
		free(chp_escape);
	} else {
		chp_tmp = Db_GetValue(dbres, 0, 2);
		if(chp_tmp && *chp_tmp) {
			chp_tmp2 = Remove_HtmlTags(chp_tmp);
			chp_escape = Chop_String(chp_tmp2, CO_MAXLEN_ENTRY_SUMMARY);
			Put_Nlist(nlp_out, "DESCRIPTION", chp_escape);
			free(chp_tmp2);
			free(chp_escape);
		}
	}
	Db_CloseDyna(dbres);
	/* エントリーに属するトラックバックの情報を得る */
	strcpy(cha_sql, "select T1.c_tb_title");	/* 0 TBタイトル */
	strcat(cha_sql, ", T1.c_tb_url");		/* 1 TBURL */
	strcat(cha_sql, ", T1.c_tb_excerpt");		/* 2 TB概要 */
	strcat(cha_sql, " from at_trackback T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_tb_create_time asc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "MSG", "Failed query to get information of trackbacks. ");
		Put_Nlist(nlp_out, "MSG", Gcha_last_error);
		fprintf(stderr, "Failed query to get information of trackbacks.\n%s(%d)\n", Gcha_last_error, __LINE__);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	if(!in_count) {
		Db_CloseDyna(dbres);
		return 0;
	}
	for(i = 0; i < in_count; i++) {
		memset(cha_str, '\0', sizeof(cha_str));
		strcpy(cha_str, "<item>\n");
		strcat(cha_str, "<title>");
		chp_tmp = Db_GetValue(dbres, i, 0);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Remove_HtmlTags(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		}
		strcat(cha_str, "</title>\n");
		strcat(cha_str, "<link>\n");
		chp_tmp = Db_GetValue(dbres, i, 1);
		if(chp_tmp && *chp_tmp) {
			strcat(cha_str, chp_tmp);
		}
		strcat(cha_str, "\n</link>\n");
		strcat(cha_str, "<description>\n");
		chp_tmp = Db_GetValue(dbres, i, 2);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Remove_HtmlTags(chp_tmp);
			strcat(cha_str, chp_escape);
			free(chp_escape);
		}
		strcat(cha_str, "\n</description>\n");
		strcat(cha_str, "</item>\n");
		Put_Nlist(nlp_out, "ITEMS", cha_str);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	tb_insert_db()
 * Description:
 *	トラックバック元の内容をデータベースに流す。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int tb_insert_db(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_entry_id, int in_blog)
{
	DBRes *dbres;
	CP_NKF nkf;
	NKF_CODE cd;
	char *chp_tmp;
	char *chp_ip;
	char *chp_escape;
	char *chp_title;
	char *chp_blog_name;
	char *chp_excerpt;
	char *chp_params;
	char cha_title[512];
	char cha_blog_name[512];
	char cha_url[512];
	char cha_excerpt[512];
	char cha_sql[512];
	char cha_subject[1024];
	char cha_system[256];
	char cha_sender[256];
	char cha_host[256];
	char cha_tmp[1024];
	int in_match;
	int in_filter;
	int in_accept;
	int in_trackback;
	int in_link_blog;
	int in_link_entry;
	int in_rt;
	int i;

	chp_tmp = Get_Nlist(nlp_in, "title", 1);
	if(chp_tmp && *chp_tmp) {
		strcpy(cha_title, chp_tmp);
	} else {
		strcpy(cha_title, "(タイトルなし)");
	}
	chp_tmp = Get_Nlist(nlp_in, "blog_name", 1);
	if(chp_tmp && *chp_tmp) {
		strcpy(cha_blog_name, chp_tmp);
	} else {
		strcpy(cha_blog_name, "(タイトルなし)");
	}
	chp_tmp = Get_Nlist(nlp_in, "excerpt", 1);
	if(chp_tmp && *chp_tmp) {
		strcpy(cha_excerpt, chp_tmp);
	} else {
		strcpy(cha_excerpt, "概要なし");
	}
	chp_tmp = Get_Nlist(nlp_in, "url", 1);
	if(!chp_tmp || !*chp_tmp){
		Put_Nlist(nlp_out, "MSG", "Can't find URL.");
		fprintf(stderr, "Can't find URL.(%d)\n", __LINE__);
		return 1;
	} else {
		strcpy(cha_url, chp_tmp);
	}

	nkf = Create_Nkf();
	Set_Input_String(nkf, cha_title);
	Set_Output_Code(nkf, NKF_EUC);
	cd = Convert_To_String(nkf, &chp_title);
	Destroy_Nkf_Engine(nkf);

	nkf = Create_Nkf();
	Set_Input_String(nkf, cha_blog_name);
	Set_Output_Code(nkf, NKF_EUC);
	cd = Convert_To_String(nkf, &chp_blog_name);
	Destroy_Nkf_Engine(nkf);

	nkf = Create_Nkf();
	Set_Input_String(nkf, cha_excerpt);
	Set_Output_Code(nkf, NKF_EUC);
	cd = Convert_To_String(nkf, &chp_excerpt);
	Destroy_Nkf_Engine(nkf);

	sprintf(cha_sql, "select T1.b_trackback from at_entry T1 where T1.n_blog_id = %d and T1.n_entry_id = %d", in_blog, in_entry_id);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Format_Nlist(nlp_out, "MSG", "Query failed to insert new trackback. (%s)", Gcha_last_error);
		fprintf(stderr, "Query failed to insert new trackback.\n%s(%d)\n", Gcha_last_error, __LINE__);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_accept = atoi(chp_tmp);
	} else {
		in_accept = 0;
	}
	Db_CloseDyna(dbres);

	if(Begin_Transact(db)) {
		Put_Format_Nlist(nlp_out, "MSG", "Failed to start transaction.(%s)", Gcha_last_error);
		fprintf(stderr, "Failed to start transaction.\n%s(%d)\n", Gcha_last_error, __LINE__);
		return 1;
	}
	sprintf(cha_sql, "select coalesce(max(T1.n_tb_id), 0) + 1 from at_trackback T1 where T1.n_blog_id = %d for update", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "MSG", "Query failed to insert new trackback.  ");
		Put_Nlist(nlp_out, "MSG", Gcha_last_error);
		fprintf(stderr, "Query failed to insert new trackback.\n%s(%d)\n", Gcha_last_error, __LINE__);
		Rollback_Transact(db);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_trackback = atoi(chp_tmp);
	} else {
		in_trackback = 1;
	}
	Db_CloseDyna(dbres);

	strcpy(cha_sql, "insert into at_trackback");
	strcat(cha_sql, "(n_blog_id");
	strcat(cha_sql, ",n_tb_id");
	strcat(cha_sql, ",n_entry_id");
	strcat(cha_sql, ",c_tb_title");
	strcat(cha_sql, ",c_tb_excerpt");
	strcat(cha_sql, ",c_tb_blog_name");
	strcat(cha_sql, ",c_tb_url");
	strcat(cha_sql, ",c_tb_ip");
	strcat(cha_sql, ",d_tb_create_time");
	if (in_accept == 2) {
		strcat(cha_sql, ",b_tb_accept");
	}
	sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d, %d", in_blog, in_trackback, in_entry_id);
	strcat(cha_sql, ", '");
	chp_escape = My_Escape_SqlString(db, chp_title);
	strcat(cha_sql, chp_escape);
	free(chp_escape);
	strcat(cha_sql, "', '");
	chp_escape = My_Escape_SqlString(db, chp_excerpt);
	strcat(cha_sql, chp_escape);
	free(chp_escape);
	strcat(cha_sql, "', '");
	chp_escape = My_Escape_SqlString(db, chp_blog_name);
	strcat(cha_sql, chp_escape);
	free(chp_escape);
	strcat(cha_sql, "', '");
	chp_escape = My_Escape_SqlString(db, cha_url);
	strcat(cha_sql, chp_escape);
	free(chp_escape);
	strcat(cha_sql, "', '");
	if (g_in_need_login) {
		in_link_blog = 0;
		in_link_entry = 0;
		if (g_in_dbb_mode) {
			char cha_cgi[512];
			char cha_blog_id[128] = {0};
			sprintf(cha_cgi, "%s%s%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location);
			chp_tmp = strchr(cha_url + strlen(cha_cgi), '/');
			if (chp_tmp) {
				in_rt = chp_tmp - (cha_url + strlen(cha_cgi));
				strncpy(cha_blog_id, cha_url + strlen(cha_cgi), in_rt);
				cha_blog_id[in_rt] = '\0';
				if (strlen(cha_blog_id) != 12) {
					cha_blog_id[0] = '\0';
				}
			}
			if (!cha_blog_id[0]) {
				chp_tmp = strstr(cha_url, "bid=");
				if (chp_tmp) {
					chp_tmp += strlen("bid=");
					strcpy(cha_blog_id, chp_tmp);
				}
			}
			if (cha_blog_id[0]) {
				in_rt = Temp_To_Blog(db, nlp_out, cha_blog_id);
				if (in_rt > 0) {
					in_link_blog = in_rt;
				}
			}
		} else {
			sprintf(cha_tmp, "%s%s%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location);
			strcat(cha_tmp, "%08d/?eid=%d");
			if (sscanf(cha_url, cha_tmp, &in_link_blog, &in_link_entry) != 2) {
				sprintf(cha_tmp, "%s%s%s/%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML);
				strcat(cha_tmp, "?eid=%d&bid=%d");
				sscanf(cha_url, cha_tmp, &in_link_entry, &in_link_blog);
			}
		}
		sprintf(cha_tmp, "%d", in_link_blog);
		strcat(cha_sql, cha_tmp);
	} else {
		chp_ip = getenv("REMOTE_ADDR");
		if(chp_ip && *chp_ip) {
			chp_escape = My_Escape_SqlString(db, chp_ip);
			strcat(cha_sql, chp_escape);
			free(chp_escape);
		}
	}
	strcat(cha_sql, "', now()");
	if (in_accept == 2) {
		strcat(cha_sql, ", 0");
	}
	strcat(cha_sql, ")");
	free(chp_title);
	free(chp_blog_name);
	free(chp_excerpt);
	in_rt = Db_ExecSql(db, cha_sql);
	if (in_rt != CO_SQL_OK) {
		Put_Nlist(nlp_out, "MSG", "Failed query to register. ");
		Put_Nlist(nlp_out, "MSG", Gcha_last_error);
		fprintf(stderr, "Failed query to register.\n%s(%d)\n", Gcha_last_error, __LINE__);
		return 1;
	}
	in_match = 0;
	in_filter = 0;
	sprintf(cha_sql, "select n_filter_id, c_filter from at_trackback_filter where b_valid != 0 and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(dbres) {
		for (i = 0; i < Db_GetRowCount(dbres); ++i) {
			chp_tmp = Db_GetValue(dbres, i, 0);
			in_filter = atoi(chp_tmp ? chp_tmp : "0");
			if (in_filter) {
				chp_tmp = Db_GetValue(dbres, i, 1);
				if (in_filter == CO_ENTRY_FILTER || (chp_tmp && chp_tmp[0])) {
					switch (in_filter) {
					case CO_URL_FILTER:
						in_match = Match_Filter(cha_url, chp_tmp);
						break;
					case CO_BLOG_FILTER:
						in_match = Match_Filter(chp_blog_name, chp_tmp);
						break;
					case CO_CONTENTS_FILTER:
						in_match = (Match_Filter(chp_title, chp_tmp) || Match_Filter(chp_excerpt, chp_tmp));
						break;
					case CO_ENTRY_FILTER:
						in_match = 1;
						cha_host[0] = '\0';
						strcpy(cha_sql, cha_url);
						if (strncmp(cha_sql, "http://", strlen("http://")) == 0) {
							sscanf(cha_sql, "http://%[^/]%*s", cha_host);
						} else if (strncmp(cha_sql, "https://", strlen("https://")) == 0){
							sscanf(cha_sql, "https://%[^/]%*s", cha_host);
						}
						if (cha_host[0]) {
							strcpy(cha_sql, cha_host);
							strcat(cha_sql, "/");
							chp_tmp = strstr(cha_url, cha_sql);
							if (chp_tmp) {
								chp_tmp += strlen(cha_host);
							} else {
								chp_tmp = cha_url;
							}
							chp_params = strchr(chp_tmp, '?');
							if (chp_params) {
								*chp_params = '\0';
								++chp_params;
							} else {
								chp_params = "";
							}
							in_rt = Http_Get_Get_Result(cha_host, chp_tmp, chp_params, NULL, &chp_escape);
							if (!in_rt) {
								if (g_in_short_name) {
									sprintf(cha_sql, "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
								} else {
									sprintf(cha_sql, "%s%s%s/%s?bid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
								}
								in_match = (Match_Filter(chp_escape, cha_sql) == 0);
								free(chp_escape);
							}
						}
						break;
					}
				}
				if (in_match)
					break;
			}
		}
		Db_CloseDyna(dbres);
	} else {
		fprintf(stderr, "Failed query to get filter string.\n%s(%d)\n", Gcha_last_error, __LINE__);
	}
	if (in_match) {
		sprintf(cha_sql, "update at_trackback set b_tb_accept = 0, n_tb_filter = %d where n_blog_id = %d and n_entry_id = %d and n_tb_id = %d", in_filter, in_blog, in_entry_id, in_trackback);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "MSG", "Query failed to update new trackback.  ");
			Put_Nlist(nlp_out, "MSG", Gcha_last_error);
			fprintf(stderr, "Query failed to update new trackback.\n%s(%d)\n", Gcha_last_error, __LINE__);
			Rollback_Transact(db);
			return 1;
		}
	}
	if(Commit_Transact(db)) {
		Put_Nlist(nlp_out, "MSG", "Query failed to commit.  ");
		Put_Nlist(nlp_out, "MSG", Gcha_last_error);
		fprintf(stderr, "Query failed to commit.\n%s(%d)\n", Gcha_last_error, __LINE__);
		Rollback_Transact(db);
		return 1;
	}

	dbres = Db_OpenDyna(db, "select c_systemname,c_managermail from sy_baseinfo");
	if(dbres) {
		strcpy(cha_system, Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "blog");
		strcpy(cha_sender, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "manager@asj.ad.jp");
		Db_CloseDyna(dbres);
	}
	sprintf(cha_sql, "select c_author_mail from at_profile where b_send_mail != 0 and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(dbres) {
		char *chp_author_mail;
		chp_author_mail = Db_GetValue(dbres, 0, 0);
		if (chp_author_mail) {
			char *chp_server;
			chp_server = getenv("SERVER_NAME");
			if(chp_server && *chp_server) {
				sprintf(cha_subject, "[%s]トラックバックが追加されました", cha_system);
				if (g_in_short_name) {
					asprintf(&chp_tmp, "トラックバックが追加されました。下記URLで確認できます。\n\n%s%s%s/%08d/?eid=%d#tb%d\n"
						, g_cha_protocol, chp_server, g_cha_base_location, in_blog, in_entry_id, in_trackback);
				} else {
					asprintf(&chp_tmp, "トラックバックが追加されました。下記URLで確認できます。\n\n%s%s%s/%s?bid=%d&eid=%d#tb%d\n"
						, g_cha_protocol, chp_server, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, in_entry_id, in_trackback);
				}
				Send_Mail(cha_sender, chp_author_mail, cha_subject, chp_tmp);
				free(chp_tmp);
			}
		}
		Db_CloseDyna(dbres);
	} else {
		fprintf(stderr, "Query failed to get author mail.\n%s(%d)\n", Gcha_last_error, __LINE__);
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
int main(void)
{
	NLIST *nlp_in;
	NLIST *nlp_out;
	DBase *db;
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char cha_msg[512];
	char cha_key[2048];
	char *chp_tmp;
	char *chp_pathinfo;
	int in_blog;
	int in_entry_id;
	int in_mode;
	int in_error;
	int in_black_list = 0;

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();

	in_mode = 0;
	in_error = 0;

	db = NULL;
	in_error = Read_Blog_Ini(cha_host,cha_db, cha_username, cha_password, &g_in_ssl_mode);
	if (in_error) {
		Put_Format_Nlist(nlp_out, "MSG", "Failed to read INI file.(%s)", strerror(in_error));
		fprintf(stderr, "Failed to read INI file.\n%s(%d)\n", strerror(in_error), __LINE__);
		goto page_out_return;
	}
	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		Put_Nlist(nlp_out, "MSG", "Failed to connect database.");
		fprintf(stderr, "Failed to connect database.\n%s(%d)\n", Gcha_last_error, __LINE__);
		goto page_out_return;
	}
	if (Read_Blog_Option(db)) {
		Put_Nlist(nlp_out, "MSG", "Failed to read blog options.");
		fprintf(stderr, "Failed to read blog options.\n%s(%d)\n", Gcha_last_error, __LINE__);
		goto page_out_return;
	}
	Set_SkelPath(g_cha_admin_skeleton);

	/* エントリーIDの取り出し */
	chp_pathinfo = getenv("PATH_INFO");
	if(!chp_pathinfo) {
		Put_Nlist(nlp_out, "MSG", "Can't exec without parameter.");
		fprintf(stderr, "Can't exec without parameter.(%d)\n", __LINE__);
		in_error++;
	} else if (!Check_RBL()) {
		cha_key[0] = '\0';
		if(sscanf(chp_pathinfo, "/%d-%d-%s", &in_blog, &in_entry_id, cha_key) != 3) {
			if(sscanf(chp_pathinfo, "/%d-%d", &in_blog, &in_entry_id) != 2) {
				Put_Format_Nlist(nlp_out, "MSG", "Path info is not correct.(%d)", __LINE__);
				fprintf(stderr, "Path info is not correct.(%d)\n", __LINE__);
				in_error++;
			}
		}
		if(!in_error && Exist_Entry_Id(db, nlp_out, in_entry_id, in_blog) == 0) {
			Put_Format_Nlist(nlp_out, "MSG", "Entry doesn't exist.(%d)", __LINE__);
			fprintf(stderr, "Entry doesn't exist.(%d)\n", __LINE__);
			in_error++;
		}
		if(!in_error && Get_Flag_Public(db, nlp_out, in_entry_id, in_blog) != 1) {
			Put_Format_Nlist(nlp_out, "MSG", "Entry doesn't exist.(%d)", __LINE__);
			fprintf(stderr, "Entry doesn't exist.(%d)\n", __LINE__);
			in_error++;
		}
		g_in_need_login = Need_Login(db, in_blog);
		if (g_in_need_login) {
			g_in_login_owner = Get_Nickname(db, nlp_out, cha_msg, NULL, cha_key);
			if (g_in_login_owner <= 0) {
				Put_Format_Nlist(nlp_out, "MSG", "Entry doesn't exist.(%d)", __LINE__);
				fprintf(stderr, "Entry doesn't exist.(%d)\n", __LINE__);
				in_error++;
			}
		}
		in_black_list = (g_in_dbb_mode && In_Black_List(db, nlp_out, 0, cha_key));
		if (in_black_list) {
			Put_Format_Nlist(nlp_out, "MSG", "Entry doesn't exist.(%d)", __LINE__);
			fprintf(stderr, "Entry doesn't exist.(%d)\n", __LINE__);
			in_error++;
		}
		if(in_error == 0) {
			/* 受信モードor送信モード */
			/* GETメソッドで、"__mode=rss"があるときのみping一覧受信 */
			chp_tmp = getenv("QUERY_STRING");
			if(chp_tmp && *chp_tmp) {
				if(strcmp(chp_tmp, "__mode=rss") == 0) {
					if(!in_error) {
						in_mode = 1;
						if(make_tblist_for_xml(db, nlp_in, nlp_out, in_entry_id, in_blog)) {
							in_error++;
						}
					}
				} else {
					Put_Nlist(nlp_out, "MSG", "Unknown request.");
					in_error++;
				}
			} else {
				if(Get_Flag_Acceptable_Trackback(db, nlp_out, in_entry_id, in_blog) == 0) {
					Put_Nlist(nlp_out, "MSG", "You can't trackback this article.");
					fprintf(stderr, "You can't trackback this article.(%d)\n", __LINE__);
					in_error++;
				}
				if(!in_error && trackback_ip_check(db, nlp_out, in_entry_id, in_blog, g_in_login_owner)) {
					in_error++;
				}
				/* TrackBack Pingをデータベース(at_tb)へ格納 */
				in_mode = 0;
				if(in_error == 0) {
					if(tb_insert_db(db, nlp_in, nlp_out, in_entry_id, in_blog)) {
						in_error++;
					} else {
						sprintf(cha_msg, "Thanks for your trackback ping for entry %d", in_entry_id);
						Put_Nlist(nlp_out, "MSG", cha_msg);
					}
				}
			}
		}
	} else {
		Put_Nlist(nlp_out, "MSG", "RBL check NG.");
		fprintf(stderr, "RBL check NG.(%d)\n", __LINE__);
		in_error++;
	}

page_out_return:
	if(in_error) {
		Put_Nlist(nlp_out, "ERROR_NO", "1");
	} else {
		Put_Nlist(nlp_out, "ERROR_NO", "0");
	}
	if(in_mode == 0 || in_error != 0) {
		page_out_xml(nlp_out, CO_SKEL_TB_RESPONSE);
	} else {
		page_out_xml(nlp_out, CO_SKEL_TBLIST_RESPONSE);
	}
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	if (db)
		Db_Disconnect(db);

	return in_error;
}
