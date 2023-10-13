-- ----------------------------------------------------------------------
-- blog毎のトランザクションテーブル
-- ----------------------------------------------------------------------

drop table if exists at_blog;
create table at_blog(
	n_blog_id integer auto_increment not null,
	c_blog_id varchar(20),						-- ユーザーID
	c_blog_title varchar(100),					-- タイトル
	c_blog_subtitle text,						-- サブタイトル
	c_blog_description text,					-- 説明
	n_blog_status integer default 1,			-- ブログの状態
	n_blog_category integer,					-- ブログのカテゴリ
	b_setting_toppage_disptype bool default 0,	-- トップページ表示基準(日数基準/件数基準)
	n_setting_toppage_index integer default 7,	-- トップページの表示件数
	n_setting_toppage_day integer default 7,	-- トップページの表示日数
	n_setting_disp_per_page integer default 10,	-- ページあたりの表示件数
	n_setting_day_format integer default 1,		-- 日付の表示形式
	n_setting_time_format integer default 1,	-- 更新時間の表示形式
	n_setting_cut_length integer default 200,	-- 「...続きを読む」を表示する文字数
	b_default_trackback bool default 1,			-- トラックバック初期値
	b_default_comment bool default 1,			-- コメント初期値
	b_default_mode bool default 1,				-- モード初期値
	n_hbuser_id integer default 0,				-- HotBizブログ時の所有者ユーザーID
	n_iteminfo_id integer default 1,			-- 商品情報表示モード
	n_iteminfo_num integer default 3,			-- 商品情報表示数
	c_mail_subject varchar(255) default '%b　更新通知',
												-- BLOGが更新されたことを知らせるメールの件名
	c_mail_body text,							-- BLOGが更新されたことを知らせるメールの本文
	b_needlogin bool default 1,					-- ブログの参照に認証が必要か
	d_create_time datetime,						-- ブログ作成日
	constraint blog_pkey primary key(n_blog_id)
) TYPE = InnoDB;

drop table if exists at_profile;
create table at_profile(
	n_blog_id integer not null,
	c_author_nickname varchar(100) not null,	-- 作者のニックネーム
	c_author_firstname varchar(100),			-- 作者の名
	c_author_lastname varchar(100),				-- 作者の姓
	c_author_selfintro text,					-- 作者の自己紹介
	c_author_mail varchar(255),					-- 作者のメールアドレス
	c_author_image MEDIUMBLOB,					-- 作者画像ファイル実体
	c_image_type varchar(50),					-- 作者画像ファイルContent-Type
	n_image_size integer,						-- 作者画像ファイルサイズ
	c_login varchar(20),						-- 管理モードログインID
	c_password varchar(20),						-- 管理モードログインPASSWORD
	n_remind integer,							-- リマインダ質問番号
	c_remind varchar(255),						-- リマインダ答え
	c_access_key varchar(255),					-- セッション管理データ
	b_send_mail bool,							-- コメントやトラックバックがあったらメールを送信
	b_japanese_calendar bool default 0,			-- カレンダーの曜日を日本語で表示
	n_next_entry integer default 1,				-- 次のエントリーのID
	n_page_view bigint default 0,				-- 前日までの総ページビュー
	n_unique_user bigint default 0,				-- 前日までの総ユニークユーザー
	n_page_view_all bigint default 0,			-- 前月までの総ページビュー
	n_unique_user_all bigint default 0,			-- 前月までの総ユニークユーザー
	constraint profile_pkey primary key(n_blog_id)
) TYPE = InnoDB;

-- 表示の生情報(CRONにより2週間分のみ確保)
drop table if exists at_view;
create table at_view (
	n_blog_id integer not null,					-- バナーの所有者(クライアント)
	c_ip_addr varchar(20),						-- アクセス元IP
	c_user_agent varchar(255),					-- ブラウザ情報
	d_view timestamp,							-- 表示日時
	b_valid bool,								-- 有効だったかどうか
	index (d_view),
	index (n_blog_id),
	index (c_ip_addr, n_blog_id)
) TYPE = InnoDB;

drop table if exists at_category;
create table at_category(
	n_blog_id integer not null,
	n_category_id integer not null,				-- カテゴリーID
	n_category_order integer,					-- カテゴリーオーダー
	c_category_name varchar(30),				-- カテゴリー名
	constraint category_pkey primary key (n_blog_id, n_category_id)
) TYPE = InnoDB;

