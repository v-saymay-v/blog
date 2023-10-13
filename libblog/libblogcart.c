/*
+* ------------------------------------------------------------------------
 * Module-Name:         libblogcart.c
 * First-Created:       2006/11/01 �� ����
%* ------------------------------------------------------------------------
 * Module-Description:
 *	blog��ǧ�ڴ�Ϣ�Υ饤�֥�ꡣ
-* ------------------------------------------------------------------------
 * Change-Log:
$* ------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <libcgi2.h>
#include <libnet.h>
#include "excart.h"
#include "libexcart.h"
#include "libblog.h"
#include "libauth.h"
#include "dispcart.h"
#include "libblogcart.h"

#define CO_ITEM_MULTI_ID (CO_ITEM_ID_LENGTH * 3) + 32
#define CO_MENU_STRING_LENGTH 32
#define CO_SC_CHAR_LIST0 "*"
#define CO_SC_CHAR_LIST1 "��"
#define CO_SC_CHAR_LIST2 "��"
#define CO_SC_CHAR_LIST3 "��"
#define CO_SC_CHAR_LIST4 "������"

int Gin_func_3;
int gin_func_10;
int Gin_func_11;
int Gin_func_12;
int Gin_func_13;
char g_cha_cartuser[21];
char g_cha_database[256];
char g_cha_imageloc[256];

/*
+* ========================================================================
 * Function:	�������ѤΥǡ����١����򥪡��ץ󤹤�
 * Description:
 *	�ǡ����١������饫������DB�Υ��������������Ф��������ץ󤹤롣
%* ========================================================================
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
=* ======================================================================*/
DBase* Open_Cart_Db(DBase *db, NLIST *nlp_out, int in_blog)
{
	char cha_host[256];
	char cha_userid[21];
	char cha_password[21];
	char cha_passfile[256];
	char cha_sql[1024];
	char *chp_tmp;
	DBRes *dbres;
	DBase *cartdb;
	FILE *fp;
	struct passwd *stp_pwd;

	sprintf(cha_sql, "select c_blog_id from at_blog where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "���ʤξ�������륯����˼��Ԥ��ޤ�����<br>%s<br>%s", Gcha_last_error, cha_sql);
		return NULL;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "���ʤξ�������륯����˼��Ԥ��ޤ�����<br>");
		Db_CloseDyna(dbres);
		return NULL;
	}
	strcpy(g_cha_cartuser, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "");
	Db_CloseDyna(dbres);

	sprintf(cha_sql,
		" select c_host"
		",c_database"
		",c_passfile"
		",c_imageloc"
		",c_purchase_cgi"
		" from sy_cartinfo");
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "���ʤξ�������륯����˼��Ԥ��ޤ�����<br>%s<br>%s", Gcha_last_error, cha_sql);
		return NULL;
	}
	if (Db_FetchNext(dbres) != CO_SQL_OK) {
		Put_Nlist(nlp_out, "ERROR", "���ʤξ�������륯����˼��Ԥ��ޤ�����<br>");
		Db_CloseDyna(dbres);
		return NULL;
	}
	strcpy(cha_host, Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "");
	strcpy(g_cha_database, Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "");
	strcpy(cha_passfile, Db_FetchValue(dbres, 2) ? Db_FetchValue(dbres, 2) : "");
	strcpy(g_cha_imageloc, Db_FetchValue(dbres, 3) ? Db_FetchValue(dbres, 3) : "");
	strcpy(g_cha_purchase_cgi, Db_FetchValue(dbres, 4) ? Db_FetchValue(dbres, 4) : "");
	Db_CloseDyna(dbres);

	stp_pwd = getpwnam(g_cha_cartuser);
	if (stp_pwd) {
		Replace_String("#USERDIR#", stp_pwd->pw_dir, cha_passfile);
	}

	fp = fopen(cha_passfile, "r");
	if (!fp) {
		Put_Format_Nlist(nlp_out, "ERROR", "�ѥ���ɤ��ɤ߹��ߤ˼��Ԥ��ޤ�����<br>%s<br>%s", strerror(errno), cha_passfile);
		return NULL;
	}
	fgets(cha_passfile, sizeof(cha_passfile), fp);
	fclose(fp);
	chp_tmp = strchr(cha_passfile, '\n');
	if (chp_tmp)
		*chp_tmp = '\0';
	chp_tmp = strchr(cha_passfile, '\r');
	if (chp_tmp)
		*chp_tmp = '\0';
	chp_tmp = strtok(cha_passfile, ":");
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�ѥ���ɤη������ְ�äƤ��ޤ���<br>");
		return NULL;
	}
	strcpy(cha_userid, chp_tmp);
	chp_tmp = strtok(NULL, ":");
	if (!chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�ѥ���ɤη������ְ�äƤ��ޤ���<br>");
		return NULL;
	}
	strcpy(cha_password, chp_tmp);
	Replace_String("#DBUSER#", cha_userid, g_cha_database);

	if (g_in_ssl_mode) {
		cartdb = Db_ConnectSSL(cha_host, g_cha_database, cha_userid, cha_password);
	} else {
		cartdb = Db_ConnectWithParam(cha_host, g_cha_database, cha_userid, cha_password);
	}
	if (!cartdb) {
		Put_Format_Nlist(nlp_out, "ERROR", "������DB�ؤ���³�˼��Ԥ��ޤ�����<br>%s", Gcha_last_error);
		return NULL;
	}

	Gin_func_3 = 0;
	gin_func_10 = 0;
	Gin_func_11 = 0;
	Gin_func_12 = 0;
	Gin_func_13 = 0;

	strcpy(cha_sql, "select coalesce(T1.b_use, 0)");	/* 0 */
	strcat(cha_sql, " from sy_function T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_function_id in (%d, %d, %d, %d, %d)"
		, CO_SC_FUNCTION_NUMBER_3, CO_SC_FUNCTION_NUMBER_10
		, CO_SC_FUNCTION_NUMBER_11, CO_SC_FUNCTION_NUMBER_12, CO_SC_FUNCTION_NUMBER_13);
	sprintf(cha_sql + strlen(cha_sql), " order by T1.n_function_id");
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(dbres) {
		if (Db_GetRowCount(dbres) == 5) {
			Gin_func_3 = *Db_GetValue(dbres, 0, 0) != '\0' ? atoi(Db_GetValue(dbres, 0, 0)) : 0;
			gin_func_10 = *Db_GetValue(dbres, 1, 0) != '\0' ? atoi(Db_GetValue(dbres, 1, 0)) : 0;
			Gin_func_11 = *Db_GetValue(dbres, 2, 0) != '\0' ? atoi(Db_GetValue(dbres, 2, 0)) : 0;
			Gin_func_12 = *Db_GetValue(dbres, 3, 0) != '\0' ? atoi(Db_GetValue(dbres, 3, 0)) : 0;
			Gin_func_13 = *Db_GetValue(dbres, 4, 0) != '\0' ? atoi(Db_GetValue(dbres, 4, 0)) : 0;
		}
		Db_CloseDyna(dbres);
	}

	return cartdb;
}

