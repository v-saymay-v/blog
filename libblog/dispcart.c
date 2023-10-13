/*
+* ------------------------------------------------------------------------
 * Module-Name:		disp.c
 * First-Created:	2002/07/22 星　俊秀
%* ------------------------------------------------------------------------
 * Module-Description:
 *	商品ページの表示
-* ------------------------------------------------------------------------
 * Change-Log:
 *			2003/02/23 星　俊秀
 *				バリエーション毎に価格を持つように変更。
 *			2004/10/29 関口雅人
 *				各種表示変更。
 *			2005/08/24 関口雅人
 *				全ソース見直し、ロジック修正。
 *			2006/02/10 関口雅人
 *				TYPE D,TYPE Eの幅指定機能追加に伴う変更
$* ------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libcgi2.h"
#include "libmysql.h"
#include "libcommon2.h"
#include "libexcart.h"
#include "libcartdb.h"
#include "excart.h"
#include "libblog.h"
#include "dispcart.h"

#define CO_VARIATION_BUFFER 16384

static char gcha_rcsid[] __attribute__((__unused__)) = "$Id: dispcart.c,v 1.17 2010/05/07 10:38:26 sekine Exp $";
char g_cha_purchase_cgi[1024] = "/e-commex/cgi-bin/ex_disp_item_detail.cgi";

/* スケルトン編集不可(旧表示) */
/* old */
void build_A_line_value_f3(DBRes *, NLIST *, char *, int);
int build_A_line_value_f11(DBase *, DBRes *, NLIST *, NLIST *, char *, char *, int);
void build_a_line_guide(DBRes *, char *, int, int);
void build_variation(DBRes *, NLIST *, char *, int *, int *, int, int);
/* new */
static int build_standard(DBase *, NLIST *, NLIST *, DBRes *, char *, int *, bool, int);
static int build_exception(DBase *, NLIST *, NLIST *, DBRes *, char *, int *, bool, int);
static int build_three_col(DBase *, NLIST *, NLIST *, DBRes *, char *, int *, bool, int);
static int build_item_info_f3(DBase *, NLIST *, NLIST *, DBRes *, char *, int *, bool, int);
static int build_item_info_f11(DBase *, NLIST *, NLIST *, DBRes *, char *, int *, bool, int);
static void builditeminfo2_f3(NLIST *, DBRes *, char *, int *, bool, int);
static int builditeminfo2_f11(DBase *db, NLIST *, NLIST *, DBRes *, int *, bool, int, int);
static int build_two_col2(DBase *db, NLIST *, NLIST *, DBRes *, char *, int *, bool, int, int);
static int build_two_col4(DBase *db, NLIST *, NLIST *, DBRes *, char *, int *, bool, int, int);
static void get_item_data_f3(DBRes *, char *, int *, int);
static int build_typeA_f3(DBase *, NLIST *, NLIST *, DBRes *, char *, int *, int, int, int);
static int build_typeA_f11(DBase *, NLIST *, NLIST *, DBRes *, char *, int *, int, int, int);
/* スケルトン編集可(現行)*/
static void put_variation_f13(DBRes *, NLIST *, NLIST *, int *, int *, int, int, char*);
static int put_col(DBase *db, NLIST *, NLIST *, DBRes *, char *, char *, char *, int *, bool, int, int, int);
static int put_item_info(DBase *, NLIST *, NLIST *, DBRes *, char *, char *, char *, int *, bool, int, int, int);
static int put_item_info_f13(DBase *, NLIST *, NLIST *, DBRes *, char *, char *, int *, bool, int, int);

/*
+* ========================================================================
 * Function:		GetItemNo
 * Include-file:	disp.h
 * Description:
 *	何件目の商品から表示するかを調べる
 *	新着、人気商品一覧はバリエーションの扱いが特殊
%* ========================================================================
 * Return:
 *	in_start
-* ======================================================================*/
int GetItemNo(DBRes *dbres
	, int in_page	/* 現在のページ */
	, int in_max	/* 1ページに何件表示するか */
	, int in_row	/* バリエーションも数に入れた総レコード数 */
	, int in_b_disp_variation)	/* バリエーションを個別に表示するか */
{
	int in_cnt;
	int in_start;
	int in_present;
	int in_list_max;
	int i;

	i = 0;
	in_start = 0;
	in_cnt = 0;
	in_list_max = in_max;
	/* 1ページずつ現在ページまで */
	while(in_cnt != in_page) {
		for(i = in_start; i < in_max; i++) {	/* 1件ずつ1ページに表示する分iを進めていく */
			in_present = i;
			while(1) {		/* バリエーションの分だけiを進めていく */
				if(in_row <= i + 1) {	/* 最後まで進んだらbreak */
					break;
				}
				/* 38 sm_subitem.c_item_id */
				/* 新着・人気商品はバリエーションが個別に表示されるので進めずにbreak */
				if(in_b_disp_variation == 1) {
					break;
				} else if(!strcmp(Db_GetValue(dbres, in_present, 38) ? Db_GetValue(dbres, in_present, 38) : "", Db_GetValue(dbres, i, 38) ? Db_GetValue(dbres, i, 38) : "")) {
					if(in_row <= i + 1) {
						break;
					}
					i++;
					continue;
				} else {
					i--;
					break;
				}
			}
			/* 1より多くバリエーションが見つかれば、バリエーション数-1だけin_maxを増やしてループ延長 */
			/* バリエーション数が1なら通常の動作 */
			in_max = in_max + i - in_present;
			if(in_row <= i + 1) {
#if 0
				if(in_row % 2 == 0) {
					i--;
				}
#endif
				break;
			}
		}
		in_start = i;		/* 進んだ件数をin_startに代入 */
		in_max += in_list_max;	/* 終了条件のin_maxをとりあえず規定表示件数分増やす */
		in_cnt++;		/* ページを進める */
	}
	/* 表示開始レコードを返す */
	return in_start;
}

/*
+* ========================================================================
 * Function:		builditeminfo
 * Include-file:	disp.h
 * Description:
 *	商品情報を作成する
%* ========================================================================
 * Return:	0:正常終了
 *			1:エラー
-* ======================================================================*/
static int build_item_info_f11(
DBase *db
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
) {
//	NLIST *nlp_cookie;
	char *chp_value;
	char *chp_sub_value;
	char *chp_maker;
	char *chp_script;
	char *chp_cgipath;
	char *chp_return_cgi_id;
	char *chp_esc;
	char *chp_layout;
	char cha_variation[CO_VARIATION_BUFFER * 10];
	char cha_standard_price[64];
	char cha_price[64];
	char cha_member_price[64];
	char cha_postage[64];
	char cha_cool_postage[64];
	char cha_extra_postage[64];
	char cha_item_type[64];
	const int in_base_i = *inp_i;
	int in_subitem;
	int in_vip_price;
	int in_count;
	int in_reserve_count;
	int in_b_reserve;
	int in_b_stock;	/* 在庫管理 */
	int in_cool;
//	int in_ret;
	int i;

	chp_value = getenv("SCRIPT_NAME");
	chp_script = strrchr(chp_value, '/');
	++chp_script;
	i = in_base_i;
	in_subitem = in_base_i;
	memset(cha_variation, '\0', sizeof(cha_variation));
	if(!strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL) || !strcmp(chp_script, CO_ACTION_CREATE_ITEM_HTML)) {
		if(strcmp(Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", Db_GetValue(dbres, in_base_i, 38) ? Db_GetValue(dbres, in_base_i, 38) : "") == 0
		   && strcmp(Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", Db_GetValue(dbres, in_base_i, 11) ? Db_GetValue(dbres, in_base_i, 11) : "") != 0) {
			strcat(cha_variation, "<td align=\"center\">\n");
			strcat(cha_variation, "<select name=\"SUBITEMID\" onchange=\"this.form.submit()\">\n");
			for(; i < in_row; i++) {
				chp_value = Get_Nlist(nlp_in, "subitem", 1);
				if(!chp_value || *chp_value == '\0') {
					chp_value = Get_Nlist(nlp_in, "SUBITEMID", 1);
				}
				if(!chp_value || *chp_value == '\0') {
					chp_value = Db_GetValue(dbres, in_base_i, 11);
				}
				if(!chp_value || *chp_value == '\0') {
					chp_value = Db_GetValue(dbres, in_base_i, 0);
				}
				if(chp_value && strcmp(chp_value, Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "") == 0) {
					chp_sub_value = Escape_HtmlString(Db_GetValue(dbres, i, 12) ? Db_GetValue(dbres, i, 12) : "");
					sprintf(&cha_variation[strlen(cha_variation)],
						"<option value=\"%s\" selected>%s</option>\n"
							, Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "", chp_sub_value);
					free(chp_sub_value);
					in_subitem = i;
				} else if(!chp_value || strcmp(chp_value, Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "") != 0) {
					chp_sub_value = Escape_HtmlString(Db_GetValue(dbres, i, 12) ? Db_GetValue(dbres, i, 12) : "");
					sprintf(&cha_variation[strlen(cha_variation)],
						"<option value=\"%s\">%s</option>\n"
							, Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "", chp_sub_value);
					free(chp_sub_value);
				}
				if(i + 1 == in_row || !Db_GetValue(dbres, i + 1, 0) || strcmp(Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", Db_GetValue(dbres, i + 1, 0) ? Db_GetValue(dbres, i + 1, 0) : "") != 0) {
					break;
				}
			}
			strcat(cha_variation, "</select>\n");
			strcat(cha_variation, "</td>\n");
		}
	} else if(!strcmp(chp_script, CO_ACTION_DISP_NEW) || !strcmp(chp_script, CO_ACTION_DISP_HIT)) {
	} else {
	}
	memset(cha_standard_price, '\0', sizeof(cha_standard_price));
	memset(cha_price, '\0', sizeof(cha_price));
	memset(cha_member_price, '\0', sizeof(cha_member_price));
	memset(cha_postage, '\0', sizeof(cha_postage));
	memset(cha_extra_postage, '\0', sizeof(cha_extra_postage));
	memset(cha_item_type, '\0', sizeof(cha_item_type));
	in_vip_price = 0;
	in_count = 0;
	in_reserve_count = 0;
	in_b_reserve = 0;
	in_b_stock = 0;
	Get_Item_Data(OldDBRes(dbres), cha_standard_price, cha_price, cha_member_price, cha_postage, cha_cool_postage, cha_extra_postage, cha_item_type
	, &in_vip_price, &in_count, &in_reserve_count, &in_b_reserve, &in_b_stock, &in_cool, in_subitem);
	/* html */
	chp_cgipath = Get_Cart_CgiPath();
	strcat(chp_dest, "<table border=\"0\" cellpadding=\"3\" cellspacing=\"2\" width=\"100%\">\n");
	strcat(chp_dest, "<tr>\n");
	strcat(chp_dest, "<td align=\"right\" valign=\"top\" width=\"30%\" nowrap><span class=\"small\">商品名：</td>\n");
	chp_sub_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1) ? Db_GetValue(dbres, in_base_i, 1) : "");
	if(b_link) {
		sprintf(chp_dest + strlen(chp_dest),
			"<td width=\"70%%\"><span class=\"small\"><a href=\"%s?id=%s\">%s</a></td>\n"
			, g_cha_purchase_cgi
				, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", chp_sub_value
		);
	} else {
		sprintf(chp_dest + strlen(chp_dest),
			"<td><span class=\"small\">%s</td>\n"
			, chp_sub_value
		);
	}
	free(chp_sub_value);
	strcat(chp_dest, "</tr>\n");
	strcat(chp_dest, "<tr>\n");
	strcat(chp_dest, "<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">\n");
	chp_value = Db_GetValue(dbres, in_base_i, 6);
	chp_maker = Db_GetValue(dbres, in_base_i, 34);
	if(chp_maker && *chp_maker != '\0') {
		chp_esc = Escape_HtmlString(chp_maker);
		strcat(chp_dest, "メーカー：</td>\n");
		if(chp_value && *chp_value != '\0' && strcmp(chp_value, "http://") > 0) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td><span class=\"small\"><a href=\"%s\" target=\"_blank\">%s  </a></td>\n"
				, chp_value, chp_esc
			);
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td><span class=\"small\">%s  </td>\n"
				, chp_esc
			);
		}
		free(chp_esc);
	} else if(chp_value && *chp_value != '\0' && strcmp(chp_value, "http://") > 0) {
		sprintf(chp_dest + strlen(chp_dest),
			"<a href=\"%s\" target=\"_blank\">メーカーリンク</a></td>\n", chp_value
		);
	}
	strcat(chp_dest, "</tr>\n");
	strcat(chp_dest, "<tr>\n");
	strcat(chp_dest, "<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">\n");
	if(cha_item_type[0] != '\0') {
		strcat(chp_dest, "メーカー型番：</td>\n");
		chp_sub_value = Escape_HtmlString(cha_item_type);
		sprintf(chp_dest + strlen(chp_dest),
			"<td><span class=\"small\">%s</td>\n"
			, chp_sub_value
		);
		free(chp_sub_value);
	}
	strcat(chp_dest, "</tr>\n");
	/* price */
	if(Db_GetValue(dbres, i, 41) && *Db_GetValue(dbres, i, 41) == '1' && cha_standard_price[0] != '\0') {
		sprintf(chp_dest + strlen(chp_dest),
			"<tr>\n"
				"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">標準価格：</td>\n"
				"<td><span class=\"small\">%s</td>\n"
			"</tr>\n"
			, cha_standard_price
		);
	}
	if(in_vip_price == 2) {
		sprintf(chp_dest + strlen(chp_dest),
			"<tr>\n"
				"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">一般価格：</td>\n"
				"<td><span class=\"small\">%s</td>\n"
			"</tr>\n"
			, cha_price
		);
		sprintf(chp_dest + strlen(chp_dest),
			"<tr>\n"
				"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">会員価格：</td>\n"
				"<td><span class=\"small\">%s</td>\n"
			"</tr>\n"
			, cha_member_price
		);
	} else if(in_vip_price == 1) {
		sprintf(chp_dest + strlen(chp_dest),
			"<tr>\n"
				"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">会員価格：</td>\n"
				"<td><span class=\"small\">%s</td>\n"
			"</tr>\n"
			, cha_member_price
		);
	} else {
		sprintf(chp_dest + strlen(chp_dest),
			"<tr>\n"
			"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">価格：</td>\n"
					"<td><span class=\"small\">%s</td>\n"
			"</tr>\n"
			, cha_price
		);
	}
	if(Db_GetValue(dbres, in_subitem, 42) && *Db_GetValue(dbres, in_subitem, 42) == '1'
	&& Db_GetValue(dbres, in_subitem, 43) && *Db_GetValue(dbres, in_subitem, 43) != '\0'  && atoi(Db_GetValue(dbres, in_subitem, 43)) > 0) {
		sprintf(chp_dest + strlen(chp_dest),
			"<tr>\n"
				"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">ポイント還元：</td>\n"
				"<td><span class=\"small\">%s％</td>\n"
			"</tr>\n"
			, Db_GetValue(dbres, in_subitem, 43)
		);
	}
	sprintf(chp_dest + strlen(chp_dest),
		"<tr>\n"
			"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">送料：</td>\n"
			"<td nowrap><span class=\"small\">%s</td>\n"
		"</tr>\n"
		, cha_postage
	);
	if(cha_extra_postage[0] != '\0') {
		sprintf(chp_dest + strlen(chp_dest),
			"<tr>\n"
				"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">別途追加送料：</td>\n"
				"<td nowrap><span class=\"small\">%s</td>\n"
			"</tr>\n"
			, cha_extra_postage
		);
	}
	/* stock */
	if(in_b_stock == 1 && Db_GetValue(dbres, in_base_i, 31) && *Db_GetValue(dbres, in_base_i, 31) == '1') {
		strcat(chp_dest, "<tr>\n");
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			strcat(chp_dest, "<td align=\"right\" valign=\"top\" nowrap>"
				"<span class=\"small\">在庫数：</td>\n");
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				strcat(chp_dest, "<td><span class=\"small\">　</td>\n");
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td><span class=\"small\">%s</td>\n"
					, Db_GetValue(dbres, in_subitem, 44)
				);
			}
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"right\" valign=\"top\" nowrap>"
					"<span class=\"small\"><font color=\"#%s\">[予約販売] </font>在庫数："
					"</td>\n", Db_GetValue(dbres, 0, 51) ? Db_GetValue(dbres, 0, 51) : ""
			);
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				strcat(chp_dest, "<td><span class=\"small\">　</td>\n");
			} else {
				sprintf(chp_dest + strlen(chp_dest), "<td><span class=\"small\">%s</td>\n", Db_GetValue(dbres, in_subitem, 45));
			}
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"right\" valign=\"top\" nowrap>"
					"<span class=\"small\"><font color=\"#%s\">[予約販売] </font>在庫数："
				"</td>\n"
				"<td><span class=\"small\">%d</td>\n"
					, Db_GetValue(dbres, 0, 51) ? Db_GetValue(dbres, 0, 51) : ""
				, in_reserve_count
			);
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"right\" valign=\"top\" nowrap>"
					"<span class=\"small\">在庫数："
				"</td>\n"
				"<td><span class=\"small\">%d</td>\n"
				, in_count
			);
		}
		strcat(chp_dest, "</tr>\n");
	} else if(in_b_stock == 1) {
		strcat(chp_dest, "<tr>\n");
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			strcat(chp_dest, "<td align=\"right\" valign=\"top\" nowrap>"
				"<span class=\"small\">　</td>\n");
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				strcat(chp_dest, "<td><span class=\"small\">　</td>\n");
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td><span class=\"small\">%s</td>\n"
					, Db_GetValue(dbres, in_subitem, 44)
				);
			}
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"right\" valign=\"top\" nowrap>"
					"<span class=\"small\"><font color=\"#%s\">[予約販売] </font>　"
				"</td>\n"
					, Db_GetValue(dbres, 0, 51) ? Db_GetValue(dbres, 0, 51) : ""
			);
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				strcat(chp_dest, "<td><span class=\"small\">　</td>\n");
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td><span class=\"small\">%s</td>\n"
					, Db_GetValue(dbres, in_subitem, 45)
				);
			}
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"right\" valign=\"top\" nowrap>"
					"<span class=\"small\"><font color=\"#%s\">[予約販売]</font>"
				"</td>\n"
					, Db_GetValue(dbres, 0, 51) ? Db_GetValue(dbres, 0, 51) : ""
			);
		}
		strcat(chp_dest, "</tr>\n");
	}
	chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
	if(!chp_return_cgi_id) {
		chp_return_cgi_id = "";
	}
	/* 詳細ページのときのみ表示 */
	chp_layout = Db_GetValue(dbres, in_base_i, 16) ? Db_GetValue(dbres, in_base_i, 16) : "";
	if (in_b_stock == 0
	|| in_count > 0
	|| strcmp(Db_GetValue(dbres, in_subitem, 0) ? Db_GetValue(dbres, in_subitem, 0) : "", Db_GetValue(dbres, in_subitem, 11) ? Db_GetValue(dbres, in_subitem, 11) : "") != 0
	|| (in_b_reserve == 1 && in_reserve_count > 0)) {
		strcat(chp_dest, "<tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<form action=\"%s/%s\" method=\"post\" name=\"%s\">\n"
			, chp_cgipath, CO_ACTION_PURCHASE_ITEM, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : ""
		);
		/* バリエーションの表示 */
		Put_Nlist(nlp_out, "ARTICLE", chp_dest);
		*chp_dest = '\0';
#if 0
		in_ret = Disp_Item_Variation(db, nlp_in, nlp_out, "ARTICLE", Db_GetValue(dbres, in_base_i, 38) ? Db_GetValue(dbres, in_base_i, 38) : "", chp_layout, 0);
		if(in_ret) {
			return 1;
		}
#endif
		strcat(chp_dest, "<td colspan=\"2\" align=\"right\">");
		sprintf(chp_dest + strlen(chp_dest),
			"<input type=\"hidden\" name=\"ITEMID\" value=\"%s\">\n"
			"<input type=\"hidden\" name=\"RETURN_CGI_ID\" value=\"%s\">\n"
					, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", chp_return_cgi_id
		);
		if(in_b_stock == 0 || in_count > 0
		|| (in_b_reserve == 1 && in_reserve_count > 0)) {
			strcat(chp_dest, "<input type=\"submit\" value=\"カートに入れる\"></td></form>\n");
		} else {
			strcat(chp_dest, "</td></form>\n");
		}
		strcat(chp_dest, "</tr>\n");
	}
	strcat(chp_dest, "</tr>\n");
	if(strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL) != 0 && strcmp(chp_script, CO_ACTION_CREATE_ITEM_HTML) != 0) {
		*inp_i = i;
	}
	strcat(chp_dest, "</table>\n");
	return 0;
}

