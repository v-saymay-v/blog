/*
+* ------------------------------------------------------------------------
 * Module-Name:		blog_build_html.c
 * First-Created:	2004/08/16 香村 信二郎
%* ------------------------------------------------------------------------
 * Module-Description:
 *	ユーザページの構造（html）を構築
-* ------------------------------------------------------------------------
 * Change-Log:
 *	2005/06/17 関根大輔
 *		RSS読み込み関連
 *	2005/07/11 関根大輔
 *		大幅組みなおし
$* ------------------------------------------------------------------------
 */
static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: blog_build_html.c,v 1.605 2011/03/01 12:56:06 hayakawa Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>
#include <libcgi2.h>
#include <excart.h>
#include <libexcart.h>
#include "libnkf.h"
#include "libjcode.h"
#include "libcgi2.h"
#include "libdb2.h"
#include "libcommon2.h"
#include "libnet.h"
#include "libhttp.h"
#include "libauth.h"
#include "libblog.h"
#include "libblogcart.h"
#include "libblogreserve.h"

#define CO_RSS_VERSION_091	0	/* RSS version 0.91 */
#define CO_RSS_VERSION_10	1	/* RSS version 1.0 */
#define CO_RSS_VERSION_20	2	/* RSS version 2.0 */
#define CO_ATOM_VERSION_03	3	/* ATOM 0.3 */
#define CO_MAX_SIDE_WIDTH	20
#define CO_MAX_MAIN_WIDTH	20
#define CO_TO_PREV_MONTH	"&lt;&lt;"
#define CO_TO_NEXT_MONTH	"&gt;&gt;"
#define CO_TARGET_BLANK		" target=\"_blank\""

static int g_in_login_blog = 0;
static int g_in_black_list = 0;
static int g_in_blog_auth = 0;
static int g_in_blog_priv = 0;
static char *chpa_week[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
static char *chpa_week_jp[7] = {"日", "月", "火", "水", "木", "金", "土"};
static char *chpa_month[12] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};
static char *chpa_month_jp[12] = {
	"1月", "2月", "3月", "4月", "5月", "6月",
	"7月", "8月", "9月", "10月", "11月", "12月"
};
static int ina_mday[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/*
+* ------------------------------------------------------------------------
 * Function:	 	get_count_entry_by_date()
 * Description:
 *	mm月dd日に公開記事が何件存在するか
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了	件数
 *	エラー時	CO_ERROR
-* ------------------------------------------------------------------------*/
int get_count_entry_by_date(DBase *db, NLIST *nlp_out, char *chp_date, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	int in_row;

	strcpy(cha_sql, "select count(*)");	/* 0 件数 */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");	/* 公開記事 */
	sprintf(cha_sql + strlen(cha_sql), " and T1.d_entry_create_time >= '%s000000'", chp_date);
	sprintf(cha_sql + strlen(cha_sql), " and T1.d_entry_create_time <= '%s235959'", chp_date);
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	if(!Db_GetValue(dbres, 0, 0)) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_row = atoi(Db_GetValue(dbres, 0, 0));
	Db_CloseDyna(dbres);
	return in_row;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_count_entry_eq_date()
 * Description:
 *	mm月dd日に公開記事が何件存在するか
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了	件数
 *	エラー時	CO_ERROR
-* ------------------------------------------------------------------------*/
int get_count_entry_eq_date(DBase *db, NLIST *nlp_out, char *chp_date, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	int in_row;

	strcpy(cha_sql, "select count(*)");	/* 0 件数 */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");	/* 公開記事 */
	sprintf(cha_sql + strlen(cha_sql), " and date_format(T1.d_entry_create_time, '%%Y%%m%%d') = '%s'", chp_date);
	strcat(cha_sql, " and date_format(T1.d_entry_real_time, '%Y%m%d') <= date_format(T1.d_entry_create_time, '%Y%m%d')");
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	if(!Db_GetValue(dbres, 0, 0)) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_row = atoi(Db_GetValue(dbres, 0, 0));
	Db_CloseDyna(dbres);
	return in_row;
}

/*
+* ------------------------------------------------------------------------
 * Function:            dayofweek
 * Description:
 *	指定日が何曜日かを返す。Zellerの公式。
%* ------------------------------------------------------------------------
 * Return:
 *	0 日曜 ... 6 土曜
-* ------------------------------------------------------------------------*/
int dayofweek(int in_year, int in_month, int in_day)
{
	if (in_month == 1 || in_month == 2) {
		in_year = in_year - 1;
		in_month = in_month + 12;
	}
	return (in_year + in_year / 4 - in_year / 100 + in_year / 400 + (26 * in_month + 16) / 10 + in_day) % 7;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	Calender_Language()
 * Description:
 *	カレンダー表示言語取得
%* ------------------------------------------------------------------------
 * Return:
 *	英語    0
 *	日本語  1
 *	エラー -1
-* ------------------------------------------------------------------------*/
int Calender_Language(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_temp[8192];
	char *chp_tmp;
	int in_japanese;

	/* 曜日の日本語表示を得る */
	sprintf(cha_temp, "select b_japanese_calendar from at_profile where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_temp);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_temp);
		return -1;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if(!chp_tmp){
		in_japanese = 0;
	} else {
		in_japanese = atoi(chp_tmp);
	}
	Db_CloseDyna(dbres);
	return in_japanese;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_calender()
 * Description:
 *	サイドコンテンツ(カレンダー)を作成
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_calender(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	struct tm *stp_now;
	time_t tm_now;
	char *chp_tmp;
	char cha_date[10];
	char cha_month[32];
	char cha_temp[8192];
	int in_year;
	int in_month;
	int in_today;
	int in_exist;
	int in_bold;
	int in_dayroop;
	int in_dw;
	int in_cnt;
	int in_japanese;
	int i;

	/* 曜日の日本語表示を得る */
	in_japanese = Calender_Language(db, nlp_out, in_blog);
	if (in_japanese < 0){
		return 1;
	}

	/* 現在を取る */
	time(&tm_now);
	stp_now = localtime(&tm_now);
	chp_tmp = Get_Nlist(nlp_in, "calendar", 1);
	if(chp_tmp) {
		/* 年月はyyyymmの形で提供されなければならない。 */
		if(Check_Numeric(chp_tmp) || strlen(chp_tmp) != 6) {
			Put_Nlist(nlp_out, "ERROR", "無効な年月指定です。<br>");
			return 1;
		}
		sscanf(chp_tmp, "%4d%2d", &in_year, &in_month);
		if(in_month < 1 || in_month > 12) {
			Put_Nlist(nlp_out, "ERROR", "無効な年月指定です。<br>");
			return 1;
		}
	} else {
		/* 年月指定がない場合は今年 */
		in_year = stp_now->tm_year + 1900;
		in_month = stp_now->tm_mon + 1;
	}
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-calendar\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"1\">\n");
	in_dw = dayofweek(in_year, in_month, 1);
	in_cnt = in_dw;
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n<td>&nbsp;</td>\n<td>");
	/* 前の月へのリンク */
	if(in_month != 1){
		if (in_japanese) {
			sprintf(cha_month, "%04d年%s", in_year, chpa_month_jp[in_month - 2]);
		} else {
			sprintf(cha_month, "%04d %s", in_year, chpa_month[in_month - 2]);
		}
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%s/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year, in_month - 1, cha_month, CO_TO_PREV_MONTH);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%08d/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year, in_month - 1, cha_month, CO_TO_PREV_MONTH);
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s/%s?calendar=%04d%02d&bid=%d#calendar\" title=\"%s\">%s</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year, in_month - 1, in_blog, cha_month, CO_TO_PREV_MONTH);
		}
	} else {
		if (in_japanese) {
			sprintf(cha_month, "%04d年%s", in_year - 1, chpa_month_jp[11]);
		} else {
			sprintf(cha_month, "%04d %s", in_year - 1, chpa_month[11]);
		}
		if (!in_year) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", CO_TO_PREV_MONTH);
		} else if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%s/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year -1, 12, cha_month, CO_TO_PREV_MONTH);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%08d/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year -1, 12, cha_month, CO_TO_PREV_MONTH);
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s/%s?calendar=%04d%02d&bid=%d#calendar\" title=\"%s\">%s</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year -1, 12, in_blog, cha_month, CO_TO_PREV_MONTH);
		}
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td>\n<td colspan=\"5\" class=\"calendar-month\">");
	/* 現在表示している年月 */
	if (in_japanese) {
		Put_Format_Nlist(nlp_out, "SIDEBAR", "%04d年%s", in_year, chpa_month_jp[in_month - 1]);
	} else {
		Put_Format_Nlist(nlp_out, "SIDEBAR", "%04d %s", in_year, chpa_month[in_month - 1]);
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td>\n<td>");
	/* 次の月へのリンク */
	if(in_month != 12){
		if (in_japanese) {
			sprintf(cha_month, "%04d年%s", in_year, chpa_month_jp[in_month]);
		} else {
			sprintf(cha_month, "%04d %s", in_year, chpa_month[in_month]);
		}
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%s/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year, in_month + 1, cha_month, CO_TO_NEXT_MONTH);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%08d/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year, in_month + 1, cha_month, CO_TO_NEXT_MONTH);
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s/%s?calendar=%04d%02d&bid=%d#calendar\" title=\"%s\">%s</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year, in_month + 1, in_blog, cha_month, CO_TO_NEXT_MONTH);
		}
	} else {
		if (in_japanese) {
			sprintf(cha_month, "%04d年%s", in_year + 1, chpa_month_jp[0]);
		} else {
			sprintf(cha_month, "%04d %s", in_year + 1, chpa_month[0]);
		}
		if (in_year == 9999) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", CO_TO_NEXT_MONTH);
		} else if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%s/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year + 1, 1, cha_month, CO_TO_NEXT_MONTH);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s%s%s/%08d/?calendar=%04d%02d#calendar\" title=\"%s\">%s</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year + 1, 1, cha_month, CO_TO_NEXT_MONTH);
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<a href=\"%s/%s?calendar=%04d%02d&bid=%d#calendar\" title=\"%s\">%s</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year + 1, 1, in_blog, cha_month, CO_TO_NEXT_MONTH);
		}
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td>\n<td>&nbsp;</td>\n</tr>\n");

	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "\t<td>&nbsp;</td>\n");
	/* 曜日セル */
	for(i = 0; i < 7; i++) {
		Put_Nlist(nlp_out, "SIDEBAR", "\t<td class=\"calendar-week\">");
		if (in_japanese) {
			Put_Nlist(nlp_out, "SIDEBAR", chpa_week_jp[i]);
		} else {
			Put_Nlist(nlp_out, "SIDEBAR", chpa_week[i]);
		}
		Put_Nlist(nlp_out, "SIDEBAR", "</td>\n");
	}
	Put_Nlist(nlp_out, "SIDEBAR", "\t<td>&nbsp;</td>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "\t<td>&nbsp;</td>\n");
	/* 月の最初の日の位置合わせ */
	for(i = 0; i < in_dw; i++){
		Put_Nlist(nlp_out, "SIDEBAR", "\t<td>&nbsp;</td>\n"); /* 空セル */
	}
	/* 今日をハイライトする際のフラグ */
	in_today = 0;
	if(in_year == stp_now->tm_year + 1900 && in_month == stp_now->tm_mon + 1) {
		in_today = stp_now->tm_mday;
	} else {
		in_today = 0;
	}
	/* 一月の日数を得るうるう年(2月) */
	if(in_month == 2 && in_year % 4 == 0 && in_year % 400 != 0){
		in_dayroop = ina_mday[in_month] + 1;
	} else {
		in_dayroop = ina_mday[in_month];
	}
	for(i = 1; i <= in_dayroop; i++){
		if(in_cnt == 0 && i != 1) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "\t<td>&nbsp;</td>\n");
		}
		memset(cha_temp, '\0', sizeof(cha_temp));
		sprintf(cha_date, "%04d%02d%02d", in_year, in_month, i);
		/* 今日ならば色を変える */
		if(i != in_today){
			strcpy(&cha_temp[strlen(cha_temp)], "\t<td class=\"cell\">");
		} else {
			strcpy(&cha_temp[strlen(cha_temp)], "\t<td class=\"today\">");
		}
		/* 組み立てようとしている日に記事があるかどうかを調べる */
		in_exist = get_count_entry_by_date(db, nlp_out, cha_date, in_blog);
		if(in_exist > 0) {
			if (g_in_dbb_mode) {
				sprintf(&cha_temp[strlen(cha_temp)], "<a href=\"%s%s%s/%s/?date=%s#archive_top\""
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, cha_date);
			} else if (g_in_short_name) {
				sprintf(&cha_temp[strlen(cha_temp)], "<a href=\"%s%s%s/%08d/?date=%s#archive_top\""
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, cha_date);
			} else {
				sprintf(&cha_temp[strlen(cha_temp)], "<a href=\"%s/%s?date=%s&bid=%d#archive_top\""
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, cha_date, in_blog);
			}
			if(i != in_today) {
				sprintf(&cha_temp[strlen(cha_temp)], ">");
			} else {
				sprintf(&cha_temp[strlen(cha_temp)], " title=\"today\">");
			}
		}
		in_bold = 0;
		if (g_in_dbb_mode && in_blog == g_in_login_blog) {
			in_bold = get_count_entry_eq_date(db, nlp_out, cha_date, in_blog);
		}
		/* 今日だけをハイライト */
		if(i != in_today) {
			sprintf(&cha_temp[strlen(cha_temp)], "%2d", i);
		} else {
			sprintf(&cha_temp[strlen(cha_temp)], "<span title=\"today\">%3d</span>", i);
		}
		if(in_exist > 0) {
			strcat(cha_temp, "</a>");
		}
		if (in_bold > 0) {
			strcat(cha_temp, "<sup>*</sup>");
		}
		strcat(cha_temp, "</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", cha_temp);
		in_cnt++;
		if(in_cnt == 7) {
			Put_Nlist(nlp_out, "SIDEBAR", "\t<td>&nbsp;</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			in_cnt = 0;
		}
	}
	/* 最後の行のセル調整 */
	if(in_cnt != 0) {
		memset(cha_temp, '\0', sizeof(cha_temp));
		while(in_cnt != 7) {
			strcat(cha_temp, "\t<td>&nbsp;</td>\n");
			in_cnt++;
		}
		strcat(cha_temp, "\t<td>&nbsp;</td>\n");
		strcat(cha_temp, "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", cha_temp);
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_archive_month()
 * Description:
 *	指定年の月別アーカイブのサイドバーを作成する。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_archive_month(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	struct tm *stp_now;
	DBRes *dbres;
	char *chp_tmp;
	char cha_temp[512];
	char cha_top[10];
	char cha_end[10];
	char cha_sql[512];
	time_t tm_now;
	int in_latest_year;
	int in_oldest_year;
	int in_count;
	int in_link;
	int in_archive_value;
	int in_year;
	int i;
	int j;
	int k;

	chp_tmp = Get_Nlist(nlp_in, "year", 1);
	if(chp_tmp) {
		if(Check_Numeric(chp_tmp) || strlen(chp_tmp) != 4) {
			Put_Nlist(nlp_out, "ERROR", "無効な年指定です。<br>");
			return 1;
		}
		in_year = atoi(chp_tmp);
	} else {
		/* 年指定がない場合は今年 */
		time(&tm_now);
		stp_now = localtime(&tm_now);
		in_year = stp_now->tm_year + 1900;
	}
	/* 頭を張る */
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-marchive\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	/* 一軒も記事がない場合は即リターン。 */
	if(Get_Total_Entry(db, nlp_in, nlp_out, 1, in_blog) == 0) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr align=\"left\">\n<td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<ul>\n<li>記事はありません</li>\n</ul>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</td>\n</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
		return 0;
	}
	/* 最新記事の年を取得 */
	in_latest_year = Get_Latest_Entry_Year(db, in_blog);
	/* 月別アーカイブの設定値を取得 */
	strcpy(cha_sql, "select T1.n_archive_value");	/* 0 何年分表示するか  */
	strcat(cha_sql, " from at_archive T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_MONTHLY);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "月別アーカイブの設定値取得に失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "月別アーカイブの設定がされていません。");
		Db_CloseDyna(dbres);
		return 1;
	}
	in_archive_value = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	/* 値が0の場合はエントリーがあるだけ表示 */
	if(in_archive_value == 0) {
		in_oldest_year = Get_Oldest_Entry_Year(db, in_blog);
	} else {
		in_oldest_year = in_latest_year - in_archive_value + 1;
	}
	in_count = in_latest_year - in_oldest_year + 1;
	for(i = 0; i < in_count; i++) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>");
		Put_Nlist(nlp_out, "SIDEBAR", "<td colspan=\"5\" class=\"side-marchive-year\">");
		/* 指定年以外の場合 */
		/* 年のみ表示し、記事がある場合はリンク表示。 */
		if(in_latest_year - i != in_year) {
			sprintf(cha_top, "%04d0101", in_latest_year - i);
			sprintf(cha_end, "%04d1231", in_latest_year - i);
			in_link = get_count_article_by_day(db, cha_top, cha_end, in_blog);
			/* 対象の年に記事がある場合は、リンク表示 */
			if(in_link > 0) {
				if (g_in_dbb_mode) {
					sprintf(cha_temp, "+ <a href=\"%s%s%s/%s/?marchive=1&year=%d#marchive\">%d</a>\n"
						, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_latest_year - i, in_latest_year - i);
				} else if (g_in_short_name) {
					sprintf(cha_temp, "+ <a href=\"%s%s%s/%08d/?marchive=1&year=%d#marchive\">%d</a>\n"
						, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_latest_year - i, in_latest_year - i);
				} else {
					sprintf(cha_temp, "+ <a href=\"%s/%s?marchive=1&year=%d&bid=%d#marchive\">%d</a>\n"
						, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_latest_year - i, in_blog, in_latest_year - i);
				}
			} else {
				sprintf(cha_temp, "+ %d\n", in_latest_year - i);
			}
			Put_Nlist(nlp_out, "SIDEBAR", cha_temp);
			Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
		/* 指定年の表示 		*/
		/* 年と、1から12月までの表示。	*/
		/* 記事がある月はリンク表示。	*/
		} else {
			sprintf(cha_temp, "- <b>%d</b>", in_latest_year - i);
			Put_Nlist(nlp_out, "SIDEBAR", cha_temp);
			Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
			/* 2段で表示 */
			for(j = 0; j < 2; j++) {
				Put_Nlist(nlp_out, "SIDEBAR", "<tr>");
				/* 整形のための列を含めて7列表示 */
				for(k = 0; k < 7; k++) {
					Put_Nlist(nlp_out, "SIDEBAR", "<td nowrap>");
					/* 最初の列は整形列 */
					if(k == 0) {
						Put_Nlist(nlp_out, "SIDEBAR", "&nbsp;");
					} else {
						sprintf(cha_top, "%04d%02d01", in_year, j * 6 + k);
						sprintf(cha_end, "%04d%02d31", in_year, j * 6 + k);
						in_link = get_count_article_by_day(db, cha_top, cha_end, in_blog);
						/* 記事がある場合はリンク表示 */
						if(in_link > 0){
							if (g_in_dbb_mode) {
								sprintf(cha_temp, "<a href=\"%s%s%s/%s/?month=%04d%02d#archive_top\">"
									, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year, j * 6 + k);
							} else if (g_in_short_name) {
								sprintf(cha_temp, "<a href=\"%s%s%s/%08d/?month=%04d%02d#archive_top\">"
									, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year, j * 6 + k);
							} else {
								sprintf(cha_temp, "<a href=\"%s/%s?month=%04d%02d&bid=%d#archive_top\">"
									, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year, j * 6 + k, in_blog);
							}
							Put_Nlist(nlp_out, "SIDEBAR", cha_temp);
						}
						sprintf(cha_temp, "%d月", j * 6 + k);
						Put_Nlist(nlp_out, "SIDEBAR", cha_temp);
						if(in_link > 0){
							Put_Nlist(nlp_out, "SIDEBAR", "</a>\n");
						}
					}
					Put_Nlist(nlp_out, "SIDEBAR", "</td>");
				}
				Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			}
		}
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>");
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	build_sidecontent_archive_category()
 * Description:
 *	カテゴリー別サイドコンテンツを作成する
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_archive_category(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_escape;
	char cha_html[8192];
	char cha_sql[512];
	int in_category_id;
	int in_count;
	int i;

	/* 記事数を格納するテンポラリテーブルを作る */
	/* (left join しても0が蹴られるので) */
	strcpy(cha_sql, "create temporary table tmp_entry_count as");
	strcat(cha_sql, " select T1.n_category_id as n_category_id");	/* 0 カテゴリID */
	strcat(cha_sql, ", coalesce(count(*), 0) as n_entry_count");	/* 1 記事数 */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " group by T1.n_category_id");
	if(Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "記事数の取得に失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	/* 表示順に沿ってカテゴリIDとカテゴリ名を取る */
	strcpy(cha_sql, "select T1.n_category_id");		/* 0 カテゴリID */
	strcat(cha_sql, ", T1.c_category_name");		/* 1 カテゴリ名 */
	strcat(cha_sql, ", coalesce(T2.n_entry_count, 0)");	/* 2 記事数 */
	strcat(cha_sql, " from at_category T1");
	strcat(cha_sql, " left join tmp_entry_count T2");
	strcat(cha_sql, " on T1.n_category_id = T2.n_category_id");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_category_order asc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-carchive\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\">\n");
	for(i = 0; i < in_count; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if(!chp_tmp || !*chp_tmp) {
			Db_CloseDyna(dbres);
			Put_Nlist(nlp_out, "ERROR", "カテゴリーIDを取得できませんでした。");
			return 1;
		}
		in_category_id = atoi(chp_tmp);
		chp_tmp = Db_GetValue(dbres, i, 1);
		if(!chp_tmp) {
			Db_CloseDyna(dbres);
			Put_Nlist(nlp_out, "ERROR", "カテゴリー名を取得できませんでした。");
			return 1;
		}
		chp_escape = Escape_HtmlString(chp_tmp);
		memset(cha_html, '\0', sizeof(cha_html));
		chp_tmp = Db_GetValue(dbres, i, 2);
		if(!chp_tmp || !*chp_tmp) {
			Db_CloseDyna(dbres);
			Put_Nlist(nlp_out, "ERROR", "カテゴリー内の記事数取得に失敗しました。");
			return 0;
		}
		/* 記事があるものはリンク表示 */
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		if(chp_tmp && atoi(chp_tmp) > 0){
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td valign=\"top\" width=\"10\">*</td><td><a href=\"%s%s%s/%s/?cid=%d#archive_top\">%s(%s)</a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_category_id, chp_escape, chp_tmp);
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td valign=\"top\" width=\"10\">*</td><td><a href=\"%s%s%s/%08d/?cid=%d#archive_top\">%s(%s)</a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_category_id, chp_escape, chp_tmp);
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td valign=\"top\" width=\"10\">*</td><td><a href=\"%s/%s?cid=%d&bid=%d#archive_top\">%s(%s)</a></td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_category_id, in_blog, chp_escape, chp_tmp);
			}
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<td valign=\"top\" width=\"10\">*</td><td>%s(0)</td>\n", chp_escape);
		}
		free(chp_escape);
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
	}
	Db_CloseDyna(dbres);
	if (g_in_cart_mode == CO_CART_SHOPPER) {
		in_count = Get_Back_Number(db, nlp_out, in_blog);
		if (in_count < 0) {
			Put_Nlist(nlp_out, "ERROR", "カートDBを開けませんでした。");
			Put_Nlist(nlp_out, "QUERY", cha_sql);
			return 1;
		}
		if (in_count > 0) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td valign=\"top\" width=\"10\">*</td><td><a href=\"%s%s%s/%s/?cid=%d#archive_top\">バックナンバー(%d)</a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, CO_BACKNO_CATEGORY, in_count);
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td valign=\"top\" width=\"10\">*</td><td><a href=\"%s%s%s/%08d/?cid=%d#archive_top\">バックナンバー(%d)</a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, CO_BACKNO_CATEGORY, in_count);
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td valign=\"top\" width=\"10\">*</td><td><a href=\"%s/%s?cid=%d&bid=%d#archive_top\">バックナンバー(%d)</a></td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, CO_BACKNO_CATEGORY, in_blog, in_count);
			}
		}
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	build_sidecontent_archive_newentry()
 * Description:
 *	最新記事サイドコンテンツを作成する
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_archive_newentry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_html[8192];
	char cha_sql[512];
	int in_newentry;
	int in_count;
	int in_entry_id;
	int i;

	/* 実際の表示件数を取る */
	strcpy(cha_sql, "select T1.n_archive_value");
	strcat(cha_sql, " from at_archive T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_NEWENTRY);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		Put_Nlist(nlp_out, "ERROR", "最新記事の表示件数を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp || atoi(chp_tmp) < 1) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "最新記事の表示件数の設定が正しくありません。");
		return 1;
	}
	in_count = atoi(chp_tmp);
	/* 設定件数分セレクトしてくる */
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select T1.n_entry_id");	/* 0 コメントID */
	strcat(cha_sql, " , T1.c_entry_title");		/* 1 記事タイトル */
	strcat(cha_sql, " , date_format(T1.d_entry_create_time,'%y年%m月%d日%H時%i分')");	/* 2 作成時間 */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");		/* 表示モードのもののみ */
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " and T1.d_entry_create_time <= now()");
	strcat(cha_sql, " order by T1.d_entry_create_time desc");
	sprintf(cha_sql + strlen(cha_sql), " limit %d", in_count);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "最新記事情報を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_newentry = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-latest-comment\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	if(!in_newentry) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td valign=\"top\" width=\"10\">*</td><td valign=\"top\">記事はありません。</td></tr>\n");
	} else {
		for(i = 0; i < in_newentry; i++) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "<td valign=\"top\" width=\"10\">*</td>\n");
			/* error check */
			chp_tmp = Db_GetValue(dbres, i, 0);
			if(!chp_tmp || !*chp_tmp) {
				Db_CloseDyna(dbres);
				Put_Nlist(nlp_out, "ERROR", "記事のIDを取得できませんでした。");
				return 1;
			}
			in_entry_id = atoi(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 1);
			if(!chp_tmp) {
				Put_Nlist(nlp_out, "ERROR", "記事タイトルを取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_tmp2 = Conv_Long_Ascii(chp_tmp, 20);
			chp_escape = Escape_HtmlString(chp_tmp2);
			free(chp_tmp2);
			chp_tmp = Db_GetValue(dbres, i, 2);
			if(!chp_tmp || !*chp_tmp) {
				Put_Nlist(nlp_out, "ERROR", "記事の投稿時刻を取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			if (g_in_dbb_mode) {
				sprintf(cha_html, "<td valign=\"top\"><a href=\"%s%s%s/%s/?eid=%d#entry\">%s</a><br>\n(%s)</td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_entry_id, chp_escape, chp_tmp);
			} else if (g_in_short_name) {
				sprintf(cha_html, "<td valign=\"top\"><a href=\"%s%s%s/%08d/?eid=%d#entry\">%s</a><br>\n(%s)</td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_entry_id, chp_escape, chp_tmp);
			} else {
				sprintf(cha_html, "<td valign=\"top\"><a href=\"%s/%s?eid=%d&bid=%d#entry\">%s</a><br>\n(%s)</td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_entry_id, in_blog, chp_escape, chp_tmp);
			}
			free(chp_escape);
			Put_Nlist(nlp_out, "SIDEBAR", cha_html);
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		}
	}
	Db_CloseDyna(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n</td>\n</tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_archive_comment()
 * Description:
 *	最新コメントのサイドコンテンツ作成。
%* ------------------------------------------------------------------------
 * Return:
 *			正常終了 0
 *			エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_archive_comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape1;
	char *chp_escape2;
	char cha_sql[512];
	char cha_temp[8192];
	int in_entry_id;
	int in_comment_id;
	int in_count;		/* 管理側で設定した最大表示数 */
	int in_comment;		/* 実際に表示する件数 */
	int i;

	/* 実際の表示件数を取る */
	strcpy(cha_sql, "select T1.n_archive_value");
	strcat(cha_sql, " from at_archive T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_COMMENT);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "コメントの表示件数を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp || atoi(chp_tmp) < 1) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "コメントの表示件数の設定が正しくありません。");
		return 1;
	}
	in_count = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	/* 設定件数分セレクトしてくる */
	strcpy(cha_sql, "select T1.n_comment_id");	/* 0 コメントID */
	strcat(cha_sql, " , T1.c_comment_author");	/* 1 コメントした人 */
	strcat(cha_sql, " , T1.n_entry_id");		/* 2 コメント元記事ID */
	strcat(cha_sql, " , T2.c_entry_title");		/* 3 元記事タイトル */
	strcat(cha_sql, " , date_format(T1.d_comment_create_time,'%y年%m月%d日%H時%i分')");	/* 4 コメント作成時間 */
	strcat(cha_sql, " from at_comment T1");
	strcat(cha_sql, " , at_entry T2");
	strcat(cha_sql, " where T1.b_comment_accept != 0");
	strcat(cha_sql, " and T1.n_entry_id = T2.n_entry_id");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	strcat(cha_sql, " and T2.b_mode = 1");		/* 親記事が表示モードのもののみ */
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_comment_create_time desc");
	sprintf(cha_sql + strlen(cha_sql), " limit %d", in_count);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "個別のコメント情報を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_comment = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<td class=\"side-latest-comment\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	if(!in_comment) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td valign=\"top\" width=\"10\">*</td><td valign=\"top\">コメントはありません。</td></tr>\n");
	} else {
		for(i = 0; i < in_comment; i++) {
			/* error check */
			chp_tmp = Db_GetValue(dbres, i, 0);
			if(!chp_tmp || !*chp_tmp) {
				Db_CloseDyna(dbres);
				Put_Nlist(nlp_out, "ERROR", "コメントIDを取得できませんでした。");
				return 1;
			}
			in_comment_id = atoi(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 2);
			if(!chp_tmp || !*chp_tmp) {
				Db_CloseDyna(dbres);
				Put_Nlist(nlp_out, "ERROR", "親記事のIDを取得できませんでした。");
				return 1;
			}
			in_entry_id = atoi(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 1);
			if(!chp_tmp){
				Put_Nlist(nlp_out, "ERROR", "コメントのauthorを取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_escape1 = Escape_HtmlString(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 3);
			if(!chp_tmp){
				Put_Nlist(nlp_out, "ERROR", "コメントの親記事タイトルを取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_tmp2 = Conv_Long_Ascii(chp_tmp, 20);
			chp_escape2 = Escape_HtmlString(chp_tmp2);
			free(chp_tmp2);
			chp_tmp = Db_GetValue(dbres, i, 4);
			if(!chp_tmp || !*chp_tmp) {
				Put_Nlist(nlp_out, "ERROR", "コメントの生成時刻を取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "<td valign=\"top\" width=\"10\">*</td>\n");
			if (g_in_dbb_mode) {
				sprintf(cha_temp, "<td><a href=\"%s%s%s/%s/?eid=%d#c%d\">%s</a><br>\n%s%s(%s)</td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_entry_id, in_comment_id
					, chp_escape1, CO_ARROW, chp_escape2, chp_tmp);
			} else if (g_in_short_name) {
				sprintf(cha_temp, "<td><a href=\"%s%s%s/%08d/?eid=%d#c%d\">%s</a><br>\n%s%s(%s)</td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_entry_id, in_comment_id
					, chp_escape1, CO_ARROW, chp_escape2, chp_tmp);
			} else {
				sprintf(cha_temp, "<td><a href=\"%s/%s?eid=%d&bid=%d#c%d\">%s</a><br>\n%s%s(%s)</td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_entry_id, in_blog, in_comment_id
					, chp_escape1, CO_ARROW, chp_escape2, chp_tmp);
			}
			free(chp_escape1);
			free(chp_escape2);
			Put_Nlist(nlp_out, "SIDEBAR", cha_temp);
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		}
	}
	Db_CloseDyna(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n</td>\n</tr>\n");
	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_archive_trackback()
 * Description:
 *	トラックバックのサイドコンテンツを作成する。
%* ------------------------------------------------------------------------
 * Return:
 *			正常終了 0
 *			エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_archive_trackback(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_entry_title;
	char *chp_tb_blog_name;
	char cha_sql[512];
	int in_entry_id;
	int in_trackback_id;
	int i;
	int in_count;			/* 設定した最大表示件数 */
	int in_trackback;		/* 実際に表示する件数 */

	in_count = 0;
	strcpy(cha_sql, "select T1.n_archive_value");
	strcat(cha_sql, " from at_archive T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_TRACKBACK);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "引用記事サイドバーの設定を得るクエリに失敗しました。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "トラックバックサイドバーの設定を得るクエリに失敗しました。<br>");
		}
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp || atoi(chp_tmp) < 1) {
		Db_CloseDyna(dbres);
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "引用記事サイドバーの設定を得るクエリに失敗しました。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "トラックバックサイドバーの設定に誤りがあります。");
		}
		return 1;
	}
	in_count = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	strcpy(cha_sql, "select T1.n_tb_id");		/* 0 トラックバックID */
	strcat(cha_sql, ", T1.c_tb_blog_name");		/* 1 トラックバック元 */
	strcat(cha_sql, ", T1.n_entry_id");			/* 2 親記事ID */
	strcat(cha_sql, ", T2.c_entry_title");		/* 3 親記事タイトル */
	strcat(cha_sql, ", date_format(T1.d_tb_create_time,'%y年%m月%d日%H時%i分')");	/* 4 TB時間 */
	strcat(cha_sql, " from at_trackback T1");
	strcat(cha_sql, ", at_entry T2");
	strcat(cha_sql, " where T1.b_tb_accept != 0");
	strcat(cha_sql, " and T1.n_entry_id = T2.n_entry_id");
	strcat(cha_sql, " and T1.n_blog_id = T2.n_blog_id");
	strcat(cha_sql, " and T2.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by d_tb_create_time desc");
	sprintf(cha_sql + strlen(cha_sql), " limit %d", in_count);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "引用記事を得るクエリーに失敗しました。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "トラックバックを得るクエリーに失敗しました。<br>");
		}
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_trackback = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<td class=\"side-latest-trackback\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	if(in_trackback == 0) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr><td valign=\"top\" width=\"10\">*</td><td valign=\"top\">引用された記事はありません。</td></tr>\n");
		} else {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr><td valign=\"top\" width=\"10\">*</td><td valign=\"top\">トラックバックはありません。</td></tr>\n");
		}
	} else {
		for(i = 0; i < in_trackback && i < in_count; i++){
			/*error check*/
			chp_tmp = Db_GetValue(dbres, i, 0);
			if(!chp_tmp || !*chp_tmp) {
				Db_CloseDyna(dbres);
				if (g_in_dbb_mode) {
					Put_Nlist(nlp_out, "ERROR", "引用記事IDを取得できませんでした。");
				} else {
					Put_Nlist(nlp_out, "ERROR", "トラックバックIDを取得できませんでした。");
				}
				return 1;
			}
			in_trackback_id = atoi(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 2);
			if(!chp_tmp || !*chp_tmp) {
				Db_CloseDyna(dbres);
				Put_Nlist(nlp_out, "ERROR", "親記事のIDを取得できませんでした。");
				return 1;
			}
			in_entry_id = atoi(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 1);
			if(!chp_tmp) {
				if (g_in_dbb_mode) {
					Put_Nlist(nlp_out, "ERROR", "記事引用元を取得できませんでした。");
				} else {
					Put_Nlist(nlp_out, "ERROR", "トラックバック元を取得できませんでした。");
				}
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_tb_blog_name = Escape_HtmlString(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 3);
			if(!chp_tmp) {
				Put_Nlist(nlp_out, "ERROR", "親記事タイトルを取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_tmp2 = Conv_Long_Ascii(chp_tmp, 20);
			chp_entry_title = Escape_HtmlString(chp_tmp2);
			free(chp_tmp2);
			chp_tmp = Db_GetValue(dbres, i, 4);
			if(!chp_tmp || !*chp_tmp){
				if (g_in_dbb_mode) {
					Put_Nlist(nlp_out, "ERROR", "記事引用元を取得できませんでした。");
				} else {
					Put_Nlist(nlp_out, "ERROR", "トラックバックの生成時刻を取得できませんでした。");
				}
				Db_CloseDyna(dbres);
				return 1;
			}
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "<td valign=\"top\" width=\"10\">*</td>\n");
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td><a href=\"%s%s%s/%s/?eid=%d#tb%d\" title=\"%s\">%s</a><br>\n<small>(%s)</small><br>\n%s%s</td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_entry_id, in_trackback_id
					, chp_entry_title, chp_entry_title, chp_tmp, CO_ARROW, chp_tb_blog_name);
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td><a href=\"%s%s%s/%08d/?eid=%d#tb%d\" title=\"%s\">%s</a><br>\n<small>(%s)</small><br>\n%s%s</td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_entry_id, in_trackback_id
					, chp_entry_title, chp_entry_title, chp_tmp, CO_ARROW, chp_tb_blog_name);
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR"
					, "<td><a href=\"%s/%s?eid=%d&bid=%d#tb%d\" title=\"%s\">%s</a><br>\n<small>(%s)</small><br>\n%s%s</td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_entry_id, in_blog, in_trackback_id
					, chp_entry_title, chp_entry_title, chp_tmp, CO_ARROW, chp_tb_blog_name);
			}
			free(chp_entry_title);
			free(chp_tb_blog_name);
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		}
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n</td>\n</tr>\n");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_links()
 * Description:
 *	リンク集のサイドコンテンツを作る
