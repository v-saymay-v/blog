-- ----------------------------------------------------------------------
-- blog��Υȥ�󥶥������ơ��֥�
-- ----------------------------------------------------------------------

drop table if exists at_blog;
create table at_blog(
	n_blog_id integer auto_increment not null,
	c_blog_id varchar(20),						-- �桼����ID
	c_blog_title varchar(100),					-- �����ȥ�
	c_blog_subtitle text,						-- ���֥����ȥ�
	c_blog_description text,					-- ����
	n_blog_status integer default 1,			-- �֥��ξ���
	n_blog_category integer,					-- �֥��Υ��ƥ���
	b_setting_toppage_disptype bool default 0,	-- �ȥåץڡ���ɽ�����(�������/������)
	n_setting_toppage_index integer default 7,	-- �ȥåץڡ�����ɽ�����
	n_setting_toppage_day integer default 7,	-- �ȥåץڡ�����ɽ������
	n_setting_disp_per_page integer default 10,	-- �ڡ����������ɽ�����
	n_setting_day_format integer default 1,		-- ���դ�ɽ������
	n_setting_time_format integer default 1,	-- �������֤�ɽ������
	n_setting_cut_length integer default 200,	-- ��...³�����ɤ�פ�ɽ������ʸ����
	b_default_trackback bool default 1,			-- �ȥ�å��Хå������
	b_default_comment bool default 1,			-- �����Ƚ����
	b_default_mode bool default 1,				-- �⡼�ɽ����
	n_hbuser_id integer default 0,				-- HotBiz�֥����ν�ͭ�ԥ桼����ID
	n_iteminfo_id integer default 1,			-- ���ʾ���ɽ���⡼��
	n_iteminfo_num integer default 3,			-- ���ʾ���ɽ����
	c_mail_subject varchar(255) default '%b����������',
												-- BLOG���������줿���Ȥ��Τ餻��᡼��η�̾
	c_mail_body text,							-- BLOG���������줿���Ȥ��Τ餻��᡼�����ʸ
	b_needlogin bool default 1,					-- �֥��λ��Ȥ�ǧ�ڤ�ɬ�פ�
	d_create_time datetime,						-- �֥�������
	constraint blog_pkey primary key(n_blog_id)
) TYPE = InnoDB;

drop table if exists at_profile;
create table at_profile(
	n_blog_id integer not null,
	c_author_nickname varchar(100) not null,	-- ��ԤΥ˥å��͡���
	c_author_firstname varchar(100),			-- ��Ԥ�̾
	c_author_lastname varchar(100),				-- ��Ԥ���
	c_author_selfintro text,					-- ��Ԥμ��ʾҲ�
	c_author_mail varchar(255),					-- ��ԤΥ᡼�륢�ɥ쥹
	c_author_image MEDIUMBLOB,					-- ��Բ����ե��������
	c_image_type varchar(50),					-- ��Բ����ե�����Content-Type
	n_image_size integer,						-- ��Բ����ե����륵����
	c_login varchar(20),						-- �����⡼�ɥ�����ID
	c_password varchar(20),						-- �����⡼�ɥ�����PASSWORD
	n_remind integer,							-- ��ޥ���������ֹ�
	c_remind varchar(255),						-- ��ޥ��������
	c_access_key varchar(255),					-- ���å��������ǡ���
	b_send_mail bool,							-- �����Ȥ�ȥ�å��Хå������ä���᡼�������
	b_japanese_calendar bool default 0,			-- �������������������ܸ��ɽ��
	n_next_entry integer default 1,				-- ���Υ���ȥ꡼��ID
	n_page_view bigint default 0,				-- �����ޤǤ���ڡ����ӥ塼
	n_unique_user bigint default 0,				-- �����ޤǤ����ˡ����桼����
	n_page_view_all bigint default 0,			-- ����ޤǤ���ڡ����ӥ塼
	n_unique_user_all bigint default 0,			-- ����ޤǤ����ˡ����桼����
	constraint profile_pkey primary key(n_blog_id)
) TYPE = InnoDB;

-- ɽ����������(CRON�ˤ��2����ʬ�Τ߳���)
drop table if exists at_view;
create table at_view (
	n_blog_id integer not null,					-- �Хʡ��ν�ͭ��(���饤�����)
	c_ip_addr varchar(20),						-- ����������IP
	c_user_agent varchar(255),					-- �֥饦������
	d_view timestamp,							-- ɽ������
	b_valid bool,								-- ͭ�����ä����ɤ���
	index (d_view),
	index (n_blog_id),
	index (c_ip_addr, n_blog_id)
) TYPE = InnoDB;

drop table if exists at_category;
create table at_category(
	n_blog_id integer not null,
	n_category_id integer not null,				-- ���ƥ��꡼ID
	n_category_order integer,					-- ���ƥ��꡼��������
	c_category_name varchar(30),				-- ���ƥ��꡼̾
	constraint category_pkey primary key (n_blog_id, n_category_id)
) TYPE = InnoDB;