drop table if exists at_looks;
create table at_looks(
	n_blog_id integer not null,
	n_category_id integer not null,				-- テーマカテゴリ
	n_theme_id integer not null,				-- テーマ
	n_sidebar integer default 0,				-- サイドバー位置
	c_user_css varchar(255),					-- ユーザーcss場所
	constraint ping_pkey primary key (n_blog_id)
) TYPE = InnoDB;

drop table if exists at_entry;
create table at_entry(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- ID
	n_category_id integer not null,				-- 分類カテゴリ
	b_comment bool not null,					-- コメント受付可否
	b_trackback bool not null,					-- トラックバック受付可否
	b_mode bool not null,						-- 公開 or 下書き
	c_entry_title varchar(100),					-- タイトル
	c_entry_summary varchar(255),				-- 要旨
	c_entry_body text,							-- 本文
	c_entry_more text,							-- 本文（記事の続き）
	c_item_kind varchar(64),					-- 記事を書いたメンバー
	c_item_id varchar(64),						-- 記事を書いたグループ
	c_item_url varchar(255),					--
	c_item_owner integer,						--
	d_entry_create_time datetime,				-- 作者指定の作成時間
	d_entry_real_time datetime,					-- 実際の作成時間
	d_entry_delete_time datetime,				-- この時間を過ぎたら自動的に削除
	d_entry_modify_time timestamp,				-- 変更時間
	constraint entry_pkey primary key (n_blog_id,n_entry_id)
) TYPE = InnoDB;

drop table if exists at_uploadfile;
create table at_uploadfile(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- 添付先記事
	n_uploadfile_id integer not null,			-- アップロードファイルID
	c_filename varchar(255) not null,			-- アップロードファイル名
	c_fileimage MEDIUMBLOB,						-- アップロードファイル実体
	c_filetype varchar(255) not null,			-- アップロードファイル種類
	n_filesize integer,							-- アップロードファイルサイズ
	n_imagesize_x integer,						-- X方向イメージサイズ
	n_imagesize_y integer,						-- X方向イメージサイズ
	b_delete_mode bool not null,				-- 1 削除予約 0 それ以外
	constraint uploadfile_pkey primary key (n_blog_id,n_entry_id,n_uploadfile_id)
) TYPE = InnoDB;

drop table if exists at_comment;
create table at_comment(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- 親記事
	n_comment_id integer not null,				-- ID
	c_comment_author varchar(100),				-- 書き込んだ人の名前
	c_comment_ip varchar(128),					-- ip
	c_comment_mail varchar(255),				-- メールアドレス
	c_comment_url varchar(255),					-- URL
	c_comment_body text,						-- 本文
	d_comment_create_time datetime,				-- 作成時刻
	b_comment_accept bool default 1,			-- 承認されたか
	b_comment_filter bool default 0,			-- フィルターにかかったか
	b_comment_read bool default 0,				-- コメントを表示したか
	constraint comment_pkey primary key (n_blog_id,n_entry_id,n_comment_id)
) TYPE = InnoDB;

drop table if exists at_comment_filter;
create table at_comment_filter(
	n_blog_id integer not null,
	c_filter text,								-- コメント本文に含まれていたらはじく文字列
	b_valid bool,								-- フィルターが有効
	constraint comment_filter_pkey primary key (n_blog_id)
) TYPE = InnoDB;

drop table if exists at_trackback;
create table at_trackback(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- TB先記事
	n_tb_id integer not null,					-- TBID
	c_tb_title varchar(100),					-- TBタイトル
	c_tb_excerpt text,							-- TB概要
	c_tb_blog_name varchar(255) not null,		-- TB元ブログ名
	c_tb_url varchar(255) not null,				-- TB元URL
	c_tb_ip varchar(128),						-- TBIP
	d_tb_create_time datetime,					-- TB時間
	b_tb_accept bool default 1,					-- 承認されたか
	n_tb_filter integer default 0,				-- フィルターにかかったか
	b_tb_read bool default 0,					-- TBを表示したか
	constraint trackback_pkey primary key (n_blog_id,n_entry_id,n_tb_id)
) TYPE = InnoDB;

drop table if exists at_trackback_filter;
create table at_trackback_filter(
	n_blog_id integer not null,
	n_filter_id integer not null,
	c_filter text,								-- 対象に含まれていたらはじく文字列
	b_valid bool,								-- フィルターが有効
	constraint trackback_filter_pkey primary key (n_blog_id,n_filter_id)
) TYPE = InnoDB;

