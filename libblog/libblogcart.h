#ifndef __LIBBLOGCART_H__
#define __LIBBLOGCART_H__

#include "libcgi2.h"
#include "blog.h"
#include "libmysql.h"

extern char g_cha_cartuser[21];
extern char g_cha_imageloc[256];
extern char g_cha_item_table[21];

DBase* Open_Cart_Db(DBase *db, NLIST *nlp_out, int in_blog);
int Blog_Usable(DBase *db, NLIST *nlp_out, int in_blog);
int Build_Cart_Item(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_item_id, int in_btn, int *inp_display);
int build_article_cart(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_item, int in_btn, int *inp_display);
int Build_Item_Info(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);
int Build_Entry_Recomend(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);
int Get_Back_Number(DBase *db, NLIST *nlp_out, int in_blog);
int Build_Back_Number(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_where, int in_limit, int in_offset, int in_blog);
int Build_Mail_Kid(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);
int Get_Allow_Review(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_chk_login);
int Get_Review_Number(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_item_id);
int Build_Review_List(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry);
int Build_Review_Form(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_entry);
int Post_Review(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);
int Cart_Auth_Login(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_loginname, char *chp_passname);
int Regist_New_Customer(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_mail, char *chp_id, char *chp_pass, int in_blog);
int Send_Blog_Update(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);

#endif