drop table if exists at_looks;
create table at_looks(
	n_blog_id integer not null,
	n_category_id integer not null,				-- �ơ��ޥ��ƥ���
	n_theme_id integer not null,				-- �ơ���
	n_sidebar integer default 0,				-- �����ɥС�����
	c_user_css varchar(255),					-- �桼����css���
	constraint ping_pkey primary key (n_blog_id)
) TYPE = InnoDB;

drop table if exists at_entry;
create table at_entry(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- ID
	n_category_id integer not null,				-- ʬ�५�ƥ���
	b_comment bool not null,					-- �����ȼ��ղ���
	b_trackback bool not null,					-- �ȥ�å��Хå����ղ���
	b_mode bool not null,						-- ���� or ����
	c_entry_title varchar(100),					-- �����ȥ�
	c_entry_summary varchar(255),				-- �׻�
	c_entry_body text,							-- ��ʸ
	c_entry_more text,							-- ��ʸ�ʵ�����³����
	c_item_kind varchar(64),					-- ������񤤤����С�
	c_item_id varchar(64),						-- ������񤤤����롼��
	c_item_url varchar(255),					--
	c_item_owner integer,						--
	d_entry_create_time datetime,				-- ��Ի���κ�������
	d_entry_real_time datetime,					-- �ºݤκ�������
	d_entry_delete_time datetime,				-- ���λ��֤�᤮���鼫ưŪ�˺��
	d_entry_modify_time timestamp,				-- �ѹ�����
	constraint entry_pkey primary key (n_blog_id,n_entry_id)
) TYPE = InnoDB;

drop table if exists at_uploadfile;
create table at_uploadfile(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- ź���赭��
	n_uploadfile_id integer not null,			-- ���åץ��ɥե�����ID
	c_filename varchar(255) not null,			-- ���åץ��ɥե�����̾
	c_fileimage MEDIUMBLOB,						-- ���åץ��ɥե��������
	c_filetype varchar(255) not null,			-- ���åץ��ɥե��������
	n_filesize integer,							-- ���åץ��ɥե����륵����
	n_imagesize_x integer,						-- X�������᡼��������
	n_imagesize_y integer,						-- X�������᡼��������
	b_delete_mode bool not null,				-- 1 ���ͽ�� 0 ����ʳ�
	constraint uploadfile_pkey primary key (n_blog_id,n_entry_id,n_uploadfile_id)
) TYPE = InnoDB;

drop table if exists at_comment;
create table at_comment(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- �Ƶ���
	n_comment_id integer not null,				-- ID
	c_comment_author varchar(100),				-- �񤭹�����ͤ�̾��
	c_comment_ip varchar(128),					-- ip
	c_comment_mail varchar(255),				-- �᡼�륢�ɥ쥹
	c_comment_url varchar(255),					-- URL
	c_comment_body text,						-- ��ʸ
	d_comment_create_time datetime,				-- ��������
	b_comment_accept bool default 1,			-- ��ǧ���줿��
	b_comment_filter bool default 0,			-- �ե��륿���ˤ����ä���
	b_comment_read bool default 0,				-- �����Ȥ�ɽ��������
	constraint comment_pkey primary key (n_blog_id,n_entry_id,n_comment_id)
) TYPE = InnoDB;

drop table if exists at_comment_filter;
create table at_comment_filter(
	n_blog_id integer not null,
	c_filter text,								-- ��������ʸ�˴ޤޤ�Ƥ�����Ϥ���ʸ����
	b_valid bool,								-- �ե��륿����ͭ��
	constraint comment_filter_pkey primary key (n_blog_id)
) TYPE = InnoDB;

drop table if exists at_trackback;
create table at_trackback(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- TB�赭��
	n_tb_id integer not null,					-- TBID
	c_tb_title varchar(100),					-- TB�����ȥ�
	c_tb_excerpt text,							-- TB����
	c_tb_blog_name varchar(255) not null,		-- TB���֥�̾
	c_tb_url varchar(255) not null,				-- TB��URL
	c_tb_ip varchar(128),						-- TBIP
	d_tb_create_time datetime,					-- TB����
	b_tb_accept bool default 1,					-- ��ǧ���줿��
	n_tb_filter integer default 0,				-- �ե��륿���ˤ����ä���
	b_tb_read bool default 0,					-- TB��ɽ��������
	constraint trackback_pkey primary key (n_blog_id,n_entry_id,n_tb_id)
) TYPE = InnoDB;

drop table if exists at_trackback_filter;
create table at_trackback_filter(
	n_blog_id integer not null,
	n_filter_id integer not null,
	c_filter text,								-- �оݤ˴ޤޤ�Ƥ�����Ϥ���ʸ����
	b_valid bool,								-- �ե��륿����ͭ��
	constraint trackback_filter_pkey primary key (n_blog_id,n_filter_id)
) TYPE = InnoDB;

drop table if exists at_sendtb;
create table at_sendtb(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- TB�赭��
	n_tb_id integer not null,					-- TBID
	c_tb_title varchar(100),					-- TB�����ȥ�
	c_tb_excerpt text,							-- TB����
	c_tb_url varchar(255) not null,				-- TB��URL
	d_tb_create_time datetime,					-- TB����
	b_success bool,								-- �������
	constraint trackback_pkey primary key (n_blog_id,n_entry_id,n_tb_id)
) TYPE = InnoDB;