/*
+* ------------------------------------------------------------------------
 * Function:	�֥������ѤǤ��뤫
 * Description:
 *	�֥������ѤǤ��뤫��Ƚ�̤��롣
%* ------------------------------------------------------------------------
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
-* ----------------------------------------------------------------------*/
int Blog_Usable(DBase *db, NLIST *nlp_out, int in_blog)
{
	DBase *db_cart;
	DBRes *dbres;
	char *chp_tmp;
	int in_ret;

	db_cart = Open_Cart_Db(db, nlp_out, in_blog);
	if (!db_cart) {
		return -1;
	}
	dbres = Db_OpenDyna(db_cart, "select b_use from sy_function where n_function_id = 16;");
	if (!dbres) {
		Put_Format_Nlist(nlp_out, "ERROR", "%d�������Ⱦ���μ����˼��Ԥ��ޤ�����(%s)", __LINE__, Gcha_last_error);
		return -1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	in_ret = (chp_tmp ? atoi(chp_tmp) : 0);
	Db_CloseDyna(dbres);

	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:	�ڡ������ϥꥹ�Ⱥ���
 * Description:
 *	�ǡ����١������龦�ʤ򸡺����ڡ������ϥꥹ�Ⱥ�������
%* ------------------------------------------------------------------------
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
-* ----------------------------------------------------------------------*/
int build_page(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_cookie, char *chp_item_id, int in_btn, int *inp_display)
{
	DBRes *dbres;
	char *chp_query;
	char *chp_undisp_itemid_list;
	char *chp_value;
	char *chp_layout;
	char cha_sql[8192];
	char cha_bgcolor[32] = "";
	char cha_border[32] = "";
	char cha_border2[32] = "";
	char cha_bordercolor[32] = "";
	char cha_cellpadding[32] = "";
	char cha_width[32];
	char cha_height[32];
	bool b_link;
	int in_stock_mode;
	int in_login;
	int in_row;
	int in_ret;
	int in_rank;
	int i;

	*inp_display = 0;
	Get_Cart_Handytype();
	in_login = Is_Logged_In(OldDBase(db), nlp_in, nlp_cookie);
	in_rank = CO_SC_DEFAULT_NON_CUSTOMER;
	if(in_login) {
		in_rank = Get_CustomerRank(OldDBase(db), atoi(Get_Nlist(nlp_cookie, "LOGINID", 1)));
	}
	Build_Hidden(nlp_out, "RELATED", "id", chp_item_id);
	/* ɽ�����ʤ�����ID */
	chp_undisp_itemid_list = NULL;
	in_ret = Un_Disp_Item(OldDBase(db), nlp_in, nlp_out, nlp_cookie, &chp_undisp_itemid_list, "T2", in_login, 'N');
	if(in_ret != 0) {
		return 1;
	}
	chp_query = (char *)malloc((CO_SC_MAX_BUF * 2 + strlen(chp_undisp_itemid_list)) * sizeof(char));
	if(chp_query == NULL) {
		free(chp_undisp_itemid_list);
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d�����ʾ���μ����˼��Ԥ��ޤ�����", __LINE__);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	in_ret = Set_Cart_Node(OldDBase(db), nlp_out, chp_item_id, CO_NODE_ITEM_DETAIL);
	if(in_ret != 0) {
		free(chp_query);
		free(chp_undisp_itemid_list);
		return 1;
	}
	in_ret = Create_TempPriceRank(OldDBase(db), nlp_in, nlp_out, nlp_cookie);
	if(in_ret) {
		return 1;
	}
	in_stock_mode = Get_Stock_Detail_Mode(OldDBase(db), nlp_out);
	if(in_stock_mode) {
		in_ret = Create_TempStockNow(OldDBase(db), nlp_out);
		if(in_ret) {
			return 1;
		}
	}
	/* ���ʾ������� */
	memset(chp_query, '\0', sizeof(chp_query));
	strcpy(chp_query, "select T1.c_item_id");	/* 0 */
	strcat(chp_query, ", T1.c_name");		/* 1 */
	strcat(chp_query, ", T1.c_guide");		/* 2 */
	strcat(chp_query, ", T1.c_guide_detail");	/* 3 */
	strcat(chp_query, ", T1.c_thumbnail");		/* 4 */
	strcat(chp_query, ", ''");			/* 5 */
	strcat(chp_query, ", T1.c_url");		/* 6 */
	strcat(chp_query, ", T2.n_standard_price");	/* 7 */
	strcat(chp_query, ", coalesce(T320.n_selling_price, T310.n_selling_price)");	/* 8 */
	strcat(chp_query, ", T2.n_postage");		/* 9 */
	strcat(chp_query, ", T2.c_item_type");		/* 10 */
	strcat(chp_query, ", T20.c_sub_id");		/* 11 */
	strcat(chp_query, ", T20.c_name");		/* 12 */
	strcat(chp_query, ", T2.c_item_type");		/* 13 */
	strcat(chp_query, ", T2.b_stock");		/* 14 */
	strcat(chp_query, ", T2.n_min_stock");		/* 15 */
	strcat(chp_query, ", T5.n_layout_detail");	/* 16 */
	strcat(chp_query, ", coalesce(T5.c_back1, '#ffffff')");	/* 17 */
	strcat(chp_query, ", coalesce(T5.c_back2, '#ffffff')");	/* 18 */
	strcat(chp_query, ", T5.c_line");			/* 19 */
	strcat(chp_query, ", T5.n_width");			/* 20 */
	strcat(chp_query, ", T5.n_list_max");			/* 21 */
	strcat(chp_query, ", T6.n_postage_id");					/* 22 */
	strcat(chp_query, ", if(T1.b_postage_apply = 1, T6.n_postage, '')");	/* 23 */
	strcat(chp_query, ", T5.n_layout");		/* 24 */
	strcat(chp_query, ", T6.b_post_price");		/* 25 */
	strcat(chp_query, ", T6.n_post_price");		/* 26 */
	strcat(chp_query, ", if(T1.b_postage_apply = 1, T6.n_prime_postage, '')");	/* 27 */
	strcat(chp_query, ", T6.n_consumption_tax");	/* 28 */
	strcat(chp_query, ", T6.n_tax_id");		/* 29���ɽ���λ��� */
	strcat(chp_query, ", T6.n_fraction_id");	/* 30ü�������λ��� */
	strcat(chp_query, ", T6.b_stock");		/* 31 */
	if(in_stock_mode) {
		strcat(chp_query, ", coalesce(T25.n_count, T24.n_count)");	/* 32 */
	} else {
		strcat(chp_query, ", T3.n_count");	/* 32 */
	}
	strcat(chp_query, ", T6.b_postage_tax");	/* 33 */
	strcat(chp_query, ", T8.c_name");		/* 34 */
	strcat(chp_query, ", T6.n_vip_price");		/* 35������Ѳ��ʤˤ������λ��� */
	strcat(chp_query, ", coalesce(T321.n_selling_price, T311.n_selling_price)");	/* 36 */
	if(in_login == 0) {
		strcat(chp_query, ", 0");		/* 37 */
	} else {
		strcat(chp_query, ", 1");		/* 37 */
	}
	strcat(chp_query, ", T2.c_item_id");		/* 38 */
	strcat(chp_query, ", T6.b_reserve");		/* 39 */
	strcat(chp_query, ", T3.n_reserve_count");	/* 40 */
	strcat(chp_query, ", T6.b_standard_price");	/* 41 */
	strcat(chp_query, ", T6.b_point");		/* 42 */
	strcat(chp_query, ", T2.n_point_rate");		/* 43 */
	strcat(chp_query, ", T6.c_comment_stock");	/* 44 */
	strcat(chp_query, ", T6.c_comment_reserve");	/* 45 */
	strcat(chp_query, ", T9.b_use");		/* 46 */
	strcat(chp_query, ", T10.b_use");		/* 47 */
	strcat(chp_query, ", coalesce(T9.n_count, 0)");	/* 48 */
	strcat(chp_query, ", T9.c_color");		/* 49 */
	strcat(chp_query, ", if(T1.b_postage_apply = 1, T11.c_special_name, '')");	/* 50 */
	strcat(chp_query, ", T12.c_text_emphasis");	/* 51 */
	strcat(chp_query, ", T5.n_tabletype_width");	/* 52 */
	strcat(chp_query, ", T5.n_tabletype_height");	/* 53 */
	strcat(chp_query, ", T5.n_tabletype_cellspacing");	/* 54 */
	strcat(chp_query, ", T5.n_tabletype_cellpadding");	/* 55 */
	strcat(chp_query, ", ''");		/* 56 */
	strcat(chp_query, ", ''");		/* 57 */
	strcat(chp_query, ", T21.c_sub_id");		/* 58 ���֥Хꥨ�������2 */
	strcat(chp_query, ", T21.c_name");		/* 59 ����̾�� */
	strcat(chp_query, ", ''");		/* 60 */
	strcat(chp_query, ", ''");		/* 61 */
	strcat(chp_query, ", ''");			/* 62 */
	strcat(chp_query, ", T14.b_use");		/* 63 ����ʣ�� */
	strcat(chp_query, ", T14.n_max");		/* 64 ʣ���������� */
	strcat(chp_query, ", T14.n_step");		/* 65 �ֳ� */
	strcat(chp_query, ", T2.n_cool");		/* 66 �������ؾ��� */
	strcat(chp_query, ", T15.n_cool_postage_cold");	/* 67 ��¢������������ */
	strcat(chp_query, ", T15.n_cool_postage_frozen");	/* 68 ���९���������� */
	strcat(chp_query, ", T15.b_cool_discount");	/* 69 �����Ŭ�� */
	strcat(chp_query, ", coalesce(T5.b_disp_non_postage, 0)");	/* 70 �����������ߡ�ɽ������ */
	strcat(chp_query, ", T5.c_name_special");	/* 71 �������ɲ�������̾�� */
	strcat(chp_query, ", T1.n_thumbnail_width");	/* 72 ����(��)�������� */
	strcat(chp_query, ", T1.n_thumbnail_height");	/* 73 ����(��)�������� */
	strcat(chp_query, ", T5.n_list_image_size");	/* 74 ����(��)ɽ�����ꥵ���� */
	strcat(chp_query, ", T22.c_name");		/* 75 �ݥ����ñ�� */
	strcat(chp_query, " from sm_item T1");
	strcat(chp_query, " left join sm_maker T8");
	strcat(chp_query, " on T1.c_maker_id = T8.c_maker_id");
	strcat(chp_query, ", sm_subitem T20");
	strcat(chp_query, ", sm_subitem T21");
	strcat(chp_query, ", sm_subitem_combo T2");
	strcat(chp_query, ", st_stock T3");
	if(in_stock_mode) {
		strcat(chp_query, " left join temp_stock_now T25");
		strcat(chp_query, " on T3.c_item_id = T25.c_item_id");
		strcat(chp_query, " and T3.c_sub1_id = T25.c_sub1_id");
		strcat(chp_query, " and T3.c_sub2_id = T25.c_sub2_id");
		strcat(chp_query, ", st_stock_item_set_count T24");
	}
	strcat(chp_query, ", st_item_price_rank T310");
	strcat(chp_query, " left join temp_price_rank T320");
	strcat(chp_query, " on T310.c_item_id = T320.c_item_id");
	strcat(chp_query, " and T310.c_sub1_id = T320.c_sub1_id");
	strcat(chp_query, " and T310.c_sub2_id = T320.c_sub2_id");
	strcat(chp_query, " and T310.n_rank = T320.n_rank");
	strcat(chp_query, ", st_item_price_rank T311");
	strcat(chp_query, " left join temp_price_rank2 T321");
	strcat(chp_query, " on T311.c_item_id = T321.c_item_id");
	strcat(chp_query, " and T311.c_sub1_id = T321.c_sub1_id");
	strcat(chp_query, " and T311.c_sub2_id = T321.c_sub2_id");
	strcat(chp_query, " and T311.n_rank = T321.n_rank");
	strcat(chp_query, ", st_customer_rank T312");
	strcat(chp_query, ", sm_charge T6");
	strcat(chp_query, ", sm_point_unit T22");
	strcat(chp_query, ", sm_style_item_list T5");
	strcat(chp_query, ", sm_review T9");
	strcat(chp_query, ", sm_inquire T10");
	strcat(chp_query, ", sm_postage_status T11");
	strcat(chp_query, ", sm_style_other T12");
	strcat(chp_query, ", sm_plural_item_add T14");
	strcat(chp_query, ", sm_cool_postage T15");
	sprintf(chp_query + strlen(chp_query), " where T1.c_item_id = '%s'", chp_item_id);
	strcat(chp_query, chp_undisp_itemid_list);
	strcat(chp_query, " and T2.b_disp = 1");
	strcat(chp_query, " and T2.b_public = 1");
	strcat(chp_query, " and T1.c_item_id = T2.c_item_id");
	strcat(chp_query, " and T2.c_item_id = T20.c_item_id");
	strcat(chp_query, " and T20.c_sub_id = T2.c_sub1_id");
	sprintf(&chp_query[strlen(chp_query)], " and T20.n_group = %d", CO_SUBITEM_GROUP_1);
	strcat(chp_query, " and T2.c_item_id = T21.c_item_id");
	sprintf(&chp_query[strlen(chp_query)], " and T21.n_group = %d", CO_SUBITEM_GROUP_2);
	strcat(chp_query, " and T21.c_sub_id = T2.c_sub2_id");
	strcat(chp_query, " and T2.c_item_id = T3.c_item_id");
	strcat(chp_query, " and T2.c_sub1_id = T3.c_sub1_id");
	strcat(chp_query, " and T2.c_sub2_id = T3.c_sub2_id");
	if(in_stock_mode) {
		strcat(chp_query, " and T3.c_item_id = T24.c_item_id");
		strcat(chp_query, " and T3.c_sub1_id = T24.c_sub1_id");
		strcat(chp_query, " and T3.c_sub2_id = T24.c_sub2_id");
	}
	strcat(chp_query, " and T6.n_point_unit = T22.n_point_unit");
	strcat(chp_query, " and (T6.b_disp_soldout = 1");
	strcat(chp_query, " or (T2.b_stock = 0");
	strcat(chp_query, " or (T2.b_stock = 1");
	if(in_stock_mode) {
		strcat(chp_query, " and (coalesce(T25.n_count, T24.n_count) > 0");
	} else {
		strcat(chp_query, " and (T3.n_count > 0");
	}
	strcat(chp_query, " or (T6.b_reserve = 1");
	strcat(chp_query, " and coalesce(T3.n_reserve_count, 0) > 0)))))");
	strcat(chp_query, " and T2.c_item_id = T310.c_item_id");
	strcat(chp_query, " and T2.c_item_id = T311.c_item_id");
	strcat(chp_query, " and T2.c_sub1_id = T310.c_sub1_id");
	strcat(chp_query, " and T2.c_sub1_id = T311.c_sub1_id");
	strcat(chp_query, " and T2.c_sub2_id = T310.c_sub2_id");
	strcat(chp_query, " and T2.c_sub2_id = T311.c_sub2_id");
	sprintf(&chp_query[strlen(chp_query)], " and T310.n_rank = %d", CO_SC_DEFAULT_NON_CUSTOMER);
	sprintf(&chp_query[strlen(chp_query)], " and T311.n_rank = if(T6.n_vip_price = 0, %d, %d)"
			, -1, in_rank);
	strcat(chp_query, " and (T311.n_selling_price is not null");
	strcat(chp_query, " or T321.n_selling_price is not null)");
	strcat(chp_query, " and T311.n_rank = T312.n_rank");
	strcat(chp_query, " order by T20.n_sec, T21.n_sec");
	dbres = Db_OpenDyna(db, chp_query);
	free(chp_query);
	if(!dbres) {
		free(chp_undisp_itemid_list);
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d�����ʾ���μ����˼��Ԥ��ޤ�����(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if(in_row <= 0) {
		free(chp_undisp_itemid_list);
		Db_CloseDyna(dbres);
		return 0;
	}
	chp_layout = Db_GetValue(dbres, 0, 16) ? Db_GetValue(dbres, 0, 16) : "";
	cha_width[0] = '\0';
	cha_height[0] = '\0';
	if(Gin_func_13) {
		/* empty */
	} else {
		if(!strcmp(chp_layout, CO_TYPE_D1) || !strcmp(chp_layout, CO_TYPE_D2)
		|| !strcmp(chp_layout, CO_TYPE_E1) || !strcmp(chp_layout, CO_TYPE_E2)) {
			/* empty */
		} else {
			chp_value = Db_GetValue(dbres, 0, 52);
			if(!chp_value) {
				chp_value = "";
			}
			strcpy(cha_width, chp_value);
			chp_value = Db_GetValue(dbres, 0, 53);
			if(!chp_value) {
				chp_value = "";
			}
			strcpy(cha_height, chp_value);
		}
	}
	chp_value = Db_GetValue(dbres, 0, 19);
	if(chp_value) {
		strcpy(cha_bordercolor, chp_value);
	}
	if(Gin_func_13) {
		chp_value = Db_GetValue(dbres, 0, 57) ? Db_GetValue(dbres, 0, 57) : "";
		strcpy(cha_border2, chp_value);
		strcpy(cha_bgcolor, chp_value);
		strcpy(cha_border, "0");
	} else {
		strcpy(cha_border2, Db_GetValue(dbres, 0, 54) ? Db_GetValue(dbres, 0, 54) : "");
		strcpy(cha_cellpadding, Db_GetValue(dbres, 0, 55) ? Db_GetValue(dbres, 0, 55) : "");
		if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
		 || !strcmp(chp_layout, CO_TYPE_A3) || !strcmp(chp_layout, CO_TYPE_A4)) {
			strcpy(cha_bgcolor, Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : "");
		} else {
			strcpy(cha_bgcolor, Db_GetValue(dbres, 0, 18) ? Db_GetValue(dbres, 0, 18) : "");
		}
		strcpy(cha_border, Db_GetValue(dbres, 0, 20) ? Db_GetValue(dbres, 0, 20) : "");
	}
	Put_Nlist(nlp_out, "ARTICLE", "<table width=\"90%\" border=\"0\" cellspacing=\"3\" cellpadding=\"2\" class=\"cart-table\" align=\"center\">\n");
	if(Gin_func_13) {
		Put_Nlist(nlp_out, "ARTICLE", "<tr>\n");
		Put_Nlist(nlp_out, "ARTICLE", "\t<td>\n");
	}
	/* ����ɽ�� */
	b_link = 0;
	i = 0;
	in_ret = Build_Item_Table2(db, nlp_in, nlp_out, dbres, chp_undisp_itemid_list, &i, b_link, in_row, 0, 0, in_btn);
	Db_CloseDyna(dbres);
	free(chp_undisp_itemid_list);
	if(Gin_func_13) {
		Put_Nlist(nlp_out, "ARTICLE", "\t</td>\n");
		Put_Nlist(nlp_out, "ARTICLE", "</tr>\n");
	}
	if(in_ret < 0) {
		return 1;
	}
	Put_Nlist(nlp_out, "ARTICLE", "</table>\n");
	*inp_display = 1;
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	�ڡ������ϥꥹ�Ⱥ���F3
 * Description:
 *	�ǡ����١������龦�ʤ򸡺����ڡ������ϥꥹ�Ⱥ�������
%* ------------------------------------------------------------------------
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
-* ----------------------------------------------------------------------*/
int build_page_f3(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_cookie, char *chp_item_id, int in_btn, int *inp_display)
{
	DBRes *dbres;
	char *chp_query;
	char *chp_undisp_itemid_list;
	char *chp_value;
	char *chp_layout;
	char cha_buf[122880];	/* CO_MAX_VARIATION_COUNT(60) * tags + tabletag */
	char cha_sql[8192];
	char cha_login[256];
	char cha_sub_id[256];
	char cha_bgcolor[32] = "";
	char cha_border[32] = "";
	char cha_bordercolor[32] = "";
	bool b_link;
	int in_start;
	int in_cnt;
	int in_login;
	int in_row;
	int in_acting;
	int in_ret;
	int i;

	*inp_display = 0;
	Get_Cart_Handytype();
	in_login = Is_Logged_In(OldDBase(db), nlp_in, nlp_cookie);
	memset(cha_login, '\0', sizeof(cha_login));
	if(in_login == 1) {
		strcpy(cha_login, Get_Nlist(nlp_cookie, "LOGINID", 1));
	}
	Build_Hidden(nlp_out, "RELATED", "id", chp_item_id);
	/* ɽ�����ʤ�����ID */
	chp_undisp_itemid_list = NULL;
	in_ret = Un_Disp_Item(OldDBase(db), nlp_in, nlp_out, nlp_cookie, &chp_undisp_itemid_list, "T3", in_login, 'N');
	if(in_ret != 0) {
		return 1;
	}
	chp_query = (char *)malloc((CO_SC_MAX_BUF * 2 + strlen(chp_undisp_itemid_list)) * sizeof(char));
	if(chp_query == NULL) {
		free(chp_undisp_itemid_list);
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d�����ʾ���μ����˼��Ԥ��ޤ�����", __LINE__);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	in_acting = CO_FUNCTION_3_ACTING_GUEST;
	if(in_login == 0) {
		memset(cha_sub_id, '\0', sizeof(cha_sub_id));
		sprintf(cha_sub_id, "'%d'", CO_FUNCTION_3_MODE_OPEN_PRICE);	/* ���� */
	} else {
		memset(cha_login, '\0', sizeof(cha_login));
		strcpy(cha_login, Get_Nlist(nlp_cookie, "LOGINID", 1));
		/* query */
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "select coalesce(T1.n_acting, %d)", CO_FUNCTION_3_ACTING_CUSTOMER);	/* 0 ������� */
		sprintf(cha_sql + strlen(cha_sql), ", coalesce(T2.n_price_rank, %d)", CO_FUNCTION_3_MODE_OPEN_PRICE);	/* 1 ���ʥ�� */
		strcat(cha_sql, " from st_customer T1");
		strcat(cha_sql, " left join st_customer T2");
		strcat(cha_sql, " on T2.c_user = T1.c_patron");
		sprintf(cha_sql + strlen(cha_sql), " where T1.n_customer_no = %s", cha_login);
		dbres = Db_OpenDyna(db, cha_sql);
		if(!dbres) {
			free(chp_undisp_itemid_list);
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d�������������˼��Ԥ��ޤ�����(%s)<br>", __LINE__, Gcha_last_error);
			Put_Nlist(nlp_out, "ERROR", cha_sql);
			return 1;
		}
		if(Db_GetRowCount(dbres) != 1) {
			Db_CloseDyna(dbres);
			free(chp_undisp_itemid_list);
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d�������������˼��Ԥ��ޤ�����<br>", __LINE__);
			Put_Nlist(nlp_out, "ERROR", cha_sql);
			return 1;
		} else {
			in_acting = atoi(Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "0");
			switch(in_acting) {
			case CO_FUNCTION_3_ACTING_CUSTOMER:
				memset(cha_sub_id, '\0', sizeof(cha_sub_id));
				strcpy(cha_sub_id, "T11.n_price_rank");
				break;
			case CO_FUNCTION_3_ACTING_TRANSFER:
				memset(cha_sub_id, '\0', sizeof(cha_sub_id));
				sprintf(cha_sub_id, "'%d'", CO_FUNCTION_3_ACTING_TRANSFER_PRICE_RANK);
				break;
			case CO_FUNCTION_3_ACTING_PATRON:
				memset(cha_sub_id, '\0', sizeof(cha_sub_id));
				sprintf(cha_sub_id, "'%s'", Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "");
				break;
			default:
				memset(cha_sub_id, '\0', sizeof(cha_sub_id));
				sprintf(cha_sub_id, "'%d'", CO_FUNCTION_3_MODE_OPEN_PRICE);	/* ���� */
				break;
			}
		}
		Db_CloseDyna(dbres);
	}
	/* ���ʾ������� */
	memset(chp_query, '\0', sizeof(chp_query));
	strcpy(chp_query, "select T1.c_item_id");	/* 0 */
	strcat(chp_query, ", T1.c_name");			/* 1 */
	strcat(chp_query, ", T1.c_guide");			/* 2 */
	strcat(chp_query, ", T1.c_guide_detail");	/* 3 */
	strcat(chp_query, ", T1.c_thumbnail");		/* 4 */
	strcat(chp_query, ", T1.c_image");			/* 5 */
	strcat(chp_query, ", T1.c_url");			/* 6 */
	strcat(chp_query, ", T2.n_standard_price");	/* 7 */
	strcat(chp_query, ", T2.n_selling_price");	/* 8 */
	strcat(chp_query, ", T2.n_postage");		/* 9 */
	strcat(chp_query, ", T2.c_item_type");		/* 10 */
	strcat(chp_query, ", T2.c_sub_id");			/* 11 */
	strcat(chp_query, ", T2.c_name");			/* 12 */
	strcat(chp_query, ", T2.c_item_type");		/* 13 */
	strcat(chp_query, ", T2.b_stock");			/* 14.�߸˴��� */
	strcat(chp_query, ", T2.n_min_stock");		/* 15 */
	strcat(chp_query, ", T5.n_layout_detail");	/* 16 */
	strcat(chp_query, ", coalesce(T5.c_back1, '#ffffff')");	/* 17 */
	strcat(chp_query, ", coalesce(T5.c_back2, '#ffffff')");	/* 18 */
	strcat(chp_query, ", T5.c_line");			/* 19 */
	strcat(chp_query, ", T5.n_width");			/* 20 */
	strcat(chp_query, ", T5.n_list_max");		/* 21 */
	strcat(chp_query, ", T6.n_postage_id");		/* 22 */
	strcat(chp_query, ", T6.n_postage");		/* 23 */
	strcat(chp_query, ", T5.n_layout");			/* 24 */
	strcat(chp_query, ", T6.b_post_price");		/* 25 */
	strcat(chp_query, ", coalesce(T6.n_post_price, 0)");	/* 26 */
	strcat(chp_query, ", coalesce(T6.n_prime_postage, 0)");	/* 27 */
	strcat(chp_query, ", T6.n_consumption_tax");	/* 28 */
	strcat(chp_query, ", T6.n_tax_id");			/* 29���ɽ���λ��� */
	strcat(chp_query, ", T6.n_fraction_id");	/* 30ü�������λ��� */
	strcat(chp_query, ", T6.b_stock");			/* 31.�߸˿�ɽ�� */
	strcat(chp_query, ", T3.n_count");			/* 32 */
	strcat(chp_query, ", T6.b_postage_tax");	/* 33 */
	strcat(chp_query, ", T8.c_name");			/* 34 */
	strcat(chp_query, ", T6.n_vip_price");		/* 35������Ѳ��ʤˤ������λ��� */
	strcat(chp_query, ", T2.n_member_selling_price");	/* 36������� */
	if(in_login == 0) {
		strcat(chp_query, ", 0");				/* 37 */
	} else {
		strcat(chp_query, ", 1");				/* 37 */
	}
	strcat(chp_query, ", T2.c_item_id");		/* 38 */
	strcat(chp_query, ", T6.b_reserve");		/* 39 */
	strcat(chp_query, ", T3.n_reserve_count");	/* 40 */
	strcat(chp_query, ", T6.b_standard_price");	/* 41 */
	strcat(chp_query, ", T6.b_point");			/* 42 */
	strcat(chp_query, ", T2.n_point_rate");		/* 43 */
	strcat(chp_query, ", T6.c_comment_stock");	/* 44 */
	strcat(chp_query, ", T6.c_comment_reserve");	/* 45 */
	strcat(chp_query, ", T9.b_use");			/* 46 */
	strcat(chp_query, ", T10.b_use");			/* 47 */
	strcat(chp_query, ", coalesce(T9.n_count, 0)");		/* 48 */
	strcat(chp_query, ", T9.c_color");			/* 49 */
	strcat(chp_query, ", T1.n_unit");			/* 50 ȯ��ñ�� */
	strcat(chp_query, ", T1.n_max");			/* 51 ��� */
	strcat(chp_query, ", T1.n_min");			/* 52 ���� */
	strcat(chp_query, ", T1.n_contents_count");	/* 53 ���� */
	strcat(chp_query, ", T1.n_unit_cost");		/* 54 �������� */
	strcat(chp_query, ", T1.n_unit_code");		/* 55 ����ñ�̥����� */
	strcat(chp_query, ", T3.n_thing_remain");	/* 56 ���ʻĿ� */
	strcat(chp_query, ", T3.n_order_remain");	/* 57 ȯ��Ŀ� */
	strcat(chp_query, ", T3.n_accept_remain");	/* 58 ����Ŀ� */
	strcat(chp_query, ", T11.b_business");		/* 59 �ĶȰ� */
	strcat(chp_query, ", T11.c_area");			/* 60 �ϰ��ʬ */
	strcat(chp_query, ", T1.c_vendor_id");		/* 61 ������ */
	strcat(chp_query, ", T11.n_price_rank");	/* 62 ���ʥ�� */
	strcat(chp_query, ", T11.n_acting");		/* 63 1:�Ҹ˴ֿ���, 2:���ȯ�� */
	strcat(chp_query, ", T12.c_name");			/* 64 ����ñ��̾ */
	strcat(chp_query, " from sm_item T1");
	strcat(chp_query, ", sm_subitem T2");
	strcat(chp_query, ", st_stock T3");
	strcat(chp_query, ", sm_style_item_list T5");
	strcat(chp_query, ", sm_charge T6");
	strcat(chp_query, ", sm_review T9");
	strcat(chp_query, ", sm_inquire T10");
	strcat(chp_query, " left join sm_maker T8");
	strcat(chp_query, " on T1.c_maker_id = T8.c_maker_id");
	strcat(chp_query, " left join st_customer T11");
	sprintf(chp_query + strlen(chp_query), " on T11.n_customer_no = %s", cha_login);
	strcat(chp_query, " left join sm_unit T12");
	strcat(chp_query, " on T12.n_unit_code = T1.n_unit_code");
	if(in_login == 0) {
		strcat(chp_query, " where T2.b_public = 1");
	} else {
		switch(in_acting) {
		case CO_FUNCTION_3_ACTING_CUSTOMER:
		case CO_FUNCTION_3_ACTING_PATRON:
			strcat(chp_query, " where T2.b_public = 1");
			sprintf(chp_query + strlen(chp_query), " and T11.n_acting = %d", in_acting);
			break;
		case CO_FUNCTION_3_ACTING_TRANSFER:
			sprintf(chp_query + strlen(chp_query), " where T11.n_acting = %d", in_acting);
			break;
		default:
			strcat(chp_query, " where T2.b_public = 1");
			break;
		}
	}
	strcat(chp_query, " and T1.c_item_id = T2.c_item_id");
	strcat(chp_query, " and T2.c_item_id = T3.c_item_id");
	sprintf(chp_query + strlen(chp_query), " and T2.c_sub_id = %s", cha_sub_id);
	strcat(chp_query, " and T3.c_sub_id = T1.c_item_id");
	strcat(chp_query, " and T2.n_selling_price is not null");
	strcat(chp_query, chp_undisp_itemid_list);
	sprintf(chp_query + strlen(chp_query), " and T1.c_item_id = '%s'", chp_item_id);
	strcat(chp_query, " order by T2.n_sec");
	dbres = Db_OpenDyna(db, chp_query);
	if(!dbres) {
		free(chp_undisp_itemid_list);
		free(chp_query);
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d�����ʾ���μ����˼��Ԥ��ޤ�����(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	if(in_row <= 0) {
		free(chp_undisp_itemid_list);
		free(chp_query);
		Db_CloseDyna(dbres);
		return 0;
	}
	chp_layout = Db_GetValue(dbres, 0, 16) ? Db_GetValue(dbres, 0, 16) : "";
	if(!strcmp(chp_layout, CO_TYPE_A1) || !strcmp(chp_layout, CO_TYPE_A2)
	 || !strcmp(chp_layout, CO_TYPE_A3) || !strcmp(chp_layout, CO_TYPE_A4)) {
		strcpy(cha_bgcolor, Db_GetValue(dbres, 0, 17) ? Db_GetValue(dbres, 0, 17) : "");
	} else {
		strcpy(cha_bgcolor, Db_GetValue(dbres, 0, 18) ? Db_GetValue(dbres, 0, 18) : "");
	}
	cha_bordercolor[0] = '\0';
	chp_value = Db_GetValue(dbres, 0, 19);
	if(chp_value) {
		strcpy(cha_bordercolor, chp_value);
	}
	cha_border[0] = '\0';
	chp_value = Db_GetValue(dbres, 0, 20);
	if(chp_value) {
		strcpy(cha_border, chp_value);
	}
	Put_Nlist(nlp_out, "ARTICLE", "<table width=\"90%\" border=\"0\" cellspacing=\"3\" cellpadding=\"2\" class=\"cart-table\" align=\"center\">\n");
	memset(cha_buf, '\0', sizeof(cha_buf));
	i = 0;
	/* ����ɽ�� */
	b_link = 0;
	in_start = 0;	/* TYPEA1-4 */
	in_cnt = 0;		/* TYPEA1-4 */
	in_ret = Build_Item_Table(db, nlp_in, nlp_out, dbres, cha_buf, &i, b_link, in_row, in_start, in_cnt, in_btn);
	if(in_ret < 0) {
		Db_CloseDyna(dbres);
		free(chp_undisp_itemid_list);
		free(chp_query);
		return 1;
	}
	Put_Nlist(nlp_out, "ARTICLE", cha_buf);
	Db_CloseDyna(dbres);
	in_ret = Set_Cart_Node(OldDBase(db), nlp_out, chp_item_id, CO_NODE_ITEM_DETAIL);
	if(in_ret != 0) {
		free(chp_undisp_itemid_list);
		free(chp_query);
		return 1;
	}
	Put_Nlist(nlp_out, "ARTICLE", "</table>\n");
	*inp_display = 1;
	return 0;
}

/*
+* ========================================================================
 * Function:	�ؿ��ƤӽФ�
 * Description:
 *	�ƴؿ���ƤӽФ�
%* ========================================================================
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
=* ========================================================================
 */
int Build_Cart_Item(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_item_id, int in_btn, int *inp_display)
{
	NLIST *nlp_cookie;
	int in_ret;

	nlp_cookie = Get_Cookie();
	if(Gin_func_3) {
		in_ret = build_page_f3(db, nlp_in, nlp_out, nlp_cookie, chp_item_id, in_btn, inp_display);
	} else {
		in_ret = build_page(db, nlp_in, nlp_out, nlp_cookie, chp_item_id, in_btn, inp_display);
	}
	Finish_Nlist(nlp_cookie);

	return in_ret;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_article_cart()
 * Description:
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int build_article_cart(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,int in_blog
	,char *chp_item
	,int in_btn
	,int *inp_display
)
{
	int in_ret;
	DBase *cartdb;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		return 1;
	}

	in_ret = Build_Cart_Item(cartdb, nlp_in, nlp_out, chp_item, in_btn, inp_display);
	Db_Disconnect(cartdb);
	return in_ret;
}

/*
+* ========================================================================
 * Function:	�ؿ��ƤӽФ�
 * Description:
 *	�ƴؿ���ƤӽФ�
%* ========================================================================
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
=* ========================================================================
 */
int Build_Entry_Recomend(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
#define CO_IDSIZE 64
	DBRes *dbres;
	DBase *cartdb;
	NLIST *nlp_cookie;
	char cha_sql[1024];
	char *chp_sql;
	char *chp_tmp;
	char *chp_list;
	char *chp_escape;
	int i;
	int in_cnt;
	int in_login;
	int in_count;
	int in_entry_id;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}

	nlp_cookie = Get_Cookie();
	Get_Cart_Handytype();
	in_login = Is_Logged_In(OldDBase(cartdb), nlp_in, nlp_cookie);
	if (!in_login) {
		Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\"><tr>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<td valign=\"top\" width=\"10\">*</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "<td>�����Ϥ���ޤ���</td>\n");
		Put_Nlist(nlp_out, "SIDEBAR", "</tr></table>\n");
		return 0;
	}

	sprintf(cha_sql,
		" select T4.c_item_id"
		" from st_charge T1"
		",st_charge_detail T2"
		",sm_item_category T3"
		",sm_item_category T4"
		" where T1.n_charge_no = T2.n_charge_no"
		" and T2.c_item_id = T3.c_item_id"
		" and T3.c_category_id = T4.c_category_id"
		" and T1.n_customer_no=%s",
		Get_Nlist(nlp_cookie, "LOGINID", 1));
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "��������μ����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Db_Disconnect(cartdb);
		return 1;
	}
	in_cnt = Db_GetRowCount(dbres);
	chp_list = malloc(in_cnt * (CO_IDSIZE + 3) + 2);
	chp_list[0] = '\0';
	while (Db_FetchNext(dbres) == CO_SQL_OK) {
		if (Db_FetchValue(dbres, 0)) {
			if (chp_list[0] == '\0') {
				strcpy(chp_list, "('");
			} else {
				strcat(chp_list, ",'");
			}
			strcat(chp_list, Db_FetchValue(dbres, 0));
			strcat(chp_list, "'");
		}
	}
	strcat(chp_list, ")");
	Db_CloseDyna(dbres);
	Db_Disconnect(cartdb);

	/* �ºݤ�ɽ��������� */
	strcpy(cha_sql, "select T1.n_archive_value");
	strcat(cha_sql, " from at_archive T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_archive_id = %d", CO_SIDECONTENT_ARCHIVE_NEWENTRY);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "�ǿ�������ɽ����������륯����˼��Ԥ��ޤ�����<br>(%s)", Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if(!chp_tmp || atoi(chp_tmp) < 1) {
		Db_CloseDyna(dbres);
		Put_Nlist(nlp_out, "ERROR", "�ǿ�������ɽ����������꤬����������ޤ���");
		return 1;
	}
	in_count = atoi(chp_tmp);
	Db_CloseDyna(dbres);
	/* ������ʬ���쥯�Ȥ��Ƥ��� */
	if(in_cnt) {
		asprintf(&chp_sql,
			" select n_entry_id"
			",c_entry_title"
			",date_format(d_entry_create_time,'%%yǯ%%m��%%d��%%H��%%iʬ')"
			" from at_entry "
			" where n_blog_id=%d"
			" and c_item_id in %s "
			" limit %d", in_blog, chp_list, in_count);
	} else {
		asprintf(&chp_sql,
			" select n_entry_id"
			",c_entry_title"
			",date_format(d_entry_create_time,'%%yǯ%%m��%%d��%%H��%%iʬ')"
			" from at_entry "
			" where n_blog_id=%d"
			" limit %d", in_blog, in_count);
	}
	free(chp_list);
	dbres = Db_OpenDyna(db, chp_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ǿ�������������륯����˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "QUERY", "<br>");
		Put_Nlist(nlp_out, "QUERY", chp_sql);
		free(chp_sql);
		return 1;
	}
	free(chp_sql);
	in_count = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "<table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\">\n");
	if(!in_count) {
		Put_Nlist(nlp_out, "SIDEBAR", "<tr><td valign=\"top\" width=\"10\">*</td><td>�����Ϥ���ޤ���</td></tr>\n");
	} else {
		for(i = 0; i < in_count; i++) {
			Put_Nlist(nlp_out, "SIDEBAR", "<tr>\n");
			Put_Nlist(nlp_out, "SIDEBAR", "<td valign=\"top\" width=\"10\">*</td>\n");
			/* error check */
			chp_tmp = Db_GetValue(dbres, i, 0);
			if(!chp_tmp || !*chp_tmp) {
				Db_CloseDyna(dbres);
				Put_Nlist(nlp_out, "ERROR", "������ID������Ǥ��ޤ���Ǥ�����");
				return 1;
			}
			in_entry_id = atoi(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 1);
			if(!chp_tmp) {
				Put_Nlist(nlp_out, "ERROR", "���������ȥ������Ǥ��ޤ���Ǥ�����");
				Db_CloseDyna(dbres);
				return 1;
			}
			chp_escape = Escape_HtmlString(chp_tmp);
			chp_tmp = Db_GetValue(dbres, i, 2);
			if(!chp_tmp || !*chp_tmp) {
				Put_Nlist(nlp_out, "ERROR", "��������ƻ��������Ǥ��ޤ���Ǥ�����");
				Db_CloseDyna(dbres);
				return 1;
			}
			if (g_in_short_name) {
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<td><a href=\"%s%s%s/%08d/?eid=%d#entry\">%s</a><br>\n(%s)</td>\n"
					, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_entry_id, chp_escape, chp_tmp);
			} else {
				Put_Format_Nlist(nlp_out, "SIDEBAR", "<td><a href=\"%s/%s?eid=%d&bid=%d#entry\">%s</a><br>\n(%s)</td>\n"
					, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_entry_id, in_blog, chp_escape, chp_tmp);
			}
			free(chp_escape);
			Put_Nlist(nlp_out, "SIDEBAR", "</tr>\n");
		}
	}
	Db_CloseDyna(dbres);
	Put_Nlist(nlp_out, "SIDEBAR", "</table>\n");

	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_menu_string
 * Description:
 *			�쥤�����Ȼ��Υե����ޥå�
%* ------------------------------------------------------------------------
 * Return:
 *			1:���顼
 *			0:���ｪλ
 * ------------------------------------------------------------------------
 */
void build_menu_string(DBRes *dbres, NLIST *nlp_out, char *chp_put_locate, char *chp_cgi, int in_layout)
{
	char *chp_id;
	char *chp_name;
	char *chp_value;
	char cha_temp[1024];
	char cha_temp2[1024];
	int in_row;
	int in_ret;
	int i;

	in_row = Db_GetRowCount(dbres);
	if(in_layout == 0) {
		Put_Nlist(nlp_out, chp_put_locate, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"2\">\n");
		for(i = 0; i < in_row; i++) {
			Put_Format_Nlist(nlp_out, chp_put_locate, "<tr>\n<td valign=\"top\">%s</td>", CO_SC_CHAR_LIST0);
			chp_id = Db_GetValue(dbres, i, 0);
			chp_name = Db_GetValue(dbres, i, 1);
			memset(cha_temp, '\0', sizeof(cha_temp));
			in_ret = Euc_Strncpy(cha_temp, chp_name, CO_MENU_STRING_LENGTH);
			if(strlen(chp_name) > CO_MENU_STRING_LENGTH && in_ret == CO_MENU_STRING_LENGTH) {
				strcat(cha_temp, CO_SC_CHAR_LIST4);
			}
			chp_value = Escape_HtmlString(cha_temp);
			Put_Format_Nlist(nlp_out, chp_put_locate,
				"<td><a href=\"%s/%s/id/%s/\">%s</a></td>\n</tr>\n"
				, CO_CART_CGI_PATH, chp_cgi, chp_id, chp_value
			);
			free(chp_value);
		}
		Put_Nlist(nlp_out, chp_put_locate, "</table>\n");
	} else {
		memset(cha_temp, '\0', sizeof(cha_temp));
		sprintf(cha_temp,
			"<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">"
			"<form action=\"%s/%s\" method=\"post\">"
			"<tr>"
				"<td>"
					"<select name=\"id\">"
			, CO_CART_CGI_PATH, chp_cgi
		);
		Put_Nlist(nlp_out, chp_put_locate, cha_temp);
		for(i = 0; i < in_row; i++) {
			chp_id = Db_GetValue(dbres, i, 0);
			chp_name = Db_GetValue(dbres, i, 1);
			memset(cha_temp, '\0', sizeof(cha_temp));
			in_ret = Euc_Strncpy(cha_temp, chp_name, CO_MENU_STRING_LENGTH);
			if(strlen(chp_name) > CO_MENU_STRING_LENGTH && in_ret == CO_MENU_STRING_LENGTH) {
				strcat(cha_temp, CO_SC_CHAR_LIST4);
			}
			chp_value = Escape_HtmlString(cha_temp);
			memset(cha_temp2, '\0', sizeof(cha_temp2));
			sprintf(cha_temp2,
				"<option value=\"%s\">%s</option>\n"
				, chp_id, chp_value
			);
			free(chp_value);
			Put_Nlist(nlp_out, chp_put_locate, cha_temp2);
		}
		Put_Nlist(nlp_out, chp_put_locate,
				"</select>"
				"<input type=\"submit\" value=\"����\">"
				"</td>"
			"</tr>"
			"</form>"
			"</table>\n"
		);
	}
	return;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_menu_event
 * Description:
 *			����Υ�˥塼����
%* ------------------------------------------------------------------------
 * Return:
 *			1:���顼
 *			0:���ｪλ
 * ------------------------------------------------------------------------
 */
int build_menu_event(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_cookie, int in_layout, int in_list_max, int in_login, int in_mode)
{
	DBRes *dbres;
	char *chp_query;
	char *chp_overtime_event;
	char *chp_timesell_event;
	char cha_sql[1024];
	char cha_d_now[64];
	int in_ret;

	memset(cha_d_now, '\0', sizeof(cha_d_now));
	Get_NowDate(cha_d_now);
	/* ���٥�Ⱦ��� */
	chp_overtime_event = NULL;
	chp_timesell_event = NULL;
	in_ret = Un_Disp_Event(OldDBase(db), nlp_out, &chp_overtime_event, &chp_timesell_event, in_login, in_mode);
	if(in_ret != 0) {
		return 1;
	}
	chp_query = (char *)malloc(CO_SC_MAX_BUF + strlen(chp_overtime_event) + strlen(chp_timesell_event));
	if(chp_query == NULL) {
		free(chp_overtime_event);
		free(chp_timesell_event);
		Put_Nlist(nlp_out, "ERROR", "���٥�Ⱦ���μ����˼��Ԥ��ޤ�����");
		return 1;
	}
	memset(chp_query, '\0', sizeof(chp_query));
	strcpy(chp_query, "select T1.c_event_id");	/* 0 */
	strcat(chp_query, ", T1.c_name");			/* 1 */
	strcat(chp_query, " from sm_event T1");
	strcat(chp_query, " where T1.b_public = 1");
	if(*chp_overtime_event != '\0' || *chp_timesell_event != '\0') {
		strcat(chp_query, " and T1.c_event_id not in (");
		if(*chp_overtime_event != '\0') {
			strcat(chp_query, chp_overtime_event);
			if(*chp_timesell_event != '\0') {
				strcat(chp_query, ",");
			}
		}
		if(*chp_timesell_event != '\0') {
			strcat(chp_query, chp_timesell_event);
		}
		strcat(chp_query, ")");
	}
	free(chp_overtime_event);
	free(chp_timesell_event);
	strcat(chp_query, " and (T1.b_vip = 0");
	if(in_login == 1) {
		strcat(chp_query, " or T1.b_vip = 1");
	}
	strcat(chp_query, ")");
	sprintf(chp_query + strlen(chp_query), " and date_format(T1.d_start, '%%Y/%%m/%%d') <= '%s'", cha_d_now);
	sprintf(chp_query + strlen(chp_query), " and date_format(T1.d_end, '%%Y/%%m/%%d') >= '%s'", cha_d_now);
	strcat(chp_query, " order by T1.n_sec");
	sprintf(chp_query + strlen(chp_query), " limit %d", in_list_max);
	dbres = Db_OpenDyna(db, chp_query);
	if(dbres == NULL) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	if(!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		return 0;
	}
	build_menu_string(dbres, nlp_out, "SIDEBAR", CO_LINK_DISP_EVENT, in_layout);
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_menu_new
 * Description:
 *			���徦�ʤΥ�˥塼����
%* ------------------------------------------------------------------------
 * Return:
 *			1:���顼
 *			0:���ｪλ
 * ------------------------------------------------------------------------
 */
int build_menu_new(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_cookie, int in_layout, int in_period, int in_list_max, int in_login, int in_mode)
{
	DBRes *dbres;
	char *chp_query;
	char *chp_undisp_itemid_list;
	char cha_sql[1024];
	char cha_list_max[CO_SC_MAX_BUF];
	char cha_login[256];
	char cha_d_now[64];
	char cha_d_prev[64];
	int in_rank;
	int in_ret;

	if(Gin_func_3) {
		memset(cha_login, '\0', sizeof(cha_login));
		if(in_login == 1) {
			strcpy(cha_login, Get_Nlist(nlp_cookie, "LOGINID", 1));
		}
	}
	memset(cha_d_now, '\0', sizeof(cha_d_now));
	Get_NowDate(cha_d_now);
	Get_PrevDate(cha_d_now, in_period, cha_d_prev);
	snprintf(cha_list_max, sizeof cha_list_max, "%d", in_list_max);
	/* ɽ�����ʤ�����ID */
	chp_undisp_itemid_list = NULL;
	in_ret = Un_Disp_Item(OldDBase(db), nlp_in, nlp_out, nlp_cookie, &chp_undisp_itemid_list, "T2", in_login, in_mode);
	if(in_ret != 0) {
		return 1;
	}
	if(in_login) {
		in_rank = Get_CustomerRank(OldDBase(db), atoi(Get_Nlist(nlp_cookie, "LOGINID", 1)));
		if(in_rank < CO_SC_DEFAULT_NON_CUSTOMER) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d�������󥯤μ����˼��Ԥ��ޤ�����", __LINE__);
			Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
			return 1;
		}
	} else {
		in_rank = CO_SC_DEFAULT_NON_CUSTOMER;
	}
	chp_query = (char *)malloc((CO_SC_MAX_BUF + strlen(chp_undisp_itemid_list)) * sizeof(char));
	if(chp_query == NULL) {
		Put_Nlist(nlp_out, "ERROR_MSG", "���ʾ���μ����˼��Ԥ��ޤ�����<br>\n");
		return 1;
	}
	memset(chp_query, '\0', sizeof(chp_query));
	strcpy(chp_query, "select concat(T1.c_item_id,'/item/',T1.c_item_id,'/subitem/',T2.c_sub1_id,'/subitem2/',T2.c_sub2_id)");
	/* 0 */
	strcat(chp_query, ", case ");
	strcat(chp_query, " when T2.c_item_id = T2.c_sub1_id and T2.c_sub1_id = T2.c_sub2_id then T1.c_name");
	strcat(chp_query, " when T2.c_item_id <> T2.c_sub1_id and T2.c_item_id = T2.c_sub2_id then concat(T1.c_name,'[',T3.c_name,']')");
	strcat(chp_query, " when T2.c_item_id = T2.c_sub1_id and T2.c_item_id <> T2.c_sub2_id then concat(T1.c_name,'[',T4.c_name,']')");
	strcat(chp_query, " else concat(T1.c_name,'[',T3.c_name,']','[',T4.c_name,']')");
	strcat(chp_query, " end c_name");	/* 1 */
	strcat(chp_query, " from sm_item T1");
	strcat(chp_query, ", sm_subitem T3");
	strcat(chp_query, ", sm_subitem T4");
	strcat(chp_query, ", sm_subitem_combo T2");
	strcat(chp_query, ", st_stock T7");
	strcat(chp_query, ", st_item_price_rank T310");
	strcat(chp_query, ", sm_charge T5");
	strcat(chp_query, " where T2.b_public = 1");
	sprintf(chp_query + strlen(chp_query), " and date_format(T2.d_update, '%%Y/%%m/%%d') between '%s' and '%s'"
			, cha_d_prev, cha_d_now);
	strcat(chp_query, " and T1.c_item_id = T3.c_item_id");
	sprintf(&chp_query[strlen(chp_query)], " and T3.n_group = %d", CO_SUBITEM_GROUP_1);
	strcat(chp_query, " and T1.c_item_id = T4.c_item_id");
	sprintf(&chp_query[strlen(chp_query)], " and T4.n_group = %d", CO_SUBITEM_GROUP_2);
	strcat(chp_query, " and T3.c_item_id = T2.c_item_id");
	strcat(chp_query, " and T3.c_sub_id = T2.c_sub1_id");
	strcat(chp_query, " and T4.c_item_id = T2.c_item_id");
	strcat(chp_query, " and T4.c_sub_id = T2.c_sub2_id");
	strcat(chp_query, " and T7.c_item_id = T2.c_item_id");
	strcat(chp_query, " and T7.c_sub1_id = T2.c_sub1_id");
	strcat(chp_query, " and T7.c_sub2_id = T2.c_sub2_id");
	strcat(chp_query, " and (T5.b_disp_soldout = 1");
	strcat(chp_query, " or (T2.b_stock = 0");
	strcat(chp_query, " or (T2.b_stock = 1");
	strcat(chp_query, " and (T7.n_count > 0");
	strcat(chp_query, " or (T5.b_reserve = 1");
	strcat(chp_query, " and coalesce(T7.n_reserve_count, 0) > 0)))))");
	strcat(chp_query, chp_undisp_itemid_list);
	strcat(chp_query, " and T2.c_item_id = T310.c_item_id");
	strcat(chp_query, " and T2.c_sub1_id = T310.c_sub1_id");
	strcat(chp_query, " and T2.c_sub2_id = T310.c_sub2_id");
	sprintf(&chp_query[strlen(chp_query)], " and T310.n_rank = if(T5.n_vip_price = 0, %d, %d)"
			, CO_SC_DEFAULT_NON_CUSTOMER, in_rank);
	strcat(chp_query, " and T310.n_selling_price is not null");
	strcat(chp_query, " and T2.b_disp = 1");
	strcat(chp_query, " order by T2.d_update desc, T1.n_sec");
	sprintf(chp_query + strlen(chp_query), " limit %s", cha_list_max);
	dbres = Db_OpenDyna(db, chp_query);
	free(chp_query);
	if(dbres == NULL) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	if(!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		return 0;
	}
	build_menu_string(dbres, nlp_out, "SIDEBAR", CO_LINK_DISP_ITEM_DETAIL, in_layout);
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:		build_menu_hit
 * Description:
 *			�͵����ʤΥ�˥塼����
%* ------------------------------------------------------------------------
 * Return:
 *			1:���顼
 *			0:���ｪλ
 * ------------------------------------------------------------------------
 */
int build_menu_hit(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_cookie, int in_layout, int in_count, int in_login, int in_mode)
{
	DBRes *dbres;
	char *chp_query;
	char *chp_undisp_itemid_list;
	char cha_sql[1024];
	char cha_count[CO_SC_MAX_BUF];
	char cha_login[256];
	char cha_d_now[64];
	char cha_d_prev[64];
	int in_rank;
	int in_ret;

	if(Gin_func_3) {
		memset(cha_login, '\0', sizeof(cha_login));
		if(in_login == 1) {
			strcpy(cha_login, Get_Nlist(nlp_cookie, "LOGINID", 1));
		}
	}
	memset(cha_d_now, '\0', sizeof(cha_d_now));
	Get_NowDate(cha_d_now);
	Get_PrevDate(cha_d_now, CO_MENU_HIT_DATE, cha_d_prev);
	snprintf(cha_count, sizeof cha_count, "%d", in_count);
	/* ɽ�����ʤ�����ID */
	chp_undisp_itemid_list = NULL;
	in_ret = Un_Disp_Item(OldDBase(db), nlp_in, nlp_out, nlp_cookie, &chp_undisp_itemid_list, "T4", in_login, in_mode);
	if(in_ret != 0) {
		return 1;
	}
	if(in_login) {
		in_rank = Get_CustomerRank(OldDBase(db), atoi(Get_Nlist(nlp_cookie, "LOGINID", 1)));
		if(in_rank < CO_SC_DEFAULT_NON_CUSTOMER) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d�������󥯤μ����˼��Ԥ��ޤ�����", __LINE__);
			Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
			return 1;
		}
	} else {
		in_rank = CO_SC_DEFAULT_NON_CUSTOMER;
	}
	chp_query = (char *)malloc((CO_SC_MAX_BUF + strlen(chp_undisp_itemid_list)) * sizeof(char));
	if(chp_query == NULL) {
		Put_Nlist(nlp_out, "ERROR_MSG", "���ʾ���μ����˼��Ԥ��ޤ�����<br>\n");
		return 1;
	}
	memset(chp_query, '\0', sizeof(chp_query));
	strcpy(chp_query, "select concat(T1.c_item_id,'/item/',T1.c_item_id,'/subitem/',T2.c_sub1_id,'/subitem2/',T2.c_sub2_id)");
	strcat(chp_query, ", case ");
	strcat(chp_query, " when T2.c_item_id = T2.c_sub1_id and T2.c_sub1_id = T2.c_sub2_id then T1.c_name");
	strcat(chp_query, " when T2.c_item_id <> T2.c_sub1_id and T2.c_item_id = T2.c_sub2_id then concat(T1.c_name,'[',T7.c_name,']')");
	strcat(chp_query, " when T2.c_item_id = T2.c_sub1_id and T2.c_item_id <> T2.c_sub2_id then concat(T1.c_name,'[',T8.c_name,']')");
	strcat(chp_query, " else concat(T1.c_name,'[',T7.c_name,']','[',T8.c_name,']')");
	strcat(chp_query, " end c_name");						/* 1 */
	strcat(chp_query, ", sum(T3.n_count) as sum_count");	/* 2 */
	strcat(chp_query, " from sm_item T1");
	strcat(chp_query, ", sm_subitem_combo T2");
	strcat(chp_query, ", sm_subitem T7");
	strcat(chp_query, ", sm_subitem T8");
	strcat(chp_query, ", st_stock T5");
	strcat(chp_query, ", st_item_price_rank T310");
	strcat(chp_query, ", st_charge_detail T3");
	strcat(chp_query, ", st_charge T4");
	strcat(chp_query, ", sm_charge T6");
	strcat(chp_query, " where T2.b_public = 1");
	strcat(chp_query, " and T1.c_item_id = T7.c_item_id");
	sprintf(&chp_query[strlen(chp_query)], " and T7.n_group = %d", CO_SUBITEM_GROUP_1);
	strcat(chp_query, " and T1.c_item_id = T8.c_item_id");
	sprintf(&chp_query[strlen(chp_query)], " and T8.n_group = %d", CO_SUBITEM_GROUP_2);
	strcat(chp_query, " and T7.c_item_id = T2.c_item_id");
	strcat(chp_query, " and T7.c_sub_id = T2.c_sub1_id");
	strcat(chp_query, " and T8.c_item_id = T2.c_item_id");
	strcat(chp_query, " and T8.c_sub_id = T2.c_sub2_id");
	strcat(chp_query, " and T1.c_item_id = T3.c_item_id");
	strcat(chp_query, " and T3.n_charge_no = T4.n_charge_no");
	strcat(chp_query, " and T1.c_item_id = T5.c_item_id");
	strcat(chp_query, " and T3.c_sub1_id = T5.c_sub1_id");
	strcat(chp_query, " and T3.c_sub2_id = T5.c_sub2_id");
	strcat(chp_query, " and T5.c_sub1_id = T2.c_sub1_id");
	strcat(chp_query, " and T5.c_sub2_id = T2.c_sub2_id");
	strcat(chp_query, " and (T6.b_disp_soldout = 1");
	strcat(chp_query, " or (T2.b_stock = 0");
	strcat(chp_query, " or (T2.b_stock = 1");
	strcat(chp_query, " and (T5.n_count > 0");
	strcat(chp_query, " or (T6.b_reserve = 1");
	strcat(chp_query, " and coalesce(T5.n_reserve_count, 0) > 0)))))");
	sprintf(chp_query + strlen(chp_query), " and date_format(T4.d_receipt, '%%Y/%%m/%%d') between '%s' and '%s'", cha_d_prev, cha_d_now);
	sprintf(chp_query + strlen(chp_query), " and date_format(T4.d_receipt, '%%Y/%%m/%%d') <> '%s'", cha_d_prev);
	strcat(chp_query, chp_undisp_itemid_list);
	strcat(chp_query, " and T2.c_item_id = T310.c_item_id");
	strcat(chp_query, " and T2.c_sub1_id = T310.c_sub1_id");
	strcat(chp_query, " and T2.c_sub2_id = T310.c_sub2_id");
	sprintf(&chp_query[strlen(chp_query)], " and T310.n_rank = if(T6.n_vip_price = 0, %d, %d)"
			, CO_SC_DEFAULT_NON_CUSTOMER, in_rank);
	strcat(chp_query, " and T310.n_selling_price is not null");
	strcat(chp_query, " and T2.b_disp = 1");
	strcat(chp_query, " group by T3.c_item_id, T3.c_sub1_id, T3.c_sub2_id");
	strcat(chp_query, " order by sum_count desc, T1.n_sec, T7.n_sec, T8.n_sec");
	sprintf(chp_query + strlen(chp_query), " limit %s", cha_count);
	dbres = Db_OpenDyna(db, chp_query);
	free(chp_query);
	if(dbres == NULL) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	if(!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		return 0;
	}
	build_menu_string(dbres, nlp_out, "SIDEBAR", CO_LINK_DISP_ITEM_DETAIL, in_layout);
	Db_CloseDyna(dbres);
	return 0;
}

/*
+* ========================================================================
 * Function:	���ʾ���
 * Description:
 *	�����ɥС��ξ��ʾ�����Ȥ�Ω�Ƥ롣
%* ========================================================================
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
=* ========================================================================
 */
int Build_Item_Info(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBRes *dbres;
	DBase *cartdb;
	NLIST *nlp_cookie;
	char cha_sql[512];
	int in_iteminfo;
	int in_itemnum;
	int in_layout;
	int in_period;
	int in_login;
	int in_ret;

	sprintf(cha_sql, "select n_iteminfo_id,n_iteminfo_num from at_blog where n_blog_id=%d;", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ɽ�����뾦�ʾ�������륯����˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	in_iteminfo = atoi(Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "1");
	in_itemnum = atoi(Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "3");
	Db_CloseDyna(dbres);

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		return 1;
	}

	dbres = Db_OpenDyna(cartdb, "select T1.n_layout,T1.n_new_period from sm_style_general T1");
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "ɽ�����뾦�ʾ�������륯����˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Db_Disconnect(cartdb);
		return 1;
	}
	if (!Db_GetRowCount(dbres)) {
		Put_Nlist(nlp_out, "ERROR", "������󤬤ߤĤ���ޤ���");
		Db_CloseDyna(dbres);
		Db_Disconnect(cartdb);
		return 1;
	}
	in_layout = atoi(Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "1");
	in_period = atoi(Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "10");
	Db_CloseDyna(dbres);

	nlp_cookie = Get_Cookie();
	Get_Cart_Handytype();
	in_login = Is_Logged_In(OldDBase(cartdb), nlp_in, nlp_cookie);

	in_ret = 0;
	switch (in_iteminfo) {
	case CO_ITEMINFO_NEW:
		in_ret = build_menu_event(cartdb, nlp_in, nlp_out, nlp_cookie, in_layout, in_itemnum, in_login, 'N');
		break;
	case CO_ITEMINFO_RECOMEND:
		in_ret = build_menu_new(cartdb, nlp_in, nlp_out, nlp_cookie, in_layout, in_period, in_itemnum, in_login, 'N');
		break;
	case CO_ITEMINFO_RANDOM:
		in_ret = build_menu_hit(cartdb, nlp_in, nlp_out, nlp_cookie, in_layout, in_itemnum, in_login, 'N');
		break;
	}
	Db_Disconnect(cartdb);
	Finish_Nlist(nlp_cookie);

	return in_ret;
}

/*
+* ========================================================================
 * Function:	���ʾ���
 * Description:
 *	�����ɥС��ξ��ʾ�����Ȥ�Ω�Ƥ롣
%* ========================================================================
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
=* ========================================================================
 */
int Get_Back_Number(DBase *db, NLIST *nlp_out, int in_blog)
{
	int in_count;
	char cha_sql[512];
	DBRes *dbres;
	DBase* cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	strcpy(cha_sql, "select count(*) from st_mail T1 where T1.d_send is not null and T1.d_send != '' and T1.n_mail_id != 3 and T1.b_disp > 0");
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
		return -1;
	}
	in_count = atoi(Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "0");
	Db_CloseDyna(dbres);
	Db_Disconnect(cartdb);
	return in_count;
}

void article_table_image_head(NLIST *nlp_in, NLIST *nlp_out);
void article_table_image_tail(NLIST *nlp_in, NLIST *nlp_out);

/*
+* ========================================================================
 * Function:		build_back_number()
 * Description:
 *	dbres�򳫤��ƥХå��ʥ�С��ꥹ�Ȥ������
%* ========================================================================
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ========================================================================
 */
int Build_Back_Number(
	 DBase *db
	,NLIST *nlp_in
	,NLIST *nlp_out
	,char *chp_where	/* sql��whereʸ */
	,int in_limit		/* ��ߥåȡ� 0�����ƤȤ��롣 */
	,int in_offset		/* ���ե��å� */
	,int in_blog)
{
	DBase* cartdb;
	DBRes *dbres;
	char *chp_escape;
	char *chp_id;
	char *chp_kid;
	char *chp_body;
	char *chp_create;
	char *chp_title;
	char *chp_tmp;
	char *chp_next;
	char cha_sql[8192];
	char cha_prev_create[64];
	char cha_home_category[1024];
	char cha_home_entry[1024];
	char cha_path[1024];
	int in_cutlen;
	int in_count;
	int i;

	/* �ȥåץڡ���ɽ��ʸ�������� */
	strcpy(cha_sql, "select T1.n_setting_cut_length");	/* 0 ʸ���� */
	strcat(cha_sql, " from at_blog T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ȥåץڡ���ɽ��ʸ���������˼��Ԥ��ޤ�����<br>");
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

	/* �����ߤ�ɬ�פʥѥ��ζ�����ʬ�򤢤餫�����äƤ����� */
	strcpy(cha_path, g_cha_protocol);
	chp_tmp = getenv("SERVER_NAME");
	if(!chp_tmp || !*chp_tmp) {
		Put_Nlist(nlp_out, "ERROR", "�����С�̾������Ǥ��ޤ���Ǥ�����");
		return 1;
	}
	strcat(cha_path, chp_tmp);

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	strcpy(cha_sql, "select T1.n_mail_no");						/* 0 ����ID */
	strcat(cha_sql, ", T1.c_subject");							/* 1 ���������ȥ� */
	strcat(cha_sql, ", T1.c_body");								/* 2 ������ʸ */
	strcat(cha_sql, ", date_format(T1.d_send,'%Y%m%d%H%i')");	/* 3 �������� */
	strcat(cha_sql, " from st_mail T1");
	strcat(cha_sql, " where T1.d_send is not null");
	strcat(cha_sql, " and T1.d_send <> ''");
	strcat(cha_sql, " and T1.n_mail_id <> 3");
	strcat(cha_sql, " and T1.b_disp > 0");
	if(chp_where && *chp_where) {
		strcat(cha_sql, chp_where);
	}
	strcat(cha_sql, " order by T1.d_send desc");
	if(in_limit > 0) {
		sprintf(cha_sql + strlen(cha_sql), " limit %d,%d", in_offset, in_limit);
	}
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�����ꥹ�Ȥ����륯����˼��Ԥ��ޤ�����<br>");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Put_Nlist(nlp_out, "QUERY", "<br>");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
		Db_Disconnect(cartdb);
		return 1;
	}
	in_count = Db_GetRowCount(dbres);
	if(in_count == 0) {
		Put_Nlist(nlp_out, "ARTICLE", "<span class=\"article-list-title\">�����Ϥ���ޤ���</span>");
		Put_Nlist(nlp_out, "TITLE_START", "<!--");
		Put_Nlist(nlp_out, "TITLE_END", "-->");
		Db_CloseDyna(dbres);
		Db_Disconnect(cartdb);
		return 0;
	}
	/* ���̵�����ɽ����ΤȤ��ϡ��㳰���������� */
	chp_kid = Get_Nlist(nlp_in, "kid", 1);
	cha_prev_create[0] = '\0';
	/* �����Υơ��֥���Ȥ�ȤȤ�ˡ��ȥ�å��Хå���ư���ΤΤ����RDF�����롣 */
	for(i = 0; i < in_count; i++) {
		chp_id = Db_GetValue(dbres, i, 0);
		chp_title = Db_GetValue(dbres, i, 1);
		chp_body = Db_GetValue(dbres, i, 2);
		chp_create = Db_GetValue(dbres, i, 3);
		if (g_in_short_name) {
			sprintf(cha_home_category, "%s%s%s/%08d/?cid=%d#archive_top", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, CO_BACKNO_CATEGORY);
			sprintf(cha_home_entry, "%s%s%s/%08d/?kid=%s#entry", g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, chp_id);
		} else {
			sprintf(cha_home_category, "%s/%s?cid=%d&bid=%d#archive_top", g_cha_user_cgi, CO_CGI_BUILD_HTML, CO_BACKNO_CATEGORY, in_blog);
			sprintf(cha_home_entry, "%s/%s?kid=%s&bid=%d#entry", g_cha_user_cgi, CO_CGI_BUILD_HTML, chp_id, in_blog);
		}
		/* �����ޤ�RDF */
		/* ���ܤβ��� */
		if(i == 0 || strncmp(cha_prev_create, chp_create, 8) != 0) {
			if(i != 0) {
				Put_Nlist(nlp_out, "ARTICLE", "</table>\n");
				article_table_image_tail(nlp_in, nlp_out);
			}
			Put_Format_Nlist(nlp_out, "ARTICLE", "<a name=\"entry%.2s%.2s\"></a>\n", chp_create + 4, chp_create + 6);
			article_table_image_head(nlp_in, nlp_out);
			Put_Format_Nlist(nlp_out, "ARTICLE"
				, "<table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"entry-day-title\"><tr><td height=\"30\">%.4sǯ%.2s��%.2s��</td></tr></table>\n"
				, chp_create, chp_create + 4, chp_create + 6);
			Put_Nlist(nlp_out, "ARTICLE", "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"entrytable\" valign=\"top\">\n");
		}
		sprintf(cha_prev_create, "%.8s", chp_create);
		/* ���������ȥ� */
		chp_escape = Escape_HtmlString(chp_title);
		Put_Format_Nlist(nlp_out, "ARTICLE", "<tr>\n<td>\n<div class=\"entry_title\"><a href=\"%s\">%s</a></div>\n", cha_home_entry, chp_escape);
		free(chp_escape);
		/* ������ʸ */
		if (chp_body && strlen(chp_body) > 0) {
			Put_Nlist(nlp_out, "ARTICLE", "\n<div class=\"entry_body\">");
			if(chp_kid) {
				if (g_in_html_editor) {
					Put_Nlist(nlp_out, "ARTICLE", chp_body);
				} else {
					chp_tmp = Conv_Blog_Br(chp_body);
					Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
					free(chp_tmp);
				}
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
			} else {
				chp_next =  Get_String_After(chp_body, in_cutlen);
				chp_tmp = (char*)malloc(chp_next - chp_body + 1);
				strncpy(chp_tmp, chp_body, chp_next - chp_body);
				chp_tmp[chp_next - chp_body] = '\0';
				if (g_in_html_editor) {
					Put_Nlist(nlp_out, "ARTICLE", chp_tmp);
				} else {
					chp_body = Conv_Blog_Br(chp_tmp);
					Put_Nlist(nlp_out, "ARTICLE", chp_body);
					free(chp_body);
				}
				free(chp_tmp);
				if (*chp_next) {
					Put_Nlist(nlp_out, "ARTICLE","��������");
				}
				Put_Nlist(nlp_out, "ARTICLE","</div>\n");
				if (*chp_next) {
					/* ���̵����ڡ����ʳ��ΤȤ��ϡ�³��������Ȥ�������󥯤�ɽ�� */
					Put_Format_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_more_link\"><a href=\"%s\">³�����ɤ�</a></div>\n", cha_home_entry);
				}
			}
		}
		Put_Nlist(nlp_out, "ARTICLE", "</td></tr>\n");
		Put_Nlist(nlp_out, "ARTICLE", "<tr><td width=\"100%\">\n");
		/* ��������Τ��������ƥ���̾����ƻ��֤�ɬ��ɽ�� */
		Put_Format_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_state\"><a href=\"%s\">�Хå��ʥ�С�</a> | %.2s��%.2sʬ"
			, cha_home_category, chp_create + 8, chp_create + 10);
		/* ���̵����ڡ����ʳ��ǤϥȥåפؤΥ�󥯤�ɽ�� */
		if(!chp_kid) {
			Put_Nlist(nlp_out, "ARTICLE", " | <a href=\"#top\">���ȥå�</a>");
		}
//		Put_Nlist(nlp_out, "ARTICLE", "</div>\n");
		Put_Nlist(nlp_out, "ARTICLE", "</td>\n</tr>\n</div>\n");
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(cartdb);
	Put_Nlist(nlp_out, "ARTICLE", "</table>\n");
	article_table_image_tail(nlp_in, nlp_out);

	return 0;
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
int Get_Id_Latest_Mail_Create_Time_Not_Draft(DBase *db)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_latest_entry_id;

	strcpy(cha_sql, "select T1.n_mail_no");
	strcat(cha_sql, " from st_mail T1");
	strcat(cha_sql, " where T1.d_send is not null");
	strcat(cha_sql, " and T1.d_send <> ''");
	strcat(cha_sql, " and T1.n_mail_id <> 3");
	strcat(cha_sql, " and T1.b_disp > 0");
	strcat(cha_sql, " order by T1.d_send desc");
	strcat(cha_sql, " limit 1");
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
 * Function:		Get_Id_Oldest_Mail_Create_Time_Not_Draft()
 * Description:
 * 	���񵭻���ǵ������դ��Ǥ�Ť������ε���ID
%* ========================================================================
 * Return:
 *	���񵭻���ǺǸŤε���ID
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Oldest_Mail_Create_Time_Not_Draft(DBase *db)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_oldest_entry_id;

	strcpy(cha_sql, "select T1.n_mail_no");
	strcat(cha_sql, " from st_mail T1");
	strcat(cha_sql, " where T1.d_send is not null");
	strcat(cha_sql, " and T1.d_send <> ''");
	strcat(cha_sql, " and T1.n_mail_id <> 3");
	strcat(cha_sql, " and T1.b_disp > 0");
	strcat(cha_sql, " order by T1.d_send");
	strcat(cha_sql, " limit 1");
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
+* =======================================================================
 * Function:            Get_Create_Time_By_Entry_Id()
 * Description:
 *
%* ========================================================================
 * Return:
 *			���ｪλ 0
 *      �����          ���顼�� CO_ERROR
=* ========================================================================*/
int Get_Create_Time_By_Mail_Id(DBase *db, int in_entry_id, char *chp_create_time)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];

	strcpy(cha_sql, "select date_format(T1.d_send, '%Y%m%d%H%i%s')");
	strcat(cha_sql, " from st_mail T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_mail_no = %d", in_entry_id);
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
 * Function:		Get_Id_Prev_Entry_Create_Time()
 * Description:
 *	���λؼ���ƻ��֤ε���ID����
%* ========================================================================
 * Return:
 *	���ꤷ��ID�������Ρ�������ƻ��֤ˤ����뵭����ID
 *	���顼�� CO_ERROR
-* ========================================================================*/
int Get_Id_Prev_Mail_Create_Time(DBase *db, int in_entry_id)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	char cha_create_time[32];
	int in_prev_eid;
	int in_ret;

	in_ret = Get_Create_Time_By_Mail_Id(db, in_entry_id, cha_create_time);
	if (in_ret == CO_ERROR) {
		return CO_ERROR;
	}
	strcpy(cha_sql, "select	T1.n_mail_no");
	strcat(cha_sql, " from st_mail T1");
	strcat(cha_sql, " where T1.d_send is not null");
	strcat(cha_sql, " and T1.d_send <> ''");
	strcat(cha_sql, " and T1.n_mail_id <> 3");
	strcat(cha_sql, " and T1.b_disp > 0");
	sprintf(cha_sql + strlen(cha_sql), " and T1.d_send < '%s'", cha_create_time);
	strcat(cha_sql, " order by T1.d_send desc");
	strcat(cha_sql, " limit 1");
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
int Get_Id_Next_Mail_Create_Time(DBase *db, int in_entry_id)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	char cha_create_time[16];
	int in_next_eid;
	int in_ret;

	in_ret = Get_Create_Time_By_Mail_Id(db, in_entry_id, cha_create_time);
	if (in_ret == CO_ERROR) {
		return CO_ERROR;
	}
	strcpy(cha_sql, "select	T1.n_mail_no");
	strcat(cha_sql, " from st_mail T1");
	strcat(cha_sql, " where T1.d_send is not null");
	strcat(cha_sql, " and T1.d_send <> ''");
	strcat(cha_sql, " and T1.n_mail_id <> 3");
	strcat(cha_sql, " and T1.b_disp > 0");
	sprintf(cha_sql + strlen(cha_sql), " and T1.d_send > '%s'", cha_create_time);
	strcat(cha_sql, " order by T1.d_send");
	strcat(cha_sql, " limit 1");
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
+* ------------------------------------------------------------------------
 * Function:	 	exitst_and_displayable_entry()
 * Description:		���Υ���ȥ�ϸ����⡼�ɤȤ���¸�ߤ��뤫
 *