/*
+* ========================================================================
 * Function:		builditeminfo_f3
 * Include-file:	disp.h
 * Description:
 *	商品情報を作成する
%* ========================================================================
 * Return:	0:正常終了
 *			1:エラー
-* ======================================================================*/
static int build_item_info_f3(
DBase *db
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
)
{
	DBRes *current_dbres;
	char *chp_value;
	char *chp_esc;
	char *chp_sub_value;
	char *chp_script;
	char *chp_cgipath;
	char *chp_return_cgi_id;
	char cha_sql[1024];
	char cha_price[64];
	const int in_base_i = *inp_i;
	int in_subitem;
	int in_count;
	int in_act;
	int i;

	chp_value = getenv("SCRIPT_NAME");
	chp_script = strrchr(chp_value, '/');
	++chp_script;
	i = in_base_i;
	in_subitem = in_base_i;
	memset(cha_price, '\0', sizeof(cha_price));
	in_count = 0;
	get_item_data_f3(dbres, cha_price, &in_count, in_subitem);
	chp_cgipath = Get_Cart_CgiPath();
	strcat(chp_dest, "<table border=\"0\" cellpadding=\"3\" cellspacing=\"2\" width=\"100%\">\n");
	strcat(chp_dest, "<tr>\n");
	strcat(chp_dest, "<td align=\"right\" valign=\"top\" width=\"30%\" nowrap><span class=\"small\">商品コード：</td>\n");
	sprintf(chp_dest + strlen(chp_dest), "<td><span class=\"small\">%s</td>\n", Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
	strcat(chp_dest, "</tr>\n");
	strcat(chp_dest, "<tr>\n");
	strcat(chp_dest, "<td align=\"right\" valign=\"top\" width=\"30%\" nowrap><span class=\"small\">商品名：</td>\n");
	chp_sub_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1) ? Db_GetValue(dbres, in_base_i, 1) : "");
	if(b_link) {
		sprintf(chp_dest + strlen(chp_dest),
			"<td width=\"70%%\"><span class=\"small\"><a href=\"%s?id=%s\">%s</a></td>\n"
			, g_cha_purchase_cgi
			, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", chp_sub_value
		);
	} else {
		sprintf(chp_dest + strlen(chp_dest),
			"<td><span class=\"small\">%s</td>\n"
			, chp_sub_value
		);
	}
	free(chp_sub_value);
	strcat(chp_dest, "</tr>\n");
	sprintf(chp_dest + strlen(chp_dest),
		"<tr>\n"
		"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">価格：</td>\n"
				"<td><span class=\"small\">%s</td>\n"
		"</tr>\n"
		, cha_price
	);
	strcat(chp_dest, "<tr>\n");
	if(in_count <= 0) {
		strcat(chp_dest, "<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">在庫数：</td>\n");
		if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
			strcat(chp_dest, "<td><span class=\"small\">　</td>\n");
		} else {
			sprintf(chp_dest + strlen(chp_dest), "<td><span class=\"small\">%s</td>\n", Db_GetValue(dbres, in_subitem, 44));
		}
	} else {
		chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_subitem, 64) ? Db_GetValue(dbres, in_subitem, 64) : "");
		sprintf(chp_dest + strlen(chp_dest),
			"<td align=\"right\" valign=\"top\" nowrap>"
				"<span class=\"small\">在庫数："
			"</td>\n"
			"<td><span class=\"small\">%d%s</td>\n"
			, in_count, chp_esc
		);
		free(chp_esc);
	}
	/* 詳細ページのときのみ表示 */
	chp_value = Db_GetValue(dbres, in_base_i, 63);
	in_act = chp_value && *chp_value != '\0' ?  atoi(chp_value) : CO_FUNCTION_3_ACTING_CUSTOMER;
	if(!strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL)) {
		/* 営業員のみ */
		if(in_act == CO_FUNCTION_3_ACTING_TRANSFER || in_act == CO_FUNCTION_3_ACTING_PATRON) {
			memset(cha_price, '\0', sizeof(cha_price));
			strcpy(cha_price,  Db_GetValue(dbres, in_base_i, 50) ? Db_GetValue(dbres, in_base_i, 50) : "");
			Edit_Money(cha_price);
			sprintf(chp_dest + strlen(chp_dest),
				"<tr>\n"
					"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">発注単位：</td>\n"
					"<td><span class=\"small\">%s</td>\n"
				"</tr>\n"
				, cha_price
			);
			memset(cha_price, '\0', sizeof(cha_price));
			strcpy(cha_price,  Db_GetValue(dbres, in_base_i, 51) ? Db_GetValue(dbres, in_base_i, 51) : "");
			Edit_Money(cha_price);
			sprintf(chp_dest + strlen(chp_dest),
				"<tr>\n"
					"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">上限：</td>\n"
					"<td><span class=\"small\">%s</td>\n"
				"</tr>\n"
				, cha_price
			);
			memset(cha_price, '\0', sizeof(cha_price));
			strcpy(cha_price,  Db_GetValue(dbres, in_base_i, 52) ? Db_GetValue(dbres, in_base_i, 52) : "");
			Edit_Money(cha_price);
			sprintf(chp_dest + strlen(chp_dest),
				"<tr>\n"
					"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">下限：</td>\n"
					"<td><span class=\"small\">%s</td>\n"
				"</tr>\n"
				, cha_price
			);
			memset(cha_price, '\0', sizeof(cha_price));
			strcpy(cha_price,  Db_GetValue(dbres, in_base_i, 53) ? Db_GetValue(dbres, in_base_i, 53) : "");
			Edit_Money(cha_price);
			sprintf(chp_dest + strlen(chp_dest),
				"<tr>\n"
					"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">入数：</td>\n"
					"<td><span class=\"small\">%s</td>\n"
				"</tr>\n"
				, cha_price
			);
			memset(cha_price, '\0', sizeof(cha_price));
			strcpy(cha_price,  Db_GetValue(dbres, in_base_i, 54) ? Db_GetValue(dbres, in_base_i, 54) : "");
			Edit_Money(cha_price);
			sprintf(chp_dest + strlen(chp_dest),
				"<tr>\n"
					"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">仕入原価：</td>\n"
					"<td><span class=\"small\">%s円</td>\n"
				"</tr>\n"
				, cha_price
			);
			memset(cha_sql, '\0', sizeof(cha_sql));
			memset(cha_sql, '\0', sizeof(cha_sql));
			strcpy(cha_sql, "select T1.n_selling_price");	/* 0 */
			strcat(cha_sql, " from st_item_price_rank T1");
			sprintf(cha_sql + strlen(cha_sql), " where T1.c_item_id = '%s'", Db_GetValue(dbres, in_base_i, 0));
			strcat(cha_sql, " and T1.c_sub1_id = T1.c_item_id");
			strcat(cha_sql, " and T1.c_sub2_id = T1.c_item_id");
			strcat(cha_sql, " order by T1.n_rank");
			current_dbres = Db_OpenDyna(db, cha_sql);
			if(!current_dbres) {
				memset(cha_sql, '\0', sizeof(cha_sql));
				sprintf(cha_sql, "%d：query error(%s)", __LINE__, Gcha_last_error);
				Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
				return 1;
			}
			in_row = Db_GetRowCount(current_dbres);
			if(in_row != 6) {
				Db_CloseDyna(current_dbres);
				memset(cha_sql, '\0', sizeof(cha_sql));
				sprintf(cha_sql, "%d：商品登録情報が正しくありません。", __LINE__);
				Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
				return 1;
			}
			for(i = 0; i < in_row - 1; i++) {
				memset(cha_price, '\0', sizeof(cha_price));
				strcpy(cha_price, Db_GetValue(current_dbres, i, 0) ? Db_GetValue(current_dbres, i, 0) : "");
				Edit_Money(cha_price);
				sprintf(chp_dest + strlen(chp_dest),
					"<tr>\n"
						"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">価格%d：</td>\n"
						"<td><span class=\"small\">%s円</td>\n"
					"</tr>\n"
					, i + 1, cha_price
				);
			}
			memset(cha_price, '\0', sizeof(cha_price));
			strcpy(cha_price, Db_GetValue(current_dbres, i, 0) ? Db_GetValue(current_dbres, i, 0) : "");
			Edit_Money(cha_price);
			sprintf(chp_dest + strlen(chp_dest),
				"<tr>\n"
					"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">上代：</td>\n"
					"<td><span class=\"small\">%s円</td>\n"
				"</tr>\n"
				, cha_price
			);
			Db_CloseDyna(current_dbres);
		}
	}
	strcat(chp_dest, "</tr>\n");
	chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
	if(!chp_return_cgi_id) {
		chp_return_cgi_id = "";
	}
	/* 詳細ページのときのみ表示 */
	if(!strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL) || !strcmp(chp_script, CO_ACTION_CREATE_ITEM_HTML)) {
		strcat(chp_dest, "<tr>\n");
		sprintf(chp_dest + strlen(chp_dest), "<form action=\"%s\" method=\"post\">\n", g_cha_purchase_cgi);
		/* バリエーションの表示無し */
		strcat(chp_dest, "<td colspan=\"2\" align=\"right\">\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<input type=\"hidden\" name=\"ITEMID\" value=\"%s\">\n"
			"<input type=\"hidden\" name=\"SUBITEMID\" value=\"%s\">\n"
			"<input type=\"hidden\" name=\"RETURN_CGI_ID\" value=\"%s\">\n"
			"<input type=\"submit\" name=\"related\" value=\"カートに入れる\">"
			"</td>\n"
			, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : ""
			, Db_GetValue(dbres, in_subitem, 11) ? Db_GetValue(dbres, in_subitem, 11) : ""
			, chp_return_cgi_id);
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "<tr>\n");
		/* 問い合わせボタン */
		if(Db_GetValue(dbres, in_base_i, 47) && *Db_GetValue(dbres, in_base_i, 47) == '1') {
			strcat(chp_dest, "<td align=\"right\">\n");
			sprintf(chp_dest + strlen(chp_dest),
				"<input type=\"button\" value=\"問い合わせ\" onclick=\"location.href='%s/%s?ITEMID=%s&SUBITEMID=%s'\">"
				"</td>\n"
					, chp_cgipath, CO_ACTION_INQUIRE_ITEM
					, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : ""
					, Db_GetValue(dbres, in_subitem, 11) ? Db_GetValue(dbres, in_subitem, 11) : "");
		}
		if(in_act == CO_FUNCTION_3_ACTING_TRANSFER || in_act == CO_FUNCTION_3_ACTING_PATRON) {
			if(Db_GetValue(dbres, in_base_i, 61) && *Db_GetValue(dbres, in_base_i, 61) != '\0') {
				strcat(chp_dest, "<td align=\"right\">");
				sprintf(chp_dest + strlen(chp_dest)
				, "<input type=\"button\" value=\"仕入先情報\" onclick=\"window.open('%s/%s?VENDORID=%s')\">"
				, chp_cgipath, CO_ACTION_VENDOR_LIST_USER, Db_GetValue(dbres, in_base_i, 61));
				sprintf(chp_dest + strlen(chp_dest)
				, "&nbsp;<input type=\"button\" value=\"仕入商品\" onclick=\"window.open('%s/%s?VENDORID=%s&ITEMID=%s')\">"
				, chp_cgipath, CO_ACTION_VENDOR_LIST_USER, Db_GetValue(dbres, in_base_i, 61), Db_GetValue(dbres, in_base_i, 0));
				strcat(chp_dest, "</td>\n");
			} else {
				strcat(chp_dest, "<td align=\"right\">　</td>\n");
			}
			strcat(chp_dest, "<td align=\"right\">\n");
		}
	} else {
		strcat(chp_dest, "<tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<form action=\"%s\" method=\"post\" name=\"%s\">\n"
				"<td colspan=\"2\" align=\"right\">\n"
				, g_cha_purchase_cgi, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : ""
		);
		/* バリエーションの表示無し */
		sprintf(chp_dest + strlen(chp_dest),
			"<input type=\"hidden\" name=\"ITEMID\" value=\"%s\">\n"
			"<input type=\"hidden\" name=\"RETURN_CGI_ID\" value=\"%s\">\n"
				, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", chp_return_cgi_id
		);
		strcat(chp_dest, "<input type=\"submit\" value=\"カートに入れる\"></td></form>\n");
		strcat(chp_dest, "</tr>\n");
	}
	strcat(chp_dest, "</tr>\n");
	if(strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL) != 0 && strcmp(chp_script, CO_ACTION_CREATE_ITEM_HTML) != 0) {
		*inp_i = i;
	}
	strcat(chp_dest, "</table>\n");
	return 0;
}

/*
+* ========================================================================
 * Function:		バリエーションセレクトボックス作成
 * Include-file:	disp.h
 * Description:
 *
%* ========================================================================
 * Return:	なし
-* ======================================================================*/
static void My_Put_Variation(
DBRes *dbres
, NLIST *nlp_in
, NLIST *nlp_out
, int *inp_subitem
, int *inp_count
, int in_row
, int in_itemid_row
)
{
#define CO_VARIATION_CHOP 64
	char *chp_root_item_id;
	char *chp_root_sub1_id;
	char *chp_dbase_item_id;
	char *chp_dbase_sub1_id;
	char *chp_dbase_sub2_id;
	char *chp_selected_item_id;
	char *chp_selected_sub1_id;
	char *chp_selected_sub2_id;
	char *chp_prev_sub1_id;
	char const *chp_form_name;
	char *chp_esc;
	char *chp_esc2;
	char cha_name1[128];
	char cha_name2[128];
	char cha_str[1024];
	int in_subitem;
	int in_have_subitem;
	int in_item_end;
	int in_flg;
	int i;

	in_have_subitem = 0;
	memset(cha_name1, '\0', sizeof(cha_name1));
	memset(cha_name2, '\0', sizeof(cha_name2));
	chp_root_item_id = Db_GetValue(dbres, in_itemid_row, 0);
	chp_selected_item_id = Get_Nlist(nlp_in, "item", 1);
	chp_selected_sub1_id = Get_Nlist(nlp_in, "subitem", 1);
	chp_selected_sub2_id = Get_Nlist(nlp_in, "subitem2", 1);
	strcpy(cha_name1, "SUBITEMID");
	strcpy(cha_name2, "SUBITEM2ID");
	chp_prev_sub1_id = NULL;
	/* バリエーション[1] */
	for(i = in_itemid_row, in_subitem = in_itemid_row, in_flg = 0; i < in_row; i++) {
		chp_dbase_item_id = Db_GetValue(dbres, i, 0);
		if(strcmp(chp_root_item_id, chp_dbase_item_id)) {
			/* 次の商品に入っている */
			break;
		}
		chp_dbase_sub1_id = Db_GetValue(dbres, i, 11);
		if(strcmp(chp_dbase_item_id, chp_dbase_sub1_id) == 0) {
			/* バリエーション[1]が存在しない。次の商品になるまで飛ばす */
			continue;
		}
		if(chp_prev_sub1_id && strcmp(chp_prev_sub1_id, chp_dbase_sub1_id) == 0) {
			/* 重複しているので、ここは飛ばす */
			continue;
		}
		chp_prev_sub1_id = chp_dbase_sub1_id;
		if(in_flg == 0) {	/* 最初 */
			chp_form_name = chp_dbase_item_id;
			memset(cha_str, '\0', sizeof(cha_str));
			/* 購入ボタンの呼び出し先がdisp_item_detailのものと、それ以外で分かれる */
			sprintf(cha_str, "<select onchange=\"javascript:CallCGI('%s')\" name=\"%s\">\n"
					, chp_form_name, cha_name1);
			Put_Nlist(nlp_out, "ITEM_VARIATION", cha_str);
			in_flg = 1;
			in_have_subitem = 1;
		}
		/* バリエーション有り */
		chp_esc = Escape_HtmlString(Db_GetValue(dbres, i, 12));
		free(chp_esc);
		chp_esc2 = Chop_String(chp_esc, CO_VARIATION_CHOP);
		memset(cha_str, '\0', sizeof(cha_str));
		if(chp_selected_item_id && chp_selected_sub1_id && !strcmp(chp_selected_item_id, chp_dbase_item_id) && !strcmp(chp_selected_sub1_id, chp_dbase_sub1_id)) {
			sprintf(cha_str, "<option value=\"%s\" selected>%s</option>\n", chp_dbase_sub1_id, chp_esc2);
			in_subitem = i;
		} else {
			sprintf(cha_str, "<option value=\"%s\">%s</option>\n", chp_dbase_sub1_id, chp_esc2);
		}
		free(chp_esc2);
		Put_Nlist(nlp_out, "ITEM_VARIATION", cha_str);
	}
	if(in_flg) {
		Put_Nlist(nlp_out, "ITEM_VARIATION", "</select>\n");
	}
	in_item_end = i;
	chp_root_sub1_id = Db_GetValue(dbres, in_subitem, 11);
	for(i = in_subitem, in_flg = 0; i < in_row; i++) {
		chp_dbase_item_id = Db_GetValue(dbres, i, 0);
		if(strcmp(chp_root_item_id, chp_dbase_item_id)) {
			/* 次の商品に入っている */
			break;
		}
		chp_dbase_sub1_id = Db_GetValue(dbres, i, 11);
		if(strcmp(chp_root_sub1_id, chp_dbase_sub1_id) != 0) {
			break;
		}
		chp_dbase_sub2_id = Db_GetValue(dbres, i, 58);
		if(strcmp(chp_dbase_item_id, chp_dbase_sub2_id) == 0) {
			break;
		}
		if(in_flg == 0) {	/* 最初 */
			chp_form_name = chp_dbase_item_id;
			memset(cha_str, '\0', sizeof(cha_str));
			/* 購入ボタンの呼び出し先がdisp_item_detailのものと、それ以外で分かれる */
			sprintf(cha_str, "<select onchange=\"javascript:CallCGI('%s')\" name=\"%s\">\n"
					, chp_form_name, cha_name2);
			Put_Nlist(nlp_out, "ITEM_VARIATION", cha_str);
			in_flg = 1;
			in_have_subitem = 1;
		}
		/* バリエーション有り */
		chp_esc = Escape_HtmlString(Db_GetValue(dbres, i, 59));
		free(chp_esc);
		chp_esc2 = Chop_String(chp_esc, CO_VARIATION_CHOP);
		memset(cha_str, '\0', sizeof(cha_str));
		if(chp_selected_item_id && chp_selected_sub2_id && !strcmp(chp_selected_item_id, chp_dbase_item_id) && !strcmp(chp_selected_sub2_id, chp_dbase_sub2_id)) {
			sprintf(cha_str, "<option value=\"%s\" selected>%s</option>\n", chp_dbase_sub2_id, chp_esc2);
			in_subitem = i;
		} else {
			sprintf(cha_str, "<option value=\"%s\">%s</option>\n", chp_dbase_sub2_id, chp_esc2);
		}
		free(chp_esc2);
		Put_Nlist(nlp_out, "ITEM_VARIATION", cha_str);
	}
	if(in_flg) {
		Put_Nlist(nlp_out, "ITEM_VARIATION", "</select>\n");
	}
	if(!in_have_subitem) {
		Put_Nlist(nlp_out, "NO_VARIATION_START", "<!--NO_VARIATION");
		Put_Nlist(nlp_out, "NO_VARIATION_END", "-->");
	}
	*inp_subitem = in_subitem;
	*inp_count = in_item_end - 1;
	return;
}

/*
+* ------------------------------------------------------------------------
 * Function:		put_item_info
 * Include-file:	disp.h
 * Description:
 *	商品情報を作成する
 *	表示タイプD-1,D-2用商品表示関数
 *	表示タイプE-1,E-2用商品表示関数
%* ------------------------------------------------------------------------
 *			0:正常終了
 *			1:エラー
-* ------------------------------------------------------------------------
 */
