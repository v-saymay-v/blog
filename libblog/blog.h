#ifndef _BLOG_H_
#define _BLOG_H_

#define CO_MAX_PATH		1024
#define CO_MAX_HOST		256
#define CO_MAX_DB		21
#define CO_MAX_USER		21
#define CO_MAX_PASS		21
#define CO_FALSE		0
#define CO_TRUE			1
#define CO_ERROR		-1

/* �֥��ξ��� */
#define CO_BLOG_STATUS_ACTIVE	1
#define CO_BLOG_STATUS_STOP		2
#define CO_BLOG_STATUS_CANCEL	3

/* �֥���HotBiz���� */
#define CO_AUTH_WRITE		2
#define CO_AUTH_DELETE		3

/*�����ɥ���ƥ�ĤΥ�����*/
#define CO_SIDECONTENT_ADMIN_MENU			0
#define CO_SIDECONTENT_CALENDAR				1
#define CO_SIDECONTENT_ARCHIVE_MONTHLY		2
#define CO_SIDECONTENT_ARCHIVE_CATEGORY		3
#define CO_SIDECONTENT_ARCHIVE_COMMENT		4
#define CO_SIDECONTENT_ARCHIVE_TRACKBACK	5
#define CO_SIDECONTENT_PROFILE				6
#define CO_SIDECONTENT_RSS_FEED				7
#define CO_SIDECONTENT_ABOUT_BLOG			8
#define CO_SIDECONTENT_ARCHIVE_NEWENTRY		9
#define CO_SIDECONTENT_ARCHIVE_LINKS		10
#define CO_SIDECONTENT_ARCHIVE_RSS			11
#define CO_SIDECONTENT_BLOG_SEARCH			12
#define CO_SIDECONTENT_VISITOR_LIST			13
#define CO_SIDECONTENT_ACCESS_LOG			14
#define CO_SIDECONTENT_ITEM_INFO			15
#define CO_SIDECONTENT_ENTRY_RECOMEND		16
#define CO_SIDECONTENT_CART_LOGIN			17
#define CO_SIDECONTENT_REGIST_CUSTOMER		18
#define CO_SIDECONTENT_BLOG_PARTS			19

/* �ȥåץڡ�����ɽ�����뵭���ο� */
#define CO_MAX_DISP_TOPPAGE_INDEX			50
#define CO_MAX_DISP_TOPPAGE_DAY				31
#define CO_MAX_TOPPAGE_ARTICLE				100		/* ξ����ˤ���������� */
#define CO_RANKING_BLOGS					50
#define CO_MAX_RANKING						100