%* ------------------------------------------------------------------------
 * Return:
 * 		���ｪλ 1 ¸�ߤ������⡼�ɤǤ���Ȥ�
 *			 0 ����ʳ�
 *		���顼�� CO_ERROR
-* ------------------------------------------------------------------------*/
int exist_and_displayable_mail(DBase *db, NLIST *nlp_out, int in_entry_id)
{
	DBRes *dbres;
	char *chp_tmp;
	char cha_sql[512];
	int in_mode;

	strcpy(cha_sql, "select T1.b_disp");	/* mode */
	strcat(cha_sql, " from st_mail T1");
	strcat(cha_sql, " where T1.d_send is not null");
	strcat(cha_sql, " and T1.d_send <> ''");
	strcat(cha_sql, " and T1.n_mail_id <> 3");
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_mail_no = %d", in_entry_id);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres){
		Put_Nlist(nlp_out, "ERROR", "�����꡼�˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", cha_sql);
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

extern char gcha_theme_first_icon[256];
extern char gcha_theme_last_icon[256];
extern char gcha_theme_prev_entry_icon[256];
extern char gcha_theme_next_entry_icon[256];
/*
+* ========================================================================
 * Function:            Build_Number_Kid()
 * Description:
 *	���̵����ڡ����ε���������
%* ========================================================================
 * Return:                      ���ｪλ 0
 *      �����          ���顼�� 1
-* ========================================================================
 */
int Build_Mail_Kid(DBase *blogdb, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBase *db;
	char *chp_tmp;
	char cha_html[8192];
	char cha_sql [128];
	int in_oldest_eid;
	int in_prev_eid;
	int in_latest_eid;
	int in_next_eid;
	int in_entry_id;

	db = Open_Cart_Db(blogdb, nlp_out, in_blog);
	if (!db) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	chp_tmp = Get_Nlist(nlp_in, "kid", 1);
	if(Check_Numeric(chp_tmp) || exist_and_displayable_mail(db, nlp_out, atoi(chp_tmp)) != 1) {
		Put_Nlist(nlp_out, "ERROR", "���ΥХå��ʥ�С���¸�ߤ��ޤ���<br>");
		Db_Disconnect(db);
		return 1;
	}
	Build_HiddenEncode(nlp_out, "HIDDEN", "kid", chp_tmp);
	in_entry_id = atoi(chp_tmp);
	Put_Nlist(nlp_out, "ARTICLE", "<div class=\"entry_explore\">\n");
	/* �ڡ����᤯���Ϣ */
	/* �Ǥ�Ť������ΤȤ����Ǥ�Ť���󥯤�ɽ�����ʤ� */
	in_oldest_eid = Get_Id_Oldest_Mail_Create_Time_Not_Draft(db);
	memset(cha_html, '\0', sizeof(cha_html));
	if(in_entry_id != in_oldest_eid) {
		if (g_in_short_name) {
			sprintf(cha_html, "<a href=\"%s%s%s/%08d/?kid=%d#explore\" title=\"�Ǥ�Ť��᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_oldest_eid, g_cha_theme_image, gcha_theme_first_icon);
		} else {
			sprintf(cha_html, "<a href=\"%s/%s?kid=%d&bid=%d#explore\" title=\"�Ǥ�Ť��᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_oldest_eid, in_blog, g_cha_theme_image, gcha_theme_first_icon);
		}
		/* ������Υ���ȥ�ID���� */
		in_prev_eid = Get_Id_Prev_Mail_Create_Time(db, in_entry_id);
		if (g_in_short_name) {
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s%s%s/%08d/?kid=%d#explore\" title=\"���Υ᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_prev_eid, g_cha_theme_image, gcha_theme_prev_entry_icon);
		} else {
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s/%s?kid=%d&bid=%d#explore\" title=\"���Υ᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_prev_eid, in_blog, g_cha_theme_image, gcha_theme_prev_entry_icon);
		}
	} else {
		sprintf(cha_html + strlen(cha_html),
			"<img src=\"%s/%s\" border=\"0\">\n<img src=\"%s/%s\" border=\"0\">\n",
			g_cha_theme_image, gcha_theme_first_icon, g_cha_theme_image, gcha_theme_prev_entry_icon);
	}
	Put_Nlist(nlp_out, "ARTICLE", cha_html);
	memset(cha_html, '\0', sizeof(cha_html));
	/* �Ǥ⿷���������ΤȤ����Ǥ⿷������󥯤�ɽ�����ʤ� */
	in_latest_eid = Get_Id_Latest_Mail_Create_Time_Not_Draft(db);
	if(in_entry_id != in_latest_eid) {
		in_next_eid = Get_Id_Next_Mail_Create_Time(db, in_entry_id);
		if (g_in_short_name) {
			sprintf(cha_html
				, "<a href=\"%s%s%s/%08d/?kid=%d#explore\" title=\"���Υ᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_next_eid, g_cha_theme_image, gcha_theme_next_entry_icon);
			/* ��ļ��Υ���ȥ�ID���� */
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s%s%s/%08d/?kid=%d#explore\" title=\"�Ǥ⿷�����᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_protocol, getenv("SERVER_NAME"), g_cha_base_location, in_blog, in_latest_eid, g_cha_theme_image, gcha_theme_last_icon);
		} else {
			sprintf(cha_html
				, "<a href=\"%s/%s?kid=%d&bid=%d#explore\" title=\"���Υ᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_next_eid, in_blog, g_cha_theme_image, gcha_theme_next_entry_icon);
			/* ��ļ��Υ���ȥ�ID���� */
			sprintf(cha_html + strlen(cha_html)
				, "<a href=\"%s/%s?kid=%d&bid=%d#explore\" title=\"�Ǥ⿷�����᡼��\"><img src=\"%s/%s\" border=\"0\"></a>\n"
				, g_cha_user_cgi, CO_CGI_BUILD_HTML, in_latest_eid, in_blog, g_cha_theme_image, gcha_theme_last_icon);
		}
	} else {
		sprintf(cha_html + strlen(cha_html),
			"<img src=\"%s/%s\" border=\"0\">\n<img src=\"%s/%s\" border=\"0\">\n",
			g_cha_theme_image, gcha_theme_next_entry_icon, g_cha_theme_image, gcha_theme_last_icon);
	}
	Put_Nlist(nlp_out, "ARTICLE", cha_html);
	Put_Nlist(nlp_out, "ARTICLE", "</div>\n");
	Db_Disconnect(db);
	/* ��ʸ�Ȥ�Ω�� */
	sprintf(cha_sql, " and T1.n_mail_no = %d", in_entry_id);
	if(Build_Back_Number(blogdb, nlp_in, nlp_out, cha_sql, 0, 0, in_blog)) {
		return 1;
	}
	/* �ȥåץڡ��� */
	return 0;
}

/*
+* ========================================================================
 * Function:	��ӥ塼���μ���
 * Description:
 *	����ξ��ʤΥ�ӥ塼�����������
%* ========================================================================
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
=* ========================================================================
 */
int Get_Allow_Review(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_chk_login)
{
	NLIST *nlp_cookie;
	DBase *cartdb;
	DBRes *dbres;
	char *chp_tmp;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	if (in_chk_login) {
		nlp_cookie = Get_Cookie();
		Get_Cart_Handytype();
		if (!Is_Logged_In(OldDBase(cartdb), nlp_in, nlp_cookie)) {
			Db_Disconnect(cartdb);
			Finish_Nlist(nlp_cookie);
			return 0;
		}
		Finish_Nlist(nlp_cookie);
	}

	dbres = Db_OpenDyna(cartdb, "select b_use from sm_review;");
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "��ӥ塼�⡼�ɤ�����Ǥ��ޤ���");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	Db_CloseDyna(dbres);
	Db_Disconnect(cartdb);

	return atoi(chp_tmp ? chp_tmp : "0");
}

/*
+* ========================================================================
 * Function:	��ӥ塼���μ���
 * Description:
 *	����ξ��ʤΥ�ӥ塼�����������
%* ========================================================================
 * Return:
 *	0: ���ｪλ
 *	1: ���顼
=* ========================================================================
 */
int Get_Review_Number(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_item_id)
{
	DBase *cartdb;
	DBRes *dbres;
	char cha_sql[512];
	char *chp_value;
	int in_disp_review;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	strcpy(cha_sql, "select count(*)");
	strcat(cha_sql, " from st_review_item T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.c_item_id = '%s'", chp_item_id);
	strcat(cha_sql, " and T1.n_state = 1");
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(dbres == NULL) {
		Put_Nlist(nlp_out, "ERROR", "��ӥ塼��������Ǥ��ޤ���");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Db_Disconnect(cartdb);
		return -1;
	}
	if (Db_GetRowCount(dbres) >= 1) {
		chp_value = Db_GetValue(dbres, 0, 0);
		in_disp_review = atoi(chp_value ? chp_value : "0");
	} else {
		in_disp_review = 0;
	}
	Db_CloseDyna(dbres);
	Db_Disconnect(cartdb);

	return in_disp_review;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	build_review_list()
 * Description:		�����ȥꥹ��
 *
%* ------------------------------------------------------------------------
 * Return:			���ｪλ 0
 *	�����          ���顼�� 1
-* ------------------------------------------------------------------------*/
int Build_Review_List(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry)
{
	DBase *cartdb;
	DBRes *dbres;
	char *chp_esc;
	char *chp_conv;
	char *chp_value;
	char cha_item[256];
	char cha_sql[512];
	int in_row;
	int i, j;

	sprintf(cha_sql, "select c_item_id from at_entry where n_blog_id=%d and n_entry_id=%d", in_blog, in_entry);
	dbres = Db_OpenDyna(db, cha_sql);
	if(dbres == NULL) {
		Put_Nlist(nlp_out, "ERROR", "��ӥ塼��ɽ���Ǥ��ޤ���");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	strcpy(cha_item, Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "");
	Db_CloseDyna(dbres);
	if (!cha_item[0]) {
		return 0;
	}
	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	/* ɾ�� */
	strcpy(cha_sql, "select T2.n_point");	/* 0 ɾ���ݥ����*/
	strcat(cha_sql, ", T2.c_title");	/* 1 �����ȥ�*/
	strcat(cha_sql, ", T2.c_name");		/* 2 ��Ƽ�̾*/
	strcat(cha_sql, ", T1.n_adopt");	/* 3 ���ѿ�*/
	strcat(cha_sql, ", date_format(T2.d_create, '%yǯ%m��%d��%H��%iʬ')");	/* 4 ��ƻ���*/
	strcat(cha_sql, ", T2.c_comment");	/* 5 ������*/
	strcat(cha_sql, " from st_review_state T1");
	strcat(cha_sql, ", st_review_item T2");
	strcat(cha_sql, " where T1.n_customer_no = T2.n_customer_no");
	sprintf(cha_sql + strlen(cha_sql), " and T2.c_item_id = '%s'", cha_item);
	strcat(cha_sql, " and T2.n_state = 1");	/* ���� */
	strcat(cha_sql, " order by T2.d_create desc");
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(dbres == NULL) {
		Put_Nlist(nlp_out, "ERROR", "��ӥ塼��ɽ���Ǥ��ޤ���");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Db_Disconnect(cartdb);
		return -1;
	}
	in_row = Db_GetRowCount(dbres);
	Put_Nlist(nlp_out, "REVIEWLIST", "<a name=\"entryreview\"></a>\n");
	if(in_row == 0) {
		Db_CloseDyna(dbres);
		Db_Disconnect(cartdb);
		return 0;
	}
	Put_Format_Nlist(nlp_out, "REVIEWLIST", "<div class=\"review-list-title\">�����Ԥˤ�뾦�ʥ�ӥ塼[%d��]</div>\n", in_row);
	Put_Nlist(nlp_out, "REVIEWLIST" ,"<table width=\"100%\" border=\"0\" class=\"article\">\n");
	for(i = 0; i < in_row; i++) {
		/* ��ӥ塼��̾ */
		Put_Nlist(nlp_out, "REVIEWLIST", "<tr><td><div class=\"review_title\">\n");
		chp_value = Escape_HtmlString(Db_GetValue(dbres, i, 1));
		Put_Nlist(nlp_out, "REVIEWLIST", chp_value);
		free(chp_value);
		Put_Nlist(nlp_out, "REVIEWLIST", " (");
		for(j = 0; j < 5; j++) {
			if(j < atoi(Db_GetValue(dbres, i, 0))) {
				Put_Nlist(nlp_out, "REVIEWLIST", "��");
			} else {
				Put_Nlist(nlp_out, "REVIEWLIST", "��");
			}
		}
		Put_Nlist(nlp_out, "REVIEWLIST", ")</div></td></tr>\n");

		/* ��ӥ塼��ʸ */
		Put_Nlist(nlp_out, "REVIEWLIST", "<tr><td><div class=\"review_body\">\n");
		chp_esc = Escape_HtmlString(Db_GetValue(dbres, i, 5));
		chp_conv = Conv_Wbr(chp_esc, CO_SC_HTML_COMMENT_WIDTH);
		Put_Nlist(nlp_out, "REVIEWLIST", chp_conv);
		free(chp_conv);
		free(chp_esc);
		Put_Nlist(nlp_out, "REVIEWLIST", "</div></td></tr>\n");

		/* ��ӥ塼���� */
		Put_Nlist(nlp_out, "REVIEWLIST", "<tr><td>\n<div class=\"review-state\">");

		Put_Nlist(nlp_out, "REVIEWLIST", "Posted by ");
		chp_value = Escape_HtmlString(Db_GetValue(dbres, i, 2));
		Put_Nlist(nlp_out, "REVIEWLIST", chp_value);
		free(chp_value);
		Put_Nlist(nlp_out, "REVIEWLIST", "(");
		Put_Nlist(nlp_out, "REVIEWLIST", Db_GetValue(dbres, i, 3));
		Put_Nlist(nlp_out, "REVIEWLIST", ") | ");
		Put_Nlist(nlp_out, "REVIEWLIST", Db_GetValue(dbres, i, 4));
		Put_Nlist(nlp_out, "REVIEWLIST", "</div></td></tr>\n");
	}
	Put_Nlist(nlp_out, "REVIEWLIST" ,"</table>\n");
	Db_CloseDyna(dbres);
	Db_Disconnect(cartdb);

	return 0;
}

extern int g_in_need_login;
/*
+* ------------------------------------------------------------------------
 * Function:	 	build_comment_form()
 * Description:
 *	��������ƥե���������
 *	���顼�������ϥǡ������ɤ߹���
%* ------------------------------------------------------------------------
 * Return:
 *	���ｪλ 0
 *	���顼�� 1
-* ------------------------------------------------------------------------*/
int Build_Review_Form(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry)
{
	NLIST *nlp_cookie;
	DBase *cartdb;
	DBRes *dbres;
	char *chp_tmp;
	char *chp_err;
	char *chp_sql;
	char *chp_value;
	char *chp_escape;
	char *chp_from_error;
	char cha_item[256];
	char cha_sql[512];
	char cha_num[32];
	char cha_nickname[32];
	int in_owner;

	sprintf(cha_sql, "select c_item_id from at_entry where n_blog_id=%d and n_entry_id=%d", in_blog, in_entry);
	dbres = Db_OpenDyna(db, cha_sql);
	if(dbres == NULL) {
		Put_Nlist(nlp_out, "ERROR", "��ӥ塼��ɽ���Ǥ��ޤ���");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	strcpy(cha_item, Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "");
	Db_CloseDyna(dbres);
	if (!cha_item[0]) {
		return 0;
	}

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	chp_err = Get_Nlist(nlp_out, "ERROR", 1);
	chp_sql = Get_Nlist(nlp_out, "QUERY", 1);
	chp_from_error = Get_Nlist(nlp_in, "from_error", 1);
	Put_Nlist(nlp_out, "REVIEWFORM", "<div class=\"review-form-title\">���ξ��ʤΥ�ӥ塼���</div>\n");
	Put_Nlist(nlp_out, "REVIEWFORM", "<form action=\"");
	Put_Nlist(nlp_out, "REVIEWFORM", CO_CGI_BUILD_HTML);
	Put_Nlist(nlp_out, "REVIEWFORM", "\" method=\"POST\">\n");
	Build_HiddenEncode(nlp_out, "REVIEWFORM", "tid", cha_item);
	sprintf(cha_num, "%d", in_entry);
	Build_HiddenEncode(nlp_out, "REVIEWFORM", "eid", cha_num);
	sprintf(cha_num, "%d", in_blog);
	Build_HiddenEncode(nlp_out, "REVIEWFORM", "bid", cha_num);
	if (Get_Nlist(nlp_in, "BTN_COMMENT.x", 1)) {
		Build_HiddenEncode(nlp_out, "REVIEWFORM", "from_error", "1");
	}
	Put_Nlist(nlp_out, "REVIEWFORM", "<table border=\"0\" width=\"90%\" align=\"center\">\n");
	Put_Nlist(nlp_out, "REVIEWFORM",  "<tr><td>");
	if (g_in_need_login) {
		in_owner = Get_Nickname(db, nlp_out, cha_nickname, NULL, NULL);
		if (!in_owner) {
			Db_Disconnect(cartdb);
			return 1;
		}
		Put_Format_Nlist(nlp_out, "REVIEWFORM" ,"��̾����%s<br>\n", cha_nickname);
		asprintf(&chp_tmp, "%d:%s", in_owner, cha_nickname);
		Build_Hidden(nlp_out, "REVIEWFORM", "review_author_name", chp_tmp);
		free(chp_tmp);
	} else if (g_in_cart_mode == CO_CART_SHOPPER) {
		nlp_cookie = Get_Cookie();
		strcpy(cha_sql, "select T3.c_nick");	/* 0 �˥å��͡��� */
		strcat(cha_sql, ", T3.c_name");			/* 1 ��̾ */
		strcat(cha_sql, " from st_customer T3");
		sprintf(cha_sql + strlen(cha_sql), " where T3.n_customer_no = %s", Get_Nlist(nlp_cookie, "LOGINID", 1));
		dbres = Db_OpenDyna(cartdb, cha_sql);
		if(!dbres) {
			Put_Format_Nlist(nlp_out, "ERROR", "%d�������������˼��Ԥ��ޤ�����(%s)<br>", __LINE__, Gcha_last_error);
			Db_Disconnect(cartdb);
			Finish_Nlist(nlp_cookie);
			return 1;
		}
		if(Db_GetRowCount(dbres) == 0) {
			Put_Nlist(nlp_out, "ERROR", "������󤬤���ޤ���<br>");
			Db_CloseDyna(dbres);
			Db_Disconnect(cartdb);
			Finish_Nlist(nlp_cookie);
			return 1;
		}
		/* 2 ��Ƽ�̾(nick)*/
		Put_Nlist(nlp_out, "REVIEWFORM", "��̾��<br>\n");
		chp_value = Db_GetValue(dbres, 0, 0);
		if(chp_value == NULL || *chp_value == '\0') {
			Put_Nlist(nlp_out, "REVIEWFORM", "<input type=\"radio\" name=\"disp_name\" value=\"0\">");
			Build_Input(nlp_out, "REVIEWFORM", "review_author_name", "", 35, CO_MAXLEN_COMMENT_AUTHOR_NAME);
			Put_Nlist(nlp_out, "REVIEWFORM", "�ʥ˥å��͡����");
			Put_Nlist(nlp_out, "REVIEWFORM", "<br>\n");
		} else {
			Put_Nlist(nlp_out, "REVIEWFORM", "<input type=\"radio\" name=\"disp_name\" value=\"3\">");
			chp_escape = Escape_HtmlString(chp_value);
			Put_Nlist(nlp_out, "REVIEWFORM", chp_escape);
			Put_Nlist(nlp_out, "REVIEWFORM", "�ʥ˥å��͡����");
			Build_Hidden(nlp_out, "REVIEWFORM", "review_author_name", chp_escape);
			free(chp_escape);
			Put_Nlist(nlp_out, "REVIEWFORM", "<br>\n");
		}
		/* 3 ��Ƽ�̾(�������λ�̾)*/
		chp_value = Db_GetValue(dbres, 0, 1);
		if(chp_value && strlen(chp_value) > 0) {
			Put_Nlist(nlp_out, "REVIEWFORM", "<input type=\"radio\" name=\"disp_name\" value=\"1\">");
			chp_escape = Escape_HtmlString(chp_value);
			Put_Nlist(nlp_out, "REVIEWFORM", chp_escape);
			Put_Nlist(nlp_out, "REVIEWFORM", "�ʲ������λ�̾��");
			Build_Hidden(nlp_out, "REVIEWFORM", "review_author_real", chp_escape);
			free(chp_escape);
			Put_Nlist(nlp_out, "REVIEWFORM", "<br>\n");
		}
		Db_CloseDyna(dbres);
	} else {
		Put_Nlist(nlp_out, "REVIEWFORM", "��̾��<br>\n");
		chp_tmp = Get_Nlist(nlp_in, "review_author_name", 1);
		if(chp_from_error && chp_tmp) {
			chp_escape = Escape_HtmlString(chp_tmp);
			Build_Input(nlp_out, "REVIEWFORM", "review_author_name", chp_escape, 35, CO_MAXLEN_COMMENT_AUTHOR_NAME);
			free(chp_escape);
		} else {
			Build_Input(nlp_out, "REVIEWFORM", "review_author_name", "", 35, CO_MAXLEN_COMMENT_AUTHOR_NAME);
		}
	}
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<br>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"�����ȥ�<br>\n");
	chp_tmp = Get_Nlist(nlp_in, "review_author_title", 1);
	if(chp_from_error && chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_Input(nlp_out, "REVIEWFORM", "review_author_title", chp_escape, 35, CO_MAXLEN_COMMENT_AUTHOR_MAIL);
		free(chp_escape);
	} else {
		Build_Input(nlp_out, "REVIEWFORM", "review_author_title", "", 35, CO_MAXLEN_COMMENT_AUTHOR_MAIL);
	}
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<br><br>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"ɾ��<br>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<select name=\"review_author_point\">\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<option value=\"\">--</option>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<option value=\"1\">����������</option>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<option value=\"2\">����������</option>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<option value=\"3\">����������</option>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<option value=\"3\">����������</option>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<option value=\"3\">����������</option>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"</select>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"<br><br>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"�����ȡ�����1,000ʸ���ޤǡ�<br>\n");
	chp_tmp = Get_Nlist(nlp_in, "review", 1);
	if(chp_from_error && chp_tmp) {
		chp_escape = Escape_HtmlString(chp_tmp);
		Build_Inputarea(nlp_out, "REVIEWFORM", "review", chp_escape, 8, 55);
		free(chp_escape);
	} else {
		Build_Inputarea(nlp_out, "REVIEWFORM", "review", "", 8, 55);
	}
	Put_Nlist(nlp_out, "REVIEWFORM", "<p align=\"center\">\n");
	Build_Submit(nlp_out, "REVIEWFORM", "BTN_POST_REVIEW_ASK", "�񤭹���");
	Build_Reset(nlp_out, "REVIEWFORM", "�ꥻ�å�");
	Put_Nlist(nlp_out, "REVIEWFORM", "</p>\n</td></tr></table>\n");
	Put_Nlist(nlp_out, "REVIEWFORM" ,"</form>\n");
	Db_Disconnect(cartdb);

	return 0;
}

#define CO_TITLE_USER_MAX 32
#define CO_COMMENT_USER_MAX 4096
/*
+* ----------------------------------------------------------------
 * Function:	insert_value_check
 * Description:
 * 		���顼�����å�
%* ----------------------------------------------------------------
 * Return:
 * 		���ｪλ 0 ���顼 1
-* ----------------------------------------------------------------
 */
int insert_value_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out)
{
	DBRes *dbres;
	char *chp_value;
	char *chp_esc;
	char cha_sql[512];
	int in_length;
	int in_err;

	in_err = 0;
	/* ɾ�� */
	chp_value = Get_Nlist(nlp_in, "review_author_point", 1);
	if(chp_value == NULL) {
		Put_Nlist(nlp_out, "ERROR", "�����ͤ�����Ǥ��ޤ���Ǥ�����<br>");
		in_err++;
	}
	/* ��Ƥ���̾ */
	chp_value = Get_Nlist(nlp_in, "review_author_title", 1);
	if(chp_value != NULL) {
		in_length = strlen(chp_value);
		if(in_length > CO_TITLE_USER_MAX) {
			Put_Format_Nlist(nlp_out, "ERROR", "�����ȥ뤬Ĺ�����ޤ���%dʸ����������Ϥ��Ƥ���������<br>", CO_TITLE_USER_MAX);
			in_err++;
		} else if(in_length == 0) {
			Put_Nlist(nlp_out, "ERROR", "�����ȥ�����Ϥ��Ƥ���������<br>");
			in_err++;
		}
	} else {
		Put_Nlist(nlp_out, "ERROR", "�����ͤ�����Ǥ��ޤ���Ǥ�����<br>");
		in_err++;
	}
	/* ��Ƽ�̾ */
	chp_value = Get_Nlist(nlp_in, "review_author_name", 1);
	if(Get_Nlist(nlp_in, "disp_name", 1) && *Get_Nlist(nlp_in, "disp_name", 1) == '0' && chp_value != NULL) {
		if(*chp_value == '\0') {
			Put_Nlist(nlp_out, "ERROR", "�˥å��͡�������Ϥ��Ƥ���������<br>");
			in_err++;
		} else if(strlen(chp_value) > CO_MAX_NAME) {
			Put_Format_Nlist(nlp_out, "ERROR", "�˥å��͡��������%dʸ����Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_MAX_NAME/2, CO_MAX_NAME);
			in_err++;
		} else {
			strcpy(cha_sql, "select count(*)");	/* 0 */
			strcat(cha_sql, " from st_customer T1");
			chp_esc = My_Escape_SqlString(db, chp_value);
			sprintf(cha_sql + strlen(cha_sql), " where T1.c_nick = '%s'", chp_esc);
			free(chp_esc);
			dbres = Db_OpenDyna(db, cha_sql);
			if(!dbres) {
				memset(cha_sql, '\0', sizeof(cha_sql));
				sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
				Put_Nlist(nlp_out, "ERROR", cha_sql);
				return 1;
			}
			if(!Db_GetValue(dbres, 0, 0)) {
				Put_Nlist(nlp_out, "ERROR", "��¸�˥å��͡���μ����˼��Ԥ��ޤ�����");
				Db_CloseDyna(dbres);
				return 1;
			} else if(atoi(Db_GetValue(dbres, 0, 0)) != 0) {
				Put_Nlist(nlp_out, "ERROR", "�˥å��͡���ϴ�����Ͽ����Ƥ��ޤ���<BR>");
				in_err++;
			}
			Db_CloseDyna(dbres);
		}
	} else if(Get_Nlist(nlp_in, "disp_name", 1) && *Get_Nlist(nlp_in, "disp_name", 1) == '3') {
		if(!Get_Nlist(nlp_in, "review_author_name", 1)) {
			Put_Nlist(nlp_out, "ERROR", "�˥å��͡��������Ǥ��ޤ���Ǥ�����<br>");
			in_err++;
		}
	} else if(Get_Nlist(nlp_in, "disp_name", 1) && *Get_Nlist(nlp_in, "disp_name", 1) == '1') {
		if(!Get_Nlist(nlp_in, "review_author_real", 1)) {
			Put_Nlist(nlp_out, "ERROR", "�������λ�̾������Ǥ��ޤ���Ǥ�����<br>");
			in_err++;
		}
	} else {
		Put_Nlist(nlp_out, "ERROR", "��Ƽ�̾�����򤵤�Ƥ��ޤ���<br>");
		in_err++;
	}
	/* ��Ƥ����� */
	chp_value = Get_Nlist(nlp_in, "review", 1);
	if(chp_value != NULL) {
		in_length = strlen(chp_value);
		if(in_length > CO_COMMENT_USER_MAX) {
			Put_Format_Nlist(nlp_out, "ERROR", "������Ƥ�ʸ�Ϥ�Ĺ�����ޤ���Ⱦ��%dʸ����������Ϥ��Ƥ���������<br>", CO_COMMENT_USER_MAX);
			in_err++;
		} else if(in_length == 0) {
			Put_Nlist(nlp_out, "ERROR", "������Ƥ����Ϥ��Ƥ���������<br>");
			in_err++;
		}
	} else {
		Put_Nlist(nlp_out, "ERROR", "�����ͤ�����Ǥ��ޤ���Ǥ�����<br>");
		in_err++;
	}
	return in_err;
}

extern int Gin_sample_site;
/*
+* ----------------------------------------------------------------
 * Function:	insert_value
 * Description:
 * 		����
%* ----------------------------------------------------------------
 * Return:
 * 		���ｪλ 0 ���顼 1
-* ----------------------------------------------------------------
 */
int insert_value(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_cookie)
{
	DBRes *dbres;
	char *chp_esc;
	char cha_sql[512];
	char cha_login[256];
	char cha_review_charge_no[32];
	int in_ret;

	in_ret = Begin_Transact(db);
	if(in_ret == CO_SQL_ERR) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	/* ��ɼ�ֹ���� */
	memset(cha_review_charge_no, '\0', sizeof(cha_review_charge_no));
	in_ret = Manage_Review_Write(OldDBase(db), nlp_in, nlp_out, nlp_cookie, cha_review_charge_no);
	if(in_ret) {
		return 1;
	}
	if(cha_review_charge_no[0] == '\0') {
		Put_Nlist(nlp_out, "ERROR", "��Ͽ���������Ǥ��ޤ���");
		return 1;
	}
	memset(cha_login, '\0', sizeof(cha_login));
	strcpy(cha_login, Get_Nlist(nlp_cookie, "LOGINID", 1));
	if(*Get_Nlist(nlp_in, "disp_name", 1) == '0') {
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "update st_customer set");
		chp_esc = My_Escape_SqlString(db, Get_Nlist(nlp_in, "nick", 1));	/* �˥å��͡���*/
		sprintf(cha_sql + strlen(cha_sql), " c_nick = '%s'", chp_esc);
		free(chp_esc);
		sprintf(cha_sql + strlen(cha_sql), " where n_customer_no = %s", cha_login);
		if(Gin_sample_site == CO_CART_SAMPLE) {
			in_ret = CO_SQL_OK;
		} else {
			in_ret = Db_ExecSql(db, cha_sql);
		}
		if(in_ret == CO_SQL_ERR) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
			Put_Nlist(nlp_out, "ERROR", cha_sql);
			Rollback_Transact(db);
			return 1;
		}
	}
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "insert into st_review_item(");
	strcat(cha_sql, "n_charge_no");	/* ��ɼ�ֹ�*/
	strcat(cha_sql, ", c_item_id");	/* itemid*/
	strcat(cha_sql, ", c_sub_id");	/* subid*/
	strcat(cha_sql, ", n_customer_no");	/* userid*/
	strcat(cha_sql, ", n_point");	/* �ݥ����*/
	strcat(cha_sql, ", n_state");	/* ����*/
	strcat(cha_sql, ", c_title");	/* �����ȥ�*/
	strcat(cha_sql, ", c_name");	/* ��Ƽ�̾*/
	strcat(cha_sql, ", c_comment");	/* ������*/
	strcat(cha_sql, ", d_create");	/* ��ƻ���*/
	strcat(cha_sql, ")");
	strcat(cha_sql, " values (");
	strcat(cha_sql, cha_review_charge_no);
	sprintf(cha_sql + strlen(cha_sql), ", '%s'", Get_Nlist(nlp_in, "ITEMID", 1));
	sprintf(cha_sql + strlen(cha_sql), ", '%s'", Get_Nlist(nlp_in, "SUBITEMID", 1));
	sprintf(cha_sql + strlen(cha_sql), ", '%s'", cha_login);
	sprintf(cha_sql + strlen(cha_sql), ", %s", Get_Nlist(nlp_in, "review_author_point", 1));
	strcat(cha_sql, ", 0");
	chp_esc = My_Escape_SqlString(db, Get_Nlist(nlp_in, "review_author_title", 1));
	sprintf(cha_sql + strlen(cha_sql), ", '%s'", chp_esc);
	free(chp_esc);
	if(Get_Nlist(nlp_in, "disp_name", 1) && (*Get_Nlist(nlp_in, "disp_name", 1) == '0' || *Get_Nlist(nlp_in, "disp_name", 1) == '3')) {
		chp_esc = My_Escape_SqlString(db, Get_Nlist(nlp_in, "review_author_name", 1));
	} else {
		chp_esc = My_Escape_SqlString(db, Get_Nlist(nlp_in, "review_author_real", 1));
	}
	sprintf(cha_sql + strlen(cha_sql), ", '%s'", chp_esc);
	free(chp_esc);
	chp_esc = My_Escape_SqlString(db, Get_Nlist(nlp_in, "review", 1));
	sprintf(cha_sql + strlen(cha_sql), ", '%s'", chp_esc);
	free(chp_esc);
	strcat(cha_sql, ", now()");
	strcat(cha_sql, ")");
	if(Gin_sample_site == CO_CART_SAMPLE) {
		in_ret = CO_SQL_OK;
	} else {
		in_ret = Db_ExecSql(db, cha_sql);
	}
	if(in_ret == CO_SQL_ERR) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select count(*)");	/* 0 */
	strcat(cha_sql, " from st_review_state T1");
	sprintf(cha_sql + strlen(cha_sql), " where T1.n_customer_no = %s", cha_login);
	dbres = Db_OpenDyna(db, cha_sql);
	if(!dbres) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	in_ret = atoi(Db_GetValue(dbres, 0, 0));
	Db_CloseDyna(dbres);
	if(in_ret == 0) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "insert into st_review_state(");
		strcat(cha_sql, "n_customer_no");
		strcat(cha_sql, ", n_sum_no");
		strcat(cha_sql, ", n_adopt");
		strcat(cha_sql, ", n_reject");
		strcat(cha_sql, ")");
		strcat(cha_sql, " values(");
		sprintf(cha_sql + strlen(cha_sql), "'%s'", cha_login);
		strcat(cha_sql, ", 1");
		strcat(cha_sql, ", 0");
		strcat(cha_sql, ", 0");
		strcat(cha_sql, ")");
	} else if(in_ret == 1) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		strcpy(cha_sql, "update st_review_state");
		strcat(cha_sql, " set n_sum_no = n_sum_no + 1");
		sprintf(cha_sql + strlen(cha_sql), " where n_customer_no = %s", cha_login);
	} else {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	if(Gin_sample_site == CO_CART_SAMPLE) {
		in_ret = CO_SQL_OK;
	} else {
		in_ret = Db_ExecSql(db, cha_sql);
	}
	if(in_ret == CO_SQL_ERR) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		Rollback_Transact(db);
		return 1;
	}
	in_ret = Commit_Transact(db);
	if(in_ret == CO_SQL_ERR) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	return 0;
}