static int put_item_info(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_script
, char *chp_skl
, char *chp_undisp_itemid_list
, int *inp_i
, bool b_link
, int in_row
, int in_disp_no	/* typeA用 */
, int in_btn
) {
	NLIST *nlp_temp;
	DBRes *dbres2;
	char *chp_dbase_item_id;
	char *chp_dir;
	char *chp_img;
	char *chp_cgipath;
	char *chp_defimagepath;
	char *chp_thumbnail;
	char *chp_image;
	char *chp_layout;
	char *chp_return_cgi_id;
	char *chp_value;
	char *chp_conv;
	char *chp_maker;
	char *chp_esc;
	char cha_sql[2048];
	char cha_str[2048];
	char cha_standard_price[64];
	char cha_price[64];
	char cha_member_price[64];
	char cha_postage[64];
	char cha_cool_postage[64];
	char cha_extra_postage[64];
	char cha_item_type[64];
	const int in_base_i = *inp_i;
	bool b_reserve;
	bool b_stock;
	bool b_review;
	int in_vip_price;
	int in_columns;
	int in_count;
	int in_reserve_count;
	int in_subitem;
	int in_ret;
	int in_login;
	int in_cool;
	int in_width;
	int in_height;
	int in_image_size;
	int in_file_type;
	int i;

	nlp_temp = Init_Nlist();
	/* 設定取得表示 */
	in_ret = Set_Style_Other(db, nlp_temp);
	if(in_ret) {
		return 1;
	}
	/* ボタンの種類 */
	if (in_btn) {
		Put_Nlist(nlp_temp, "BTNTYPE", "submit");
	} else {
		Put_Nlist(nlp_temp, "BTNTYPE", "button");
	}
	chp_dbase_item_id = Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "";
	/* 設定 */
	chp_value = Db_GetValue(dbres, 0, 52);
	if(chp_value && *chp_value != '\0') {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " width=\"%s\"", chp_value);
		Put_Nlist(nlp_temp, "TABLE_WIDTH", cha_str);
	} else {
		memset(cha_str, '\0', sizeof(cha_str));
		strcpy(cha_str, " width=\"100%\"");
		Put_Nlist(nlp_temp, "TABLE_WIDTH", cha_str);
	}
	chp_value = Db_GetValue(dbres, 0, 53);
	if(chp_value && *chp_value != '\0') {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " height=\"%s\"", chp_value);
		Put_Nlist(nlp_temp, "TABLE_HEIGHT", cha_str);
	} else {
		/* empty is old format */
	}
	/* format */
	chp_value = Db_GetValue(dbres, 0, 20) ? Db_GetValue(dbres, 0, 20) : "";
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, " border=\"%s\"", chp_value);
	Put_Nlist(nlp_temp, "TABLE_BORDER", cha_str);
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, " height=\"%s\"", chp_value);
	Put_Nlist(nlp_temp, "TABLE_BORDER2", cha_str);
	if(atoi(chp_value) > 0) {
		chp_value = Db_GetValue(dbres, 0, 19) ? Db_GetValue(dbres, 0, 19) : "";
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " bordercolor=\"#%s\"", chp_value);
		Put_Nlist(nlp_temp, "TABLE_BORDER_COLOR", cha_str);
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " bgcolor=\"#%s\"", chp_value);
		Put_Nlist(nlp_temp, "TABLE_BORDER_COLOR2", cha_str);
	} else {
		/* empty */
	}
	chp_layout = Db_GetValue(dbres, in_base_i, 16) ? Db_GetValue(dbres, in_base_i, 16) : "";
	i = in_base_i;
	in_subitem = in_base_i;
	if(!strcmp(chp_script, CO_ACTION_DISP_NEW) || !strcmp(chp_script, CO_ACTION_DISP_HIT)) {
		/* empty */
	} else {
		Build_Hidden(nlp_temp, "HIDDEN", "id", chp_dbase_item_id);
		My_Put_Variation(dbres, nlp_in, nlp_temp, &in_subitem, &i, in_row, in_base_i);
	}
	memset(cha_standard_price, '\0', sizeof(cha_standard_price));
	memset(cha_price, '\0', sizeof(cha_price));
	memset(cha_member_price, '\0', sizeof(cha_member_price));
	memset(cha_postage, '\0', sizeof(cha_postage));
	memset(cha_cool_postage, '\0', sizeof(cha_cool_postage));
	memset(cha_extra_postage, '\0', sizeof(cha_extra_postage));
	memset(cha_item_type, '\0', sizeof(cha_item_type));		/* 未使用 */
	in_vip_price = 0;
	in_count = 0;
	in_reserve_count = 0;
	b_reserve = 0;
	b_stock = 0;
	Get_Item_Data(OldDBRes(dbres), cha_standard_price, cha_price, cha_member_price, cha_postage, cha_cool_postage, cha_extra_postage, cha_item_type
	, &in_vip_price, &in_count, &in_reserve_count, &b_reserve, &b_stock, &in_cool, in_subitem);
	if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
	|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%d", in_disp_no + 1);
		Put_Nlist(nlp_temp, "NO", cha_str);
		if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)) {
			in_columns = 6;		/* 6 - rowspan=2 * 1 = 5 <td> */
			if(in_vip_price == 2) {
				in_columns++;
			}
			if(Db_GetValue(dbres, in_subitem, 31) && *Db_GetValue(dbres, in_subitem, 31) == '1') {
				in_columns++;
			}
			if(Db_GetValue(dbres, in_subitem, 41) && *Db_GetValue(dbres, in_subitem, 41) == '1') {
				in_columns++;
			}
			if(Db_GetValue(dbres, in_subitem, 42) && *Db_GetValue(dbres, in_subitem, 42) == '1') {
				in_columns++;
			}
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d", in_columns);
			Put_Nlist(nlp_temp, "COLSPAN", cha_str);
		} else {
			Put_Nlist(nlp_temp, "COLSPAN", "1");
		}
	}
	chp_dir = GetCartImageURL();
	chp_thumbnail = Db_GetValue(dbres, in_base_i, 4) ? Db_GetValue(dbres, in_base_i, 4) : "";
	chp_image = Db_GetValue(dbres, in_base_i, 5) ? Db_GetValue(dbres, in_base_i, 5) : "";
	/* image */
	if(!strcmp(chp_layout, CO_TYPE_B1)  || !strcmp(chp_layout, CO_TYPE_B2)
	   || !strcmp(chp_layout, CO_TYPE_B3)  || !strcmp(chp_layout, CO_TYPE_B4)
	   || !strcmp(chp_layout, CO_TYPE_B5)  || !strcmp(chp_layout, CO_TYPE_B6)
	   || !strcmp(chp_layout, CO_TYPE_B7)  || !strcmp(chp_layout, CO_TYPE_B8)
	   || !strcmp(chp_layout, CO_TYPE_B9)  || !strcmp(chp_layout, CO_TYPE_B10)
	   || !strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_B12)
	   || !strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_E1)) {
		chp_img = chp_thumbnail;
	} else {
		chp_img = chp_image;
	}
	chp_cgipath = Get_Cart_CgiPath();
	chp_defimagepath = Get_Cart_DefImagePath();
	/* html */
	if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
	|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
		if(in_disp_no % 2) {
			chp_value = Db_GetValue(dbres, 0, 18) ? Db_GetValue(dbres, 0, 18) : "";
		} else {
			chp_value = Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : "";
		}
	} else {
		chp_value = Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : "";
	}
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, " bgcolor=\"#%s\"", chp_value);
	Put_Nlist(nlp_temp, "BG_COLOR", cha_str);
	Put_Nlist(nlp_temp, "ITEM_ID", chp_dbase_item_id);
	if(b_link) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "<a href=\"%s?id=%s\">", g_cha_purchase_cgi, chp_dbase_item_id);
		Put_Nlist(nlp_temp, "DISP_ITEM_DETAIL_CGI", cha_str);
	}
	chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1) ? Db_GetValue(dbres, in_base_i, 1) : "");
	Put_Nlist(nlp_temp, "ITEM_NAME", chp_esc);
	if(chp_img != chp_thumbnail) {
		/* st_image */
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, " select T1.c_image");	/* 0 ファイル名 */
		strcat(cha_sql, ", T1.n_width");
		strcat(cha_sql, ", T1.n_height");
		strcat(cha_sql, " from st_image T1");
		sprintf(cha_sql + strlen(cha_sql), " where c_item_id = '%s'", Db_GetValue(dbres, in_subitem, 0));
		strcat(cha_sql, " and T1.c_image is not null");
		strcat(cha_sql, " and T1.c_image <> ''");
		strcat(cha_sql, " order by T1.n_image");
		strcat(cha_sql, " limit 1");
		dbres2 = Db_OpenDyna(db, cha_sql);
		if(!dbres2) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d：query error(%s)", __LINE__, Gcha_last_error);
			Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
			return 1;
		}
		if(Db_GetRowCount(dbres2) && Db_GetValue(dbres2, 0, 0) && *Db_GetValue(dbres2, 0, 0)) {
			chp_img = Db_GetValue(dbres2, 0, 0);
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%s/%s", chp_defimagepath, chp_img);
			Put_Nlist(nlp_temp, "ITEM_IMAGE", cha_str);
			in_width = 0;
			in_height = 0;
			in_image_size = 0;
			chp_value = Db_GetValue(dbres2, 0, 1);
			if(chp_value != NULL && *chp_value != '\0') {
				in_width = atoi(chp_value);
			}
			chp_value = Db_GetValue(dbres2, 0, 2);
			if(chp_value != NULL && *chp_value != '\0') {
				in_height = atoi(chp_value);
			}
			chp_value = Db_GetValue(dbres, in_subitem, 74);	/* 取る場所違う */
			if(chp_value != NULL && *chp_value != '\0') {
				in_image_size = atoi(chp_value);
			}
			memset(cha_sql, '\0', sizeof(cha_sql));
			if(in_image_size && in_width && in_height) {
				Build_Image_Size(cha_sql, in_width, in_height, in_image_size);
			}
			Put_Nlist(nlp_temp, "IMAGE_SIZE", cha_sql);
		} else {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%s/%s", chp_defimagepath, CO_NOT_PICTURE_FILE);
			Put_Nlist(nlp_temp, "ITEM_IMAGE", cha_str);
		}
		Db_CloseDyna(dbres2);
	} else { /* サムネイルは通常表示 */
		if(chp_img && *chp_img != '\0') {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%s/%s", chp_defimagepath, chp_img);
			Put_Nlist(nlp_temp, "ITEM_IMAGE", cha_str);
			in_width = 0;
			in_height = 0;
			in_image_size = 0;
			chp_value = Db_GetValue(dbres, in_subitem, 72);
			if(chp_value != NULL && *chp_value != '\0') {
				in_width = atoi(chp_value);
			}
			chp_value = Db_GetValue(dbres, in_subitem, 73);
			if(chp_value != NULL && *chp_value != '\0') {
				in_height = atoi(chp_value);
			}
			chp_value = Db_GetValue(dbres, in_subitem, 74);
			if(chp_value != NULL && *chp_value != '\0') {
				in_image_size = atoi(chp_value);
			}
			if(!in_width || !in_height) {
				memset(cha_str, '\0', sizeof(cha_str));
				sprintf(cha_str, "%s/%s", GetCartImageDirectory(), chp_img);
				in_file_type = GetImageType_Extension(cha_str);
				GetImageSize(nlp_out, cha_str, &in_width, &in_height, in_file_type);
				if(in_width && in_height) {
					memset(cha_sql, '\0', sizeof(cha_sql));
					strcpy(cha_sql, "update sm_item");
					sprintf(&cha_sql[strlen(cha_sql)], " set n_thumbnail_width = %d", in_width);
					sprintf(&cha_sql[strlen(cha_sql)], ", n_thumbnail_height = %d", in_height);
					chp_value = Escape_SqlString(chp_dbase_item_id);
					sprintf(&cha_sql[strlen(cha_sql)], " where c_item_id = '%s'", chp_value);
					free(chp_value);
					Db_ExecSql(db, cha_sql);
				}
			}
			if(in_image_size && in_width && in_height) {
				memset(cha_sql, '\0', sizeof(cha_sql));
				Build_Image_Size(cha_sql, in_width, in_height, in_image_size);
			}
			Put_Nlist(nlp_temp, "IMAGE_SIZE", cha_sql);
		} else {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%s/%s", chp_defimagepath, CO_NOT_PICTURE_FILE);
			Put_Nlist(nlp_temp, "ITEM_IMAGE", cha_str);
		}
	}
	free(chp_esc);
	chp_value = Db_GetValue(dbres, in_base_i, 6) ? Db_GetValue(dbres, in_base_i, 6) : "";
	chp_maker = Db_GetValue(dbres, in_base_i, 34) ? Db_GetValue(dbres, in_base_i, 34) : "";
	if(chp_maker && *chp_maker != '\0') {
		Put_Nlist(nlp_temp, "MAKER2_START", "<!--MAKER2_START");
		Put_Nlist(nlp_temp, "MAKER2_END", "MAKER2_END-->");
		if(chp_value && *chp_value != '\0' && (strcmp(chp_value, "http://") > 0 || strcmp(chp_value, "https://") > 0)) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "<a href=\"%s\">", chp_value);
			Put_Nlist(nlp_temp, "MAKER_URL", cha_str);
		}
		chp_esc = Escape_HtmlString(chp_maker);
		Put_Nlist(nlp_temp, "MAKER_NAME", chp_esc);
		free(chp_esc);
	} else if(chp_value && *chp_value != '\0' && (strcmp(chp_value, "http://") > 0 || strcmp(chp_value, "https://") > 0)) {
		Put_Nlist(nlp_temp, "MAKER1_START", "<!--MAKER1_START");
		Put_Nlist(nlp_temp, "MAKER1_END", "MAKER1_END-->");
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "<a href=\"%s\">", chp_value);
		Put_Nlist(nlp_temp, "MAKER_URL", cha_str);
	} else {
		Put_Nlist(nlp_temp, "MAKER1_START", "<!--MAKER1_START");
		Put_Nlist(nlp_temp, "MAKER1_END", "MAKER1_END-->");
		Put_Nlist(nlp_temp, "MAKER2_START", "<!--MAKER2_START");
		Put_Nlist(nlp_temp, "MAKER2_END", "MAKER2_END-->");
	}
	if(cha_item_type[0] != '\0') {
		chp_esc = Escape_HtmlString(cha_item_type);
		Put_Nlist(nlp_temp, "MAKER_TYPE", chp_esc);
		free(chp_esc);
	} else {
		Put_Nlist(nlp_temp, "MAKER_TYPE_START", "<!--MAKER_TYPE_START");
		Put_Nlist(nlp_temp, "MAKER_TYPE_END", "MAKER_TYPE_END-->");
	}
	chp_value = Db_GetValue(dbres, 0, 41);
	if(chp_value && *chp_value == '1') {
		if(cha_standard_price[0] != '\0') {
			Put_Nlist(nlp_temp, "STANDARD_PRICE", cha_standard_price);
		} else {
			if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
			|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
				Put_Nlist(nlp_temp, "STANDARD_PRICE", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "STANDARD_PRICE_START", "<!--STANDARD_PRICE_START");
				Put_Nlist(nlp_temp, "STANDARD_PRICE_END", "STANDARD_PRICE_END-->");
			}
		}
	} else {
		Put_Nlist(nlp_temp, "STANDARD_PRICE_START", "<!--STANDARD_PRICE_START");
		Put_Nlist(nlp_temp, "STANDARD_PRICE_END", "STANDARD_PRICE_END-->");
	}
	if(in_vip_price == 2) {
		Put_Nlist(nlp_temp, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
		Put_Nlist(nlp_temp, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
		Put_Nlist(nlp_temp, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
		Put_Nlist(nlp_temp, "ITEM_PRICE", cha_price);
		Put_Nlist(nlp_temp, "VIP_PRICE", cha_member_price);
	} else if(in_vip_price == 1) {
		Put_Nlist(nlp_temp, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
		Put_Nlist(nlp_temp, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
		Put_Nlist(nlp_temp, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE", cha_member_price);
	} else {
		Put_Nlist(nlp_temp, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
		Put_Nlist(nlp_temp, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
		Put_Nlist(nlp_temp, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
		Put_Nlist(nlp_temp, "ITEM_PRICE", cha_price);
	}
	if(Db_GetValue(dbres, 0, 42) && *Db_GetValue(dbres, 0, 42) == '1') {
		if(Db_GetValue(dbres, in_subitem, 43) && *Db_GetValue(dbres, in_subitem, 43) != '\0'  && atoi(Db_GetValue(dbres, in_subitem, 43)) > 0) {
			Put_Nlist(nlp_temp, "POINT", Db_GetValue(dbres, in_subitem, 43));
			Put_Nlist(nlp_temp, "POINT_UNIT", Db_GetValue(dbres, in_subitem, 75));
		} else {
			if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
			|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
				Put_Nlist(nlp_temp, "POINT", "0");
			} else {
				Put_Nlist(nlp_temp, "POINT_START", "<!--POINT_START");
				Put_Nlist(nlp_temp, "POINT_END", "POINT_END-->");
			}
		}
	} else {
		Put_Nlist(nlp_temp, "POINT_START", "<!--POINT_START");
		Put_Nlist(nlp_temp, "POINT_END", "POINT_END-->");
	}
	if(cha_postage[0] != '\0') {
		Put_Nlist(nlp_temp, "POSTAGE", cha_postage);
	} else {
		if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
		|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
			Put_Nlist(nlp_temp, "POSTAGE", "&nbsp;");
		} else {
			Put_Nlist(nlp_temp, "POSTAGE_START", "<!--POSTAGE_START");
			Put_Nlist(nlp_temp, "POSTAGE_END", "POSTAGE_END-->");
		}
	}
	if(cha_cool_postage[0] != '\0') {
		Put_Nlist(nlp_temp, "COOL_POSTAGE", cha_cool_postage);
	} else {
		if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
		|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
			Put_Nlist(nlp_temp, "COOL_POSTAGE", "&nbsp;");
		} else {
			Put_Nlist(nlp_temp, "COOL_POSTAGE_START", "<!--EXTRA_POSTAGE_START");
			Put_Nlist(nlp_temp, "COOL_POSTAGE_END", "EXTRA_POSTAGE_END-->");
		}
	}
	if(cha_extra_postage[0] != '\0') {
		Put_Nlist(nlp_temp, "EXTRA_POSTAGE", cha_extra_postage);
		if(Db_GetValue(dbres, in_subitem, 71)) {	/* 「別途追加送料」名称 */
			chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_subitem, 71));	/* 「別途追加送料」名称 */
			Put_Nlist(nlp_temp, "NAME_SPECIAL", chp_esc);
			free(chp_esc);
		}
	} else {
		if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
		|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
			Put_Nlist(nlp_temp, "EXTRA_POSTAGE", "&nbsp;");
		} else {
			Put_Nlist(nlp_temp, "EXTRA_POSTAGE_START", "<!--EXTRA_POSTAGE_START");
			Put_Nlist(nlp_temp, "EXTRA_POSTAGE_END", "EXTRA_POSTAGE_END-->");
		}
	}
	if(b_stock && Db_GetValue(dbres, 0, 31) && *Db_GetValue(dbres, 0, 31) == '1') {
		if(in_count == 0 && Db_GetValue(dbres, 0, 39) && *Db_GetValue(dbres, 0, 39) == '0') {
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				Put_Nlist(nlp_temp, "STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "STOCK", Db_GetValue(dbres, in_subitem, 44));
			}
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, 0, 39) && *Db_GetValue(dbres, 0, 39) == '1'
		 && in_reserve_count == 0) {
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				Put_Nlist(nlp_temp, "RESERVE_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "RESERVE_STOCK", Db_GetValue(dbres, in_subitem, 45));
			}
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, 0, 39) && *Db_GetValue(dbres, 0, 39) == '1'
		 && in_reserve_count > 0) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d", in_reserve_count);
			Put_Nlist(nlp_temp, "RESERVE_STOCK", cha_str);
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d", in_count);
			Put_Nlist(nlp_temp, "STOCK", cha_str);
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		}
	} else if(b_stock) {
		if(in_count == 0 && Db_GetValue(dbres, 0, 39) && *Db_GetValue(dbres, 0, 39) == '0') {
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				Put_Nlist(nlp_temp, "UNDISP_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "UNDISP_STOCK", Db_GetValue(dbres, in_subitem, 44));
			}
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, 0, 39) && *Db_GetValue(dbres, 0, 39) == '1'
		 && in_reserve_count == 0) {
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK", Db_GetValue(dbres, in_subitem, 45));
			}
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, 0, 39) && *Db_GetValue(dbres, 0, 39) == '1'
		 && in_reserve_count > 0) {
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
		} else {
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		}
	} else {
		if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
		|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
			chp_value = Db_GetValue(dbres, 0, 31);
			if(chp_value && *chp_value == '1') {
				Put_Nlist(nlp_temp, "STOCK", "&nbsp;");
				Put_Nlist(nlp_temp, "UNDISP_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
				Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
				Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
				Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			}
		} else {
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
		}
		Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
		Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
		Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
		Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
		Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
		Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
	}
	chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
	if(!chp_return_cgi_id) {
		chp_return_cgi_id = "";
	}
	b_review = atoi(Db_GetValue(dbres, 0, 46) ? Db_GetValue(dbres, 0, 46) : "");
	in_login = atoi(Db_GetValue(dbres, 0, 37) ? Db_GetValue(dbres, 0, 37) : "");
	/* 購入可能判定 */
	if(!b_stock || in_count > 0 || (b_reserve && in_reserve_count > 0)) {
		Put_Nlist(nlp_temp, "PURCHASE_ITEM_CGI", g_cha_purchase_cgi);
		Build_Hidden(nlp_temp, "HIDDEN", "ITEMID", chp_dbase_item_id);
		Build_Hidden(nlp_temp, "HIDDEN", "RETURN_CGI", chp_return_cgi_id);
		/* カートに入れる */
		Put_Nlist(nlp_temp, "CART_ACTION_NAME", "");
	} else {
		Put_Nlist(nlp_temp, "BUTTON_BUY_START", "<!--BUTTON_BUY_START");
		Put_Nlist(nlp_temp, "BUTTON_BUY_END", "BUTTON_BUY_END-->");
	}
	/* 説明 */
	if(!strcmp(chp_layout, CO_TYPE_A1)     || !strcmp(chp_layout, CO_TYPE_A2)
	   || !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)
	   || !strcmp(chp_layout, CO_TYPE_B1)  || !strcmp(chp_layout, CO_TYPE_B2)
	   || !strcmp(chp_layout, CO_TYPE_B3)  || !strcmp(chp_layout, CO_TYPE_B4)
	   || !strcmp(chp_layout, CO_TYPE_B5)  || !strcmp(chp_layout, CO_TYPE_B6)
	   || !strcmp(chp_layout, CO_TYPE_B7)  || !strcmp(chp_layout, CO_TYPE_B8)
	   || !strcmp(chp_layout, CO_TYPE_B9)  || !strcmp(chp_layout, CO_TYPE_B10)
	   || !strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_B12)
	   || !strcmp(chp_layout, CO_TYPE_D1)  || !strcmp(chp_layout, CO_TYPE_E1)) {
		chp_value = Db_GetValue(dbres, in_base_i, 2);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_temp, "GUIDE", chp_conv);
			free(chp_conv);
		} else {
			Put_Nlist(nlp_temp, "GUIDE", "&nbsp;");
		}
	/* 詳細説明*/
	} else {
		chp_value = Db_GetValue(dbres, in_base_i, 3);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_temp, "GUIDE_DETAIL", chp_conv);
			free(chp_conv);
		} else {
			Put_Nlist(nlp_temp, "GUIDE_DETAIL", "&nbsp;");
		}
	}
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, "%s/%s", g_cha_user_skeleton, chp_skl);
	chp_value = Setpage_Out_Mem_To_Euc(nlp_temp, cha_str);
	Finish_Nlist(nlp_temp);
	if(!chp_value) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "Can't stat skeleton(%s)<br>\n", chp_skl);
		Put_Nlist(nlp_out, "ERROR", cha_str);
		return 1;
	}
	Put_Nlist(nlp_out, "ARTICLE", chp_value);
	free(chp_value);
	*inp_i = i;
	return 0;
}

/*
+* ========================================================================
 * Function:		バリエーションセレクトボックス作成
 * Include-file:	disp.h
 * Description:
 *
%* ========================================================================
 * Return:	なし
-* ======================================================================*/
void build_variation(DBRes *dbres, NLIST *nlp_in, char *chp_variation, int *inp_subitem, int *inp_count, int in_row, int in_itemid_row)
{
	char *chp_esc;
	int in_subitem;
	int in_flg;
	int i;

	in_subitem = in_itemid_row;
	i = in_itemid_row;
	in_flg = 0;
	while(1) {
		if(!strcmp(Db_GetValue(dbres, in_itemid_row, 0) ? Db_GetValue(dbres, in_itemid_row, 0) : "", Db_GetValue(dbres, i, 0) ? Db_GetValue(dbres, i, 0) : "")) {
			if(in_flg == 0 && strcmp(Db_GetValue(dbres, i, 38) ? Db_GetValue(dbres, i, 38) : "", Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "") != 0) {
				sprintf(chp_variation,
					"<select onchange = \"javascript:CallCGI('%s')\" name=\"SUBITEMID\">\n"
					, Db_GetValue(dbres, in_itemid_row, 0) ? Db_GetValue(dbres, in_itemid_row, 0) : ""
				);
				in_flg = 1;
			}
			/* バリエーション有り（選択時） */
			if(Get_Nlist(nlp_in, "item", 1) && Get_Nlist(nlp_in, "subitem", 1)
			&& !strcmp(Get_Nlist(nlp_in, "item", 1), Db_GetValue(dbres, i, 0) ? Db_GetValue(dbres, i, 0) : "")
			&& !strcmp(Get_Nlist(nlp_in, "subitem", 1), Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "")) {
				chp_esc = Escape_HtmlString(Db_GetValue(dbres, i, 12) ? Db_GetValue(dbres, i, 12) : "");
				sprintf(&chp_variation[strlen(chp_variation)],
					"<option value=\"%s\" selected>%s</option>\n"
						, Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "", chp_esc
				);
				free(chp_esc);
				in_subitem = i;
				if(in_row == i + 1) {
					strcat(chp_variation, "</select>\n");
					break;
				}
				i++;
			/* バリエーション有り */
			} else if(strcmp(Db_GetValue(dbres, i, 38) ? Db_GetValue(dbres, i, 38) : "", Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "") != 0) {
				chp_esc = Escape_HtmlString(Db_GetValue(dbres, i, 12) ? Db_GetValue(dbres, i, 12) : "");
				sprintf(&chp_variation[strlen(chp_variation)],
					"<option value=\"%s\">%s</option>\n"
					, Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "", chp_esc
				);
				free(chp_esc);
				if(in_row == i + 1) {
					strcat(chp_variation, "</select>\n");
					break;
				}
				i++;
			/* バリエーション無し */
			} else {
				break;
			}
		/* 次の商品 */
		} else {
			strcat(chp_variation, "</select>\n");
			i--;
			break;
		}
	}
	*inp_subitem = in_subitem;
	*inp_count = i;
	return;
}