drop table if exists at_sendlink;
create table at_sendlink(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- ��󥯸�����
	n_link_id integer not null,					-- ���ID
	c_link_title varchar(255),					-- ��󥯥����ȥ�
	c_link_url varchar(255) not null,			-- �����URL
	b_link_tb bool,								-- �ȥ�å��Хå��ˤ���󥯤�
	constraint reratelink_pkey primary key (n_blog_id,n_entry_id,n_link_id)
) TYPE = InnoDB;

drop table if exists at_recvlink;
create table at_recvlink(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- ����赭��
	n_link_id integer not null,					-- ���ID
	n_from_blog integer not null,				-- ��󥯸��֥�
	n_from_entry integer not null,				-- ��󥯸�����
	constraint reratelink_pkey primary key (n_blog_id,n_entry_id,n_link_id)
) TYPE = InnoDB;

drop table if exists at_sidecontent;
create table at_sidecontent(
	n_blog_id integer not null,
	n_sidecontent_id integer not null,			-- �����ɥ���ƥ��ID
	n_sidecontent_order integer,				-- �����ɥ���ƥ�Ľ���
	n_sidecontent_type integer not null,		-- �����ɥ���ƥ�ļ���
	c_sidecontent_title varchar(255),			-- �����ɥ���ƥ�ĥ����ȥ�
	b_display bool,								-- ɽ������
	b_allow_outer bool,							-- �֥��˥�����ɬ�פʤȤ���ɽ�����뤫
	constraint sidecontent_pkey primary key (n_blog_id,n_sidecontent_id,n_sidecontent_type)
) TYPE = InnoDB;

drop table if exists at_archive;
create table at_archive(
	n_blog_id integer not null,
	n_archive_id integer not null,				-- ����������ID
	n_archive_value integer,					-- ɽ�����
	constraint archive_pkey primary key (n_blog_id,n_archive_id)
) TYPE = InnoDB;

drop table if exists at_linkitem;
create table at_linkitem(
	n_blog_id integer not null,
	n_linkitem_id integer not null,				-- ���ID
	n_linkgroup_id integer not null,			-- ��󥯥��롼��ID
	n_linkitem_order integer,					-- ��󥯥��롼�׽���
	c_link_title varchar(100) not null,			-- ��󥯥����ȥ�
	c_link_url varchar(255) not null,			-- ���URL
	b_link_target bool,							-- 0 Ʊ��������ɥ� 1 ����������ɥ�
	constraint linkitem_pkey primary key(n_blog_id, n_linkgroup_id, n_linkitem_id)
) TYPE = InnoDB;

drop table if exists at_rss;
create table at_rss(
	n_blog_id integer not null,
	n_rss_id integer not null,					-- �桼�������������RSSID
	c_rss_title varchar(100),					-- ������RSS�����ȥ�
	c_rss_url varchar(255),						-- RSS�ۿ���URL
	n_rss_article integer,						-- RSS������
	b_rss_target bool,							-- 0 Ʊ��������ɥ� 1 ����������ɥ�
	b_default bool,								-- ���������
	constraint rss_pkey primary key (n_blog_id,n_rss_id)
) TYPE = InnoDB;

drop table if exists at_ping;
create table at_ping(
	n_blog_id integer not null,
	n_ping_id integer not null,					-- �ԥ�������ID
	c_ping_site varchar(255),					-- �ԥ������襵����
	c_ping_url varchar(255),					-- �ԥ�����URL
	b_default bool,								-- ���������
	constraint ping_pkey primary key (n_blog_id,n_ping_id)
) TYPE = InnoDB;

drop table if exists at_denycomment;
create table at_denycomment (
	n_blog_id integer not null,
	n_host_id integer not null,
	c_host varchar(255),						-- �����ȵ��ݥۥ���
	constraint denycomment_pkey primary key (n_blog_id,n_host_id)
) TYPE = InnoDB;

drop table if exists at_denytrackback;
create table at_denytrackback (
	n_blog_id integer not null,
	n_host_id integer not null,
	c_host varchar(255),						-- �ȥ�å��Хå����ݥۥ���
	constraint denytrackback_pkey primary key (n_blog_id,n_host_id)
) TYPE = InnoDB;

drop table if exists at_visitor;
create table at_visitor (
	n_blog_id integer not null,
	n_visitor_id integer not null,
	n_owner_id integer,
	c_nickname varchar(100),
	c_mailaddr varchar(255),
	d_visit_time timestamp
) TYPE = InnoDB;

-- ----------------------------------------------------------------------
-- �����ƥ�ơ��֥�
-- ----------------------------------------------------------------------

drop table if exists sy_archive;
create table sy_archive(
	n_archive_id integer not null,				-- ����������ID
	n_archive_value integer,					-- ɽ�����
	constraint archive_pkey primary key (n_archive_id)
) TYPE = InnoDB;
insert into sy_archive values(2, 0);			-- ���̥���������
insert into sy_archive values(4, 5);			-- �����ȥ���������
insert into sy_archive values(5, 5);			-- �ȥ�å��Хå�����������
insert into sy_archive values(9, 5);			-- �ǿ���������������

drop table if exists sy_authinfo;
create table sy_authinfo(
	c_host varchar(255),						-- ǧ�ھ���DB������ۥ���
	c_db varchar(20),							-- ǧ�ھ���DB��̾��
	c_user varchar(20),							-- ǧ�ھ���DB�ؤΥ�����桼����
	c_password varchar(20),						-- ǧ�ھ���DB�ؤΥ�����ѥ����
	c_passfile varchar(255),					-- ǧ�ھ���DB�ؤΥ�����ѥ���ɤ�Ͽ�����ե�����
	c_table varchar(20),						-- ǧ�ھ���DB��Υǡ���������ơ��֥�
	c_table_login varchar(20),					-- ǧ�ھ���DB��Υ��å������󤬤���ơ��֥�
	c_column_login varchar(20),					-- ǧ�ھ���DB��Υ�����ID�Υ����
	c_column_password varchar(20),				-- ǧ�ھ���DB��Υ�����ѥ���ɤΥ����
	c_column_session varchar(20),				-- ǧ�ھ���DB��Υ��å�������Υ����
	c_column_blog varchar(20),					-- ǧ�ھ���DB��Υ֥�ID�Υ����
	c_column_owner varchar(20),					-- ǧ�ھ���DB��Υ����ʡ�ID�Υ����
	c_column_nickname varchar(20),				-- ǧ�ھ���DB��Υ˥å��͡���Υ����
	c_column_question varchar(20),				-- ǧ�ھ���DB��Υ�ޥ��������Υ����
	c_column_answer varchar(20),				-- ǧ�ھ���DB��Υ�ޥ���������Υ����
	c_column_mail varchar(20),					-- ǧ�ھ���DB��Υ᡼�륢�ɥ쥹�Υ����
	c_cookie varchar(20),						-- ���å��������񤭹���COOKIE̾
	c_owner_cookie varchar(20),					-- �����ʡ�ID��񤭹���COOKIE̾
	c_cookie_host varchar(255),					-- Cookie��񤭹���ۥ���
	c_cookie_path varchar(255),					-- Cookie��񤭹���ѥ�
	c_table_remid varchar(20),					-- ��ޥ�����μ���ơ��֥�
	c_remid_id varchar(20),						-- ��ޥ�����μ���ID�����
	c_remid_name varchar(20),					-- ��ޥ�����μ������ƥ����
	c_table_black varchar(20),					-- �֥�å��ꥹ�ȥơ��֥�
	c_black_blog varchar(20),					-- �֥�����Ͽ����Ƥ��뤫�Υ����
	c_table_detail varchar(20),					-- �֥�å��ꥹ�ȥơ��֥�
	c_mail_open varchar(20),					-- �֥�����Ͽ����Ƥ��뤫�Υ����
	c_image_location varchar(255),				-- ���Х����Υ��᡼��������Ȥ���
	c_login_cgi varchar(255)					-- �������Ѥ�CGI
) TYPE = InnoDB;
insert into sy_authinfo values (
	'localhost',
	'#USER#',
	'#USER#',
	'#FILE#',
	'#USERDIR#/data/hotbiz/password.dat',
	'USER',
	'',
	'LOGIN',
	'PASSWORD',
	'MD5',
	'BLOGID',
	'USERID',
	'NAME',
	'',
	'',
	'MAIL1',
	'HotBiz',
	'',
	'',
	'',
	'',
	'',
	'',
	'',
	'b_blog',
	'',
	'',
	'',
	'#SCRIPT#/../hotbiz.cgi');

drop table if exists sy_baseinfo;
create table sy_baseinfo(
	c_systemname varchar(255),
	c_managermail varchar(255),
	b_dbbmode bool,					-- �ɥ꡼��١����ܡ���Υ����ʡ��֥�
	b_cartmode bool,				-- ����åѡ��ץ饹��ŹĹ�֥�
	b_hbmode bool,					-- HotBiz�Υ��С��֥�
	b_hostingmode bool,				-- �����ԥ⡼�ɤؤ�ǧ����ˡ��Apache��ǧ�ڤ��ɤ���
	b_shortname bool,				-- �֥�����������/0000000x/�Τ褦����ˡ��Ȥ뤫�ɤ���
	b_useeditor bool,
	c_hostname varchar(255),
	c_aspskelpath varchar(255),
	c_adminskelpath varchar(255),
	c_userskelpath varchar(255),
	c_rsspath varchar(255),
	c_csspath varchar(255),
	c_uploadpath varchar(255),
	c_aspcgiloc varchar(255),
	c_admincgiloc varchar(255),
	c_usercgiloc varchar(255),
	c_adminimagesloc varchar(255),
	c_userimagesloc varchar(255),
	c_themeimagesloc varchar(255),
	c_cssloc varchar(255),
	c_rssloc varchar(255),
	c_scriptsloc varchar(255),
	c_baseloc varchar(255),
	n_max_attach integer
) TYPE = InnoDB;
insert into sy_baseinfo values (
	'dbb_blog',
	'manager@d-bb.com',
	0, 0, 1, 0, 0, 0,
	'#HOST#',
	'/usr/local/apache/hotbiz/skeleton/blog/asp',
	'/usr/local/apache/hotbiz/skeleton/blog/admin',
	'/usr/local/apache/hotbiz/skeleton/blog/user',
	'#USERDIR#/public_html/rss',
	'#USERDIR#/public_html/css',
	'#USERDIR#',
	'/~#USER#/blog',
	'/~#USER#/blog',
	'/~#USER#/blog',
	'/hotbiz/images/blog/admin',
	'/hotbiz/images/blog/user',
	'/hotbiz/images/blog/theme',
	'/~#USER#/css',
	'/~#USER#/rss',
	'/hotbiz/jscripts',
	'/~#USER#',
	5
);