%* ------------------------------------------------------------------------
 * Return:
 * 			正常終了 0
 *			エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_links(DBase *db, NLIST *nlp_out, int in_group_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_title;
	char *chp_url;
	char cha_sql[512];
	char cha_html[8192];
	int in_row;
	int i;

	strcpy(cha_sql, "select T1.c_link_title");	/* 0 リンクタイトル */
	strcat(cha_sql, ", T1.c_link_url");			/* 1 リンクURL */
	strcat(cha_sql, ", T1.b_link_target");		/* 2 ターゲット */
	strcat(cha_sql, " from at_linkitem T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_linkgroup_id = %d", in_group_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_linkitem_order asc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<td class=\"side-links\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	if(!in_row) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Format_Nlist(nlp_out, "SIDEBAR", "	<td width=\"10\" valign=\"top\">*</td>\n", g_cha_theme_image);
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">リンク集にリンクが設定されていません。</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
	} else {
		for(i = 0; i < in_row; i++) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Format_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n", g_cha_theme_image);
			chp_tmp = Db_GetValue(dbres, i, 0);
			if(!chp_tmp || !*chp_tmp) {
				Put_Nlist(nlp_out, "ERROR", "リンクのタイトルを取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_title = Escape_HtmlString(chp_tmp);
			chp_url = Db_GetValue(dbres, i, 1);
			if(!chp_url || !*chp_url){
				free(chp_title);
				Put_Nlist(nlp_out, "ERROR", "リンクのURLを取得できませんでした。");
				Db_CloseDyna(dbres);
				return 1;
			}
			memset(cha_html, '\0', sizeof(cha_html));
			chp_tmp = Db_GetValue(dbres, i, 2);
			if(chp_tmp && atoi(chp_tmp)) {
				sprintf(cha_html, "<td><a href=\"%s\" target=\"_blank\">%s</a></td>\n"
					, chp_url, chp_title);
			} else {
				sprintf(cha_html, "<td><a href=\"%s\">%s</a></td>\n"
					, chp_url, chp_title);
			}
			free(chp_title);
			Put_Nlist(nlp_out, "SIDEBAR", cha_html);
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		}
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</td>\n</tr>\n");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_profile()
 * Description:
 *	プロフィールのサイドコンテンツを作成する
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_profile(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_html[8192];
	char cha_sql[512];

	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n<td class=\"side-profile\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	if (g_in_dbb_mode) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"center\">\n");
		if (Set_My_Avatar(db, nlp_out, "SIDEBAR", in_blog)) {
			return 1;
		}
		Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	} else {
		strcpy(cha_sql, "select T1.c_author_nickname");	/* 0 作者ニックネーム */
		strcat(cha_sql, ", T1.c_author_image");		/* 1 作者イメージ */
		strcat(cha_sql, ", T1.c_author_selfintro");	/* 2 作者について */
		strcat(cha_sql, " from at_profile T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		chp_tmp2 = Db_GetValue(dbres, 0, 1);
		if(chp_tmp && *chp_tmp) {
			memset(cha_html, '\0', sizeof(cha_html));
			chp_escape = Escape_HtmlString(chp_tmp);
			sprintf(cha_html, "<tr><td><b>このブログの著者</b></td></tr>\n<tr><td style=\"padding:0 0 4px 8px;\">%s</td></tr>\n", chp_escape);
			Put_Nlist(nlp_out, "SIDEBAR", cha_html);
			free(chp_escape);
		}
		if(chp_tmp2 && *chp_tmp2) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr><td>-IMAGE-</td></tr>\n");
			if(chp_tmp && *chp_tmp) {
				chp_escape = Escape_HtmlString(chp_tmp);
				if (g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR",
						"<tr><td><a href=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%s\" title=\"%s\">"
						"<img src=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%s\" alt=\"%s\" width=\"120\" height=\"90\" border=\"0\">"
						"</a></td></tr>\n",
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, chp_escape,
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp, chp_escape);
				} else {
					Put_Format_Nlist(nlp_out, "SIDEBAR",
						"<tr><td><a href=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%d\" title=\"%s\">"
						"<img src=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%d\" alt=\"%s\" width=\"120\" height=\"90\" border=\"0\">"
						"</a></td></tr>\n",
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, chp_escape,
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, chp_escape);
				}
				free(chp_escape);
			} else {
				if (g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR",
						"<tr><td><a href=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%s\" title=\"author_image\">"
						"<img src=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%s\" alt=\"author_image\" width=\"120\" height=\"90\">"
						"</a></td></tr>\n",
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp,
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, g_cha_blog_temp);
				} else {
					Put_Format_Nlist(nlp_out, "SIDEBAR",
						"<tr><td><a href=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%d\" title=\"author_image\">"
						"<img src=\"%s%s%s/%s?BTN_DISP_AUTHOR_IMAGE=1&bid=%d\" alt=\"author_image\" width=\"120\" height=\"90\">"
						"</a></td></tr>\n",
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog,
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
				}
			}
		}
		chp_tmp = Db_GetValue(dbres, 0, 2);
		if(chp_tmp && *chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td>-ABOUT AUTHOR-</td></tr>\n<tr><td><b>%s</td></tr>\n", chp_escape);
			free(chp_escape);
		}
		Db_CloseDyna(dbres);
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n</td>\n</tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_rss()
 * Description:
 *	RSSのサイドコンテンツを読み込む
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_rss(DBase *db, NLIST *nlp_out, int in_sidecontent_id, int in_blog)
{
	DBRes *dbres;
	CP_NKF nkf;
	char *chp_uri;
	char *chp_count;
	char *chp_head;
	char *chp_tail;
	char *chp_tmp;
	char *chp_escape;
	char *chp_escape2;
	char *chp_head_article;
	char *chp_tail_article;
	char *chp_inbuff;
	char *chp_outbuff;
	char *chp_top;
	char cha_html[8192];
	char cha_item[32768];
	char cha_article[8192];
	char cha_title[1024];
	char cha_link[512];
	char cha_hostname[256];
	char cha_pathname[512];
	char cha_sql[1024];
	int in_target;	/* ターゲットウィンドウ */
	int in_inbyte;	/* 変換前データ長 */
	int in_outbyte;	/* 変換後データ長 */
	int in_count;	/*サイドバーで表示するRSSの記事数 */
	int in_rss_version;
	int in_item;	/*RSS内の実際の記事数 */
	int i;

	/* RSSのURLを取る */
	strcpy(cha_sql, "select T1.c_rss_url");	/* 0 RSS送信元URL */
	strcat(cha_sql, ", T1.n_rss_article");	/* 1 RSS記事数 */
	strcat(cha_sql, ", T1.b_rss_target");	/* 2 ターゲット */
	strcat(cha_sql, " from at_rss T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_rss_id = %d", in_sidecontent_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "RSSのURLを取得するクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_uri = Db_GetValue(dbres, 0, 0);
	if(!chp_uri || !*chp_uri) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "RSSのURLを取得できませんでした。");
		return 1;
	}
	chp_count = Db_GetValue(dbres, 0, 1);
	if(!chp_count || atoi(chp_count) < 1) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "RSS取込の表示件数の設定に誤りがあります。");
		return 1;
	}
	/* 頭を貼り付ける */
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<td class=\"side-rss-import\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");

	memset(cha_hostname, '\0', sizeof(cha_hostname));
	memset(cha_pathname, '\0', sizeof(cha_pathname));
	if (sscanf(chp_uri, "http://%[^/]%s", cha_hostname, cha_pathname ) < 2 &&
		sscanf(chp_uri, "https://%[^/]%s", cha_hostname, cha_pathname ) < 2) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">URLを確認してください。</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table></td></tr>\n");
		return 0;
	}
	in_count = atoi(chp_count);
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if(chp_tmp && atoi(chp_tmp)) {
		in_target = atoi(chp_tmp);
	} else {
		in_target = 0;
	}
	Db_CloseDyna(dbres);

	/* RSSファイル内容をバッファへ取得 */
	Http_Get_Get_Result(cha_hostname, cha_pathname, "", NULL, &chp_inbuff);
	if(!chp_inbuff || strlen(chp_inbuff) == 0) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">URLを確認してください。</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table></td></tr>\n");
		return 0;
	}
	if(!strstr(chp_inbuff, "</channel>") && !strstr(chp_inbuff, "</feed>")) {
		chp_head = strstr(chp_inbuff, "Location: ");
		if (chp_head) {
			chp_tmp = strchr(chp_head, '\r');
			if (chp_tmp)
				*chp_tmp = '\0';
			chp_tmp = strchr(chp_head, '\n');
			if (chp_tmp)
				*chp_tmp = '\0';
			if (sscanf(chp_head + strlen("Location: "), "http://%[^/]%s", cha_hostname, cha_pathname ) < 2) {
				Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">URLを確認してください。</td>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "</table></td></tr>\n");
				return 0;
			}
			Http_Get_Get_Result(cha_hostname, cha_pathname, "", NULL, &chp_inbuff);
			if(!chp_inbuff || strlen(chp_inbuff) == 0) {
				Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">URLを確認してください。</td>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
				Put_Nlist(nlp_out, "SIDEBAR", "</table></td></tr>\n");
				return 0;
			}
		}
	}

	nkf = Create_Nkf_String(chp_inbuff);
	Set_Output_Code(nkf, NKF_EUC);
	Convert_To_String(nkf, &chp_outbuff);
	Destroy_Nkf_Engine(nkf);
	in_inbyte = strlen(chp_inbuff);
	in_outbyte = strlen(chp_outbuff);

	if(strlen(chp_outbuff) == 0) {
		free(chp_outbuff);
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">データは空です。もう一度URLを確認してください。</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table></td></tr>\n");
		/* エラー画面にはしない */
		return 0;
	}
	/* 出力データの解析,RSS生成 */
	/* title */
	chp_head = strstr(chp_outbuff, "<rss version");
	if(chp_head) {
		in_rss_version = CO_RSS_VERSION_091;
	} else {
		chp_head = strstr(chp_outbuff, "<feed version");
		if(chp_head) {
			in_rss_version = CO_ATOM_VERSION_03;
		} else {
			in_rss_version = CO_RSS_VERSION_10;
		}
	}
	/* 途中でフォーマットの異常によりbreakしているところがあるが、 */
	/* あくまでもRSS送信元が悪いと判断します。 */
	/* そのため、エラー画面にはせず、エラーメッセージのみサイドバーに続けて表示します。 */
	switch(in_rss_version) {
	/* RSS version 1.0 */
	case CO_RSS_VERSION_10:
		/* <item>の数を数える */
		chp_top = NULL;
		in_item = 0;
		chp_top = strstr(chp_outbuff, "</item>");
		if(chp_top) {
			in_item++;
			while(chp_top) {
				chp_top = strstr(chp_top + strlen("</item>"), "</item>");
				if(!chp_top) {
					break;
				}
				in_item++;
			}
		}
		/* channel RSS送信元情報 */
		chp_head = strstr(chp_outbuff, "<channel rdf");
		if(!chp_head) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS1.0) head of channel missing...</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		chp_tail = strstr(chp_outbuff, "</channel>");
		if(!chp_tail){
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS1.0) tail of channel missing...</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* title */
		chp_head = strstr(chp_outbuff, "<title");
		chp_tail = strstr(chp_outbuff, "</title>");
		if(!chp_head || !chp_tail) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS1.0) Illeagl format: Can't find title.</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* link */
		chp_head = strstr(chp_outbuff, "<link");
		chp_tail = strstr(chp_outbuff, "</link>");
		if(!chp_head || !chp_tail) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS1.0) Illeagl format: Can't find link.</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* description */
		chp_head = strstr(chp_outbuff, "<description");
		chp_tail = strstr(chp_outbuff, "</description>");
		if(!chp_head || !chp_tail) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS1.0) Illeagl format: Can't find description.</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* item rdf:about 各記事 */
		chp_head = strstr(chp_outbuff, "<item rdf:about");
		chp_tail = NULL;
		/* item切り出し */
		for(i = 0; chp_head && i < in_count && i < in_item; i++) {
			/* iが0のときは、頭から検索。それ以外のときは、前の続きから検索 */
			if(i != 0) {
				chp_head = strstr(chp_tail, "<item rdf:about");
			} else {
				chp_head = strstr(chp_outbuff, "<item rdf:about");
			}
			chp_tail = strstr(chp_head, "</item>");
			/* 終了(実際は先に</item>を数えているためここで終わることはまずない) */
			if(!chp_head || !chp_tail) {
				break;
			}
			/* <item rdf: .... </item>の部分を切り出す */
			memset(cha_item, '\0', sizeof(cha_item));
			strncpy(cha_item, chp_head, (int)(chp_tail - chp_head));
			/* 記事の内容 */
			/* <item> 内の<link>を切り出す */
			chp_head_article = strstr(cha_item, "<link");
			chp_tail_article = strstr(cha_item, "</link>");
			memset(cha_link, '\0', sizeof(cha_link));
			if(chp_head_article && chp_tail_article) {
				memset(cha_article, '\0', sizeof(cha_article));
				memcpy(cha_article, chp_head_article, chp_tail_article - chp_head_article);
				strcpy(cha_link, strstr(cha_article, ">") + strlen(">"));
			}
			/* <item> 内の<title>を切り出す */
			chp_head_article = strstr(cha_item, "<title");
			chp_tail_article = strstr(cha_item, "</title>");
			memset(cha_title, '\0', sizeof(cha_title));
			if(chp_head_article && chp_tail_article){
				memset(cha_article, '\0', sizeof(cha_article));
				memcpy(cha_article, chp_head_article, chp_tail_article - chp_head_article);
				strcpy(cha_title, strstr(cha_article, ">") + strlen(">"));
			}
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			chp_escape = Remove_HtmlTags(cha_link);
			chp_escape2 = Remove_HtmlTags(cha_title);
			memset(cha_html, '\0', sizeof(cha_html));
			if(in_target) {
				sprintf(cha_html, "<td><a href=\"%s\" %s>%s</a></td>\n"
						, chp_escape, CO_TARGET_BLANK, chp_escape2[0] ? chp_escape2 : "(タイトルなし)");
			} else {
				sprintf(cha_html, "<td><a href=\"%s\">%s</a></td>\n"
					, chp_escape, chp_escape2[0] ? chp_escape2 : "(タイトルなし)");
			}
			free(chp_escape);
			free(chp_escape2);
			Put_Nlist(nlp_out, "SIDEBAR", cha_html);
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		}
		break;
	/* ver.0.91もしくは2.0のとき */
	case CO_RSS_VERSION_091:
		chp_top = NULL;
		in_item = 0;
		/* item数を数える */
		chp_top = strstr(chp_outbuff, "<item>");
		if(chp_top) {
			in_item++;
			while(chp_top) {
				chp_top = strstr(chp_top + strlen("<item>"), "<item>");
				if(!chp_top) {
					break;
				}
				in_item++;
			}
		}
		chp_tail = strstr(chp_outbuff, "</channel>");
		if(!chp_tail) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS0.91/2.0)tail channel missing...</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* title */
		chp_head = strstr(chp_outbuff, "<title");
		chp_tail = strstr(chp_outbuff, "</title>");
		if(!chp_head || !chp_tail){
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS0.91/2.0)Illeagl format: Can't find title.</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* link */
		chp_head = strstr(chp_outbuff, "<link");
		chp_tail = strstr(chp_outbuff, "</link>");
		if(!chp_head || !chp_tail){
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(RSS0.91/2.0)Illeagl format: Can't find title.</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* item 各記事 */
		chp_head = strstr(chp_outbuff, "<item>");
		chp_tail = NULL;
		for( i = 0; chp_head && i < in_count && i < in_item; i++) {
			/* 各<item>の頭を特定 */
			if(i != 0) {
				chp_head = strstr(chp_tail, "<item>");
			} else {
				chp_head = strstr(chp_outbuff, "<item>");
			}
			chp_tail = strstr(chp_head, "</item>");
			if(!chp_head || !chp_tail){
				break;
			}
			memset(cha_item, '\0', sizeof(cha_item));
			strncpy(cha_item, chp_head, (int)(chp_tail - chp_head));
			chp_head_article = strstr(cha_item, "<link>");
			chp_tail_article = strstr(cha_item, "</link>");
			memset(cha_link, '\0', sizeof(cha_link));
			if(chp_head_article && chp_tail_article){
				memset(cha_article, '\0', sizeof(cha_article));
				strncpy(cha_article, chp_head_article, (int)(chp_tail_article - chp_head_article));
				strcpy(cha_link, strstr(cha_article, ">") + strlen(">"));
			}
			chp_head_article = strstr(cha_item, "<title>");
			chp_tail_article = strstr(cha_item, "</title>");
			memset(cha_title, '\0', sizeof(cha_title));
			if(chp_head_article && chp_tail_article){
				memset(cha_article, '\0', sizeof(cha_article));
				strncpy(cha_article, chp_head_article, (int)(chp_tail_article - chp_head_article));
				strcpy(cha_title, strstr(cha_article, ">") + strlen(">"));
			}
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			memset(cha_html, '\0', sizeof(cha_html));
			chp_escape = Remove_HtmlTags(cha_link);
			chp_escape2 = Remove_HtmlTags(cha_title);
			if(in_target) {
				sprintf(cha_html, "<td><a href=\"%s\" %s>%s</a></td>\n"
					, chp_escape, CO_TARGET_BLANK, chp_escape2);
			} else {
				sprintf(cha_html, "<td><a href=\"%s\">%s</a></td>\n"
					, chp_escape, chp_escape2);
			}
			free(chp_escape);
			free(chp_escape2);
			Put_Nlist(nlp_out, "SIDEBAR", cha_html);
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		}
		break;
	/* atom0.3 のとき */
	case CO_ATOM_VERSION_03:
		chp_top = NULL;
		in_item = 0;
		chp_top = strstr(chp_outbuff, "<entry>");
		/* item数(<entry>の数)を得る */
		if(chp_top) {
			in_item++;
			while(chp_top) {
				chp_top = strstr(chp_top + strlen("<entry>"), "<entry>");
				if(!chp_top){
					break;
				}
				in_item++;
			}
		}
		chp_tail = strstr(chp_outbuff, "</feed>");
		if(!chp_tail){
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">(Atom0.3)tail channel missing...</td>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
			break;
		}
		/* entry 各記事 */
		chp_head = strstr(chp_outbuff, "<entry");
		chp_tail = NULL;
		for(i = 0; chp_head && i < in_count && i < in_item; i++) {
			if(i != 0) {
				chp_head = strstr(chp_tail, "<entry");
			}
			chp_tail = strstr(chp_head, "</entry>");
			if(!chp_head || !chp_tail){
				break;
			}
			memset(cha_item, '\0', sizeof(cha_item));
			strncpy(cha_item, chp_head, (int)(chp_tail - chp_head));
			chp_head_article = strstr(cha_item, "<link");
			chp_tail_article = strstr(cha_item, "/>");
			memset(cha_link, '\0', sizeof(cha_link));
			if(chp_head_article && chp_tail_article){
				memset(cha_article, '\0', sizeof(cha_article));
				strncpy(cha_article, chp_head_article, (int)(chp_tail_article - chp_head_article));
				chp_tmp = strstr(cha_article, "href=");
				sscanf(chp_tmp, "href=\"%[^\"]\"", cha_link);
			}
			chp_head_article = strstr(cha_item, "<title>");
			chp_tail_article = strstr(cha_item, "</title>");
			memset(cha_title, '\0', sizeof(cha_title));
			if(chp_head_article && chp_tail_article) {
				memset(cha_article, '\0', sizeof(cha_article));
				strncpy(cha_article, chp_head_article, (int)(chp_tail_article - chp_head_article));
				strcpy(cha_title, strstr(cha_article, ">") + strlen(">"));
			}
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
			chp_escape = Remove_HtmlTags(cha_link);
			chp_escape2 = Remove_HtmlTags(cha_title);
			if(in_target) {
				sprintf(cha_html, "<td><a href=\"%s\" %s>%s</a></td>\n", chp_escape, CO_TARGET_BLANK, chp_escape2);
			} else {
				sprintf(cha_html, "<td><a href=\"%s\">%s</a></td>\n", chp_escape, chp_escape2);
			}
			free(chp_escape);
			free(chp_escape2);
			Put_Nlist(nlp_out, "SIDEBAR", cha_html);
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		}
		break;
	default:
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\" width=\"10\">*</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td valign=\"top\">Unknown format.</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		break;
	}
	free(chp_outbuff);
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n</td>\n</tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_sidecontent_blogparts()
 * Description:
 *	ブログパーツを構築する
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_blogparts(DBase *db, NLIST *nlp_out, int in_sidecontent_id, int in_blog)
{
	DBRes *dbres;
	char cha_sql[512];
	char *chp_tmp;

	sprintf(cha_sql, "select c_parts_body from at_parts where n_blog_id = %d and n_parts_id = %d", in_blog, in_sidecontent_id);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ブログ情報を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-blog-parts\">\n");
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\"><tr><td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", chp_tmp);
		Put_Nlist(nlp_out, "SIDEBAR", "\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table></td></tr>\n");
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidecontent_about_blog()
 * Description:
 *	ブログについてのサイドコンテンツを作成する
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_about_blog(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_br;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];
	int in_browser;

	in_browser = Get_Browser();
	strcpy(cha_sql, "select T1.c_blog_title");
	strcat(cha_sql, ", T1.c_blog_description");
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ブログ情報を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<td class=\"side-about-blog\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	Put_Format_Nlist(nlp_out, "SIDEBAR", "<form method=\"post\" action=\"%s\">\n", CO_CGI_BUILD_HTML);
	if (g_in_dbb_mode) {
		Build_HiddenEncode(nlp_out, "SIDEBAR", "bid", g_cha_blog_temp);
	} else {
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"bid\" value=\"%d\">\n", in_blog);
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td><b>タイトル</b></td></tr>\n<tr><td style=\"padding:0 0 4px 8px;\">%s</td></tr>\n", chp_escape);
		free(chp_escape);
	}
	chp_tmp = Db_GetValue(dbres, 0 , 1);
	if(chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		chp_br = Conv_Blog_Br(chp_escape);
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td><b>このブログについて</b></td></tr>\n<tr><td style=\"padding:0 0 4px 8px;\">%s</td></tr>\n", chp_br);
		free(chp_br);
		free(chp_escape);
	}
	if (g_in_dbb_mode) {
		if (!Get_Nickname_From_Blog(db, nlp_out, in_blog, cha_sql)) {
			return 1;
		}
		chp_escape = Escape_HtmlString(cha_sql);
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td><b>このブログのオーナー</b></td></tr>\n<tr><td style=\"padding:0 0 4px 8px;\">%s</td></tr>\n", chp_escape);
		free(chp_escape);
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</form>\n</table>\n</td>\n</tr>\n");
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_rss_feed()
 * Description:
 *	RSSFEEDのサイドコンテンツを作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_rss_feed(NLIST *nlp_out, int in_blog)
{
	char *chp_server;

	chp_server = getenv("SERVER_NAME");
	if(!chp_server || !*chp_server) {
		Put_Nlist(nlp_out, "ERROR", "パスの取得に失敗しました。");
		return 1;
	}
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<td class=\"side-rss-import\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<td valign=\"top\" width=\"10\">*</td>\n");
	Put_Format_Nlist(nlp_out, "SIDEBAR",
		"<td><a href=\"%s%s%s/%08d.rdf\"><img src=\"%s/feed_rss.gif\" border=\"0\" align=\"absmiddle\"><br>(RSS 1.0)</a></td>\n",
		g_cha_protocol, chp_server, g_cha_rss_location, in_blog, g_cha_user_image);
	Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n</table>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</td>\n</tr>\n");

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_rss_feed()
 * Description:
 *	RSSFEEDのサイドコンテンツを作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_blog_search(NLIST *nlp_out, int in_blog)
{
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<td class=\"side-blog-search\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	Put_Format_Nlist(nlp_out, "SIDEBAR", "<form method=\"post\" action=\"%s\">\n", CO_CGI_BUILD_HTML);
	if (g_in_dbb_mode) {
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"bid\" value=\"%s\">\n", g_cha_blog_temp);
	} else {
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"bid\" value=\"%d\">\n", in_blog);
	}
	Put_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"BTN_BLOG_SEARCH\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<td><input type=\"text\" name=\"search_string\" maxlength=\"256\"><input type=\"submit\" name=\"BTN_BLOG_SEARCH\" value=\"検索\"></td></tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td nowrap>");
	Put_Nlist(nlp_out, "SIDEBAR", "<input type=\"checkbox\" name=\"search_subject\" value=\"1\">件名");
	Put_Nlist(nlp_out, "SIDEBAR", "<input type=\"checkbox\" name=\"search_content\" value=\"1\" checked>本文");
	Put_Nlist(nlp_out, "SIDEBAR", "<input type=\"checkbox\" name=\"search_comment\" value=\"1\">コメント");
	if (g_in_dbb_mode) {
		Put_Nlist(nlp_out, "SIDEBAR", "<input type=\"checkbox\" name=\"search_trackback\" value=\"1\">引用記事");
	} else {
		Put_Nlist(nlp_out, "SIDEBAR", "<input type=\"checkbox\" name=\"search_trackback\" value=\"1\">トラックバック");
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr></form>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_visitor_list()
 * Description:
 *	訪問者リストを作成
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_visitor_list(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_owner_room[256] = {0};
	char cha_sql[512];
	char *chp_tmp;
	char *chp_esc;
	int in_cnt;
	int i;

	if (g_in_dbb_mode) {
		sprintf(cha_sql, "select c_dbb_disp_owner_room_cgi from sy_authinfo");
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "訪問リストを得るクエリに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			strcpy(cha_owner_room, chp_tmp);
		}
		Db_CloseDyna(dbres);
	}

	sprintf(cha_sql,
		" select "
		" T2.c_nickname"
		",T2.n_visitor_id"
		" from"
		" at_visitor T2"
		" where T2.n_blog_id=%d"
		" order by T2.d_visit_time desc"
		" limit 10", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "訪問リストを得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}

	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<td class=\"side-visitor-list\">\n");
	in_cnt = Db_GetRowCount(dbres);
	if (!in_cnt) {
		Put_Nlist(nlp_out, "SIDEBAR", "<ul>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<li>訪問者はまだいません。</li>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</ul>\n");
	} else {
		Put_Nlist(nlp_out, "SIDEBAR", "<ul>\n");
		for (i = 0; i < in_cnt; ++i) {
			chp_tmp = Db_GetValue(dbres, i, 0);
			chp_esc = Escape_HtmlString(chp_tmp);
			chp_tmp = Conv_Long_Ascii(chp_esc, 12);
			free(chp_esc);
			if (atoi(Db_GetValue(dbres, i, 1)) != INT_MAX) {
				if (g_in_dbb_mode) {
					if (!Blog_To_Temp(db, nlp_out, atoi(Db_GetValue(dbres, i, 1)), cha_sql)) {
						Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s?owner=%s\" target=\"_blank\">%s</a></li>\n"
							, cha_owner_room, cha_sql, chp_tmp);
					} else {
						Put_Format_Nlist(nlp_out, "SIDEBAR", "<li>%s</li>\n", chp_tmp);
					}
				} else if (g_in_short_name) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%08d/\">%s</a></li>\n"
						, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, atoi(Db_GetValue(dbres, i, 1)), chp_tmp);
				} else {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?bid=%s\">%s</a></li>\n"
						, g_cha_user_cgi, CO_CGI_BUILD_HTML, Db_GetValue(dbres, i, 1), chp_tmp);
				}
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li>%s</li>\n", chp_tmp);
			}
			free(chp_tmp);
		}
		Put_Nlist(nlp_out, "SIDEBAR", "</ul>\n");
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<table border=\"0\" width=\"100%%\"><tr><td align=\"right\"><a href=\"%s%s%s/%s/?BTN_DISP_VISITOR_LIST=1\">...more</a></td></tr></table>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<table border=\"0\" width=\"100%%\"><tr><td align=\"right\"><a href=\"%s%s%s/%08d/?BTN_DISP_VISITOR_LIST=1\">...more</a></td></tr></table>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<table border=\"0\" width=\"100%%\"><tr><td align=\"right\"><a href=\"%s/%s?bid=%d&BTN_DISP_VISITOR_LIST=1\">...more</a></td></tr></table>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_admin_menu()
 * Description:
 *	RSSFEEDのサイドコンテンツを作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_admin_menu(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	int in_browser;
	int in_entry_count;

	if (g_in_login_blog > 0) {
		in_entry_count = Get_Total_Entry(db, nlp_in, nlp_out, 0, in_blog);
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-admin-menu\">\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<ul>\n");
		if (g_in_login_blog == in_blog || (g_in_hb_mode && g_in_blog_auth >= CO_AUTH_DELETE)) {
			if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_NEWENTRY=1&from_user=1&blogid=%d\">新たに記事を投稿する</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY, in_blog);
				if (in_entry_count > 0) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_ENTRYLIST=1&from_user=1&blogid=%d\">記事を編集・削除する</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY_LIST, in_blog);
				}
				if (!g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_CATEGORY=1&from_user=1&blogid=%d\">カテゴリーの設定</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_CATEGORY, in_blog);
				}
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_CONFIGURE=1&from_user=1&blogid=%d\">Blog全体に関する設定</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_CONFIGURE, in_blog);
				if (!g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_PROFILE=1&sidecontent=6:6&from_user=1&blogid=%d\">プロフィールの編集</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_SIDECONTENT, in_blog);
				}
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_SETTING_SIDECONTENT=1&from_user=1&blogid=%d\">サイドバーの設定</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_SIDECONTENT, in_blog);
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_SETTING_LOOKS=1&from_user=1&blogid=%d\">デザインの設定</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_LOOKS, in_blog);
				in_browser = Get_Browser();
				if (!g_in_dbb_mode && in_browser != CO_BROWSER_NS47 && in_browser != CO_BROWSER_OTHER) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_SETTING_BOOKMARKLET=1&from_user=1&blogid=%d\">ブックマークレットの設定</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_BOOKMARKLET, in_blog);
				}
				if (!g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_BLOG_EXPORT=1&from_user=1&blogid=%d\">保存(ダウンロード)</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_BLOG_IMPORT=1&from_user=1&blogid=%d\">記事のアップロード</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU, in_blog);
				}
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_NEWENTRY=1&from_user=1&blogid=%d\">新たに記事を投稿する</a></li>\n", g_cha_admin_cgi, CO_CGI_ENTRY, in_blog);
				if (in_entry_count > 0) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_ENTRYLIST=1&from_user=1&blogid=%d\">記事を編集・削除する</a></li>\n", g_cha_admin_cgi, CO_CGI_ENTRY_LIST, in_blog);
				}
				if (!g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_CATEGORY=1&from_user=1&blogid=%d\">カテゴリーの設定</a></li>\n", g_cha_admin_cgi, CO_CGI_CATEGORY, in_blog);
				}
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_CONFIGURE=1&from_user=1&blogid=%d\">Blog全体に関する設定</a></li>\n", g_cha_admin_cgi, CO_CGI_CONFIGURE, in_blog);
				if (!g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_PROFILE=1&sidecontent=6:6&from_user=1&blogid=%d\">プロフィールの編集</a></li>\n", g_cha_admin_cgi, CO_CGI_SIDECONTENT, in_blog);
				}
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_SETTING_SIDECONTENT=1&from_user=1&blogid=%d\">サイドバーの設定</a></li>\n", g_cha_admin_cgi, CO_CGI_SIDECONTENT, in_blog);
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_SETTING_LOOKS=1&from_user=1&blogid=%d\">デザインの設定</a></li>\n", g_cha_admin_cgi, CO_CGI_LOOKS, in_blog);
				in_browser = Get_Browser();
				if (!g_in_dbb_mode && in_browser != CO_BROWSER_NS47 && in_browser != CO_BROWSER_OTHER) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_SETTING_BOOKMARKLET=1&from_user=1&blogid=%d\">ブックマークレットの設定</a></li>\n", g_cha_admin_cgi, CO_CGI_BOOKMARKLET, in_blog);
				}
				if (!g_in_dbb_mode) {
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_BLOG_EXPORT=1&from_user=1&blogid=%d\">保存(ダウンロード)</a></li>\n", g_cha_admin_cgi, CO_CGI_MENU, in_blog);
					Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_BLOG_IMPORT=1&from_user=1&blogid=%d\">記事のアップロード</a></li>\n", g_cha_admin_cgi, CO_CGI_MENU, in_blog);
				}
			}
		} else if (g_in_hb_mode && g_in_blog_auth >= CO_AUTH_WRITE) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_NEWENTRY=1&blogid=%d&from_user=1&blogid=%d\">新たに記事を投稿する</a></li>\n", g_cha_admin_cgi, CO_CGI_ENTRY, in_blog);
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_ENTRYLIST=1&blogid=%d&from_user=1&blogid=%d\">記事を編集・削除する</a></li>\n", g_cha_admin_cgi, CO_CGI_ENTRY_LIST, in_blog);
		}
		if (!g_in_need_login && !g_in_cart_mode) {
			if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s%s%s/%s?BTN_DISP_LOGOFF=1&from_user=1\">ログアウト</a></li>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_MENU);
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<li><a href=\"%s/%s?BTN_DISP_LOGOFF=1&from_user=1\">ログアウト</a></li>\n", g_cha_admin_cgi, CO_CGI_MENU);
			}
		}
		Put_Nlist(nlp_out, "SIDEBAR", "</ul>\n");
	} else {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-admin-login\">\n");
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<form method=\"post\" action=\"%s\">\n", CO_CGI_BUILD_HTML);
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"bid\" value=\"%s\">\n", g_cha_blog_temp);
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"bid\" value=\"%d\">\n", in_blog);
		}
		Put_Nlist(nlp_out, "SIDEBAR", "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\">\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td>ログインID</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td><input type=\"text\" name=\"btn_id\" maxlength=\"100\" style=\"font-size:9pt; width:100px;\" onfocus=\"this.select();\"></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td>パスワード</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td><input type=\"password\" name=\"btn_pass\" maxlength=\"100\" style=\"font-size:9pt; width:100px;\" onfocus=\"this.select();\"></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td align=\"right\" valign=\"bottom\" colspan=\"2\"><input type=\"submit\" name=\"BTN_ADMIN_LOGIN\" value=\"ログイン\"></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
		Put_Nlist(nlp_out, "SIDEBAR" ,"</form>\n");
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_access_log()
 * Description:
 *	アクセスカウンタの表示
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_access_log(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[1024];
	char cha_to[32];
	char cha_from[32];
	char *chp_tmp;
	int in_page_view;
	int in_unique_user;
	int in_today_all;
	int in_today_unique;
	int in_count;

	Get_NowDate(cha_from);
	cha_from[4] = '-';
	cha_from[7] = '-';
	strcpy(cha_to, cha_from);

	strcat(cha_from, " 00:00:00");
	strcat(cha_to, " 23:59:59");

	sprintf(cha_sql,
		" select n_page_view + n_page_view_all"
		",n_unique_user + n_unique_user_all"
		" from at_profile"
		" where n_blog_id = %d",
		in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_page_view = atoi(chp_tmp);
	} else {
		in_page_view = 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		in_unique_user = atoi(chp_tmp);
	} else {
		in_unique_user = 0;
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		" select count(*)"
		" from at_view"
		" where n_blog_id = %d"
		" and d_view >= '%s'"
		" and d_view <= '%s'"
		, in_blog, cha_from, cha_to);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_today_all = atoi(chp_tmp);
	} else {
		in_today_all = 0;
	}
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		" select count(*)"
		" from at_view"
		" where n_blog_id = %d"
		" and d_view >= '%s'"
		" and d_view <= '%s'"
		" and b_valid != 0"
		, in_blog, cha_from, cha_to);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_today_unique = atoi(chp_tmp);
	} else {
		in_today_unique = 0;
	}
	Db_CloseDyna(dbres);

	Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<td class=\"side-access-counter\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "	<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">");
	Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"left\"><b>・</b>アクセス数(累計)</td><td align=\"right\">%d</td></tr>\n", in_page_view + in_today_all);
	Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"left\"><b>・</b>訪問者数(累計)</td><td align=\"right\">%d</td></tr>\n", in_unique_user + in_today_unique);
	Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"left\"><b>・</b>アクセス数(本日)</td><td align=\"right\">%d</td></tr>\n", in_today_all);
	Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"left\"><b>・</b>訪問者数(本日)</td><td align=\"right\">%d</td></tr>\n", in_today_unique);
	if (g_in_dbb_mode) {
		long in_seq;
		sprintf(cha_sql, "select n_rank from at_blog_ranking where n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "アクセス数を得るクエリに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			in_seq = atol(chp_tmp);
		} else {
			in_seq = 0;
		}
		Db_CloseDyna(dbres);

		if (in_seq) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"left\"><b>・</b>ランキング</td><td align=\"right\">%d</td></tr>\n", in_seq);
		} else {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"left\"><b>・</b>ランキング</td><td align=\"right\">−</td></tr>\n");
		}

		in_count = Count_Favorite_Blog(db, nlp_out, in_blog);
		if (in_count < 0) {
			return 1;
		}
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<tr><td align=\"left\"><b>・</b>お気に入り登録数</td><td align=\"right\">%d</td></tr>\n", in_count);
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_item_info()
 * Description:
 *	商品情報の表示
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_item_info(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	int in_ret;
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-item-info\">\n");
	in_ret = Build_Item_Info(db, nlp_in, nlp_out, in_blog);
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");

	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_entry_recomend()
 * Description:
 *	商品情報の表示
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_entry_recomend(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	int in_error;
	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-entry-recomend\">\n");
	in_error = Build_Entry_Recomend(db, nlp_in, nlp_out, in_blog);
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");

	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_cart_login()
 * Description:
 *	RSSFEEDのサイドコンテンツを作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_cart_login(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	NLIST *nlp_cookie;
	DBase *cartdb;
	DBRes *dbres;
	int in_login;
	char cha_sql[512];

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "カートDBを開けませんでした。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	nlp_cookie = Get_Cookie();
	Get_Cart_Handytype();
	in_login = Is_Logged_In(OldDBase(cartdb), nlp_in, nlp_cookie);

	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-cart-login\">\n");
	if (in_login) {
		sprintf(cha_sql, "select T3.c_name from st_customer T3 where T3.n_customer_no = %s", Get_Nlist(nlp_cookie, "LOGINID", 1));
		dbres = Db_OpenDyna(cartdb, cha_sql);
		if(!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "%d：設定情報取得に失敗しました。(%s)<br>", __LINE__, Gcha_last_error);
			Db_Disconnect(cartdb);
			Finish_Nlist(nlp_cookie);
			return 1;
		}
		Put_Nlist(nlp_out, "SIDEBAR", "<table cellpadding=\"0\" cellspacing=\"1\" border=\"0\" align=\"center\">\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<td align=\"center\" target=\"blank\" nowrap><a href=\"%s/ex_index.cgi\">いらっしゃいませ</a></td>\n", g_cha_user_cgi);
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<td>%sさん</td>\n", Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "ゲスト");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<td><b>現在ログイン中です。</b></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
		Db_CloseDyna(dbres);
	} else {
		Put_Nlist(nlp_out, "SIDEBAR", "<table cellpadding=\"0\" cellspacing=\"1\" border=\"0\" align=\"center\">\n");
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<form method=\"post\" action=\"%s\">\n", CO_CGI_BUILD_HTML);
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"bid\" value=\"%s\">\n", g_cha_blog_temp);
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<input type=\"hidden\" name=\"bid\" value=\"%d\">\n", in_blog);
		}
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<td colspan=\"2\" target=\"blank\" nowrap><a href=\"%s/ex_index.cgi\">いらっしゃいませ</a></td>\n", g_cha_user_cgi);
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td colspan=\"2\"><b>ゲストさん</b></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td style=\"padding-right:4px;\">ログインID</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td><input type=\"text\" name=\"btn_id\" maxlength=\"100\" style=\"font-size:9pt; width:100px;\" onfocus=\"this.select();\"></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td style=\"padding-right:4px;\">パスワード</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td><input type=\"password\" name=\"btn_pass\" maxlength=\"100\" style=\"font-size:9pt; width:100px;\" onfocus=\"this.select();\"></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "	<td align=\"right\" valign=\"bottom\" colspan=\"2\"><input type=\"submit\" name=\"BTN_CART_LOGIN\" value=\"ログイン\"></td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR" ,"</form>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	Finish_Nlist(nlp_cookie);
	Db_Disconnect(cartdb);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_sidecontent_regist_customer()
 * Description:
 *	RSSFEEDのサイドコンテンツを作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_regist_customer(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	NLIST *nlp_cookie;
	DBase *cartdb;
	int in_login;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "カートDBを開けませんでした。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	nlp_cookie = Get_Cookie();
	Get_Cart_Handytype();
	in_login = Is_Logged_In(OldDBase(cartdb), nlp_in, nlp_cookie);

	Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"side-regist-customer\">\n");
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\">\n");
	if (in_login) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td valign=\"top\" width=\"10\">*</td><td><a href=\"ex_user_customer_edit.cgi\">登録情報の更新</a></td></tr>\n");
	} else {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td valign=\"top\" width=\"10\">*</td><td><a href=\"ex_user_customer_create.cgi\">会員登録</a></td></tr>\n");
	}
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_sidecontent_visible()
 * Description:
 *	表示するサイドコンテンツ数を得る。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 サイドコンテンツ数
 *	エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int get_sidecontent_visible(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_value;
	char cha_sql[512];
	int in_count;

	strcpy(cha_sql, "select count(T1.n_sidecontent_id)");
	strcat(cha_sql, " from at_sidecontent T1");
	strcat(cha_sql, " where T1.b_display = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ数を得るクエリに失敗。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_value = Db_GetValue(dbres , 0, 0);
	if(!chp_value) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_count = atoi(chp_value);
	Db_CloseDyna(dbres);
	return in_count;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_sidecontent_title()
 * Description:
 *	指定された順番のサイドコンテンツのタイトルを得て、埋め込む。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidecontent_title(DBase *db, NLIST *nlp_out, int in_sidecontent_id, int in_sidecontent_type, int in_blog, int in_open, int in_disp)
{
	DBRes *dbres;
	int in_title;
	char *chp_tmp;
	char *chp_escape;
	char cha_sql[512];

	in_title = 1;
	if (in_sidecontent_type == CO_SIDECONTENT_BLOG_PARTS) {
		sprintf(cha_sql, "select b_parts_title from at_parts where n_blog_id = %d and n_parts_id = %d", in_blog, in_sidecontent_id);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres){
			Put_Nlist(nlp_out, "ERROR", "サイドバータイトルを得るクエリに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (!chp_tmp || !atoi(chp_tmp)) {
			in_title = 0;
		}
		Db_CloseDyna(dbres);
	}

	strcpy(cha_sql, "select T1.c_sidecontent_title");
	strcat(cha_sql, " from at_sidecontent T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_sidecontent_id = %d", in_sidecontent_id);
	sprintf(cha_sql + strlen(cha_sql), " and b_display = %d", in_disp);
	sprintf(cha_sql + strlen(cha_sql), " and n_sidecontent_type = %d", in_sidecontent_type);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "サイドバータイトルを得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp){
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツのタイトルを取得できませんでした。");
		Db_CloseDyna(dbres);
		return 1;
	}
	if (in_title) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td class=\"linktitle\">");
		if (in_open) {
			chp_tmp = gcha_theme_minus_icon;
			chp_escape = "BTN_CLOSE";
		} else {
			chp_tmp = gcha_theme_plus_icon;
			chp_escape = "BTN_OPEN";
		}
		if (in_disp) {
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "SIDEBAR",
					 "<a href=\"%s%s%s/%s/?%s=%d#SIDE%d\"><img src=\"%s/%s\" border=\"0\"></a>",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_escape, in_sidecontent_type * 256 + in_sidecontent_id, in_sidecontent_type, g_cha_theme_image, chp_tmp);
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "SIDEBAR",
					 "<a href=\"%s%s%s/%08d/?%s=%d#SIDE%d\"><img src=\"%s/%s\" border=\"0\"></a>",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_escape, in_sidecontent_type * 256 + in_sidecontent_id, in_sidecontent_type, g_cha_theme_image, chp_tmp);
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR",
					 "<a href=\"%s/%s?bid=%d&%s=%d#SIDE%d\"><img src=\"%s/%s\" border=\"0\"></a>",
					g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, chp_escape, in_sidecontent_type * 256 + in_sidecontent_id, in_sidecontent_type, g_cha_theme_image, chp_tmp);
			}
		} else {
			Put_Format_Nlist(nlp_out, "SIDEBAR", "<img src=\"%s/transparent.gif\" border=\"0\" width=\"5\" height=\"5\">", g_cha_user_image);
		}
		Put_Format_Nlist(nlp_out, "SIDEBAR", "<a name=\"SIDE%d\"></a>", in_sidecontent_type);

		chp_escape = Escape_HtmlString(Db_GetValue(dbres, 0, 0));
		Put_Nlist(nlp_out, "SIDEBAR", chp_escape);
		free(chp_escape);
		Put_Nlist(nlp_out, "SIDEBAR", "</td></tr>\n");
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_sidebar()
 * Description:
 *	サイドバーを作成する
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_sidebar(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	NLIST *nlp_cookie;
	char *chp_opn;
	char *chp_open;
	char *chp_close;
	char *chp_tmp;
	char *chp_tmp2;
	char cha_sql[512];
	int *inp_sidecontent_id;
	int *inp_sidecontent_type;
	int *inp_sidecontent_allow;
	int in_sidecontent_id;
	int in_sidecontent_type;
	int in_allow;
	int in_count;
	int in_error;
	int in_access;
	int in_visit;
	int in_topout;
	int i;

	strcpy(cha_sql, " select T1.n_sidecontent_id");	/* 0 サイドコンテンツID */
	strcat(cha_sql, ",T1.n_sidecontent_type");		/* 1 サイドコンテンツタイプ */
	strcat(cha_sql, ",T1.b_allow_outer");			/* 2 他人のサイトでも表示するか */
	strcat(cha_sql, " from at_sidecontent T1");
	strcat(cha_sql, " where T1.b_display = 1");		/* 表示する */
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.n_sidecontent_order asc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ情報を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	/* サイドバーがなければそのままリターン */
	if(!in_count) {
		Db_CloseDyna(dbres);
		return 0;
	}
	/* サイドコンテンツIDを取っておく */
	/* (DBResをもう一度開くため) */
	inp_sidecontent_id = (int *)malloc(in_count * sizeof(int));
	inp_sidecontent_type = (int *)malloc(in_count * sizeof(int));
	inp_sidecontent_allow = (int *)malloc(in_count * sizeof(int));
	if(!inp_sidecontent_id || !inp_sidecontent_type || !inp_sidecontent_allow) {
		if (inp_sidecontent_id) {
			free(inp_sidecontent_id);
		}
		if (inp_sidecontent_type) {
			free(inp_sidecontent_type);
		}
		if (inp_sidecontent_allow) {
			free(inp_sidecontent_allow);
		}
		Put_Nlist(nlp_out, "ERROR", "サイドコンテンツ構築のメモリ確保に失敗しました。");
		Db_CloseDyna(dbres);
		return 1;
	}
	for(i = 0; i < in_count; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		chp_tmp2 = Db_GetValue(dbres, i, 1);
		chp_opn = Db_GetValue(dbres, i, 2);
		if(!chp_tmp || !*chp_tmp || Check_Numeric(chp_tmp) ||
		   !chp_tmp2 || !*chp_tmp2 || Check_Numeric(chp_tmp2) ||
		   !chp_opn || !*chp_opn || Check_Numeric(chp_opn)) {
			free(inp_sidecontent_id);
			free(inp_sidecontent_type);
			free(inp_sidecontent_allow);
			Db_CloseDyna(dbres);
			Put_Nlist(nlp_out, "ERROR", "サイドコンテンツの設定に不正なデータがあります。");
			return 1;
		}
		inp_sidecontent_id[i] = atoi(chp_tmp);
		inp_sidecontent_type[i] = atoi(chp_tmp2);
		inp_sidecontent_allow[i] = atoi(chp_opn);
	}
	Db_CloseDyna(dbres);

	in_topout = 0;
	in_error = 0;
	in_visit = 0;
	in_access = 0;
	chp_open = Get_Nlist(nlp_in, "BTN_OPEN", 1);
	chp_close = Get_Nlist(nlp_in, "BTN_CLOSE", 1);
	nlp_cookie = Get_Cookie();
	/* 各サイドコンテンツ組み立て */
	for(i = 0; i < in_count; i++) {
		/* サイドコンテンツの種類を取る */
		in_allow = inp_sidecontent_allow[i];
		in_sidecontent_id = inp_sidecontent_id[i];
		in_sidecontent_type = inp_sidecontent_type[i];
		if (in_sidecontent_type == CO_SIDECONTENT_ADMIN_MENU) {
			if (g_in_hosting_mode || g_in_need_login) {
				if (in_blog != g_in_login_blog) {
					if (g_in_hb_mode) {
						if (g_in_blog_auth < CO_AUTH_WRITE) {
							continue;
						}
					} else {
						continue;
					}
				}
			}
		} else if (!in_allow && in_blog != g_in_login_blog) {
			continue;
		}
		sprintf(cha_sql, "sc_%d_%d_%d", in_blog, in_sidecontent_type, in_sidecontent_id);
		chp_opn = Get_Nlist(nlp_cookie, cha_sql, 1);
		if (!chp_opn)
			chp_opn = "1";
		if (chp_open && atoi(chp_open) == in_sidecontent_type * 256 + in_sidecontent_id) {
			chp_opn = "1";
		} else if (chp_close && atoi(chp_close) == in_sidecontent_type * 256 + in_sidecontent_id) {
			chp_opn = "0";
		}
		/* 一部コンテンツ用にアンカーを埋め込む */
		switch(in_sidecontent_type) {
		case CO_SIDECONTENT_ARCHIVE_MONTHLY:
			Put_Nlist(nlp_out, "SIDEBAR", "<a name=\"marchive\"></a>\n");
			break;
		case CO_SIDECONTENT_CALENDAR:
			Put_Nlist(nlp_out, "SIDEBAR", "<a name=\"calendar\"></a>\n");
			break;
		default:
			break;
		}
		if (!in_topout) {
			Put_Nlist(nlp_out, "SIDEBAR", "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"sidebar\">\n");
			++in_topout;
		}
		/* サイドコンテンツのタイトルを作成 */
		if(build_sidecontent_title(db, nlp_out, in_sidecontent_id, in_sidecontent_type, in_blog, atoi(chp_opn), 1)) {
			return 1;
		}
		if (atoi(chp_opn)) {
			switch(in_sidecontent_type) {
			/* カレンダー */
			case CO_SIDECONTENT_ADMIN_MENU:
				in_error = build_sidecontent_admin_menu(db, nlp_in, nlp_out, in_blog);
				break;
			/* カレンダー */
			case CO_SIDECONTENT_CALENDAR:
				in_error = build_sidecontent_calender(db, nlp_in, nlp_out, in_blog);
				break;
			/* 月別アーカイブ */
			case CO_SIDECONTENT_ARCHIVE_MONTHLY:
				in_error = build_sidecontent_archive_month(db, nlp_in, nlp_out, in_blog);
				break;
			/* カテゴリー別アーカイブ */
			case CO_SIDECONTENT_ARCHIVE_CATEGORY:
				in_error = build_sidecontent_archive_category(db, nlp_in, nlp_out, in_blog);
				break;
			/* 最新コメント */
			case CO_SIDECONTENT_ARCHIVE_COMMENT:
				in_error = build_sidecontent_archive_comment(db, nlp_in, nlp_out, in_blog);
				break;
			/* 最新トラックバック */
			case CO_SIDECONTENT_ARCHIVE_TRACKBACK:
				in_error = build_sidecontent_archive_trackback(db, nlp_in, nlp_out, in_blog);
				break;
			/* プロフィール */
			case CO_SIDECONTENT_PROFILE:
				in_error = build_sidecontent_profile(db, nlp_out, in_blog);
				break;
			/* RSS取り込み */
			case CO_SIDECONTENT_ARCHIVE_RSS:
				in_error = build_sidecontent_rss(db, nlp_out, in_sidecontent_id, in_blog);
				break;
			/* ブログについて */
			case CO_SIDECONTENT_ABOUT_BLOG:
				in_error = build_sidecontent_about_blog(db, nlp_out, in_blog);
				break;
			/* リンク集 */
			case CO_SIDECONTENT_ARCHIVE_LINKS:
				in_error = build_sidecontent_links(db, nlp_out, in_sidecontent_id, in_blog);
				break;
			/* RSS配信 */
			case CO_SIDECONTENT_RSS_FEED:
				in_error = build_sidecontent_rss_feed(nlp_out, in_blog);
				break;
			/* 最新記事 */
			case CO_SIDECONTENT_ARCHIVE_NEWENTRY:
				in_error = build_sidecontent_archive_newentry(db, nlp_in, nlp_out, in_blog);
				break;
			/* 検索 */
			case CO_SIDECONTENT_BLOG_SEARCH:
				in_error = build_sidecontent_blog_search(nlp_out, in_blog);
				break;
			/* あしあと */
			case CO_SIDECONTENT_VISITOR_LIST:
				in_error = build_sidecontent_visitor_list(db, nlp_out, in_blog);
				break;
			/* アクセスログ */
			case CO_SIDECONTENT_ACCESS_LOG:
				in_error = build_sidecontent_access_log(db, nlp_out, in_blog);
				break;
			/* 商品情報(カート) */
			case CO_SIDECONTENT_ITEM_INFO:
				in_error = build_sidecontent_item_info(db, nlp_in, nlp_out, in_blog);
				break;
			/* おすすめ商品(カート) */
			case CO_SIDECONTENT_ENTRY_RECOMEND:
				in_error = build_sidecontent_entry_recomend(db, nlp_in, nlp_out, in_blog);
				break;
			/* カートログイン(カート) */
			case CO_SIDECONTENT_CART_LOGIN:
				in_error = build_sidecontent_cart_login(db, nlp_in, nlp_out, in_blog);
				break;
			/* 会員登録(カート) */
			case CO_SIDECONTENT_REGIST_CUSTOMER:
				in_error = build_sidecontent_regist_customer(db, nlp_in, nlp_out, in_blog);
				break;
			/* ブログパーツ */
			case CO_SIDECONTENT_BLOG_PARTS:
				in_error = build_sidecontent_blogparts(db, nlp_out, in_sidecontent_id, in_blog);
				break;
			default:
				Put_Nlist(nlp_out, "ERROR", "サイドコンテンツの設定に誤りがあります。");
				return 1;
			}
			if(in_error) {
				break;
			}
		}
		Set_Cookie(cha_sql, chp_opn, "");
	}
	Finish_Nlist(nlp_cookie);
	free(inp_sidecontent_id);
	free(inp_sidecontent_type);
	if (in_topout) {
		Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");
	}
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_count_toppage_article
 * Description:		トップページに表示する件数を引く
 *
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 件数
 *	エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int get_count_toppage_article(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_count;

	strcpy(cha_sql, "select T1.n_setting_toppage_index");	/* 0 トップページ表示件数 */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if(chp_tmp && *chp_tmp) {
		in_count = atoi(chp_tmp);
	} else {
		in_count = CO_ERROR;
	}
	Db_CloseDyna(dbres);
	return in_count;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_article_attach()
 * Description:
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int build_article_attach(
	 DBase *db
	,NLIST *nlp_out
	,int in_blog
	,int in_entry
)
{
	DBRes *dbres;
	int i, c;
	int in_uploadfile_id;
	char cha_sql[8192];
	char *chp_tmp;
	char *chp_escape;
	char *chp_filename;
	int in_row;

	strcpy(cha_sql, "select T1.n_uploadfile_id");	/* 0 ファイルID */
	strcat(cha_sql, ",T1.c_fileimage");				/* 1 ファイルイメージ */
	strcat(cha_sql, ",T1.c_filename");				/* 2 ファイル名表示名 */
	strcat(cha_sql, ",T1.c_filetype");				/* 3 ファイルタイプ*/
	strcat(cha_sql, ",T1.n_imagesize_x");			/* 4 X方向サイズ */
	strcat(cha_sql, ",T1.n_imagesize_y");			/* 5 Y方向サイズ */
	strcat(cha_sql, " from at_uploadfile T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "アップロードファイルの情報を得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	/* イメージ用タグ作成しながら、SQL文作成 */
	c = Db_GetRowCount(dbres);
	if (c)
		Put_Nlist(nlp_out, "ARTICLE", "\n<div class=\"entry_body\">");
	for(i = 0; i < c; i++) {
		chp_tmp = Db_GetValue(dbres, i, 0);
		if (!chp_tmp)
			continue;
		in_uploadfile_id = atoi(chp_tmp);
		if(Db_GetValue(dbres, i, 3) && strstr(Db_GetValue(dbres, i, 3), "image/")) {
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
			Put_Format_Nlist(nlp_out, "ARTICLE",
				"<a href=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">"
				"<img src=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" width=\"%d\" height=\"%d\" border=\"0\">"
				"</a>\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, in_entry, in_uploadfile_id,
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, in_entry, in_uploadfile_id,
				(int)dbl_x, (int)dbl_y);
		} else {
			chp_tmp = Db_GetValue(dbres, i, 2);
			chp_escape = Escape_HtmlString(chp_tmp);
			Put_Format_Nlist(nlp_out, "ARTICLE",
				"<a href=\"%s%s%s/%s/%d-%d-%d?BTN_DISP_ATTACH_FILE=1\" target=\"_blank\">%s</a>\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, in_entry, in_uploadfile_id, chp_escape);
			free(chp_escape);
		}
	}
	Db_CloseDyna(dbres);
	/* 動画 */
	if (g_in_cart_mode == CO_CART_SHOPPER || g_in_cart_mode == CO_CART_RESERVE) {
		strcpy(cha_sql, "select T1.c_filename");	/* 0 */
		strcat(cha_sql, ", T1.n_entry_id");			/* 1 */
		strcat(cha_sql, " from at_uploadmovie T1");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry);
		sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "記事に属している動画ファイルを得るクエリに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
			return 1;
		}
		in_row = Db_GetRowCount(dbres);
		/* ファイルリスト */
		if (in_row != 0) {
			for(i = 0; i < in_row; i++){
				chp_filename = Db_GetValue(dbres, i, 0);
				if (chp_filename){
					Put_Nlist(nlp_out, "ARTICLE", "<object width=\"320\" height=\"240\">\n");
					Put_Nlist(nlp_out, "ARTICLE", "<param name=\"allowScriptAccess\" value=\"sameDomain\" />\n");
					if (g_in_cart_mode == CO_CART_RESERVE) {
						Put_Nlist(nlp_out, "ARTICLE", "<param name=\"movie\" value=\"/reserve/cgi-bin/asjplayer.swf\" />\n");
						Put_Format_Nlist(nlp_out, "ARTICLE", "<param name=\"FlashVars\" value=\"flvfile=/reserve/images/%s\" />\n", chp_filename);
					} else {
						Put_Nlist(nlp_out, "ARTICLE", "<param name=\"movie\" value=\"/e-commex/cgi-bin/asjplayer.swf\" />\n");
						Put_Format_Nlist(nlp_out, "ARTICLE", "<param name=\"FlashVars\" value=\"flvfile=/e-commex/images/upload/%s\" />\n", chp_filename);
					}
					Put_Nlist(nlp_out, "ARTICLE", "<param name=\"quality\" value=\"high\" />\n");
					if (g_in_cart_mode == CO_CART_RESERVE) {
						Put_Format_Nlist(nlp_out, "ARTICLE", "<embed src=\"/reserve/cgi-bin/asjplayer.swf\" FlashVars=\"flvfile=/reserve/images/%s\" quality=\"high\" width=\"300\" height=\"250\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" />\n", chp_filename);
					} else {
						Put_Format_Nlist(nlp_out, "ARTICLE", "<embed src=\"/e-commex/cgi-bin/asjplayer.swf\" FlashVars=\"flvfile=/e-commex/images/%s\" quality=\"high\" width=\"300\" height=\"250\" allowScriptAccess=\"sameDomain\" type=\"application/x-shockwave-flash\" pluginspage=\"http://www.macromedia.com/go/getflashplayer\" />\n", chp_filename);
					}
					Put_Nlist(nlp_out, "ARTICLE", "</object>\n");
				}
			}
		}
		Db_CloseDyna(dbres);
	}
	if (c) {
		Put_Nlist(nlp_out, "ARTICLE", "<br /></div>\n");
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_article_detail()
 * Description:
 *	dbresを開いて記事リストを作成。
 *	この関数内で、全ての公開記事が作成時間の降順でセレクトされるSQLが組まれている。
 *	引数chp_where_etcにwhere文の残りを渡すと、条件に沿って表示される。
 *	例) chp_where_etc に " and T1.n_category_id = 0"と渡すと
 *	    カテゴリIDが0、つまりカテゴリー無しのものだけがセレクトされる。
 *	in_limitには最大件数を渡す。0以下を渡すと全てとみなされる。
 *	in_offsetはオフセットを渡す。0以下を渡すと最初からとみなされる。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int build_article_detail(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,char *chp_where	/* sqlのwhere文 */
	,int in_limit		/* リミット。 0は全てとする。 */
	,int in_offset		/* オフセット */
	,int in_blog
)
{
	DBRes *dbres;
	DBRes *dbles;
	char *chp_escape;
	char *chp_ascii;
	char *chp_br;
	char *chp_id;
	char *chp_comment;
	char *chp_trackback;
	char *chp_comment_count;
	char *chp_trackback_count;
	char *chp_body;
	char *chp_create;
	char *chp_title;
	char *chp_category;
	char *chp_category_id;
	char *chp_entry_more;
	char *chp_eid;
	char *chp_summary;
	char *chp_attach;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_next;
	char cha_sql[8192];
	char cha_prev_create[64];
	char cha_home_category[1024];
	char cha_home_entry[1024];
	char cha_path[1024];
	char cha_inform[1024];
	char cha_temp[512];
	char cha_nickname[CO_MAXLEN_BLOG_AUTHOR_NICKNAME + 16];
	int in_cutlen;
	int in_count;
	int in_dispcart;
	int in_links;
	int i, j;

	cha_inform[0] = '\0';
	if (g_in_dbb_mode) {
		dbres = Db_OpenDyna(db, "select c_inform_cgi from sy_authinfo");
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "通報CGI取得に失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		strcpy(cha_inform, chp_tmp ? chp_tmp : "");
		Db_CloseDyna(dbres);
	}
	/* RDF埋め込みに必要なニックネームを取る */
	strcpy(cha_sql, "select T1.c_author_nickname");	/* 0 ニックネーム */
	strcat(cha_sql, " from at_profile T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ニックネーム取得に失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp && *chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		strcpy(cha_nickname, chp_escape);
		free(chp_escape);
	} else {
		cha_nickname[0] = '\0';
	}
	Db_CloseDyna(dbres);
	/* トップページ表示文字数を取る */
	strcpy(cha_sql, "select T1.n_setting_cut_length");	/* 0 文字列 */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ニックネーム取得に失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%s<br>%s<br>", Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp && *chp_tmp) {
		in_cutlen = atoi(chp_tmp);
	} else {
		in_cutlen = 200;
	}
	Db_CloseDyna(dbres);
	/* 埋め込みに必要なパスの共通部分をあらかじめ作っておく。 */
	strcpy(cha_path, g_cha_protocol);
	chp_tmp = getenv("SERVER_NAME");
	if(!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "サーバー名を取得できませんでした。");
		return 1;
	}
	strcat(cha_path, chp_tmp);
	/* テンポラリテーブルに記事ごとのコメント数とカテゴリ数を入れる。 */
	if(Make_Tmptable_For_Count(db, nlp_out, in_blog, 0)) {
		return 1;
	}
	strcpy(cha_sql, " select T1.n_entry_id");				/* 0 記事ID */
	strcat(cha_sql, ",T1.b_comment");						/* 1 コメント可否 */
	strcat(cha_sql, ",T1.b_trackback");						/* 2 トラックバック可否 */
	strcat(cha_sql, ",T1.c_entry_title");					/* 3 記事タイトル */
	strcat(cha_sql, ",T1.c_entry_body");					/* 4 記事本文 */
	strcat(cha_sql, ",date_format(T1.d_entry_create_time,'%Y%m%d%H%i')");		/* 5 作成時間 */
	strcat(cha_sql, ",T1.n_category_id");					/* 6 カテゴリID */
	strcat(cha_sql, ",T2.c_category_name");					/* 7 カテゴリ名 */
	strcat(cha_sql, ",coalesce(T3.n_comment_count, 0)");	/* 8 コメント数 */
	strcat(cha_sql, ",coalesce(T4.n_tb_count, 0)");			/* 9 トラックバック数 */
	strcat(cha_sql, ",T1.c_entry_more");					/* 10 本文続き */
	strcat(cha_sql, ",T1.c_entry_summary");					/* 11 記事概要(RDFのために) */
	strcat(cha_sql, ",T1.c_item_id");						/* 12 カートの関連商品 */
	strcat(cha_sql, ",T1.c_item_kind");						/* 13 関連商品種別 */
	strcat(cha_sql, ",T1.c_item_url");						/* 14 関連商品URL */
	strcat(cha_sql, ",T1.c_item_owner");					/* 15 関連商品オーナー */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " left join at_category T2 on T1.n_category_id = T2.n_category_id and T1.n_blog_id = T2.n_blog_id");
	strcat(cha_sql, " left join tmp_comment T3 on T1.n_entry_id = T3.n_entry_id");
	strcat(cha_sql, " left join tmp_trackback T4 on T1.n_entry_id = T4.n_entry_id");
	strcat(cha_sql, " where T1.b_mode = 1");
	if(chp_where && *chp_where) {
		strcat(cha_sql, chp_where);
	}
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	strcat(cha_sql, " order by T1.d_entry_create_time desc, T1.n_entry_id desc");
	if(in_limit > 0) {
		sprintf(cha_sql + strlen(cha_sql), " limit %d,%d", in_offset, in_limit);
	}
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "記事リストを得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	if(in_count == 0) {
		Put_Nlist(nlp_out, "ARTICLE", "<span class=\"article-list-title\">記事はありません。</span>");
		Put_Nlist(nlp_out, "TITLE_START", "<!--");
		Put_Nlist(nlp_out, "TITLE_END", "-->");
		Db_CloseDyna(dbres);
		return 0;
	}
	/* 個別記事を表示中のときは、例外処理がある */
	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	cha_prev_create[0] = '\0';
	/* 記事のテーブルを組むとともに、トラックバック自動検知のためのRDFも埋める。 */
	for(i = 0; i < in_count; i++) {
		chp_id = Db_GetValue(dbres, i, 0);
		chp_comment = Db_GetValue(dbres, i, 1);
		chp_trackback = Db_GetValue(dbres, i, 2);
		chp_title = Db_GetValue(dbres, i, 3);
		chp_body = Db_GetValue(dbres, i, 4);
		chp_create = Db_GetValue(dbres, i, 5);
		chp_category_id = Db_GetValue(dbres, i, 6);
		chp_category = Db_GetValue(dbres, i, 7);
		chp_comment_count = Db_GetValue(dbres, i, 8);
		chp_trackback_count = Db_GetValue(dbres, i, 9);
		chp_entry_more = Db_GetValue(dbres, i, 10);
		chp_summary = Db_GetValue(dbres, i, 11);
		chp_attach = Db_GetValue(dbres, i, 12);
		if (g_in_dbb_mode) {
			sprintf(cha_home_category, "%s%s%s/%s/?cid=%s#archive_top", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_category_id);
			sprintf(cha_home_entry, "%s%s%s/%s/?eid=%s#entry", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_id);
		} else if (g_in_short_name) {
			sprintf(cha_home_category, "%s%s%s/%08d/?cid=%s#archive_top", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_category_id);
			sprintf(cha_home_entry, "%s%s%s/%08d/?eid=%s#entry", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_id);
		} else {
			sprintf(cha_home_category, "%s/%s?cid=%s&bid=%d#archive_top", g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_category_id, in_blog);
			sprintf(cha_home_entry, "%s/%s?eid=%s&bid=%d#entry", g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_id, in_blog);
		}
		/* ここまでRDF */
		/* 境目の画像 */
		if(i == 0 || strncmp(cha_prev_create, chp_create, 8) != 0) {
			if(i != 0) {
				Put_Nlist(nlp_out, "ARTICLE", "</table>\n");
				article_table_image_tail(nlp_in, nlp_out);
			}
			Put_Format_Nlist(nlp_out, "ARTICLE", "<a name=\"entry%.2s%.2s\"></a>\n", chp_create + 4, chp_create + 6);
			article_table_image_head(nlp_in, nlp_out);
			Put_Format_Nlist(nlp_out, "ARTICLE"
				, "<table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"entry-day-title\"><tr><td height=\"30\">%.4s年%.2s月%.2s日</td></tr></table>\n"
				, chp_create, chp_create + 4, chp_create + 6);
			Put_Nlist(nlp_out, "ARTICLE", "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"entrytable\" valign=\"top\">\n");
		}
		sprintf(cha_prev_create, "%.8s", chp_create);
		/* 境目の画像の後にRDFを埋める */
		Put_Nlist(nlp_out, "ARTICLE", "<!--\n");
		/* RDF名前空間宣言 */
		Put_Nlist(nlp_out, "ARTICLE", "<rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"\n");
		Put_Nlist(nlp_out, "ARTICLE", "\txmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n");
		Put_Nlist(nlp_out, "ARTICLE", "\txmlns:trackback=\"http://madskills.com/public/xml/rss/module/trackback/\">\n");
		/* サマリーなどの概要説明 */
		Put_Nlist(nlp_out, "ARTICLE", "<rdf:Description\n");
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "ARTICLE", "\trdf:about=\"%s%s%s/%s/?eid=%s\"\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_id);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "ARTICLE", "\trdf:about=\"%s%s%s/%08d/?eid=%s\"\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_id);
		} else {
			Put_Format_Nlist(nlp_out, "ARTICLE", "\trdf:about=\"%s%s?eid=%s&bid=%d\"\n", cha_path, CO_CGI_BUILD_HTML, chp_id, in_blog);
		}
		Put_Format_Nlist(nlp_out, "ARTICLE", "\ttrackback:ping=\"%s%s/%d-%s\"\n", cha_path, CO_CGI_TB, in_blog, chp_id);
		if(chp_title && *chp_title) {
			chp_escape = Escape_HtmlString(chp_title);
			Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:title=\"%s\"\n", chp_escape);
			free(chp_escape);
		} else {
			Put_Nlist(nlp_out, "ARTICLE", "\tdc:title=\"\"\n");
		}
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:identifier=\"%s%s%s/%s/?eid=%s\"\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_id);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:identifier=\"%s%s%s/%08d/?eid=%s\"\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_id);
		} else {
			Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:identifier=\"%s%s?bid=%d&eid=%s\"\n", cha_path, CO_CGI_BUILD_HTML, in_blog, chp_id);
		}
		if(chp_category && *chp_category) {
			chp_escape = Remove_HtmlTags(chp_category);
			Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:subject=\"%s\"\n", chp_escape);
			free(chp_escape);
		}
		if(chp_summary && *chp_summary) {
			chp_escape = Remove_HtmlTags(chp_summary);
		} else {
			chp_tmp = Remove_HtmlTags(chp_body);
			chp_escape = Chop_String(chp_tmp, CO_MAXLEN_ENTRY_SUMMARY);
			free(chp_tmp);
		}
		Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:description=\"%s...\"\n", chp_escape);
		free(chp_escape);
		if(cha_nickname[0] != '\0') {
			Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:creator=\"%s\"\n", cha_nickname);
		}
		Put_Format_Nlist(nlp_out, "ARTICLE", "\tdc:date=\"%.4s-%.2s-%.2sT%.2s:%.2s:00+09:00\" />\n"
			, chp_create, chp_create + 4, chp_create + 6, chp_create + 8, chp_create + 10);
		Put_Nlist(nlp_out, "ARTICLE", "</rdf:RDF>\n");
		Put_Nlist(nlp_out, "ARTICLE", "-->\n");
		/* 記事タイトル */
		chp_escape = Escape_HtmlString(chp_title);
		chp_ascii = Conv_Long_Ascii(chp_escape, 50);
		chp_br = Conv_Blog_Br(chp_ascii);
		Put_Format_Nlist(nlp_out, "ARTICLE", "<tr>\n<td>\n<div class=\"entry_title\"><a href=\"%s\">%s</a></div>\n", cha_home_entry, chp_br);
		free(chp_br);
		free(chp_ascii);
		free(chp_escape);
		/* 添付ファイル */
		if (!g_in_html_editor && build_article_attach(db, nlp_out, in_blog, atoi(chp_id))) {
			Db_CloseDyna(dbres);
			return 1;
		}
		/* 記事本文 */
		in_dispcart = 0;
		if (chp_entry_more && strlen(chp_entry_more) > 0) {
			Put_Nlist(nlp_out, "ARTICLE", "\n<div class=\"entry_body\">");
			Put_Nlist(nlp_out, "ARTICLE", chp_entry_more);
			Put_Nlist(nlp_out, "ARTICLE","</div>\n");
		}
		if (chp_body && strlen(chp_body) > 0) {
			Put_Nlist(nlp_out, "ARTICLE", "\n<div class=\"entry_body\">");
			chp_tmp = chp_body;
//			chp_escape = Remove_Invalid_Tags(chp_tmp);
//			chp_body = Conv_Long_Ascii(chp_escape, 56);
			chp_body = Conv_Long_Ascii(chp_tmp, 56);
			if(chp_eid) {
				if (g_in_html_editor) {
					Put_Nlist(nlp_out, "ARTICLE", chp_body);
				} else {
					chp_tmp = Conv_Blog_Br(chp_body);
					Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
					free(chp_tmp);
				}
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
				if (g_in_dbb_mode) {
					if (Blog_To_Temp(db, nlp_out, in_blog, cha_temp)) {
						return 1;
					}
					Put_Format_Nlist(nlp_out, "ARTICLE", "<div style=\"float:right;\"><a href=\"%s?kind=4&blog_id=%s&entry_id=%d\"><img src=\"%s/btn_inform.gif\" border=\"0\" alt=\"通報\"></a></div>\n", cha_inform, cha_temp, atoi(chp_eid), g_cha_user_image);
				}
			} else {
//				char *chp_tmp3;
//				chp_tmp3 = Remove_HtmlTags(chp_body);
//				chp_next =  Get_String_After(chp_tmp3, in_cutlen);
//				chp_tmp = (char*)malloc(chp_next - chp_tmp3 + 1);
//				strncpy(chp_tmp, chp_tmp3, chp_next - chp_tmp3);
//				chp_tmp[chp_next - chp_tmp3] = '\0';

				chp_next =  Get_String_After(chp_body, in_cutlen);
				chp_tmp = (char*)malloc(chp_next - chp_body + 1);
				strncpy(chp_tmp, chp_body, chp_next - chp_body);
				chp_tmp[chp_next - chp_body] = '\0';
				if (g_in_html_editor) {
					Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
				} else {
					chp_tmp2 = Conv_Blog_Br(chp_tmp);
					Put_Nlist(nlp_out, "ARTICLE", chp_tmp2);
					free(chp_tmp2);
				}
				free(chp_tmp);
//				free(chp_tmp3);
				if (*chp_next) {
					Put_Nlist(nlp_out, "ARTICLE","　．．．");
				}
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
				if (*chp_next) {
					/* 個別記事ページ以外のときは、続きがあるときだけリンクを表示 */
					Put_Format_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_more_link\"><a href=\"%s\">続きを読む</a></div>\n", cha_home_entry);
				}
			}
			free(chp_body);