/*
+* ========================================================================
 * Function:		バリエーションラジオボタン作成(f13)
 * Include-file:	disp.h
 * Description:
%* ========================================================================
 * Return:	なし
-* ======================================================================*/
static void put_variation_f13(
DBRes *dbres
, NLIST *nlp_in
, NLIST *nlp_out
, int *inp_subitem
, int *inp_count
, int in_row
, int in_itemid_row
, char *chp_script
)
{
	char *chp_dbase_item_id;
	char *chp_dbase_sub_id;
	char *chp_nlist_item_id;
	char *chp_nlist_sub_id;
	char *chp_esc;
	char *chp_tmp;
	char cha_str[1024];
	char cha_standard_price[128];
	char cha_price[128];
	char cha_member_price[128];
	char cha_postage[128];
	char cha_cool_postage[128];
	char cha_extra_postage[128];
	char cha_item_type[128];
	int in_vip_price;
	int in_count;
	int in_reserve_count;
	int in_b_reserve;
	int in_b_stock;
	int in_subitem;
	int in_cool;
	int in_flg;
	int i;

	in_subitem = in_itemid_row;
	i = in_itemid_row;
	chp_nlist_item_id = Get_Nlist(nlp_in, "item", 1);
	chp_nlist_sub_id = Get_Nlist(nlp_in, "subitem", 1);
	in_flg = 0;
	while(1) {
		chp_dbase_item_id = Db_GetValue(dbres, i, 0) ? Db_GetValue(dbres, i, 0) : "";
		if(!strcmp(Db_GetValue(dbres, in_itemid_row, 0) ? Db_GetValue(dbres, in_itemid_row, 0) : "", chp_dbase_item_id)) {
			chp_dbase_sub_id = Db_GetValue(dbres, i, 11) ? Db_GetValue(dbres, i, 11) : "";
			if(in_flg == 0 && strcmp(chp_dbase_item_id, chp_dbase_sub_id) != 0) {
				in_flg = 1;
			}
			Get_Item_Data(dbres, cha_standard_price, cha_price, cha_member_price
				      , cha_postage, cha_cool_postage, cha_extra_postage, cha_item_type
				      , &in_vip_price, &in_count, &in_reserve_count, &in_b_reserve, &in_b_stock, &in_cool, i);
			/* バリエーション有り（選択時） */
			if((in_flg && chp_nlist_item_id && chp_nlist_sub_id && !strcmp(chp_nlist_item_id, chp_dbase_item_id) && !strcmp(chp_nlist_sub_id, chp_dbase_sub_id)) || (in_flg && i == in_itemid_row && !chp_nlist_sub_id)) {
				memset(cha_str, '\0', sizeof(cha_str));
				if(!strcmp(chp_script, CO_ACTION_DISP_NEW) || !strcmp(chp_script, CO_ACTION_DISP_HIT)) {
					Put_Nlist(nlp_out, "NORMAL_VARIATION_START", "<!--");
					Put_Nlist(nlp_out, "NORMAL_VARIATION_END", "-->");
					Build_Hidden(nlp_out, "HIDDEN", "SUBITEMID", chp_dbase_sub_id);
				} else {
					sprintf(cha_str, "<input type=\"radio\" name=\"SUBITEMID\" checked value=\"%s\">", chp_dbase_sub_id);
				}
				in_subitem = i;
			/* バリエーション有り */
			} else if(in_flg && strcmp(chp_dbase_item_id, chp_dbase_sub_id) != 0) {
				memset(cha_str, '\0', sizeof(cha_str));
				if(!strcmp(chp_script, CO_ACTION_DISP_NEW) || !strcmp(chp_script, CO_ACTION_DISP_HIT)) {
					Put_Nlist(nlp_out, "NORMAL_VARIATION_START", "<!--");
					Put_Nlist(nlp_out, "NORMAL_VARIATION_END", "-->");
					Build_Hidden(nlp_out, "HIDDEN", "SUBITEMID", chp_dbase_sub_id);
				} else {
					sprintf(cha_str, "<input type=\"radio\" name=\"SUBITEMID\" value=\"%s\">", chp_dbase_sub_id);
				}
			/* バリエーション無し */
			} else {
				Put_Nlist(nlp_out, "NORMAL_VARIATION_START", "<!--");
				Put_Nlist(nlp_out, "NORMAL_VARIATION_END", "-->");
				chp_tmp = Db_GetValue(dbres, i, 1);
				if(chp_tmp && *chp_tmp) {
					chp_esc = Escape_HtmlString(chp_tmp);
					Put_Nlist(nlp_out, "ITEM_DIRECTION", chp_esc);
					Put_Nlist(nlp_out, "ITEM_DIRECTION", "&nbsp;");
					free(chp_esc);
				}
				memset(cha_str, '\0', sizeof(cha_str));
				sprintf(cha_str, "<font color=\"#%s\">", Db_GetValue(dbres, i, 51) ? Db_GetValue(dbres, i, 51) : "");
				if(in_vip_price == 2) {
					sprintf(cha_str + strlen(cha_str), "￥%s(会員価格￥&nbsp;%s)"
						, cha_price, cha_member_price);
				} else if(in_vip_price == 1) {
					sprintf(cha_str + strlen(cha_str), "￥%s", cha_member_price);
				} else {
					sprintf(cha_str + strlen(cha_str), "￥%s", cha_price);
				}
				sprintf(cha_str + strlen(cha_str), "</font>");
				Put_Nlist(nlp_out, "ITEM_DIRECTION", cha_str);
				if(cha_item_type[0] != '\0') {
					memset(cha_str, '\0', sizeof(cha_str));
					chp_esc = Escape_HtmlString(cha_item_type);
					sprintf(cha_str, "&nbsp;(%s)", cha_item_type);
					free(chp_esc);
					Put_Nlist(nlp_out, "ITEM_DIRECTION", cha_str);
				}
				break;
			}
			Put_Nlist(nlp_out, "ITEM_VARIATION", cha_str);
			chp_tmp = Db_GetValue(dbres, i, 12);
			if(chp_tmp && *chp_tmp) {
				chp_esc = Escape_HtmlString(chp_tmp);
				Put_Nlist(nlp_out, "ITEM_VARIATION", chp_esc);
				Put_Nlist(nlp_out, "ITEM_DIRECTION", chp_esc);
				Put_Nlist(nlp_out, "ITEM_DIRECTION", "&nbsp;");
				free(chp_esc);
			}
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "<font color=\"#%s\">", Db_GetValue(dbres, i, 51) ? Db_GetValue(dbres, i, 51) : "");
			if(in_vip_price == 2) {
				sprintf(cha_str + strlen(cha_str), "￥%s(会員価格￥&nbsp;%s)"
					, cha_price, cha_member_price);
			} else if(in_vip_price == 1) {
				sprintf(cha_str + strlen(cha_str), "￥%s", cha_member_price);
			} else {
				sprintf(cha_str + strlen(cha_str), "￥%s", cha_price);
			}
			sprintf(cha_str + strlen(cha_str), "</font>");
			Put_Nlist(nlp_out, "ITEM_DIRECTION", cha_str);
			if(cha_item_type[0] != '\0') {
				memset(cha_str, '\0', sizeof(cha_str));
				chp_esc = Escape_HtmlString(cha_item_type);
				sprintf(cha_str, "&nbsp;(%s)", cha_item_type);
				free(chp_esc);
				Put_Nlist(nlp_out, "ITEM_DIRECTION", cha_str);
			}
			if(in_row == i + 1) {
				break;
			} else {
				Put_Nlist(nlp_out, "ITEM_VARIATION", "&nbsp;");
				Put_Nlist(nlp_out, "ITEM_DIRECTION", "、&nbsp;");
			}
			i++;
		/* 次の商品 */
		} else if(in_flg) {
			i--;
			break;
		}
	}
	*inp_subitem = in_subitem;
	*inp_count = i;
	return;
}

/*
+* ------------------------------------------------------------------------
 * Function:		put_item_info_f13
 * Include-file:	disp.h
 * Description:
 *	商品情報を作成する。F13用
 %* ------------------------------------------------------------------------
 *			0:正常終了
 *			1:エラー
-* ------------------------------------------------------------------------
 */
static int put_item_info_f13(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_script
, char *chp_skl
, int *inp_i
, bool b_link
, int in_row
, int in_disp_no	/* typeA用 */
) {
	NLIST *nlp_skl;
//	NLIST *nlp_cookie;
	char *chp_dir;
	char *chp_img;
	char *chp_cgipath;
	char *chp_defimagepath;
	char *chp_thumbnail;
	char *chp_image;
	char *chp_layout;
	char *chp_return_cgi_id;
	char *chp_value;
	char *chp_conv;
	char *chp_maker;
	char *chp_esc;
	char cha_str[2048];
	char cha_standard_price[64];
	char cha_price[64];
	char cha_member_price[64];
	char cha_postage[64];
	char cha_cool_postage[64];
	char cha_extra_postage[64];
	char cha_item_type[64];
	const int in_base_i = *inp_i;
//	bool b_review;
	int in_vip_price;
	int in_count;
	int in_reserve_count;
	int in_b_reserve;
	int in_b_stock;
	int in_subitem;
	int in_cool;
//	int in_ret;
//	int in_login;
	int i;

	nlp_skl = Init_Nlist();
	/* format */
	Put_Nlist(nlp_skl, "BORDER", Db_GetValue(dbres, in_base_i, 20) ? Db_GetValue(dbres, in_base_i, 20) : "");
	Put_Nlist(nlp_skl, "BORDERCOLOR", Db_GetValue(dbres, in_base_i, 19) ? Db_GetValue(dbres, in_base_i, 19) : "");
	Put_Nlist(nlp_skl, "BGCOLOR", Db_GetValue(dbres, in_base_i, 17) ? Db_GetValue(dbres, in_base_i, 17) : "");
	Put_Nlist(nlp_skl, "BGCOLOR2", Db_GetValue(dbres, in_base_i, 18) ? Db_GetValue(dbres, in_base_i, 18) : "");
	Put_Nlist(nlp_skl, "WIDTH", Db_GetValue(dbres, in_base_i, 52) ?  Db_GetValue(dbres, in_base_i, 52) : "");
	Put_Nlist(nlp_skl, "BORDER2", Db_GetValue(dbres, in_base_i, 54) ?  Db_GetValue(dbres, in_base_i, 54) : "");
	Put_Nlist(nlp_skl, "CELLPADDING", Db_GetValue(dbres, in_base_i, 55) ?  Db_GetValue(dbres, in_base_i, 55) : "");
	chp_layout = Db_GetValue(dbres, in_base_i, 16) ? Db_GetValue(dbres, in_base_i, 16) : "";
	i = in_base_i;
	in_subitem = in_base_i;
	put_variation_f13(dbres, nlp_in, nlp_skl, &in_subitem, &i, in_row, in_base_i, chp_script);
//	Disp_Item_Variation_F13(db, nlp_in, nlp_skl, "OPTIONS", Db_GetValue(dbres, in_base_i, 38) ? Db_GetValue(dbres, in_base_i, 38) : "", Db_GetValue(dbres, in_base_i, 17) ? Db_GetValue(dbres, in_base_i, 17) : "", 0);
	memset(cha_standard_price, '\0', sizeof(cha_standard_price));
	memset(cha_price, '\0', sizeof(cha_price));
	memset(cha_member_price, '\0', sizeof(cha_member_price));
	memset(cha_postage, '\0', sizeof(cha_postage));
	memset(cha_extra_postage, '\0', sizeof(cha_extra_postage));
	memset(cha_item_type, '\0', sizeof(cha_item_type));		/* 未使用 */
	in_vip_price = 0;
	in_count = 0;
	in_reserve_count = 0;
	in_b_reserve = 0;
	in_b_stock = 0;
	Get_Item_Data(dbres, cha_standard_price, cha_price, cha_member_price, cha_postage, cha_cool_postage, cha_extra_postage, cha_item_type
	, &in_vip_price, &in_count, &in_reserve_count, &in_b_reserve, &in_b_stock, &in_cool, in_subitem);
	chp_dir = GetCartImageURL();
	chp_thumbnail = Db_GetValue(dbres, in_base_i, 4) ? Db_GetValue(dbres, in_base_i, 4) : "";
	chp_image = Db_GetValue(dbres, in_base_i, 5) ? Db_GetValue(dbres, in_base_i, 5) : "";
	/* image */
	if(!strcmp(chp_layout, CO_TYPE_B1)  || !strcmp(chp_layout, CO_TYPE_B2)
	   || !strcmp(chp_layout, CO_TYPE_B3)  || !strcmp(chp_layout, CO_TYPE_B4)
	   || !strcmp(chp_layout, CO_TYPE_B5)  || !strcmp(chp_layout, CO_TYPE_B6)
	   || !strcmp(chp_layout, CO_TYPE_B7)  || !strcmp(chp_layout, CO_TYPE_B8)
	   || !strcmp(chp_layout, CO_TYPE_B9)  || !strcmp(chp_layout, CO_TYPE_B10)
	   || !strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_B12)
	   || !strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_E1)) {
		chp_img = chp_thumbnail;
	} else {
		chp_img = chp_image;
	}
	chp_cgipath = Get_Cart_CgiPath();
	chp_defimagepath = Get_Cart_DefImagePath();
	Put_Nlist(nlp_skl, "ITEM_ID", Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
	if(b_link) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "<a href=\"%s/%s?id=%s\">", chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
		Put_Nlist(nlp_skl, "DISP_ITEM_DETAIL_CGI", cha_str);
		Put_Nlist(nlp_skl, "DISP_ITEM_DETAIL_CGI_END", "</a>");
	}
	chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1) ? Db_GetValue(dbres, in_base_i, 1) : "");
	Put_Nlist(nlp_skl, "ITEM_NAME", chp_esc);
	free(chp_esc);
	chp_value = Db_GetValue(dbres, in_base_i, 6) ? Db_GetValue(dbres, in_base_i, 6) : "";
	chp_maker = Db_GetValue(dbres, in_base_i, 34);
	if(chp_maker && *chp_maker != '\0') {
		Put_Nlist(nlp_skl, "MAKER2START", "<!--MAKER2START");
		Put_Nlist(nlp_skl, "MAKER2END", "MAKER2END-->");
		if(chp_value && *chp_value != '\0' && strcmp(chp_value, "http://") > 0) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "<a href=\"%s\">", chp_value);
			Put_Nlist(nlp_skl, "MAKER_URL", cha_str);
		}
		chp_esc = Escape_HtmlString(chp_maker);
		Put_Nlist(nlp_skl, "MAKER_NAME", chp_esc);
		free(chp_esc);
	} else if(chp_value && *chp_value != '\0' && strcmp(chp_value, "http://") > 0) {
		Put_Nlist(nlp_skl, "MAKER1START", "<!--MAKER1START");
		Put_Nlist(nlp_skl, "MAKER1END", "MAKER1END-->");
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "<a href=\"%s\">", chp_value);
		Put_Nlist(nlp_skl, "MAKER_URL", cha_str);
	} else {
		Put_Nlist(nlp_skl, "MAKER1START", "<!--MAKER1START");
		Put_Nlist(nlp_skl, "MAKER1END", "MAKER1END-->");
		Put_Nlist(nlp_skl, "MAKER2START", "<!--MAKER2START");
		Put_Nlist(nlp_skl, "MAKER2END", "MAKER2END-->");
	}
	if(cha_item_type[0] != '\0') {
		chp_esc = Escape_HtmlString(cha_item_type);
		Put_Nlist(nlp_skl, "MAKER_TYPE", chp_esc);
		free(chp_esc);
	} else {
		Put_Nlist(nlp_skl, "MAKER_TYPE_START", "<!--MAKER_TYPE_START");
		Put_Nlist(nlp_skl, "MAKER_TYPE_END", "MAKER_TYPE_END-->");
	}
	chp_value = Db_GetValue(dbres, i, 41);
	if(chp_value && *chp_value == '1' && cha_standard_price[0] != '\0') {
		Put_Nlist(nlp_skl, "STANDARD_PRICE", cha_standard_price);
	} else {
		Put_Nlist(nlp_skl, "STANDARD_PRICE_START", "<!--STANDARD_PRICE_START");
		Put_Nlist(nlp_skl, "STANDARD_PRICE_END", "STANDARD_PRICE_END-->");
	}
	if(in_vip_price == 2) {
		Put_Nlist(nlp_skl, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
		Put_Nlist(nlp_skl, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
		Put_Nlist(nlp_skl, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
		Put_Nlist(nlp_skl, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
		Put_Nlist(nlp_skl, "ITEM_PRICE", cha_price);
		Put_Nlist(nlp_skl, "VIP_PRICE", cha_member_price);
	} else if(in_vip_price == 1) {
		Put_Nlist(nlp_skl, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
		Put_Nlist(nlp_skl, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
		Put_Nlist(nlp_skl, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
		Put_Nlist(nlp_skl, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
		Put_Nlist(nlp_skl, "VIP_PRICE", cha_member_price);
	} else {
		Put_Nlist(nlp_skl, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
		Put_Nlist(nlp_skl, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
		Put_Nlist(nlp_skl, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
		Put_Nlist(nlp_skl, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
		Put_Nlist(nlp_skl, "ITEM_PRICE", cha_price);
	}
	if(Db_GetValue(dbres, in_subitem, 42) && *Db_GetValue(dbres, in_subitem, 42) == '1'
	 && Db_GetValue(dbres, in_subitem, 43) && *Db_GetValue(dbres, in_subitem, 43) != '\0'  && atoi(Db_GetValue(dbres, in_subitem, 43)) > 0) {
		Put_Nlist(nlp_skl, "POINT", Db_GetValue(dbres, in_subitem, 43));
	} else {
		Put_Nlist(nlp_skl, "POINT_START", "<!--POINT_START");
		Put_Nlist(nlp_skl, "POINT_END", "POINT_END-->");
	}
	if(cha_postage[0] != '\0') {
		Put_Nlist(nlp_skl, "POSTAGE", cha_postage);
	} else {
		Put_Nlist(nlp_skl, "POSTAGE_START", "<!--POSTAGE_START");
		Put_Nlist(nlp_skl, "POSTAGE_END", "POSTAGE_END-->");
	}
	if(cha_extra_postage[0] != '\0') {
		Put_Nlist(nlp_skl, "EXTRA_POSTAGE", cha_extra_postage);
	} else {
		Put_Nlist(nlp_skl, "EXTRA_POSTAGE_START", "<!--EXTRA_POSTAGE_START");
		Put_Nlist(nlp_skl, "EXTRA_POSTAGE_END", "EXTRA_POSTAGE_END-->");
	}
	if(in_b_stock == 1
	&& Db_GetValue(dbres, in_base_i, 31) && *Db_GetValue(dbres, in_base_i, 31) == '1') {
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				Put_Nlist(nlp_skl, "STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_skl, "STOCK", Db_GetValue(dbres, in_subitem, 44));
			}
			Put_Nlist(nlp_skl, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				Put_Nlist(nlp_skl, "RESERVE_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_skl, "RESERVE_STOCK", Db_GetValue(dbres, in_subitem, 45));
			}
			Put_Nlist(nlp_skl, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_skl, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d", in_reserve_count);
			Put_Nlist(nlp_skl, "RESERVE_STOCK", cha_str);
			Put_Nlist(nlp_skl, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_skl, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d", in_count);
			Put_Nlist(nlp_skl, "STOCK", cha_str);
			Put_Nlist(nlp_skl, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		}
	} else if(in_b_stock == 1) {
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				Put_Nlist(nlp_skl, "UNDISP_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_skl, "UNDISP_STOCK", Db_GetValue(dbres, in_subitem, 44));
			}
			Put_Nlist(nlp_skl, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_skl, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK", Db_GetValue(dbres, in_subitem, 45));
			}
			Put_Nlist(nlp_skl, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_skl, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			Put_Nlist(nlp_skl, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_skl, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
		} else {
			Put_Nlist(nlp_skl, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_skl, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		}
	} else {
		Put_Nlist(nlp_skl, "STOCK_START", "<!--STOCK_START");
		Put_Nlist(nlp_skl, "STOCK_END", "STOCK_END-->");
		Put_Nlist(nlp_skl, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
		Put_Nlist(nlp_skl, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
		Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
		Put_Nlist(nlp_skl, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
		Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
		Put_Nlist(nlp_skl, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
	}
	/* 彫刻内容 */
	chp_value = Db_GetValue(dbres, in_base_i, 56);
	if(!chp_value || atoi(chp_value) == 0) {
		Put_Nlist(nlp_skl, "DISP_CARVING_WORD_START", "<!--DISP_CARVING_WORD_START");
		Put_Nlist(nlp_skl, "DISP_CARVING_WORD_END", "DISP_CARVING_WORD_END-->");
	} else {
		chp_value = Get_Nlist(nlp_in, "carving_word", 1);
		if(chp_value && *chp_value && Get_Nlist(nlp_in, "item", 1)
		&& strcmp(Get_Nlist(nlp_in, "item", 1), Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "") == 0) {
			chp_esc = Escape_HtmlString(chp_value);
			Put_Nlist(nlp_skl, "CARVING_WORD", chp_esc);
			free(chp_esc);
		}
	}
	chp_value = Get_Nlist(nlp_in, "demand", 1);
	if(chp_value && *chp_value && Get_Nlist(nlp_in, "item", 1) && Db_GetValue(dbres, in_base_i, 0)
	&& strcmp(Get_Nlist(nlp_in, "item", 1), Db_GetValue(dbres, in_base_i, 0)) == 0) {
		chp_esc = Escape_HtmlString(chp_value);
		Put_Nlist(nlp_skl, "DEMAND", chp_esc);
		free(chp_esc);
	}
	chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
	if(!chp_return_cgi_id) {
		chp_return_cgi_id = "";
	}
	/* 買い物篭を見るボタン */
	Put_Nlist(nlp_skl, "PURCHASE_ITEM_CGI", g_cha_purchase_cgi);
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, "%s/%s", chp_cgipath, CO_ACTION_PURCHASE_LIST);
	Put_Nlist(nlp_skl, "PURCHASE_ITEM_LIST", cha_str);
	Build_Hidden(nlp_skl, "HIDDEN", "ITEMID", Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
	Build_Hidden(nlp_skl, "HIDDEN", "RETURN_CGI", chp_return_cgi_id);
	if(!strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL)) {
		/* 問い合わせボタン */
		if(Db_GetValue(dbres, in_base_i, 47) && *Db_GetValue(dbres, in_base_i, 47) == '1') {
			if(Get_Nlist(nlp_skl, "MAKER_URL", 1)) {
				Put_Nlist(nlp_skl, "SEPARATOR_START", "<!--SEPARATOR_START");
				Put_Nlist(nlp_skl, "SEPARATOR_END", "SEPARATOR_END-->");
			}
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, " onclick=\"location.href='%s/%s?ITEMID=%s&SUBITEMID=%s'\""
					, chp_cgipath, CO_ACTION_INQUIRE_ITEM, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", Db_GetValue(dbres, in_subitem, 11) ? Db_GetValue(dbres, in_subitem, 11) : "");
			Put_Nlist(nlp_skl, "INQUIRE_ACTION", cha_str);
		} else {
			Put_Nlist(nlp_skl, "INQUIRE_START", "<!--INQUIRE_START");
			Put_Nlist(nlp_skl, "INQUIRE_END", "INQUIRE_END-->");
		}
#if 0
		/* レビューを書くボタン */
		nlp_cookie = Get_Cookie();
		in_login = Is_Logged_In(db, nlp_in, nlp_cookie);
		if(in_login == 1) {
			in_ret = Manage_Review_Write(db, nlp_in, NULL, nlp_cookie, NULL);
			if(in_ret) {
				b_review = 0;
			} else {
				b_review = 1;
			}
		} else {
			b_review = 0;
		}
		Finish_Nlist(nlp_cookie);
#endif
		if(Db_GetValue(dbres, in_base_i, 46) && *Db_GetValue(dbres, in_base_i, 46) == '1') {
/*
			if(b_review == 1 && Db_GetValue(dbres, in_base_i, 37) && *Db_GetValue(dbres, in_base_i, 37) == '1') {
				Put_Nlist(nlp_skl, "REVIEW_NO_START", "<!--REVIEW_NO_START");
				Put_Nlist(nlp_skl, "REVIEW_NO_END", "REVIEW_NO_END-->");
			} else {
*/
				Put_Nlist(nlp_skl, "REVIEW_OK_START", "<!--REVIEW_OK_START");
				Put_Nlist(nlp_skl, "REVIEW_OK_END", "REVIEW_OK_END-->");
/*
			}
*/
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, " onclick=\"location.href='%s/%s?ITEMID=%s&SUBITEMID=%s'\""
				, chp_cgipath, CO_ACTION_REVIEW_ITEM, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", Db_GetValue(dbres, in_subitem, 11) ? Db_GetValue(dbres, in_subitem, 11) : "");
			Put_Nlist(nlp_skl, "REVIEW_ACTION", cha_str);
		} else {
			Put_Nlist(nlp_skl, "REVIEW_OK_START", "<!--REVIEW_OK_START");
			Put_Nlist(nlp_skl, "REVIEW_OK_END", "REVIEW_OK_END-->");
			Put_Nlist(nlp_skl, "REVIEW_NO_START", "<!--REVIEW_NO_START");
			Put_Nlist(nlp_skl, "REVIEW_NO_END", "REVIEW_NO_END-->");
		}
	}
	/* 説明 */
	if(!strcmp(chp_layout, CO_TYPE_A1)     || !strcmp(chp_layout, CO_TYPE_A2)
	   || !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)
	   || !strcmp(chp_layout, CO_TYPE_B1)  || !strcmp(chp_layout, CO_TYPE_B2)
	   || !strcmp(chp_layout, CO_TYPE_B3)  || !strcmp(chp_layout, CO_TYPE_B4)
	   || !strcmp(chp_layout, CO_TYPE_B5)  || !strcmp(chp_layout, CO_TYPE_B6)
	   || !strcmp(chp_layout, CO_TYPE_B7)  || !strcmp(chp_layout, CO_TYPE_B8)
	   || !strcmp(chp_layout, CO_TYPE_B9)  || !strcmp(chp_layout, CO_TYPE_B10)
	   || !strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_B12)
	   || !strcmp(chp_layout, CO_TYPE_D1)  || !strcmp(chp_layout, CO_TYPE_E1)) {
		chp_value = Db_GetValue(dbres, in_base_i, 2);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_skl, "GUIDE", chp_conv);
			free(chp_conv);
		}
	/* 詳細説明*/
	} else {
		chp_value = Db_GetValue(dbres, in_base_i, 3);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_skl, "GUIDE_DETAIL", chp_conv);
			free(chp_conv);
		}
	}
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, "%s/%s", GetCartSkeletonDirectory(), CO_F13_ITEM_ORDER_TABLE);
	chp_value = Setpage_Out_Mem_To_Euc(nlp_skl, cha_str);
	Finish_Nlist(nlp_skl);
	if(!chp_value) {
		return 1;
	}
	nlp_skl = Init_Nlist();
	if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
	|| !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%d", in_disp_no + 1);
		Put_Nlist(nlp_skl, "NO", cha_str);
		if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)) {
			Put_Nlist(nlp_skl, "COLSPAN", "2");
		} else {
			Put_Nlist(nlp_skl, "COLSPAN", "1");
		}
	}
	Put_Nlist(nlp_skl, "DISP", chp_value);
	free(chp_value);
	/* 設定 */
	memset(cha_str, '\0', sizeof(cha_str));
	chp_value = Db_GetValue(dbres, in_base_i, 52);
	if(chp_value && *chp_value != '\0') {
		sprintf(cha_str, " width=\"%s\"", chp_value);
	} else {
		strcpy(cha_str, " width=\"100%\"");
	}
	Put_Nlist(nlp_skl, "TABLE_WIDTH", cha_str);
	chp_value = Db_GetValue(dbres, in_base_i, 53);
	if(chp_value && *chp_value != '\0') {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " height=\"%s\"", chp_value);
		Put_Nlist(nlp_skl, "TABLE_HEIGHT", cha_str);
	} else {
		/* empty is old format */
	}
	Put_Nlist(nlp_skl, "BORDER2", Db_GetValue(dbres, in_base_i, 54) ? Db_GetValue(dbres, in_base_i, 54) : "");
	Put_Nlist(nlp_skl, "CELLPADDING", Db_GetValue(dbres, in_base_i, 55) ? Db_GetValue(dbres, in_base_i, 55) : "");
	if(chp_img && *chp_img != '\0') {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%s/%s", chp_defimagepath, chp_img);
	} else {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%s/%s", chp_dir, CO_NOT_PICTURE_FILE);
	}
	Put_Nlist(nlp_skl, "ITEM_IMAGE", cha_str);
	if(b_link) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "<a href=\"%s/%s?id=%s\">", chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
		Put_Nlist(nlp_skl, "DISP_ITEM_DETAIL_CGI", cha_str);
	}
	chp_layout = Db_GetValue(dbres, in_base_i, 16);
	if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_D2) || !strcmp(chp_layout, CO_TYPE_E1) || !strcmp(chp_layout, CO_TYPE_E2)) {
		/* empty */
	} else {
		chp_value = Db_GetValue(dbres, in_base_i, 52);
		if(!chp_value) {
			chp_value = "";
		}
		Put_Nlist(nlp_skl, "WIDTH", chp_value);
		chp_value = Db_GetValue(dbres, in_base_i, 53);
		if(!chp_value) {
			chp_value = "";
		}
		Put_Nlist(nlp_skl, "HEIGHT", chp_value);
	}
	Put_Nlist(nlp_skl, "BORDER2", Db_GetValue(dbres, in_base_i, 54) ? Db_GetValue(dbres, in_base_i, 54) : "");
	Put_Nlist(nlp_skl, "CELLPADDING", Db_GetValue(dbres, in_base_i, 55) ? Db_GetValue(dbres, in_base_i, 55) : "");
	/* html */
	if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2) || !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
		if(in_disp_no % 2) {
			chp_value = Db_GetValue(dbres, in_base_i, 18) ? Db_GetValue(dbres, in_base_i, 18) : "";
		} else {
			chp_value = Db_GetValue(dbres, in_base_i, 17) ? Db_GetValue(dbres, in_base_i, 17) : "";
		}
	} else {
		chp_value = Db_GetValue(dbres, in_base_i, 17) ? Db_GetValue(dbres, in_base_i, 17) : "";
	}
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, " bgcolor=\"#%s\"", chp_value);
	Put_Nlist(nlp_skl, "BG_COLOR", cha_str);
	chp_value = Db_GetValue(dbres, in_base_i, 19);
	if(chp_value) {
		Put_Nlist(nlp_skl, "BORDERCOLOR", chp_value);
	}
	chp_value = Db_GetValue(dbres, in_base_i, 20);
	if(chp_value) {
		Put_Nlist(nlp_skl, "BORDER", chp_value);
	}
	/* 説明 */
	if(!strcmp(chp_layout, CO_TYPE_A1)     || !strcmp(chp_layout, CO_TYPE_A2)
	   || !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)
	   || !strcmp(chp_layout, CO_TYPE_B1)  || !strcmp(chp_layout, CO_TYPE_B2)
	   || !strcmp(chp_layout, CO_TYPE_B3)  || !strcmp(chp_layout, CO_TYPE_B4)
	   || !strcmp(chp_layout, CO_TYPE_B5)  || !strcmp(chp_layout, CO_TYPE_B6)
	   || !strcmp(chp_layout, CO_TYPE_B7)  || !strcmp(chp_layout, CO_TYPE_B8)
	   || !strcmp(chp_layout, CO_TYPE_B9)  || !strcmp(chp_layout, CO_TYPE_B10)
	   || !strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_B12)
	   || !strcmp(chp_layout, CO_TYPE_D1)  || !strcmp(chp_layout, CO_TYPE_E1)) {
		chp_value = Db_GetValue(dbres, in_base_i, 2);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_skl, "GUIDE", chp_conv);
			free(chp_conv);
		}
	/* 詳細説明*/
	} else {
		chp_value = Db_GetValue(dbres, in_base_i, 3);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_skl, "GUIDE_DETAIL", chp_conv);
			free(chp_conv);
		}
	}
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, "%s/%s", GetCartSkeletonDirectory(), chp_skl);
	chp_value = Setpage_Out_Mem_To_Euc(nlp_skl, cha_str);
	Finish_Nlist(nlp_skl);
	if(!chp_value) {
		return 1;
	}
	Put_Nlist(nlp_out, "ARTICLE", chp_value);
	free(chp_value);
	*inp_i = i;
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		builditeminfo2
 * Include-file:	disp.h
 * Description:
 *	商品情報を作成する
 *	表示タイプD-1,D-2用商品表示関数
 *	表示タイプE-1,E-2用商品表示関数