drop table if exists sy_blogstatus;
create table sy_blogstatus(
	n_status_id integer not null,				-- ����ID
	c_status_name varchar(30),					-- ����̾
	constraint blogstatus_pkey primary key (n_status_id)
) TYPE = InnoDB;
insert into sy_blogstatus values (1,'��Ư��');
insert into sy_blogstatus values (2,'�����');
insert into sy_blogstatus values (3,'�ѻ�');

drop table if exists sy_cartinfo;
create table sy_cartinfo(
	c_host varchar(255),						-- ������DB������ۥ���
	c_database varchar(255),					-- ������DB�Υǡ����١���̾
	c_passfile varchar(255),					-- ������DB�ؤΥ�����ѥ���ɤ��񤫤�Ƥ���ե�����
	c_imageloc varchar(255),					-- ���ʲ�����ɽ�����뤿��Υ١������������
	c_purchase_cgi varchar(255)					-- ����������뤿���CGI
) TYPE = InnoDB;
insert into sy_cartinfo values (
	'localhost',
	'pib_#USERID#',
	'#USERDIR#/data/e-commex/pass.dat',
	'/e-commex/images',
	'/e-commex/cgi-bin/ex_purchase_item.cgi'
);

drop table if exists sy_category;
create table sy_category(
	n_category_id integer not null,				-- ���ƥ��꡼ID
	n_category_order integer,					-- ���ƥ��꡼��������
	c_category_name varchar(30),				-- ���ƥ��꡼̾
	constraint category_pkey primary key (n_category_id)
) TYPE = InnoDB;
insert into sy_category values (0,0,'���ƥ��꡼�ʤ�');

drop table if exists sy_filter;
create table sy_filter(
	n_filter_id integer not null,
	c_filter_name varchar(30),
	b_need_text bool,
	constraint filter_pkey primary key (n_filter_id)
) TYPE = InnoDB;
insert into sy_filter values (1,'URL�ե��륿',1);
insert into sy_filter values (2,'�֥�̾�ե��륿',1);
insert into sy_filter values (3,'����ƥ�ĥե��륿',1);
insert into sy_filter values (4,'���ո������ե��륿',0);

drop table if exists sy_iteminfo;
create table sy_iteminfo(
	n_iteminfo_id integer not null,				-- ���ʾ���ɽ����ˡID
	c_iteminfo_name varchar(32),				-- ���ʾ���ɽ����ˡ
	constraint ping_pkey primary key (n_iteminfo_id)
) TYPE = InnoDB;
insert into sy_iteminfo values (1, '���侦��');
insert into sy_iteminfo values (2, '���徦��');
insert into sy_iteminfo values (3, '�͵�����');

drop table if exists sy_ping;
create table sy_ping(
	n_ping_id integer not null,					-- �ԥ�������ID
	c_ping_site varchar(200),					-- �ԥ������襵����
	c_ping_url varchar(200),					-- �ԥ�����URL
	b_default bool,								-- ���������
	constraint ping_pkey primary key (n_ping_id)
) TYPE = InnoDB;
insert into sy_ping values(1, 'ping.bloggers.jp', 'http://ping.bloggers.jp/rpc/', 0);
insert into sy_ping values(2, 'ping.cocolog-nifty.com', 'http://ping.cocolog-nifty.com/xmlrpc', 0);
insert into sy_ping values(3, 'myblog.jp', 'http://ping.myblog.jp/', 0);
insert into sy_ping values(4, 'weblogs.com', 'http://rpc.weblogs.com/RPC2', 0);
insert into sy_ping values(5, 'ping.blo.gs', 'http://ping.blo.gs/', 0);

drop table if exists sy_remind;
create table sy_remind (
	n_remind integer not null,					-- �ֹ�
	c_remind varchar(50),						-- ����
	primary key (n_remind)
) TYPE = InnoDB;
insert into sy_remind values (1,'�ڥåȤ�̾���ϡ�');
insert into sy_remind values (2,'�����ʷ�ǽ�ͤϡ�');
insert into sy_remind values (3,'��Ƥε����ϡ�');
insert into sy_remind values (4,'��̣�ϡ�');

