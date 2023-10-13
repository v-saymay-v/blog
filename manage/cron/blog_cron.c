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
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_cron.c,v 1.18 2008/01/24 03:24:50 hori Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include "libcommon2.h"
#include "libblog.h"
#include "blog.h"

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

static int g_in_ssl_mode;
static char g_cha_host[CO_MAX_HOST];
static char g_cha_db[CO_MAX_DB];
static char g_cha_username[CO_MAX_USER];
static char g_cha_password[CO_MAX_PASS];

int Cron_Proc()
{
	DBase *db;
	DBRes *dbres;
	char cha_start[256];
	char cha_lastday[256];
	char cha_sql[1024];
	char *chp_tmp;
	int in_cnt;
	int in_blog;

	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(g_cha_host, g_cha_db, g_cha_username, g_cha_password);
	} else {
		db = Db_ConnectWithParam(g_cha_host, g_cha_db, g_cha_username, g_cha_password);
	}
	if(!db){
		printf("データベース接続に失敗しました。");
		return 1;
	}
	if (Read_Blog_Option(db)) {
		printf("ブログオプションの読み込みに失敗しました。");
		Db_Disconnect(db);
		return 1;
	}
	if (Begin_Transact(db)) {
		printf("Beginning transaction failed.(%s)", Gcha_last_error);
		Db_Disconnect(db);
		return 1;
	}

	Get_NowDate(cha_start);
	Get_PrevDate(cha_start, 1, cha_lastday);
	cha_start[4] = '-';
	cha_start[7] = '-';
	strcat(cha_start, " 00:00:00");
	cha_lastday[4] = '-';
	cha_lastday[7] = '-';

	sprintf(cha_sql, "select n_blog_id,count(*) from at_view where d_view < '%s' group by n_blog_id", cha_start);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}
	while (Db_FetchNext(dbres) == CO_SQL_OK) {
		chp_tmp = Db_FetchValue(dbres, 0);
		if (chp_tmp) {
			in_blog = atoi(chp_tmp);
			chp_tmp = Db_FetchValue(dbres, 1);
			if (chp_tmp) {
				in_cnt = atoi(chp_tmp);
			} else {
				in_cnt = 0;
			}
			sprintf(cha_sql, "update at_profile set n_page_view = n_page_view + %d where n_blog_id = %d", in_cnt, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				printf("Query failed.(%s)", Gcha_last_error);
				Db_CloseDyna(dbres);
				Rollback_Transact(db);
				Db_Disconnect(db);
				return 1;
			}
		}
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select n_blog_id,count(*) from at_view where b_valid != 0 and d_view < '%s' group by n_blog_id", cha_start);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}
	while (Db_FetchNext(dbres) == CO_SQL_OK) {
		chp_tmp = Db_FetchValue(dbres, 0);
		if (chp_tmp) {
			in_blog = atoi(chp_tmp);
			chp_tmp = Db_FetchValue(dbres, 1);
			if (chp_tmp) {
				in_cnt = atoi(chp_tmp);
			} else {
				in_cnt = 0;
			}
			sprintf(cha_sql, "update at_profile set n_unique_user = n_unique_user + %d where n_blog_id = %d", in_cnt, in_blog);
			if (Db_ExecSql(db, cha_sql)) {
				printf("Query failed.(%s)", Gcha_last_error);
				Db_CloseDyna(dbres);
				Rollback_Transact(db);
				Db_Disconnect(db);
				return 1;
			}
			sprintf(cha_sql, "insert into at_access (n_blog_id,d_date,n_count) values (%d,'%s',%d)", in_blog, cha_lastday, in_cnt);
			if (Db_ExecSql(db, cha_sql)) {
				printf("Query failed.(%s)", Gcha_last_error);
				Db_CloseDyna(dbres);
				Rollback_Transact(db);
				Db_Disconnect(db);
				return 1;
			}
		}
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "delete from at_view where d_view < '%s'", cha_start);
	if (Db_ExecSql(db, cha_sql)) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}
