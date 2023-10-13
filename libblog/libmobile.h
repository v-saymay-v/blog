#ifndef __LIBMOBILE_H__
#define __LIBMOBILE_H__

#include "libcgi2.h"
#include "libmysql.h"

#define CO_MAXLEN_ENTRY_BODY_MOBILE			8000	/* 記事本文 */
#define CO_MAXLEN_ENTRY_TITLE_MOBILE		50		/* 記事タイトル */

extern char g_cha_mobile_user_cgi[22];
extern char g_cha_mobile_admin_cgi[23];

void Conv_Depend_String(NLIST *nlp_out);
int Check_Mobile_Login(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_owner, int *inp_admin);
int Jump_To_Login_Mobile_Cgi(DBase *db, NLIST *nlp_out);
void Mobile_Prepare_Form(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char *chp_page);
void Mobile_Error(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_page, char **chpp_hidden);
void Mobile_Error_Top(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_name, int in_confirm);
void Mobile_Confirm(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_page, char **chpp_hidden);
int Is_Post_Method();
int Mobile_Team_Combo(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, const char *chp_hidden, const char *chp_team, const char *chp_date);
char *Sjis_to_Euc(char *chp_string);
int Page_Out_Mobile(NLIST *nlp_name, char *chp_skeleton);

#endif