drop table if exists sy_rss;
create table sy_rss(
	n_rss_id integer not null,					-- �Ѱդ��줿RSS������ID
	c_rss_title varchar(50),					-- RSS�����ȥ�
	c_rss_url varchar(200),						-- RSSURL
	constraint rss_pkey primary key(n_rss_id)
) TYPE = InnoDB;
insert into sy_rss values(1, '�����ҥ���', 'http://www3.asahi.com/rss/index.rdf');
insert into sy_rss values(2, '����BP', 'http://nikkeibp.jp/jp/flash/index.rdf');
insert into sy_rss values(3, 'mycom', 'http://journal.mycom.co.jp/haishin/rss/index.rdf');
-- insert into sy_rss values(4, '���俷ʹ���Ҳ�', 'http://www.yomiusa.com/rdf/yomisyakai.xml');
-- insert into sy_rss values(5, '���п�ʹ���Ҳ�', 'http://www.yomiusa.com/rdf/sansyakai.xml');
insert into sy_rss values(4, 'CNET Japan', 'http://japan.cnet.com/rss/index.rdf');
insert into sy_rss values(5, 'FNN Headline News', 'http://rss.fujitv.co.jp/fnnnews.xml');

drop table if exists sy_sidecontent;
create table sy_sidecontent(
	n_sidecontent_id integer not null,			-- �����ɥ���ƥ��ID
	n_sidecontent_order integer,				-- �����ɥ���ƥ�Ľ���
	n_sidecontent_type integer not null,		-- �����ɥ���ƥ�ļ���
	c_sidecontent_title varchar(200),			-- �����ɥ���ƥ�ĥ����ȥ�
	b_display bool,								-- ɽ������
	b_allow_outer bool,							-- �֥��˥�����ɬ�פʤȤ���ɽ�����뤫
	constraint sidecontent_pkey primary key (n_sidecontent_id)
) TYPE = InnoDB;
insert into sy_sidecontent values(0, 0, 0, '�����ԥ�˥塼', 1, 0);
insert into sy_sidecontent values(1, 1, 1, '��������', 1, 1);
insert into sy_sidecontent values(2, 2, 2, '���̥���������', 1, 1);
insert into sy_sidecontent values(3, 3, 3, '���ƥ��꡼�̥���������', 1, 1);
insert into sy_sidecontent values(4, 4, 4, '�ǿ�������', 1, 1);
insert into sy_sidecontent values(5, 5, 5, '�ǿ��ȥ�å��Хå�', 1, 1);
insert into sy_sidecontent values(6, 6, 6, '�ץ�ե�����', 1, 1);
insert into sy_sidecontent values(7, 7, 7, 'RSS�ۿ�', 1, 1);
insert into sy_sidecontent values(8, 8, 8, '�֥��ˤĤ���', 1, 1);
insert into sy_sidecontent values(9, 9, 9, '�ǿ�����', 1, 1);
insert into sy_sidecontent values(12, 10, 12, '�֥��⸡��', 1, 1);
insert into sy_sidecontent values(13, 11, 13, 'ˬ��ԥꥹ��', 1, 0);
insert into sy_sidecontent values(14, 12, 14, '����������', 1, 0);

drop table if exists sy_theme_category;
create table sy_theme_category(
	n_category_id integer not null,				-- ���ƥ���ID
	c_category_title varchar(32),				-- ���ƥ��꥿���ȥ�
	c_theme_thumbnail varchar(255)				-- ���ƥ��ꥵ��ͥ���
) TYPE = InnoDB;
insert into sy_theme_category values( 1, '���ʡ�����', 'icon_cate_food.jpg');
insert into sy_theme_category values( 2, '�������ġ��²ۻ�', 'icon_cate_sweets.jpg');
insert into sy_theme_category values( 3, '�ե��å����', 'icon_cate_fashion.jpg');
insert into sy_theme_category values( 4, '�٥ӡ�����', 'icon_cate_baby.jpg');
insert into sy_theme_category values( 5, '���š�ʡ��', 'icon_cate_medical.jpg');
insert into sy_theme_category values( 6, '���ơ���', 'icon_cate_beauty.jpg');
insert into sy_theme_category values( 7, '����ƥꥢ������', 'icon_cate_interior.jpg');
insert into sy_theme_category values( 8, '���ե�������', 'icon_cate_office.jpg');
insert into sy_theme_category values( 9, '���š��ŵ�', 'icon_cate_audio.jpg');
insert into sy_theme_category values(10, '��̣�����', 'icon_cate_hobby.jpg');
insert into sy_theme_category values(11, '������㡦������', 'icon_cate_game.jpg');
insert into sy_theme_category values(12, '���ݡ�������', 'icon_cate_sports.jpg');
insert into sy_theme_category values(13, '���ڡ���������', 'icon_cate_media.jpg');
insert into sy_theme_category values(14, '�ڥå�����', 'icon_cate_animal.jpg');
insert into sy_theme_category values(15, '�֡����ե�', 'icon_cate_season.jpg');
insert into sy_theme_category values(16, '�ȡ��ޥ󥷥��', 'icon_cate_house.jpg');

