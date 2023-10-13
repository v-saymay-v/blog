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
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: newblog.c,v 1.48 2010/09/27 00:35:11 hayakawa Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "libcommon2.h"
#include "libblog.h"
#include "blog.h"

extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;

static int g_in_ssl_mode = 0;
static char g_cha_vdomain_catalog[] = "/etc/vdomain.catalog";
static char g_cha_pass_dat[1024] = "/data/e-commex/pass.dat";
static char g_cha_pass_rsv[1024] = "/data/reserve/passwd.dat";
static char g_cha_user_dir[1024] = "as-blog";
static char g_cha_apache_dir[1024] = "as-blog";
static char httpdconf[1024] = "/usr/local/apache/etc/httpd.conf";
static char apachectl[1024] = "/usr/local/apache/sbin/apachectl";
static char g_cha_db_host[256] = "192.168.100.140";

int RestartAppche(const char *chp_domain, const char *chp_user)
{
	FILE* infile;
	FILE* outfile;
	mode_t oldmask;
	struct stat sb;
	char sql[1024];
	char line[128];
	char linessl[128];
	char newconf[1025];
	bool bComment = false;

	if (stat(httpdconf, &sb) < 0) {
		syslog(LOG_NOTICE, "failed to get status of %s.", httpdconf);
		return 1;
	}
	oldmask = sb.st_mode;
	infile = fopen(httpdconf, "r");
	if (!infile) {
		syslog(LOG_NOTICE, "failed to open %s.", httpdconf);
		return 1;
	}
	strcpy(newconf, httpdconf);
	strcat(newconf, ".new");
	outfile = fopen(newconf, "w");
	if (!outfile) {
		fclose(infile);
		syslog(LOG_NOTICE, "failed to create %s.", newconf);
		return 1;
	}

	bComment = false;
	sprintf(line, "<VirtualHost www.%s:80>\n", chp_domain);
	sprintf(linessl, "<VirtualHost www.%s:443>\n", chp_domain);
	while (fgets(sql, sizeof(sql), infile)) {
		if (strcasecmp(sql, "</VirtualHost>\n") == 0) {
			if (bComment) {
				fprintf(outfile, "### AS-Blog Entry Start ########################################\n");
				fprintf(outfile, "    Alias /%s/css/ /home/%s/data/%s/css/\n", g_cha_user_dir, chp_user, g_cha_user_dir);
				fprintf(outfile, "    Alias /%s/rss/ /home/%s/data/%s/rss/\n", g_cha_user_dir, chp_user, g_cha_user_dir);
				fprintf(outfile, "    Alias /%s/images/admin/ /usr/local/apache/share/%s/images/admin/\n", g_cha_user_dir, g_cha_apache_dir);
				fprintf(outfile, "    Alias /%s/images/user/ /usr/local/apache/share/%s/images/user/\n", g_cha_user_dir, g_cha_apache_dir);
				fprintf(outfile, "    Alias /%s/images/theme/ /usr/local/apache/share/%s/images/theme/\n", g_cha_user_dir, g_cha_apache_dir);
				fprintf(outfile, "    Alias /%s/images/ /usr/local/apache/share/%s/images/\n", g_cha_user_dir, g_cha_apache_dir);
				fprintf(outfile, "    Alias /%s/scripts/ /usr/local/apache/share/%s/scripts/\n", g_cha_user_dir, g_cha_apache_dir);
				fprintf(outfile, "### AS-Blog Entry End ##########################################\n");
				bComment = false;
			}
		} else if (strcasecmp(sql, line) == 0 || strcasecmp(sql, linessl) == 0) {
			bComment = true;
		}
		fputs(sql, outfile);
	}

	fclose(infile);
	fclose(outfile);

	strcpy(sql, httpdconf);
	strcat(sql, ".old");
	rename(httpdconf, sql);
	rename(newconf, httpdconf);
	chmod(httpdconf, oldmask);

	strcpy(newconf, apachectl);
	strcat(newconf, " restart");
	infile = popen(newconf, "r");
	if (!infile) {
		syslog(LOG_NOTICE, "%s - Can not restart Apache.", chp_domain);
		return 1;
	}
	while (fgets(sql, sizeof(sql), infile));
	pclose(infile);

	return 0;
}