//			free(chp_escape);
			/* 関連リンクリストを取る */
			if (g_in_dbb_mode) {
				sprintf(cha_sql,
					" select"
						" T1.c_link_title"			/* 0 リンクタイトルL */
						",T1.c_link_url"			/* 1 リンクURL */
						",T1.c_link_ip"				/* 2 リンクIP */
					" from"
						" at_sendlink T1"
					" where"
						" T1.n_entry_id=%s"
					" and"
						" T1.n_blog_id=%d"
					" order by"
						" T1.n_link_id",
					chp_id, in_blog);
			} else {
				sprintf(cha_sql,
					" select"
						" T1.c_link_title"			/* 0 リンクタイトルL */
						",T1.c_link_url"			/* 1 リンクURL */
					" from"
						" at_sendlink T1"
					" where"
						" T1.n_entry_id=%s"
					" and"
						" T1.n_blog_id=%d"
					" order by"
						" T1.n_link_id",
					chp_id, in_blog);
			}
			dbles = Db_OpenDyna(db, cha_sql);
			if (!dbles) {
				Put_Nlist(nlp_out, "ERROR", "関連リンクリストを得るクエリに失敗しました。<br>");
				Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
				return 1;
			}
			in_links = Db_GetRowCount(dbles);
			if (in_links) {
				Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_associatelink\">関連リンク：<br>\n");
				for (j = 0; j < in_links; ++j) {
					chp_tmp = Db_GetValue(dbles, j, 0);
					chp_escape = Db_GetValue(dbles, j, 1);
					if (chp_escape && chp_escape[0]) {
						if (g_in_dbb_mode) {
							chp_tmp2 = Db_GetValue(dbles, j, 2);
							if (chp_tmp2) {
								int in_entry = 0;
								if (Blog_To_Temp(db, nlp_out, atoi(chp_tmp2), cha_temp)) {
									Db_CloseDyna(dbles);
									Db_CloseDyna(dbres);
									return 1;
								}
								sprintf(cha_sql, "%s%s%s/%s?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML);
								strcat(cha_sql, "eid=%d&bid=%s");
								sscanf(chp_escape, cha_sql, &in_entry, cha_path);
								chp_tmp2 = Escape_HtmlString(chp_tmp);
								if (in_entry) {
									sprintf(cha_path, "%s%s%s/%s?eid=%d&bid=%s", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_entry, cha_temp);
									Put_Format_Nlist(nlp_out, "ARTICLE", "<a target=\"_blank\" href=\"%s\">%s</a><br>\n", cha_path, (chp_tmp2 && chp_tmp2[0]) ? chp_tmp2 : cha_path);
								} else if (chp_tmp) {
									Put_Format_Nlist(nlp_out, "ARTICLE", "%s<br>\n", chp_tmp2);
								}
								free(chp_tmp2);
							} else if (chp_tmp) {
								Put_Format_Nlist(nlp_out, "ARTICLE", "%s<br>\n", chp_tmp);
							}
						} else {
							Put_Format_Nlist(nlp_out, "ARTICLE", "<a target=\"_blank\" href=\"%s\">%s</a><br>\n",
								chp_escape ? chp_escape : "", (chp_tmp && chp_tmp[0]) ? chp_tmp : chp_escape);
						}
					} else if (chp_tmp) {
						Put_Format_Nlist(nlp_out, "ARTICLE", "%s<br>\n", chp_tmp);
					}
				}
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
			}
			if (g_in_dbb_mode) {
				chp_tmp = Db_GetValue(dbres, i, 13);
				if (chp_tmp && (chp_tmp[0] == 'L' || chp_tmp[0] == 'l' || chp_tmp[0] == 'C' || chp_tmp[0] == 'c' || chp_tmp[0] == 'B' || chp_tmp[0] == 'b')) {
					chp_tmp = Get_DBB_Topic(db, nlp_out, in_blog, Db_GetValue(dbres, i, 13), Db_GetValue(dbres, i, 12), Db_GetValue(dbres, i, 15), Db_GetValue(dbres, i, 14));
					if (chp_tmp) {
						Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_associatelink\">DBBトピック：<br>\n");
						Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
					}
				}
			}
			Db_CloseDyna(dbles);
			if (g_in_cart_mode == CO_CART_SHOPPER && chp_attach && chp_attach[0]) {
				Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_body\">\n");
				if (build_article_cart(db, nlp_in, nlp_out, in_blog, chp_attach, 1, &in_dispcart)) {
					Db_CloseDyna(dbres);
					return 1;
				}
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
			} else if (g_in_cart_mode == CO_CART_RESERVE && chp_attach && chp_attach[0]) {
				Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_body\">\n");
				if (Build_Reserve_Item(db, nlp_in, nlp_out, chp_attach, "ARTICLE")) {
					Db_CloseDyna(dbres);
					return 1;
				}
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
			}
		}
		Put_Nlist(nlp_out, "ARTICLE", "</td></tr>\n");
		Put_Nlist(nlp_out, "ARTICLE", "<tr><td width=\"100%\">\n");
		/* 記事情報のうち、カテゴリ名と投稿時間は必ず表示 */
		Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_state\">");
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "ARTICLE", "%.2s時%.2s分", chp_create + 8, chp_create + 10);
		} else if (g_in_hb_mode) {
			chp_escape = Escape_HtmlString(chp_category);
			chp_tmp = Escape_HtmlString(chp_attach ? chp_attach : "");
			if (chp_tmp[0]) {
				Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"%s\">%s</a> | %s | %.2s時%.2s分"
					, cha_home_category, chp_escape, chp_tmp, chp_create + 8, chp_create + 10);
			} else {
				Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"%s\">%s</a> | %.2s時%.2s分"
					, cha_home_category, chp_escape, chp_create + 8, chp_create + 10);
			}
			free(chp_tmp);
			free(chp_escape);
		} else {
			chp_escape = Escape_HtmlString(chp_category);
			Put_Format_Nlist(nlp_out, "ARTICLE", "<a href=\"%s\">%s</a> | %.2s時%.2s分"
				, cha_home_category, chp_escape, chp_create + 8, chp_create + 10);
			free(chp_escape);
		}
		/* コメントを認めるときは、コメントのリンクを表示 */
		if(chp_comment && atoi(chp_comment)) {
			if(!chp_comment_count || !*chp_comment_count){
				Put_Format_Nlist(nlp_out, "ARTICLE", " | <a href=\"%scomment\">コメント(0)</a>", cha_home_entry);
			} else {
				Put_Format_Nlist(nlp_out, "ARTICLE", " | <a href=\"%scomment\">コメント(%s)</a>", cha_home_entry, chp_comment_count);
			}
		}
		/* トラックバックを認めるときは、トラックバックのリンクを表示 */
		if(chp_trackback && atoi(chp_trackback)){
			if(!chp_trackback_count || !*chp_trackback_count) {
				Put_Format_Nlist(nlp_out, "ARTICLE", " | <a href=\"%strackback\">%s(0)</a>", cha_home_entry, g_in_dbb_mode ? "引用記事" : "トラックバック");
			} else {
				Put_Format_Nlist(nlp_out, "ARTICLE", " | <a href=\"%strackback\">%s(%s)</a>", cha_home_entry, g_in_dbb_mode ? "引用記事" : "トラックバック", chp_trackback_count);
			}
		}
		/* 商品を表示した場合は、レビューのリンクを表示 */
		if (in_dispcart) {
			in_dispcart = Get_Allow_Review(db, nlp_in, nlp_out, in_blog, 0);
			if (in_dispcart < 0) {
				Db_CloseDyna(dbres);
				return 1;
			}
			if (in_dispcart) {
				in_dispcart = Get_Review_Number(db, nlp_in, nlp_out, in_blog, chp_attach);
				if (in_dispcart < 0) {
					Db_CloseDyna(dbres);
					return 1;
				}
				Put_Format_Nlist(nlp_out, "ARTICLE", " | <a href=\"%sreview\">レビュー(%d)</a>", cha_home_entry, in_dispcart);
			}
		}
		/* 個別記事ページ以外ではトップへのリンクを表示 */
		if(!chp_eid) {
			Put_Nlist(nlp_out, "ARTICLE", " | <a href=\"#top\">↑トップ</a>");
		}
		Put_Nlist(nlp_out, "ARTICLE", "</div>\n");
		Put_Nlist(nlp_out, "ARTICLE", "</td>\n</tr>\n</div>\n");
	}
	Db_CloseDyna(dbres);
	Put_Nlist(nlp_out, "ARTICLE", "</table>\n");
	article_table_image_tail(nlp_in, nlp_out);
	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	get_toppage_dayspan()
 * Description:
 *	トップページに表示する日の範囲を配列に入れて返す。
 *	開始日、終了日はそれぞれyyyymmddの形で返る。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int get_toppage_dayspan(
	 DBase *db
	,int in_span		/* 表示日数 */
	,char *chp_start	/* 開始日 */
	,char *chp_finish	/* 終了日 */
	,int in_blog
)
{
	struct tm *stp_time;
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	time_t tm_time;

	strcpy(cha_sql, "select date_format(max(T1.d_entry_create_time),'%Y%m%d')");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.b_mode = 1");
	strcat(cha_sql, " and d_entry_create_time <= '");
	time(&tm_time);
	stp_time = localtime(&tm_time);
	strftime(cha_sql + strlen(cha_sql), sizeof(cha_sql) - strlen(cha_sql), "%Y-%m-%d 23:59:00", stp_time);
	sprintf(cha_sql + strlen(cha_sql), "' and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	Db_CloseDyna(dbres);
	if(!chp_tmp) {
		return CO_ERROR;
	}
	/* 年月日で区切る */
	strcpy(chp_finish, chp_tmp);
	if(sscanf(chp_finish, "%4d%2d%2d", &stp_time->tm_year, &stp_time->tm_mon, &stp_time->tm_mday) != 3) {
		return CO_ERROR;
	}
	stp_time->tm_year -= 1900;
	stp_time->tm_mon -= 1;
	stp_time->tm_mday -= (in_span - 1);
	tm_time = mktime(stp_time);
	stp_time = localtime(&tm_time);
	strftime(chp_start, 10, "%Y%m%d", stp_time);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_count_entry_month()
 * Description:
 *		月別アーカイブに表示する件数を引く
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 件数（0以上の整数）
 *		エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int get_count_entry_month(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_month, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_count;

	strcpy(cha_sql, "select count(*)");	/* 0 件数 */
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.d_entry_create_time >= '");
	strcat(cha_sql, chp_month);
	strcat(cha_sql, "01000000'");
	strcat(cha_sql, " and T1.d_entry_create_time <= '");
	strcat(cha_sql, chp_month);
	strcat(cha_sql, "31235959'");
	strcat(cha_sql, " and b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_count = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_count;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_count_entry_category()
 * Description:
 *	カテゴリーアーカイブに表示する件数を引く
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 件数（0以上の整数）
 *		エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int get_count_entry_category(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_category, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_count;

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from at_entry T1");
	strcat(cha_sql, " where T1.n_category_id = ");
	strcat(cha_sql, chp_category ? chp_category : "0");
	strcat(cha_sql, " and T1.b_mode = 1");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_count = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_count;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_day_toppage_article()
 * Description:
 *	トップページに表示する日数を引く
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 日数（0以上の整数）
 *	エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int get_day_toppage_article(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[100];
	int in_days;

	strcpy(cha_sql, "select T1.n_setting_toppage_day");
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_days = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_days;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_disptype_toppage()
 * Description:
 *	サイドバーの表示場所(右or左)を取る
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0 左 or 1 右
 *	エラー CO_ERROR
-* ------------------------------------------------------------------------*/
int get_disptype_toppage(DBase *db, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[100];
	int in_disptype;

	strcpy(cha_sql, "select T1.b_setting_toppage_disptype");
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp || !*chp_tmp) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_disptype = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_disptype;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_comment_list()
 * Description:		コメントリスト
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int build_comment_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_html[32768];
	char cha_owner_temp[512];
	char cha_blog_temp[512];
	char cha_inform[256];
	char cha_sql[512];
	int in_all;
	int in_count;
	int i;

	in_all = 0;
	chp_tmp = Get_Nlist(nlp_in, "FLG_FORCE_ALL", 1);
	if (chp_tmp && *chp_tmp && (g_in_login_blog == in_blog || (g_in_hb_mode && g_in_blog_auth >= CO_AUTH_WRITE))) {
		in_all = 1;
	}

	cha_inform[0] = '\0';
	cha_blog_temp[0] = '\0';
	if (g_in_dbb_mode) {
		if (Blog_To_Temp(db, nlp_out, in_blog, cha_blog_temp)) {
			return 1;
		}
		dbres = Db_OpenDyna(db, "select c_inform_cgi from sy_authinfo");
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "通報CGI取得に失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		strcpy(cha_inform, chp_tmp ? chp_tmp : "");
		Db_CloseDyna(dbres);
	}

	strcpy(cha_sql, "select T1.c_comment_author");	/* 0 コメント著者 */
	strcat(cha_sql, ", T1.c_comment_mail");		/* 1 メールアドレス */
	strcat(cha_sql, ", T1.c_comment_url");		/* 2 URL */
	strcat(cha_sql, ", T1.c_comment_body");		/* 3 コメント本文 */
	strcat(cha_sql, ", date_format(T1.d_comment_create_time,'%y年%m月%d日%H時%i分')");	/* 4 コメント日時 */
	strcat(cha_sql, ", T1.n_comment_id");		/* 5 コメントID */
	strcat(cha_sql, ", T1.c_comment_ip");		/* 6 コメントIP */
	strcat(cha_sql, " from at_comment T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %s", Get_Nlist(nlp_in, "eid", 1));
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	if (!in_all)
		strcat(cha_sql, " and T1.b_comment_accept != 0");
	strcat(cha_sql, " order by T1.d_comment_create_time asc");
	strcat(cha_sql, " , T1.n_comment_id asc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "COMMENTLIST", "<a name=\"entrycomment\"></a>\n");
	if (in_count) {
		sprintf(cha_html, "<div class=\"comment-list-title\">この記事へのコメント[%d件]</div>\n", in_count);
		Put_Nlist(nlp_out, "COMMENTLIST" , cha_html);
		if(!in_count) {
			Db_CloseDyna(dbres);
			return 0;
		}
		Put_Nlist(nlp_out, "COMMENTLIST" ,"<table width=\"100%\" border=\"0\" class=\"article\">\n");
		for(i = 0; i < in_count; i++) {
			Put_Nlist(nlp_out, "COMMENTLIST", "<tr>\n<td>");
			/* アンカー */
			chp_tmp = Db_GetValue(dbres, i, 5);
			if(chp_tmp) {
				sprintf(cha_html, "<a name=\"c%s\"></a>", chp_tmp);
				Put_Nlist(nlp_out, "COMMENTLIST", cha_html);
			}
			/* コメント本文 */
			Put_Nlist(nlp_out, "COMMENTLIST", "\n<div class=\"comment_body\">\n");
			chp_tmp = Db_GetValue(dbres, i, 3);
			if(chp_tmp) {
				chp_tmp2 = Conv_Long_Ascii(chp_tmp, 72);
				chp_escape = Escape_HtmlString(chp_tmp2);
				free(chp_tmp2);
				chp_tmp = Conv_Br(chp_escape);
				free(chp_escape);
				Put_Nlist(nlp_out, "COMMENTLIST", chp_tmp);
				free(chp_tmp);
			}
			Put_Nlist(nlp_out, "COMMENTLIST", "\n</div>\n</td></tr>\n");
			/* コメント状態 */
			Put_Nlist(nlp_out, "COMMENTLIST", "<tr><td>\n<div class=\"comment-state\">");
			chp_tmp2 = Db_GetValue(dbres, i, 0);
			if (chp_tmp2 && *chp_tmp2) {
				chp_escape = Escape_HtmlString(chp_tmp2);
				if (g_in_dbb_mode) {
					chp_tmp = Db_GetValue(dbres, i, 6);
					if (chp_tmp && *chp_tmp) {
						if (Owner_To_Temp(db, nlp_out, atoi(chp_tmp), cha_owner_temp)) {
						//	Put_Format_Nlist(nlp_out, "COMMENTLIST", "Posted by %s\n", chp_escape);
							Db_CloseDyna(dbres);
							return 1;
						} else {
							Put_Format_Nlist(nlp_out, "COMMENTLIST", "Posted by <a href=\"%s%s%s/%s/\">%s</a>\n",
									g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, cha_owner_temp, chp_escape);
						}
					} else {
						Put_Format_Nlist(nlp_out, "COMMENTLIST", "Posted by %s\n", chp_escape);
					}
				} else {
					chp_tmp = Db_GetValue(dbres, i, 2);
					if (chp_tmp && *chp_tmp) {
						Put_Format_Nlist(nlp_out, "COMMENTLIST", "Posted by <a href=\"%s\">%s</a>\n", chp_tmp, chp_escape);
					} else {
						Put_Format_Nlist(nlp_out, "COMMENTLIST", "Posted by %s\n", chp_escape);
					}
				}
				free(chp_escape);
			}
			chp_tmp = Db_GetValue(dbres, i, 4);
			if(chp_tmp) {
				Put_Nlist(nlp_out, "COMMENTLIST", " | ");
				Put_Nlist(nlp_out, "COMMENTLIST", chp_tmp);
			}
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "COMMENTLIST", "&nbsp;<a href=\"%s?kind=5&blog_id=%s&entry_id=%s&comment_id=%s\"><img src=\"%s/btn_inform.gif\" border=\"0\" alt=\"通報\"></a>\n", cha_inform, cha_blog_temp, Get_Nlist(nlp_in, "eid", 1), Db_GetValue(dbres, i, 5), g_cha_user_image);
			}
			Put_Nlist(nlp_out, "COMMENTLIST", "\n</div>\n</td></tr>\n");
		}
		Put_Nlist(nlp_out, "COMMENTLIST", "</table>\n");
	}
	Db_CloseDyna(dbres);

	if (g_in_need_login && g_in_login_blog == in_blog && !g_in_admin_mode) {
		if (Begin_Transact(db) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>", __LINE__, Gcha_last_error);
			return 1;
		}
		sprintf(cha_sql,
			" update at_comment T1"
			" set b_comment_read = 1"
			" where T1.n_entry_id = %s"
			" and T1.n_blog_id = %d"
			, Get_Nlist(nlp_in, "eid", 1), in_blog);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "クエリに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション終了に失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>", __LINE__, Gcha_last_error);
			return 1;
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_comment_form()
 * Description:
 *	コメント投稿フォームを作成
 *	エラー復帰時はデータを読み込む
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_comment_form(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_err;
	char *chp_sql;
	char *chp_eid;
	char *chp_escape;
	char *chp_from_error;
	char cha_num[32];
	char cha_sql[512];
	char cha_nickname[32];
	int in_owner;
	int in_accept;

	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	sprintf(cha_sql, "select b_comment from at_entry T1 where T1.n_blog_id = %d and n_entry_id = %s", in_blog, chp_eid);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_accept = atoi(chp_tmp);
	} else {
		in_accept = 0;
	}
	Db_CloseDyna(dbres);

	/* 引継ぎかどうか。 */
	sprintf(cha_num, "%d", in_blog);
	chp_err = Get_Nlist(nlp_out, "ERROR", 1);
	chp_sql = Get_Nlist(nlp_out, "QUERY", 1);
	chp_from_error = Get_Nlist(nlp_in, "from_error", 1);
	Put_Nlist(nlp_out, "COMMENTFORM", "<div class=\"comment-form-title\">この記事にコメントする</div>\n");
	Put_Nlist(nlp_out, "COMMENTFORM", "<form action=\"");
	Put_Nlist(nlp_out, "COMMENTFORM", CO_CGI_BUILD_HTML);
	Put_Nlist(nlp_out, "COMMENTFORM", "\" method=\"POST\">\n");
	Build_HiddenEncode(nlp_out, "COMMENTFORM", "eid", chp_eid);
	if (g_in_dbb_mode) {
		Put_Format_Nlist(nlp_out, "COMMENTFORM", "<input type=\"hidden\" name=\"bid\" value=\"%s\">\n", g_cha_blog_temp);
	} else {
		Build_HiddenEncode(nlp_out, "COMMENTFORM", "bid", cha_num);
	}
	if (Get_Nlist(nlp_in, "BTN_COMMENT.x", 1)) {
		Build_HiddenEncode(nlp_out, "COMMENTFORM", "from_error", "1");
	}
	Put_Nlist(nlp_out, "COMMENTFORM", "<table border=\"0\" width=\"90%\" align=\"center\">\n");
	Put_Nlist(nlp_out, "COMMENTFORM",  "<tr><td>");
	if (g_in_need_login) {
		in_owner = Get_Nickname(db, nlp_out, cha_nickname, NULL, NULL);
		if (!in_owner) {
			return 1;
		}
		chp_escape = Escape_HtmlString(cha_nickname);
		Put_Format_Nlist(nlp_out, "COMMENTFORM" ,"お名前：%s<br>\n", chp_escape);
		asprintf(&chp_tmp, "%d:%s", in_owner, chp_escape);
		Build_HiddenEncode(nlp_out, "COMMENTFORM", "comment_author_name", chp_tmp);
		Build_HiddenEncode(nlp_out, "COMMENTFORM", "comment_author_mail", "");
		free(chp_escape);
		free(chp_tmp);
	} else {
		Put_Nlist(nlp_out, "COMMENTFORM", "お名前<br>\n");
		chp_tmp = Get_Nlist(nlp_in, "comment_author_name", 1);
		if(chp_from_error && chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Build_Input(nlp_out, "COMMENTFORM", "comment_author_name", chp_escape, 35, CO_MAXLEN_COMMENT_AUTHOR_NAME);
			free(chp_escape);
		} else {
			Build_Input(nlp_out, "COMMENTFORM", "comment_author_name", "", 35, CO_MAXLEN_COMMENT_AUTHOR_NAME);
		}
		Put_Nlist(nlp_out, "COMMENTFORM" ,"<br><br>\n");
		Put_Nlist(nlp_out, "COMMENTFORM" ,"メールアドレス(必須ではありません)<br>\n");
		chp_tmp = Get_Nlist(nlp_in, "comment_author_mail", 1);
		if(chp_from_error && chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Build_Input(nlp_out, "COMMENTFORM", "comment_author_mail", chp_escape, 35, CO_MAXLEN_COMMENT_AUTHOR_MAIL);
			free(chp_escape);
		} else {
			Build_Input(nlp_out, "COMMENTFORM", "comment_author_mail", "", 35, CO_MAXLEN_COMMENT_AUTHOR_MAIL);
		}
	}
	if (!g_in_need_login) {
		Put_Nlist(nlp_out, "COMMENTFORM" ,"<br><br>\n");
		Put_Nlist(nlp_out, "COMMENTFORM" ,"URL<br>\n");
		chp_tmp = Get_Nlist(nlp_in, "comment_author_url", 1);
		if(chp_from_error && chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Build_Input(nlp_out, "COMMENTFORM", "comment_author_url", chp_escape, 35, CO_MAXLEN_COMMENT_AUTHOR_URL);
			free(chp_escape);
		} else {
			Build_Input(nlp_out, "COMMENTFORM", "comment_author_url", "", 35, CO_MAXLEN_COMMENT_AUTHOR_URL);
		}
	}
	Put_Nlist(nlp_out, "COMMENTFORM" ,"<br><br>\n");
	Put_Nlist(nlp_out, "COMMENTFORM" ,"コメント（全角1,000文字まで）<br>\n");
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if(chp_from_error && chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_Inputarea(nlp_out, "COMMENTFORM", "comment", chp_escape, 8, 55);
		free(chp_escape);
	} else {
		Build_Inputarea(nlp_out, "COMMENTFORM", "comment", "", 8, 55);
	}
	if (in_accept == 2) {
		Put_Nlist(nlp_out, "COMMENTFORM" ,"<br>※コメントは承認されるまで表示されません。\n");
	}
	Put_Nlist(nlp_out, "COMMENTFORM", "<p align=\"center\">\n");
	Build_Submit(nlp_out, "COMMENTFORM", "BTN_POST_COMMENT_ASK", "書き込む");
	if (g_in_dbb_mode) {
		Put_Format_Nlist(nlp_out, "COMMENTFORM",
			"<button onclick=\"location.href='%s%s%s/%s/?eid=%s'\">リセット</button>\n",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_eid);
	} else if (g_in_short_name) {
		Put_Format_Nlist(nlp_out, "COMMENTFORM",
			"<button onclick=\"location.href='%s%s%s/%08d/?eid=%s'\">リセット</button>\n",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_eid);
	} else {
		Put_Format_Nlist(nlp_out, "COMMENTFORM",
			"<button onclick=\"location.href='%s%s%s/%s?bid=%d&eid=%s'\">リセット</button>\n",
			g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, chp_eid);
	}
	Put_Nlist(nlp_out, "COMMENTFORM", "</p>\n</td></tr></table>\n");
	Put_Nlist(nlp_out, "COMMENTFORM" ,"</form>\n");

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_trackback_list()
 * Description:		トラックバック記事一覧の作成
 *
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int build_trackback_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_access)
{
	DBRes *dbres;
	char *chp_eid;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_tmp3;
	char *chp_tmp4;
	char *chp_escape;
	char cha_html[8192];
	char cha_sql[1024];
	char cha_inform[512];
	char cha_blog_temp[512];
	char cha_trackback_url[512];
	int in_entry;
	int in_all;
	int in_count;
	int in_browser;
	int i;

	in_all = 0;
	chp_tmp = Get_Nlist(nlp_in, "FLG_FORCE_ALL", 1);
	if (chp_tmp && *chp_tmp && (g_in_login_blog == in_blog || (g_in_hb_mode && g_in_blog_auth >= CO_AUTH_WRITE))) {
		in_all = 1;
	}

	cha_inform[0] = '\0';
	cha_blog_temp[0] = '\0';
	if (g_in_dbb_mode) {
		if (Blog_To_Temp(db, nlp_out, in_blog, cha_blog_temp)) {
			return 1;
		}
		dbres = Db_OpenDyna(db, "select c_inform_cgi from sy_authinfo");
		if(!dbres) {
			Put_Nlist(nlp_out, "ERROR", "通報CGI取得に失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		strcpy(cha_inform, chp_tmp ? chp_tmp : "");
		Db_CloseDyna(dbres);
	}

	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if (!chp_eid)
		chp_eid = "0";
	in_browser = Get_Browser();
	if (!g_in_dbb_mode && in_access) {
		Put_Nlist(nlp_out, "TRACKBACKURL",
			"<tr><td align=\"center\">\n"
			"<div id=\"trackback-url-area\">\n"
			"<a name=\"trackback\"></a>"
			"<fieldset>\n"
			"<legend>この記事のトラックバックURL</legend>\n");
		sprintf(cha_trackback_url, "http://%s%s/%s/%d-%s", getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_TB, in_blog, chp_eid);
		Put_Nlist(nlp_out, "JAVASCRIPT_ARG_TRACKBACKURL", cha_trackback_url);
		Put_Nlist(nlp_out, "TRACKBACKURL", "<table width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"padding:0px 8px 8px 0px; line-height:120%;\"><tr><td>");
		if(Get_OperatingSystem() == CO_OS_WIN && in_browser == CO_BROWSER_IE) {
			Put_Format_Nlist(nlp_out, "TRACKBACKURL",
				"<a title=\"クリックでクリップボードへコピー\" href=\"javascript:clip()\">%s<br>クリックすることでクリップボードへコピーできます。</a>",
				cha_trackback_url);
		} else {
			Put_Nlist(nlp_out, "TRACKBACKURL", cha_trackback_url);
		}
		Put_Nlist(nlp_out, "TRACKBACKURL", "</td></tr></table></fieldset><br><br>\n");
		Put_Nlist(nlp_out, "TRACKBACKURL", "</div></td></tr>\n");
	}
	strcpy(cha_sql, "select T1.c_tb_title");	/* 0 タイトル */
	strcat(cha_sql, " , T1.c_tb_excerpt");		/* 1 本文 */
	strcat(cha_sql, " , T1.c_tb_url");			/* 2 URL */
	strcat(cha_sql, " , T1.c_tb_blog_name");	/* 3 ブログ名 */
	strcat(cha_sql, " , date_format(T1.d_tb_create_time,'%y年%m月%d日%H時%i分')");	/* 4 生成時間 */
	strcat(cha_sql, " , T1.n_entry_id");		/* 5 エントリーID */
	strcat(cha_sql, " , T1.n_tb_id");			/* 6 トラックバックID */
	strcat(cha_sql, " , T1.c_tb_ip");			/* 7 トラックバックIP */
	strcat(cha_sql, " from at_trackback T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_entry_id = %s", chp_eid);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	if (!in_all)
		strcat(cha_sql, " and T1.b_tb_accept != 0");
	strcat(cha_sql, " order by T1.d_tb_create_time desc");
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "ERROR", "引用記事を得るクエリに失敗しました。<br>");
		} else {
			Put_Nlist(nlp_out, "ERROR", "トラックバックを得るクエリに失敗しました。<br>");
		}
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	for(i = 0; i < in_count; i++) {
		if (g_in_dbb_mode) {
			Put_Nlist(nlp_out, "TRACKBACKLIST", "<tr><td>\n");
		} else {
			Put_Nlist(nlp_out, "TRACKBACKLIST", "<tr class=\"article\"><td>\n");
		}
		if (i == 0) {
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "TRACKBACKLIST",
					"<a name=\"entrytrackback\"></a>\n"
					"<div class=\"trackback-list-title\">"
					"この記事を引用している記事[%d件]</div>\n", in_count);
			} else {
				Put_Format_Nlist(nlp_out, "TRACKBACKLIST",
					"<a name=\"entrytrackback\"></a>\n"
					"<div class=\"trackback-list-title\">"
					"この記事へのトラックバック[%d件]</div>\n", in_count);
			}
		}
		Put_Nlist(nlp_out, "TRACKBACKLIST", "<div class=\"trackback\">\n");
		/* アンカー */
		chp_tmp = Db_GetValue(dbres, i, 6);
		if(chp_tmp) {
			memset(cha_html, '\0', sizeof(cha_html));
			sprintf(cha_html, "<a name=\"tb%s\"></a>", chp_tmp);
			Put_Nlist(nlp_out, "TRACKBACKLIST", cha_html);
		}
		/* トラックバックタイトル */
		Put_Nlist(nlp_out, "TRACKBACKLIST", "\n<table class=\"trackback-title\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%\"><tr><td>\n");
		chp_tmp2 = Db_GetValue(dbres, i, 0);
		if (chp_tmp2 && chp_tmp2[0]) {
			chp_escape = Escape_HtmlString(chp_tmp2);
			if (g_in_dbb_mode) {
				chp_tmp = Db_GetValue(dbres, i, 7);
				if (chp_tmp && chp_tmp[0]) {
					chp_tmp2 = Db_GetValue(dbres, i, 2);
					if (chp_tmp2) {
						chp_tmp3 = strstr(chp_tmp2, "eid=");
						if (chp_tmp3) {
							chp_tmp3 += strlen("eid=");
							chp_tmp4 = strchr(chp_tmp3, '\r');
							if (chp_tmp4)
								*chp_tmp4 = '\0';
							chp_tmp4 = strchr(chp_tmp3, '\n');
							if (chp_tmp4)
								*chp_tmp4 = '\0';
							in_entry = atoi(chp_tmp3);
							if (Blog_To_Temp(db, nlp_out, atoi(chp_tmp), cha_trackback_url)) {
								Put_Nlist(nlp_out, "TRACKBACKLIST", chp_escape);
							} else {
								Put_Format_Nlist(nlp_out, "TRACKBACKLIST", "<a href=\"%s%s%s/%s/?eid=%d\">%s</a>",
									g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, cha_trackback_url, in_entry, chp_escape);
							}
						} else {
							Put_Nlist(nlp_out, "TRACKBACKLIST", chp_escape);
						}
					} else {
						Put_Nlist(nlp_out, "TRACKBACKLIST", chp_escape);
					}
				} else {
					Put_Nlist(nlp_out, "TRACKBACKLIST", chp_escape);
				}
			} else {
				chp_tmp = Db_GetValue(dbres, i, 2);
				if (chp_tmp && chp_tmp[0]) {
					Put_Format_Nlist(nlp_out, "TRACKBACKLIST", "<a href=\"%s\">%s</a>", chp_tmp, chp_escape);
				} else if(chp_tmp2) {
					Put_Nlist(nlp_out, "TRACKBACKLIST", chp_escape);
				}
			}
			free(chp_escape);
		}
		Put_Nlist(nlp_out, "TRACKBACKLIST", "\n</td></tr></table>\n");
		/* トラックバック本体 */
		Put_Nlist(nlp_out, "TRACKBACKLIST", "<div class=\"trackback-body\">\n");
		chp_tmp = Db_GetValue(dbres, i, 1);
		if(chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			chp_tmp = Conv_Br(chp_escape);
			Put_Nlist(nlp_out, "TRACKBACKLIST", chp_tmp);
			free(chp_tmp);
			free(chp_escape);
		}
		Put_Nlist(nlp_out, "TRACKBACKLIST", "</div>\n</td></tr>\n");
		/* トラックバック情報 */
		Put_Nlist(nlp_out, "TRACKBACKLIST", "<tr><td>\n<div class=\"trackback-state\">");
		chp_tmp = Db_GetValue(dbres, i, 2);
		chp_tmp2 = Db_GetValue(dbres, i, 3);
		if(chp_tmp && chp_tmp2) {
			chp_escape = Escape_HtmlString(chp_tmp2);
			memset(cha_html, '\0', sizeof(cha_html));
			sprintf(cha_html, "Posted by <a href=\"%s\">%s</a>\n", chp_tmp, chp_escape);
			Put_Nlist(nlp_out, "TRACKBACKLIST", cha_html);
			free(chp_escape);
		} else if(chp_tmp2) {
			memset(cha_html, '\0', sizeof(cha_html));
			chp_escape = Escape_HtmlString(chp_tmp2);
			sprintf(cha_html, "Posted by %s", chp_escape);
			free(chp_escape);
			Put_Nlist(nlp_out, "TRACKBACKLIST", cha_html);
		}
		Put_Nlist(nlp_out, "TRACKBACKLIST", " | ");
		chp_tmp = Db_GetValue(dbres, i, 4);
		if(chp_tmp){
			Put_Nlist(nlp_out, "TRACKBACKLIST", chp_tmp);
		}
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "TRACKBACKLIST", "&nbsp;<a href=\"%s?kind=6&blog_id=%s&entry_id=%s&tb_id=%s\"><img src=\"%s/btn_inform.gif\" border=\"0\" alt=\"通報\"></a>\n", cha_inform, cha_blog_temp, chp_eid, Db_GetValue(dbres, i, 6), g_cha_user_image);
		}
		Put_Nlist(nlp_out, "TRACKBACKLIST", "</div>\n");
		Put_Nlist(nlp_out, "TRACKBACKLIST", "</td></tr>\n");
	}
	Db_CloseDyna(dbres);

	if (g_in_need_login && g_in_login_blog == in_blog && !g_in_admin_mode) {
		if (Begin_Transact(db) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>", __LINE__, Gcha_last_error);
			return 1;
		}
		sprintf(cha_sql,
			" update at_trackback T1"
			" set b_tb_read = 1"
			" where T1.n_entry_id = %s"
			" and T1.n_blog_id = %d"
			, chp_eid, in_blog);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "クエリに失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
		if (Commit_Transact(db) != CO_SQL_OK) {
			Put_Nlist(nlp_out, "ERROR", "トランザクション終了に失敗しました。");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>", __LINE__, Gcha_last_error);
			return 1;
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	exitst_and_displayable_entry()
 * Description:		そのエントリは公開モードとして存在するか
 *
%* ------------------------------------------------------------------------
 * Return:
 * 		正常終了 1 存在し公開モードであるとき
 *			 0 それ以外
 *		エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int exist_and_displayable_entry(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_mode;

	strcpy(cha_sql, "select T1.b_mode");	/* mode */
	strcat(cha_sql, " from at_entry T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_entry_id = %d", in_entry_id);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if(!chp_tmp) {
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_mode = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_mode;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	exist_category()
 * Description:
 *	そのカテゴリは存在するか
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 1 存在し公開モードであるとき
 *		 0 それ以外
 *	エラー時 CO_ERROR
-* ------------------------------------------------------------------------*/
int exist_category(DBase *db, NLIST *nlp_out, int in_category_id, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_exist;

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from at_category T1");
	sprintf(cha_sql + strlen(cha_sql), " where n_category_id = %d", in_category_id);
	sprintf(cha_sql + strlen(cha_sql), " and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres , 0, 0);
	if(!chp_tmp){
		Db_CloseDyna(dbres);
		return CO_ERROR;
	}
	in_exist = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	return in_exist;
}

/*
+* ------------------------------------------------------------------------
 * Function:            visitor_list()
 * Description:
 *	訪問者リストを作成
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int disp_visitor_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_owner_room[256] = {0};
	char cha_sql[512];
	char cha_param[512];
	char cha_cgi[512];
	char *chp_esc;
	char *chp_tmp;
	char *chp_blogname;
	char *chp_nickname;
	char *chp_authormail;
	int in_disp;
//	int in_owner;
//	int in_black;
//	int in_only;
//	int in_friend;
	int in_start;
	int in_pagestart;
	int in_cnt;
	int i;

	if (g_in_login_blog != in_blog) {
		Put_Nlist(nlp_out, "ERROR", "訪問リストは参照できません。<br>");
		return 1;
	}

	if (g_in_dbb_mode) {
		sprintf(cha_sql, "select c_dbb_disp_owner_room_cgi from sy_authinfo");
		dbres = Db_OpenDyna(db, cha_sql);
		if (!dbres) {
			Put_Nlist(nlp_out, "ERROR", "訪問リストを得るクエリに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			return 1;
		}
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp) {
			strcpy(cha_owner_room, chp_tmp);
		}
		Db_CloseDyna(dbres);
	}

	sprintf(cha_sql,
		" select "
		" T2.n_visitor_id"
		",T3.c_blog_title"
		",T2.c_nickname"
		",T2.c_mailaddr"
		",date_format(T2.d_visit_time,'%%Y年%%c月%%e日 %%k時%%i分')"
		" from"
		" at_visitor T2"
		" left join at_blog T3 on T2.n_visitor_id=T3.n_blog_id"
		" where T2.n_blog_id=%d"
		" order by T2.d_visit_time desc", in_blog);

	chp_tmp = Get_Nlist(nlp_in, "start", 1);
	if (chp_tmp) {
		in_start = atoi(chp_tmp);
	} else {
		in_start = 0;
	}
	chp_tmp = Get_Nlist(nlp_in, "pagestart", 1);
	if (chp_tmp) {
		in_pagestart = atoi(chp_tmp);
	} else {
		in_pagestart = 0;
	}
	if (g_in_dbb_mode) {
		sprintf(cha_param, "&BTN_DISP_VISITOR_LIST=1");
		sprintf(cha_cgi, "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
	} else if (g_in_short_name) {
		sprintf(cha_param, "&BTN_DISP_VISITOR_LIST=1");
		sprintf(cha_cgi, "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
	} else {
		sprintf(cha_param, "&bid=%d&BTN_DISP_VISITOR_LIST=1", in_blog);
		sprintf(cha_cgi, "%s/%s", g_cha_user_cgi, CO_CGI_BUILD_HTML);
	}
	chp_tmp = Page_Control_Str_by_Sql(db, cha_sql, cha_cgi, cha_param, 10, in_start, in_pagestart, &in_cnt);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "PAGE", chp_tmp);
		free(chp_tmp);
	}
	sprintf(cha_cgi, "%d", in_cnt);
	Put_Nlist(nlp_out, "ALL", cha_cgi);

	sprintf(cha_sql + strlen(cha_sql), " limit %d, %d", in_start, 10);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "訪問リストを得るクエリに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_cnt = Db_GetRowCount(dbres);
	for (i = 0; i < in_cnt; ++i) {
		Put_Nlist(nlp_out, "LIST", "<tr>\n");
		/* ニックネーム */
		chp_tmp = Db_GetValue(dbres, i, 1);
		if (chp_tmp) {
			chp_blogname = Escape_HtmlString(chp_tmp);
		} else {
			chp_blogname = Copy_Str("");
		}
		chp_tmp = Db_GetValue(dbres, i, 3);
		chp_authormail = Escape_HtmlString(chp_tmp ? chp_tmp : "");
		if (g_in_dbb_mode) {
			if (!Get_Nickname_From_Blog(db, nlp_out, atoi(Db_GetValue(dbres, i, 0)), cha_param)) {
				return 1;
			}
			if (Blog_To_Temp(db, nlp_out, atoi(Db_GetValue(dbres, i, 0)), cha_sql)) {
				return 1;
			}
			chp_esc = Escape_HtmlString(cha_param);
			chp_tmp = Conv_Long_Ascii(chp_esc, 12);
			Put_Format_Nlist(nlp_out, "LIST",
				"<td class=\"visitor-list-nickname\"><a href=\"%s?owner=%s\" target=\"_blank\">%s</a></td>\n"
				, cha_owner_room, cha_sql, chp_tmp);
			free(chp_tmp);
			free(chp_esc);
		} else {
			chp_tmp = Db_GetValue(dbres, i, 2);
			chp_nickname = Escape_HtmlString(chp_tmp ? chp_tmp : "");
			if (chp_authormail[0]) {
				Put_Format_Nlist(nlp_out, "LIST",
					"<td class=\"visitor-list-nickname\"><a href=\"mailto:%s\">%s</a></td>\n", chp_authormail, chp_nickname);
			} else {
				Put_Format_Nlist(nlp_out, "LIST",
					"<td class=\"visitor-list-nickname\">%s</td>\n", chp_nickname);
			}
			free(chp_nickname);
		}
		if (chp_blogname[0]) {
			if (g_in_dbb_mode) {
				if (Blog_To_Temp(db, nlp_out, atoi(Db_GetValue(dbres, i, 0)), cha_sql)) {
					return 1;
				}
				in_disp = 1;
/*
				in_owner = Temp_To_Owner(db, nlp_out, cha_sql);
				if (in_owner < 0) {
					return 1;
				}
				in_black = In_Black_List(db, nlp_out, in_owner, NULL);
				if (in_black) {
					in_disp = 0;
				} else {
					in_only = Accept_Only_Friend(db, nlp_out, atoi(Db_GetValue(dbres, i, 0)));
					if (in_only < 0) {
						return 1;
					}
					if (in_only) {
						in_friend = Is_Ball_Friend(db, nlp_out, atoi(Db_GetValue(dbres, i, 0)), g_in_login_blog);
						if (in_friend < 0) {
							return 1;
						}
						if (!in_friend) {
							in_disp = 0;
						}
					}
				}
*/
				if (!in_disp) {
					Put_Format_Nlist(nlp_out, "LIST",
						"<td class=\"visitor-list-blogname\">%s</td>\n", chp_blogname);
				} else {
					Put_Format_Nlist(nlp_out, "LIST",
						"<td class=\"visitor-list-blogname\"><a href=\"%s%s%s/%s/?visitor_list=%s\">%s</a></td>\n",
						g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, cha_sql, g_cha_blog_temp, chp_blogname);
				}
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "LIST",
					"<td class=\"visitor-list-blogname\"><a href=\"%s%s%s/%08d/\">%s</a></td>\n",
					g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, atoi(Db_GetValue(dbres, i, 0)), chp_blogname);
			} else {
				Put_Format_Nlist(nlp_out, "LIST",
					"<td class=\"visitor-list-blogname\"><a href=\"%s/%s?bid=%s\">%s</a></td>\n",
					g_cha_user_cgi, CO_CGI_BUILD_HTML, Db_GetValue(dbres, i, 0), chp_blogname);
			}
		} else {
			Put_Nlist(nlp_out, "LIST", "<td class=\"visitor-list-blogname\">(ブログを開設していません)</td>\n");
		}
		chp_tmp = Db_GetValue(dbres, i, 4);
		Put_Format_Nlist(nlp_out, "LIST", "<td class=\"visitor-list-datetime\">%s</td>\n", chp_tmp ? chp_tmp : "");
		free(chp_authormail);
		free(chp_blogname);
		Put_Nlist(nlp_out, "LIST", "</tr>\n");
	}
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            search_result()
 * Description:
 *	個別記事ページの記事生成。
%* ------------------------------------------------------------------------
 * Return:                      正常終了 0
 *      戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int search_result(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[256];
	char cha_cgi[512];
	char *chp_param;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_add;
	char *chp_esc;
	char *chp_base;
	char *chp_string;
	char *chp_subject;
	char *chp_content;
	char *chp_comment;
	char *chp_trackback;
	int i;
	int in_cnt;
	int in_start;
	int in_pagestart;

	chp_string = Get_Nlist(nlp_in, "search_string", 1);
	if (!chp_string || !chp_string[0]) {
		Put_Nlist(nlp_out, "ERROR", "検索する文字列が指定されていません。<br>");
		return 1;
	}
	chp_tmp = Escape_HtmlString(chp_string);
	Put_Nlist(nlp_out, "STRING", chp_tmp);
	free(chp_tmp);

	chp_subject = Get_Nlist(nlp_in, "search_subject", 1);
	chp_content = Get_Nlist(nlp_in, "search_content", 1);
	chp_comment = Get_Nlist(nlp_in, "search_comment", 1);
	chp_trackback = Get_Nlist(nlp_in, "search_trackback", 1);
	if (!chp_subject && !chp_content && !chp_comment && !chp_trackback) {
		Put_Nlist(nlp_out, "ERROR", "検索対象が選択されていません。<br>");
		return 1;
	}

	if (chp_comment) {
		if (chp_trackback) {
			asprintf(&chp_base,
				"select distinct "
					"T1.c_entry_title,"
					"T1.c_entry_body,"
					"date_format(T1.d_entry_create_time, '%%Y年%%m月%%d日 %%h時%%i分'),"
					"T1.n_entry_id,"
					"T1.c_entry_summary "
				"from "
					"at_entry T1 "
					"left join at_comment T2 on T2.n_entry_id = T1.n_entry_id and T2.n_blog_id = %d "
					"left join at_trackback T3 on T3.n_entry_id = T1.n_entry_id and T3.n_blog_id = %d "
				"where "
					"T1.b_mode != 0 and "
					"T1.n_blog_id = %d ",
				 in_blog, in_blog, in_blog);
		} else {
			asprintf(&chp_base,
				"select distinct  "
					"T1.c_entry_title,"
					"T1.c_entry_body,"
					"date_format(T1.d_entry_create_time, '%%Y年%%m月%%d日 %%h時%%i分'),"
					"T1.n_entry_id,"
					"T1.c_entry_summary "
				"from "
					"at_entry T1 "
					"left join at_comment T2 on T2.n_entry_id = T1.n_entry_id and T2.n_blog_id = %d "
				"where "
					"T1.b_mode != 0 and "
					"T1.n_blog_id = %d ",
				 in_blog, in_blog);
		}
	} else {
		if (chp_trackback) {
			asprintf(&chp_base,
				"select distinct  "
					"T1.c_entry_title,"
					"T1.c_entry_body,"
					"date_format(T1.d_entry_create_time, '%%Y年%%m月%%d日 %%h時%%i分'),"
					"T1.n_entry_id,"
					"T1.c_entry_summary "
				"from "
					"at_entry T1 "
					"left join at_trackback T3 on T3.n_entry_id = T1.n_entry_id and T3.n_blog_id = %d "
				"where "
					"T1.b_mode != 0 and "
					"T1.n_blog_id = %d ",
				 in_blog, in_blog);
		} else {
			asprintf(&chp_base,
				"select distinct  "
					"T1.c_entry_title,"
					"T1.c_entry_body,"
					"date_format(T1.d_entry_create_time, '%%Y年%%m月%%d日 %%h時%%i分'),"
					"T1.n_entry_id,"
					"T1.c_entry_summary "
				"from "
					"at_entry T1 "
				"where "
					"T1.b_mode != 0 and "
					"T1.n_blog_id = %d ",
				 in_blog);
		}
	}

	chp_tmp = strtok(chp_string, " ");
	while (chp_tmp) {
		if (chp_tmp[0]) {
			int in_first = 1;
			chp_esc = (char*)malloc(strlen(chp_tmp) * 5);
			My_Db_EscapeString(db, chp_esc, chp_tmp, strlen(chp_tmp));
			if (chp_subject) {
				Put_Nlist(nlp_out, "SUBJECT", " checked");
				asprintf(&chp_add, "and (T1.c_entry_title like '%%%s%%' or T1.c_entry_title like '%s%%' or T1.c_entry_title like '%%%s' or T1.c_entry_title = '%s'"
					, chp_esc, chp_esc, chp_esc, chp_esc);
				Add_Str(&chp_base, chp_add);
				free(chp_add);
				in_first = 0;
			}
			if (chp_content) {
				Put_Nlist(nlp_out, "CONTENT", " checked");
				if (in_first) {
					asprintf(&chp_add,
						" and (T1.c_entry_body like '%%%s%%' or T1.c_entry_body like '%%%s' or T1.c_entry_body like '%s%%' or T1.c_entry_body = '%s'"
						" or T1.c_entry_more like '%%%s%%' or T1.c_entry_more like '%%%s' or T1.c_entry_more like '%s%%' or T1.c_entry_more = '%s'"
						 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
				} else {
					asprintf(&chp_add,
						" or T1.c_entry_body like '%%%s%%' or T1.c_entry_body like '%%%s' or T1.c_entry_body like '%s%%' or T1.c_entry_body = '%s'"
						" or T1.c_entry_more like '%%%s%%' or T1.c_entry_more like '%%%s' or T1.c_entry_more like '%s%%' or T1.c_entry_more = '%s'"
						 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
				}
				Add_Str(&chp_base, chp_add);
				free(chp_add);
				in_first = 0;
			}
			if (chp_comment) {
				Put_Nlist(nlp_out, "COMMENT", " checked");
				if (in_first) {
					asprintf(&chp_add,
						" and (T2.c_comment_body like '%%%s%%' or T2.c_comment_body like '%%%s' or T2.c_comment_body like '%s%%' or T2.c_comment_body = '%s'"
						 , chp_esc, chp_esc, chp_esc, chp_esc);
				} else {
					asprintf(&chp_add,
						" or T2.c_comment_body like '%%%s%%' or T2.c_comment_body like '%%%s' or T2.c_comment_body like '%s%%' or T2.c_comment_body = '%s'"
						 , chp_esc, chp_esc, chp_esc, chp_esc);
				}
				Add_Str(&chp_base, chp_add);
				free(chp_add);
				in_first = 0;
			}
			if (chp_trackback) {
				Put_Nlist(nlp_out, "TRACKBACK", " checked");
				if (in_first) {
					asprintf(&chp_add,
						" and (T3.c_tb_title like '%%%s%%' or T3.c_tb_title like '%%%s' or T3.c_tb_title like '%s%%' or T3.c_tb_title = '%s'"
						" or T3.c_tb_excerpt like '%%%s%%' or T3.c_tb_excerpt like '%%%s' or T3.c_tb_excerpt like '%s%%' or T3.c_tb_excerpt = '%s'"
						 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
				} else {
					asprintf(&chp_add,
						" or T3.c_tb_title like '%%%s%%' or T3.c_tb_title like '%%%s' or T3.c_tb_title like '%s%%' or T3.c_tb_title = '%s'"
						" or T3.c_tb_excerpt like '%%%s%%' or T3.c_tb_excerpt like '%%%s' or T3.c_tb_excerpt like '%s%%' or T3.c_tb_excerpt = '%s'"
						 , chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc, chp_esc);
				}
				Add_Str(&chp_base, chp_add);
				free(chp_add);
				in_first = 0;
			}
			free(chp_esc);
			if (!in_first) {
				Add_Str(&chp_base, ")");
			}
		}
		chp_tmp = strtok(NULL, " ");
	}

	chp_tmp = Get_Nlist(nlp_in, "start", 1);
	if (chp_tmp) {
		in_start = atoi(chp_tmp);
	} else {
		in_start = 0;
	}
	chp_tmp = Get_Nlist(nlp_in, "pagestart", 1);
	if (chp_tmp) {
		in_pagestart = atoi(chp_tmp);
	} else {
		in_pagestart = 0;
	}

	Encode_Url(chp_string, &chp_tmp);
	if (g_in_dbb_mode) {
		asprintf(&chp_param, "&BTN_BLOG_SEARCH=1&search_subject=%s&search_content=%s&search_comment=%s&search_trackback=%s&search_string=%s",
			chp_subject, chp_content, chp_comment, chp_trackback, chp_tmp);
		sprintf(cha_cgi, "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
	} else if (g_in_short_name) {
		asprintf(&chp_param, "&BTN_BLOG_SEARCH=1&search_subject=%s&search_content=%s&search_comment=%s&search_trackback=%s&search_string=%s",
			chp_subject, chp_content, chp_comment, chp_trackback, chp_tmp);
		sprintf(cha_cgi, "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
	} else {
		asprintf(&chp_param, "&bid=%d&BTN_BLOG_SEARCH=1&search_subject=%s&search_content=%s&search_comment=%s&search_trackback=%s&search_string=%s",
			in_blog, chp_subject, chp_content, chp_comment, chp_trackback, chp_tmp);
		sprintf(cha_cgi, "%s/%s", g_cha_user_cgi, CO_CGI_BUILD_HTML);
	}
	chp_tmp = Page_Control_Str_by_Sql(db, chp_base, cha_cgi, chp_param, 10, in_start, in_pagestart, &in_cnt);
	free(chp_param);
	if (chp_tmp) {
		Put_Nlist(nlp_out, "PAGE", chp_tmp);
		free(chp_tmp);
	}
	Put_Format_Nlist(nlp_out, "ALL", "%d", in_cnt);
	sprintf(cha_sql, " limit %d, %d", in_start, 10);
	Add_Str(&chp_base, cha_sql);

	dbres = Db_OpenDyna(db, chp_base);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, chp_base);
		free(chp_base);
		return 1;
	}
	free(chp_base);

	in_cnt = Db_GetRowCount(dbres);
	if (!in_cnt) {
		Put_Nlist(nlp_out, "RESULT_START", "<!--");
		Put_Nlist(nlp_out, "RESULT_END", "-->");
	}
	for (i = 0; i < in_cnt; ++i) {
		Put_Nlist(nlp_out, "RESULT", "<tr><td>");
		if (g_in_dbb_mode) {
			chp_tmp = Db_GetValue(dbres, i, 0);
			chp_esc = Escape_HtmlString(chp_tmp ? chp_tmp : "");
			Put_Format_Nlist(nlp_out, "RESULT",
				"<div class=\"search-result-title\"><a href=\"%s%s%s/%s/?eid=%s\">%s</a></div>\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, Db_GetValue(dbres, i, 3), chp_esc);
			free(chp_esc);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "RESULT",
				"<div class=\"search-result-title\"><a href=\"%s%s%s/%08d/?eid=%s\">%s</a></div>\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, Db_GetValue(dbres, i, 3), Db_GetValue(dbres, i, 0));
		} else {
			Put_Format_Nlist(nlp_out,
				"RESULT", "<div class=\"search-result-title\"><a href=\"%s%s%s/%s?bid=%d&eid=%s\">%s</a></div>\n",
				g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, Db_GetValue(dbres, i, 3), Db_GetValue(dbres, i, 0));
		}
		if (Db_GetValue(dbres, i, 4) && *Db_GetValue(dbres, i, 4)) {
			/* 概要はCO_MAXLEN_ENTRY_SUMMARYを超えないはず */
			chp_tmp2 = Remove_HtmlTags(Db_GetValue(dbres, i, 4));
			chp_esc = Escape_HtmlString(chp_tmp2);
			chp_tmp = Chop_String(chp_esc, CO_MAXLEN_ENTRY_SUMMARY/2);
			Put_Format_Nlist(nlp_out, "RESULT", "<div class=\"search-result-contents\">%s", chp_tmp);
			free(chp_tmp);
			free(chp_esc);
			free(chp_tmp2);
		} else if(Db_GetValue(dbres, i, 1) && *Db_GetValue(dbres, i, 1)) {
			/* 本文を概要として使用する場合は途中で切る */
			chp_tmp2 = Remove_HtmlTags(Db_GetValue(dbres, i, 1));
			chp_esc = Escape_HtmlString(chp_tmp2);
			chp_tmp = Chop_String(chp_esc, CO_MAXLEN_ENTRY_SUMMARY/2);
			Put_Format_Nlist(nlp_out, "RESULT", "<div class=\"search-result-contents\">%s", chp_tmp);
			free(chp_tmp);
			free(chp_esc);
			free(chp_tmp2);
		}
		Put_Nlist(nlp_out, "RESULT", "　|　");
		Put_Format_Nlist(nlp_out, "RESULT", "<span class=\"search-result-datetime\">%s</span></div>\n", Db_GetValue(dbres, i, 2));
		Put_Nlist(nlp_out, "RESULT", "</td></tr>\n");
	}

	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ========================================================================
 * Function:		my_check_date
 * Description:
 *	日付(YYYYMMDD)が正しいかチェックする
%* ========================================================================
 * Return:
 *	0: ＯＫ
 *	1: ＮＧ
 *	2: 入力文字エラー
-* ======================================================================*/
int my_check_date(char *chp_date)
{
	char cha_date[20];

	if(Check_Numeric(chp_date)) {
		return 2;
	}
	if(strlen(chp_date) != strlen("yyyymmdd")) {
		return 1;
	}
	Reformat_Date(cha_date, chp_date);
	if(Check_Date(cha_date)) {
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_article_archive()
 * Description:
 *	月別・日別・カテゴリ別の記事ページ作成。
%* ------------------------------------------------------------------------
 * Return:
 *		正常終了 0
 *		エラー時 1
-* ------------------------------------------------------------------------*/
int build_article_archive(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_page;
	char *chp_mode;
	char *chp_mode_value;
	char cha_title[1024];
	char cha_date[12];
	char cha_prev_date[12];
	char cha_next_date[12];
	char cha_prev_month[8];
	char cha_next_month[8];
	char cha_where[512];
	static char* chpa_top[1][2] = {{NULL, NULL}};
	static char* chpa_tail[2][2] = {{"65535", "バックナンバー"},{NULL, NULL}};
	int i;
	int in_count;
	int in_backno;
	int in_limit;
	int in_page;
	int in_total_page;
	int in_mod;
	int in_year;
	int in_month;
	int in_day;
	int in_japanese;
	int in_year_next_date;
	int in_month_next_date;
	int in_day_next_date;
	int in_year_prev_date;
	int in_month_prev_date;
	int in_day_prev_date;
	int in_year_next_month;
	int in_month_next_month;
	int in_year_prev_month;
	int in_month_prev_month;
	int in_thisyear;
	int in_thismonth;
	int in_thisday;
	int in_dayroop;
	int in_today;
	int in_exist;

	chp_mode = NULL;
	chp_mode_value = NULL;
	in_count = 0;
	in_limit = CO_PERPAGE_ARTICLE;
	memset(cha_where, '\0', sizeof(cha_where));
	memset(cha_title, '\0', sizeof(cha_title));
	/* アーカイブの種類ごとに、タイトル、件数、記事テーブル作成時のwhere文を取得する */
	/* 月別アーカイブ */
	if(Get_Nlist(nlp_in, "month", 1)) {
		chp_mode = "month";
		chp_mode_value = Get_Nlist(nlp_in, "month", 1);
		/* 数字のみで6文字、かつ、下2桁が1から12 */
		if(!chp_mode_value || strlen(chp_mode_value) != 6 || Check_Numeric(chp_mode_value)) {
			Put_Nlist(nlp_out, "ERROR", "存在しない月です。<br>");
			return 1;
		}
		in_month = atoi(chp_mode_value + 4);
		chp_mode_value[4] = 0;
		in_year = atoi(chp_mode_value);
//		sscanf(chp_mode_value, "%04d%02d", &in_year, &in_month);
		if(in_month < 1 || in_month > 12) {
			Put_Nlist(nlp_out, "ERROR", "存在しない月です。<br>");
			return 1;
		}
		/* 曜日の日本語表示を得る */
		in_japanese = Calender_Language(db, nlp_out, in_blog);
		if (in_japanese < 0){
			return 1;
		}

		Put_Nlist(nlp_out, "CALENDER",
			"<a name=\"calendar\"></a>\n"
			"<table align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"760\">\n"
			"<tr>\n"
			"	<td>\n"
			"	<table cellspacing=\"1\" border=\"0\" cellpadding=\"0\" width=\"100%\">\n"
			"	<tr valign=\"bottom\">\n"
			"		<td class=\"calendar-month_a\">\n");
		if (in_japanese) {
			Put_Format_Nlist(nlp_out, "CALENDER", "%04d年%s", in_year, chpa_month_jp[in_month - 1]);
		} else {
			Put_Format_Nlist(nlp_out, "CALENDER", "%04d %s", in_year, chpa_month[in_month - 1]);
		}
		Put_Nlist(nlp_out, "CALENDER", "</td>\n");

		/* 次の月が年をまたぐ */
		if(in_month == 12) {
			in_year_next_month = in_year + 1;
			in_month_next_month = 1;
		} else {
			in_year_next_month = in_year;
			in_month_next_month = in_month + 1;
		}
		memset(cha_next_month, '\0', sizeof(cha_next_month));
		sprintf(cha_next_month, "%04d/%02d", in_year_next_month, in_month_next_month);
		/* 前の月が年をまたぐ */
		if(in_month == 1) {
			in_year_prev_month = in_year - 1;
			in_month_prev_month = 12;
		} else {
			in_year_prev_month = in_year;
			in_month_prev_month = in_month - 1;
		}
		sprintf(cha_prev_month, "%04d/%02d", in_year_prev_month, in_month_prev_month);

		/* 一月の日数を得るうるう年(2月) */
		Get_NowDate(cha_date);
		cha_date[4] = '\0';
		cha_date[7] = '\0';
		in_thisyear = atoi(cha_date);
		in_thismonth = atoi(cha_date + 5);
		in_thisday = atoi(cha_date + 8);
		in_dayroop = ina_mday[in_month];
		if(in_month == 2 && in_year % 4 == 0 && in_year % 400 != 0){
			in_dayroop += 1;
		}
		for (i = 1; i <= in_dayroop; ++i) {
			in_today = (in_year == in_thisyear && in_month == in_thismonth && i == in_thisday);
			Put_Nlist(nlp_out, "CALENDER", "<td class=\"");
			if (in_today) {
				Put_Nlist(nlp_out, "CALENDER", "today_a\">");
			} else {
				Put_Nlist(nlp_out, "CALENDER", "cell_a\">");
			}
			sprintf(cha_date, "%04d%02d%02d", in_year, in_month, i);
			in_exist = get_count_entry_by_date(db, nlp_out, cha_date, in_blog);
			if (in_exist) {
				if (in_today) {
					Put_Format_Nlist(nlp_out, "CALENDER", "<a href=\"#entry%02d%02d\" title=\"今日\"><span title=\"今日\">% 2d</span></a>", in_month, i, i);
				} else {
					Put_Format_Nlist(nlp_out, "CALENDER", "<a href=\"#entry%02d%02d\">% 2d</a>", in_month, i, i);
				}
			} else {
				Put_Format_Nlist(nlp_out, "CALENDER", "% 2d", i);
			}
			Put_Nlist(nlp_out, "CALENDER", "</td>\n");
		}
		Put_Nlist(nlp_out, "CALENDER",
			"		</tr>\n"
			"		</table>\n"
			"	</td>\n"
			"</tr>\n"
			"</table>\n");

		/* 前の月へのリンク */
		Put_Nlist(nlp_out, "LIST_TITLE", "<div class=\"article-list-title\">");
		if (in_year_prev_month < 0) {
			Put_Nlist(nlp_out, "LIST_TITLE", "前月");
		} else if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%s/?month=%04d%02d\" title=\"%s\">前月</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year_prev_month, in_month_prev_month, cha_prev_month);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%08d/?month=%04d%02d\" title=\"%s\">前月</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year_prev_month, in_month_prev_month, cha_prev_month);
		} else {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s/%s?month=%04d%02d&bid=%d\" title=\"%s\">前月</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year_prev_month, in_month_prev_month, in_blog, cha_prev_month);
		}
		/* 表示している月 */
		Put_Format_Nlist(nlp_out, "LIST_TITLE", "　|　記事トップページ　|　", in_year, in_month);
		/* 次の月へのリンク */
		if (in_year_next_month >= 10000) {
			Put_Nlist(nlp_out, "LIST_TITLE", "翌月");
		} else if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%s/?month=%04d%02d\" title=\"%s\">翌月</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year_next_month, in_month_next_month, cha_next_month);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%08d/?month=%04d%02d\" title=\"%s\">翌月</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year_next_month, in_month_next_month, cha_next_month);
		} else {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s/%s?month=%04d%02d&bid=%d\" title=\"%s\">翌月</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year_next_month, in_month_next_month, in_blog, cha_next_month);
		}
		Put_Nlist(nlp_out, "LIST_TITLE", "</div>\n");

		in_count = 1;
		in_limit = 0;
//		in_count = get_count_entry_month(db, nlp_in, nlp_out, Get_Nlist(nlp_in, "month", 1), in_blog);
		sprintf(cha_where, " and T1.d_entry_create_time >= '%d%02d01'", in_year, in_month);
		sprintf(cha_where + strlen(cha_where), " and T1.d_entry_create_time < '%d%02d31235959'", in_year, in_month);
	/* 日付アーカイブ */
	} else if(Get_Nlist(nlp_in, "date", 1)) {
		chp_mode = "date";
		/* yyyymmddが提供される必要がある。 */
		chp_mode_value = Get_Nlist(nlp_in, "date", 1);
		if(my_check_date(chp_mode_value)) {
			Put_Nlist(nlp_out, "ERROR", "存在しない日付です。<br>");
			return 1;
		}
		memset(cha_date, '\0', sizeof(cha_date));
		memset(cha_prev_date, '\0', sizeof(cha_prev_date));
		memset(cha_next_date, '\0', sizeof(cha_next_date));
		/* Get_xx_dateには"yyyy/mm/dd"の形が必要 */
		Reformat_Date(cha_date, chp_mode_value);
		Get_PrevDate(cha_date, 1, cha_prev_date);
		Get_NextDate(cha_date, 1, cha_next_date);
		/* cha_xxx_dateにはyyyy/mm/ddが期待される */
		/* next_date、prevdateから分解 */
		sscanf(cha_next_date, "%4d/%2d/%2d", &in_year_next_date, &in_month_next_date, &in_day_next_date);
		sscanf(cha_prev_date, "%4d/%2d/%2d", &in_year_prev_date, &in_month_prev_date, &in_day_prev_date);

		/* 前日へのリンク */
		Put_Nlist(nlp_out, "LIST_TITLE", "<div class=\"article-list-title\">");
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%s/?date=%04d%02d%02d\" title=\"%s\">前日</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year_prev_date, in_month_prev_date, in_day_prev_date, cha_prev_date);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%08d/?date=%04d%02d%02d\" title=\"%s\">前日</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year_prev_date, in_month_prev_date, in_day_prev_date, cha_prev_date);
		} else {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s/%s?date=%04d%02d%02d&bid=%d\" title=\"%s\">前日</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year_prev_date, in_month_prev_date, in_day_prev_date, in_blog, cha_prev_date);
		}
		/* 表示している日の表示 */
		/* 8桁の数字を年月日に分解 */
		sscanf(chp_mode_value, "%4d%2d%2d", &in_year, &in_month, &in_day);
		Put_Format_Nlist(nlp_out, "LIST_TITLE", "　|　%04d年%02d月%02d日の記事　|　", in_year, in_month, in_day);
		/* 後日へのリンク */
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%s/?date=%04d%02d%02d\" title=\"%s\">翌日</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_year_next_date, in_month_next_date, in_day_next_date, cha_next_date);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s%s%s/%08d/?date=%04d%02d%02d\" title=\"%s\">翌日</a>"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_year_next_date, in_month_next_date, in_day_next_date, cha_next_date);
		} else {
			Put_Format_Nlist(nlp_out, "LIST_TITLE", "<a href=\"%s/%s?date=%04d%02d%02d&bid=%d\" title=\"%s\">翌日</a>"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_year_next_date, in_month_next_date, in_day_next_date, in_blog, cha_next_date);
		}
		Put_Nlist(nlp_out, "LIST_TITLE", "</div>\n");

		in_count = get_count_entry_by_date(db, nlp_out, chp_mode_value, in_blog);
		sprintf(cha_where, " and T1.d_entry_create_time >= '%s000000'", chp_mode_value);
		sprintf(cha_where + strlen(cha_where), " and T1.d_entry_create_time <= '%s235959'", chp_mode_value);
	/* カテゴリ別ページ */
	} else if(Get_Nlist(nlp_in, "cid", 1)) {
		chp_mode = "cid";
		chp_mode_value = Get_Nlist(nlp_in, "cid", 1);
		if(Check_Numeric(chp_mode_value) || (atoi(chp_mode_value) != CO_BACKNO_CATEGORY && exist_category(db, nlp_out, atoi(chp_mode_value), in_blog) != 1)) {
			Put_Nlist(nlp_out, "ERROR", "そのカテゴリーは存在しません。<br>");
			return 1;
		}
		sprintf(cha_date, "%d", in_blog);
		sprintf(cha_title,
			"select distinct "
				"T1.n_category_id,"
				"T1.c_category_name "
			"from "
				"at_category T1,"
				"at_entry T2 "
			"where "
				"T1.n_blog_id = %d and "
				"T2.n_blog_id = %d and "
				"T1.n_category_id = T2.n_category_id "
			"order by n_category_order", in_blog, in_blog);
		Put_Nlist(nlp_out, "LIST_TITLE", "<span class=\"article-list-title\">カテゴリー選択：</span>");
		in_backno = 0;
		if (g_in_cart_mode == CO_CART_SHOPPER) {
			in_backno = Get_Back_Number(db, nlp_out, in_blog);
			if (in_backno < 0) {
				return 1;
			}
		}
		Build_ComboDb(OldDBase(db), nlp_out, "LIST_TITLE", cha_title, "cid\" class=\"article-category-title\" onchange=\"this.form.submit()",
			chpa_top, g_in_cart_mode == CO_CART_SHOPPER && in_backno ? chpa_tail : NULL, chp_mode_value);
		Put_Nlist(nlp_out, "LIST_TITLE", "<br>\n");
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "HIDDEN", "<input type=\"hidden\" name=\"bid\" value=\"%s\">\n", g_cha_blog_temp);
		} else {
			Build_HiddenEncode(nlp_out, "HIDDEN", "bid", cha_date);
		}
		in_count = get_count_entry_category(db, nlp_in, nlp_out, chp_mode_value, in_blog);
		strcpy(cha_where, " and T1.n_category_id = ");
		strcat(cha_where, chp_mode_value);
	}
	/* 記事数が取れなかったとき */
	if(in_count == CO_ERROR) {
		Put_Nlist(nlp_out, "ERROR", "記事数を取得できませんでした。");
		return 1;
	}
	/* ページ数を得る */
	in_mod = in_count % CO_PERPAGE_ARTICLE;
	if(in_mod == 0) {
		in_total_page = in_count / CO_PERPAGE_ARTICLE;
	} else {
		in_total_page = 1 + (in_count / CO_PERPAGE_ARTICLE);
	}
	/* 現在いるページを得る */
	chp_page = Get_Nlist(nlp_in, "page", 1);
	if(chp_page) {
		in_page = atoi(chp_page);
	} else {
		in_page = 0;
	}
	if(in_total_page > 0 && in_page < 0 && in_page >= in_total_page) {
		Put_Nlist(nlp_out, "ERROR", "存在しないページです。");
		return 1;
	}

	/* 記事リストのタイトル(ページめくりとリストのタイトル) */
	if(in_total_page > 1 && chp_mode_value) {
		Put_Nlist(nlp_out, "PAGELINK", "<table width=\"100%\"><tr><td align=\"center\"><table><tr>\n");
		/* 前のページへのリンク */
		if(in_page == 0) {
			Put_Format_Nlist(nlp_out, "PAGELINK",
				"<td align=\"left\"><a title=\"リストの先頭です\"><img src=\"%s/%s\" border=\"0\"></a></td>\n", g_cha_theme_image, gcha_theme_first_icon);
			Put_Format_Nlist(nlp_out, "PAGELINK",
				"<td align=\"left\"><a title=\"リストの先頭です\"><img src=\"%s/%s\" border=\"0\"></a></td>\n", g_cha_theme_image, gcha_theme_prev_page_icon);
		} else {
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"left\"><a href=\"%s%s%s/%s/?%s=%s&page=0#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_mode, chp_mode_value, g_cha_theme_image, gcha_theme_first_icon);
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"left\"><a href=\"%s%s%s/%s/?%s=%s&page=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_mode,chp_mode_value, in_page - 1, g_cha_theme_image, gcha_theme_prev_page_icon);
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"left\"><a href=\"%s%s%s/%08d/?%s=%s&page=0#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_mode, chp_mode_value, g_cha_theme_image, gcha_theme_first_icon);
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"left\"><a href=\"%s%s%s/%08d/?%s=%s&page=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_mode,chp_mode_value, in_page - 1, g_cha_theme_image, gcha_theme_prev_page_icon);
			} else {
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"left\"><a href=\"%s/%s?%s=%s&page=0&bid=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_mode,chp_mode_value, in_blog, g_cha_theme_image, gcha_theme_first_icon);
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"left\"><a href=\"%s/%s?%s=%s&page=%d&bid=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_mode,chp_mode_value, in_page - 1, in_blog, g_cha_theme_image, gcha_theme_prev_page_icon);
			}
		}
		/* 次のページへのリンク */
		if(in_page == in_total_page - 1) {
			Put_Format_Nlist(nlp_out, "PAGELINK",
				"<td align=\"right\"><a title=\"リストの最後です\"><img src=\"%s/%s\" border=\"0\"></a></td>\n", g_cha_theme_image, gcha_theme_next_page_icon);
			Put_Format_Nlist(nlp_out, "PAGELINK",
				"<td align=\"right\"><a title=\"リストの最後です\"><img src=\"%s/%s\" border=\"0\"></a></td>\n", g_cha_theme_image, gcha_theme_last_icon);
		} else {
			if (g_in_dbb_mode) {
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"right\"><a href=\"%s%s%s/%s/?%s=%s&page=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_mode, chp_mode_value, in_page + 1, g_cha_theme_image, gcha_theme_next_page_icon);
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"right\"><a href=\"%s%s%s/%s/?%s=%s&page=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_mode, chp_mode_value, in_total_page - 1, g_cha_theme_image, gcha_theme_last_icon);
			} else if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"right\"><a href=\"%s%s%s/%08d/?%s=%s&page=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_mode, chp_mode_value, in_page + 1, g_cha_theme_image, gcha_theme_next_page_icon);
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"right\"><a href=\"%s%s%s/%08d/?%s=%s&page=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_mode, chp_mode_value, in_total_page - 1, g_cha_theme_image, gcha_theme_last_icon);
			} else {
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"right\"><a href=\"%s/%s?%s=%s&page=%d&bid=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_mode, chp_mode_value, in_page + 1, in_blog, g_cha_theme_image, gcha_theme_next_page_icon);
				Put_Format_Nlist(nlp_out, "PAGELINK",
					"<td align=\"right\"><a href=\"%s/%s?%s=%s&page=%d&bid=%d#archive_top\"><img src=\"%s/%s\" border=\"0\"></a></td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_mode, chp_mode_value, in_total_page - 1, in_blog, g_cha_theme_image, gcha_theme_last_icon);
			}
		}
		Put_Nlist(nlp_out, "PAGELINK", "</tr></table></td></tr></table>\n");
	}
	if (strcmp(chp_mode, "cid") == 0 && atoi(chp_mode_value) == CO_BACKNO_CATEGORY) {
		if(Build_Back_Number(db, nlp_in, nlp_out, "", in_limit, in_page * in_limit, in_blog)) {
			return 1;
		}
	} else {
		if(build_article_detail(db, nlp_in, nlp_out, cha_where, in_limit, in_page * in_limit, in_blog)) {
			return 1;
		}
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_article_eid()
 * Description:
 *	個別記事ページの記事生成。
%* ------------------------------------------------------------------------
 * Return:                      正常終了 0
 *      戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int build_article_eid(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_html[8192];
	char cha_sql [256];
	int i;
	int in_oldest_eid;
	int in_prev_eid;
	int in_latest_eid;
	int in_next_eid;
	int in_entry_id;
	int in_access;
	int in_deny;
	int in_trackback;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if(Check_Numeric(chp_tmp) || exist_and_displayable_entry(db, nlp_out, atoi(chp_tmp), in_blog) != 1) {
		Put_Nlist(nlp_out, "ERROR", "そのエントリは存在しません。<br>");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp);
	in_entry_id = atoi(chp_tmp);
	/* ページめくり関連 */
	/* 最も古い記事のとき、最も古いリンクを表示しない */
	in_oldest_eid = Get_Id_Oldest_Entry_Create_Time_Not_Draft(db, in_blog);
	memset(cha_html, '\0', sizeof(cha_html));
	if(in_entry_id != in_oldest_eid) {
		if (g_in_dbb_mode) {
			sprintf(cha_html, "<a href=\"%s%s%s/%s/?eid=%d#explore\" title=\"最も古い記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_oldest_eid, g_cha_theme_image, gcha_theme_first_icon);
		} else if (g_in_short_name) {
			sprintf(cha_html, "<a href=\"%s%s%s/%08d/?eid=%d#explore\" title=\"最も古い記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_oldest_eid, g_cha_theme_image, gcha_theme_first_icon);
		} else {
			sprintf(cha_html, "<a href=\"%s/%s?eid=%d&bid=%d#explore\" title=\"最も古い記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_oldest_eid, in_blog, g_cha_theme_image, gcha_theme_first_icon);
		}
		/* 一つ前のエントリIDを取る */
		in_prev_eid = Get_Id_Prev_Entry_Create_Time(db, in_entry_id, in_blog);
		if (g_in_dbb_mode) {
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s%s%s/%s/?eid=%d#explore\" title=\"前の記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_prev_eid, g_cha_theme_image, gcha_theme_prev_entry_icon);
		} else if (g_in_short_name) {
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s%s%s/%08d/?eid=%d#explore\" title=\"前の記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_prev_eid, g_cha_theme_image, gcha_theme_prev_entry_icon);
		} else {
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s/%s?eid=%d&bid=%d#explore\" title=\"前の記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_prev_eid, in_blog, g_cha_theme_image, gcha_theme_prev_entry_icon);
		}
	} else {
		sprintf(cha_html + strlen(cha_html),
			"<img src=\"%s/%s\" border=\"0\">\n<img src=\"%s/%s\" border=\"0\">\n",
			g_cha_theme_image, gcha_theme_first_icon, g_cha_theme_image, gcha_theme_prev_entry_icon);
	}
	Put_Nlist(nlp_out, "ARTICLE", cha_html);
	memset(cha_html, '\0', sizeof(cha_html));
	/* 最も新しい記事のとき、最も新しいリンクを表示しない */
	in_latest_eid = Get_Id_Latest_Entry_Create_Time_Not_Draft(db, in_blog);
	if(in_entry_id != in_latest_eid) {
		in_next_eid = Get_Id_Next_Entry_Create_Time(db, in_entry_id, in_blog);
		if (g_in_dbb_mode) {
			sprintf(cha_html
				, "<a href=\"%s%s%s/%s/?eid=%d#explore\" title=\"次の記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_next_eid, g_cha_theme_image, gcha_theme_next_entry_icon);
			/* 一つ次のエントリIDを取る */
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s%s%s/%s/?eid=%d#explore\" title=\"最も新しい記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, in_latest_eid, g_cha_theme_image, gcha_theme_last_icon);
		} else if (g_in_short_name) {
			sprintf(cha_html
				, "<a href=\"%s%s%s/%08d/?eid=%d#explore\" title=\"次の記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_next_eid, g_cha_theme_image, gcha_theme_next_entry_icon);
			/* 一つ次のエントリIDを取る */
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s%s%s/%08d/?eid=%d#explore\" title=\"最も新しい記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_latest_eid, g_cha_theme_image, gcha_theme_last_icon);
		} else {
			sprintf(cha_html
				, "<a href=\"%s/%s?eid=%d&bid=%d#explore\" title=\"次の記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_next_eid, in_blog, g_cha_theme_image, gcha_theme_next_entry_icon);
			/* 一つ次のエントリIDを取る */
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s/%s?eid=%d&bid=%d#explore\" title=\"最も新しい記事\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_latest_eid, in_blog, g_cha_theme_image, gcha_theme_last_icon);
		}
	} else {
		sprintf(cha_html + strlen(cha_html),
			"<img src=\"%s/%s\" border=\"0\">\n<img src=\"%s/%s\" border=\"0\">\n",
			g_cha_theme_image, gcha_theme_next_entry_icon, g_cha_theme_image, gcha_theme_last_icon);
	}
	Put_Nlist(nlp_out, "ARTICLE", cha_html);
	/* 本文組み立て */
	sprintf(cha_sql, " and T1.n_entry_id = %d", in_entry_id);
	if (build_article_detail(db, nlp_in, nlp_out, cha_sql, 0, 0, in_blog)) {
		return 1;
	}
	/* コメントを受け付けるときはもしくは既に投稿されたコメントがあるときは、投稿されたコメントを表示 */
	if (Get_Flag_Acceptable_Comment(db, nlp_out, in_entry_id, in_blog, g_in_login_blog) ||
		Get_Total_Comment_By_Entry_Id(db, nlp_in, nlp_out, in_entry_id, in_blog, 0) != 0) {
		if(build_comment_list(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	}
	/* コメントを受け付けるときのみ、コメント投稿フォームを追加 */
	strcpy(cha_sql, "select T1.c_host");	/* 0 コメント禁止ホスト */
	strcat(cha_sql, " from at_denycomment T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "禁止IPリストの取得に失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	in_deny = 0;
	for (i = 0; i < Db_GetRowCount(dbres); ++i) {
		/* 禁止ホストは改行区切りで格納されている。一つずつ比べて一致するものがあれば拒否。 */
		chp_tmp = Db_GetValue(dbres, i, 0);
		if(chp_tmp && *chp_tmp && atoi(chp_tmp) == g_in_login_owner) {
			in_deny = 1;
			break;
		}
	}
	Db_CloseDyna(dbres);
	if (!g_in_black_list && !g_in_admin_mode && !in_deny &&
		Get_Flag_Acceptable_Comment(db, nlp_out, in_entry_id, in_blog, g_in_login_blog)) {
		if (build_comment_form(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	}
	/* トラックバックを受け付けるときは、投稿されたトラックバックを表示 */
	in_access = Get_Flag_Acceptable_Trackback(db, nlp_out, in_entry_id, in_blog);
	if (Get_Total_Trackback_By_Entry_Id(db, nlp_in, nlp_out, in_entry_id, in_blog, 0) != 0) {
		if (build_trackback_list(db, nlp_in, nlp_out, in_blog, in_access)) {
			return 1;
		}
	}
	in_trackback = 1;
	if (g_in_dbb_mode) {
		if (in_access == CO_CMTRB_FRIEND) {
			in_trackback = Is_Ball_Friend(db, nlp_out, in_blog, g_in_login_blog);
			if (in_trackback < 0) {
				return 1;
			}
		}
		Put_Format_Nlist(nlp_out, "PATH", "%s%s%s/%s?blogid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_admin_cgi, CO_CGI_ENTRY, in_blog);
		Put_Format_Nlist(nlp_out, "BID", "%d", in_blog);
		Put_Format_Nlist(nlp_out, "EID", "%d", in_entry_id);
	}
	if (!in_access || trackback_ip_check(db, nlp_out, in_entry_id, in_blog, g_in_login_owner)) {
		Put_Nlist(nlp_out, "TBSTP_START", "<!--");
		Put_Nlist(nlp_out, "TBSTP_END", "-->");
	} else if (g_in_dbb_mode && (!in_trackback || g_in_black_list || g_in_admin_mode)) {
		Put_Nlist(nlp_out, "TBSTP_START", "<!--");
		Put_Nlist(nlp_out, "TBSTP_END", "-->");
	}

	if (g_in_cart_mode == CO_CART_SHOPPER) {
		if (Build_Review_List(db, nlp_in, nlp_out, in_blog, in_entry_id)) {
			return 1;
		}
		if (Get_Allow_Review(db, nlp_in, nlp_out, in_blog, 1) > 0 &&
			Build_Review_Form(db, nlp_in, nlp_out, in_blog, in_entry_id)) {
			return 1;
		}
	}
	/* トップページ */
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            build_article_toppage()
 * Description:
 *		トップページの記事生成。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_article_toppage(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char cha_start_day[32];
	char cha_fin_day[32];
	char cha_where[1024];
	char cha_entry[32];
	int in_disptype;
	int in_latest_entry_id;
	int in_count;
	int in_limit;

	in_disptype = get_disptype_toppage(db, in_blog);
	if(in_disptype == CO_ERROR) {
		Put_Nlist(nlp_out, "ERROR", "トップページの設定がされていません。");
		return 1;
	}
	if(Get_Total_Entry(db, nlp_in, nlp_out, 1, in_blog) == 0) {
		Put_Nlist(nlp_out, "ARTICLE", "<span class=\"article-list-title\">記事はありません。</span>");
		return 0;
	}
	/* 最近のIDを取る */
	in_latest_entry_id = Get_Id_Latest_Entry_Current_Time(db, in_blog);
	if(in_latest_entry_id == CO_ERROR){
		Put_Nlist(nlp_out, "ERROR", "今日の最新記事IDの取得に失敗しました。");
		return 1;
	}
	memset(cha_entry, '\0', sizeof(cha_entry));
	if (!in_latest_entry_id) {
		Get_DateTimeNow(cha_entry);
		cha_entry[4] = '-';
		cha_entry[7] = '-';
		strcat(cha_entry, ":00");
	} else {
		Get_Create_Time_By_Entry_Id(db, in_latest_entry_id, in_blog, cha_entry);
	}
	memset(cha_where, '\0', sizeof(cha_where));
	/* 最新N件の記事 */
	if(in_disptype == 0){
		/* 設定する記事数を取る */
		in_count = get_count_toppage_article(db, nlp_out, in_blog);
		if(in_count == CO_ERROR) {
			Put_Nlist(nlp_out, "ERROR", "記事数の取得に失敗しました。");
			return 1;
		}
		in_limit = in_count;
		if(cha_entry[0] != '\0') {
			strcpy(cha_where, " and T1.d_entry_create_time <= '");
			strcat(cha_where, cha_entry);
			strcat(cha_where, "'");
		}
	/* 最近N日の記事 */
	} else {
		/* 設定する記事数を取る */
		in_count = get_day_toppage_article(db, in_blog);
		if(in_count == CO_ERROR) {
			Put_Nlist(nlp_out, "ARTICLE", "記事日数の取得に失敗しました。");
			return 1;
		}
		/* sql に渡すwhere文を作る。 */
		get_toppage_dayspan(db, in_count, cha_start_day, cha_fin_day, in_blog);
		sprintf(cha_where, " and T1.d_entry_create_time >= '%s000000'", cha_start_day);
		sprintf(cha_where + strlen(cha_where), " and T1.d_entry_create_time <= '%s235959'", cha_fin_day);
		sprintf(cha_where + strlen(cha_where), " and T1.d_entry_create_time <= now()");
		/* 日付の範囲内の記事は全て表示 */
		in_limit = 0;
	}
	if(build_article_detail(db, nlp_in, nlp_out, cha_where, in_limit, 0, in_blog)) {
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_title()
 * Description:
 *	ブログのトップタイトルを生成する
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_title(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_subtitle;
	char *chp_title;
	char *chp_ascii;
	char *chp_esc;
	char *chp_br;
	char cha_html[8192];
	char cha_sql[512];

	strcpy(cha_sql, "select T1.c_blog_title");	/* 0 ブログタイトル */
	strcat(cha_sql ,", T1.c_blog_subtitle");	/* 1 ブログサブタイトル */
	strcat(cha_sql ," from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_title = Db_GetValue(dbres, 0 , 0);
	if(chp_title) {
		chp_esc = Escape_HtmlString(chp_title);
		chp_ascii = Conv_Long_Ascii(chp_esc, 60);
		chp_br = Conv_Blog_Br(chp_ascii);
		if (g_in_dbb_mode) {
			sprintf(cha_html, "<a href=\"%s%s%s/%s/\">%s</a><br>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp, chp_br);
		} else if (g_in_short_name) {
			sprintf(cha_html, "<a href=\"%s%s%s/%08d/\">%s</a><br>\n", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_br);
		} else {
			sprintf(cha_html, "<a href=\"%s/%s?bid=%d\">%s</a><br>\n", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, chp_br);
		}
		/* トップタイトル */
		Put_Nlist(nlp_out, "TITLE", cha_html);
		/* ブラウザのタイトル */
		Put_Nlist(nlp_out, "BLOGTITLE", chp_esc);
		free(chp_br);
		free(chp_ascii);
		free(chp_esc);
	}
	chp_subtitle = Db_GetValue(dbres, 0, 1);
	if(chp_subtitle) {
		int in_crlf = 0;
		int in_len = strlen(chp_subtitle);
		if (in_len > strlen("\r\n") && strcmp(chp_subtitle + in_len - strlen("\r\n"), "\r\n") == 0) {
			in_crlf = 1;
		} else if (in_len > strlen("\n") && strcmp(chp_subtitle + in_len - strlen("\n"), "\n") == 0) {
			in_crlf = 1;
		}
		if (!in_crlf) {
			asprintf(&chp_br, "%s\n", chp_subtitle);
			chp_esc = Escape_HtmlString(chp_br);
			free(chp_br);
		} else {
			chp_esc = Escape_HtmlString(chp_subtitle);
		}
		sprintf(cha_html, "<span class=\"description\">%s</span>\n", chp_esc);
		free(chp_esc);
		Put_Nlist(nlp_out, "TITLE", cha_html);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_main()
 * Description:
 *		メインページ作成
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int build_main(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
//	DBRes *dbres;
//	char cha_sql[512];
//	char *chp_ip;
//	char *chp_ua;
//	char *chp_tmp;
//	int b_vld;
//	int in_cnt;

	/* ブログタイトルの作成 */
	if(build_title(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	/* 検索結果 */
	if (Get_Nlist(nlp_in, "BTN_BLOG_SEARCH", 1)) {
		if (search_result(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	/* 訪問者リスト */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_VISITOR_LIST", 1)) {
		if (disp_visitor_list(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	/* 日・月・カテゴリー別アーカイブ画面 */
	} else if (Get_Nlist(nlp_in, "date", 1) || Get_Nlist(nlp_in, "month", 1) || Get_Nlist(nlp_in, "cid", 1)) {
		if (build_article_archive(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	/* 個別記事ページ */
	} else if (Get_Nlist(nlp_in, "eid", 1)) {
		if (build_article_eid(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	/* 個別バックナンバーページ */
	} else if (Get_Nlist(nlp_in, "kid", 1)) {
		if (Build_Mail_Kid(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	/* トップページ */
	} else {
		if (build_sidebar(db, nlp_in, nlp_out, in_blog)){
			return 1;
		}
		if (build_article_toppage(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	}

	return 0;
}
/*
+* ------------------------------------------------------------------------
 * Function:	 	page_out_usertop()
 * Description:
 *	ユーザートップページ
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1
-* ------------------------------------------------------------------------*/
int page_out_usertop(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_count_sidecontent;

	in_count_sidecontent = get_sidecontent_visible(db, nlp_out, in_blog);
	/* サイドコンテンツがないときは、サイドバー無しのスケルトンにページアウト */
	if(in_count_sidecontent == 0) {
		Page_Out(nlp_out, CO_SKEL_ARTICLE_LIST);
		return 0;
	}
	strcpy(cha_sql, "select T1.n_sidebar");		/* 0 サイドバー方向 */
	strcat(cha_sql, " from at_looks T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "レイアウトを得るクエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0 , 0);
	if(chp_tmp) {
		if(atoi(chp_tmp) == 1) {
			Page_Out(nlp_out, CO_SKEL_USERTOP_RIGHT);
		} else {
			Page_Out(nlp_out, CO_SKEL_USERTOP_LEFT);
		}
	} else {
		/* 取れなかったらとりあえず左に */
		Page_Out(nlp_out, CO_SKEL_USERTOP_LEFT);
	}
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_root()
 * Description:
 *	画面に現れないところを作成する
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int build_root(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_css[1024];
	char cha_sql[1024];
	char cha_rss[512];
	int in_skin_no = 1;
	int in_category_no = 1;
	int in_ret;

	chp_tmp = getenv("SERVER_NAME");
	if (chp_tmp) {
		strcpy(cha_rss, chp_tmp);
	} else {
		gethostname(cha_rss, sizeof(cha_rss));
	}

	/* スタイルシート */
	strcpy(cha_sql, "select T1.n_theme_id");	/* 0 テンプレートナンバー */
	strcat(cha_sql, ", T1.c_user_css");			/* 1 ユーザー指定CSS */
	strcat(cha_sql, ", T1.n_category_id");		/* 2 カテゴリ */
	strcat(cha_sql, " from at_looks T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ユーザーのCSS設定を取得するパスに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp) {
		in_skin_no = atoi(chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if(chp_tmp) {
		sprintf(cha_css, "%s%s%s/%s", g_cha_protocol, cha_rss, g_cha_css_location, chp_tmp);
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if(chp_tmp) {
		in_category_no = atoi(chp_tmp);
	}
	Db_CloseDyna(dbres);

	/* テンプレートナンバーが0のときはユーザー指定 */
	/* それ以外のときは、用意されたCSSへのパスを改めてコピー */
	strcpy(cha_sql, " select T1.c_theme_file");	/* 0 用意されたテーマファイル */
	strcat(cha_sql, ",T1.c_theme_prev_entry_icon");
	strcat(cha_sql, ",T1.c_theme_next_entry_icon");
	strcat(cha_sql, ",T1.c_theme_prev_page_icon");
	strcat(cha_sql, ",T1.c_theme_next_page_icon");
	strcat(cha_sql, ",T1.c_theme_first_icon");
	strcat(cha_sql, ",T1.c_theme_last_icon");
	strcat(cha_sql, ",T1.c_theme_plus_icon");
	strcat(cha_sql, ",T1.c_theme_minus_icon");
	strcat(cha_sql, ",T1.c_theme_page_top_icon");
	strcat(cha_sql, ",T1.c_theme_blog_top_icon");
	strcat(cha_sql, " from sy_theme T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_theme_id = %d and T1.n_category_id = %d", in_skin_no, in_category_no);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "用意されたCSSファイル名取得に失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	if (in_skin_no) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if(chp_tmp) {
			sprintf(cha_css, "%s%s%s/%s", g_cha_protocol, cha_rss, g_cha_css_location, chp_tmp);
		}
	}
	Put_Nlist(nlp_out, "PATHCSS", cha_css);
	strcpy(gcha_theme_prev_entry_icon, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "news_back_01.gif");
	strcpy(gcha_theme_next_entry_icon, Db_GetValue(dbres, 0, 2) ? Db_GetValue(dbres, 0, 2) : "news_next_01.gif");
	strcpy(gcha_theme_prev_page_icon, Db_GetValue(dbres, 0, 3) ? Db_GetValue(dbres, 0, 3) : "news_backpage_01.gif");
	strcpy(gcha_theme_next_page_icon, Db_GetValue(dbres, 0, 4) ? Db_GetValue(dbres, 0, 4) : "news_nextpage_01.gif");
	strcpy(gcha_theme_first_icon, Db_GetValue(dbres, 0, 5) ? Db_GetValue(dbres, 0, 5) : "news_back_01_01.gif");
	strcpy(gcha_theme_last_icon, Db_GetValue(dbres, 0, 6) ? Db_GetValue(dbres, 0, 6) : "news_next_01_01.gif");
	strcpy(gcha_theme_plus_icon, Db_GetValue(dbres, 0, 7) ? Db_GetValue(dbres, 0, 7) : "plus_01.gif");
	strcpy(gcha_theme_minus_icon, Db_GetValue(dbres, 0, 8) ? Db_GetValue(dbres, 0, 8) : "minus_01.gif");
	strcpy(gcha_theme_back_top_icon, Db_GetValue(dbres, 0, 9) ? Db_GetValue(dbres, 0, 9) : "back_top.gif");
	strcpy(gcha_theme_back_blogtop_icon, Db_GetValue(dbres, 0, 10) ? Db_GetValue(dbres, 0, 10) : "back_blogtop.gif");
	Db_CloseDyna(dbres);

	/* RSSリーダーにRSSを認識させるための埋め込み */
	Put_Format_Nlist(nlp_out, "PATHRSS", "%s%s%s/%08d.rdf", g_cha_protocol, cha_rss, g_cha_rss_location, in_blog);
	Put_Nlist(nlp_out, "BACK_TOP", gcha_theme_back_top_icon);
	Put_Nlist(nlp_out, "BACK_BLOGTOP", gcha_theme_back_blogtop_icon);
	/* メイン画面の組み立て */
	if (build_main(db, nlp_in, nlp_out, in_blog)) {
		return 1;
	}
	// お気に入りに追加か削除かを設定
	if (g_in_dbb_mode && in_blog != g_in_login_blog) {
		Put_Nlist(nlp_out, "FAV_START", "-->");
		Put_Nlist(nlp_out, "FAV_END", "<!--");
		in_ret = Is_Favorite_Blog(db, nlp_out, in_blog, g_in_login_blog);
		if (in_ret < 0) {
			return 1;
		}
		if (g_in_dbb_mode) {
			Put_Format_Nlist(nlp_out, "FAVORITE", "%s%s%s/%s/?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
		} else if (g_in_short_name) {
			Put_Format_Nlist(nlp_out, "FAVORITE", "%s%s%s/%08d/?", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
		} else {
			Put_Format_Nlist(nlp_out, "FAVORITE", "%s/%s?bid=%d&", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
		}
		if (in_ret) {
			Put_Nlist(nlp_out, "FAVORITE", "DEL_FAVORITE.x=1");
			Put_Nlist(nlp_out, "FAVGIF", "btn_blog_fav-delete.gif");
		} else {
			Put_Nlist(nlp_out, "FAVORITE", "ADD_FAVORITE.x=1");
			Put_Nlist(nlp_out, "FAVGIF", "btn_blog_favorite.gif");
		}
	}
	/* 月別アーカイブ */
	if(Get_Nlist(nlp_in, "month", 1)) {
		Page_Out(nlp_out, CO_SKEL_ARTICLE_LIST);
	/* 日別アーカイブ */
	} else if(Get_Nlist(nlp_in, "date", 1)) {
		Page_Out(nlp_out, CO_SKEL_ARTICLE_LIST);
	/* カテゴリ別アーカイブ */
	} else if(Get_Nlist(nlp_in, "cid", 1)) {
		Page_Out(nlp_out, CO_SKEL_ARTICLE_LIST);
	/* 単記事ページ */
	} else if(Get_Nlist(nlp_in, "eid", 1) || Get_Nlist(nlp_in, "kid", 1)) {
		Page_Out(nlp_out, CO_SKEL_ARTICLE);
	/* 検索結果 */
	} else if(Get_Nlist(nlp_in, "BTN_BLOG_SEARCH", 1)) {
		Page_Out(nlp_out, CO_SKEL_SEARCH_RESULT);
	/* 訪問者リスト */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_VISITOR_LIST", 1)) {
		if (in_blog == g_in_login_blog) {
			Page_Out(nlp_out, CO_SKEL_VISITOR_LIST);
		} else {
			page_out_usertop(db, nlp_out, in_blog);
		}
	/* カレンダー */
	} else if(Get_Nlist(nlp_in, "calendar", 1)) {
		page_out_usertop(db, nlp_out, in_blog);
	/* ユーザトップ */
	} else {
		page_out_usertop(db, nlp_out, in_blog);
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	post_comment()
 * Description:
 *	コメント投稿処理。
%* ------------------------------------------------------------------------
 * Return:
 *	正常終了 0
 *	エラー時 1（入力エラー), CO_ERROR（予期しないエラー)
-* ------------------------------------------------------------------------*/
int post_comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_eid;
	char *chp_escape;
	char *chp_author_name;
	char *chp_author_mail;
	char *chp_author_url;
	char *chp_author_ip;
	char *chp_comment;
	char *chp_tmp;
	char cha_sql[(CO_MAXLEN_COMMENT_AUTHOR_NAME
		      + CO_MAXLEN_COMMENT_AUTHOR_MAIL
		      + CO_MAXLEN_COMMENT_AUTHOR_URL
		      + CO_MAXLEN_COMMENT
		      + 256) * 6];
	char cha_error[2048];
	char cha_subject[1024];
	char cha_system[256];
	char cha_sender[256];
	char cha_author_url[256];
	int in_match;
	int in_accept;
	int in_comment_id;
	int in_error = 0;

	chp_eid = Get_Nlist(nlp_in, "eid", 1);
	if(!chp_eid || Check_Numeric(chp_eid)) {
		Put_Nlist(nlp_out, "ERROR", "記事の指定に誤りがあります。");
	}
	if(Get_Flag_Public(db, nlp_out, atoi(chp_eid), in_blog) != 1) {
		Put_Nlist(nlp_out, "ERROR", "記事が存在しません。");
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_eid);
	if(Get_Flag_Acceptable_Comment(db, nlp_out, atoi(chp_eid), in_blog, g_in_login_blog) == 0) {
		Put_Nlist(nlp_out, "ERROR", "この記事はコメントを受け付けていません。");
		return 1;
	}
	if (g_in_need_login) {
		if (comment_owner_check(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	} else {
		if (comment_ip_check(db, nlp_in, nlp_out, in_blog)) {
			return 1;
		}
	}
	/* 名前 */
	chp_author_ip = "0";
	chp_author_name = Get_Nlist(nlp_in, "comment_author_name", 1);
	if (g_in_need_login) {
		chp_tmp = strchr(chp_author_name, ':');
		if (chp_tmp) {
			*chp_tmp = '\0';
			chp_author_ip = chp_author_name;
			++chp_tmp;
			chp_author_name = chp_tmp;
		}
	}
	if(!chp_author_name || !*chp_author_name || Check_Space_Only(chp_author_name)) {
		Put_Nlist(nlp_out, "ERROR", "名前を入力してください。<br>");
		in_error++;
	} else if(strlen(chp_author_name) > CO_MAXLEN_COMMENT_AUTHOR_NAME){
		Put_Format_Nlist(nlp_out, "ERROR", "名前は半角%d文字（全角%d文字）以内で入力してください。<br>"
			, CO_MAXLEN_COMMENT_AUTHOR_NAME, CO_MAXLEN_COMMENT_AUTHOR_NAME / 2 );
		in_error++;
	}
	/* メールアドレス */
	chp_author_mail = Get_Nlist(nlp_in, "comment_author_mail", 1);
	if(chp_author_mail && *chp_author_mail) {
		if(Check_Mail(chp_author_mail)) {
			Put_Nlist(nlp_out, "ERROR", "メールアドレスに誤りがあります。<br>");
			in_error++;
		}
		if(strlen(chp_author_mail) > CO_MAXLEN_COMMENT_AUTHOR_MAIL) {
			Put_Format_Nlist(nlp_out, "ERROR", "メールアドレスは半角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_COMMENT_AUTHOR_MAIL);
			in_error++;
		}
	}
	/* URL */
	chp_author_url = Get_Nlist(nlp_in, "comment_author_url", 1);
	if(chp_author_url && *chp_author_url) {
		if(Check_URL(chp_author_url)){
			Put_Nlist(nlp_out, "ERROR", "URLに誤りがあります。<br>");
			in_error++;
		}
		if(strlen(chp_author_url) > CO_MAXLEN_COMMENT_AUTHOR_URL) {
			Put_Format_Nlist(nlp_out, "ERROR", "URLは半角%d文字以内で入力してください。<br>"
				, CO_MAXLEN_COMMENT_AUTHOR_URL);
			in_error++;
		}
	} else if (g_in_need_login && g_in_dbb_mode && g_in_login_blog > 0 && g_in_login_blog != INT_MAX) {
		if (Blog_To_Temp(db, nlp_out, g_in_login_blog, cha_sql)) {
			in_error++;
		} else {
			sprintf(cha_author_url, "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, cha_sql);
			chp_author_url = cha_author_url;
		}
	}
	chp_comment = Get_Nlist(nlp_in, "comment", 1);
	if(!chp_comment || strlen(chp_comment) == 0 || Check_Space_Only(chp_comment)) {
		Put_Nlist(nlp_out, "ERROR", "コメントを入力してください。<br>");
		in_error++;
	} else if(strlen(chp_comment) > CO_MAXLEN_COMMENT) {
		Put_Format_Nlist(nlp_out, "ERROR", "コメントは半角%d文字（全角%d文字）以内で入力してください。<br>"
			, CO_MAXLEN_COMMENT, CO_MAXLEN_COMMENT/2);
		in_error++;
	}
	if(in_error) {
		return 1;
	}

	sprintf(cha_sql, "select b_comment from at_entry T1 where T1.n_blog_id = %d and n_entry_id = %s", in_blog, chp_eid);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
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
		Put_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	sprintf(cha_sql, "select coalesce(max(T1.n_comment_id), 0) + 1 from at_comment T1 where T1.n_blog_id = %d for update", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return CO_ERROR;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		in_comment_id = atoi(chp_tmp);
	} else {
		in_comment_id = 1;
	}
	Db_CloseDyna(dbres);

	/* セレクトインサートで重複をできるだけ防ぐ */
	strcpy(cha_sql, "insert into at_comment");
	strcat(cha_sql, "(n_blog_id");
	strcat(cha_sql, ",n_comment_id");
	strcat(cha_sql, ",n_entry_id");
	strcat(cha_sql, ",c_comment_author");
	strcat(cha_sql, ",c_comment_ip");
	strcat(cha_sql, ",c_comment_mail");
	strcat(cha_sql, ",c_comment_url");
	strcat(cha_sql, ",c_comment_body");
	strcat(cha_sql, ",d_comment_create_time");
	if (in_accept == 2) {
		strcat(cha_sql, ",b_comment_accept");
	}
	sprintf(cha_sql + strlen(cha_sql), ") values (%d, %d", in_blog, in_comment_id);	/* 0 コメントID */
	strcat(cha_sql, ", ");
	strcat(cha_sql, chp_eid);		/* 1 エントリID */
	strcat(cha_sql, ", '");
	chp_escape = My_Escape_SqlString(db, chp_author_name);
	strcat(cha_sql, chp_escape);
	free(chp_escape);				/* 2 名前 */
	strcat(cha_sql, "', '");
	if (!g_in_need_login) {
		chp_author_ip = getenv("REMOTE_ADDR");
	}
	if(chp_author_ip && *chp_author_ip) {
		strcat(cha_sql, chp_author_ip);			/* 3 ip or owner */
	}
	strcat(cha_sql, "', '");
	if(chp_author_mail && *chp_author_mail) {
		chp_escape = My_Escape_SqlString(db, chp_author_mail);
		strcat(cha_sql, chp_escape);			/* 4 e-mail */
		free(chp_escape);
	}
	strcat(cha_sql, "', '");
	if(chp_author_url && *chp_author_url) {
		chp_escape = My_Escape_SqlString(db, chp_author_url);
		strcat(cha_sql, chp_escape);			/* 5 url */
		free(chp_escape);
	}
	strcat(cha_sql, "', '");
	chp_escape = My_Escape_SqlString(db, chp_comment);
	strcat(cha_sql, chp_escape);			/* 6 comment */
	free(chp_escape);
	strcat(cha_sql, "', now()");
	if (in_accept == 2) {
		strcat(cha_sql, ", 0");
	}
	strcat(cha_sql, ")");
	if(Db_ExecSql(db, cha_sql)) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	in_match = 0;
	sprintf(cha_sql, "select c_filter from at_comment_filter where b_valid != 0 and n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(dbres) {
		chp_tmp = Db_GetValue(dbres, 0, 0);
		if (chp_tmp && chp_tmp[0]) {
			in_match = Match_Filter(chp_comment, chp_tmp);
		}
		Db_CloseDyna(dbres);
	}
	if (in_match) {
		sprintf(cha_sql, "update at_comment set b_comment_accept = 0, b_comment_filter = 1 where n_blog_id = %d and n_entry_id = %s and n_comment_id = %d", in_blog, chp_eid, in_comment_id);
		if(Db_ExecSql(db, cha_sql)) {
			Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
			Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
			Rollback_Transact(db);
			return 1;
		}
	}
	if(Commit_Transact(db)) {
		Put_Nlist(nlp_out, "ERROR", "コミットに失敗しました。");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
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
		chp_author_mail = Db_GetValue(dbres, 0, 0);
		if (chp_author_mail) {
			char *chp_server;
			chp_server = getenv("SERVER_NAME");
			if(chp_server && *chp_server) {
				sprintf(cha_subject, "[%s]コメントが追加されました", cha_system);
				if (g_in_dbb_mode) {
					sprintf(cha_error, "コメントが追加されました。下記URLで確認できます。\n\n%s%s%s/%s/?eid=%s#c%d\n"
						, g_cha_protocol, chp_server, g_cha_base_location, g_cha_blog_temp, chp_eid, in_comment_id);
				} else if (g_in_short_name) {
					sprintf(cha_error, "コメントが追加されました。下記URLで確認できます。\n\n%s%s%s/%08d/?eid=%s#c%d\n"
						, g_cha_protocol, chp_server, g_cha_base_location, in_blog, chp_eid, in_comment_id);
				} else {
					sprintf(cha_error, "コメントが追加されました。下記URLで確認できます。\n\n%s%s%s/%s?bid=%d&eid=%s#c%d\n"
						, g_cha_protocol, chp_server, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog, chp_eid, in_comment_id);
				}
				Send_Mail(cha_sender, chp_author_mail, cha_subject, cha_error);
			}
		}
		Db_CloseDyna(dbres);
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            disp_page_attachfile()
 * Description:
 *      記事の画像を表示する
%* ------------------------------------------------------------------------
 * Return:                      正常終了 0
 *      戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int disp_page_attach_file(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry, int in_upload)
{
	DBRes *dbres;
	char cha_sql[4096];
	char* chp_upload;

	if (!in_entry) {
		chp_upload = Get_Nlist(nlp_in, "eid", 1);
		if (!chp_upload) {
			printf("Content-Type: text/plain\n\nno image\n");
			return 0;
		}
		in_entry = atoi(chp_upload);
	}
	if (!in_upload) {
		chp_upload = Get_Nlist(nlp_in, "uid", 1);
		if (!chp_upload) {
			printf("Content-Type: text/plain\n\nno image\n");
			return 0;
		}
		in_upload = atoi(chp_upload);
	}
	sprintf(cha_sql, "select c_fileimage,c_filetype,n_filesize from at_uploadfile where n_blog_id = %d and n_entry_id = %d and n_uploadfile_id = %d;", in_blog, in_entry, in_upload);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		printf("Content-Type: text/plain\n\nno image\n");
		return 0;
	}
	if(Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		printf("Content-Type: text/plain\n\nno image\n");
		return 0;
	}
	if (!Db_FetchValue(dbres, 0) || !Db_FetchValue(dbres, 1) || !Db_FetchValue(dbres, 2)) {
		Db_CloseDyna(dbres);
		printf("Content-Type: text/plain\n\nno image\n");
		return 0;
	}
	fprintf(stdout, "Content-Type: %s\n\n", Db_FetchValue(dbres, 1));
	fwrite(Db_FetchValue(dbres, 0), atoi(Db_FetchValue(dbres, 2)), 1, stdout);
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:            disp_profile_image()
 * Description:
 *      プロフィールの画像を表示する
%* ------------------------------------------------------------------------
 * Return:                      正常終了 0
 *      戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int disp_page_author_image(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char cha_sql[4096];

	sprintf(cha_sql, "select c_author_image,c_image_type,n_image_size from at_profile where n_blog_id=%d;", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		printf("Content-Type: text/plain\n\nno image\n");
		return 0;
	}
	if(Db_FetchNext(dbres) != CO_SQL_OK) {
		Db_CloseDyna(dbres);
		printf("Content-Type: text/plain\n\nno image\n");
		return 0;
	}
	if (!Db_FetchValue(dbres, 0) || !Db_FetchValue(dbres, 1) || !Db_FetchValue(dbres, 2)) {
		Db_CloseDyna(dbres);
		printf("Content-Type: text/plain\n\nno image\n");
		return 0;
	}

	fprintf(stdout, "Content-Type: %s\n\n", Db_FetchValue(dbres, 1));
	fwrite(Db_FetchValue(dbres, 0), atoi(Db_FetchValue(dbres, 2)), 1, stdout);
	Db_CloseDyna(dbres);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	post_comment_ask()
 * Description:
 *	コメントの最終確認
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
void post_comment_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp ? chp_tmp : "");

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_name", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_name", chp_escape ? chp_escape : "");
	if (chp_escape) {
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_mail", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_mail", chp_escape ? chp_escape : "");
	if (chp_escape) {
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_url", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_url", chp_escape ? chp_escape : "");
	if (chp_escape) {
		free(chp_escape);
	}

	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_Hidden(nlp_out, "HIDDEN", "comment", chp_escape ? chp_escape : "");
		free(chp_escape);
		chp_tmp2 = Conv_Long_Ascii(chp_tmp, 72);
		chp_escape = Escape_HtmlString(chp_tmp2);
		chp_tmp = Conv_Br(chp_escape);
		Put_Nlist(nlp_out, "MSG", chp_tmp);
		free(chp_tmp);
		free(chp_escape);
		free(chp_tmp2);
	}

	Page_Out(nlp_out, CO_SKEL_POST_COMMENT_ASK);
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	post_review_ask()
 * Description:
 *	レビューの最終確認
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
void post_review_ask(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	char *chp_tmp;
	char *chp_escape;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp ? chp_tmp : "");

	chp_tmp = Get_Nlist(nlp_in, "tid", 1);
	Build_HiddenEncode(nlp_out, "HIDDEN", "tid", chp_tmp ? chp_tmp : "");
	Build_HiddenEncode(nlp_out, "HIDDEN", "ITEMID", chp_tmp ? chp_tmp : "");

	chp_tmp = Get_Nlist(nlp_in, "disp_name", 1);
	Build_HiddenEncode(nlp_out, "HIDDEN", "disp_name", chp_tmp ? chp_tmp : "");

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "review_author_name", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "review_author_name", chp_escape ? chp_escape : "");
	if (chp_escape) {
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "review_author_title", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "review_author_title", chp_escape ? chp_escape : "");
	if (chp_escape) {
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "review_author_point", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "review_author_point", chp_escape ? chp_escape : "");
	if (chp_escape) {
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "review", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "review", chp_escape ? chp_escape : "");
	if (chp_escape) {
		chp_tmp = Conv_Br(chp_escape);
		Put_Nlist(nlp_out, "MSG", chp_tmp);
		free(chp_tmp);
		free(chp_escape);
	}

	Page_Out(nlp_out, CO_SKEL_POST_REVIEW_ASK);
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	update_stsyle_sheet()
 * Description:
 *	スタイルシートファイルを更新する
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 1
-* ------------------------------------------------------------------------*/
int update_style_sheet(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	FILE *fp;
	char *chp_tmp;
	char *chp_css;
	char cha_css[1024];
	char cha_sql[1024];
	int fd;
	int in_skin_no = 0;

	chp_tmp = Get_Nlist(nlp_in, "SrcCode", 1);
	if (!chp_tmp || !chp_tmp[0]) {
		return 0;
	}
	/* スタイルシート */
	strcpy(cha_sql, "select T1.n_theme_id");	/* 0 テンプレートナンバー */
	strcat(cha_sql, ", T1.c_user_css");	/* 1 ユーザー指定CSS */
	strcat(cha_sql, " from at_looks T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp) {
		in_skin_no = atoi(chp_tmp);
	}
	cha_css[0] = '\0';
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if(chp_tmp) {
		strcpy(cha_css, chp_tmp);
	}
	Db_CloseDyna(dbres);

	if (in_skin_no) {
		sprintf(cha_css, "%s/temp.XXXXXX", g_cha_css_path);
		fd = mkstemp(cha_css);
		fp = fdopen(fd, "r+");
		chp_css = strrchr(cha_css, '/');
		if (chp_css) {
			++chp_css;
		} else {
			chp_css = cha_css;
		}
	} else {
		chp_css = strrchr(cha_css, '/');
		if (chp_css) {
			++chp_css;
		} else {
			chp_css = cha_css;
		}
		sprintf(cha_css, "%s/%s", g_cha_css_path, chp_css);
		fp = fopen(cha_css, "w+");
	}

	if (!fp) {
		Put_Nlist(nlp_out, "ERROR", "CSSファイルのオープンに失敗しました。");
		return 1;
	}
	chp_tmp = Get_Nlist(nlp_in, "SrcCode", 1);
	fwrite(chp_tmp, strlen(chp_tmp), 1, fp);
	fclose(fp);
	chmod(cha_css, 0644);

	if (in_skin_no) {
		if(Begin_Transact(db)) {
			Put_Format_Nlist(nlp_out, "ERROR", "トランザクション開始に失敗しました。(%s)", Gcha_last_error);
			return 1;
		}
		sprintf(cha_sql, "update at_looks T1 set T1.n_theme_id = 0,T1.c_user_css = '%s/%s' where T1.n_blog_id = %d;", g_cha_css_location, chp_css, in_blog);
		if (Db_ExecSql(db, cha_sql) != CO_SQL_OK) {
			Put_Format_Nlist(nlp_out, "ERROR", "ユーザーのCSSを設定するクエリに失敗しました。(%s)", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
		if(Commit_Transact(db)) {
			Put_Format_Nlist(nlp_out, "ERROR", "コミットに失敗しました。(%s)", Gcha_last_error);
			Rollback_Transact(db);
			return 1;
		}
	}

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	get_blog_status()
 * Description:
 *	スタイルシートファイルを更新する
%* ------------------------------------------------------------------------
 * Return:			正常終了 ブログ状態
 *	戻り値          エラー時 0
-* ------------------------------------------------------------------------*/
int get_blog_status(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[1024];
	int in_status = 0;

	strcpy(cha_sql, "select T1.n_blog_status");	/* 0 ステータス */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 0;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(chp_tmp) {
		in_status = atoi(chp_tmp);
	}
	Db_CloseDyna(dbres);

	if (!in_status) {
		Put_Nlist(nlp_out, "ERROR", "ブログが使用できません。<br>");
	}

	return in_status;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	regist_new_customer()
 * Description:
 *	新規顧客を追加する
%* ------------------------------------------------------------------------
 * Return:			正常終了 ブログ状態
 *	戻り値          エラー時 0
-* ------------------------------------------------------------------------*/
int regist_new_customer(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	int in_error;
	char *chp_val1;
	char *chp_val2;
	char *chp_mail;
	char *chp_id;
	char *chp_pass;

	in_error = 0;
	chp_val1 = Get_Nlist(nlp_in, "btn_mail", 1);
	if (!chp_val1 || !chp_val1[0]) {
		Put_Nlist(nlp_out, "ERROR", "メールアドレスが入力されていません。<br>");
		++in_error;
	}
	chp_val2 = Get_Nlist(nlp_in, "btn_mail_rtp", 1);
	if (!chp_val2 || !chp_val2[0]) {
		Put_Nlist(nlp_out, "ERROR", "メールアドレス(確認用)が入力されていません。<br>");
		++in_error;
	}
	if (chp_val1 && chp_val1[0] && chp_val2 && chp_val2[0]) {
		if (strcmp(chp_val1, chp_val2) != 0) {
			Put_Nlist(nlp_out, "ERROR", "メールアドレスが一致しません。<br>");
			++in_error;
		} else if (Check_Mail(chp_val1)) {
			Put_Nlist(nlp_out, "ERROR", "メールアドレスが正しくありません。<br>");
			++in_error;
		}
	}
	chp_mail = chp_val1;
	chp_val1 = Get_Nlist(nlp_in, "btn_id", 1);
	if (!chp_val1 || !chp_val1[0]) {
		Put_Nlist(nlp_out, "ERROR", "ログインIDが入力されていません。<br>");
		++in_error;
	} else if (Check_Code(chp_val1)) {
		Put_Nlist(nlp_out, "ERROR", "ログインIDが正しくありません。<br>");
		++in_error;
	}
	chp_id = chp_val1;
	chp_val1 = Get_Nlist(nlp_in, "btn_pass", 1);
	if (!chp_val1 || !chp_val1[0]) {
		Put_Nlist(nlp_out, "ERROR", "パスワードが入力されていません。<br>");
		++in_error;
	}
	chp_val2 = Get_Nlist(nlp_in, "btn_pass_rtp", 1);
	if (!chp_val2 || !chp_val2[0]) {
		Put_Nlist(nlp_out, "ERROR", "パスワード(確認用)が入力されていません。<br>");
		++in_error;
	}
	if (chp_val1 && chp_val1[0] && chp_val2 && chp_val2[0]) {
		if (strcmp(chp_val1, chp_val2) != 0) {
			Put_Nlist(nlp_out, "ERROR", "パスワードが一致しません。<br>");
			++in_error;
		} else if (Check_Pass(chp_val1)) {
			Put_Nlist(nlp_out, "ERROR", "パスワードが正しくありません。<br>");
			++in_error;
		}
	}
	chp_pass = chp_val1;
	if (!in_error) {
		in_error = Regist_New_Customer(db, nlp_in, nlp_out, chp_mail, chp_id, chp_pass, in_blog);
	}
	return in_error;
}

/*
+* ------------------------------------------------------------------------
 * Function:		add_faivorite_blog()
 * Description:
 *	現在表示中のブログをお気に入りに追加する。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 <>0
-* ------------------------------------------------------------------------*/
int add_faivorite_blog(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_sql[1024];

	if (Add_Favorite_Blog(db, nlp_out, in_blog, g_in_login_blog)) {
		return 1;
	}

	sprintf(cha_sql, "select c_blog_title from at_blog where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp || !chp_tmp[0]) {
		Put_Nlist(nlp_out, "MESSAGE", "このブログをお気に入りリストに追加しました。");
	} else {
		Put_Format_Nlist(nlp_out, "MESSAGE", "「%s」をお気に入りリストに追加しました。", chp_tmp);
	}
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_name", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_name", chp_escape ? chp_escape : "");
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_mail", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_mail", chp_escape ? chp_escape : "");
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_url", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_url", chp_escape ? chp_escape : "");
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment", chp_escape ? chp_escape : "");
		chp_tmp2 = Conv_Long_Ascii(chp_escape, 72);
		free(chp_escape);
		chp_tmp = Conv_Br(chp_tmp2);
		free(chp_tmp2);
		Put_Nlist(nlp_out, "MSG", chp_tmp);
		free(chp_tmp);
	}

	if (g_in_dbb_mode) {
		Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
	} else if (g_in_short_name) {
		Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
	} else {
		Put_Format_Nlist(nlp_out, "RETURN", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
	}
	Page_Out(nlp_out, CO_SKEL_CONFIRM);

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		del_faivorite_blog()
 * Description:
 *	現在表示中のブログをお気に入りに追加する。
%* ------------------------------------------------------------------------
 * Return:			正常終了 0
 *	戻り値          エラー時 <>0
-* ------------------------------------------------------------------------*/
int del_faivorite_blog(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	char *chp_tmp;
	char *chp_tmp2;
	char *chp_escape;
	char cha_sql[1024];

	if (Del_Favorite_Blog(db, nlp_out, in_blog, g_in_login_blog)) {
		return 1;
	}

	sprintf(cha_sql, "select c_blog_title from at_blog where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "クエリーに失敗しました。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "ブログタイトルを取得できません。<br>");
		Put_Format_Nlist(nlp_out, "QUERY", "%d:%s<br>", __LINE__, Gcha_last_error);
		Db_CloseDyna(dbres);
		return 1;
	}
	if (!chp_tmp || !chp_tmp[0]) {
		Put_Nlist(nlp_out, "MESSAGE", "このブログをお気に入りリストからはずしました。");
	} else {
		Put_Format_Nlist(nlp_out, "MESSAGE", "「%s」をお気に入りリストからはずしました。", chp_tmp);
	}
	Db_CloseDyna(dbres);

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	if (chp_tmp) {
		Build_HiddenEncode(nlp_out, "HIDDEN", "eid", chp_tmp);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_name", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_name", chp_escape ? chp_escape : "");
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_mail", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_mail", chp_escape ? chp_escape : "");
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment_author_url", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment_author_url", chp_escape ? chp_escape : "");
		free(chp_escape);
	}

	chp_escape = NULL;
	chp_tmp = Get_Nlist(nlp_in, "comment", 1);
	if (chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "comment", chp_escape ? chp_escape : "");
		chp_tmp2 = Conv_Long_Ascii(chp_escape, 72);
		free(chp_escape);
		chp_tmp = Conv_Br(chp_tmp2);
		free(chp_tmp2);
		Put_Nlist(nlp_out, "MSG", chp_tmp);
		free(chp_tmp);
	}
	if (g_in_dbb_mode) {
		Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
	} else if (g_in_short_name) {
		Put_Format_Nlist(nlp_out, "RETURN", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
	} else {
		Put_Format_Nlist(nlp_out, "RETURN", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
	}

	Page_Out(nlp_out, CO_SKEL_CONFIRM);

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
int main()
{
	NLIST *nlp_in;
	NLIST *nlp_out;
	NLIST *nlp_cookie;
	DBase *db;
	int in_error;
	int in_blog;
	int in_owner;
	int in_entry;
	int in_upload;
	int in_status;
	int in_ret;
	char cha_host[CO_MAX_HOST];
	char cha_db[CO_MAX_DB];
	char cha_username[CO_MAX_USER];
	char cha_password[CO_MAX_PASS];
	char *chp_tmp;
	char *chp_blog;
	char *chpa_esc[] = {
		"BTN_COMMENT.x", "BTN_COMMENT.y",
		"BTN_RETURN_COMMENT.x", "BTN_RETURN_COMMENT.y",
		"BTN_BLOG_SEARCH", "BTN_ADMIN_LOGIN",
		"BTN_CART_LOGIN",
		"visitor_list", "visitor_return",
		"bid", "eid", "cid", NULL
	};

	in_error = Read_Blog_Ini(cha_host,cha_db, cha_username, cha_password, &g_in_ssl_mode);
	if (in_error) {
		printf("\n\nINIファイルの読み込みに失敗しました。(%s)", strerror(in_error));
		return in_error;
	}
	if (g_in_ssl_mode) {
		db = Db_ConnectSSL(cha_host, cha_db, cha_username, cha_password);
	} else {
		db = Db_ConnectWithParam(cha_host, cha_db, cha_username, cha_password);
	}
	if(!db){
		printf("\n\nデータベース接続に失敗しました。(%s - %s - %s - %s - %s)", cha_host, cha_db, cha_username, cha_password, Gcha_last_error);
		return 1;
	}
	if (Read_Blog_Option(db)) {
		printf("\n\nブログオプションの読み込みに失敗しました。");
		Db_Disconnect(db);
		return 1;
	}
	Set_SkelPath(g_cha_user_skeleton);

	nlp_in = Init_Cgi();
	nlp_out = Init_Nlist();
	nlp_cookie = Get_Cookie();

	Put_Nlist(nlp_out, "CSS", g_cha_css_location);
	Put_Nlist(nlp_out, "SCRIPTS", g_cha_script_location);
	Put_Nlist(nlp_out, "IMAGES", g_cha_user_image);
	Put_Nlist(nlp_out, "THEME", g_cha_theme_image);

	if (g_in_hb_mode) {
		Delete_Old_Entry(db);
	}

	in_owner = 0;
	in_error = 0;
	in_entry = 0;
	in_upload = 0;
	if (Get_Nlist(nlp_in, "visitor_return", 0)) {
		chp_blog = Get_Nlist(nlp_in, "visitor_return", 1);
	} else {
		chp_blog = Get_Nlist(nlp_in, "bid", 1);
	}
	if (!chp_blog) {
		chp_blog = getenv("PATH_INFO");
		if (!chp_blog) {
			if (g_in_dbb_mode) {
				Put_Nlist(nlp_out, "ERROR", "ブログIDを取得できません。<br>");
				in_error = __LINE__;
				goto clear_finish;
			} else {
				in_blog = Check_Unix_User(db, nlp_out);
				if (in_blog < 0) {
					in_error = __LINE__;
					goto clear_finish;
				} else if (!in_blog) {
					Put_Nlist(nlp_out, "ERROR", "ブログIDを取得できません。<br>");
					in_error = __LINE__;
					goto clear_finish;
				}
			}
		} else if (g_in_dbb_mode) {
			if (sscanf(chp_blog, "/%s-%d-%d", g_cha_blog_temp, &in_entry, &in_upload) < 2 && sscanf(chp_blog, "/%s", g_cha_blog_temp) < 1) {
				Put_Nlist(nlp_out, "ERROR", "ブログIDを取得できません。<br>");
				in_error = __LINE__;
				goto clear_finish;
			}
			if (strlen(g_cha_blog_temp) == 12) {
				in_blog = Temp_To_Blog(db, nlp_out, g_cha_blog_temp);
			} else {
				in_blog = atoi(g_cha_blog_temp);
				Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp);
			}
			if (in_blog < 0) {
				in_error = __LINE__;
				goto clear_finish;
			}
			in_owner = Temp_To_Owner(db, nlp_out, g_cha_blog_temp);
			Build_HiddenEncode(nlp_out, "HIDDEN", "bid", g_cha_blog_temp);
		} else {
			if (sscanf(chp_blog, "/%d-%d-%d", &in_blog, &in_entry, &in_upload) < 2 && sscanf(chp_blog, "/%s", g_cha_blog_temp) < 1) {
				Put_Nlist(nlp_out, "ERROR", "ブログIDを取得できません。<br>");
				in_error = __LINE__;
				goto clear_finish;
			}
			Put_Format_Nlist(nlp_out, "HIDDEN", "<input type=\"hidde\' name=\"bid\" value=\"%d\">\n", in_blog);
		}
	} else if (g_in_dbb_mode) {
		if (strlen(chp_blog) == 12) {
			in_blog = Temp_To_Blog(db, nlp_out, chp_blog);
			strcpy(g_cha_blog_temp, chp_blog);
		} else {
			in_blog = atoi(chp_blog);
			Blog_To_Temp(db, nlp_out, in_blog, g_cha_blog_temp);
		}
		if (in_blog < 0) {
			in_error = __LINE__;
			goto clear_finish;
		}
		in_owner = Temp_To_Owner(db, nlp_out, g_cha_blog_temp);
		Build_HiddenEncode(nlp_out, "HIDDEN", "bid", chp_blog);
	} else {
		in_blog = atoi(chp_blog);
		Build_HiddenEncode(nlp_out, "HIDDEN", "bid", chp_blog);
	}
	if (g_in_dbb_mode) {
		Put_Format_Nlist(nlp_out, "BLOGTOP", "%s%s%s/%s/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, g_cha_blog_temp);
	} else if (g_in_short_name) {
		Put_Format_Nlist(nlp_out, "BLOGTOP", "%s%s%s/%08d/", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog);
	} else {
		Put_Format_Nlist(nlp_out, "BLOGTOP", "%s/%s?bid=%d", g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);
	}

	g_in_login_blog = 0;
	if (g_in_hosting_mode) {
		DBRes *dbres;
		char cha_sql[1024];
		chp_tmp = Get_Nlist(nlp_cookie, "as_blog_remote_user", 1);
		if (chp_tmp) {
			sprintf(cha_sql, "select n_blog_id from at_profile where c_access_key = '%s'", chp_tmp);
			dbres = Db_OpenDyna(db, cha_sql);
			if (dbres) {
				chp_tmp = Db_GetValue(dbres, 0, 0);
				g_in_login_blog = (chp_tmp ? atoi(chp_tmp) : 0);
				Db_CloseDyna(dbres);
			}
		}
	} else {
		g_in_login_blog = Check_Login(db, nlp_out, &g_in_login_owner, &g_in_admin_mode);
	}
	if (g_in_dbb_mode) {
		in_ret = Check_Retire(db, nlp_out, in_blog);
		if (in_ret < 0) {
			in_error = __LINE__;
			goto clear_finish;
		} else if (in_ret) {
			Put_Nlist(nlp_out, "ERROR", "このブログは閉鎖されました。<br>");
			in_error = __LINE__;
			goto clear_finish;
		}
	}
	g_in_need_login = Need_Login(db, in_blog);
	//g_in_need_login = 0;
	if (g_in_need_login) {
		if (g_in_login_blog < 0) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Page_Out(nlp_out, CO_SKEL_LOGIN);
			goto clear_finish;
		} else if (!g_in_login_blog) {
			if (Get_Nlist(nlp_in, CO_DISP_REMIND, 1)) {
				Disp_Remind_Page(db, nlp_in, nlp_out, CO_CGI_BUILD_HTML);
				Page_Out(nlp_out, CO_SKEL_REMIND);
				goto clear_finish;
			} else if (Get_Nlist(nlp_in, CO_SEND_REMIND, 1)) {
				if (Auth_Reminder(db, nlp_in, nlp_out, CO_CGI_BUILD_HTML)) {
					Put_Nlist(nlp_out, "ERR_START", "-->");
					Put_Nlist(nlp_out, "ERR_END", "<!--");
					Disp_Remind_Page(db, nlp_in, nlp_out, CO_CGI_BUILD_HTML);
					Page_Out(nlp_out, CO_SKEL_REMIND);
					goto clear_finish;
				} else {
					Disp_Remind_Finish(nlp_in, nlp_out, CO_CGI_BUILD_HTML);
					Page_Out(nlp_out, CO_SKEL_REMIND_FINISH);
					goto clear_finish;
				}
			} else if (Get_Nlist(nlp_in, CO_LOGIN_ID, 1) && Get_Nlist(nlp_in, CO_PASSWORD, 1)) {
				g_in_login_blog = Auth_Login(db, nlp_in, nlp_out, CO_LOGIN_ID, CO_PASSWORD, &g_in_login_owner);
				if (g_in_login_blog < 0) {
					Put_Nlist(nlp_out, "ERR_START", "-->");
					Put_Nlist(nlp_out, "ERR_END", "<!--");
					Page_Out(nlp_out, CO_SKEL_LOGIN);
					goto clear_finish;
				} else if (!g_in_login_blog) {
					Put_Nlist(nlp_out, "ERROR", "ログインIDまたはパスワードが間違っています。<br>");
					Put_Nlist(nlp_out, "ERR_START", "-->");
					Put_Nlist(nlp_out, "ERR_END", "<!--");
					Page_Out(nlp_out, CO_SKEL_LOGIN);
					goto clear_finish;
				}
			} else {
				if (g_in_dbb_mode) {
					in_error = Jump_To_Login_Cgi(db, nlp_out);
					if (in_error) {
						in_error = __LINE__;
					}
				} else {
					Disp_Login_Page(nlp_in, nlp_out, CO_CGI_BUILD_HTML);
					Page_Out(nlp_out, CO_SKEL_LOGIN);
				}
				goto clear_finish;
			}
		}
	}
	if (!g_in_admin_mode && (!g_in_need_login || in_blog != g_in_login_blog)) {
		update_access_conter(db, in_blog, g_in_login_blog);
	}
	g_in_black_list = (g_in_dbb_mode && In_Black_List(db, nlp_out, g_in_login_owner, NULL));
	if (g_in_dbb_mode) {
		if ((!g_in_admin_mode || g_in_login_owner != 0) && In_Black_List(db, nlp_out, in_owner, NULL)) {
			if (!Get_Nlist(nlp_in, "visitor_list", 1)) {
				Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
			} else {
				Build_HiddenEncode(nlp_out, "HIDDEN", "visitor_return", Get_Nlist(nlp_in, "visitor_list", 1));
				Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_VISITOR_LIST", "1");
			}
			Put_Nlist(nlp_out, "ERROR", "管理者により一部サービスのご利用を停止しております。<br>");
			in_error = __LINE__;
			goto clear_finish;
		}
		if (in_blog != g_in_login_blog) {
			int in_only;
			int in_friend;
			in_only = Accept_Only_Friend(db, nlp_out, in_blog);
			if (in_only < 0) {
				Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
				in_error = __LINE__;
				goto clear_finish;
			}
			in_friend = Is_Ball_Friend(db, nlp_out, in_blog, g_in_login_blog);
			if (in_friend < 0) {
				Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
				in_error = __LINE__;
				goto clear_finish;
			}
			if (in_only && !in_friend) {
				if (!Get_Nlist(nlp_in, "visitor_list", 0)) {
					Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
				} else {
					Build_HiddenEncode(nlp_out, "HIDDEN", "visitor_return", Get_Nlist(nlp_in, "visitor_list", 1));
					Build_HiddenEncode(nlp_out, "HIDDEN", "BTN_DISP_VISITOR_LIST", "1");
				}
				Put_Nlist(nlp_out, "ERROR", "球友以外の方はご覧いただけません。<br>");
				in_error = __LINE__;
				goto clear_finish;
			}
		}
	}
	if (g_in_cart_mode == CO_CART_SHOPPER && Blog_Usable(db, nlp_out, in_blog) <= 0) {
		Put_Nlist(nlp_out, "BUTTON_NAME", "\" onclick=\"window.close()");
		Put_Nlist(nlp_out, "ERROR", "ブログはお使いになれません。<br>");
		in_error = __LINE__;
		goto clear_finish;
	}
	if (g_in_cart_mode == CO_CART_RESERVE) {
		g_in_blog_priv = Get_Blog_Priv(db, nlp_out, g_in_login_owner);
		if (g_in_blog_priv < 0) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Page_Out(nlp_out, CO_SKEL_LOGIN);
			goto clear_finish;
		} else if (g_in_blog_priv) {
			g_in_login_blog = in_blog;
		}
	}
	if (g_in_hb_mode && g_in_login_owner) {
		g_in_blog_auth = Get_Blog_Auth(db, nlp_out, g_in_login_owner, in_blog);
		if (g_in_blog_auth < 0) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Page_Out(nlp_out, CO_SKEL_LOGIN);
			goto clear_finish;
		}
	}

	in_status = get_blog_status(db, nlp_out, in_blog);
	switch (in_status) {
	case 0:
		in_error = __LINE__;
		goto clear_finish;
	case CO_BLOG_STATUS_STOP:
		in_error = __LINE__;
		Put_Nlist(nlp_out, "ERROR", "このブログは停止中です。");
		goto clear_finish;
	case CO_BLOG_STATUS_CANCEL:
		in_error = __LINE__;
		Put_Nlist(nlp_out, "ERROR", "このブログは廃止されました。");
		goto clear_finish;
	}

	/* 作者画像表示 */
	if (Get_Nlist(nlp_in, "BTN_DISP_AUTHOR_IMAGE", 1)) {
		disp_page_author_image(db, nlp_in, nlp_out, in_blog);
		goto clear_finish;
	/* 添付ファイル表示 */
	} else if (Get_Nlist(nlp_in, "BTN_DISP_ATTACH_FILE", 1)) {
		disp_page_attach_file(db, nlp_in, nlp_out, in_blog, in_entry, in_upload);
		goto clear_finish;
	/* 管理者ログイン */
	} else if (Get_Nlist(nlp_in, "BTN_ADMIN_LOGIN", 1)) {
		g_in_login_blog = Auth_Login(db, nlp_in, nlp_out, "btn_id", "btn_pass", &g_in_login_owner);
		if (g_in_login_blog < 0) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			in_error = __LINE__;
		} else if (!g_in_login_blog) {
			g_in_login_blog = -1;
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_out, "ERROR", "ログインIDまたはパスワードが間違っています。<br>");
			in_error = __LINE__;
		}
	/* カートログイン */
	} else if (Get_Nlist(nlp_in, "BTN_CART_LOGIN", 1)) {
		if (Cart_Auth_Login(db, nlp_in, nlp_out, in_blog, "btn_id", "btn_pass")) {
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
		}
	/* CSSエディターからの保存ボタン */
	} else if (Get_Nlist(nlp_in, "BTN_WRITE_CSS", 1)) {
		in_error = update_style_sheet(db, nlp_in, nlp_out, in_blog);
		printf("Content-Type: text/html\n\n");
		printf("<html><body>\n");
		printf("<script language=\"JavaScript\">\n");
		printf("<!--\n");
		if (in_error) {
			printf("allert(\"%s\");\n", Get_Nlist(nlp_out, "ERROR", 1));
		}
		printf("window.close();\n");
		printf("-->\n");
		printf("</script>\n");
		printf("</body></html>\n");
		in_error = 0;
		goto clear_finish;
	/* 個別記事が表示されていて、コメント投稿ボタンが押されたときは、コメント確認 */
	} else if(Get_Nlist(nlp_in, "eid", 1) && Get_Nlist(nlp_in, "BTN_POST_COMMENT_ASK", 1)) {
		post_comment_ask(db, nlp_in, nlp_out, in_blog);
		goto clear_finish;
	/* コメント確認ページが表示されていて、書き直しボタンが押されたときは、エラーからの復帰とする */
	} else if(Get_Nlist(nlp_in, "eid", 1) && Get_Nlist(nlp_in, "BTN_RETURN_COMMENT.x", 1)) {
		Put_Nlist(nlp_in, "from_error", "1");
	/* コメント確認ページが表示されていて、コメント書き込みボタンが押されたときは、コメント投稿 */
	} else if(Get_Nlist(nlp_in, "eid", 1) && Get_Nlist(nlp_in, "BTN_COMMENT.x", 1)) {
		/* POST以外でコメントを送ることをはじく */
		chp_tmp = getenv("REQUEST_METHOD");
		if(!chp_tmp || strcasecmp(chp_tmp, "POST") != 0) {
			Put_Nlist(nlp_out, "ERROR", "CGIから直接コメントを投稿することはできません。");
			in_error = __LINE__;
			goto clear_finish;
		}
		if (post_comment(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "BTN_COMMENT.x", "1");
		}
	/* 個別記事が表示されていて、レビュー投稿ボタンが押されたときは、コメント確認 */
	} else if(Get_Nlist(nlp_in, "eid", 1) && Get_Nlist(nlp_in, "BTN_POST_REVIEW_ASK", 1)) {
		post_review_ask(db, nlp_in, nlp_out, in_blog);
		goto clear_finish;
	/* コメント確認ページが表示されていて、書き直しボタンが押されたときは、エラーからの復帰とする */
	} else if(Get_Nlist(nlp_in, "eid", 1) && Get_Nlist(nlp_in, "BTN_RETURN_REVIEW.x", 1)) {
		Put_Nlist(nlp_in, "from_error", "1");
	/* コメント確認ページが表示されていて、コメント書き込みボタンが押されたときは、コメント投稿 */
	} else if(Get_Nlist(nlp_in, "eid", 1) && Get_Nlist(nlp_in, "BTN_REVIEW.x", 1)) {
		/* POST以外でコメントを送ることをはじく */
		chp_tmp = getenv("REQUEST_METHOD");
		if(!chp_tmp || strcasecmp(chp_tmp, "POST") != 0) {
			Put_Nlist(nlp_out, "ERROR", "CGIから直接レビューを投稿することはできません。");
			in_error = __LINE__;
			goto clear_finish;
		}
		if (Post_Review(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "BTN_REVIEW.x", "1");
		}
	// お気に入りに追加
	} else if (Get_Nlist(nlp_in, "ADD_FAVORITE.x", 1)) {
		if (add_faivorite_blog(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "BTN_COMMENT.x", "1");
			++in_error;
		}
		goto clear_finish;
	// お気に入りから削除
	} else if (Get_Nlist(nlp_in, "DEL_FAVORITE.x", 1)) {
		if (del_faivorite_blog(db, nlp_in, nlp_out, in_blog)){
			Put_Nlist(nlp_out, "ERR_START", "-->");
			Put_Nlist(nlp_out, "ERR_END", "<!--");
			Put_Nlist(nlp_in, "BTN_COMMENT.x", "1");
			++in_error;
		}
		goto clear_finish;
	}
	if(!in_error) {
		/* 各種ページを表示 */
		if (build_root(db, nlp_in, nlp_out, in_blog)) {
			in_error = __LINE__;
		/* 訪問者リストを更新 */
		} else {
			update_visitor_list(db, nlp_in, nlp_out, in_blog, g_in_login_blog);
		}
	}

clear_finish:
	if (in_error) {
		Put_Nlist(nlp_out, "ACTION", CO_CGI_BUILD_HTML);
		Put_Format_Nlist(nlp_in, "error", "%d", in_error);
		Build_HiddenAll(nlp_in, nlp_out, "HIDDEN", chpa_esc);
		Build_HiddenEncode(nlp_out, "HIDDEN", "from_error", "1");
//		Put_Format_Nlist(nlp_out, "ERROR_MSG", "エラー発生(%d)", in_error);
		Page_Out(nlp_out, CO_SKEL_ERROR);
	}
	Finish_Nlist(nlp_in);
	Finish_Nlist(nlp_out);
	Finish_Nlist(nlp_cookie);
	Db_Disconnect(db);

	return in_error;
}