drop table if exists at_sendtb;
create table at_sendtb(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- TB先記事
	n_tb_id integer not null,					-- TBID
	c_tb_title varchar(100),					-- TBタイトル
	c_tb_excerpt text,							-- TB概要
	c_tb_url varchar(255) not null,				-- TB元URL
	d_tb_create_time datetime,					-- TB時間
	b_success bool,								-- 送信結果
	constraint trackback_pkey primary key (n_blog_id,n_entry_id,n_tb_id)
) TYPE = InnoDB;

drop table if exists at_sendlink;
create table at_sendlink(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- リンク元記事
	n_link_id integer not null,					-- リンクID
	c_link_title varchar(255),					-- リンクタイトル
	c_link_url varchar(255) not null,			-- リンク先URL
	b_link_tb bool,								-- トラックバックによるリンクか
	constraint reratelink_pkey primary key (n_blog_id,n_entry_id,n_link_id)
) TYPE = InnoDB;

drop table if exists at_recvlink;
create table at_recvlink(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- リンク先記事
	n_link_id integer not null,					-- リンクID
	n_from_blog integer not null,				-- リンク元ブログ
	n_from_entry integer not null,				-- リンク元記事
	constraint reratelink_pkey primary key (n_blog_id,n_entry_id,n_link_id)
) TYPE = InnoDB;

drop table if exists at_sidecontent;
create table at_sidecontent(
	n_blog_id integer not null,
	n_sidecontent_id integer not null,			-- サイドコンテンツID
	n_sidecontent_order integer,				-- サイドコンテンツ順番
	n_sidecontent_type integer not null,		-- サイドコンテンツ種別
	c_sidecontent_title varchar(255),			-- サイドコンテンツタイトル
	b_display bool,								-- 表示可否
	b_allow_outer bool,							-- ブログにログインが必要なとき、表示するか
	constraint sidecontent_pkey primary key (n_blog_id,n_sidecontent_id,n_sidecontent_type)
) TYPE = InnoDB;

drop table if exists at_archive;
create table at_archive(
	n_blog_id integer not null,
	n_archive_id integer not null,				-- アーカイブID
	n_archive_value integer,					-- 表示件数
	constraint archive_pkey primary key (n_blog_id,n_archive_id)
) TYPE = InnoDB;

drop table if exists at_linkitem;
create table at_linkitem(
	n_blog_id integer not null,
	n_linkitem_id integer not null,				-- リンクID
	n_linkgroup_id integer not null,			-- リンクグループID
	n_linkitem_order integer,					-- リンクグループ順番
	c_link_title varchar(100) not null,			-- リンクタイトル
	c_link_url varchar(255) not null,			-- リンクURL
	b_link_target bool,							-- 0 同じウィンドウ 1 新規ウィンドウ
	constraint linkitem_pkey primary key(n_blog_id, n_linkgroup_id, n_linkitem_id)
) TYPE = InnoDB;

drop table if exists at_rss;
create table at_rss(
	n_blog_id integer not null,
	n_rss_id integer not null,					-- ユーザー定義取り込みRSSID
	c_rss_title varchar(100),					-- 取り込みRSSタイトル
	c_rss_url varchar(255),						-- RSS配信元URL
	n_rss_article integer,						-- RSS記事数
	b_rss_target bool,							-- 0 同じウィンドウ 1 新規ウィンドウ
	b_default bool,								-- 送信初期値
	constraint rss_pkey primary key (n_blog_id,n_rss_id)
) TYPE = InnoDB;

drop table if exists at_ping;
create table at_ping(
	n_blog_id integer not null,
	n_ping_id integer not null,					-- ピング送信先ID
	c_ping_site varchar(255),					-- ピング送信先サイト
	c_ping_url varchar(255),					-- ピング送信URL
	b_default bool,								-- 送信初期値
	constraint ping_pkey primary key (n_blog_id,n_ping_id)
) TYPE = InnoDB;

drop table if exists at_denycomment;
create table at_denycomment (
	n_blog_id integer not null,
	n_host_id integer not null,
	c_host varchar(255),						-- コメント拒否ホスト
	constraint denycomment_pkey primary key (n_blog_id,n_host_id)
) TYPE = InnoDB;