/*
+* ------------------------------------------------------------------------
 * Function:	 	Post_Review()
 * Description:
 *	��ӥ塼��ƽ�����
%* ------------------------------------------------------------------------
 * Return:
 *	���ｪλ 0
 *	���顼�� 1�����ϥ��顼), CO_ERROR��ͽ�����ʤ����顼)
-* ------------------------------------------------------------------------*/
int Post_Review(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBase *cartdb;
	NLIST *nlp_cookie;
	int in_ret;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	/* �����������å� */
	nlp_cookie = Get_Cookie();
	Get_Cart_Handytype();
	in_ret = Manage_Review_Write(OldDBase(cartdb), nlp_in, nlp_out, nlp_cookie, NULL);
	if(in_ret) {
		Finish_Nlist(nlp_cookie);
		Db_Disconnect(cartdb);
		return 1;
	}
	/* ��Ͽ�ͤΥ��顼�����å� */
	in_ret = insert_value_check(cartdb, nlp_in, nlp_out);
	if(in_ret) {
		Finish_Nlist(nlp_cookie);
		Db_Disconnect(cartdb);
		return 1;
	}
	/* ��Ͽ */
	in_ret = insert_value(cartdb, nlp_in, nlp_out, nlp_cookie);
	if(in_ret) {
		Finish_Nlist(nlp_cookie);
		Db_Disconnect(cartdb);
		return 1;
	}
	Finish_Nlist(nlp_cookie);
	Db_Disconnect(cartdb);
	return 0;
}

