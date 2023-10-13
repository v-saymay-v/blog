#include "libcgi2.h"
#include "libblog.h"
#include "libauth.h"
#include "libmobile.h"
#include "blog_mobile_menu.h"

extern char g_cha_mobile_admin_cgi[23];

int Mobile_Admin_Menu(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_login_blog)
{
	int in_owner;
	int in_black;
	char cha_nickname[100];
	char cha_sql[100];
	char *chp_tmp;

	if (!Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_nickname)) {
		return 1;
	}
	Put_Nlist(nlp_out, "NAME", cha_nickname);

	in_owner = Get_Nickname_From_Blog(db, nlp_out, in_login_blog, cha_sql);
	in_black = In_Black_List(db, nlp_out, in_owner, NULL);

	if (!in_black) {
		NLIST *nlp_tmp;
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_login_blog, NULL);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_new_entry.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "ENTWRT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
		nlp_tmp = Init_Nlist();
		Mobile_Prepare_Form(db, nlp_in, nlp_tmp, in_login_blog, NULL);
		Conv_Depend_String(nlp_tmp);
		chp_tmp = Page_Out_Mem(nlp_tmp, "parts_link_edit_entry.skl");
		if (chp_tmp) {
			Put_Nlist(nlp_out, "ENTEDT", chp_tmp);
			free(chp_tmp);
		}
		Finish_Nlist(nlp_tmp);
	} else {
		Put_Nlist(nlp_out, "ENTWRT", "記事を書く");
		Put_Nlist(nlp_out, "ENTEDT", "記事を編集");
	}

	Mobile_Prepare_Form(db, nlp_in, nlp_out, in_login_blog, NULL);

	Conv_Depend_String(nlp_out);
	Page_Out_Mobile(nlp_out, "mobile_admin_menu.skl");

	return 0;
}
