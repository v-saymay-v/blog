#ifndef __LIBBLOG_H__
#define __LIBBLOG_H__

#include "libcgi2.h"
#include "blog.h"
#include "libmysql.h"

extern int g_in_ssl_mode;
extern int g_in_dbb_mode;
extern int g_in_cart_mode;
extern int g_in_hb_mode;
extern int g_in_hosting_mode;
extern int g_in_short_name;
extern int g_in_html_editor;
extern int g_in_admin_mode;
extern int g_in_login_owner;
extern char g_cha_protocol[256];
extern char g_cha_asp_skeleton[256];
extern char g_cha_admin_skeleton[256];
extern char g_cha_user_skeleton[256];
extern char g_cha_rss_path[256];
extern char g_cha_css_path[256];
extern char g_cha_upload_path[256];
extern char g_cha_asp_cgi[256];
extern char g_cha_admin_cgi[256];
extern char g_cha_user_cgi[256];
extern char g_cha_admin_image[256];
extern char g_cha_user_image[256];
extern char g_cha_theme_image[256];
extern char g_cha_css_location[256];
extern char g_cha_rss_location[256];
extern char g_cha_script_location[256];
extern char g_cha_base_location[256];
extern char g_cha_host_name[256];
extern char g_cha_blog_temp[256];

extern int g_in_need_login;
extern char gcha_theme_first_icon[256];
extern char gcha_theme_last_icon[256];
extern char gcha_theme_prev_entry_icon[256];
extern char gcha_theme_next_entry_icon[256];
extern char gcha_theme_prev_page_icon[256];
extern char gcha_theme_next_page_icon[256];
extern char gcha_theme_plus_icon[256];
extern char gcha_theme_minus_icon[256];
extern char gcha_theme_back_top_icon[256];
extern char gcha_theme_back_blogtop_icon[256];

static void debug_log(char *chp_format, ...);
void Replace_String(const char *chp_before, const char *chp_after, char *chp_string);

int Read_Ini(const char* chp_ini, char* chp_host, char* chp_db, char* chp_username, char* chp_password, int *inp_ssl);
int Read_Blog_Ini(char* chp_host, char* chp_db, char* chp_username, char* chp_password, int *inp_ssl);
int Read_Blog_Option(DBase *db);
int Get_Browser(void);
int Get_OperatingSystem(void);
void Build_Radio_With_Id(NLIST *nlp_out, char *chp_insert, char *chp_name, char *chp_value, int in_flag, char *chp_id, char *chp_message);
void Build_Checkbox_With_Id(NLIST *nlp_out, char *chp_insert, char *chp_name, char *chp_value, int in_flag, char *chp_id, char *chp_message);
int Get_Create_Time_By_Entry_Id(DBase *db, int in_entry_id, int in_blog, char *chp_create_time);
int Get_Total_Entry(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_mode, int in_blog);
int Get_Total_Comment(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_all);
int Get_Total_Comment_By_Entry_Id(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_entry_id, int in_blog, int in_all);
int Get_Total_Trackback(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_all);
int Get_Total_Trackback_By_Entry_Id(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_entry_id, int in_blog, int in_all);
int Get_Id_Latest_Entry_Current_Time(DBase *db, int in_blog);
int Get_Id_Latest_Entry(DBase *db, int in_blog);
int Get_Id_Latest_Entry_Create_Time_Not_Draft(DBase *db, int in_blog);
int Get_Id_Oldest_Entry_Create_Time_Not_Draft(DBase *db, int in_blog);
int Get_Id_Prev_Entry_Create_Time(DBase *db, int in_entry_id, int in_blog);
int Get_Id_Next_Entry_Create_Time(DBase *db, int in_entry_id, int in_blog);
int Exist_Entry_Id(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog);
int Get_Count_Category(DBase *db, int in_blog);
int Get_Latest_Entry_Year(DBase *db, int in_blog);
int Get_Oldest_Entry_Year(DBase *db, int in_blog);
int Get_Category_By_Cid(DBase *db, NLIST *nlp_out, int in_cid, int in_blog, char *chp_category);
int Get_Count_Link_In_Linklist(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_linkgroup_id, int in_blog);
int Get_Flag_Public(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog);
int Get_Flag_Acceptable_Comment(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog, int in_login_blog);
int Get_Flag_Acceptable_Trackback(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog);