/* ʸ��Ĺ������� */
#define CO_MAXLEN_CATEGORY_NAME				30		/* ���ƥ���̾ */
#define CO_MAXLEN_ENTRY_TITLE				100		/* ���������ȥ� */
#define CO_MAXLEN_ENTRY_SUMMARY				200		/* �������� */
#define CO_MAXLEN_ENTRY_BODY				8000	/* �������� */
#define CO_MAXLEN_ENTRY_MORE				8000	/* ����³�� */
#define CO_MAXLEN_TRACKBACK_URL				1000	/* �ȥ�å��Хå�URL */
#define CO_MAXLEN_UPDATE_PING				1000	/* ���åץǡ���PING */
#define CO_MAXLEN_BLOG_TITLE				50		/* �֥������ȥ� */
#define CO_MAXLEN_BLOG_SUBTITLE				200		/* �֥����֥����ȥ� */
#define CO_MAXLEN_BLOG_DESCRIPTION			1000	/* �֥����� */
#define CO_MAXLEN_BLOG_AUTHOR_NICKNAME		100		/* ɮ�ԥ˥å��͡��� */
#define CO_MAXLEN_BLOG_AUTHOR_FIRSTNAME		30		/* ɮ��̾ */
#define CO_MAXLEN_BLOG_AUTHOR_SECONDNAME	30		/* ɮ���� */
#define CO_MAXLEN_BLOG_AUTHOR_SELFINTRO		1024	/* ɮ�Լ��ʾҲ� */
#define CO_MAXLEN_BLOG_AUTHOR_MAIL			200		/* ɮ�ԥ᡼�륢�ɥ쥹 */
#define CO_MAXLEN_BLOG_LOGIN				20		/* ������̾ */
#define CO_MAXLEN_BLOG_PASSWORD				20		/* �ѥ���� */
#define CO_MAXLEN_COMMENT_AUTHOR_NAME		100		/* �����ȼ�̾ */
#define CO_MAXLEN_COMMENT_AUTHOR_MAIL		200		/* �����ȥ᡼�륢�ɥ쥹 */
#define CO_MAXLEN_COMMENT_AUTHOR_URL		200		/* ������URL */
#define CO_MAXLEN_COMMENT					2000	/* ��������ʸ */
#define CO_MAXLEN_SIDECONTENT_TITLE			200		/* �����ɥ���ƥ�ĥ����ȥ� */
#define CO_MAXLEN_LINK_TITLE				100		/* ��󥯥����ȥ� */
#define CO_MAXLEN_LINK_URL					200		/* ���URL */
#define CO_MAXLEN_TRACKBACK_TITLE			100		/* �ȥ�å��Хå��������ȥ� */
#define CO_MAXLEN_TRACKBACK_EXCERPT			200		/* �ȥ�å��Хå������� */
#define CO_MAXLEN_TRACKBACK_BLOG_NAME		200		/* �ȥ�å��Хå����֥�̾ */
#define CO_MAXLEN_TRACKBACK_ARTICLE_URL		100		/* �ȥ�å��Хå���URL */
#define CO_MAXLEN_USER_CSS					500		/* �桼����CSS�ե�����̾ */
#define CO_MAXLEN_BLOG_REMIND_ANSWER		255		/* ��ޥ���������� */
#define CO_MAXLEN_RSS_TITLE					50		/* RSS�����ȥ� */
#define CO_MAXLEN_RSS_URL					200		/* RSS�����߸�URL */
#define CO_MAXLEN_LINKITEM_TITLE			100		/* ��󥯥����ȥ� */
#define CO_MAXLEN_LINKITEM_URL				200		/* ���URL */
#define CO_MAXLEN_LINKLIST_TITLE			50		/* ��󥯽������ȥ� */
#define CO_MAXLEN_FILTER_STRING				1024	/* �ե��륿����ʸ����κ���Ĺ */
#define CO_MAXLEN_FRIEND_REASON				1024	/* ��ͧã�ʤꤿ����ͳ�κ���Ĺ */
#define CO_MAXLEN_PARTS_TITLE				50		/* �֥��ѡ��ĤΥ����ȥ� */
#define CO_MAXLEN_PARTS_BODY				4096	/* �֥��ѡ��ĤΥ����� */

#define CO_MAX_DENIAL_COMMENT_HOST			50		/* �����ȵ��ݥۥ��� */
#define CO_MAX_DENIAL_TRACKBACK_HOST		50		/* �ȥ�å��Хå����ݥۥ��� */
#define CO_PING_TEXTAREA_COL				50		/* �ƥ����ȥ��ꥢ�� */
#define CO_PING_TEXTAREA_ROW				5		/* �ƥ����ȥ��ꥢ�� */
#define CO_MAX_RSS_IMPORTS					10		/* RSS�����ߺ���� */
#define CO_MAX_RSS_ARTICLE					30		/* RSS������1�濫����κ��絭���� */
#define CO_MAX_LINK_LISTS					10		/* ��󥯥ꥹ�Ⱥ���� */
#define CO_MAX_LINK_ITEM					30		/* ��󥯥ꥹ��1�濫����κ��絭���� */
#define CO_MAX_ARCHIVE_COMMENT				50		/* �����ȥ���������ɽ���� */
#define CO_MAX_ARCHIVE_TRACKBACK			50		/* �ȥ�å��Хå�����������ɽ���� */
#define CO_MAX_ARCHIVE_NEWENTRY				50		/* ������ƥ���������ɽ���� */
#define CO_MAX_ARCHIVE_MONTHLY				10		/* ���̥���������ɽ���� */
#define CO_PERPAGE_ARTICLE					10		/* �����ڡ���1�ڡ����������ɽ����� */
#define CO_PERPAGE_THEME					20		/* �ǥ���������ڡ���1�ڡ����������ɽ����� */
#define CO_RETURN_THEME						4		/* �ǥ���������ڡ�����Ԥ������ɽ����� */
#define CO_RETURN_CATEGORY					4		/* ����������ڡ�����Ԥ������ɽ����� */
#define CO_MAX_UPLOAD_SIZE					512		/* ���åץ��ɥ�����������kB */
#define CO_RSS_MAX							10		/* rss�ۿ������� */
#define CO_LENGTH_TIMESTAMP					12		/* yyyymmddhhmm �����ѥ����ॹ����� */
#define CO_LENGTH_TIMESTAMP_WITH_SEC		14		/* yyyymmddhhmmss �����ȡ��ȥ�å��Хå��ѥ����ॹ����� */
#define CO_MAX_TRACKBACK_URLS				5		/* �ȥ�å��Хå�����κ����� */
#define CO_MAX_UPDATEPING_URLS				5		/* ����PING��������κ����� */
#define CO_MAX_CATEGORIES					30		/* 1�֥�������κ��祫�ƥ���� */
#define CO_BACKNO_CATEGORY					65535	/* �����ȥ⡼�ɻ��β��ۥ��ƥ���֥Хå��ʥ�С��פΥ��ƥ���ID */
#define CO_MAX_FUTURE_ENTRY					3		/* DBB�⡼�ɻ���̤������դ���ƤǤ��뵭���� */
#define CO_MAX_BLOG_PARTS					10		/* ��Ͽ�Ǥ���֥��ѡ��ĺ���� */