%* ------------------------------------------------------------------------
 *			0:正常終了
 *			1:エラー
-* ------------------------------------------------------------------------
 */
static int builditeminfo2_f11(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, int *inp_i
, bool b_link
, int in_row
, int in_btn
) {
	NLIST *nlp_temp;
	char *chp_dir;
	char *chp_img;
	char *chp_cgipath;
	char *chp_defimagepath;
	char *chp_thumbnail;
	char *chp_image;
	char *chp_layout;
	char *chp_return_cgi_id;
	char *chp_value;
	char *chp_script;
	char *chp_conv;
	char *chp_maker;
	char *chp_esc;
	char cha_str[2048];
	char cha_standard_price[64];
	char cha_price[64];
	char cha_member_price[64];
	char cha_postage[64];
	char cha_cool_postage[64];
	char cha_extra_postage[64];
	char cha_item_type[64];
	const int in_base_i = *inp_i;
	int in_vip_price;
	int in_count;
	int in_reserve_count;
	int in_b_reserve;
	int in_b_stock;
	int in_subitem;
	int in_cool;
//	int in_ret;
	int i;

	nlp_temp = Init_Nlist();
	/* 設定 */
	chp_value = Db_GetValue(dbres, 0, 52);
	if(chp_value && *chp_value != '\0') {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " width=\"%s\"", chp_value);
		Put_Nlist(nlp_temp, "TABLE_WIDTH", cha_str);
	} else {
		memset(cha_str, '\0', sizeof(cha_str));
		strcpy(cha_str, " width=\"100%\"");
		Put_Nlist(nlp_temp, "TABLE_WIDTH", cha_str);
	}
	chp_value = Db_GetValue(dbres, 0, 53);
	if(chp_value && *chp_value != '\0') {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " height=\"%s\"", chp_value);
		Put_Nlist(nlp_temp, "TABLE_HEIGHT", cha_str);
	} else {
		/* empty is old format */
	}
	/* format */
	chp_value = Db_GetValue(dbres, 0, 20);
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, " border=\"%s\"", chp_value);
	Put_Nlist(nlp_temp, "TABLE_BORDER", cha_str);
	if(atoi(chp_value) > 0) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, " bordercolor=\"#%s\"", Db_GetValue(dbres, 0, 19));
		Put_Nlist(nlp_temp, "TABLE_BORDER_COLOR", cha_str);
	} else {
		/* empty */
	}
	chp_layout = Db_GetValue(dbres, in_base_i, 16) ? Db_GetValue(dbres, in_base_i, 16) : "";
	chp_value = getenv("SCRIPT_NAME");
	chp_script = strrchr(chp_value, '/');
	++chp_script;
	i = in_base_i;
	in_subitem = in_base_i;
#if 0
	if(!strcmp(chp_script, CO_ACTION_DISP_NEW) || !strcmp(chp_script, CO_ACTION_DISP_HIT)) {
		in_ret = Disp_Item_Variation(db, nlp_in, nlp_temp, "ITEM_VARIATION", Db_GetValue(dbres, in_base_i, 38) ? Db_GetValue(dbres, in_base_i, 38) : "", chp_layout, 0);
		if(in_ret) {
			Finish_Nlist(nlp_temp);
			return 1;
		}
	} else {
		in_ret = Disp_Item_Variation(db, nlp_in, nlp_temp, "ITEM_VARIATION", Db_GetValue(dbres, in_base_i, 38) ? Db_GetValue(dbres, in_base_i, 38) : "", chp_layout, 0);
		if(in_ret) {
			Finish_Nlist(nlp_temp);
			return 1;
		}
	}