/*
+* ========================================================================
 * Function:	Cart_Auth_Login
 * Description:
 *
%* ========================================================================
 * Return:	<0	���顼
 * 			=0	��������
 *			>0	������Ѥߥ֥�ID
-* ======================================================================
 */
int Cart_Auth_Login(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_loginname, char *chp_passname)
{
	DBase *cartdb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	char cha_sql[512];
	char cha_loginval[256];
	char cha_passval[256];
	char *chp_val;
	char *chp_esc;
	int in_login_type;
	int in_ret;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}
	in_login_type = Get_LoginType(OldDBase(cartdb));
	chp_val = Get_Nlist(nlp_in, chp_loginname, 1);
	if(!chp_val || *chp_val == '\0') {
		if(in_login_type == CO_LOGIN_TYPE_MAIL) {
			Put_Nlist(nlp_out, "ERROR_MSG", "�᡼�륢�ɥ쥹�����ꤵ��Ƥ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR_MSG", "�桼���������ꤵ��Ƥ��ޤ���");
		}
		Db_Disconnect(cartdb);
		return 1;
	} else if(strlen(chp_val) > 256 || Check_Alnum_Opt(chp_val, CO_LOGIN_AVAILABLE_CHARS)) {
		if(in_login_type == CO_LOGIN_TYPE_MAIL) {
			Put_Nlist(nlp_out, "ERROR_MSG", "�᡼�륢�ɥ쥹�ޤ��ϥѥ���ɤ��ۤʤäƤ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR_MSG", "�桼�����ޤ��ϥѥ���ɤ��ְ�äƤ��ޤ���");
		}
		Db_Disconnect(cartdb);
		return 1;
	}
	strcpy(cha_loginval, chp_val);
	chp_val = Get_Nlist(nlp_in, chp_passname, 1);
	if(!chp_val || *chp_val == '\0') {
		Put_Nlist(nlp_out, "ERROR_MSG", "�ѥ���ɤ����ꤵ��Ƥ��ޤ���");
		Db_Disconnect(cartdb);
		return 1;
	} else if(strlen(chp_val) > 256 || Check_Pass(chp_val)) {
		if(in_login_type == CO_LOGIN_TYPE_MAIL) {
			Put_Nlist(nlp_out, "ERROR_MSG", "�᡼�륢�ɥ쥹�ޤ��ϥѥ���ɤ��ۤʤäƤ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR_MSG", "�桼�����ޤ��ϥѥ���ɤ��ְ�äƤ��ޤ���");
		}
		Db_Disconnect(cartdb);
		return 1;
	}
	strcpy(cha_passval, chp_val);

	strcpy(cha_sql, "select T1.c_pass");	/* 0 */
	strcat(cha_sql, ", T1.c_name");			/* 1 */
	strcat(cha_sql, ", T1.n_customer_no");	/* 2 */
	strcat(cha_sql, " from st_customer T1");
	chp_esc = My_Escape_SqlString(db, cha_loginval);
	if(in_login_type == CO_LOGIN_TYPE_MAIL) {
		sprintf(cha_sql + strlen(cha_sql), " where (T1.c_mail = '%s'", chp_esc);
		sprintf(cha_sql + strlen(cha_sql), " or T1.c_mobile_mail = '%s')", chp_esc);
	} else {
		sprintf(cha_sql + strlen(cha_sql), " where T1.c_user = '%s'", chp_esc);
	}
	free(chp_esc);
	sprintf(cha_sql + strlen(cha_sql), " and T1.n_customer_no <> %d", CO_SC_DEFAULT_CUSTOMER_NO);
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(!dbres) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d���������¹ԤǤ��ޤ���Ǥ�����(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
		Db_Disconnect(cartdb);
		return 1;
	}
	if(!Db_GetRowCount(dbres)) {
		Db_CloseDyna(dbres);
		if(in_login_type == CO_LOGIN_TYPE_MAIL) {
			Put_Nlist(nlp_out, "ERROR_MSG", "�᡼�륢�ɥ쥹�ޤ��ϥѥ���ɤ��ۤʤäƤ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR_MSG", "�桼�����ޤ��ϥѥ���ɤ��ְ�äƤ��ޤ���");
		}
		Db_Disconnect(cartdb);
		return 1;
	}
	if(!Db_GetValue(dbres, 0, 0) || strcmp(cha_passval, Db_GetValue(dbres, 0, 0))) {
		Db_CloseDyna(dbres);
		if(in_login_type == CO_LOGIN_TYPE_MAIL) {
			Put_Nlist(nlp_out, "ERROR_MSG", "�᡼�륢�ɥ쥹�ޤ��ϥѥ���ɤ��ۤʤäƤ��ޤ���");
		} else {
			Put_Nlist(nlp_out, "ERROR_MSG", "�桼�����ޤ��ϥѥ���ɤ��ְ�äƤ��ޤ���");
		}
		Db_Disconnect(cartdb);
		return 1;
	}
	if(Db_GetValue(dbres, 0, 2)) {
		memset(cha_loginval, '\0', sizeof(cha_loginval));
		strcpy(cha_loginval, Db_GetValue(dbres, 0, 2));
	}
	Db_CloseDyna(dbres);

	nlp_cookie = Get_Cookie();
	in_ret = U_Build_Login_Cookie(OldDBase(cartdb), nlp_in, nlp_out, nlp_cookie, atoi(cha_loginval));
	Finish_Nlist(nlp_cookie);
	Db_Disconnect(cartdb);

	return in_ret;
}