int Write_To_Dbb_Db(DBase *db, int in_blog, char *chp_owner)
{
	DBase *authdb;
	DBRes *dbres;
	struct passwd *stp_pwd;
	char cha_sql[255];
	char cha_host[256];
	char cha_db[21];
	char cha_user[21];
	char cha_password[21];
	char cha_table[21];
	char cha_column_blog[21];
	char cha_column_owner[21];
	char cha_cookie_host[256];
	char cha_cookie_path[256];
	char cha_passfile[256];

	sprintf(cha_sql,
		"select c_host,"
		" c_db,"
		" c_user,"
		" c_password,"
		" c_table,"
		" c_column_blog,"
		" c_column_owner,"
		" c_passfile "
		" from sy_authinfo");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		return 1;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "no records for login.");
		Db_CloseDyna(dbres);
		return 1;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "");
	strcpy(cha_db, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "");
	strcpy(cha_user, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "");
	strcpy(cha_password, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "");
	strcpy(cha_table, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "");
	strcpy(cha_column_blog, Db_FetchValue(dbres, 5) ? Db_FetchValue(dbres, 5) : "");
	strcpy(cha_column_owner, Db_FetchValue(dbres, 6) ? Db_FetchValue(dbres, 6) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 7) ? Db_FetchValue(dbres, 7) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwuid(getuid());
	if (stp_pwd) {
		char cha_hostname[256];
		char *chp;
		gethostname(cha_hostname, sizeof(cha_hostname));
		Replace_String("#USER#", stp_pwd->pw_name, cha_db);
		Replace_String("#USER#", stp_pwd->pw_name, cha_user);
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
		Replace_String("#HOST#", cha_cookie_host, cha_hostname);
		chp = getenv("SCRIPT_NAME");
		if (chp) {
			strcpy(cha_hostname, chp);
			chp = strchr(cha_hostname, '/');
			if (chp) {
				++chp;
				*chp = '\0';
			}
			Replace_String("#SCRIPT#", cha_hostname, cha_cookie_path);
		}
	}
	if (strcmp(cha_password, "#FILE#") == 0) {
		FILE *fp;
		char *chp;
		fp = fopen(cha_passfile, "r");
		if (fp) {
			fread(cha_password, sizeof(cha_password), 1, fp);
			chp = strchr(cha_password, '\r');
			if (chp)
				*chp = '\0';
			chp = strchr(cha_password, '\n');
			if (chp)
				*chp = '\0';
			fclose(fp);
		}
	}

	authdb = Db_ConnectWithParam(cha_host, cha_db, cha_user, cha_password);
	if (!authdb){
		syslog(LOG_NOTICE, "failed to connect db(%s)", Gcha_last_error);
		return 1;
	}
	if(Begin_Transact(authdb)) {
		syslog(LOG_NOTICE, "failed to start transaction(%s)", Gcha_last_error);
		Db_Disconnect(authdb);
		return 1;
	}
	sprintf(cha_sql, "update %s set %s=%d where %s=%s", cha_table, cha_column_blog, in_blog, cha_column_owner, chp_owner);
	if (Db_ExecSql(authdb, cha_sql) != CO_SQL_OK) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Db_Disconnect(authdb);
		return 1;
	}
	if(Commit_Transact(authdb)) {
		syslog(LOG_NOTICE, "failed to commit transaction(%s)", Gcha_last_error);
		Rollback_Transact(authdb);
		Db_Disconnect(authdb);
		return 1;
	}
	Db_Disconnect(authdb);

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
static int read_pass_dat(char ch_mode, const char* chp_user, const char *chp_head, char* chp_host, char* chp_db, char* chp_username, char* chp_password, char *chp_dir)
{
	FILE* fp;
	struct stat sb;
	struct passwd *pwd;
	char cha_oneline[1024];

	pwd = getpwnam(chp_user);
	if (!pwd) {
		syslog(LOG_NOTICE, "user '%s' was not found.", chp_user);
		return 1;
	}
	strcpy(cha_oneline, pwd->pw_dir);
	if (ch_mode == 'c') {
		strcat(cha_oneline, g_cha_pass_dat);
	} else {
		strcat(cha_oneline, g_cha_pass_rsv);
	}
	fp = fopen(cha_oneline, "r");
	if (!fp) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		return errno;
	}
	strcpy(chp_host, g_cha_db_host);
	while (fgets(cha_oneline, sizeof(cha_oneline), fp)) {
		char *chp_name;
		char *chp_tmp = strchr(cha_oneline, '\r');
		if (chp_tmp)
			*chp_tmp = '\0';
		chp_tmp = strchr(cha_oneline, '\n');
		if (chp_tmp)
			*chp_tmp = '\0';
		chp_name = strtok(cha_oneline, ":");
		if (chp_name) {
			strcpy(chp_username, chp_name);
		}
		sprintf(chp_db, "%s%s", chp_head, chp_name);
		chp_tmp = strtok(NULL, ":");
		if (chp_tmp) {
			strcpy(chp_password, chp_tmp);
		}
		chp_tmp = strtok(NULL, ":");
		if (chp_tmp) {
			g_in_ssl_mode = atoi(chp_tmp);
		}
	}
	fclose(fp);

	sprintf(cha_oneline, "%s/data/%s", pwd->pw_dir, g_cha_user_dir);
	if (mkdir(cha_oneline, 0755) < 0 && errno != EEXIST) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		return errno;
	}
	if (chown(cha_oneline, pwd->pw_uid, pwd->pw_gid) < 0) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		rmdir(cha_oneline);
		return errno;
	}

	sprintf(cha_oneline, "%s/data/%s/css", pwd->pw_dir, g_cha_user_dir);
	if (mkdir(cha_oneline, 0755) < 0 && errno != EEXIST) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		return errno;
	}
	if (chown(cha_oneline, pwd->pw_uid, pwd->pw_gid) < 0) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		rmdir(cha_oneline);
		return errno;
	}
	strcpy(chp_dir, cha_oneline);

	sprintf(cha_oneline, "%s/data/%s/rss", pwd->pw_dir, g_cha_user_dir);
	if (mkdir(cha_oneline, 0755) < 0 && errno != EEXIST) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		return errno;
	}
	if (chown(cha_oneline, pwd->pw_uid, pwd->pw_gid) < 0) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		rmdir(cha_oneline);
		return errno;
	}

	sprintf(cha_oneline, "%s/data/%s/upload", pwd->pw_dir, g_cha_user_dir);
	if (mkdir(cha_oneline, 0755) < 0 && errno != EEXIST) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		return errno;
	}
	if (chown(cha_oneline, pwd->pw_uid, pwd->pw_gid) < 0) {
		syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
		rmdir(cha_oneline);
		return errno;
	}

	sprintf(cha_oneline, "%s/data/%s/dbb_blog.ini", pwd->pw_dir, g_cha_user_dir);
	if (stat(cha_oneline, &sb) < 0) {
		fp = fopen(cha_oneline, "w+");
		if (!fp) {
			syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
			return errno;
		}
		fprintf(fp, "username=%s\n", chp_username);
		fprintf(fp, "dbname=%s\n", chp_db);
		fprintf(fp, "hostname=%s\n", chp_host);
		fprintf(fp, "password=%s\n", chp_password);
		fprintf(fp, "ssl=%d\n", g_in_ssl_mode);
		fclose(fp);
		if (chown(cha_oneline, pwd->pw_uid, pwd->pw_gid) < 0 || chmod(cha_oneline, 0400) < 0) {
			syslog(LOG_NOTICE, "%s - %s\n", cha_oneline, strerror(errno));
			remove(cha_oneline);
			sprintf(cha_oneline, "%s/data/%s", pwd->pw_dir, g_cha_user_dir);
			rmdir(cha_oneline);
			return errno;
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
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int main(int argc, char**argv)
{
	DBase *db;
	char cha_sql[1024];
	char cha_host[256];
	char cha_db[256];
	char cha_username[256];
	char cha_password[256];
	char cha_user[20];
	char cha_domain[256];
	char cha_skeleton[256];
	char cha_cssdir[256];
	char cha_csspath[256];
	char cha_owner[256];
	char cha_header[256];
	char ch;
	char ch_mode;
	char **chpp_argv;
	int in_ssl;
	int in_blog;
	int in_error;

	ch_mode = 0;
	cha_user[0] = '\0';
	cha_domain[0] = '\0';
	cha_owner[0] = '\0';
	cha_skeleton[0] = '\0';
	cha_header[0] = '\0';
	while ((ch = getopt(argc, argv, "dchre:i:j:k:ln:o:s:u:")) != (char)-1) {
		switch (ch) {
		case 'd':
		case 'c':
		case 'h':
		case 'r':
			ch_mode = ch;
			break;
		case 'e':
			if (strlen(optarg) < sizeof(cha_header)) {
				strcpy(cha_header, optarg);
			}
			break;
		case 'i':
			if (strlen(optarg) < sizeof(g_cha_apache_dir)) {
				strcpy(g_cha_apache_dir, optarg);
			}
			break;
		case 'j':
			if (strlen(optarg) < sizeof(g_cha_user_dir)) {
				strcpy(g_cha_user_dir, optarg);
			}
			break;
		case 'k':
			if (strlen(optarg) < sizeof(cha_skeleton)) {
				strcpy(cha_skeleton, optarg);
			}
			break;
		case 'l':
			g_in_ssl_mode = 1;
			break;
		case 'n':
			if (strlen(optarg) < sizeof(cha_owner)) {
				strcpy(cha_owner, optarg);
			}
			break;
		case 'o':
			if (strlen(optarg) < sizeof(cha_domain)) {
				strcpy(cha_domain, optarg);
				if (!cha_user[0]) {
					Domain_To_User(optarg, cha_user);
				}
			}
			break;
		case 's':
			if (strlen(optarg) < sizeof(g_cha_db_host)) {
				strcpy(g_cha_db_host, optarg);
			}
			break;
		case 'u':
			if (strlen(optarg) < sizeof(cha_user)) {
				strcpy(cha_user, optarg);
			}
			break;
		case '?':
			printf("Usage: newblog [-d [-n owner]] [-e prefix] [-h] [-c -o domain -k file] [-u unixuser] [-i dir] [-s host]\n");
			printf("\t-c        : cartex mode\n");
			printf("\t-d        : dbb mode\n");
			printf("\t-e prefix : dbname prefix\n");
			printf("\t-h        : hotbiz mode\n");
			printf("\t-i dir    : blog directory in apache\n");
			printf("\t-j dir    : blog directory in user dir\n");
			printf("\t-k file   : cartex db skeleton\n");
			printf("\t-l        : connect with ssl\n");
			printf("\t-o domain : cartex domain name\n");
			printf("\t-r        : eReserve mode\n");
			printf("\t-s host   : db host\n");
			printf("\t-u user   : cartex unix user\n");
			return 0;
		}
	}

	openlog("newblog", LOG_PID, LOG_LOCAL7);

	cha_sql[0] = '\0';
	chpp_argv = argv;
	while (*chpp_argv) {
		strcat(cha_sql, *chpp_argv);
		++chpp_argv;
	}
	syslog(LOG_NOTICE, "Creating blog start: %s", cha_sql);

	in_error = 0;
	if (ch_mode == 'c') {
		if (!cha_domain[0]) {
			syslog(LOG_NOTICE, "domain was not spedified.");
			++in_error;
		}
		if (!cha_skeleton[0]) {
			syslog(LOG_NOTICE, "db skeleton was not spedified.");
			++in_error;
		}
	}
	if (ch_mode == 'c' || ch_mode == 'r') {
		if (!cha_user[0]) {
			syslog(LOG_NOTICE, "Can not determin user id.");
			++in_error;
		}
	}
	if (in_error) {
		return 0;
	}

	if (ch_mode == 'c' || ch_mode == 'r') {
		in_error = read_pass_dat(ch_mode, cha_user, cha_header, cha_host,cha_db, cha_username, cha_password, cha_cssdir);
	} else if (cha_user[0]) {
		struct passwd *pwd;
		pwd = getpwnam(cha_user);
		if (!pwd) {
			syslog(LOG_NOTICE, "failed to get user info (%s)", cha_user);
			closelog();
			return 0;
		}
		strcpy(cha_sql, pwd->pw_dir);
		strcat(cha_sql, "/data/dbb_blog.ini");
		in_error = Read_Ini(cha_sql, cha_host, cha_db, cha_username, cha_password, &in_ssl);
	} else {
		in_error = Read_Blog_Ini(cha_host, cha_db, cha_username, cha_password, &in_ssl);
	}
	if (in_error) {
		syslog(LOG_NOTICE, "failed to read INI file(%s)", strerror(in_error));
		closelog();
		return 0;
	}
	if (cha_skeleton[0]) {
		FILE *fp;
		if (g_in_ssl_mode) {
			sprintf(cha_sql, "/usr/local/bin/mysql --ssl --ssl-key=/dev/null -h %s -u %s -p%s %s < %s > /dev/null 2>&1", cha_host, cha_username, cha_password, cha_db, cha_skeleton);
		} else {
			sprintf(cha_sql, "/usr/local/bin/mysql -h %s -u %s -p%s %s < %s > /dev/null 2>&1", cha_host, cha_username, cha_password, cha_db, cha_skeleton);
		}
		fp = popen(cha_sql, "r");
		if (!fp) {
			syslog(LOG_NOTICE, "%s\n", strerror(errno));
			return 0;
		}
		while (fgets(cha_sql, sizeof(cha_sql), fp));
		pclose(fp);
	}
	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		syslog(LOG_NOTICE, "can not to connect to database(%s)", Gcha_last_error);
		syslog(LOG_NOTICE, "(%d)%s %s %s %s", g_in_ssl_mode, cha_host, cha_db, cha_username, cha_password);
		closelog();
		return 0;
	}
	if (Read_Blog_Option(db)) {
		syslog(LOG_NOTICE, "can not to read option");
		closelog();
		return 0;
	}

	if (ch_mode == 'c' || ch_mode == 'r') {
		strcpy(cha_csspath, "/usr/local/apache/share/");
		strcat(cha_csspath, g_cha_apache_dir);
		strcat(cha_csspath, "/css");
		sprintf(cha_sql, "install -c -m 644 -o %s %s/*.css %s", cha_user, cha_csspath, cha_cssdir);
		system(cha_sql);
		if (strcmp(g_cha_apache_dir, "er-blog") == 0) {
			sprintf(cha_sql, "(cd %s; %s/as2er.sh)", cha_cssdir, cha_csspath);
			system(cha_sql);
		}
	}

	if(Begin_Transact(db)) {
		syslog(LOG_NOTICE, "failed to start transaction(%s)", Gcha_last_error);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	if (ch_mode == 'c' || ch_mode == 'r') {
		if (strcmp(g_cha_apache_dir, "as-blog")) {
			sprintf(cha_sql,
				"update sy_baseinfo set"
					" c_aspskelpath = replace(c_aspskelpath, 'as-blog', '%s')"
					",c_adminskelpath = replace(c_adminskelpath, 'as-blog', '%s')"
					",c_userskelpath = replace(c_userskelpath, 'as-blog', '%s')"
					",c_rsspath = replace(c_rsspath, 'as-blog', '%s')"
					",c_csspath = replace(c_csspath, 'as-blog', '%s')"
					",c_uploadpath = replace(c_uploadpath, 'as-blog', '%s')",
				g_cha_apache_dir, g_cha_apache_dir, g_cha_apache_dir, g_cha_apache_dir, g_cha_apache_dir, g_cha_apache_dir);
			if (Db_ExecSql(db, cha_sql)) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				Rollback_Transact(db);
				Db_Disconnect(db);
				closelog();
				return 0;
			}
		}
		if (strcmp(g_cha_user_dir, "as-blog")) {
			sprintf(cha_sql,
				"update sy_baseinfo set"
					" c_adminimagesloc = replace(c_adminimagesloc, 'as-blog', '%s')"
					",c_userimagesloc = replace(c_userimagesloc, 'as-blog', '%s')"
					",c_themeimagesloc = replace(c_themeimagesloc, 'as-blog', '%s')"
					",c_cssloc = replace(c_cssloc, 'as-blog', '%s')"
					",c_rssloc = replace(c_rssloc, 'as-blog', '%s')"
					",c_scriptsloc = replace(c_scriptsloc, 'as-blog', '%s')"
					",c_baseloc = replace(c_baseloc, 'as-blog', '%s')",
				g_cha_user_dir, g_cha_user_dir, g_cha_user_dir, g_cha_user_dir, g_cha_user_dir, g_cha_user_dir, g_cha_user_dir);
			if (Db_ExecSql(db, cha_sql)) {
				syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
				Rollback_Transact(db);
				Db_Disconnect(db);
				closelog();
				return 0;
			}
		}
		sprintf(cha_sql, "update sy_authinfo set c_host = '%s'", g_cha_db_host);
		if (Db_ExecSql(db, cha_sql)) {
			syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			Db_Disconnect(db);
			closelog();
			return 0;
		}
		sprintf(cha_sql, "update sy_cartinfo set c_host = '%s'", g_cha_db_host);
		if (Db_ExecSql(db, cha_sql)) {
			syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			Db_Disconnect(db);
			closelog();
			return 0;
		}
	}
	if(Db_ExecSql(db, "insert into at_blog (n_blog_id,d_create_time) values (NULL,now());")){
		syslog(LOG_NOTICE, "failed to exec query(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
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
	sprintf(cha_sql+strlen(cha_sql), ",b_needlogin = %d", (ch_mode == 'h' || ch_mode == 'd') ? 1 : 0);
	strcat(cha_sql, ",c_mail_subject = '%b　更新通知'");
	strcat(cha_sql, ",c_mail_body = '%c　様\r\nブログを更新しましたのでお知らせします。下記URLよりアクセスしてください。\r\n%u\r\n'");
	if (cha_user[0])
		sprintf(cha_sql+strlen(cha_sql), ",c_blog_id = '%s'", cha_user);
	sprintf(cha_sql+strlen(cha_sql), " where n_blog_id = %d", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	/* 用意されたPING送信先のチェックボックス更新 */
	sprintf(cha_sql,
		"insert into at_ping (n_blog_id,n_ping_id,c_ping_site,c_ping_url,b_default)"
		" select %d,n_ping_id,c_ping_site,c_ping_url,0 from sy_ping", in_blog);
	if (Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// プロフィール初期値作成
	sprintf(cha_sql, "insert into at_profile (n_blog_id,c_author_nickname,c_login,c_password) values (%d,'','','');", in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// テーマ初期値作成
	sprintf(cha_sql, "insert into at_looks (n_blog_id,n_theme_id,n_category_id) values (%d,1,1);", in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// サイドコンテンツ初期値
	sprintf(cha_sql,
		"insert into at_sidecontent "
			"(n_blog_id,n_sidecontent_id,n_sidecontent_order,n_sidecontent_type,c_sidecontent_title,b_display,b_allow_outer) "
		"select "
			"%d,n_sidecontent_id,n_sidecontent_order,n_sidecontent_type,c_sidecontent_title,1,b_allow_outer "
		"from "
			"sy_sidecontent;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// 用意された取り込みRSS初期値
	sprintf(cha_sql,
		"insert into at_rss "
			"(n_blog_id,n_rss_id,c_rss_title,c_rss_url,n_rss_article,b_rss_target,b_default) "
		"select "
			"%d,n_rss_id,c_rss_title,c_rss_url,0,0,1 "
		"from "
			"sy_rss;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// アーカイブ初期値
	sprintf(cha_sql,
		"insert into at_archive "
			"(n_blog_id,n_archive_id,n_archive_value) "
		"select "
			"%d,n_archive_id,n_archive_value "
		"from "
			"sy_archive;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// カテゴリ初期値
	sprintf(cha_sql,
		"insert into at_category "
			"(n_blog_id,n_category_id,n_category_order,c_category_name) "
		"select "
			"%d,n_category_id,n_category_order,c_category_name "
		"from "
			"sy_category;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// コメントフィルター初期化
	sprintf(cha_sql,
		"insert into at_comment_filter "
			"(n_blog_id,c_filter,b_valid) "
		"values "
			"(%d,'',0) "
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	// トラックバックフィルター初期化
	sprintf(cha_sql,
		"insert into at_trackback_filter "
			"(n_blog_id,n_filter_id,c_filter,b_valid) "
		"select "
			"%d,n_filter_id,'',0 "
		"from "
			"sy_filter;"
		, in_blog);
	if(Db_ExecSql(db, cha_sql)) {
		syslog(LOG_NOTICE, "failed to exec query(%s)(%s)", Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}
	if(Commit_Transact(db)) {
		syslog(LOG_NOTICE, "failed to commit transaction(%s)", Gcha_last_error);
		Rollback_Transact(db);
		Db_Disconnect(db);
		closelog();
		return 0;
	}

	if (ch_mode == 'd' && cha_owner[0]) {
		Write_To_Dbb_Db(db, in_blog, cha_owner);
	}
	if ((ch_mode == 'c') || (ch_mode == 'r' && cha_domain[0])) {
		RestartAppche(cha_domain, cha_user);
	}
	Db_Disconnect(db);
	closelog();

	return in_blog;
}