#endif
	memset(cha_standard_price, '\0', sizeof(cha_standard_price));
	memset(cha_price, '\0', sizeof(cha_price));
	memset(cha_member_price, '\0', sizeof(cha_member_price));
	memset(cha_postage, '\0', sizeof(cha_postage));
	memset(cha_extra_postage, '\0', sizeof(cha_extra_postage));
	memset(cha_item_type, '\0', sizeof(cha_item_type));		/* 未使用 */
	in_vip_price = 0;
	in_count = 0;
	in_reserve_count = 0;
	in_b_reserve = 0;
	in_b_stock = 0;
	Get_Item_Data(dbres, cha_standard_price, cha_price, cha_member_price, cha_postage, cha_cool_postage, cha_extra_postage, cha_item_type
	, &in_vip_price, &in_count, &in_reserve_count, &in_b_reserve, &in_b_stock, &in_cool, in_subitem);

	chp_dir = GetCartImageURL();
	chp_thumbnail = Db_GetValue(dbres, in_base_i, 4) ? Db_GetValue(dbres, in_base_i, 4) : "";
	chp_image = Db_GetValue(dbres, in_base_i, 5) ? Db_GetValue(dbres, in_base_i, 5) : "";
	if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_E1)) {
		chp_img = chp_thumbnail;
	} else {
		chp_img = chp_image;
	}
	chp_cgipath = Get_Cart_CgiPath();
	chp_defimagepath = Get_Cart_DefImagePath();
	/* html */
	memset(cha_str, '\0', sizeof(cha_str));
	sprintf(cha_str, " bgcolor=\"#%s\"", Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : "");
	Put_Nlist(nlp_temp, "BG_COLOR", cha_str);
	Put_Nlist(nlp_temp, "ITEM_ID", Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
	if(b_link) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "<a href=\"%s/%s?id=%s\">", chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
		Put_Nlist(nlp_temp, "DISP_ITEM_DETAIL_CGI", cha_str);
	}
	chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1) ? Db_GetValue(dbres, in_base_i, 1) : "");
	Put_Nlist(nlp_temp, "ITEM_NAME", chp_esc);
	free(chp_esc);
	if(chp_img && *chp_img != '\0') {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%s/%s", chp_defimagepath, chp_img);
		Put_Nlist(nlp_temp, "ITEM_IMAGE", cha_str);
	} else {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%s/%s", chp_dir, CO_NOT_PICTURE_FILE);
		Put_Nlist(nlp_temp, "ITEM_IMAGE", cha_str);
	}
	chp_value = Db_GetValue(dbres, in_base_i, 6) ? Db_GetValue(dbres, in_base_i, 6) : "";
	chp_maker = Db_GetValue(dbres, in_base_i, 34);
	if(chp_maker && *chp_maker != '\0') {
		Put_Nlist(nlp_temp, "MAKER2START", "<!--MAKER2START");
		Put_Nlist(nlp_temp, "MAKER2END", "MAKER2END-->");
		if(chp_value && *chp_value != '\0' && strcmp(chp_value, "http://") > 0) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "<a href=\"%s\">", chp_value);
			Put_Nlist(nlp_temp, "MAKER_URL", cha_str);
		}
		chp_esc = Escape_HtmlString(chp_maker);
		Put_Nlist(nlp_temp, "MAKER_NAME", chp_esc);
		free(chp_esc);
	} else if(chp_value && *chp_value != '\0' && strcmp(chp_value, "http://") > 0) {
		Put_Nlist(nlp_temp, "MAKER1START", "<!--MAKER1START");
		Put_Nlist(nlp_temp, "MAKER1END", "MAKER1END-->");
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "<a href=\"%s\">", chp_value);
		Put_Nlist(nlp_temp, "MAKER_URL", cha_str);
	} else {
		Put_Nlist(nlp_temp, "MAKER1START", "<!--MAKER1START");
		Put_Nlist(nlp_temp, "MAKER1END", "MAKER1END-->");
		Put_Nlist(nlp_temp, "MAKER2START", "<!--MAKER2START");
		Put_Nlist(nlp_temp, "MAKER2END", "MAKER2END-->");
	}
	if(cha_item_type[0] != '\0') {
		chp_esc = Escape_HtmlString(cha_item_type);
		Put_Nlist(nlp_temp, "MAKER_TYPE", chp_esc);
		free(chp_esc);
	} else {
		Put_Nlist(nlp_temp, "MAKER_TYPE_START", "<!--MAKER_TYPE_START");
		Put_Nlist(nlp_temp, "MAKER_TYPE_END", "MAKER_TYPE_END-->");
	}
	chp_value = Db_GetValue(dbres, i, 41);
	if(chp_value && *chp_value == '1' && cha_standard_price[0] != '\0') {
		Put_Nlist(nlp_temp, "STANDARD_PRICE", cha_standard_price);
	} else {
		Put_Nlist(nlp_temp, "STANDARD_PRICE_START", "<!--STANDARD_PRICE_START");
		Put_Nlist(nlp_temp, "STANDARD_PRICE_END", "STANDARD_PRICE_END-->");
	}
	if(in_vip_price == 2) {
		Put_Nlist(nlp_temp, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
		Put_Nlist(nlp_temp, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
		Put_Nlist(nlp_temp, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
		Put_Nlist(nlp_temp, "ITEM_PRICE", cha_price);
		Put_Nlist(nlp_temp, "VIP_PRICE", cha_member_price);
	} else if(in_vip_price == 1) {
		Put_Nlist(nlp_temp, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
		Put_Nlist(nlp_temp, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
		Put_Nlist(nlp_temp, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE", cha_member_price);
	} else {
		Put_Nlist(nlp_temp, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
		Put_Nlist(nlp_temp, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
		Put_Nlist(nlp_temp, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
		Put_Nlist(nlp_temp, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
		Put_Nlist(nlp_temp, "ITEM_PRICE", cha_price);
	}
	if(Db_GetValue(dbres, in_subitem, 42) && *Db_GetValue(dbres, in_subitem, 42) == '1'
	&& Db_GetValue(dbres, in_subitem, 43) && *Db_GetValue(dbres, in_subitem, 43) != '\0'  && atoi(Db_GetValue(dbres, in_subitem, 43)) > 0) {
		Put_Nlist(nlp_temp, "POINT", Db_GetValue(dbres, in_subitem, 43));
	} else {
		Put_Nlist(nlp_temp, "POINT_START", "<!--POINT_START");
		Put_Nlist(nlp_temp, "POINT_END", "POINT_END-->");
	}
	if(cha_postage[0] != '\0') {
		Put_Nlist(nlp_temp, "POSTAGE", cha_postage);
	} else {
		Put_Nlist(nlp_temp, "POSTAGE_START", "<!--POSTAGE_START");
		Put_Nlist(nlp_temp, "POSTAGE_END", "POSTAGE_END-->");
	}
	if(cha_extra_postage[0] != '\0') {
		Put_Nlist(nlp_temp, "EXTRA_POSTAGE", cha_extra_postage);
	} else {
		Put_Nlist(nlp_temp, "EXTRA_POSTAGE_START", "<!--EXTRA_POSTAGE_START");
		Put_Nlist(nlp_temp, "EXTRA_POSTAGE_END", "EXTRA_POSTAGE_END-->");
	}
	if(in_b_stock == 1
	&& Db_GetValue(dbres, in_base_i, 31) && *Db_GetValue(dbres, in_base_i, 31) == '1') {
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				Put_Nlist(nlp_temp, "STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "STOCK", Db_GetValue(dbres, in_subitem, 44));
			}
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				Put_Nlist(nlp_temp, "RESERVE_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "RESERVE_STOCK", Db_GetValue(dbres, in_subitem, 45));
			}
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d", in_reserve_count);
			Put_Nlist(nlp_temp, "RESERVE_STOCK", cha_str);
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d", in_count);
			Put_Nlist(nlp_temp, "STOCK", cha_str);
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		}
	} else if(in_b_stock == 1) {
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				Put_Nlist(nlp_temp, "UNDISP_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "UNDISP_STOCK", Db_GetValue(dbres, in_subitem, 44));
			}
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK", "&nbsp;");
			} else {
				Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK", Db_GetValue(dbres, in_subitem, 45));
			}
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
		} else {
			Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
			Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_START", "<!--UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "UNDISP_STOCK_END", "UNDISP_STOCK-->");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
			Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
		}
	} else {
		Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
		Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
		Put_Nlist(nlp_temp, "RESERVE_STOCK_START", "<!--RESERVE_STOCK_START");
		Put_Nlist(nlp_temp, "RESERVE_STOCK_END", "RESERVE_STOCK_END-->");
		Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_START", "<!--RESERVE_UNDISP_STOCK_START");
		Put_Nlist(nlp_temp, "RESERVE_UNDISP_STOCK_END", "RESERVE_UNDISP_STOCK_END-->");
		Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_START", "<!--RESERVE_UNUSE_STOCK_START");
		Put_Nlist(nlp_temp, "RESERVE_UNUSE_STOCK_END", "RESERVE_UNUSE_STOCK_END-->");
	}
	chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
	if(!chp_return_cgi_id) {
		chp_return_cgi_id = "";
	}
	if(in_b_stock == 0 || in_count > 0
	   || strcmp(Db_GetValue(dbres, in_subitem, 0) ? Db_GetValue(dbres, in_subitem, 0) : "", Db_GetValue(dbres, in_subitem, 11) ? Db_GetValue(dbres, in_subitem, 11) : "") != 0
	|| (in_b_reserve == 1 && in_reserve_count > 0)) {
		Put_Nlist(nlp_temp, "PURCHASE_ITEM_CGI", g_cha_purchase_cgi);
		Put_Nlist(nlp_temp, "RETURN_CGI", chp_return_cgi_id);
		if(in_b_stock == 0 || in_count > 0
		|| (in_b_reserve == 1 && in_reserve_count > 0)) {
			/* empty */
		} else {
			Put_Nlist(nlp_temp, "BUTTON_BUY_START", "<!--BUTTON_BUY_START");
			Put_Nlist(nlp_temp, "BUTTON_BUY_END", "BUTTON_BUY_END-->");
		}
	}
	if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_E1)) {
		/* D1 E1 説明 */
		chp_value = Db_GetValue(dbres, in_base_i, 2);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_temp, "GUIDE", chp_conv);
			free(chp_conv);
		}
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%s/%s", CO_SC_ADMIN_SKELETON_DIRECTORY, CO_SC_SKEL_DISP_TYPE_D1E1);
		chp_value = Setpage_Out_Mem_To_Euc(nlp_temp, cha_str);
	} else {
		/* D2 E2 詳細説明*/
		chp_value = Db_GetValue(dbres, in_base_i, 3);
		if(chp_value && *chp_value != '\0') {
			chp_conv = Conv_Crlf(chp_value);
			Put_Nlist(nlp_temp, "GUIDE_DETAIL", chp_conv);
			free(chp_conv);
		}
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%s/%s", CO_SC_ADMIN_SKELETON_DIRECTORY, CO_SC_SKEL_DISP_TYPE_D2E2);
		chp_value = Setpage_Out_Mem_To_Euc(nlp_temp, cha_str);
	}
	Finish_Nlist(nlp_temp);
	if(!chp_value) {
		return 1;
	}
	Put_Nlist(nlp_out, "ARTICLE", chp_value);
	free(chp_value);
	*inp_i = i;
	return 0;
}

/*
+* ========================================================================
 * Function:		builditeminfo2_f3
 * Include-file:	disp.h
 * Description:
 *	商品情報を作成する
%* ========================================================================
 * Return:
 *	なし
-* ======================================================================*/
static void builditeminfo2_f3(
NLIST *nlp_in
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
) {
	char *chp_dir;
	char *chp_img;
	char *chp_cgipath;
	char *chp_imagepath;
	char *chp_defimagepath;
	char *chp_thumbnail;
	char *chp_image;
	char *chp_layout;
	char *chp_return_cgi_id;
	char *chp_value;
	char *chp_sub_value;
	char *chp_esc;
	char *chp_conv;
	char cha_price[64];
	char cha_buf[1024];
	const int in_base_i = *inp_i;
	int in_count;
	int in_subitem;
	int i;

	i = in_base_i;
	in_subitem = in_base_i;
	memset(cha_price, '\0', sizeof(cha_price));
	in_count = 0;
	get_item_data_f3(dbres, cha_price, &in_count, in_subitem);
	chp_dir = GetCartImageURL();
	chp_thumbnail = Db_GetValue(dbres, in_base_i, 4) ? Db_GetValue(dbres, in_base_i, 4) : "";
	chp_image = Db_GetValue(dbres, in_base_i, 5) ? Db_GetValue(dbres, in_base_i, 5) : "";
	chp_layout = Db_GetValue(dbres, in_base_i, 16) ? Db_GetValue(dbres, in_base_i, 16) : "";
	if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_E1)) {
		chp_img = chp_thumbnail;
	} else {
		chp_img = chp_image;
	}
	chp_cgipath = Get_Cart_CgiPath();
	chp_imagepath = GetCartImageDirectory();
	chp_defimagepath = Get_Cart_DefImagePath();
	strcat(chp_dest, "<td align=\"top\">\n");
	strcat(chp_dest, "<table border=\"0\">\n");
	strcat(chp_dest, "<tr><td>\n");
	if(*Db_GetValue(dbres, 0, 20) != '0') {
		sprintf(chp_dest + strlen(chp_dest),
			"<table border=\"1\" bordercolor=\"#%s\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\">\n"
				, Db_GetValue(dbres, 0, 19) ? Db_GetValue(dbres, 0, 19) : ""
		);
	} else {
		strcat(chp_dest, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">\n");
	}
	sprintf(chp_dest + strlen(chp_dest),
		"<tr bgcolor=\"#%s\"><a name=\"%s\"></a>\n"
		, Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : "", Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : ""
	);
	if(chp_img && *chp_img != '\0') {
		sprintf(chp_dest + strlen(chp_dest), "<td align=\"center\"><a href=\"%s/%s?id=%s\">"
			, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
		memset(cha_buf, '\0', sizeof(cha_buf));
		sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
		if(access(cha_buf, F_OK) == 0) {
			chp_sub_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1) ? Db_GetValue(dbres, in_base_i, 1) : "");
			sprintf(chp_dest + strlen(chp_dest),
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
				, chp_defimagepath, chp_img, chp_sub_value);
			free(chp_sub_value);
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
				, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME);
		}
		strcat(chp_dest, "</a></td>\n");
	} else {
		sprintf(chp_dest + strlen(chp_dest),
			"<td align=\"center\">"
			"<a href=\"%s/%s?id=%s\">"
			"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
			"</a></td>\n"
			, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : ""
			, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
		);
	}
	strcat(chp_dest, "</tr></table>\n");
	strcat(chp_dest, "</td></tr>\n");
	strcat(chp_dest, "<tr><td>\n");
	if(*Db_GetValue(dbres, 0, 20) != '0') {
		sprintf(chp_dest + strlen(chp_dest),
			"<table border=\"1\" bordercolor=\"#%s\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\">\n"
			, Db_GetValue(dbres, 0, 19) ? Db_GetValue(dbres, 0, 19) : ""
		);
	} else {
		strcat(chp_dest, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">\n");
	}
	sprintf(chp_dest + strlen(chp_dest),
		"<tr bgcolor=\"#%s\">\n", Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : ""
	);
	strcat(chp_dest, "<td>\n");
	strcat(chp_dest, "<table border=\"0\" cellpadding=\"3\" cellspacing=\"2\" width=\"100%\">\n");
	strcat(chp_dest, "<tr>\n");
	strcat(chp_dest, "<td valign=\"top\"nowrap align=\"right\"><span class=small>商品コード：</span></td>\n");
	sprintf(chp_dest + strlen(chp_dest), "<td><span class=\"small\">%s</td>\n", Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "");
	strcat(chp_dest, "</tr>\n");
	strcat(chp_dest, "<tr>\n");
	strcat(chp_dest, "<td valign=\"top\"nowrap align=\"right\"><span class=small>商品名：</span></td>\n");
	chp_sub_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1) ? Db_GetValue(dbres, in_base_i, 1) : "");
	if(b_link) {
		sprintf(chp_dest + strlen(chp_dest),
			"<td><span class=\"small\"><a href=\"%s/%s?id=%s\">%s</a></span></td>\n"
			, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL
			, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", chp_sub_value
		);
	} else {
		sprintf(chp_dest + strlen(chp_dest),
			"<td><span class=\"small\">%s</span></td>\n"
			, chp_sub_value
		);
	}
	free(chp_sub_value);
	strcat(chp_dest, "</tr>\n");
	sprintf(chp_dest + strlen(chp_dest),
		"<tr>\n"
			"<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">価格：</span></td>\n"
			"<td><span class=\"small\">%s</span></td>\n"
		"</tr>\n"
		, cha_price
	);
	strcat(chp_dest, "<tr>\n");
	if(in_count <= 0) {
		strcat(chp_dest, "<td align=\"right\" valign=\"top\" nowrap><span class=\"small\">在庫数：</td>\n");
		if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
			strcat(chp_dest, "<td><span class=\"small\">　</td>\n");
		} else {
			sprintf(chp_dest + strlen(chp_dest), "<td><span class=\"small\">%s</td>\n", Db_GetValue(dbres, in_subitem, 44));
		}
	} else {
		chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_subitem, 64) ? Db_GetValue(dbres, in_subitem, 64) : "");
		sprintf(chp_dest + strlen(chp_dest),
			"<td align=\"right\" valign=\"top\" nowrap>"
				"<span class=\"small\">在庫数："
			"</td>\n"
			"<td><span class=\"small\">%d%s</td>\n"
			, in_count, chp_esc
		);
		free(chp_esc);
	}
	strcat(chp_dest, "</tr>\n");
	chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
	if(!chp_return_cgi_id) {
		chp_return_cgi_id = "";
	}
	strcat(chp_dest, "<tr>\n");
	sprintf(chp_dest + strlen(chp_dest),
		"<td align=\"right\" colspan=\"2\">\n"
		"<form action=\"%s\" method=\"post\" name=\"%s\">\n"
		, g_cha_purchase_cgi, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : ""
	);
	/* バリエーションの表示無し */
	sprintf(chp_dest + strlen(chp_dest),
		"<input type=\"hidden\" name=\"ITEMID\" value=\"%s\">\n"
		"<input type=\"hidden\" name=\"RETURN_CGI_ID\" value=\"%s\">\n"
		, Db_GetValue(dbres, in_base_i, 0) ? Db_GetValue(dbres, in_base_i, 0) : "", chp_return_cgi_id
	);
	strcat(chp_dest, "<br><input type=\"submit\" value=\"カートに入れる\"></form></td>\n");
	strcat(chp_dest, "</tr>\n");
	strcat(chp_dest, "</table>\n");
	strcat(chp_dest, "</td>\n");
	strcat(chp_dest, "</tr></table>\n");
	if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_E1)) {
		/* D1 E1 説明 */
		if(Db_GetValue(dbres, in_base_i, 2) && *Db_GetValue(dbres, in_base_i, 2) != '\0') {
			if(Db_GetValue(dbres, 0, 20) && *Db_GetValue(dbres, 0, 20) != '0') {
				sprintf(chp_dest + strlen(chp_dest),
					"<tr>"
						"<td>"
						"<table border=\"1\" bordercolor=\"#%s\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\">\n"
						, Db_GetValue(dbres, 0, 19) ? Db_GetValue(dbres, 0, 19) : ""
				);
			} else {
				strcat(chp_dest,
					"<tr>"
						"<td>"
						"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">\n");
			}
			sprintf(chp_dest + strlen(chp_dest),
					"<tr bgcolor=\"#%s\">\n", Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : ""
			);
			sprintf(chp_dest + strlen(chp_dest),
							"<td align=\"left\" valign=\"top\" nowrap>"
								"<span class=\"small\">%s</span>"
							"</td>\n"
						"</tr>\n"
						"</table>\n"
					, Db_GetValue(dbres, in_base_i, 2) ? Db_GetValue(dbres, in_base_i, 2) : ""
			);
		}
	} else {
		/* D2 E2 詳細説明*/
		chp_value = Db_GetValue(dbres, in_base_i, 3);
		if(chp_value && *chp_value != '\0') {
			if(Db_GetValue(dbres, 0, 20) && *Db_GetValue(dbres, 0, 20) != '0') {
				sprintf(chp_dest + strlen(chp_dest),
					"<tr>"
						"<td>"
						"<table border=\"1\" bordercolor=\"#%s\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%%\">\n"
						, Db_GetValue(dbres, 0, 19) ? Db_GetValue(dbres, 0, 19) : ""
				);
			} else {
				strcat(chp_dest,
					"<tr>"
						"<td>"
						"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\">\n");
			}
			sprintf(chp_dest + strlen(chp_dest),
					"<tr bgcolor=\"#%s\">\n", Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : ""
			);
			chp_conv = Conv_Crlf(chp_value);
			sprintf(chp_dest + strlen(chp_dest),
							"<td align=\"left\" valign=\"top\" nowrap>"
								"<span class=\"small\">%s</span>"
							"</td>\n"
						"</tr>\n"
						"</table>\n"
				, chp_conv
			);
			free(chp_conv);
		}
	}
	strcat(chp_dest,
			"</td>\n"
		"</tr>\n"
		"</table>\n"
	);
	strcat(chp_dest, "</td>\n");
	*inp_i = i;
	return;
}

/*
+* ========================================================================
 * Function:		商品情報表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプA-1,A-2,A-3,A-4用商品表示関数
%* ========================================================================
 * Return:	なし
-* ======================================================================*/
int build_A_line_value_f11(
DBase *db
, DBRes *dbres
, NLIST *nlp_in
, NLIST *nlp_out
, char *chp_dest
, char *chp_vari_string
, int in_subitem
) {
	char *chp_return_cgi_id;
	char *chp_value;
	char *chp_layout;
	char cha_button[512];
	char cha_str[512];
	char cha_standard_price[64];
	char cha_price[64];
	char cha_member_price[64];
	char cha_postage[64];
	char cha_cool_postage[64];
	char cha_extra_postage[64];
	char cha_item_type[64];
	int in_vip_price;
	int in_count;
	int in_reserve_count;
	int in_b_reserve;
	int in_b_stock;
	int in_cool;
//	int in_ret;

	memset(cha_standard_price, '\0', sizeof(cha_standard_price));
	memset(cha_price, '\0', sizeof(cha_price));
	memset(cha_member_price, '\0', sizeof(cha_member_price));
	memset(cha_postage, '\0', sizeof(cha_postage));
	memset(cha_extra_postage, '\0', sizeof(cha_extra_postage));
	memset(cha_item_type, '\0', sizeof(cha_item_type));		/* 未使用 */
	in_vip_price = 0;
	in_count = 0;
	in_reserve_count = 0;
	in_b_reserve = 0;
	in_b_stock = 0;
	Get_Item_Data(dbres, cha_standard_price, cha_price, cha_member_price, cha_postage, cha_cool_postage, cha_extra_postage, cha_item_type
	, &in_vip_price, &in_count, &in_reserve_count, &in_b_reserve, &in_b_stock, &in_cool, in_subitem);

	memset(cha_button, '\0', sizeof(cha_button));
	if(in_b_stock == 0 || in_count > 0
	|| (in_b_reserve == 1 && in_reserve_count > 0)) {
		chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
		if(!chp_return_cgi_id) {
			chp_return_cgi_id = "";
		}
		sprintf(cha_button,
			"<td align=\"center\">\n"
			"<input type=\"hidden\" name=\"ITEMID\" value=\"%s\">\n"
			"<input type=\"hidden\" name=\"RETURN_CGI_ID\" value=\"%s\">\n"
			, Db_GetValue(dbres, in_subitem, 0) ? Db_GetValue(dbres, in_subitem, 0) : "", chp_return_cgi_id);
		strcat(cha_button, "<input type=\"submit\" value=\"カートに入れる\"></td>");
	}
	chp_value = Escape_HtmlString(Db_GetValue(dbres, in_subitem, 1) ? Db_GetValue(dbres, in_subitem, 1) : "");
	sprintf(chp_dest + strlen(chp_dest),
		"<td nowrap><span class=\"small\"><a href=\"%s/%s?id=%s\">%s</a>"
		, Get_Cart_CgiPath(), CO_ACTION_DISP_ITEM_DETAIL
			, Db_GetValue(dbres, in_subitem, 0) ? Db_GetValue(dbres, in_subitem, 0) : "", chp_value);
	free(chp_value);
	/* バリエーションの表示 */
	chp_layout = Db_GetValue(dbres, in_subitem, 16) ? Db_GetValue(dbres, in_subitem, 16) : "";
	Put_Nlist(nlp_out, "ARTICLE", chp_dest);
	*chp_dest = '\0';
#if 0
	in_ret = Disp_Item_Variation(db, nlp_in, nlp_out, "ARTICLE", Db_GetValue(dbres, in_subitem, 38) ? Db_GetValue(dbres, in_subitem, 38) : "", chp_layout, 0);
	if(in_ret) {
		return 1;
	}
#endif
	strcat(chp_dest, "</span></td>\n");
	if(Db_GetValue(dbres, in_subitem, 41) && *Db_GetValue(dbres, in_subitem, 41) == '1'
	 && cha_standard_price[0] != '\0') {
		sprintf(chp_dest + strlen(chp_dest),
			"<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n", cha_standard_price);
	} else if(Db_GetValue(dbres, in_subitem, 41) && *Db_GetValue(dbres, in_subitem, 41) == '1') {
		strcat(chp_dest, "<td nowrap>　</td>\n");
	}

	if(in_vip_price == 2) {
		sprintf(chp_dest + strlen(chp_dest),
			"<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n", cha_price);
		sprintf(chp_dest + strlen(chp_dest),
			"<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n", cha_member_price);
	} else if(in_vip_price == 1) {
		sprintf(chp_dest + strlen(chp_dest),
			"<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n", cha_member_price);
	} else {
		sprintf(chp_dest + strlen(chp_dest),
			"<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n", cha_price);
	}
	if(Db_GetValue(dbres, in_subitem, 42) && *Db_GetValue(dbres, in_subitem, 42) == '1'
	 && Db_GetValue(dbres, in_subitem, 43) && *Db_GetValue(dbres, in_subitem, 43) != '\0'  && atoi(Db_GetValue(dbres, in_subitem, 43)) > 0) {
		sprintf(chp_dest + strlen(chp_dest),
			"<td nowrap align=\"right\"><span class=\"small\">%s％</span></td>\n"
		, Db_GetValue(dbres, in_subitem, 43));
	} else if(Db_GetValue(dbres, in_subitem, 42) && *Db_GetValue(dbres, in_subitem, 42) == '1') {
		strcat(chp_dest, "<td nowrap align=\"right\"><span class=\"small\">0％</span></td>\n");
	}

	sprintf(chp_dest + strlen(chp_dest),
		"<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n", cha_postage);
	if(cha_extra_postage[0] != '\0') {
		sprintf(chp_dest + strlen(chp_dest),
			"<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n"
			, cha_extra_postage);
	} else {
		strcat(chp_dest, "<td nowrap align=\"right\"><span class=\"small\">　</span></td>\n");
	}
	memset(cha_str, '\0', sizeof(cha_str));
	if(in_b_stock == 1
	&& Db_GetValue(dbres, in_subitem, 31) && *Db_GetValue(dbres, in_subitem, 31) == '1') {
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			strcat(cha_str, "<td nowrap align=\"right\"><span class=\"small\">0</span></td>\n");
			memset(cha_button, '\0', sizeof(cha_button));
			strcpy(cha_button, "<td align=\"left\" nowrap><span class=\"small\">");
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				strcat(cha_button, "　");
			} else {
				strcat(cha_button, Db_GetValue(dbres, in_subitem, 44));
			}
			strcat(cha_button, "</span></td>\n");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			sprintf(cha_str + strlen(cha_str),
				"<td nowrap align=\"right\"><span class=\"small\">"
				"<font color=\"#%s\">[予約販売] </font>0</span></td>\n"
				, Db_GetValue(dbres, 0, 51)
			);
			memset(cha_button, '\0', sizeof(cha_button));
			strcpy(cha_button, "<td align=\"left\" nowrap><span class=\"small\">");
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				strcat(cha_button, "　");
			} else {
				strcat(cha_button, Db_GetValue(dbres, in_subitem, 45));
			}
			strcat(cha_button, "</span></td>\n");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td nowrap align=\"right\"><span class=\"small\">"
				"<font color=\"#%s\">[予約販売] </font>%d</span></td>\n"
				, Db_GetValue(dbres, 0, 51) ? Db_GetValue(dbres, 0, 51) : ""
				, in_reserve_count
			);
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td nowrap align=\"right\"><span class=\"small\">%d</span></td>\n"
			, in_count);
		}
	} else if(in_b_stock == 1) {
		if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '0') {
			memset(cha_button, '\0', sizeof(cha_button));
			strcpy(cha_button, "<td align=\"left\" nowrap><span class=\"small\">\n");
			if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
				strcat(cha_button, "　");
			} else {
				strcat(cha_button, Db_GetValue(dbres, in_subitem, 44));
			}
			strcat(cha_button, "</span></td>\n");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count == 0) {
			memset(cha_button, '\0', sizeof(cha_button));
			sprintf(cha_button, "<td align=\"left\" nowrap>"
				"<span class=\"small\"><font color=\"#%s\">[予約販売] </font></span>\n"
				"<span class=\"small\">"
				, Db_GetValue(dbres, 0, 51) ? Db_GetValue(dbres, 0, 51) : ""
			);
			if(!Db_GetValue(dbres, in_subitem, 45) || *Db_GetValue(dbres, in_subitem, 45) == '\0') {
				strcat(cha_button, "　");
			} else {
				strcat(cha_button, Db_GetValue(dbres, in_subitem, 45));
			}
			strcat(cha_button, "</span></td>\n");
		} else if(in_count == 0 && Db_GetValue(dbres, in_subitem, 39) && *Db_GetValue(dbres, in_subitem, 39) == '1'
		 && in_reserve_count > 0) {
			sprintf(cha_str,
				"<td align=\"left\" nowrap>"
				"<span class=\"small\"><font color=\"#%s\">[予約販売]</font></span></td>%s\n"
				, Db_GetValue(dbres, 0, 51) ? Db_GetValue(dbres, 0, 51) : ""
				, cha_button
			);
			memset(cha_button, '\0', sizeof(cha_button));
			sprintf(cha_button, "<td><table><tr>%s</tr></table></td>", cha_str);
			memset(cha_str, '\0', sizeof(cha_str));
		}
	} else if(Db_GetValue(dbres, in_subitem, 31) && *Db_GetValue(dbres, in_subitem, 31) == '1') {
		strcat(chp_dest, "<td nowrap align=\"right\"><span class=\"small\">　</span></td>\n");
	}
	strcat(chp_dest, cha_str);
	if(!strcmp(Db_GetValue(dbres, in_subitem, 16) ? Db_GetValue(dbres, in_subitem, 16) : "", CO_TYPE_A3)) {
		build_a_line_guide(dbres, chp_dest, in_subitem, 1);
	}
	strcat(chp_dest, cha_button);
	return 0;
}

/*
+* ========================================================================
 * Function:		商品情報表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプA-1,A-2,A-3,A-4用商品表示関数
%* ========================================================================
 * Return:	なし
-* ======================================================================*/
void build_A_line_value_f3(DBRes *dbres, NLIST *nlp_in, char *chp_dest, int in_subitem)
{
	char *chp_return_cgi_id;
	char *chp_esc;
	char cha_button[512];
	char cha_str[512];
	char cha_price[64];
	int in_count;

	memset(cha_price, '\0', sizeof(cha_price));
	in_count = 0;
	get_item_data_f3(dbres, cha_price, &in_count, in_subitem);
	memset(cha_button, '\0', sizeof(cha_button));
	chp_return_cgi_id = Get_Nlist(nlp_in, "id", 1);
	if(!chp_return_cgi_id) {
		chp_return_cgi_id = "";
	}
	memset(cha_button, '\0', sizeof(cha_button));
	sprintf(cha_button
	, "<td align=\"center\"><input type=\"hidden\" name=\"ITEMID\" value=\"%s\"><input type=\"hidden\" name=\"RETURN_CGI_ID\" value=\"%s\"><input type=\"submit\" value=\"カートに入れる\"></td>\n"
	, Db_GetValue(dbres, in_subitem, 0), chp_return_cgi_id);
	chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_subitem, 1));
	sprintf(chp_dest + strlen(chp_dest), "<td nowrap><span class=\"small\">%s</span></td>\n", Db_GetValue(dbres, in_subitem, 0));
	sprintf(chp_dest + strlen(chp_dest), "<td nowrap><span class=\"small\"><a href=\"%s/%s?id=%s\">%s</a></span></td>\n"
	, Get_Cart_CgiPath(), CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_subitem, 0), chp_esc);
	free(chp_esc);
	sprintf(chp_dest + strlen(chp_dest), "<td align=\"right\" nowrap><span class=\"small\">%s</span></td>\n", cha_price);
	memset(cha_str, '\0', sizeof(cha_str));
	if(in_count <= 0) {
		if(!Db_GetValue(dbres, in_subitem, 44) || *Db_GetValue(dbres, in_subitem, 44) == '\0') {
			strcpy(cha_str, "<td nowrap align=\"right\"><span class=\"small\">　</span></td>\n");
		} else {
			sprintf(cha_str, "<td nowrap align=\"right\"><span class=\"small\">%s</span></td>\n", Db_GetValue(dbres, in_subitem, 44));
		}
	} else {
		chp_esc = Escape_HtmlString(Db_GetValue(dbres, in_subitem, 64));
		sprintf(chp_dest + strlen(chp_dest), "<td nowrap align=\"right\"><span class=\"small\">%d%s</span></td>\n", in_count, chp_esc);
		free(chp_esc);
	}
	strcat(chp_dest, cha_str);
	if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A3)) {
		build_a_line_guide(dbres, chp_dest, in_subitem, 1);
	}
	strcat(chp_dest, cha_button);
	return;
}

