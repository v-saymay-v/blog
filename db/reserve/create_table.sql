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
	c_item_kind varchar(64),					-- DBBモード時各関連項目の種類
	c_item_id varchar(64),						-- カートモード時の商品ID、DBBモード時各関連項目ID
	c_item_url varchar(255),					-- DBBモード時各関連項目へのURL
	c_item_owner integer,						-- DBBモード時各関連項目のオーナー
	d_entry_create_time datetime,				-- 作者指定の作成時間
	d_entry_real_time datetime,					-- 実際の作成時間
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

drop table if exists at_uploadmovie;
create table at_uploadmovie(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- 添付先記事
	c_filename varchar(255) not null,			-- アップロードファイル名
	c_origname varchar(255) not null,			-- オリジナルファイル名
	b_delete_mode bool not null,				-- 1 削除予約 0 それ以外
	constraint uploadfile_pkey primary key (n_blog_id,n_entry_id)
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
	c_link_url varchar(255) not null,			-- リンク元URL
	n_from_blog integer,				-- リンク元ブログ
	n_from_entry integer,				-- リンク元記事
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

drop table if exists at_parts;
create table at_parts(
	n_blog_id integer not null,
	n_parts_id integer not null,				-- ブログパーツID
	c_parts_title varchar(100),					-- ブログパーツタイトル
	c_parts_body text,							-- ブログパーツタグ
	b_parts_title bool,							-- 0 タイトル表示せず 1 タイトル表示する
	constraint rss_pkey primary key (n_blog_id,n_parts_id)
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
	'192.168.100.140',
	'rsv_#DBUSER#',
	'#USERDIR#/data/reserve/passwd.dat',
	'/reserve/images',
	'/reserve/cgi-bin/rsv_reserve.cgi'
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
insert into sy_filter values (3,'概要フィルタ',1);
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
insert into sy_ping values(2, 'drecom.jp', 'http://ping.rss.drecom.jp/', 0);
insert into sy_ping values(3, 'weblogs.com', 'http://rpc.weblogs.com/RPC2', 0);
insert into sy_ping values(4, 'ping.blo.gs', 'http://ping.blo.gs/', 0);

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
insert into sy_rss values(2, '日経BP', 'http://pheedo.nikkeibp.co.jp/f/nikkeibp_news_flash');
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
insert into sy_sidecontent values(7, 7, 7, 'RSS配信', 1, 0);
insert into sy_sidecontent values(8, 8, 8, 'ブログについて', 1, 1);
insert into sy_sidecontent values(9, 9, 9, '最新記事', 1, 1);
insert into sy_sidecontent values(12, 10, 12, 'ブログ内検索', 1, 1);
insert into sy_sidecontent values(14, 11, 14, 'アクセス数', 1, 0);

drop table if exists sy_theme_category;
create table sy_theme_category(
	n_category_id integer not null,				-- カテゴリID
	c_category_title varchar(32),				-- カテゴリタイトル
	c_theme_thumbnail varchar(255)				-- カテゴリサムネイル
) TYPE = InnoDB;
insert into sy_theme_category values( 1, 'ホテル・旅館', 'icon_hotel.jpg');
insert into sy_theme_category values( 2, '美容院・エステ', 'icon_beauty.jpg');
insert into sy_theme_category values( 3, '病院・クリニック', 'icon_clinic.jpg');
insert into sy_theme_category values( 4, '飲食店', 'icon_restaurant.jpg');
insert into sy_theme_category values( 5, '講座・スクール', 'icon_school.jpg');
insert into sy_theme_category values( 6, '施設・設備', 'icon_facilities.jpg');
insert into sy_theme_category values( 7, 'レンタル', 'icon_rental.jpg');

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
insert into sy_theme values(1, 1, 'ホテル・旅館 No.1', 'blog04_01.css', 'hotel01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_01.gif', 'minus_04_01.gif', 'link_01.gif', 'back_blogtop_04_01.gif', 'back_top_04_01.gif');
insert into sy_theme values(2, 1, 'ホテル・旅館 No.2', 'blog04_02.css', 'hotel02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_02.gif', 'minus_04_02.gif', 'link_01.gif', 'back_blogtop_04_02.gif', 'back_top_04_02.gif');
insert into sy_theme values(3, 1, 'ホテル・旅館 No.3', 'blog04_03.css', 'hotel03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_03.gif', 'minus_04_03.gif', 'link_01.gif', 'back_blogtop_04_03.gif', 'back_top_04_03.gif');
insert into sy_theme values(4, 1, 'ホテル・旅館 No.4', 'blog04_04.css', 'hotel04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_04.gif', 'minus_04_04.gif', 'link_01.gif', 'back_blogtop_04_04.gif', 'back_top_04_04.gif');
insert into sy_theme values(5, 1, 'ホテル・旅館 No.5', 'blog04_05.css', 'hotel05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_05.gif', 'minus_04_05.gif', 'link_01.gif', 'back_blogtop_04_05.gif', 'back_top_04_05.gif');
insert into sy_theme values(6, 1, 'ホテル・旅館 No.6', 'blog04_06.css', 'hotel06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_06.gif', 'minus_04_06.gif', 'link_01.gif', 'back_blogtop_04_06.gif', 'back_top_04_06.gif');
insert into sy_theme values(7, 1, 'ホテル・旅館 No.7', 'blog04_07.css', 'hotel07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_07.gif', 'minus_04_07.gif', 'link_01.gif', 'back_blogtop_04_07.gif', 'back_top_04_07.gif');
insert into sy_theme values(8, 1, 'ホテル・旅館 No.8', 'blog04_08.css', 'hotel08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_08.gif', 'minus_04_08.gif', 'link_01.gif', 'back_blogtop_04_08.gif', 'back_top_04_08.gif');
insert into sy_theme values(9, 1, 'ホテル・旅館 No.9', 'blog04_09.css', 'hotel09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_09.gif', 'minus_04_09.gif', 'link_01.gif', 'back_blogtop_04_09.gif', 'back_top_04_09.gif');
insert into sy_theme values(10, 1, 'ホテル・旅館 No.10', 'blog04_10.css', 'hotel10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_10.gif', 'minus_04_10.gif', 'link_01.gif', 'back_blogtop_04_10.gif', 'back_top_04_10.gif');
insert into sy_theme values(11, 1, 'ホテル・旅館 No.11', 'blog04_11.css', 'hotel11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_11.gif', 'minus_04_11.gif', 'link_01.gif', 'back_blogtop_04_11.gif', 'back_top_04_11.gif');
insert into sy_theme values(12, 1, 'ホテル・旅館 No.12', 'blog04_12.css', 'hotel12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_12.gif', 'minus_04_12.gif', 'link_01.gif', 'back_blogtop_04_12.gif', 'back_top_04_12.gif');
insert into sy_theme values(13, 1, 'ホテル・旅館 No.13', 'blog04_13.css', 'hotel13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_13.gif', 'minus_04_13.gif', 'link_01.gif', 'back_blogtop_04_13.gif', 'back_top_04_13.gif');
insert into sy_theme values(14, 1, 'ホテル・旅館 No.14', 'blog04_14.css', 'hotel14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_14.gif', 'minus_04_14.gif', 'link_01.gif', 'back_blogtop_04_14.gif', 'back_top_04_14.gif');
insert into sy_theme values(15, 1, 'ホテル・旅館 No.15', 'blog04_15.css', 'hotel15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_15.gif', 'minus_04_15.gif', 'link_01.gif', 'back_blogtop_04_15.gif', 'back_top_04_15.gif');
insert into sy_theme values(16, 1, 'ホテル・旅館 No.16', 'blog04_16.css', 'hotel16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_16.gif', 'minus_04_16.gif', 'link_01.gif', 'back_blogtop_04_16.gif', 'back_top_04_16.gif');
insert into sy_theme values(17, 1, 'ホテル・旅館 No.17', 'blog04_17.css', 'hotel17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_17.gif', 'minus_04_17.gif', 'link_01.gif', 'back_blogtop_04_17.gif', 'back_top_04_17.gif');
insert into sy_theme values(18, 1, 'ホテル・旅館 No.18', 'blog04_18.css', 'hotel18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_18.gif', 'minus_04_18.gif', 'link_01.gif', 'back_blogtop_04_18.gif', 'back_top_04_18.gif');
insert into sy_theme values(19, 1, 'ホテル・旅館 No.19', 'blog04_19.css', 'hotel19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_19.gif', 'minus_04_19.gif', 'link_01.gif', 'back_blogtop_04_19.gif', 'back_top_04_19.gif');
insert into sy_theme values(20, 1, 'ホテル・旅館 No.20', 'blog04_20.css', 'hotel20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_04_20.gif', 'minus_04_20.gif', 'link_01.gif', 'back_blogtop_04_20.gif', 'back_top_04_20.gif');
insert into sy_theme values(1, 2, '美容院・エステ No.1', 'blog01_01.css', 'beauty01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_01.gif', 'minus_01_01.gif', 'link_01.gif', 'back_blogtop_01_01.gif', 'back_top_01_01.gif');
insert into sy_theme values(2, 2, '美容院・エステ No.2', 'blog01_02.css', 'beauty02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_02.gif', 'minus_01_02.gif', 'link_01.gif', 'back_blogtop_01_02.gif', 'back_top_01_02.gif');
insert into sy_theme values(3, 2, '美容院・エステ No.3', 'blog01_03.css', 'beauty03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_03.gif', 'minus_01_03.gif', 'link_01.gif', 'back_blogtop_01_03.gif', 'back_top_01_03.gif');
insert into sy_theme values(4, 2, '美容院・エステ No.4', 'blog01_04.css', 'beauty04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_04.gif', 'minus_01_04.gif', 'link_01.gif', 'back_blogtop_01_04.gif', 'back_top_01_04.gif');
insert into sy_theme values(5, 2, '美容院・エステ No.5', 'blog01_05.css', 'beauty05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_05.gif', 'minus_01_05.gif', 'link_01.gif', 'back_blogtop_01_05.gif', 'back_top_01_05.gif');
insert into sy_theme values(6, 2, '美容院・エステ No.6', 'blog01_06.css', 'beauty06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_06.gif', 'minus_01_06.gif', 'link_01.gif', 'back_blogtop_01_06.gif', 'back_top_01_06.gif');
insert into sy_theme values(7, 2, '美容院・エステ No.7', 'blog01_07.css', 'beauty07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_07.gif', 'minus_01_07.gif', 'link_01.gif', 'back_blogtop_01_07.gif', 'back_top_01_07.gif');
insert into sy_theme values(8, 2, '美容院・エステ No.8', 'blog01_08.css', 'beauty08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_08.gif', 'minus_01_08.gif', 'link_01.gif', 'back_blogtop_01_08.gif', 'back_top_01_08.gif');
insert into sy_theme values(9, 2, '美容院・エステ No.9', 'blog01_09.css', 'beauty09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_09.gif', 'minus_01_09.gif', 'link_01.gif', 'back_blogtop_01_09.gif', 'back_top_01_09.gif');
insert into sy_theme values(10, 2, '美容院・エステ No.10', 'blog01_10.css', 'beauty10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_10.gif', 'minus_01_10.gif', 'link_01.gif', 'back_blogtop_01_10.gif', 'back_top_01_10.gif');
insert into sy_theme values(11, 2, '美容院・エステ No.11', 'blog01_11.css', 'beauty11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_11.gif', 'minus_01_11.gif', 'link_01.gif', 'back_blogtop_01_11.gif', 'back_top_01_11.gif');
insert into sy_theme values(12, 2, '美容院・エステ No.12', 'blog01_12.css', 'beauty12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_12.gif', 'minus_01_12.gif', 'link_01.gif', 'back_blogtop_01_12.gif', 'back_top_01_12.gif');
insert into sy_theme values(13, 2, '美容院・エステ No.13', 'blog01_13.css', 'beauty13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_13.gif', 'minus_01_13.gif', 'link_01.gif', 'back_blogtop_01_13.gif', 'back_top_01_13.gif');
insert into sy_theme values(14, 2, '美容院・エステ No.14', 'blog01_14.css', 'beauty14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_14.gif', 'minus_01_14.gif', 'link_01.gif', 'back_blogtop_01_14.gif', 'back_top_01_14.gif');
insert into sy_theme values(15, 2, '美容院・エステ No.15', 'blog01_15.css', 'beauty15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_15.gif', 'minus_01_15.gif', 'link_01.gif', 'back_blogtop_01_15.gif', 'back_top_01_15.gif');
insert into sy_theme values(16, 2, '美容院・エステ No.16', 'blog01_16.css', 'beauty16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_16.gif', 'minus_01_16.gif', 'link_01.gif', 'back_blogtop_01_16.gif', 'back_top_01_16.gif');
insert into sy_theme values(17, 2, '美容院・エステ No.17', 'blog01_17.css', 'beauty17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_17.gif', 'minus_01_17.gif', 'link_01.gif', 'back_blogtop_01_17.gif', 'back_top_01_17.gif');
insert into sy_theme values(18, 2, '美容院・エステ No.18', 'blog01_18.css', 'beauty18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_18.gif', 'minus_01_18.gif', 'link_01.gif', 'back_blogtop_01_18.gif', 'back_top_01_18.gif');
insert into sy_theme values(19, 2, '美容院・エステ No.19', 'blog01_19.css', 'beauty19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_19.gif', 'minus_01_19.gif', 'link_01.gif', 'back_blogtop_01_19.gif', 'back_top_01_19.gif');
insert into sy_theme values(20, 2, '美容院・エステ No.20', 'blog01_20.css', 'beauty20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_01_20.gif', 'minus_01_20.gif', 'link_01.gif', 'back_blogtop_01_20.gif', 'back_top_01_20.gif');
insert into sy_theme values(1, 3, '病院・クリニック No.1', 'blog02_01.css', 'clinic01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_01.gif', 'minus_02_01.gif', 'link_01.gif', 'back_blogtop_02_01.gif', 'back_top_02_01.gif');
insert into sy_theme values(2, 3, '病院・クリニック No.2', 'blog02_02.css', 'clinic02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_02.gif', 'minus_02_02.gif', 'link_01.gif', 'back_blogtop_02_02.gif', 'back_top_02_02.gif');
insert into sy_theme values(3, 3, '病院・クリニック No.3', 'blog02_03.css', 'clinic03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_03.gif', 'minus_02_03.gif', 'link_01.gif', 'back_blogtop_02_03.gif', 'back_top_02_03.gif');
insert into sy_theme values(4, 3, '病院・クリニック No.4', 'blog02_04.css', 'clinic04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_04.gif', 'minus_02_04.gif', 'link_01.gif', 'back_blogtop_02_04.gif', 'back_top_02_04.gif');
insert into sy_theme values(5, 3, '病院・クリニック No.5', 'blog02_05.css', 'clinic05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_05.gif', 'minus_02_05.gif', 'link_01.gif', 'back_blogtop_02_05.gif', 'back_top_02_05.gif');
insert into sy_theme values(6, 3, '病院・クリニック No.6', 'blog02_06.css', 'clinic06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_06.gif', 'minus_02_06.gif', 'link_01.gif', 'back_blogtop_02_06.gif', 'back_top_02_06.gif');
insert into sy_theme values(7, 3, '病院・クリニック No.7', 'blog02_07.css', 'clinic07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_07.gif', 'minus_02_07.gif', 'link_01.gif', 'back_blogtop_02_07.gif', 'back_top_02_07.gif');
insert into sy_theme values(8, 3, '病院・クリニック No.8', 'blog02_08.css', 'clinic08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_08.gif', 'minus_02_08.gif', 'link_01.gif', 'back_blogtop_02_08.gif', 'back_top_02_08.gif');
insert into sy_theme values(9, 3, '病院・クリニック No.9', 'blog02_09.css', 'clinic09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_09.gif', 'minus_02_09.gif', 'link_01.gif', 'back_blogtop_02_09.gif', 'back_top_02_09.gif');
insert into sy_theme values(10, 3, '病院・クリニック No.10', 'blog02_10.css', 'clinic10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_10.gif', 'minus_02_10.gif', 'link_01.gif', 'back_blogtop_02_10.gif', 'back_top_02_10.gif');
insert into sy_theme values(11, 3, '病院・クリニック No.11', 'blog02_11.css', 'clinic11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_11.gif', 'minus_02_11.gif', 'link_01.gif', 'back_blogtop_02_11.gif', 'back_top_02_11.gif');
insert into sy_theme values(12, 3, '病院・クリニック No.12', 'blog02_12.css', 'clinic12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_12.gif', 'minus_02_12.gif', 'link_01.gif', 'back_blogtop_02_12.gif', 'back_top_02_12.gif');
insert into sy_theme values(13, 3, '病院・クリニック No.13', 'blog02_13.css', 'clinic13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_13.gif', 'minus_02_13.gif', 'link_01.gif', 'back_blogtop_02_13.gif', 'back_top_02_13.gif');
insert into sy_theme values(14, 3, '病院・クリニック No.14', 'blog02_14.css', 'clinic14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_14.gif', 'minus_02_14.gif', 'link_01.gif', 'back_blogtop_02_14.gif', 'back_top_02_14.gif');
insert into sy_theme values(15, 3, '病院・クリニック No.15', 'blog02_15.css', 'clinic15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_15.gif', 'minus_02_15.gif', 'link_01.gif', 'back_blogtop_02_15.gif', 'back_top_02_15.gif');
insert into sy_theme values(16, 3, '病院・クリニック No.16', 'blog02_16.css', 'clinic16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_16.gif', 'minus_02_16.gif', 'link_01.gif', 'back_blogtop_02_16.gif', 'back_top_02_16.gif');
insert into sy_theme values(17, 3, '病院・クリニック No.17', 'blog02_17.css', 'clinic17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_17.gif', 'minus_02_17.gif', 'link_01.gif', 'back_blogtop_02_17.gif', 'back_top_02_17.gif');
insert into sy_theme values(18, 3, '病院・クリニック No.18', 'blog02_18.css', 'clinic18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_18.gif', 'minus_02_18.gif', 'link_01.gif', 'back_blogtop_02_18.gif', 'back_top_02_18.gif');
insert into sy_theme values(19, 3, '病院・クリニック No.19', 'blog02_19.css', 'clinic19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_19.gif', 'minus_02_19.gif', 'link_01.gif', 'back_blogtop_02_19.gif', 'back_top_02_19.gif');
insert into sy_theme values(20, 3, '病院・クリニック No.20', 'blog02_20.css', 'clinic20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_02_20.gif', 'minus_02_20.gif', 'link_01.gif', 'back_blogtop_02_20.gif', 'back_top_02_20.gif');
insert into sy_theme values(1, 4, '飲食店 No.1', 'blog05_01.css', 'restaurant01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_01.gif', 'minus_05_01.gif', 'link_01.gif', 'back_blogtop_05_01.gif', 'back_top_05_01.gif');
insert into sy_theme values(2, 4, '飲食店 No.2', 'blog05_02.css', 'restaurant02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_02.gif', 'minus_05_02.gif', 'link_01.gif', 'back_blogtop_05_02.gif', 'back_top_05_02.gif');
insert into sy_theme values(3, 4, '飲食店 No.3', 'blog05_03.css', 'restaurant03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_03.gif', 'minus_05_03.gif', 'link_01.gif', 'back_blogtop_05_03.gif', 'back_top_05_03.gif');
insert into sy_theme values(4, 4, '飲食店 No.4', 'blog05_04.css', 'restaurant04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_04.gif', 'minus_05_04.gif', 'link_01.gif', 'back_blogtop_05_04.gif', 'back_top_05_04.gif');
insert into sy_theme values(5, 4, '飲食店 No.5', 'blog05_05.css', 'restaurant05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_05.gif', 'minus_05_05.gif', 'link_01.gif', 'back_blogtop_05_05.gif', 'back_top_05_05.gif');
insert into sy_theme values(6, 4, '飲食店 No.6', 'blog05_06.css', 'restaurant06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_06.gif', 'minus_05_06.gif', 'link_01.gif', 'back_blogtop_05_06.gif', 'back_top_05_06.gif');
insert into sy_theme values(7, 4, '飲食店 No.7', 'blog05_07.css', 'restaurant07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_07.gif', 'minus_05_07.gif', 'link_01.gif', 'back_blogtop_05_07.gif', 'back_top_05_07.gif');
insert into sy_theme values(8, 4, '飲食店 No.8', 'blog05_08.css', 'restaurant08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_08.gif', 'minus_05_08.gif', 'link_01.gif', 'back_blogtop_05_08.gif', 'back_top_05_08.gif');
insert into sy_theme values(9, 4, '飲食店 No.9', 'blog05_09.css', 'restaurant09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_05_09.gif', 'minus_05_09.gif', 'link_01.gif', 'back_blogtop_05_09.gif', 'back_top_05_09.gif');
insert into sy_theme values(1, 5, '講座・スクール No.1', 'blog06_01.css', 'school01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_01.gif', 'minus_06_01.gif', 'link_01.gif', 'back_blogtop_06_01.gif', 'back_top_06_01.gif');
insert into sy_theme values(2, 5, '講座・スクール No.2', 'blog06_02.css', 'school02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_02.gif', 'minus_06_02.gif', 'link_01.gif', 'back_blogtop_06_02.gif', 'back_top_06_02.gif');
insert into sy_theme values(3, 5, '講座・スクール No.3', 'blog06_03.css', 'school03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_03.gif', 'minus_06_03.gif', 'link_01.gif', 'back_blogtop_06_03.gif', 'back_top_06_03.gif');
insert into sy_theme values(4, 5, '講座・スクール No.4', 'blog06_04.css', 'school04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_04.gif', 'minus_06_04.gif', 'link_01.gif', 'back_blogtop_06_04.gif', 'back_top_06_04.gif');
insert into sy_theme values(5, 5, '講座・スクール No.5', 'blog06_05.css', 'school05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_05.gif', 'minus_06_05.gif', 'link_01.gif', 'back_blogtop_06_05.gif', 'back_top_06_05.gif');
insert into sy_theme values(6, 5, '講座・スクール No.6', 'blog06_06.css', 'school06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_06.gif', 'minus_06_06.gif', 'link_01.gif', 'back_blogtop_06_06.gif', 'back_top_06_06.gif');
insert into sy_theme values(7, 5, '講座・スクール No.7', 'blog06_07.css', 'school07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_07.gif', 'minus_06_07.gif', 'link_01.gif', 'back_blogtop_06_07.gif', 'back_top_06_07.gif');
insert into sy_theme values(8, 5, '講座・スクール No.8', 'blog06_08.css', 'school08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_08.gif', 'minus_06_08.gif', 'link_01.gif', 'back_blogtop_06_08.gif', 'back_top_06_08.gif');
insert into sy_theme values(9, 5, '講座・スクール No.9', 'blog06_09.css', 'school09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_09.gif', 'minus_06_09.gif', 'link_01.gif', 'back_blogtop_06_09.gif', 'back_top_06_09.gif');
insert into sy_theme values(10, 5, '講座・スクール No.10', 'blog06_10.css', 'school10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_10.gif', 'minus_06_10.gif', 'link_01.gif', 'back_blogtop_06_10.gif', 'back_top_06_10.gif');
insert into sy_theme values(11, 5, '講座・スクール No.11', 'blog06_11.css', 'school11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_11.gif', 'minus_06_11.gif', 'link_01.gif', 'back_blogtop_06_11.gif', 'back_top_06_11.gif');
insert into sy_theme values(12, 5, '講座・スクール No.12', 'blog06_12.css', 'school12.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_12.gif', 'minus_06_12.gif', 'link_01.gif', 'back_blogtop_06_12.gif', 'back_top_06_12.gif');
insert into sy_theme values(13, 5, '講座・スクール No.13', 'blog06_13.css', 'school13.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_13.gif', 'minus_06_13.gif', 'link_01.gif', 'back_blogtop_06_13.gif', 'back_top_06_13.gif');
insert into sy_theme values(14, 5, '講座・スクール No.14', 'blog06_14.css', 'school14.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_14.gif', 'minus_06_14.gif', 'link_01.gif', 'back_blogtop_06_14.gif', 'back_top_06_14.gif');
insert into sy_theme values(15, 5, '講座・スクール No.15', 'blog06_15.css', 'school15.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_15.gif', 'minus_06_15.gif', 'link_01.gif', 'back_blogtop_06_15.gif', 'back_top_06_15.gif');
insert into sy_theme values(16, 5, '講座・スクール No.16', 'blog06_16.css', 'school16.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_16.gif', 'minus_06_16.gif', 'link_01.gif', 'back_blogtop_06_16.gif', 'back_top_06_16.gif');
insert into sy_theme values(17, 5, '講座・スクール No.17', 'blog06_17.css', 'school17.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_17.gif', 'minus_06_17.gif', 'link_01.gif', 'back_blogtop_06_17.gif', 'back_top_06_17.gif');
insert into sy_theme values(18, 5, '講座・スクール No.18', 'blog06_18.css', 'school18.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_18.gif', 'minus_06_18.gif', 'link_01.gif', 'back_blogtop_06_18.gif', 'back_top_06_18.gif');
insert into sy_theme values(19, 5, '講座・スクール No.19', 'blog06_19.css', 'school19.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_19.gif', 'minus_06_19.gif', 'link_01.gif', 'back_blogtop_06_19.gif', 'back_top_06_19.gif');
insert into sy_theme values(20, 5, '講座・スクール No.20', 'blog06_20.css', 'school20.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_06_20.gif', 'minus_06_20.gif', 'link_01.gif', 'back_blogtop_06_20.gif', 'back_top_06_20.gif');
insert into sy_theme values(1, 6, '施設・設備 No.1', 'blog03_01.css', 'facilities01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_01.gif', 'minus_03_01.gif', 'link_01.gif', 'back_blogtop_03_01.gif', 'back_top_03_01.gif');
insert into sy_theme values(2, 6, '施設・設備 No.2', 'blog03_02.css', 'facilities02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_02.gif', 'minus_03_02.gif', 'link_01.gif', 'back_blogtop_03_02.gif', 'back_top_03_02.gif');
insert into sy_theme values(3, 6, '施設・設備 No.3', 'blog03_03.css', 'facilities03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_03.gif', 'minus_03_03.gif', 'link_01.gif', 'back_blogtop_03_03.gif', 'back_top_03_03.gif');
insert into sy_theme values(4, 6, '施設・設備 No.4', 'blog03_04.css', 'facilities04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_04.gif', 'minus_03_04.gif', 'link_01.gif', 'back_blogtop_03_04.gif', 'back_top_03_04.gif');
insert into sy_theme values(5, 6, '施設・設備 No.5', 'blog03_05.css', 'facilities05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_05.gif', 'minus_03_05.gif', 'link_01.gif', 'back_blogtop_03_05.gif', 'back_top_03_05.gif');
insert into sy_theme values(6, 6, '施設・設備 No.6', 'blog03_06.css', 'facilities06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_06.gif', 'minus_03_06.gif', 'link_01.gif', 'back_blogtop_03_06.gif', 'back_top_03_06.gif');
insert into sy_theme values(7, 6, '施設・設備 No.7', 'blog03_07.css', 'facilities07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_07.gif', 'minus_03_07.gif', 'link_01.gif', 'back_blogtop_03_07.gif', 'back_top_03_07.gif');
insert into sy_theme values(8, 6, '施設・設備 No.8', 'blog03_08.css', 'facilities08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_08.gif', 'minus_03_08.gif', 'link_01.gif', 'back_blogtop_03_08.gif', 'back_top_03_08.gif');
insert into sy_theme values(9, 6, '施設・設備 No.9', 'blog03_09.css', 'facilities09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_09.gif', 'minus_03_09.gif', 'link_01.gif', 'back_blogtop_03_09.gif', 'back_top_03_08.gif');
insert into sy_theme values(10, 6, '施設・設備 No.10', 'blog03_10.css', 'facilities10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_10.gif', 'minus_03_10.gif', 'link_01.gif', 'back_blogtop_03_10.gif', 'back_top_03_10.gif');
insert into sy_theme values(11, 6, '施設・設備 No.11', 'blog03_11.css', 'facilities11.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_11.gif', 'minus_03_11.gif', 'link_01.gif', 'back_blogtop_03_11.gif', 'back_top_03_11.gif');

insert into sy_theme values(1, 7, 'レンタル No.1', 'blog07_01.css', 'rental01.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_01.gif', 'minus_07_01.gif', 'link_01.gif', 'back_blogtop_07_01.gif', 'back_top_07_01.gif');
insert into sy_theme values(2, 7, 'レンタル No.2', 'blog07_02.css', 'rental02.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_02.gif', 'minus_07_02.gif', 'link_01.gif', 'back_blogtop_07_02.gif', 'back_top_07_02.gif');
insert into sy_theme values(3, 7, 'レンタル No.3', 'blog07_03.css', 'rental03.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_03.gif', 'minus_07_03.gif', 'link_01.gif', 'back_blogtop_07_03.gif', 'back_top_07_03.gif');
insert into sy_theme values(4, 7, 'レンタル No.4', 'blog07_04.css', 'rental04.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_04.gif', 'minus_07_04.gif', 'link_01.gif', 'back_blogtop_07_04.gif', 'back_top_07_04.gif');
insert into sy_theme values(5, 7, 'レンタル No.5', 'blog07_05.css', 'rental05.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_05.gif', 'minus_07_05.gif', 'link_01.gif', 'back_blogtop_07_05.gif', 'back_top_07_05.gif');
insert into sy_theme values(6, 7, 'レンタル No.6', 'blog07_06.css', 'rental06.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_06.gif', 'minus_07_06.gif', 'link_01.gif', 'back_blogtop_07_06.gif', 'back_top_07_06.gif');
insert into sy_theme values(7, 7, 'レンタル No.7', 'blog07_07.css', 'rental07.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_07.gif', 'minus_07_07.gif', 'link_01.gif', 'back_blogtop_07_07.gif', 'back_top_07_07.gif');
insert into sy_theme values(8, 7, 'レンタル No.8', 'blog07_08.css', 'rental08.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_08.gif', 'minus_07_08.gif', 'link_01.gif', 'back_blogtop_07_08.gif', 'back_top_07_08.gif');
insert into sy_theme values(9, 7, 'レンタル No.9', 'blog07_09.css', 'rental09.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_09.gif', 'minus_07_09.gif', 'link_01.gif', 'back_blogtop_07_09.gif', 'back_top_07_09.gif');
insert into sy_theme values(10, 7, 'レンタル No.10', 'blog07_10.css', 'rental10.gif', 'news_back_01.gif', 'news_next_01.gif', 'news_backpage_01.gif', 'news_nextpage_01.gif', 'news_back_01_01.gif', 'news_next_01_01.gif', 'plus_03_10.gif', 'minus_07_10.gif', 'link_01.gif', 'back_blogtop_07_10.gif', 'back_top_07_10.gif');
