#include <stdlib.h>
#include <string.h>
#include "libcgi2.h"
#include "libmysql.h"
#include "libauth.h"
#include "libmobile.h"
#include "libblog.h"
#include "blog_mobile_top_page.h"

int Mobile_User_About_Blog(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog)
{
	DBRes *dbres;
	int in_entry;
	char cha_sql[512];
	char cha_nickname[100];
	char *chp_tmp;
	char *chp_esc;
	char *chp_bdy;

	chp_tmp = Get_Nlist(nlp_in, "eid", 1);
	in_entry = chp_tmp ? atoi(chp_tmp) : 0;

	sprintf(cha_sql, "select c_blog_title,c_blog_subtitle,c_blog_description from at_blog where n_blog_id = %d", in_blog);
	dbres = Db_OpenDyna(db, cha_sql);
	if (!dbres) {
		sprintf(cha_sql, "ｸｴﾘに失敗しました。(%d)<br>%s<br>%s<br>", __LINE__, Gcha_last_error, cha_sql);
		Put_Nlist(nlp_out, "ERROR", cha_sql);
		return 1;
	}
	chp_tmp = Db_GetValue(dbres, 0, 0);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "TITLE", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Db_GetValue(dbres, 0, 1);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		Put_Nlist(nlp_out, "SUBTITLE", chp_esc);
		free(chp_esc);
	}
	chp_tmp = Db_GetValue(dbres, 0, 2);
	if (chp_tmp) {
		chp_esc = Escape_HtmlString(chp_tmp);
		chp_bdy = Conv_Long_Ascii(chp_esc, 56);
		chp_tmp = Conv_Blog_Br(chp_bdy);
		Put_Nlist(nlp_out, "DESCRIPTION", chp_tmp);
		free(chp_bdy);
		free(chp_esc);
	}
	Db_CloseDyna(dbres);

	if (!Get_Nickname_From_Blog(db, nlp_out, in_blog, cha_nickname)) {
		return 1;
	}
	chp_esc = Escape_HtmlString(cha_nickname);
	Put_Nlist(nlp_out, "NICKNAME", chp_esc);
	free(chp_esc);

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_blog, NULL);

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_about_blog.skl");

	return 0;
}
