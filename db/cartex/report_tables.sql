drop table if exists rt_seo_report;
create table rt_seo_report(
	d_report datetime,			-- レポート作成日付
	c_keyword varchar(64),		-- キーワード
	n_engine integer,			-- 検索エンジン番号
	c_engine varchar(100),		-- 検索エンジン名
	n_order integer,			-- 順位
	n_number integer,			-- 検索総数
	n_point integer,			-- 点数
	c_rank varchar(8),			-- ランク
	constraint seo_pkey primary key (d_report,c_keyword,c_engine)
) TYPE = InnoDB;

drop table if exists rt_year_sales_report;
create table rt_year_sales_report(
	d_report datetime,			-- 何年何月分のレポートか
	n_amount integer,			-- 売り上げ金額
	n_count integer,			-- 売り上げ件数
	n_amount_per_day integer,	-- 一日あたりの売り上げ金額
	n_count_per_day float,		-- 一日あたりの売り上げ件数
	constraint year_sales_pkey primary key (d_report)
) TYPE = InnoDB;

drop table if exists rt_sales_ranking_report;
create table rt_sales_ranking_report(
	d_report datetime,			-- 何年何月分のレポートか
	c_name varchar(128),		-- 商品名
	n_order integer,			-- 順位
	n_amount integer,			-- 売り上げ金額
	n_percent integer,			-- 売り上げ金額の全体に対するパーセンテージ
	n_count integer,			-- 売り上げ件数
	n_last_order integer,		-- 前月の順位
	constraint sales_ranking_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_top_sales_report;
create table rt_top_sales_report(
	d_report datetime,			-- 何年何月分のレポートか
	d_data datetime,			-- 何年何月のデータか
	c_item_id varchar(64),		-- 商品ID
	c_name varchar(128),		-- 商品名
	n_amount integer,			-- 売り上げ金額
	constraint top_sales_pkey primary key (d_report,d_data,c_item_id)
) TYPE = InnoDB;

drop table if exists rt_top_daily_report;
create table rt_top_daily_report(
	d_report datetime,			-- 何年何月分のレポートか
	d_data datetime,			-- 何年何月のデータか
	c_item_id varchar(64),		-- 商品ID
	c_name varchar(128),		-- 商品名
	n_count integer,			-- 売り上げ金額
	constraint top_daily_pkey primary key (d_report,d_data,c_item_id)
) TYPE = InnoDB;

drop table if exists rt_access_report;
create table rt_access_report(
	d_report datetime,			-- 何年何月分のレポートか
	n_visit integer,			-- 訪問数
	constraint access_pkey primary key (d_report)
) TYPE = InnoDB;

drop table if exists rt_refer_report;
create table rt_refer_report(
	d_report datetime,			-- 何年何月分のレポートか
	n_order integer,			-- 順位
	c_url varchar(255),			-- URL
	n_visit integer,			-- 訪問数
	constraint refer_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_enter_report;
create table rt_enter_report(
	d_report datetime,			-- 何年何月分のレポートか
	n_order integer,			-- 順位
	c_url varchar(255),			-- URL
	n_visit integer,			-- 訪問数
	constraint enter_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_exit_report;
create table rt_exit_report(
	d_report datetime,			-- 何年何月分のレポートか
	n_order integer,			-- 順位
	c_url varchar(255),			-- URL
	n_visit integer,			-- 訪問数
	constraint exit_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_keyword_report;
create table rt_keyword_report(
	d_report datetime,			-- 何年何月分のレポートか
	n_order integer,			-- 順位
	c_keyword varchar(64),		-- キーワード
	c_engine varchar(32),		-- サーチエンジン
	n_visit integer,			-- 訪問数
	n_last integer,				-- 前月順位
	constraint exit_pkey primary key (d_report,n_order)
) TYPE = InnoDB;