/*
+* ========================================================================
 * Function:		商品説明表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプA-1,A-2,A-3,A-4用商品表示関数
%* ========================================================================
 * Return:	なし
-* ======================================================================*/
void build_a_line_guide(DBRes *dbres, char *chp_dest, int in_subitem, int in_columns)
{
	sprintf(chp_dest + strlen(chp_dest),
		"<td colspan=\"%d\"><span class=\"small\">%s</span></td>", in_columns, Db_GetValue(dbres, in_subitem, 2));
	return;
}

/*
+* ========================================================================
 * Function:		商品表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプA-1,A-2,A-3,A-4用商品表示関数
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
static int build_typeA_f11(
DBase *db
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, int in_start
, int in_row
, int in_cnt
)
{
	char *chp_value;
	char *chp_script;
	char cha_variation[CO_VARIATION_BUFFER * 10];
	char cha_color[32];
	const int in_base_i = *inp_i;
	int in_vip_price;
	int in_subitem;
	int in_columns;
	int in_ret;
	int i;

	/* バリエーションの表示 */
	chp_value = getenv("SCRIPT_NAME");
	chp_script = strrchr(chp_value, '/');
	++chp_script;
	i = in_base_i;
	in_subitem = in_base_i;
	cha_variation[0] = '\0';
	if(Db_GetValue(dbres, in_subitem, 37) && *Db_GetValue(dbres, in_subitem, 37) == '1') {//login状態
		if(Db_GetValue(dbres, in_subitem, 35) && *Db_GetValue(dbres, in_subitem, 35) == '2') {//表示フラグ
			in_vip_price = 2;
		} else if(*Db_GetValue(dbres, in_subitem, 35) == '1') {
			in_vip_price = 1;
		} else {
			in_vip_price = 0;
		}
	} else {
		in_vip_price = 0;
	}
	in_columns = 5;		/* 6 - rowspan=2 * 1 = 5 <td> */
	if(in_vip_price == 2) {
		in_columns++;
	}
	if(*Db_GetValue(dbres, in_subitem, 31) == '1') {
		in_columns++;
	}
	if(*Db_GetValue(dbres, in_subitem, 41) == '1') {
		in_columns++;
	}
	if(*Db_GetValue(dbres, in_subitem, 42) == '1') {
		in_columns++;
	}
	/* 1行目 */
	if(in_base_i == in_start) {
		strcpy(chp_dest, "<tr>\n");
		strcat(chp_dest, "<td align=\"center\"><span class=\"small\">No.</span></td>\n");
		if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A4)) {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">備考</span></td>\n");
		}
		strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\">商品名</span></td>\n");
		if(Db_GetValue(dbres, in_subitem, 41) && *Db_GetValue(dbres, in_subitem, 41) == '1') {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">標準価格</span></td>\n");
		}
		if(in_vip_price == 2) {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">一般価格</span></td>\n");
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">会員価格</span></td>\n");
		} else if(in_vip_price == 1) {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">会員価格</span></td>\n");
		} else {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">価　格</span></td>\n");
		}
		if(Db_GetValue(dbres, in_subitem, 42) && *Db_GetValue(dbres, in_subitem, 42) == '1') {
			strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\">ポイント還元</span></td>\n");
		}
		strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\">送　料</span></td>\n");
		strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\">別途追加送料</span></td>\n");
		if(Db_GetValue(dbres, in_subitem, 31) && *Db_GetValue(dbres, in_subitem, 31) == '1') {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">在庫数</span></td>\n");
		}
		if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A3)) {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\">備考</span></td>\n");
		}
		strcat(chp_dest, "<td align=\"center\"><span class=\"small\">カート</span></td>\n");
		strcat(chp_dest, "</tr>\n");
	}
	/* 2行目 */
	memset(cha_color, '\0', sizeof(cha_color));
	if(in_cnt % 2) {
		sprintf(cha_color, "<tr bgcolor=\"#%s\">\n", Db_GetValue(dbres, in_subitem, 18));
	} else {
		sprintf(cha_color, "<tr bgcolor=\"#%s\">\n", Db_GetValue(dbres, in_subitem, 17));
	}
	strcat(chp_dest, cha_color);
	if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A1) || !strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A2)) {
		strcat(chp_dest, "<td rowspan=\"2\" align=\"center\">");
	} else {
		strcat(chp_dest, "<td align=\"center\">");
	}
	sprintf(chp_dest + strlen(chp_dest),
		"<a name=\"%s\">%d</a></td>\n"
		"<form action=\"%s\" method=\"post\" name=\"%s\">\n"
	, Db_GetValue(dbres, in_subitem, 0), in_cnt + 1
	, g_cha_purchase_cgi, Db_GetValue(dbres, in_subitem, 0));
	if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A1)) {
		in_ret = build_A_line_value_f11(db, dbres, nlp_in, nlp_out, chp_dest, cha_variation, in_subitem);
		if(in_ret) {
			return -1;
		}
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, cha_color);
		build_a_line_guide(dbres, chp_dest, in_subitem, in_columns);
		strcat(chp_dest, "</tr></form>\n");
	} else if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A2)) {
		build_a_line_guide(dbres, chp_dest, in_subitem, in_columns);
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, cha_color);
		in_ret = build_A_line_value_f11(db, dbres, nlp_in, nlp_out, chp_dest, cha_variation, in_subitem);
		if(in_ret) {
			return -1;
		}
		strcat(chp_dest, "</tr></form>\n");
	} else if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A3)) {
		in_ret = build_A_line_value_f11(db, dbres, nlp_in, nlp_out, chp_dest, cha_variation, in_subitem);
		if(in_ret) {
			return -1;
		}
		strcat(chp_dest, "</tr></form>\n");
	} else {/* CO_TYPE_A4 */
		build_a_line_guide(dbres, chp_dest, in_subitem, 1);
		in_ret = build_A_line_value_f11(db, dbres, nlp_in, nlp_out, chp_dest, cha_variation, in_subitem);
		if(in_ret) {
			return -1;
		}
		strcat(chp_dest, "</tr></form>\n");
	}
	*inp_i = i;
	return 0;
}

/*
+* ========================================================================
 * Function:		商品表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプA-1,A-2,A-3,A-4用商品表示関数
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
static int build_typeA_f3(
DBase *db
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, int in_start
, int in_row
, int in_cnt
)
{
	DBRes *current_dbres;
	char cha_sql[512];
	char cha_color[32];
	const int in_base_i = *inp_i;
	int in_subitem;
	int in_columns;
	int i;

	i = in_base_i;
	in_subitem = in_base_i;
	/* バリエーションの表示無し */
	in_columns = 5;
	if(*Db_GetValue(dbres, in_subitem, 31) == '1') {
		in_columns++;
	}
	if(*Db_GetValue(dbres, in_subitem, 41) == '1') {
		in_columns++;
	}
	if(*Db_GetValue(dbres, in_subitem, 42) == '1') {
		in_columns++;
	}
	/* 1行目 */
	if(in_base_i == in_start) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "select T1.c_text");
		strcat(cha_sql, " from sm_style_general T1");
		current_dbres = Db_OpenDyna(db, cha_sql);
		if(!current_dbres) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d：query error(%s)", __LINE__, Gcha_last_error);
			Put_Nlist(nlp_out, "ERROR", cha_sql);
			return -1;
		}
		if(!Db_GetRowCount(dbres)) {
			Db_CloseDyna(current_dbres);
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d：画面設定情報が正しくありません。", __LINE__);
			Put_Nlist(nlp_out, "ERROR", cha_sql);
			return -1;
		}
		sprintf(chp_dest, "<tr bgcolor=\"#%s\">\n", Db_GetValue(current_dbres, 0, 0));
		Db_CloseDyna(current_dbres);
		strcat(chp_dest, "<td align=\"center\"><span class=\"small\"><font color=\"#ffffff\"><b>No.</b></font></span></td>\n");
		if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A4)) {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\"><font color=\"#ffffff\"><b>備考</b></font></span></td>\n");
		}
		strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\"><font color=\"#ffffff\"><b>商品コード</b></font></span></td>\n");
		strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\"><font color=\"#ffffff\"><b>商品名</b></font></span></td>\n");
		strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\"><font color=\"#ffffff\"><b>価　格</b></font></span></td>\n");
		strcat(chp_dest, "<td align=\"center\" nowrap><span class=\"small\"><font color=\"#ffffff\"><b>在庫数</b></font></span></td>\n");
		if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A3)) {
			strcat(chp_dest, "<td align=\"center\"><span class=\"small\"><font color=\"#ffffff\"><b>備考</b></font></span></td>\n");
		}
		strcat(chp_dest, "<td align=\"center\"><span class=\"small\"><font color=\"#ffffff\"><b>カート</b></font></span></td>\n");
		strcat(chp_dest, "</tr>\n");
	}
	/* 2行目 */
	memset(cha_color, '\0', sizeof(cha_color));
	if(in_cnt % 2) {
		sprintf(cha_color, "<tr bgcolor=\"#%s\">\n", Db_GetValue(dbres, in_subitem, 18));
	} else {
		sprintf(cha_color, "<tr bgcolor=\"#%s\">\n", Db_GetValue(dbres, in_subitem, 17));
	}
	strcat(chp_dest, cha_color);
	if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A1) || !strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A2)) {
		strcat(chp_dest, "<td rowspan=\"2\" align=\"center\">");
	} else {
		strcat(chp_dest, "<td align=\"center\">");
	}
	sprintf(chp_dest + strlen(chp_dest),
		"<a name=\"%s\">%d</a></td>\n"
		"<form action=\"%s\" method=\"post\" name=\"%s\">\n"
	, Db_GetValue(dbres, in_subitem, 0), in_cnt + 1
	, g_cha_purchase_cgi, Db_GetValue(dbres, in_subitem, 0));
	if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A1)) {
		build_A_line_value_f3(dbres, nlp_in, chp_dest, in_subitem);
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, cha_color);
		build_a_line_guide(dbres, chp_dest, in_subitem, in_columns);
		strcat(chp_dest, "</tr></form>\n");
	} else if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A2)) {
		build_a_line_guide(dbres, chp_dest, in_subitem, in_columns);
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, cha_color);
		build_A_line_value_f3(dbres, nlp_in, chp_dest, in_subitem);
		strcat(chp_dest, "</tr></form>\n");
	} else if(!strcmp(Db_GetValue(dbres, in_subitem, 16), CO_TYPE_A3)) {
		build_A_line_value_f3(dbres, nlp_in, chp_dest, in_subitem);
		strcat(chp_dest, "</tr></form>\n");
	} else {/* CO_TYPE_A4 */
		build_a_line_guide(dbres, chp_dest, in_subitem, 1);
		build_A_line_value_f3(dbres, nlp_in, chp_dest, in_subitem);
		strcat(chp_dest, "</tr></form>\n");
	}
	*inp_i = i;
	return 0;
}

/*
+* ========================================================================
 * Function:		商品表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプB-1,B-2,B-7,B-8,C-1,C-2,C-7,C-8,A,B,G,H用商品表示関数
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
static int build_standard(
DBase *db
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
) {
	char *chp_dir;
	char *chp_img;
	char *chp_thumbnail;
	char *chp_image;
	char *chp_layout;
	char *chp_conv;
	char *chp_cgipath;
	char *chp_imagepath;
	char *chp_defimagepath;
	char *chp_value;
	char cha_buf[1024];
	const int in_base_i = *inp_i;
	int in_ret;

	chp_dir = GetCartImageURL();
	chp_cgipath = Get_Cart_CgiPath();
	chp_imagepath = GetCartImageDirectory();
	chp_defimagepath = Get_Cart_DefImagePath();
	chp_thumbnail = Db_GetValue(dbres, in_base_i, 4);
	chp_image = Db_GetValue(dbres, in_base_i, 5);
	chp_layout = Db_GetValue(dbres, in_base_i, 16);
	sprintf(chp_dest + strlen(chp_dest),
		"<tr bgcolor=\"#%s\"><a name=\"%s\"></a>\n"
		, Db_GetValue(dbres, 0, 17), Db_GetValue(dbres, in_base_i, 0)
	);
	if(!strcmp(chp_layout, CO_TYPE_B1) || !strcmp(chp_layout, CO_TYPE_C1) || !strcmp(chp_layout, CO_TYPE_A)) {
		if(!strcmp(chp_layout, CO_TYPE_B1)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		if(chp_img && *chp_img != '\0') {
			sprintf(chp_dest + strlen(chp_dest), "<td align=\"center\" width=\"30%%\"><a href=\"%s/%s?id=%s\">"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0));
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
					, chp_defimagepath, chp_img, chp_value);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME);
			}
			strcat(chp_dest, "</a></td>\n");
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\" width=\"30%%\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "<td align=\"center\" valign=\"top\" width=\"70%\">\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<table bgcolor=\"#%s\" border=\"0\" cellspacing=\"1\" cellpadding=\"1\" width=\"100%%\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		strcat(chp_dest, "<tr>\n");
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "<tr>\n");
		if(!strcmp(chp_layout, CO_TYPE_B1)) {
			sprintf(chp_dest + strlen(chp_dest),
/*				"<td colspan=\"3\"><span class=\"small\">%s</span></td>\n" */
				"<td><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
