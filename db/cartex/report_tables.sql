drop table if exists rt_seo_report;
create table rt_seo_report(
	d_report datetime,			-- ��ݡ��Ⱥ�������
	c_keyword varchar(64),		-- �������
	n_engine integer,			-- �������󥸥��ֹ�
	c_engine varchar(100),		-- �������󥸥�̾
	n_order integer,			-- ���
	n_number integer,			-- �������
	n_point integer,			-- ����
	c_rank varchar(8),			-- ���
	constraint seo_pkey primary key (d_report,c_keyword,c_engine)
) TYPE = InnoDB;

drop table if exists rt_year_sales_report;
create table rt_year_sales_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	n_amount integer,			-- ���夲���
	n_count integer,			-- ���夲���
	n_amount_per_day integer,	-- ��������������夲���
	n_count_per_day float,		-- ��������������夲���
	constraint year_sales_pkey primary key (d_report)
) TYPE = InnoDB;

drop table if exists rt_sales_ranking_report;
create table rt_sales_ranking_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	c_name varchar(128),		-- ����̾
	n_order integer,			-- ���
	n_amount integer,			-- ���夲���
	n_percent integer,			-- ���夲��ۤ����Τ��Ф���ѡ�����ơ���
	n_count integer,			-- ���夲���
	n_last_order integer,		-- ����ν��
	constraint sales_ranking_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_top_sales_report;
create table rt_top_sales_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	d_data datetime,			-- ��ǯ����Υǡ�����
	c_item_id varchar(64),		-- ����ID
	c_name varchar(128),		-- ����̾
	n_amount integer,			-- ���夲���
	constraint top_sales_pkey primary key (d_report,d_data,c_item_id)
) TYPE = InnoDB;

drop table if exists rt_top_daily_report;
create table rt_top_daily_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	d_data datetime,			-- ��ǯ����Υǡ�����
	c_item_id varchar(64),		-- ����ID
	c_name varchar(128),		-- ����̾
	n_count integer,			-- ���夲���
	constraint top_daily_pkey primary key (d_report,d_data,c_item_id)
) TYPE = InnoDB;

drop table if exists rt_access_report;
create table rt_access_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	n_visit integer,			-- ˬ���
	constraint access_pkey primary key (d_report)
) TYPE = InnoDB;

drop table if exists rt_refer_report;
create table rt_refer_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	n_order integer,			-- ���
	c_url varchar(255),			-- URL
	n_visit integer,			-- ˬ���
	constraint refer_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_enter_report;
create table rt_enter_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	n_order integer,			-- ���
	c_url varchar(255),			-- URL
	n_visit integer,			-- ˬ���
	constraint enter_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_exit_report;
create table rt_exit_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	n_order integer,			-- ���
	c_url varchar(255),			-- URL
	n_visit integer,			-- ˬ���
	constraint exit_pkey primary key (d_report,n_order)
) TYPE = InnoDB;

drop table if exists rt_keyword_report;
create table rt_keyword_report(
	d_report datetime,			-- ��ǯ����ʬ�Υ�ݡ��Ȥ�
	n_order integer,			-- ���
	c_keyword varchar(64),		-- �������
	c_engine varchar(32),		-- ���������󥸥�
	n_visit integer,			-- ˬ���
	n_last integer,				-- ������
	constraint exit_pkey primary key (d_report,n_order)
) TYPE = InnoDB;
