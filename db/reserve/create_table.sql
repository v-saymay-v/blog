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
	c_item_kind varchar(64),					-- DBB�⡼�ɻ��ƴ�Ϣ���ܤμ���
	c_item_id varchar(64),						-- �����ȥ⡼�ɻ��ξ���ID��DBB�⡼�ɻ��ƴ�Ϣ����ID
	c_item_url varchar(255),					-- DBB�⡼�ɻ��ƴ�Ϣ���ܤؤ�URL
	c_item_owner integer,						-- DBB�⡼�ɻ��ƴ�Ϣ���ܤΥ����ʡ�
	d_entry_create_time datetime,				-- ��Ի���κ�������
	d_entry_real_time datetime,					-- �ºݤκ�������
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

drop table if exists at_uploadmovie;
create table at_uploadmovie(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- ź���赭��
	c_filename varchar(255) not null,			-- ���åץ��ɥե�����̾
	c_origname varchar(255) not null,			-- ���ꥸ�ʥ�ե�����̾
	b_delete_mode bool not null,				-- 1 ���ͽ�� 0 ����ʳ�
	constraint uploadfile_pkey primary key (n_blog_id,n_entry_id)
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
	c_link_url varchar(255) not null,			-- ��󥯸�URL
	n_from_blog integer,				-- ��󥯸��֥�
	n_from_entry integer,				-- ��󥯸�����
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

drop table if exists at_parts;
create table at_parts(
	n_blog_id integer not null,
	n_parts_id integer not null,				-- �֥��ѡ���ID
	c_parts_title varchar(100),					-- �֥��ѡ��ĥ����ȥ�
	c_parts_body text,							-- �֥��ѡ��ĥ���
	b_parts_title bool,							-- 0 �����ȥ�ɽ������ 1 �����ȥ�ɽ������
	constraint rss_pkey primary key (n_blog_id,n_parts_id)
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
	'192.168.100.140',
	'rsv_#DBUSER#',
	'#DBFILE#',
	'#DBFILE#',
	'#USERDIR#/data/reserve/passwd.dat',
	'sy_login',
	'',
	'c_login',
	'c_pass',
	'c_access_key',
	'\'1\'',
	'n_admin',
	'c_name',
	'',
	'',
	'',
	'key',
	'',
	'',
	'',
	'',
	'',
	'',
	'',
	'',
	'',
	'',
	'',
	'/reserve/cgi-bin/admin/rsv_index.cgi');

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
	c_baseloc varchar(255)
) TYPE = InnoDB;
insert into sy_baseinfo values (
	'eReserve_blog',
	'manager@d-bb.com',
	0, 2, 0, 0, 0, 0,
	'www.#DOMAIN#',
	'/usr/local/apache/share/as-blog/skeleton',
	'/usr/local/apache/share/as-blog/skeleton',
	'/usr/local/apache/share/as-blog/skeleton',
	'#USERDIR#/data/as-blog/rss',
	'#USERDIR#/data/as-blog/css',
	'#USERDIR#/data/as-blog/upload',
	'/reserve/cgi-bin/admin',
	'/reserve/cgi-bin/admin',
	'/reserve/cgi-bin',
	'/as-blog/images/admin',
	'/as-blog/images/user',
	'/as-blog/images/theme',
	'/as-blog/css',
	'/as-blog/rss',
	'/as-blog/scripts',
	'/as-blog'
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
	'192.168.100.140',
	'rsv_#DBUSER#',
	'#USERDIR#/data/reserve/passwd.dat',
	'/reserve/images',
	'/reserve/cgi-bin/rsv_reserve.cgi'
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
insert into sy_filter values (3,'���ץե��륿',1);
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
insert into sy_ping values(2, 'drecom.jp', 'http://ping.rss.drecom.jp/', 0);
insert into sy_ping values(3, 'weblogs.com', 'http://rpc.weblogs.com/RPC2', 0);
insert into sy_ping values(4, 'ping.blo.gs', 'http://ping.blo.gs/', 0);

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
insert into sy_rss values(2, '����BP', 'http://pheedo.nikkeibp.co.jp/f/nikkeibp_news_flash');
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
insert into sy_sidecontent values(7, 7, 7, 'RSS�ۿ�', 1, 0);
insert into sy_sidecontent values(8, 8, 8, '�֥��ˤĤ���', 1, 1);
insert into sy_sidecontent values(9, 9, 9, '�ǿ�����', 1, 1);
insert into sy_sidecontent values(12, 10, 12, '�֥��⸡��', 1, 1);
insert into sy_sidecontent values(14, 11, 14, '����������', 1, 0);

drop table if exists sy_theme_category;
create table sy_theme_category(
	n_category_id integer not null,				-- ���ƥ���ID
	c_category_title varchar(32),				-- ���ƥ��꥿���ȥ�
	c_theme_thumbnail varchar(255)				-- ���ƥ��ꥵ��ͥ���
) TYPE = InnoDB;
insert into sy_theme_category values( 1, '�ۥƥ롦ι��', 'icon_hotel.jpg');
insert into sy_theme_category values( 2, '���Ʊ���������', 'icon_beauty.jpg');
insert into sy_theme_category values( 3, '�±�������˥å�', 'icon_clinic.jpg');
insert into sy_theme_category values( 4, '����Ź', 'icon_restaurant.jpg');
insert into sy_theme_category values( 5, '�ֺ¡���������', 'icon_school.jpg');
insert into sy_theme_category values( 6, '���ߡ�����', 'icon_facilities.jpg');
insert into sy_theme_category values( 7, '��󥿥�', 'icon_rental.jpg');

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
insert into sy_theme values(1, 1, '�ۥƥ롦ι�� No.1', 'blog04_01.css', 'hotel01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_01.gif', 'minus_04_01.gif', 'link_01.gif', 'back_blogtop_04_01.gif', 'back_top_04_01.gif');
insert into sy_theme values(2, 1, '�ۥƥ롦ι�� No.2', 'blog04_02.css', 'hotel02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_02.gif', 'minus_04_02.gif', 'link_01.gif', 'back_blogtop_04_02.gif', 'back_top_04_02.gif');
insert into sy_theme values(3, 1, '�ۥƥ롦ι�� No.3', 'blog04_03.css', 'hotel03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_03.gif', 'minus_04_03.gif', 'link_01.gif', 'back_blogtop_04_03.gif', 'back_top_04_03.gif');
insert into sy_theme values(4, 1, '�ۥƥ롦ι�� No.4', 'blog04_04.css', 'hotel04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_04.gif', 'minus_04_04.gif', 'link_01.gif', 'back_blogtop_04_04.gif', 'back_top_04_04.gif');
insert into sy_theme values(5, 1, '�ۥƥ롦ι�� No.5', 'blog04_05.css', 'hotel05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_05.gif', 'minus_04_05.gif', 'link_01.gif', 'back_blogtop_04_05.gif', 'back_top_04_05.gif');
insert into sy_theme values(6, 1, '�ۥƥ롦ι�� No.6', 'blog04_06.css', 'hotel06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_06.gif', 'minus_04_06.gif', 'link_01.gif', 'back_blogtop_04_06.gif', 'back_top_04_06.gif');
insert into sy_theme values(7, 1, '�ۥƥ롦ι�� No.7', 'blog04_07.css', 'hotel07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_07.gif', 'minus_04_07.gif', 'link_01.gif', 'back_blogtop_04_07.gif', 'back_top_04_07.gif');
insert into sy_theme values(8, 1, '�ۥƥ롦ι�� No.8', 'blog04_08.css', 'hotel08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_08.gif', 'minus_04_08.gif', 'link_01.gif', 'back_blogtop_04_08.gif', 'back_top_04_08.gif');
insert into sy_theme values(9, 1, '�ۥƥ롦ι�� No.9', 'blog04_09.css', 'hotel09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_09.gif', 'minus_04_09.gif', 'link_01.gif', 'back_blogtop_04_09.gif', 'back_top_04_09.gif');
insert into sy_theme values(10, 1, '�ۥƥ롦ι�� No.10', 'blog04_10.css', 'hotel10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_10.gif', 'minus_04_10.gif', 'link_01.gif', 'back_blogtop_04_10.gif', 'back_top_04_10.gif');
insert into sy_theme values(11, 1, '�ۥƥ롦ι�� No.11', 'blog04_11.css', 'hotel11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_11.gif', 'minus_04_11.gif', 'link_01.gif', 'back_blogtop_04_11.gif', 'back_top_04_11.gif');
insert into sy_theme values(12, 1, '�ۥƥ롦ι�� No.12', 'blog04_12.css', 'hotel12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_12.gif', 'minus_04_12.gif', 'link_01.gif', 'back_blogtop_04_12.gif', 'back_top_04_12.gif');
insert into sy_theme values(13, 1, '�ۥƥ롦ι�� No.13', 'blog04_13.css', 'hotel13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_13.gif', 'minus_04_13.gif', 'link_01.gif', 'back_blogtop_04_13.gif', 'back_top_04_13.gif');
insert into sy_theme values(14, 1, '�ۥƥ롦ι�� No.14', 'blog04_14.css', 'hotel14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_14.gif', 'minus_04_14.gif', 'link_01.gif', 'back_blogtop_04_14.gif', 'back_top_04_14.gif');
insert into sy_theme values(15, 1, '�ۥƥ롦ι�� No.15', 'blog04_15.css', 'hotel15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_15.gif', 'minus_04_15.gif', 'link_01.gif', 'back_blogtop_04_15.gif', 'back_top_04_15.gif');
insert into sy_theme values(16, 1, '�ۥƥ롦ι�� No.16', 'blog04_16.css', 'hotel16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_16.gif', 'minus_04_16.gif', 'link_01.gif', 'back_blogtop_04_16.gif', 'back_top_04_16.gif');
insert into sy_theme values(17, 1, '�ۥƥ롦ι�� No.17', 'blog04_17.css', 'hotel17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_17.gif', 'minus_04_17.gif', 'link_01.gif', 'back_blogtop_04_17.gif', 'back_top_04_17.gif');
insert into sy_theme values(18, 1, '�ۥƥ롦ι�� No.18', 'blog04_18.css', 'hotel18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_18.gif', 'minus_04_18.gif', 'link_01.gif', 'back_blogtop_04_18.gif', 'back_top_04_18.gif');
insert into sy_theme values(19, 1, '�ۥƥ롦ι�� No.19', 'blog04_19.css', 'hotel19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_19.gif', 'minus_04_19.gif', 'link_01.gif', 'back_blogtop_04_19.gif', 'back_top_04_19.gif');
insert into sy_theme values(20, 1, '�ۥƥ롦ι�� No.20', 'blog04_20.css', 'hotel20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_20.gif', 'minus_04_20.gif', 'link_01.gif', 'back_blogtop_04_20.gif', 'back_top_04_20.gif');
insert into sy_theme values(1, 2, '���Ʊ��������� No.1', 'blog01_01.css', 'beauty01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_01.gif', 'minus_01_01.gif', 'link_01.gif', 'back_blogtop_01_01.gif', 'back_top_01_01.gif');
insert into sy_theme values(2, 2, '���Ʊ��������� No.2', 'blog01_02.css', 'beauty02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_02.gif', 'minus_01_02.gif', 'link_01.gif', 'back_blogtop_01_02.gif', 'back_top_01_02.gif');
insert into sy_theme values(3, 2, '���Ʊ��������� No.3', 'blog01_03.css', 'beauty03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_03.gif', 'minus_01_03.gif', 'link_01.gif', 'back_blogtop_01_03.gif', 'back_top_01_03.gif');
insert into sy_theme values(4, 2, '���Ʊ��������� No.4', 'blog01_04.css', 'beauty04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_04.gif', 'minus_01_04.gif', 'link_01.gif', 'back_blogtop_01_04.gif', 'back_top_01_04.gif');
insert into sy_theme values(5, 2, '���Ʊ��������� No.5', 'blog01_05.css', 'beauty05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_05.gif', 'minus_01_05.gif', 'link_01.gif', 'back_blogtop_01_05.gif', 'back_top_01_05.gif');
insert into sy_theme values(6, 2, '���Ʊ��������� No.6', 'blog01_06.css', 'beauty06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_06.gif', 'minus_01_06.gif', 'link_01.gif', 'back_blogtop_01_06.gif', 'back_top_01_06.gif');
insert into sy_theme values(7, 2, '���Ʊ��������� No.7', 'blog01_07.css', 'beauty07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_07.gif', 'minus_01_07.gif', 'link_01.gif', 'back_blogtop_01_07.gif', 'back_top_01_07.gif');
insert into sy_theme values(8, 2, '���Ʊ��������� No.8', 'blog01_08.css', 'beauty08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_08.gif', 'minus_01_08.gif', 'link_01.gif', 'back_blogtop_01_08.gif', 'back_top_01_08.gif');
insert into sy_theme values(9, 2, '���Ʊ��������� No.9', 'blog01_09.css', 'beauty09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_09.gif', 'minus_01_09.gif', 'link_01.gif', 'back_blogtop_01_09.gif', 'back_top_01_09.gif');
insert into sy_theme values(10, 2, '���Ʊ��������� No.10', 'blog01_10.css', 'beauty10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_10.gif', 'minus_01_10.gif', 'link_01.gif', 'back_blogtop_01_10.gif', 'back_top_01_10.gif');
insert into sy_theme values(11, 2, '���Ʊ��������� No.11', 'blog01_11.css', 'beauty11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_11.gif', 'minus_01_11.gif', 'link_01.gif', 'back_blogtop_01_11.gif', 'back_top_01_11.gif');
insert into sy_theme values(12, 2, '���Ʊ��������� No.12', 'blog01_12.css', 'beauty12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_12.gif', 'minus_01_12.gif', 'link_01.gif', 'back_blogtop_01_12.gif', 'back_top_01_12.gif');
insert into sy_theme values(13, 2, '���Ʊ��������� No.13', 'blog01_13.css', 'beauty13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_13.gif', 'minus_01_13.gif', 'link_01.gif', 'back_blogtop_01_13.gif', 'back_top_01_13.gif');
insert into sy_theme values(14, 2, '���Ʊ��������� No.14', 'blog01_14.css', 'beauty14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_14.gif', 'minus_01_14.gif', 'link_01.gif', 'back_blogtop_01_14.gif', 'back_top_01_14.gif');
insert into sy_theme values(15, 2, '���Ʊ��������� No.15', 'blog01_15.css', 'beauty15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_15.gif', 'minus_01_15.gif', 'link_01.gif', 'back_blogtop_01_15.gif', 'back_top_01_15.gif');
insert into sy_theme values(16, 2, '���Ʊ��������� No.16', 'blog01_16.css', 'beauty16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_16.gif', 'minus_01_16.gif', 'link_01.gif', 'back_blogtop_01_16.gif', 'back_top_01_16.gif');
insert into sy_theme values(17, 2, '���Ʊ��������� No.17', 'blog01_17.css', 'beauty17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_17.gif', 'minus_01_17.gif', 'link_01.gif', 'back_blogtop_01_17.gif', 'back_top_01_17.gif');
insert into sy_theme values(18, 2, '���Ʊ��������� No.18', 'blog01_18.css', 'beauty18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_18.gif', 'minus_01_18.gif', 'link_01.gif', 'back_blogtop_01_18.gif', 'back_top_01_18.gif');
insert into sy_theme values(19, 2, '���Ʊ��������� No.19', 'blog01_19.css', 'beauty19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_19.gif', 'minus_01_19.gif', 'link_01.gif', 'back_blogtop_01_19.gif', 'back_top_01_19.gif');
insert into sy_theme values(20, 2, '���Ʊ��������� No.20', 'blog01_20.css', 'beauty20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_20.gif', 'minus_01_20.gif', 'link_01.gif', 'back_blogtop_01_20.gif', 'back_top_01_20.gif');
insert into sy_theme values(1, 3, '�±�������˥å� No.1', 'blog02_01.css', 'clinic01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_01.gif', 'minus_02_01.gif', 'link_01.gif', 'back_blogtop_02_01.gif', 'back_top_02_01.gif');
insert into sy_theme values(2, 3, '�±�������˥å� No.2', 'blog02_02.css', 'clinic02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_02.gif', 'minus_02_02.gif', 'link_01.gif', 'back_blogtop_02_02.gif', 'back_top_02_02.gif');
insert into sy_theme values(3, 3, '�±�������˥å� No.3', 'blog02_03.css', 'clinic03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_03.gif', 'minus_02_03.gif', 'link_01.gif', 'back_blogtop_02_03.gif', 'back_top_02_03.gif');
insert into sy_theme values(4, 3, '�±�������˥å� No.4', 'blog02_04.css', 'clinic04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_04.gif', 'minus_02_04.gif', 'link_01.gif', 'back_blogtop_02_04.gif', 'back_top_02_04.gif');
insert into sy_theme values(5, 3, '�±�������˥å� No.5', 'blog02_05.css', 'clinic05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_05.gif', 'minus_02_05.gif', 'link_01.gif', 'back_blogtop_02_05.gif', 'back_top_02_05.gif');
insert into sy_theme values(6, 3, '�±�������˥å� No.6', 'blog02_06.css', 'clinic06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_06.gif', 'minus_02_06.gif', 'link_01.gif', 'back_blogtop_02_06.gif', 'back_top_02_06.gif');
insert into sy_theme values(7, 3, '�±�������˥å� No.7', 'blog02_07.css', 'clinic07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_07.gif', 'minus_02_07.gif', 'link_01.gif', 'back_blogtop_02_07.gif', 'back_top_02_07.gif');
insert into sy_theme values(8, 3, '�±�������˥å� No.8', 'blog02_08.css', 'clinic08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_08.gif', 'minus_02_08.gif', 'link_01.gif', 'back_blogtop_02_08.gif', 'back_top_02_08.gif');
insert into sy_theme values(9, 3, '�±�������˥å� No.9', 'blog02_09.css', 'clinic09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_09.gif', 'minus_02_09.gif', 'link_01.gif', 'back_blogtop_02_09.gif', 'back_top_02_09.gif');
insert into sy_theme values(10, 3, '�±�������˥å� No.10', 'blog02_10.css', 'clinic10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_10.gif', 'minus_02_10.gif', 'link_01.gif', 'back_blogtop_02_10.gif', 'back_top_02_10.gif');
insert into sy_theme values(11, 3, '�±�������˥å� No.11', 'blog02_11.css', 'clinic11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_11.gif', 'minus_02_11.gif', 'link_01.gif', 'back_blogtop_02_11.gif', 'back_top_02_11.gif');
insert into sy_theme values(12, 3, '�±�������˥å� No.12', 'blog02_12.css', 'clinic12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_12.gif', 'minus_02_12.gif', 'link_01.gif', 'back_blogtop_02_12.gif', 'back_top_02_12.gif');
insert into sy_theme values(13, 3, '�±�������˥å� No.13', 'blog02_13.css', 'clinic13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_13.gif', 'minus_02_13.gif', 'link_01.gif', 'back_blogtop_02_13.gif', 'back_top_02_13.gif');
insert into sy_theme values(14, 3, '�±�������˥å� No.14', 'blog02_14.css', 'clinic14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_14.gif', 'minus_02_14.gif', 'link_01.gif', 'back_blogtop_02_14.gif', 'back_top_02_14.gif');
insert into sy_theme values(15, 3, '�±�������˥å� No.15', 'blog02_15.css', 'clinic15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_15.gif', 'minus_02_15.gif', 'link_01.gif', 'back_blogtop_02_15.gif', 'back_top_02_15.gif');
insert into sy_theme values(16, 3, '�±�������˥å� No.16', 'blog02_16.css', 'clinic16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_16.gif', 'minus_02_16.gif', 'link_01.gif', 'back_blogtop_02_16.gif', 'back_top_02_16.gif');
insert into sy_theme values(17, 3, '�±�������˥å� No.17', 'blog02_17.css', 'clinic17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_17.gif', 'minus_02_17.gif', 'link_01.gif', 'back_blogtop_02_17.gif', 'back_top_02_17.gif');
insert into sy_theme values(18, 3, '�±�������˥å� No.18', 'blog02_18.css', 'clinic18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_18.gif', 'minus_02_18.gif', 'link_01.gif', 'back_blogtop_02_18.gif', 'back_top_02_18.gif');
insert into sy_theme values(19, 3, '�±�������˥å� No.19', 'blog02_19.css', 'clinic19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_19.gif', 'minus_02_19.gif', 'link_01.gif', 'back_blogtop_02_19.gif', 'back_top_02_19.gif');
insert into sy_theme values(20, 3, '�±�������˥å� No.20', 'blog02_20.css', 'clinic20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_20.gif', 'minus_02_20.gif', 'link_01.gif', 'back_blogtop_02_20.gif', 'back_top_02_20.gif');
insert into sy_theme values(1, 4, '����Ź No.1', 'blog05_01.css', 'restaurant01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_01.gif', 'minus_05_01.gif', 'link_01.gif', 'back_blogtop_05_01.gif', 'back_top_05_01.gif');
insert into sy_theme values(2, 4, '����Ź No.2', 'blog05_02.css', 'restaurant02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_02.gif', 'minus_05_02.gif', 'link_01.gif', 'back_blogtop_05_02.gif', 'back_top_05_02.gif');
insert into sy_theme values(3, 4, '����Ź No.3', 'blog05_03.css', 'restaurant03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_03.gif', 'minus_05_03.gif', 'link_01.gif', 'back_blogtop_05_03.gif', 'back_top_05_03.gif');
insert into sy_theme values(4, 4, '����Ź No.4', 'blog05_04.css', 'restaurant04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_04.gif', 'minus_05_04.gif', 'link_01.gif', 'back_blogtop_05_04.gif', 'back_top_05_04.gif');
insert into sy_theme values(5, 4, '����Ź No.5', 'blog05_05.css', 'restaurant05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_05.gif', 'minus_05_05.gif', 'link_01.gif', 'back_blogtop_05_05.gif', 'back_top_05_05.gif');
insert into sy_theme values(6, 4, '����Ź No.6', 'blog05_06.css', 'restaurant06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_06.gif', 'minus_05_06.gif', 'link_01.gif', 'back_blogtop_05_06.gif', 'back_top_05_06.gif');
insert into sy_theme values(7, 4, '����Ź No.7', 'blog05_07.css', 'restaurant07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_07.gif', 'minus_05_07.gif', 'link_01.gif', 'back_blogtop_05_07.gif', 'back_top_05_07.gif');
insert into sy_theme values(8, 4, '����Ź No.8', 'blog05_08.css', 'restaurant08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_08.gif', 'minus_05_08.gif', 'link_01.gif', 'back_blogtop_05_08.gif', 'back_top_05_08.gif');
insert into sy_theme values(9, 4, '����Ź No.9', 'blog05_09.css', 'restaurant09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_09.gif', 'minus_05_09.gif', 'link_01.gif', 'back_blogtop_05_09.gif', 'back_top_05_09.gif');
insert into sy_theme values(1, 5, '�ֺ¡��������� No.1', 'blog06_01.css', 'school01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_01.gif', 'minus_06_01.gif', 'link_01.gif', 'back_blogtop_06_01.gif', 'back_top_06_01.gif');
insert into sy_theme values(2, 5, '�ֺ¡��������� No.2', 'blog06_02.css', 'school02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_02.gif', 'minus_06_02.gif', 'link_01.gif', 'back_blogtop_06_02.gif', 'back_top_06_02.gif');
insert into sy_theme values(3, 5, '�ֺ¡��������� No.3', 'blog06_03.css', 'school03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_03.gif', 'minus_06_03.gif', 'link_01.gif', 'back_blogtop_06_03.gif', 'back_top_06_03.gif');
insert into sy_theme values(4, 5, '�ֺ¡��������� No.4', 'blog06_04.css', 'school04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_04.gif', 'minus_06_04.gif', 'link_01.gif', 'back_blogtop_06_04.gif', 'back_top_06_04.gif');
insert into sy_theme values(5, 5, '�ֺ¡��������� No.5', 'blog06_05.css', 'school05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_05.gif', 'minus_06_05.gif', 'link_01.gif', 'back_blogtop_06_05.gif', 'back_top_06_05.gif');
insert into sy_theme values(6, 5, '�ֺ¡��������� No.6', 'blog06_06.css', 'school06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_06.gif', 'minus_06_06.gif', 'link_01.gif', 'back_blogtop_06_06.gif', 'back_top_06_06.gif');
insert into sy_theme values(7, 5, '�ֺ¡��������� No.7', 'blog06_07.css', 'school07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_07.gif', 'minus_06_07.gif', 'link_01.gif', 'back_blogtop_06_07.gif', 'back_top_06_07.gif');
insert into sy_theme values(8, 5, '�ֺ¡��������� No.8', 'blog06_08.css', 'school08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_08.gif', 'minus_06_08.gif', 'link_01.gif', 'back_blogtop_06_08.gif', 'back_top_06_08.gif');
insert into sy_theme values(9, 5, '�ֺ¡��������� No.9', 'blog06_09.css', 'school09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_09.gif', 'minus_06_09.gif', 'link_01.gif', 'back_blogtop_06_09.gif', 'back_top_06_09.gif');
insert into sy_theme values(10, 5, '�ֺ¡��������� No.10', 'blog06_10.css', 'school10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_10.gif', 'minus_06_10.gif', 'link_01.gif', 'back_blogtop_06_10.gif', 'back_top_06_10.gif');
insert into sy_theme values(11, 5, '�ֺ¡��������� No.11', 'blog06_11.css', 'school11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_11.gif', 'minus_06_11.gif', 'link_01.gif', 'back_blogtop_06_11.gif', 'back_top_06_11.gif');
insert into sy_theme values(12, 5, '�ֺ¡��������� No.12', 'blog06_12.css', 'school12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_12.gif', 'minus_06_12.gif', 'link_01.gif', 'back_blogtop_06_12.gif', 'back_top_06_12.gif');
insert into sy_theme values(13, 5, '�ֺ¡��������� No.13', 'blog06_13.css', 'school13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_13.gif', 'minus_06_13.gif', 'link_01.gif', 'back_blogtop_06_13.gif', 'back_top_06_13.gif');
insert into sy_theme values(14, 5, '�ֺ¡��������� No.14', 'blog06_14.css', 'school14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_14.gif', 'minus_06_14.gif', 'link_01.gif', 'back_blogtop_06_14.gif', 'back_top_06_14.gif');
insert into sy_theme values(15, 5, '�ֺ¡��������� No.15', 'blog06_15.css', 'school15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_15.gif', 'minus_06_15.gif', 'link_01.gif', 'back_blogtop_06_15.gif', 'back_top_06_15.gif');
insert into sy_theme values(16, 5, '�ֺ¡��������� No.16', 'blog06_16.css', 'school16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_16.gif', 'minus_06_16.gif', 'link_01.gif', 'back_blogtop_06_16.gif', 'back_top_06_16.gif');
insert into sy_theme values(17, 5, '�ֺ¡��������� No.17', 'blog06_17.css', 'school17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_17.gif', 'minus_06_17.gif', 'link_01.gif', 'back_blogtop_06_17.gif', 'back_top_06_17.gif');
insert into sy_theme values(18, 5, '�ֺ¡��������� No.18', 'blog06_18.css', 'school18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_18.gif', 'minus_06_18.gif', 'link_01.gif', 'back_blogtop_06_18.gif', 'back_top_06_18.gif');
insert into sy_theme values(19, 5, '�ֺ¡��������� No.19', 'blog06_19.css', 'school19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_19.gif', 'minus_06_19.gif', 'link_01.gif', 'back_blogtop_06_19.gif', 'back_top_06_19.gif');
insert into sy_theme values(20, 5, '�ֺ¡��������� No.20', 'blog06_20.css', 'school20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_20.gif', 'minus_06_20.gif', 'link_01.gif', 'back_blogtop_06_20.gif', 'back_top_06_20.gif');
insert into sy_theme values(1, 6, '���ߡ����� No.1', 'blog03_01.css', 'facilities01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_01.gif', 'minus_03_01.gif', 'link_01.gif', 'back_blogtop_03_01.gif', 'back_top_03_01.gif');
insert into sy_theme values(2, 6, '���ߡ����� No.2', 'blog03_02.css', 'facilities02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_02.gif', 'minus_03_02.gif', 'link_01.gif', 'back_blogtop_03_02.gif', 'back_top_03_02.gif');
insert into sy_theme values(3, 6, '���ߡ����� No.3', 'blog03_03.css', 'facilities03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_03.gif', 'minus_03_03.gif', 'link_01.gif', 'back_blogtop_03_03.gif', 'back_top_03_03.gif');
insert into sy_theme values(4, 6, '���ߡ����� No.4', 'blog03_04.css', 'facilities04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_04.gif', 'minus_03_04.gif', 'link_01.gif', 'back_blogtop_03_04.gif', 'back_top_03_04.gif');
insert into sy_theme values(5, 6, '���ߡ����� No.5', 'blog03_05.css', 'facilities05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_05.gif', 'minus_03_05.gif', 'link_01.gif', 'back_blogtop_03_05.gif', 'back_top_03_05.gif');
insert into sy_theme values(6, 6, '���ߡ����� No.6', 'blog03_06.css', 'facilities06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_06.gif', 'minus_03_06.gif', 'link_01.gif', 'back_blogtop_03_06.gif', 'back_top_03_06.gif');
insert into sy_theme values(7, 6, '���ߡ����� No.7', 'blog03_07.css', 'facilities07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_07.gif', 'minus_03_07.gif', 'link_01.gif', 'back_blogtop_03_07.gif', 'back_top_03_07.gif');
insert into sy_theme values(8, 6, '���ߡ����� No.8', 'blog03_08.css', 'facilities08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_08.gif', 'minus_03_08.gif', 'link_01.gif', 'back_blogtop_03_08.gif', 'back_top_03_08.gif');
insert into sy_theme values(9, 6, '���ߡ����� No.9', 'blog03_09.css', 'facilities09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_09.gif', 'minus_03_09.gif', 'link_01.gif', 'back_blogtop_03_09.gif', 'back_top_03_08.gif');
insert into sy_theme values(10, 6, '���ߡ����� No.10', 'blog03_10.css', 'facilities10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_10.gif', 'minus_03_10.gif', 'link_01.gif', 'back_blogtop_03_10.gif', 'back_top_03_10.gif');
insert into sy_theme values(11, 6, '���ߡ����� No.11', 'blog03_11.css', 'facilities11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_11.gif', 'minus_03_11.gif', 'link_01.gif', 'back_blogtop_03_11.gif', 'back_top_03_11.gif');

insert into sy_theme values(1, 7, '��󥿥� No.1', 'blog07_01.css', 'rental01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_01.gif', 'minus_07_01.gif', 'link_01.gif', 'back_blogtop_07_01.gif', 'back_top_07_01.gif');
insert into sy_theme values(2, 7, '��󥿥� No.2', 'blog07_02.css', 'rental02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_02.gif', 'minus_07_02.gif', 'link_01.gif', 'back_blogtop_07_02.gif', 'back_top_07_02.gif');
insert into sy_theme values(3, 7, '��󥿥� No.3', 'blog07_03.css', 'rental03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_03.gif', 'minus_07_03.gif', 'link_01.gif', 'back_blogtop_07_03.gif', 'back_top_07_03.gif');
insert into sy_theme values(4, 7, '��󥿥� No.4', 'blog07_04.css', 'rental04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_04.gif', 'minus_07_04.gif', 'link_01.gif', 'back_blogtop_07_04.gif', 'back_top_07_04.gif');
insert into sy_theme values(5, 7, '��󥿥� No.5', 'blog07_05.css', 'rental05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_05.gif', 'minus_07_05.gif', 'link_01.gif', 'back_blogtop_07_05.gif', 'back_top_07_05.gif');
insert into sy_theme values(6, 7, '��󥿥� No.6', 'blog07_06.css', 'rental06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_06.gif', 'minus_07_06.gif', 'link_01.gif', 'back_blogtop_07_06.gif', 'back_top_07_06.gif');
insert into sy_theme values(7, 7, '��󥿥� No.7', 'blog07_07.css', 'rental07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_07.gif', 'minus_07_07.gif', 'link_01.gif', 'back_blogtop_07_07.gif', 'back_top_07_07.gif');
insert into sy_theme values(8, 7, '��󥿥� No.8', 'blog07_08.css', 'rental08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_08.gif', 'minus_07_08.gif', 'link_01.gif', 'back_blogtop_07_08.gif', 'back_top_07_08.gif');
insert into sy_theme values(9, 7, '��󥿥� No.9', 'blog07_09.css', 'rental09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_09.gif', 'minus_07_09.gif', 'link_01.gif', 'back_blogtop_07_09.gif', 'back_top_07_09.gif');
insert into sy_theme values(10, 7, '��󥿥� No.10', 'blog07_10.css', 'rental10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_10.gif', 'minus_07_10.gif', 'link_01.gif', 'back_blogtop_07_10.gif', 'back_top_07_10.gif');