/* �ե��륿�� */
#define CO_URL_FILTER			1	/* �ȥ�å��Хå���URL�ե��륿�� */
#define CO_BLOG_FILTER			2	/* �ȥ�å��Хå����֥�̾�ե��륿�� */
#define CO_CONTENTS_FILTER		3	/* �ȥ�å��Хå���ʸ�ե��륿�� */
#define CO_ENTRY_FILTER			4	/* �ȥ�å��Хå��������ե��륿�� */

/* ���ʾ��� */
#define CO_ITEMINFO_NEW			1	/* ���徦�� */
#define CO_ITEMINFO_RECOMEND	2	/* ��������� */
#define CO_ITEMINFO_RANDOM		3	/* ������ɽ�� */

/* �����ȡ��ȥ�å��Хå����վ��� */
#define CO_CMTRB_DENY			0	/* ���� */
#define CO_CMTRB_ACCEPT			1	/* ���� */
#define CO_CMTRB_QUERY			2	/* ��ǧ */
#define CO_CMTRB_FRIEND			3	/* ��ͧ */

/* �����ȥ⡼�ɤμ��� */
#define CO_CART_SHOPPER		1	/* ����åѡ��ץ饹 */
#define CO_CART_RESERVE		2	/* ����åѡ��ץ饹 */

/* �ݥ���Ⱦܺ� */
#define CO_POINT_BLACKLIST	6
#define CO_POINT_CAUTION	7
#define CO_POINT_SPECIAL	8

/* ʸ�� */
#define CO_ARROW		"��"
#define CO_CHECKED		"checked"
#define CO_LOGIN_ID		"btn_id"
#define CO_PASSWORD		"btn_pass"
#define CO_QUESTION		"btn_question"
#define CO_ANSWER		"btn_answer"
#define CO_DISP_REMIND	"btn_remind.x"
#define CO_SEND_REMIND	"btn_send_remind.x"

/* CGI̾ */
#define CO_CGI_MENU			"blog_menu.cgi"
#define CO_CGI_LOOKS		"blog_looks.cgi"
#define CO_CGI_CATEGORY		"blog_category.cgi"
#define CO_CGI_CONFIGURE	"blog_configure.cgi"
#define CO_CGI_SIDECONTENT	"blog_sidecontent.cgi"
#define CO_CGI_ENTRY		"blog_entry.cgi"
#define CO_CGI_COMMENT_LIST	"blog_comment_list.cgi"
#define CO_CGI_TB_LIST		"blog_tb_list.cgi"
#define CO_CGI_ENTRY_LIST	"blog_entry_list.cgi"
#define CO_CGI_BOOKMARKLET	"blog_bookmarklet.cgi"
#define CO_CGI_TB			"tb.cgi"
#define CO_CGI_BUILD_HTML	"blog_build_html.cgi"
#define CO_CGI_BLOG_LIST	"http://cl01.d-bb.com/cgi-bin/dbb_blog_list.cgi"
#define CO_CGI_RANKING		"blog_ranking.cgi"