/*
	Get_NowDate(cha_sql);
	Get_PrevDate(cha_sql, 8, cha_lastday);
	cha_lastday[4] = '-';
	cha_lastday[7] = '-';
	sprintf(cha_sql, "delete from at_access where d_date <= '%s'", cha_lastday);
	if (Db_ExecSql(db, cha_sql)) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}
*/
	Get_NowDate(cha_sql);
	Get_PrevDate(cha_sql, 8, cha_lastday);
	cha_lastday[4] = '-';
	cha_lastday[7] = '-';
	sprintf(cha_sql, "delete from at_keyword_appear where d_appear <= '%s'", cha_lastday);
	if (Db_ExecSql(db, cha_sql)) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}

	Get_NowDate(cha_sql);
	if (strcmp(cha_sql + 8, "01") == 0) {
		sprintf(cha_sql, "update at_profile set n_page_view_all = n_page_view_all + n_page_view");
		if (Db_ExecSql(db, cha_sql)) {
			printf("Query failed.(%s)", Gcha_last_error);
			Rollback_Transact(db);
			Db_Disconnect(db);
			return 1;
		}
		sprintf(cha_sql, "update at_profile set n_page_view = 0");
		if (Db_ExecSql(db, cha_sql)) {
			printf("Query failed.(%s)", Gcha_last_error);
			Rollback_Transact(db);
			Db_Disconnect(db);
			return 1;
		}
		sprintf(cha_sql, "update at_profile set n_unique_user_all = n_unique_user_all + n_unique_user");
		if (Db_ExecSql(db, cha_sql)) {
			printf("Query failed.(%s)", Gcha_last_error);
			Rollback_Transact(db);
			Db_Disconnect(db);
			return 1;
		}
		sprintf(cha_sql, "update at_profile set n_unique_user = 0");
		if (Db_ExecSql(db, cha_sql)) {
			printf("Query failed.(%s)", Gcha_last_error);
			Rollback_Transact(db);
			Db_Disconnect(db);
			return 1;
		}
	}

	sprintf(cha_sql, "drop table at_access_ranking");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}

	sprintf(cha_sql, "create table at_access_ranking (n_seq int auto_increment primary key,n_blog_id int,n_count bigint,n_entry bigint)TYPE = InnoDB;");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}
	sprintf(cha_sql,
		" insert into at_access_ranking"
		" select"
		" null"
		",T2.n_blog_id"
		",T2.n_unique_user + T2.n_unique_user_all as n_count"
		",ifnull(count(T4.n_entry_id),0) as n_entry"
		" from at_blog T1"
		",at_entry T4"
		",at_profile T2"
		" where T1.n_blog_id = T2.n_blog_id"
		" and T1.n_blog_id = T4.n_blog_id"
		" and T1.n_blog_status = 1"
		" and T1.c_blog_title <> ''"
		" and T1.c_blog_title is not null"
		" and T1.n_hbuser_id = 0"
		" group by T2.n_blog_id"
		" having n_count > 0 and n_entry > 0"
		" order by n_count desc, T1.d_create_time desc, T1.n_blog_id asc");
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}

	sprintf(cha_sql," select count(*) from at_access_ranking");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_cnt = atol(chp_tmp);
	} else {
		in_cnt = 0;
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "update at_access_valid set n_total = %d", in_cnt);
	if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
		printf("Query failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}

	if(Commit_Transact(db)) {
		printf("Commiting transaction failed.(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		return 1;
	}
	Db_Disconnect(db);

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
int main(int argc, char **argv)
{
	struct passwd* pw;
	char cha_sql[1024];
	char cha_file[1024];
	char cha_user[1024];
	char ch;
	char ch_mode;
	char *chp_tmp;
	int in_ret;
	int in_error;

	cha_user[0] = '\0';
	ch_mode = 0;
	while ((ch = getopt(argc, argv, "dchu:")) != (char)-1) {
		switch (ch) {
		case 'd':
		case 'c':
		case 'h':
			ch_mode = ch;
			break;
		case 'u':
			if (strlen(optarg) < sizeof(cha_user)) {
				strcpy(cha_user, optarg);
			}
			break;
		}
	}

	in_ret = 0;
	if (ch_mode == 'c') {
		setpassent(1);
		while ((pw = getpwent())) {
			strcpy(cha_file, pw->pw_dir);
			strcat(cha_file, "/data/as-blog/dbb_blog.ini");
			if (!Read_Ini(cha_file, g_cha_host, g_cha_db, g_cha_username, g_cha_password, &g_in_ssl_mode)) {
				Cron_Proc();
			}
		}
		endpwent();
	} else {
		strcpy(cha_sql, argv[0]);
		chp_tmp = strrchr(cha_sql, '/');
		if (chp_tmp) {
			*chp_tmp = '\0';
			chdir(chp_tmp);
		}
		if (cha_user[0]) {
			pw = getpwnam(cha_user);
			if (!pw) {
				printf("wrong user '%s'.", cha_user);
				return 1;
			}
			strcpy(cha_file, pw->pw_dir);
			strcat(cha_file, "/data/dbb_blog.ini");
			in_error = Read_Ini(cha_file, g_cha_host, g_cha_db, g_cha_username, g_cha_password, &g_in_ssl_mode);
			if (in_error) {
				printf("failed to read %s.(%s)", cha_file, strerror(in_error));
				return in_error;
			}
		} else {
			in_error = Read_Blog_Ini(g_cha_host, g_cha_db, g_cha_username, g_cha_password, &g_in_ssl_mode);
			if (in_error) {
				printf("failed to read dbb_blog.ini.(%s)", strerror(in_error));
				return in_error;
			}
		}
		in_ret = Cron_Proc();
	}

	return in_ret;
}
