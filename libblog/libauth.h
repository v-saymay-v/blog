#ifndef __LIBAUTH_H__
#define __LIBAUTH_H__

#include "libcgi2.h"
#include "blog.h"
#include "libmysql.h"

int Read_From_Pass_File(NLIST *nlp_out, const char *chp_file, char *chp_user, char *chp_pass);
int Check_Login(DBase *db, NLIST *nlp_out, int *inp_owner, int *inp_auth);
int Auth_Login(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_loginname, char *chp_passname, int *inp_owner);
int Clear_Login(DBase *db, NLIST *nlp_out, int in_blog);
int Check_Retire(DBase *db, NLIST *nlp_out, int in_blog);
int Check_Remote_User(DBase *db, NLIST *nlp_out);
int Check_Unix_User(DBase *db, NLIST *nlp_out);
void Disp_Login_Page(NLIST *nlp_in, NLIST *nlp_out, char* chp_action);
int Disp_Remind_Page(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char* chp_action);
void Disp_Remind_Finish(NLIST *nlp_in, NLIST *nlp_out, char* chp_action);
int Auth_Reminder(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char* chp_action);

int Set_My_Avatar(DBase *db, NLIST *nlp_out, const char *chp_name, int in_blog);
int Disp_Team_Player(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_err, const char *chp_name, const char *chp_hidden);
int Build_Team_Combo(DBase *db, NLIST *nlp_in, NLIST *nlp_out, const char *chp_name, int in_blog, const char *chp_hidden, const char *chp_team, const char *chp_date);

int Get_Nickname(DBase *db, NLIST *nlp_out, char *chp_nickname, char *chp_mailaddr, char *chp_key);
int Get_Nickname_From_Blog(DBase *db, NLIST *nlp_out, int in_blog, char *chp_nickname);
int Get_Nickname_From_Owner(DBase *db, NLIST *nlp_out, int in_owner, char *chp_nickname);
int Get_Owner_From_Nickname(DBase *db, NLIST *nlp_out, char *chp_nickname);
int In_Black_List(DBase *db, NLIST *nlp_out, int in_owner, char *chp_key);
int Is_Black_List(DBase *db, NLIST *nlp_out, const char *chp_access_key);
int Is_Caution_Owner(DBase *db, NLIST *nlp_out, const char *chp_access_key);
int Get_Blog_Auth(DBase *db, NLIST *nlp_out, int in_owner, int in_blog);

char *Get_DBB_Topic(DBase *db, NLIST *nlp_out, int in_blog, char *chp_kind, char *chp_id, char *chp_owner, char *chp_url);

int Is_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog);
int Add_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog);
int Del_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog);
int Count_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog);
int Clear_Favorite_Blog(DBase *db, NLIST *nlp_out, int in_blog);

int Temp_To_Owner(DBase *db, NLIST *nlp_out, const char* chp_temp_id);
int Temp_To_Blog(DBase *db, NLIST *nlp_out, const char* chp_temp_id);
int Owner_To_Temp(DBase *db, NLIST *nlp_out, int in_owner, char* chp_temp_id);
int Blog_To_Temp(DBase *db, NLIST *nlp_out, int in_blog, char* chp_temp_id);

int Accept_Only_Friend(DBase *db, NLIST *nlp_out, int in_blog);
int Is_Ball_Friend(DBase *db, NLIST *nlp_out, int in_blog, int in_login_blog);

int Jump_To_Login_Cgi(DBase *db, NLIST *nlp_out);
int Disp_Battle_Player(DBase *db, NLIST *nlp_in, NLIST *nlp_out, NLIST *nlp_err, const char *chp_name, const char *chp_hidden, int in_blog);
int In_Battle_Owner(DBase *db, NLIST *nlp_out, int in_owner, char *chp_battle);

#endif