drop table if exists sy_theme;
create table sy_theme(
	n_theme_id integer not null,				-- �ơ���ID
	n_category_id integer not null,				-- ���ƥ���ID
	c_theme_title varchar(32),					-- �ơ��ޥ����ȥ�
	c_theme_file varchar(255),					-- �ơ���CSS
	c_theme_thumbnail varchar(255),				-- �ơ��ޥ���ͥ���
	c_theme_prev_entry_icon varchar(255),		-- ���ε����ؤΥ�������
	c_theme_next_entry_icon varchar(255),		-- ���ε����ؤΥ�������
	c_theme_prev_page_icon varchar(255),		-- ���Υڡ����ؤΥ�������
	c_theme_next_page_icon varchar(255),		-- ���Υڡ����ؤΥ�������
	c_theme_first_icon varchar(255),			-- �ǽ�ؤΥ�������
	c_theme_last_icon varchar(255),				-- �Ǹ�ؤΥ�������
	c_theme_plus_icon varchar(255),				-- ���ѤΤʤ��˥ץ饹(+)
	c_theme_minus_icon varchar(255),			-- ���ѤΤʤ��˥ޥ��ʥ�(-)
	c_theme_link_btn_icon varchar(255),			-- ��󥯥ܥ���Υ�������
	c_theme_blog_top_icon varchar(255),			-- �֥��ȥåפؤΥ�������
	c_theme_page_top_icon varchar(255),			-- �ڡ����ȥåפؤΥ�������
	constraint theme_pkey primary key(n_category_id, n_theme_id)
) TYPE = InnoDB;
-- CART�⡼����
insert into sy_theme values(1, 1, '���ʡ����� No.1', 'blog31.css', 'food01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_31.gif', 'minus_31.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 1, '���ʡ����� No.2', 'blog32.css', 'food02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_32.gif', 'minus_32.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 1, '���ʡ����� No.3', 'blog33.css', 'food03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_33.gif', 'minus_33.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 1, '���ʡ����� No.4', 'blog34.css', 'food04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_34.gif', 'minus_34.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 1, '���ʡ����� No.5', 'blog35.css', 'food05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_35.gif', 'minus_35.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 2, '�������ġ��²ۻ� No.1', 'blog76.css', 'sweets01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_76.gif', 'minus_76.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 2, '�������ġ��²ۻ� No.2', 'blog77.css', 'sweets02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_77.gif', 'minus_77.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 2, '�������ġ��²ۻ� No.3', 'blog78.css', 'sweets03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_78.gif', 'minus_78.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 2, '�������ġ��²ۻ� No.4', 'blog79.css', 'sweets04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_79.gif', 'minus_79.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 2, '�������ġ��²ۻ� No.5', 'blog80.css', 'sweets05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_80.gif', 'minus_80.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 3, '�ե��å���� No.1', 'blog21.css', 'fashion01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_21.gif', 'minus_21.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 3, '�ե��å���� No.2', 'blog22.css', 'fashion02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_22.gif', 'minus_22.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 3, '�ե��å���� No.3', 'blog23.css', 'fashion03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_23.gif', 'minus_23.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 3, '�ե��å���� No.4', 'blog24.css', 'fashion04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_24.gif', 'minus_24.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 3, '�ե��å���� No.5', 'blog25.css', 'fashion05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_25.gif', 'minus_25.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 4, '�٥ӡ����� No.1', 'blog01.css', 'baby01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01.gif', 'minus_01.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 4, '�٥ӡ����� No.2', 'blog02.css', 'baby02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02.gif', 'minus_02.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 4, '�٥ӡ����� No.3', 'blog03.css', 'baby03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03.gif', 'minus_03.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 4, '�٥ӡ����� No.4', 'blog04.css', 'baby04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04.gif', 'minus_04.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 4, '�٥ӡ����� No.5', 'blog05.css', 'baby05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05.gif', 'minus_05.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 5, '���š�ʡ�� No.1', 'blog56.css', 'medical01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_56.gif', 'minus_56.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 5, '���š�ʡ�� No.2', 'blog57.css', 'medical02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_57.gif', 'minus_57.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 5, '���š�ʡ�� No.3', 'blog58.css', 'medical03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_58.gif', 'minus_58.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 5, '���š�ʡ�� No.4', 'blog59.css', 'medical04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_59.gif', 'minus_59.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 5, '���š�ʡ�� No.5', 'blog60.css', 'medical05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_60.gif', 'minus_60.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 6, '���ơ��� No.1', 'blog06.css', 'beauty01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06.gif', 'minus_06.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 6, '���ơ��� No.2', 'blog07.css', 'beauty02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_07.gif', 'minus_07.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 6, '���ơ��� No.3', 'blog08.css', 'beauty03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_08.gif', 'minus_08.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 6, '���ơ��� No.4', 'blog09.css', 'beauty04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_09.gif', 'minus_09.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 6, '���ơ��� No.5', 'blog10.css', 'beauty05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_10.gif', 'minus_10.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 7, '����ƥꥢ������ No.1', 'blog51.css', 'interior01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_51.gif', 'minus_51.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 7, '����ƥꥢ������ No.2', 'blog52.css', 'interior02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_52.gif', 'minus_52.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 7, '����ƥꥢ������ No.3', 'blog53.css', 'interior03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_53.gif', 'minus_53.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 7, '����ƥꥢ������ No.4', 'blog54.css', 'interior04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_54.gif', 'minus_54.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 7, '����ƥꥢ������ No.5', 'blog55.css', 'interior05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_55.gif', 'minus_55.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 8, '���ե������� No.1', 'blog61.css', 'office01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_61.gif', 'minus_61.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 8, '���ե������� No.2', 'blog62.css', 'office02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_62.gif', 'minus_62.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 8, '���ե������� No.3', 'blog63.css', 'office03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_63.gif', 'minus_63.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 8, '���ե������� No.4', 'blog64.css', 'office04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_64.gif', 'minus_64.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 8, '���ե������� No.5', 'blog65.css', 'office05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_65.gif', 'minus_65.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 9, '���š��ŵ� No.1', 'blog16.css', 'electro01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_16.gif', 'minus_16.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 9, '���š��ŵ� No.2', 'blog17.css', 'electro02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_17.gif', 'minus_17.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 9, '���š��ŵ� No.3', 'blog18.css', 'electro03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_18.gif', 'minus_18.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 9, '���š��ŵ� No.4', 'blog19.css', 'electro04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_19.gif', 'minus_19.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 9, '���š��ŵ� No.5', 'blog20.css', 'electro05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_20.gif', 'minus_20.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 10, '��̣����� No.1', 'blog41.css', 'hobby01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_41.gif', 'minus_41.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 10, '��̣����� No.2', 'blog42.css', 'hobby02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_42.gif', 'minus_42.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 10, '��̣����� No.3', 'blog43.css', 'hobby03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_43.gif', 'minus_43.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 10, '��̣����� No.4', 'blog44.css', 'hobby04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_44.gif', 'minus_44.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 10, '��̣����� No.5', 'blog45.css', 'hobby05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_45.gif', 'minus_45.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 11, '������㡦������ No.1', 'blog36.css', 'games01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_36.gif', 'minus_36.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 11, '������㡦������ No.2', 'blog37.css', 'games02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_37.gif', 'minus_37.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 11, '������㡦������ No.3', 'blog38.css', 'games03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_38.gif', 'minus_38.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 11, '������㡦������ No.4', 'blog39.css', 'games04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_39.gif', 'minus_39.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 11, '������㡦������ No.5', 'blog40.css', 'games05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_40.gif', 'minus_40.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 12, '���ݡ������� No.1', 'blog71.css', 'sports01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_71.gif', 'minus_71.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 12, '���ݡ������� No.2', 'blog72.css', 'sports02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_72.gif', 'minus_72.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 12, '���ݡ������� No.3', 'blog73.css', 'sports03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_73.gif', 'minus_73.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 12, '���ݡ������� No.4', 'blog74.css', 'sports04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_74.gif', 'minus_74.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 12, '���ݡ������� No.5', 'blog75.css', 'sports05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_75.gif', 'minus_75.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 13, '���ڡ��������� No.1', 'blog11.css', 'books01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_11.gif', 'minus_11.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 13, '���ڡ��������� No.2', 'blog12.css', 'books02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_12.gif', 'minus_12.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 13, '���ڡ��������� No.3', 'blog13.css', 'books03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_13.gif', 'minus_13.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 13, '���ڡ��������� No.4', 'blog14.css', 'books04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_14.gif', 'minus_14.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 13, '���ڡ��������� No.5', 'blog15.css', 'books05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_15.gif', 'minus_15.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 14, '�ڥå����� No.1', 'blog66.css', 'pet01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_66.gif', 'minus_66.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 14, '�ڥå����� No.2', 'blog67.css', 'pet02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_67.gif', 'minus_67.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 14, '�ڥå����� No.3', 'blog68.css', 'pet03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_68.gif', 'minus_68.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 14, '�ڥå����� No.4', 'blog69.css', 'pet04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_69.gif', 'minus_69.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 14, '�ڥå����� No.5', 'blog70.css', 'pet05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_70.gif', 'minus_70.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 15, '�֡����ե� No.1', 'blog26.css', 'flower01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_26.gif', 'minus_26.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 15, '�֡����ե� No.2', 'blog27.css', 'flower02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_27.gif', 'minus_27.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 15, '�֡����ե� No.3', 'blog28.css', 'flower03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_28.gif', 'minus_28.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 15, '�֡����ե� No.4', 'blog29.css', 'flower04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_29.gif', 'minus_29.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 15, '�֡����ե� No.5', 'blog30.css', 'flower05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_30.gif', 'minus_30.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 16, '�ȡ��ޥ󥷥�� No.1', 'blog46.css', 'housing01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_46.gif', 'minus_46.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 16, '�ȡ��ޥ󥷥�� No.2', 'blog47.css', 'housing02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_47.gif', 'minus_47.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 16, '�ȡ��ޥ󥷥�� No.3', 'blog48.css', 'housing03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_48.gif', 'minus_48.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 16, '�ȡ��ޥ󥷥�� No.4', 'blog49.css', 'housing04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_49.gif', 'minus_49.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 16, '�ȡ��ޥ󥷥�� No.5', 'blog50.css', 'housing05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_50.gif', 'minus_50.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