/*				"<td colspan=\"3\"><span class=\"small\">%s</span></td>\n" */
				"<td><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "</table>\n");
	} else if(!strcmp(chp_layout, CO_TYPE_B2) || !strcmp(chp_layout, CO_TYPE_C2) || !strcmp(chp_layout, CO_TYPE_B)) {
		if(!strcmp(chp_layout, CO_TYPE_B2)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "<td>\n");
		sprintf(chp_dest + strlen(chp_dest),
				"<table bgcolor=\"#%s\" border=\"0\" cellspacing=\"3\" cellpadding=\"2\" width=\"100%%\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		strcat(chp_dest, "<tr>\n");
		if(!strcmp(chp_layout, CO_TYPE_B2)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td><span class=\"small\">%s</span></td>\n", Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td><span class=\"small\">%s</span></td>\n", chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "<tr>\n");
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "</table>\n");
	} else if(!strcmp(chp_layout, CO_TYPE_B7) || !strcmp(chp_layout, CO_TYPE_C7) || !strcmp(chp_layout, CO_TYPE_G)) {
		if(!strcmp(chp_layout, CO_TYPE_B7)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		if(!strcmp(chp_layout, CO_TYPE_B7)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "<td align=\"center\">\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "<br>\n");
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B8) || !strcmp(chp_layout, CO_TYPE_C8) || !strcmp(chp_layout, CO_TYPE_H)) {
		if(!strcmp(chp_layout, CO_TYPE_B8)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		if(!strcmp(chp_layout, CO_TYPE_B8)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "<td align=\"center\">\n");
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "<br>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
	}
	strcat(chp_dest, "</td>\n");
	strcat(chp_dest, "</tr>\n");
	return 0;
}

/*
+* ========================================================================
 * Function:		商品表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプB-3,B-4,B-5,B-6,C-3,C-4,C-5,C-6,C,D,E,F用商品表示関数
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
static int build_exception(
DBase *db
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
) {
	char *chp_dir;
	char *chp_img;
	char *chp_thumbnail;
	char *chp_image;
	char *chp_layout;
	char *chp_conv;
	char *chp_cgipath;
	char *chp_defimagepath;
	char *chp_imagepath;
	char *chp_value;
	char cha_buf[1024];
	const int in_base_i = *inp_i;
	int in_ret;

	chp_dir = GetCartImageURL();
	chp_cgipath = Get_Cart_CgiPath();
	chp_imagepath = GetCartImageDirectory();
	chp_defimagepath = Get_Cart_DefImagePath();
	chp_thumbnail = Db_GetValue(dbres, in_base_i, 4);
	chp_image = Db_GetValue(dbres, in_base_i, 5);
	chp_layout = Db_GetValue(dbres, in_base_i, 16);
	sprintf(chp_dest + strlen(chp_dest),
		"<tr bgcolor=\"#%s\"><a name=\"%s\"></a>\n"
		, Db_GetValue(dbres, 0, 17), Db_GetValue(dbres, in_base_i, 0)
	);
	if(!strcmp(chp_layout, CO_TYPE_B3) || !strcmp(chp_layout, CO_TYPE_C3) || !strcmp(chp_layout, CO_TYPE_C)) {
		if(!strcmp(chp_layout, CO_TYPE_B3)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		strcat(chp_dest, "<td width=\"40%\" align=\"center\">\n");
		strcat(chp_dest, "<table border=\"0\" cellspacing=\"3\" cellpadding=\"2\" width=\"100%\">\n");
		strcat(chp_dest, "<tr>\n");
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "<tr>\n");
		if(chp_img && *chp_img != '\0') {
			sprintf(chp_dest + strlen(chp_dest), "<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0));
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
					, chp_defimagepath, chp_img, chp_value);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME);
			}
			strcat(chp_dest, "</a></td>\n");
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "</table>\n");
		strcat(chp_dest, "</td>\n");
		if(!strcmp(chp_layout, CO_TYPE_B3)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td width=\"60%%\" valign=\"top\" bgcolor=\"#%s\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, 0, 17), Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td width=\"60%%\" valign=\"top\" bgcolor=\"#%s\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, 0, 17), chp_conv
			);
			free(chp_conv);
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B4) || !strcmp(chp_layout, CO_TYPE_C4) || !strcmp(chp_layout, CO_TYPE_D)) {
		if(!strcmp(chp_layout, CO_TYPE_B4)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		strcat(chp_dest, "<td width=\"40%\" align=\"center\">\n");
		strcat(chp_dest, "<table border=\"0\" cellspacing=\"3\" cellpadding=\"2\" width=\"100%\">\n");
		strcat(chp_dest, "<tr>\n");
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "<tr>\n");
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "</table>\n");
		strcat(chp_dest, "</td>\n");
		if(!strcmp(chp_layout, CO_TYPE_B4)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td width=\"60%%\" valign=\"top\" bgcolor=\"#%s\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, 0, 17), Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td width=\"60%%\" valign=\"top\" bgcolor=\"#%s\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, 0, 17), chp_conv
			);
			free(chp_conv);
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B5) || !strcmp(chp_layout, CO_TYPE_C5) || !strcmp(chp_layout, CO_TYPE_E)) {
		if(!strcmp(chp_layout, CO_TYPE_B5)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		strcat(chp_dest, "<td width=\"70%\">\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "<table cellspacing=\"3\" cellpadding=\"2\" width=\"100%\">\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">"
			, Db_GetValue(dbres, 0, 17)
		);
		if(!strcmp(chp_layout, CO_TYPE_B5)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n", Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "</tr>\n");
		strcat(chp_dest, "</table>\n");
		strcat(chp_dest, "</td>\n");
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\" width=\"30%%\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\" width=\"30%%\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\" width=\"30%%\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B6) || !strcmp(chp_layout, CO_TYPE_C6) || !strcmp(chp_layout, CO_TYPE_F)) {
		if(!strcmp(chp_layout, CO_TYPE_B6)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		if(!strcmp(chp_layout, CO_TYPE_B6)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "<br>\n");
		strcat(chp_dest, "<br>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "<td  align=\"center\">\n");
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "</td>\n");
	}
	strcat(chp_dest, "</tr>");
	return 0;
}

/*
+* ========================================================================
 * Function:		商品表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプB-9,B-10,B-11,B-12,C-9,C-10,C-11,C-12,I,J,K,L用商品表示関数
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
static int build_three_col(
DBase *db
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
) {
	char *chp_dir;
	char *chp_img;
	char *chp_thumbnail;
	char *chp_image;
	char *chp_layout;
	char *chp_conv;
	char *chp_cgipath;
	char *chp_defimagepath;
	char *chp_imagepath;
	char *chp_value;
	char cha_buf[1024];
	const int in_base_i = *inp_i;
	int in_ret;

	chp_dir = GetCartImageURL();
	chp_cgipath = Get_Cart_CgiPath();
	chp_defimagepath = Get_Cart_DefImagePath();
	chp_imagepath = GetCartImageDirectory();
	chp_thumbnail = Db_GetValue(dbres, in_base_i, 4);
	chp_image = Db_GetValue(dbres, in_base_i, 5);
	chp_layout = Db_GetValue(dbres, in_base_i, 16);
	sprintf(chp_dest + strlen(chp_dest),
		"<tr bgcolor=\"#%s\"><a name=\"%s\"></a>\n"
		, Db_GetValue(dbres, 0, 17), Db_GetValue(dbres, in_base_i, 0)
	);
	if(!strcmp(chp_layout, CO_TYPE_B9) || !strcmp(chp_layout, CO_TYPE_C9) || !strcmp(chp_layout, CO_TYPE_I)) {
		if(!strcmp(chp_layout, CO_TYPE_B9)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		if(chp_img && *chp_img != '\0') {
			sprintf(chp_dest + strlen(chp_dest), "<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0));
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
					, chp_defimagepath, chp_img, chp_value);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\">"
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME);
			}
			strcat(chp_dest, "</a></td>\n");
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		if(!strcmp(chp_layout, CO_TYPE_B9)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		if(strcmp(chp_layout, CO_TYPE_I)) {
			strcat(chp_dest, "<tr>\n");
			sprintf(chp_dest + strlen(chp_dest),
				"<td bgcolor=\"%s\" height=\"%s\"></td>\n"
				, Db_GetValue(dbres, 0, 19) , Db_GetValue(dbres, 0, 20)
			);
			strcat(chp_dest, "</tr>\n");
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B10) || !strcmp(chp_layout, CO_TYPE_C10) || !strcmp(chp_layout, CO_TYPE_J)) {
		if(!strcmp(chp_layout, CO_TYPE_B10)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		if(!strcmp(chp_layout, CO_TYPE_B10)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		if(strcmp(chp_layout, CO_TYPE_J)) {
			strcat(chp_dest, "<tr>\n");
			sprintf(chp_dest + strlen(chp_dest),
				"<td bgcolor=\"%s\" height=\"%s\"></td>\n"
				, Db_GetValue(dbres, 0, 19) , Db_GetValue(dbres, 0, 20)
			);
			strcat(chp_dest, "</tr>\n");
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_C11) || !strcmp(chp_layout, CO_TYPE_K)) {
		if(!strcmp(chp_layout, CO_TYPE_B11)) {
			chp_img = chp_thumbnail;
		} else {
			chp_img = chp_image;
		}
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		if(!strcmp(chp_layout, CO_TYPE_B11)) {
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		if(strcmp(chp_layout, CO_TYPE_K)) {
			strcat(chp_dest, "<tr>\n");
			sprintf(chp_dest + strlen(chp_dest),
				"<td bgcolor=\"%s\" height=\"%s\"></td>\n"
				, Db_GetValue(dbres, 0, 19) , Db_GetValue(dbres, 0, 20)
			);
			strcat(chp_dest, "</tr>\n");
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B12) || !strcmp(chp_layout, CO_TYPE_C12) || !strcmp(chp_layout, CO_TYPE_L)) {
		if(!strcmp(chp_layout, CO_TYPE_B12)) {
			chp_img = chp_thumbnail;
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, Db_GetValue(dbres, in_base_i, 2)
			);
		} else {
			chp_img = chp_image;
			chp_conv = Conv_Crlf(Db_GetValue(dbres, in_base_i, 3));
			sprintf(chp_dest + strlen(chp_dest),
				"<td valign=\"top\"><span class=\"small\">%s</span></td>\n"
				, chp_conv
			);
			free(chp_conv);
		}
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		strcat(chp_dest, "<td>\n");
		if(Gin_func_3) {
			in_ret = build_item_info_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		} else if(Gin_func_11) {
			in_ret = build_item_info_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
			if(in_ret) {
				return -1;
			}
		}
		strcat(chp_dest, "</td>\n");
		strcat(chp_dest, "</tr>\n");
		sprintf(chp_dest + strlen(chp_dest),
			"<tr bgcolor=\"#%s\">\n"
			, Db_GetValue(dbres, 0, 17)
		);
		if(chp_img && *chp_img != '\0') {
			memset(cha_buf, '\0', sizeof(cha_buf));
			sprintf(cha_buf, "%s/%s", chp_imagepath, chp_img);
			if(access(cha_buf, F_OK) == 0) {
				chp_value = Escape_HtmlString(Db_GetValue(dbres, in_base_i, 1));
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, chp_img, chp_value
					);
				free(chp_value);
			} else {
				sprintf(chp_dest + strlen(chp_dest),
					"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
					"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
					, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
					, chp_defimagepath, CO_PREPARATION_PICTURE_FILE, CO_PREPARATION_PICTURE_NAME
					);
			}
		} else {
			sprintf(chp_dest + strlen(chp_dest),
				"<td align=\"center\"><a href=\"%s/%s?id=%s\">"
				"<img src=\"%s/%s\" alt=\"%s\" border=\"0\"></a></td>\n"
				, chp_cgipath, CO_ACTION_DISP_ITEM_DETAIL, Db_GetValue(dbres, in_base_i, 0)
				, chp_dir, CO_NOT_PICTURE_FILE, CO_NOT_PICTURE_NAME
			);
		}
		if(strcmp(chp_layout, CO_TYPE_L)) {
			strcat(chp_dest, "<tr>\n");
			sprintf(chp_dest + strlen(chp_dest),
				"<td bgcolor=\"%s\" height=\"%s\"></td>\n"
				, Db_GetValue(dbres, 0, 19) , Db_GetValue(dbres, 0, 20)
			);
			strcat(chp_dest, "</tr>\n");
		}
	}
	return 0;
}

/*
+* ========================================================================
 * Function:		商品表示
 * Include-file:	disp.h
 * Description:
 *	表示タイプD-1,D-2用商品表示関数
%* ========================================================================
 * Return:	0以上:表示件数
 *			負数:エラー
-* ======================================================================*/
static int build_two_col4(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
, int in_btn
) {
	int in_ret;
	int i;

	strcat(chp_dest, "<tr valign=\"bottom\">\n");
	for(i = 0; i < 4; i++) {
		if(*inp_i >= in_row) {
			break;
		}
		if(Gin_func_3) {
			builditeminfo2_f3(nlp_in, dbres, chp_dest, inp_i, b_link, in_row);
		} else if(Gin_func_11) {
			in_ret = builditeminfo2_f11(db, nlp_in, nlp_out, dbres, inp_i, b_link, in_row, in_btn);
			if(in_ret) {
				return -1;
			}
		}
		*inp_i = *inp_i + 1;
	}
	*inp_i = *inp_i - 1;
	strcat(chp_dest, "</tr>\n");
	return i;
}

/*
+* ------------------------------------------------------------------------
 * Function:		put_col
 * Description:
 *	表示タイプE-1,E-2用商品表示関数
%* ------------------------------------------------------------------------
 * Return:	0以上:表示件数
 *			負数:エラー
-* ------------------------------------------------------------------------
 */
static int put_col(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_script
, char *chp_skl
, char *chp_undisp_itemid_list
, int *inp_i
, bool b_link
, int in_row
, int in_col
, int in_btn
) {
	char cha_str[256];
	int in_ret;
	int i;

	Put_Nlist(nlp_out, "ARTICLE", "<tr valign=\"bottom\">\n");
	for(i = 0; i < in_col; i++) {
		if(*inp_i >= in_row) {
			break;
		}
		if(Gin_func_13) {
			in_ret = put_item_info_f13(db, nlp_in, nlp_out, dbres, chp_script, chp_skl, inp_i, b_link, in_row, NULL);
		} else {
			in_ret = put_item_info(db, nlp_in, nlp_out, dbres, chp_script, chp_skl, chp_undisp_itemid_list, inp_i, b_link, in_row, NULL, in_btn);
		}
		if(in_ret) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d：ページの表示に失敗しました。", __LINE__);
			Put_Nlist(nlp_out, "ERROR", cha_str);
			return -1;
		}
		*inp_i = *inp_i + 1;
	}
	*inp_i = *inp_i - 1;
	Put_Nlist(nlp_out, "ARTICLE", "</tr>\n");
	return i;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_two_col2
 * Include-file:	disp.h
 * Description:
 *	表示タイプE-1,E-2用商品表示関数
%* ------------------------------------------------------------------------
 * Return:	0以上:表示件数
 *			負数:エラー
-* ------------------------------------------------------------------------
 */
static int build_two_col2(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
, int in_btn
) {
	int in_ret;
	int i;

	strcat(chp_dest, "<tr valign=\"bottom\">\n");
	for(i = 0; i < 2; i++) {
		if(*inp_i >= in_row) {
			break;
		}
		if(Gin_func_3) {
			builditeminfo2_f3(nlp_in, dbres, chp_dest, inp_i, b_link, in_row);
		} else if(Gin_func_11) {
			in_ret = builditeminfo2_f11(db, nlp_in, nlp_out, dbres, inp_i, b_link, in_row, in_btn);
			if(in_ret) {
				return -1;
			}
		}
		*inp_i = *inp_i + 1;
	}
	*inp_i = *inp_i - 1;
	strcat(chp_dest, "</tr>\n");
	return i;
}

/*
+* ========================================================================
 * Function:		BuildTable
 * Include-file:	disp.h
 * Description:
 *	表示タイプ別のテーブル作成関数を呼び出す
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
int Build_Item_Table(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_dest
, int *inp_i
, bool b_link
, int in_row
, int in_start
, int in_cnt
, int in_btn
) {
	char *chp_layout;
	int in_col;

	/* old type */
	in_col = 0;
	chp_layout = Db_GetValue(dbres, *inp_i, 16);
	if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
	 || !strcmp(chp_layout, CO_TYPE_A3) || !strcmp(chp_layout, CO_TYPE_A4)) {
		if(Gin_func_3) {
			in_col = build_typeA_f3(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, in_start, in_row, in_cnt);
		} else if(Gin_func_11) {
			in_col = build_typeA_f11(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, in_start, in_row, in_cnt);
		}
	} else if(!strcmp(chp_layout, CO_TYPE_B1) || !strcmp(chp_layout, CO_TYPE_C1)
	|| !strcmp(chp_layout, CO_TYPE_B2) || !strcmp(chp_layout, CO_TYPE_C2)
	|| !strcmp(chp_layout, CO_TYPE_B7) || !strcmp(chp_layout, CO_TYPE_C7)
	|| !strcmp(chp_layout, CO_TYPE_B8) || !strcmp(chp_layout, CO_TYPE_C8)
	|| !strcmp(chp_layout, CO_TYPE_A) || !strcmp(chp_layout, CO_TYPE_B)
	|| !strcmp(chp_layout, CO_TYPE_G) || !strcmp(chp_layout, CO_TYPE_H)) {
		in_col = build_standard(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
	} else if(!strcmp(chp_layout, CO_TYPE_B3) || !strcmp(chp_layout, CO_TYPE_C3)
	|| !strcmp(chp_layout, CO_TYPE_B4) || !strcmp(chp_layout, CO_TYPE_C4)
	|| !strcmp(chp_layout, CO_TYPE_B5) || !strcmp(chp_layout, CO_TYPE_C5)
	|| !strcmp(chp_layout, CO_TYPE_B6) || !strcmp(chp_layout, CO_TYPE_C6)
	|| !strcmp(chp_layout, CO_TYPE_C) || !strcmp(chp_layout, CO_TYPE_D)
	|| !strcmp(chp_layout, CO_TYPE_E) || !strcmp(chp_layout, CO_TYPE_F)) {
		in_col = build_exception(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
	} else if(!strcmp(chp_layout, CO_TYPE_B9) || !strcmp(chp_layout, CO_TYPE_C9)
	|| !strcmp(chp_layout, CO_TYPE_B10) || !strcmp(chp_layout, CO_TYPE_C10)
	|| !strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_C11)
	|| !strcmp(chp_layout, CO_TYPE_B12) || !strcmp(chp_layout, CO_TYPE_C12)
	|| !strcmp(chp_layout, CO_TYPE_I) || !strcmp(chp_layout, CO_TYPE_J)
	|| !strcmp(chp_layout, CO_TYPE_K) || !strcmp(chp_layout, CO_TYPE_L)) {
		in_col = build_three_col(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row);
	} else if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_D2)) {
		in_col = build_two_col4(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row, in_btn);
	} else if(!strcmp(chp_layout, CO_TYPE_E1) || !strcmp(chp_layout, CO_TYPE_E2)) {
		in_col = build_two_col2(db, nlp_in, nlp_out, dbres, chp_dest, inp_i, b_link, in_row, in_btn);
	}
	return in_col;
}
/*
+* ========================================================================
 * Function:		Build_Item_Table2
 * Include-file:	disp.h
 * Description:
 *	表示タイプ別のテーブル作成関数を呼び出す
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
int Build_Item_Table2(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_undisp_itemid_list
, int *inp_i
, bool b_link
, int in_row
, int in_start		/* typeA用 */
, int in_disp_no	/* typeA用 */
, int in_btn
) {
	NLIST *nlp_temp;
	char *chp_value;
	char *chp_script;
	char *chp_layout;
	char cha_skl[1024];
	char cha_str[512];
	int in_col;
	int in_vip_price;
	int in_ret;

	/* new type */
/*
	chp_value = getenv("SCRIPT_NAME");
	chp_script = strrchr(chp_value, '/');
	++chp_script;
	if(!strcmp(chp_script, CO_ACTION_DISP_CATEGORY)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_LIST);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_EVENT)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_LIST);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_NEW)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_NEW);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_HIT)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_HIT);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_ITEM)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_LIST);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL) || !strcmp(chp_script, CO_ACTION_CREATE_ITEM_HTML)) {
		chp_script = CO_ACTION_DISP_ITEM_DETAIL;
		if(Gin_func_13) {
			if(Db_GetValue(dbres, *inp_i, 0)) {
				memset(cha_skl, '\0', sizeof(cha_skl));
				sprintf(cha_skl, "%s%s%s", CO_F13_ITEM_DETAIL_SKELETON_PREFIX, Db_GetValue(dbres, *inp_i, 0), CO_F13_ITEM_DETAIL_SKELETON_SUFFIX);
			} else {
				memset(cha_skl, '\0', sizeof(cha_skl));
				strcpy(cha_skl, CO_SC_SKEL_DISP_ITEM_DETAIL);
			}
		} else {
			memset(cha_skl, '\0', sizeof(cha_skl));
			strcpy(cha_skl, CO_SC_SKEL_DISP_ITEM_DETAIL);
		}
	} else {
		cha_skl[0] = '\0';
	}
*/
	chp_script = CO_ACTION_DISP_ITEM;
	strcpy(cha_skl, CO_SC_SKEL_DISP_LIST);
	in_col = 0;
	chp_layout = Db_GetValue(dbres, *inp_i, 16) ? Db_GetValue(dbres, *inp_i, 16) : "";
	if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_D2)) {
		in_col = put_col(db, nlp_in, nlp_out, dbres, chp_script, cha_skl, chp_undisp_itemid_list, inp_i, b_link, in_row, 4, in_btn);
	} else if(!strcmp(chp_layout, CO_TYPE_E1) || !strcmp(chp_layout, CO_TYPE_E2)) {
		in_col = put_col(db, nlp_in, nlp_out, dbres, chp_script, cha_skl, chp_undisp_itemid_list, inp_i, b_link, in_row, 2, in_btn);
	/* 商品詳細 */
	} else if(!strcmp(chp_layout, CO_TYPE_A) || !strcmp(chp_layout, CO_TYPE_B)
		   || !strcmp(chp_layout, CO_TYPE_C) || !strcmp(chp_layout, CO_TYPE_D)
		   || !strcmp(chp_layout, CO_TYPE_E) || !strcmp(chp_layout, CO_TYPE_F)
		   || !strcmp(chp_layout, CO_TYPE_G) || !strcmp(chp_layout, CO_TYPE_H)
		   || !strcmp(chp_layout, CO_TYPE_I) || !strcmp(chp_layout, CO_TYPE_J)
		   || !strcmp(chp_layout, CO_TYPE_K) || !strcmp(chp_layout, CO_TYPE_L)
	/* 各商品表示 */
		   || !strcmp(chp_layout, CO_TYPE_A1)  || !strcmp(chp_layout, CO_TYPE_A2)
		   || !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)
		   || !strcmp(chp_layout, CO_TYPE_B1)  || !strcmp(chp_layout, CO_TYPE_B2)
		   || !strcmp(chp_layout, CO_TYPE_B3)  || !strcmp(chp_layout, CO_TYPE_B4)
		   || !strcmp(chp_layout, CO_TYPE_B5)  || !strcmp(chp_layout, CO_TYPE_B6)
		   || !strcmp(chp_layout, CO_TYPE_B7)  || !strcmp(chp_layout, CO_TYPE_B8)
		   || !strcmp(chp_layout, CO_TYPE_B9)  || !strcmp(chp_layout, CO_TYPE_B10)
		   || !strcmp(chp_layout, CO_TYPE_B11) || !strcmp(chp_layout, CO_TYPE_B12)
		   || !strcmp(chp_layout, CO_TYPE_C1)  || !strcmp(chp_layout, CO_TYPE_C2)
		   || !strcmp(chp_layout, CO_TYPE_C3)  || !strcmp(chp_layout, CO_TYPE_C4)
		   || !strcmp(chp_layout, CO_TYPE_C5)  || !strcmp(chp_layout, CO_TYPE_C6)
		   || !strcmp(chp_layout, CO_TYPE_C7)  || !strcmp(chp_layout, CO_TYPE_C8)
		   || !strcmp(chp_layout, CO_TYPE_C9)  || !strcmp(chp_layout, CO_TYPE_C10)
		   || !strcmp(chp_layout, CO_TYPE_C11) || !strcmp(chp_layout, CO_TYPE_C12)) {
		if(!strcmp(chp_layout, CO_TYPE_A1)  || !strcmp(chp_layout, CO_TYPE_A2)
		   || !strcmp(chp_layout, CO_TYPE_A3)  || !strcmp(chp_layout, CO_TYPE_A4)) {
			if(*inp_i == in_start) {
				nlp_temp = Init_Nlist();
				chp_value = Db_GetValue(dbres, 0, 41);
				if(chp_value && *chp_value == '1') {
					/* empty */
				} else {
					Put_Nlist(nlp_temp, "STANDARD_PRICE_START", "<!--STANDARD_PRICE_START");
					Put_Nlist(nlp_temp, "STANDARD_PRICE_END", "STANDARD_PRICE_END-->");
				}
				chp_value = Db_GetValue(dbres, 0, 42);
				if(chp_value && *chp_value == '1') {
					/* empty */
				} else {
					Put_Nlist(nlp_temp, "POINT_START", "<!--POINT_START");
					Put_Nlist(nlp_temp, "POINT_END", "POINT_END-->");
				}
				chp_value = Db_GetValue(dbres, 0, 37);	//login状態
				if(chp_value && *chp_value == '1') {
					if(Db_GetValue(dbres, 0, 35) && *Db_GetValue(dbres, 0, 35) == '2') {//表示フラグ
						in_vip_price = 2;
					} else if(Db_GetValue(dbres, 0, 35) && *Db_GetValue(dbres, 0, 35) == '1') {
						in_vip_price = 1;
					} else {
						in_vip_price = 0;
					}
				} else {
					in_vip_price = 0;
				}
				if(in_vip_price == 2) {
					Put_Nlist(nlp_temp, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
					Put_Nlist(nlp_temp, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
					Put_Nlist(nlp_temp, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
					Put_Nlist(nlp_temp, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
				} else if(in_vip_price == 1) {
					Put_Nlist(nlp_temp, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
					Put_Nlist(nlp_temp, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
					Put_Nlist(nlp_temp, "VIP_PRICE0_START", "<!--VIP_PRICE0_START");
					Put_Nlist(nlp_temp, "VIP_PRICE0_END", "VIP_PRICE0-_END-->");
				} else {
					Put_Nlist(nlp_temp, "VIP_PRICE2_START", "<!--VIP_PRICE2_START");
					Put_Nlist(nlp_temp, "VIP_PRICE2_END", "VIP_PRICE2_END-->");
					Put_Nlist(nlp_temp, "VIP_PRICE1_START", "<!--VIP_PRICE1_START");
					Put_Nlist(nlp_temp, "VIP_PRICE1_END", "VIP_PRICE1_END-->");
				}
				chp_value = Db_GetValue(dbres, 0, 31);
				if(chp_value && *chp_value == '1') {
					/* empty */
				} else {
					Put_Nlist(nlp_temp, "STOCK_START", "<!--STOCK_START");
					Put_Nlist(nlp_temp, "STOCK_END", "STOCK_END-->");
				}
				memset(cha_str, '\0', sizeof(cha_str));
				sprintf(cha_str, "%s/%s", GetCartSkeletonDirectory(), cha_skl);
				chp_value = Setpage_Out_Mem_To_Euc(nlp_temp, cha_str);
				Finish_Nlist(nlp_temp);
				if(!chp_value) {
					memset(cha_str, '\0', sizeof(cha_str));
					sprintf(cha_str, "%d：ページの表示に失敗しました。", __LINE__);
					Put_Nlist(nlp_out, "ERROR", cha_str);
					return -1;
				}
				Put_Nlist(nlp_out, "ARTICLE", chp_value);
				free(chp_value);
			}
			memset(cha_skl, '\0', sizeof(cha_skl));
			strcpy(cha_skl, CO_SC_SKEL_DISP_TYPEA_RECORD);
		}
		if(Gin_func_13) {
			in_ret = put_item_info_f13(db, nlp_in, nlp_out, dbres, chp_script, cha_skl, inp_i, b_link, in_row, in_disp_no);
		} else {
			in_ret = put_item_info(db, nlp_in, nlp_out, dbres, chp_script, cha_skl, chp_undisp_itemid_list, inp_i, b_link, in_row, in_disp_no, in_btn);
		}
		if(in_ret) {
			memset(cha_str, '\0', sizeof(cha_str));
			sprintf(cha_str, "%d：ページの表示に失敗しました。", __LINE__);
			Put_Nlist(nlp_out, "ERROR", cha_str);
			return -1;
		}
	}
	return in_col;
}

/*
+* ========================================================================
 * Function:		Build_Item_Table_Template
 * Include-file:	disp.h
 * Description:
 *	表示タイプ別のテーブル作成関数を呼び出す
%* ========================================================================
 * Return:	0以上:正常終了
 *			負数:エラー
-* ======================================================================*/
int Build_Item_Table_Template(
DBase *db	/* dbase */
, NLIST *nlp_in
, NLIST *nlp_out
, DBRes *dbres
, char *chp_undisp_itemid_list
, int *inp_i
, bool b_link
, int in_row
) {
	char *chp_value;
	char *chp_script;
	char cha_skl[1024];
	char cha_str[512];
	int in_col;
	int in_ret;

	/* new type */
	chp_value = getenv("SCRIPT_NAME");
	chp_script = strrchr(chp_value, '/');
	++chp_script;
	if(!strcmp(chp_script, CO_ACTION_DISP_CATEGORY)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_LIST);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_EVENT)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_LIST);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_NEW)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_NEW);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_HIT)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_HIT);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_ITEM)) {
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_LIST);
	} else if(!strcmp(chp_script, CO_ACTION_DISP_ITEM_DETAIL) || !strcmp(chp_script, CO_ACTION_CREATE_ITEM_HTML)) {
		chp_script = CO_ACTION_DISP_ITEM_DETAIL;
		memset(cha_skl, '\0', sizeof(cha_skl));
		strcpy(cha_skl, CO_SC_SKEL_DISP_ITEM_DETAIL);
	} else {
		cha_skl[0] = '\0';
	}
	in_col = 0;
	in_ret = Put_Template_Item_Info(db, nlp_in, nlp_out, dbres, chp_script, cha_skl, chp_undisp_itemid_list, inp_i, b_link, in_row);
	if(in_ret) {
		memset(cha_str, '\0', sizeof(cha_str));
		sprintf(cha_str, "%d：ページの表示に失敗しました。", __LINE__);
		Put_Nlist(nlp_out, "ERROR", cha_str);
		return -1;
	}
	return in_col;
}

/*
+* ========================================================================
 * Function:		共通商品データ取得
 * Include-file:	disp.h
 * Description:
 *
%* ========================================================================
 * Return:	なし
-* ======================================================================*/
static void get_item_data_f3(DBRes *dbres, char *chp_price, int *inp_count, int in_subitem)
{
	char *chp_value;

	/* 商品表示に消費税使わない */
	/* 販売価格 */
	chp_value = Db_GetValue(dbres, in_subitem, 8);
	if(!chp_value || *chp_value == '\0') {
		chp_value = "0";
	}
	strcpy(chp_price, chp_value);
	Edit_Money(chp_price);
	strcat(chp_price, "円");
	/* 有効在庫 */
	chp_value = Db_GetValue(dbres, in_subitem, 32);
	if(!chp_value || *chp_value == '\0') {
		chp_value = "0";
	}
	*inp_count = atoi(chp_value);
	return;
}