drop table if exists at_denytrackback;
create table at_denytrackback (
	n_blog_id integer not null,
	n_host_id integer not null,
	c_host varchar(255),						-- トラックバック拒否ホスト
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
-- システムテーブル
-- ----------------------------------------------------------------------

drop table if exists sy_archive;
create table sy_archive(
	n_archive_id integer not null,				-- アーカイブID
	n_archive_value integer,					-- 表示件数
	constraint archive_pkey primary key (n_archive_id)
) TYPE = InnoDB;
insert into sy_archive values(2, 0);			-- 月別アーカイブ
insert into sy_archive values(4, 5);			-- コメントアーカイブ
insert into sy_archive values(5, 5);			-- トラックバックアーカイブ
insert into sy_archive values(9, 5);			-- 最新記事アーカイブ

drop table if exists sy_authinfo;
create table sy_authinfo(
	c_host varchar(255),						-- 認証情報DBがあるホスト
	c_db varchar(20),							-- 認証情報DBの名前
	c_user varchar(20),							-- 認証情報DBへのログインユーザー
	c_password varchar(20),						-- 認証情報DBへのログインパスワード
	c_passfile varchar(255),					-- 認証情報DBへのログインパスワードを記録したファイル
	c_table varchar(20),						-- 認証情報DB内のデータがあるテーブル
	c_table_login varchar(20),					-- 認証情報DB内のセッション情報があるテーブル
	c_column_login varchar(20),					-- 認証情報DB内のログインIDのカラム
	c_column_password varchar(20),				-- 認証情報DB内のログインパスワードのカラム
	c_column_session varchar(20),				-- 認証情報DB内のセッション情報のカラム
	c_column_blog varchar(20),					-- 認証情報DB内のブログIDのカラム
	c_column_owner varchar(20),					-- 認証情報DB内のオーナーIDのカラム
	c_column_nickname varchar(20),				-- 認証情報DB内のニックネームのカラム
	c_column_question varchar(20),				-- 認証情報DB内のリマインダ質問のカラム
	c_column_answer varchar(20),				-- 認証情報DB内のリマインダ答えのカラム
	c_column_mail varchar(20),					-- 認証情報DB内のメールアドレスのカラム
	c_cookie varchar(20),						-- セッション情報を書き込むCOOKIE名
	c_owner_cookie varchar(20),					-- オーナーIDを書き込むCOOKIE名
	c_cookie_host varchar(255),					-- Cookieを書き込むホスト
	c_cookie_path varchar(255),					-- Cookieを書き込むパス
	c_table_remid varchar(20),					-- リマインダの質問テーブル
	c_remid_id varchar(20),						-- リマインダの質問IDカラム
	c_remid_name varchar(20),					-- リマインダの質問内容カラム
	c_table_black varchar(20),					-- ブラックリストテーブル
	c_black_blog varchar(20),					-- ブログが登録されているかのカラム
	c_table_detail varchar(20),					-- ブラックリストテーブル
	c_mail_open varchar(20),					-- ブログが登録されているかのカラム
	c_image_location varchar(255),				-- アバターのイメージがあるところ
	c_login_cgi varchar(255)					-- ログイン用のCGI
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
	b_dbbmode bool,					-- ドリームベースボールのオーナーブログ
	b_cartmode bool,				-- ショッパープラスの店長ブログ
	b_hbmode bool,					-- HotBizのメンバーブログ
	b_hostingmode bool,				-- 管理者モードへの認証方法がApacheの認証かどうか
	b_shortname bool,				-- ブログアクセスに/0000000x/のような方法をとるかどうか
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
	n_status_id integer not null,				-- 状態ID
	c_status_name varchar(30),					-- 状態名
	constraint blogstatus_pkey primary key (n_status_id)
) TYPE = InnoDB;
insert into sy_blogstatus values (1,'稼働中');
insert into sy_blogstatus values (2,'停止中');
insert into sy_blogstatus values (3,'廃止');

drop table if exists sy_cartinfo;
create table sy_cartinfo(
	c_host varchar(255),						-- カートDBがあるホスト
	c_database varchar(255),					-- カートDBのデータベース名
	c_passfile varchar(255),					-- カートDBへのログインパスワードが書かれているファイル
	c_imageloc varchar(255),					-- 商品画像を表示するためのベースロケーション
	c_purchase_cgi varchar(255)					-- かごに入れるためのCGI
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
	n_category_id integer not null,				-- カテゴリーID
	n_category_order integer,					-- カテゴリーオーダー
	c_category_name varchar(30),				-- カテゴリー名
	constraint category_pkey primary key (n_category_id)
) TYPE = InnoDB;
insert into sy_category values (0,0,'カテゴリーなし');

drop table if exists sy_filter;
create table sy_filter(
	n_filter_id integer not null,
	c_filter_name varchar(30),
	b_need_text bool,
	constraint filter_pkey primary key (n_filter_id)
) TYPE = InnoDB;
insert into sy_filter values (1,'URLフィルタ',1);
insert into sy_filter values (2,'ブログ名フィルタ',1);
insert into sy_filter values (3,'コンテンツフィルタ',1);
insert into sy_filter values (4,'送付元記事フィルタ',0);

drop table if exists sy_iteminfo;
create table sy_iteminfo(
	n_iteminfo_id integer not null,				-- 商品情報表示方法ID
	c_iteminfo_name varchar(32),				-- 商品情報表示方法
	constraint ping_pkey primary key (n_iteminfo_id)
) TYPE = InnoDB;
insert into sy_iteminfo values (1, '特売商品');
insert into sy_iteminfo values (2, '新着商品');
insert into sy_iteminfo values (3, '人気商品');

drop table if exists sy_ping;
create table sy_ping(
	n_ping_id integer not null,					-- ピング送信先ID
	c_ping_site varchar(200),					-- ピング送信先サイト
	c_ping_url varchar(200),					-- ピング送信URL
	b_default bool,								-- 送信初期値
	constraint ping_pkey primary key (n_ping_id)
) TYPE = InnoDB;
insert into sy_ping values(1, 'ping.bloggers.jp', 'http://ping.bloggers.jp/rpc/', 0);
insert into sy_ping values(2, 'ping.cocolog-nifty.com', 'http://ping.cocolog-nifty.com/xmlrpc', 0);
insert into sy_ping values(3, 'myblog.jp', 'http://ping.myblog.jp/', 0);
insert into sy_ping values(4, 'weblogs.com', 'http://rpc.weblogs.com/RPC2', 0);
insert into sy_ping values(5, 'ping.blo.gs', 'http://ping.blo.gs/', 0);

drop table if exists sy_remind;
create table sy_remind (
	n_remind integer not null,					-- 番号
	c_remind varchar(50),						-- 質問
	primary key (n_remind)
) TYPE = InnoDB;
insert into sy_remind values (1,'ペットの名前は？');
insert into sy_remind values (2,'好きな芸能人は？');
insert into sy_remind values (3,'母親の旧姓は？');
insert into sy_remind values (4,'趣味は？');

drop table if exists sy_rss;
create table sy_rss(
	n_rss_id integer not null,					-- 用意されたRSS取り込みID
	c_rss_title varchar(50),					-- RSSタイトル
	c_rss_url varchar(200),						-- RSSURL
	constraint rss_pkey primary key(n_rss_id)
) TYPE = InnoDB;
insert into sy_rss values(1, 'アサヒコム', 'http://www3.asahi.com/rss/index.rdf');
insert into sy_rss values(2, '日経BP', 'http://nikkeibp.jp/jp/flash/index.rdf');
insert into sy_rss values(3, 'mycom', 'http://journal.mycom.co.jp/haishin/rss/index.rdf');
-- insert into sy_rss values(4, '読売新聞・社会', 'http://www.yomiusa.com/rdf/yomisyakai.xml');
-- insert into sy_rss values(5, '産経新聞・社会', 'http://www.yomiusa.com/rdf/sansyakai.xml');
insert into sy_rss values(4, 'CNET Japan', 'http://japan.cnet.com/rss/index.rdf');
insert into sy_rss values(5, 'FNN Headline News', 'http://rss.fujitv.co.jp/fnnnews.xml');

drop table if exists sy_sidecontent;
create table sy_sidecontent(
	n_sidecontent_id integer not null,			-- サイドコンテンツID
	n_sidecontent_order integer,				-- サイドコンテンツ順番
	n_sidecontent_type integer not null,		-- サイドコンテンツ種別
	c_sidecontent_title varchar(200),			-- サイドコンテンツタイトル
	b_display bool,								-- 表示可否
	b_allow_outer bool,							-- ブログにログインが必要なとき、表示するか
	constraint sidecontent_pkey primary key (n_sidecontent_id)
) TYPE = InnoDB;
insert into sy_sidecontent values(0, 0, 0, '管理者メニュー', 1, 0);
insert into sy_sidecontent values(1, 1, 1, 'カレンダー', 1, 1);
insert into sy_sidecontent values(2, 2, 2, '月別アーカイブ', 1, 1);
insert into sy_sidecontent values(3, 3, 3, 'カテゴリー別アーカイブ', 1, 1);
insert into sy_sidecontent values(4, 4, 4, '最新コメント', 1, 1);
insert into sy_sidecontent values(5, 5, 5, '最新トラックバック', 1, 1);
insert into sy_sidecontent values(6, 6, 6, 'プロフィール', 1, 1);
insert into sy_sidecontent values(7, 7, 7, 'RSS配信', 1, 1);
insert into sy_sidecontent values(8, 8, 8, 'ブログについて', 1, 1);
insert into sy_sidecontent values(9, 9, 9, '最新記事', 1, 1);
insert into sy_sidecontent values(12, 10, 12, 'ブログ内検索', 1, 1);
insert into sy_sidecontent values(13, 11, 13, '訪問者リスト', 1, 0);
insert into sy_sidecontent values(14, 12, 14, 'アクセス数', 1, 0);

drop table if exists sy_theme_category;
create table sy_theme_category(
	n_category_id integer not null,				-- カテゴリID
	c_category_title varchar(32),				-- カテゴリタイトル
	c_theme_thumbnail varchar(255)				-- カテゴリサムネイル
) TYPE = InnoDB;
insert into sy_theme_category values( 1, '食品・飲料', 'icon_cate_food.jpg');
insert into sy_theme_category values( 2, 'スイーツ・和菓子', 'icon_cate_sweets.jpg');
insert into sy_theme_category values( 3, 'ファッション', 'icon_cate_fashion.jpg');
insert into sy_theme_category values( 4, 'ベビー用品', 'icon_cate_baby.jpg');
insert into sy_theme_category values( 5, '医療・福祉', 'icon_cate_medical.jpg');
insert into sy_theme_category values( 6, '美容・健康', 'icon_cate_beauty.jpg');
insert into sy_theme_category values( 7, 'インテリア・雑貨', 'icon_cate_interior.jpg');
insert into sy_theme_category values( 8, 'オフィス用品', 'icon_cate_office.jpg');
insert into sy_theme_category values( 9, '家電・電気', 'icon_cate_audio.jpg');
insert into sy_theme_category values(10, '趣味・娯楽', 'icon_cate_hobby.jpg');
insert into sy_theme_category values(11, 'おもちゃ・ゲーム', 'icon_cate_game.jpg');
insert into sy_theme_category values(12, 'スポーツ用品', 'icon_cate_sports.jpg');
insert into sy_theme_category values(13, '音楽・映像・本', 'icon_cate_media.jpg');
insert into sy_theme_category values(14, 'ペット用品', 'icon_cate_animal.jpg');
insert into sy_theme_category values(15, '花・ギフト', 'icon_cate_season.jpg');
insert into sy_theme_category values(16, '家・マンション', 'icon_cate_house.jpg');

drop table if exists sy_theme;
create table sy_theme(
	n_theme_id integer not null,				-- テーマID
	n_category_id integer not null,				-- カテゴリID
	c_theme_title varchar(32),					-- テーマタイトル
	c_theme_file varchar(255),					-- テーマCSS
	c_theme_thumbnail varchar(255),				-- テーマサムネイル
	c_theme_prev_entry_icon varchar(255),		-- 前の記事へのアイコン
	c_theme_next_entry_icon varchar(255),		-- 次の記事へのアイコン
	c_theme_prev_page_icon varchar(255),		-- 前のページへのアイコン
	c_theme_next_page_icon varchar(255),		-- 次のページへのアイコン
	c_theme_first_icon varchar(255),			-- 最初へのアイコン
	c_theme_last_icon varchar(255),				-- 最後へのアイコン
	c_theme_plus_icon varchar(255),				-- 三角のなかにプラス(+)
	c_theme_minus_icon varchar(255),			-- 三角のなかにマイナス(-)
	c_theme_link_btn_icon varchar(255),			-- リンクボタンのアイコン
	c_theme_blog_top_icon varchar(255),			-- ブログトップへのアイコン
	c_theme_page_top_icon varchar(255),			-- ページトップへのアイコン
	constraint theme_pkey primary key(n_category_id, n_theme_id)
) TYPE = InnoDB;
-- CARTモード用
insert into sy_theme values(1, 1, '食品・飲料 No.1', 'blog31.css', 'food01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_31.gif', 'minus_31.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 1, '食品・飲料 No.2', 'blog32.css', 'food02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_32.gif', 'minus_32.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 1, '食品・飲料 No.3', 'blog33.css', 'food03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_33.gif', 'minus_33.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 1, '食品・飲料 No.4', 'blog34.css', 'food04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_34.gif', 'minus_34.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 1, '食品・飲料 No.5', 'blog35.css', 'food05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_35.gif', 'minus_35.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 2, 'スイーツ・和菓子 No.1', 'blog76.css', 'sweets01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_76.gif', 'minus_76.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 2, 'スイーツ・和菓子 No.2', 'blog77.css', 'sweets02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_77.gif', 'minus_77.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 2, 'スイーツ・和菓子 No.3', 'blog78.css', 'sweets03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_78.gif', 'minus_78.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 2, 'スイーツ・和菓子 No.4', 'blog79.css', 'sweets04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_79.gif', 'minus_79.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 2, 'スイーツ・和菓子 No.5', 'blog80.css', 'sweets05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_80.gif', 'minus_80.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 3, 'ファッション No.1', 'blog21.css', 'fashion01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_21.gif', 'minus_21.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 3, 'ファッション No.2', 'blog22.css', 'fashion02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_22.gif', 'minus_22.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 3, 'ファッション No.3', 'blog23.css', 'fashion03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_23.gif', 'minus_23.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 3, 'ファッション No.4', 'blog24.css', 'fashion04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_24.gif', 'minus_24.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 3, 'ファッション No.5', 'blog25.css', 'fashion05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_25.gif', 'minus_25.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 4, 'ベビー用品 No.1', 'blog01.css', 'baby01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01.gif', 'minus_01.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 4, 'ベビー用品 No.2', 'blog02.css', 'baby02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02.gif', 'minus_02.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 4, 'ベビー用品 No.3', 'blog03.css', 'baby03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03.gif', 'minus_03.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 4, 'ベビー用品 No.4', 'blog04.css', 'baby04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04.gif', 'minus_04.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 4, 'ベビー用品 No.5', 'blog05.css', 'baby05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05.gif', 'minus_05.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 5, '医療・福祉 No.1', 'blog56.css', 'medical01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_56.gif', 'minus_56.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 5, '医療・福祉 No.2', 'blog57.css', 'medical02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_57.gif', 'minus_57.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 5, '医療・福祉 No.3', 'blog58.css', 'medical03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_58.gif', 'minus_58.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 5, '医療・福祉 No.4', 'blog59.css', 'medical04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_59.gif', 'minus_59.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 5, '医療・福祉 No.5', 'blog60.css', 'medical05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_60.gif', 'minus_60.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 6, '美容・健康 No.1', 'blog06.css', 'beauty01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06.gif', 'minus_06.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 6, '美容・健康 No.2', 'blog07.css', 'beauty02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_07.gif', 'minus_07.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 6, '美容・健康 No.3', 'blog08.css', 'beauty03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_08.gif', 'minus_08.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 6, '美容・健康 No.4', 'blog09.css', 'beauty04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_09.gif', 'minus_09.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 6, '美容・健康 No.5', 'blog10.css', 'beauty05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_10.gif', 'minus_10.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 7, 'インテリア・雑貨 No.1', 'blog51.css', 'interior01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_51.gif', 'minus_51.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 7, 'インテリア・雑貨 No.2', 'blog52.css', 'interior02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_52.gif', 'minus_52.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 7, 'インテリア・雑貨 No.3', 'blog53.css', 'interior03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_53.gif', 'minus_53.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 7, 'インテリア・雑貨 No.4', 'blog54.css', 'interior04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_54.gif', 'minus_54.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 7, 'インテリア・雑貨 No.5', 'blog55.css', 'interior05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_55.gif', 'minus_55.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 8, 'オフィス用品 No.1', 'blog61.css', 'office01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_61.gif', 'minus_61.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 8, 'オフィス用品 No.2', 'blog62.css', 'office02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_62.gif', 'minus_62.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 8, 'オフィス用品 No.3', 'blog63.css', 'office03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_63.gif', 'minus_63.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 8, 'オフィス用品 No.4', 'blog64.css', 'office04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_64.gif', 'minus_64.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 8, 'オフィス用品 No.5', 'blog65.css', 'office05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_65.gif', 'minus_65.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 9, '家電・電気 No.1', 'blog16.css', 'electro01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_16.gif', 'minus_16.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 9, '家電・電気 No.2', 'blog17.css', 'electro02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_17.gif', 'minus_17.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 9, '家電・電気 No.3', 'blog18.css', 'electro03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_18.gif', 'minus_18.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 9, '家電・電気 No.4', 'blog19.css', 'electro04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_19.gif', 'minus_19.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 9, '家電・電気 No.5', 'blog20.css', 'electro05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_20.gif', 'minus_20.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 10, '趣味・娯楽 No.1', 'blog41.css', 'hobby01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_41.gif', 'minus_41.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 10, '趣味・娯楽 No.2', 'blog42.css', 'hobby02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_42.gif', 'minus_42.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 10, '趣味・娯楽 No.3', 'blog43.css', 'hobby03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_43.gif', 'minus_43.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 10, '趣味・娯楽 No.4', 'blog44.css', 'hobby04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_44.gif', 'minus_44.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 10, '趣味・娯楽 No.5', 'blog45.css', 'hobby05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_45.gif', 'minus_45.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 11, 'おもちゃ・ゲーム No.1', 'blog36.css', 'games01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_36.gif', 'minus_36.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 11, 'おもちゃ・ゲーム No.2', 'blog37.css', 'games02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_37.gif', 'minus_37.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 11, 'おもちゃ・ゲーム No.3', 'blog38.css', 'games03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_38.gif', 'minus_38.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 11, 'おもちゃ・ゲーム No.4', 'blog39.css', 'games04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_39.gif', 'minus_39.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 11, 'おもちゃ・ゲーム No.5', 'blog40.css', 'games05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_40.gif', 'minus_40.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 12, 'スポーツ用品 No.1', 'blog71.css', 'sports01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_71.gif', 'minus_71.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 12, 'スポーツ用品 No.2', 'blog72.css', 'sports02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_72.gif', 'minus_72.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 12, 'スポーツ用品 No.3', 'blog73.css', 'sports03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_73.gif', 'minus_73.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 12, 'スポーツ用品 No.4', 'blog74.css', 'sports04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_74.gif', 'minus_74.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 12, 'スポーツ用品 No.5', 'blog75.css', 'sports05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_75.gif', 'minus_75.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 13, '音楽・映像・本 No.1', 'blog11.css', 'books01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_11.gif', 'minus_11.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 13, '音楽・映像・本 No.2', 'blog12.css', 'books02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_12.gif', 'minus_12.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 13, '音楽・映像・本 No.3', 'blog13.css', 'books03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_13.gif', 'minus_13.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 13, '音楽・映像・本 No.4', 'blog14.css', 'books04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_14.gif', 'minus_14.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 13, '音楽・映像・本 No.5', 'blog15.css', 'books05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_15.gif', 'minus_15.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 14, 'ペット用品 No.1', 'blog66.css', 'pet01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_66.gif', 'minus_66.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 14, 'ペット用品 No.2', 'blog67.css', 'pet02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_67.gif', 'minus_67.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 14, 'ペット用品 No.3', 'blog68.css', 'pet03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_68.gif', 'minus_68.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 14, 'ペット用品 No.4', 'blog69.css', 'pet04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_69.gif', 'minus_69.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 14, 'ペット用品 No.5', 'blog70.css', 'pet05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_70.gif', 'minus_70.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 15, '花・ギフト No.1', 'blog26.css', 'flower01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_26.gif', 'minus_26.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 15, '花・ギフト No.2', 'blog27.css', 'flower02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_27.gif', 'minus_27.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 15, '花・ギフト No.3', 'blog28.css', 'flower03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_28.gif', 'minus_28.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 15, '花・ギフト No.4', 'blog29.css', 'flower04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_29.gif', 'minus_29.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 15, '花・ギフト No.5', 'blog30.css', 'flower05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_30.gif', 'minus_30.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(1, 16, '家・マンション No.1', 'blog46.css', 'housing01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_46.gif', 'minus_46.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(2, 16, '家・マンション No.2', 'blog47.css', 'housing02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_47.gif', 'minus_47.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(3, 16, '家・マンション No.3', 'blog48.css', 'housing03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_48.gif', 'minus_48.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(4, 16, '家・マンション No.4', 'blog49.css', 'housing04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_49.gif', 'minus_49.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
insert into sy_theme values(5, 16, '家・マンション No.5', 'blog50.css', 'housing05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_50.gif', 'minus_50.gif', 'link_01.gif', 'back_blogtop.gif', 'back_top.gif');
