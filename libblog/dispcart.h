#ifndef _DISPCART_H_
#define _DISPCART_H_

extern int Gin_func_3;
extern int Gin_func_11;
extern int Gin_func_13;
extern char g_cha_purchase_cgi[1024];

int GetItemNo(DBRes *dbres, int in_page, int in_max, int in_row, int in_b_disp_variation);
int Build_Item_Table(DBase *db, NLIST *nlp_in, NLIST *nlp_out, DBRes *dbres, char *chp_dest, int *in_row, bool b_link, int in_max, int in_start, int in_cnt, int in_btn);
int Build_Item_Table2(DBase *db, NLIST *nlp_in, NLIST *nlp_out, DBRes *dbres, char *chp_undisp_itemid_list, int *in_row, bool b_link, int in_max, int in_start, int in_cnt, int in_btn);

#endif