/*
+* ========================================================================
 * Function:	Regist_New_Customer
 * Description:
 *
%* ========================================================================
 * Return:	<0	���顼
 * 			=0	��������
 *			>0	������Ѥߥ֥�ID
-* ======================================================================
 */
int Regist_New_Customer(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_mail, char *chp_id, char *chp_pass, int in_blog)
{
	DBase *cartdb;
	DBRes *dbres;
	NLIST *nlp_cookie;
	char cha_sql[512];
	char *chp_value;
	int i;
	int in_row;
	int in_new_customer_no;
	int in_default_point;
	int in_error;
	int in_ret;

	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return -1;
	}

	strcpy(cha_sql, "select T1.c_user");	/* 0 */
	strcat(cha_sql, ", T1.c_mail");			/* 1 */
	strcat(cha_sql, " from st_customer T1");
	strcat(cha_sql, " group by T1.c_user");
	strcat(cha_sql, " order by T1.c_user");
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(!dbres) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "%d��query error(%s)", __LINE__, Gcha_last_error);
		Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
		Db_Disconnect(cartdb);
		return 1;
	}
	in_row = Db_GetRowCount(dbres);
	for(i = 0, in_error = 0; i < in_row; i++) {
		chp_value = Db_GetValue(dbres, i, 0);
		if(chp_value && *chp_value != '\0') {
			if(strcmp(chp_value, chp_id) == 0) {
				Put_Nlist(nlp_out, "ERROR_MSG", "�桼����ID�ϴ�����Ͽ����Ƥ��ޤ���<br>");
				in_error++;
			}
		}
		chp_value = Db_GetValue(dbres, i, 1);
		if(chp_value && *chp_value != '\0') {
			if(strcmp(chp_value, chp_mail) == 0) {
				Put_Nlist(nlp_out, "ERROR_MSG", "�᡼�륢�ɥ쥹�ϴ�����Ͽ����Ƥ��ޤ���<br>");
				in_error++;
			}
		}
	}
	Db_CloseDyna(dbres);
	if(in_error != 0) {
		Db_Disconnect(cartdb);
		return 1;
	}
	in_error = Begin_Transact(cartdb);
	if(in_error == CO_SQL_ERR) {
		Put_Nlist(nlp_out, "ERROR_MSG", "�ȥ�󥶥������γ��Ϥ˼��Ԥ��ޤ�����");
		Db_Disconnect(cartdb);
		return 1;
	}
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "update seq_customer set");
	strcat(cha_sql, " n_customer_no = last_insert_id(n_customer_no + 1)");
	in_ret = Db_ExecSql(cartdb, cha_sql);
	if(in_ret == CO_SQL_ERR) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "�ֹ�γ����˼��Ԥ��ޤ�����(%s)(%d)", Gcha_last_error, __LINE__);
		return 1;
	}
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "select coalesce(T1.n_default_point, 0)");	/* �ݥ���Ƚ���� */
	strcat(cha_sql, ", last_insert_id()");	/* ���ֹ� */
	strcat(cha_sql, " from sm_charge T1");
	dbres = Db_OpenDyna(cartdb, cha_sql);
	if(!dbres) {
		memset(cha_sql, '\0', sizeof(cha_sql));
		sprintf(cha_sql, "�ݥ���Ƚ���ͤμ����˼��Ԥ��ޤ�����(%s)(%d)", Gcha_last_error, __LINE__);
		return 1;
	}
	in_default_point = atoi(Db_GetValue(dbres, 0, 0));
	in_new_customer_no = atoi(Db_GetValue(dbres, 0, 1));
	Db_CloseDyna(dbres);
	/* ������󿷵���Ͽ */
	memset(cha_sql, '\0', sizeof(cha_sql));
	strcpy(cha_sql, "insert into st_customer");
	strcat(cha_sql, "(");
	strcat(cha_sql, "n_customer_no");
	strcat(cha_sql, "c_user");
	strcat(cha_sql, ", c_pass");
	strcat(cha_sql, ", c_mail");
	strcat(cha_sql, ", n_point");
	strcat(cha_sql, ", n_rank");
	strcat(cha_sql, ")");
	strcat(cha_sql, " values(");
	sprintf(&cha_sql[strlen(cha_sql)], "%d", in_new_customer_no);
	strcat(cha_sql, ",'");
	strcat(cha_sql, chp_id);
	strcat(cha_sql, "'");
	strcat(cha_sql, ",'");
	strcat(cha_sql, chp_pass);
	strcat(cha_sql, "'");
	strcat(cha_sql, ",'");
	strcat(cha_sql, chp_mail);
	strcat(cha_sql, "'");
	sprintf(&cha_sql[strlen(cha_sql)], ",%d", in_default_point);
	sprintf(&cha_sql[strlen(cha_sql)], ",%d", CO_SC_DEFAULT_CUSTOMER_RANK_NO);
	strcat(cha_sql, ")");
	in_error = Db_ExecSql(cartdb, cha_sql);
	if(in_error == CO_SQL_ERR) {
		Rollback_Transact(cartdb);
		Put_Nlist(nlp_out, "ERROR_MSG", "����ǡ�������Ͽ�˼��Ԥ��ޤ�����<br>");
		return 1;
	}
	if(Gin_func_3) {
		strcpy(cha_sql, "insert into st_my_business");
		strcat(cha_sql, "(");
		strcat(cha_sql, "c_user");
		strcat(cha_sql, ", c_business_user");
		strcat(cha_sql, ")");
		strcat(cha_sql, " values(");
		sprintf(cha_sql + strlen(cha_sql), "'%s'", chp_id);
		strcat(cha_sql, ", null");
		strcat(cha_sql, ")");
		in_error = Db_ExecSql(cartdb, cha_sql);
		if(in_error == CO_SQL_ERR) {
			memset(cha_sql, '\0', sizeof(cha_sql));
			sprintf(cha_sql, "%d������ǡ�������Ͽ�˼��Ԥ��ޤ�����(%s)", __LINE__, Gcha_last_error);
			Put_Nlist(nlp_out, "ERROR_MSG", cha_sql);
			Rollback_Transact(cartdb);
			Db_Disconnect(cartdb);
			return 1;
		}
	}
	in_error = Commit_Transact(cartdb);
	if(in_error == CO_SQL_ERR) {
		Put_Nlist(nlp_out, "ERROR_MSG", "�ȥ�󥶥������Υ��ߥåȤ˼��Ԥ��ޤ�����");
		return 1;
	}
	nlp_cookie = Get_Cookie();
	in_error = U_Build_Login_Cookie(OldDBase(cartdb), nlp_in, nlp_out, nlp_cookie, in_new_customer_no);
	Finish_Nlist(nlp_cookie);
	Db_Disconnect(cartdb);

	return in_error;
}