int Check_Space_Only(char *chp_str);
int Check_URL_With_Crlf(NLIST *nlp_out, char *chp_source, const char *chp_name, int in_max);
//int Check_Alnum_Opt(char *chp_source, char *chp_reject);

int Create_Rss(DBase *db, NLIST *nlp_out, int in_blog);
int Write_To_Xml(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, FILE* fp_out);
int Read_Xml_From_File(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, char* chp_filename);

void Reformat_Date(char *chp_dest, char *chp_src);

int Make_Tmptable_For_Count(DBase *db, NLIST *nlp_out, int in_blog, int in_all);

void Put_Format_Nlist(NLIST *nlp_out, char *chp_insert, char *chp_format, ...);
int Change_Order_Db_Blog(DBase *db ,char *chp_id, char *chp_name_id, char *chp_name_seq, char *chp_name_table, char *chp_where1, char *chp_where2, char *chp_mod);
char* Conv_Blog_Br(char *chp_string);
char* Conv_Long_Ascii(char *chp_string, int in_len);

char* Copy_Str(const char* chp_str);
char* Concat_Str(const char* chp_str1, const char* chp_str2);
void Add_Str(char** chpp_org, const char* chp_add);
char* Replace_Str(const char* chp_org, const char* chp_before, const char* chp_after);

char* Page_Control_Str_by_Sql(DBase *db, const char* chp_sql, const char* chp_myname, const char* chp_param, int in_items_per_page, int in_start, int in_pagestart, int* inp_rows);

int Match_Filter(const char* chp_entry, const char* chp_filter);
int Check_RBL();

int Duplicate_Login(DBase *db, NLIST *nlp_out, int in_blog, char *chp_login);
int Need_Login(DBase *db, int in_blog);

int Build_MultiComboDb_Blog(DBase *db, NLIST *nlp_out, char *chp_insert, char *chp_name, char *chp_query, int in_row, char *chp_selected);

void Dialog_Message(NLIST *nlp_out, char *chp_insert, char *chp_message);
void Dialog_Message_Query(NLIST *nlp_out, char *chp_insert, char *chp_message, char *chp_query);
void Dialog_Message_Jump(char *chp_message, char *chp_jump);

char* Get_String_After(char *chp_str, int in_len);
int Decode_Blog_Form(char* chp_value);
char* Encode_Blog_Url(const char* chp_url);

void article_table_image_head(NLIST *nlp_in, NLIST *nlp_out);
void article_table_image_tail(NLIST *nlp_in, NLIST *nlp_out);

int Create_Blog(DBase *db);

char *Remove_Invalid_Tags(char *chp_source, NLIST *nlp_out, int in_mobile);
int Calc_Body_Point(DBase *db, NLIST *nlp_out, int in_blog, int in_entry, int in_new, const char *chp_source, const char *chp_access_key);
void Delete_Old_Entry(DBase *db);

int check_future(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);
int insert_link(DBase *db, NLIST *nlp_out, int in_blog, int in_entry, char *chp_url, int in_trackback, int in_tb_blog, int in_tb_entry);
int trackback(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_new_entry_id, char *chp_tburl, int in_blog, int in_tbid, char *chp_key);
int trackback_ip_check(DBase *db, NLIST *nlp_out, int in_entry_id, int in_blog, int in_login_owner);
int comment_owner_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);
int comment_ip_check(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog);
int get_count_article_by_day(DBase *db, char *chp_day_top, char *chp_day_end, int in_blog);
int update_visitor_list(DBase *db, NLIST *nlp_in, NLIST *nlp_out, int in_blog, int in_login_blog);
void update_access_conter(DBase *db, int in_blog, int in_login_blog);

#define CO_BROWSER_OTHER	0
#define CO_BROWSER_IE		1
#define CO_BROWSER_NS47		2
#define CO_BROWSER_NS6		3
#define CO_BROWSER_OPERA	4
#define CO_BROWSER_SAFARI	5

#define CO_OS_OTHER		0
#define CO_OS_WIN		1
#define CO_OS_MAC		2

#endif