/* ������ȥ�admin */
#define CO_SKEL_ACCEPT_COMMENT_ASK		"blog_accept_comment_ask.skl"
#define CO_SKEL_ACCEPT_TRACKBACK_ASK	"blog_accept_trackback_ask.skl"
#define CO_SKEL_BLOG_IMPORT				"blog_entry_import.skl"
#define CO_SKEL_BOOKMARKLET				"blog_bookmarklet.skl"
#define CO_SKEL_CATEGORY_EDIT			"blog_category_edit.skl"
#define CO_SKEL_CATEGORY_LIST			"blog_category_list.skl"
#define CO_SKEL_CATEGORY_LIST_EMPTY		"blog_category_list_empty.skl"
#define CO_SKEL_COMMENT_LIST			"blog_comment_list.skl"
#define CO_SKEL_CONFIGURE				"blog_configure.skl"
#define CO_SKEL_DELETE_CATEGORY_ASK		"blog_delete_category_ask.skl"
#define CO_SKEL_DELETE_COMMENT_ASK		"blog_delete_comment_ask.skl"
#define CO_SKEL_DELETE_ENTRY_ASK		"blog_delete_entry_ask.skl"
#define CO_SKEL_DELETE_FILE_ASK			"blog_delete_file_ask.skl"
#define CO_SKEL_DELETE_LINKITEM_ASK		"blog_delete_linkitem_ask.skl"
#define CO_SKEL_DELETE_LINKLIST_ASK		"blog_delete_linklist_ask.skl"
#define CO_SKEL_DELETE_RSS_ASK			"blog_delete_rss_ask.skl"
#define CO_SKEL_DELETE_TRACKBACK_ASK	"blog_delete_trackback_ask.skl"
#define CO_SKEL_DELETE_PARTS_ASK		"blog_delete_parts_ask.skl"
#define CO_SKEL_EDITENTRY				"blog_editentry.skl"
#define CO_SKEL_EDITENTRY_BUFFER		"blog_editentry_buffer.skl"
#define CO_SKEL_ENTRY_LIST				"blog_entry_list.skl"
#define CO_SKEL_LINKITEM				"blog_linkitem.skl"
#define CO_SKEL_LINKLIST				"blog_linklist.skl"
#define CO_SKEL_LINKLIST_TITLE			"blog_linklist_title.skl"
#define CO_SKEL_LOGIN					"blog_login.skl"
#define CO_SKEL_LOOKS					"blog_looks.skl"
#define CO_SKEL_LOOKS_CATEGORY			"blog_looks_category.skl"
#define CO_SKEL_POST_COMMENT_ASK		"blog_post_comment_ask.skl"
#define CO_SKEL_POST_REVIEW_ASK			"blog_post_review_ask.skl"
#define CO_SKEL_PREVIEW					"blog_preview.skl"
#define CO_SKEL_PROFILE					"blog_profile.skl"
#define CO_SKEL_REMIND					"blog_remind.skl"
#define CO_SKEL_REMIND_FINISH			"blog_remind_finish.skl"
#define CO_SKEL_RSS						"blog_rss.skl"
#define CO_SKEL_RSS_NEW					"blog_rss_new.skl"
#define CO_SKEL_SEARCH_RESULT			"blog_search_result.skl"
#define CO_SKEL_SIDECONTENT				"blog_sidecontent.skl"
#define CO_SKEL_TB_RESPONSE				"blog_tb_response.skl"
#define CO_SKEL_TBLIST_RESPONSE			"blog_tblist_response.skl"
#define CO_SKEL_TOPMENU					"blog_topmenu.skl"
#define CO_SKEL_TRACKBACK_LIST			"blog_trackback_list.skl"
#define CO_SKEL_BLOG_RANKING			"blog_ranking.skl"
#define CO_SKEL_FRIEND_SHIP				"blog_friend_ship.skl"
#define CO_SKEL_UPLOAD_MOVIE			"blog_upload_movie.skl"

/* ������ȥ�user */
#define CO_SKEL_ARTICLE_LIST		"blog_article_list.skl"
#define CO_SKEL_ARTICLE				"blog_article.skl"
#define CO_SKEL_USERTOP_LEFT		"blog_usertop_left.skl"
#define CO_SKEL_USERTOP_RIGHT		"blog_usertop_right.skl"
#define CO_SKEL_VISITOR_LIST		"blog_visitor_list.skl"
#define CO_SKEL_FRIEND_LIST			"blog_friend_list.skl"
#define CO_SKEL_FRIEND_UPDATE		"blog_friend_update.skl"
#define CO_SKEL_UPDATE_FRIEND_ASK	"blog_update_friend_ask.skl"

/* ���顼������ȥ� */
#define CO_SKEL_ERROR				"blog_error.skl"
#define CO_SKEL_CONFIRM				"blog_confirm.skl"

/* �ץ�ӥ塼�ѥ�����ȥ�Υ١���̾ */
#define CO_SKEL_LOOKS_PREVIEW		"blog_looks_preview"

#endif