/*
+* ========================================================================
 * Function:	Send_Blog_Update
 * Description:
 *
%* ========================================================================
 * Return:	<0	���顼
 * 			=0	��������
 *			>0	������Ѥߥ֥�ID
-* ========================================================================
 */
int Send_Blog_Update(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog)
{
	DBase *cartdb;
	DBRes *dbres;
	char cha_subject[255];
	char cha_blog[255];
	char cha_sender[255];
	char cha_url[1024];
	char cha_sql[1024];
	char *chp_tmp;
	char *chp_mail;
	char *chp_body;
	char *chp_name;
	char *chp_cur_sub;
	char *chp_cur_body;

	sprintf(cha_url, "%s%s%s/%s?bid=%d", g_cha_protocol, getenv("SERVER_NAME"), g_cha_user_cgi, CO_CGI_BUILD_HTML, in_blog);

	dbres = Db_OpenDyna(db, "select c_managermail from sy_baseinfo");
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�֥�����μ����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	strcpy(cha_sender, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "manager@asj.ad.jp");
	Db_CloseDyna(dbres);

	sprintf(cha_sql, "select c_blog_title,c_mail_subject,c_mail_body from at_blog where n_blog_id = %d;", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�֥�����μ����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		return 1;
	}
	strcpy(cha_blog, Db_GetValue(dbres, 0, 0) ? Db_GetValue(dbres, 0, 0) : "");
	strcpy(cha_subject, Db_GetValue(dbres, 0, 1) ? Db_GetValue(dbres, 0, 1) : "");
	chp_body = malloc(strlen(Db_GetValue(dbres, 0, 2) ? Db_GetValue(dbres, 0, 2) : "") + 1);
	strcpy(chp_body, Db_GetValue(dbres, 0, 2) ? Db_GetValue(dbres, 0, 2) : "");
	Db_CloseDyna(dbres);

	// ��̾�����Ƥ��񤭹��ޤ�Ƥ��ʤ������������ʤ���
	if (!chp_body[0] || !cha_subject[0]) {
		return 0;
	}
/*
	if (!chp_body[0]) {
		free(chp_body);
		asprintf(&chp_body, "%%c����\r\n�֥��򹹿����ޤ����ΤǤ��Τ餻���ޤ�������URL��ꥢ���������Ƥ���������\r\n%%u\r\n");
	}
	if (!cha_subject[0]) {
		strcpy(cha_subject, "[ŹĹ�֥�]�֥��򹹿����ޤ���");
	}
*/
	cartdb = Open_Cart_Db(db, nlp_out, in_blog);
	if (!cartdb) {
		Put_Nlist(nlp_out, "ERROR", "������DB�򳫤��ޤ���Ǥ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		free(chp_body);
		return 1;
	}

	dbres = Db_OpenDyna(cartdb,
		" select c_mail"
		",c_name"
		" from st_customer T1"
		" where T1.b_email != 0"
		" and c_mail is not null"
		" and c_mail != ''");
	if (!dbres) {
		Put_Nlist(nlp_out, "ERROR", "�ܵҾ���μ����˼��Ԥ��ޤ�����");
		Put_Nlist(nlp_out, "QUERY", Gcha_last_error);
		Db_Disconnect(cartdb);
	}
	if (!fork()) {
		while (Db_FetchNext(dbres) == CO_SQL_OK) {
			chp_mail = Db_FetchValue(dbres, 0) ? Db_FetchValue(dbres, 0) : "";
			chp_name = Db_FetchValue(dbres, 1) ? Db_FetchValue(dbres, 1) : "";

			chp_cur_sub = malloc(strlen(cha_subject) + 1);
			strcpy(chp_cur_sub, cha_subject);
			chp_cur_body = malloc(strlen(chp_body) + 1);
			strcpy(chp_cur_body, chp_body);

			chp_tmp = malloc(strlen(chp_cur_sub) + strlen(cha_blog) + 1);
			strcpy(chp_tmp, chp_cur_sub);
			Replace_String("%b", cha_blog, chp_tmp);
			free(chp_cur_sub);
			chp_cur_sub = chp_tmp;

			chp_tmp = malloc(strlen(chp_cur_sub) + strlen(chp_name) + 1);
			strcpy(chp_tmp, chp_cur_sub);
			Replace_String("%c", chp_name, chp_tmp);
			free(chp_cur_sub);
			chp_cur_sub = chp_tmp;

			chp_tmp = malloc(strlen(chp_cur_sub) + strlen(cha_url) + 1);
			strcpy(chp_tmp, chp_cur_sub);
			Replace_String("%u", cha_url, chp_tmp);
			free(chp_cur_sub);
			chp_cur_sub = chp_tmp;

			chp_tmp = malloc(strlen(chp_cur_body) + strlen(cha_blog) + 1);
			strcpy(chp_tmp, chp_cur_body);
			Replace_String("%b", cha_blog, chp_tmp);
			free(chp_cur_body);
			chp_cur_body = chp_tmp;

			chp_tmp = malloc(strlen(chp_cur_body) + strlen(chp_name) + 1);
			strcpy(chp_tmp, chp_cur_body);
			Replace_String("%c", chp_name, chp_tmp);
			free(chp_cur_body);
			chp_cur_body = chp_tmp;

			chp_tmp = malloc(strlen(chp_cur_body) + strlen(cha_url) + 1);
			strcpy(chp_tmp, chp_cur_body);
			Replace_String("%u", cha_url, chp_tmp);
			free(chp_cur_body);
			chp_cur_body = chp_tmp;

			Send_Mail(cha_sender, chp_mail, chp_cur_sub, chp_cur_body);
			free(chp_cur_sub);
			free(chp_cur_body);
		}
		free(chp_body);
		Db_CloseDyna(dbres);
		Db_Disconnect(cartdb);
		exit(0);
	}
	free(chp_body);
	Db_CloseDyna(dbres);
	Db_Disconnect(cartdb);

	return 0;
}
