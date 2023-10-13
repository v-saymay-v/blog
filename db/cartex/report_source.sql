drop table if exists sy_auth_info;
create table sy_auth_info(
	c_user varchar(32),
	c_pass varchar(32),
	c_line varchar(32),
	c_value_begin varchar(32),
	c_value_end varchar(32),
	n_month integer,
	n_visit integer,
	n_columns integer,
	c_refer_line varchar(32),
	c_refer_begin varchar(32),
	c_refer_end varchar(32),
	c_refer_html1 varchar(32),
	c_refer_html2 varchar(32),
	c_enter_mark varchar(32),
	n_enter_visit integer,
	n_enter_url integer,
	c_exit_mark varchar(32),
	n_exit_visit integer,
	n_exit_url integer,
	c_refer_mark varchar(32),
	n_refer_visit integer,
	n_refer_url integer
) TYPE = InnoDB;
insert into sy_auth_info values (
	'asjadm', '48B9F95BF5',
	'<TR><TD NOWRAP>', '<FONT SIZE="-1">', '</FONT>', 0, 7, 11,
	'<TR>\r\n<TD ALIGN=center>', '<FONT SIZE="-1"><B>', '</B></FONT>', '<FONT SIZE="-1">', '</FONT>',
	'入口ページ中', 2, 3,
	'出口ページ中', 2, 3,
	'リンク元中', 1, 2);

drop table if exists sy_search_engine;
create table sy_search_engine(
	n_engine integer not null,
	c_engine varchar(100),
	n_max integer,
	f_multiple float,
	c_code varchar(10),
	c_cookie varchar(255),
	c_url varchar(255),
	c_whole_head varchar(255),
	c_whole_tail varchar(255),
	c_first_head varchar(255),
	c_line_head varchar(255),
	c_child_head varchar(255),
	c_url_head varchar(64),
	c_url_tail varchar(64),
	constraint seo_pkey primary key (n_engine)
) TYPE = InnoDB;
insert into sy_search_engine values(1, 'Yahoo!サーチ', 100, 1.1, 'UTF-8', '', 'http://search.yahoo.co.jp/search?ei=UTF-8\\&fr=sfp_as\\&n=100\\&p=%s\\&meta=vc%%3D', ' 約<strong>', '</strong>件', '', '<li><div class="ttl"><a class="yschttl"  href="','','/*-','"');
insert into sy_search_engine values(2, 'Google', 100, 1.1, 'UTF-8', '', 'http://www.google.co.jp/search?num=100\\&hl=ja\\&q=%s\\&lr=', '</b> の検索結果 約 <b>', '</b> 件中 <b>', '', '<div class=g','', '<a href="','"');
insert into sy_search_engine values(3, 'MSNサーチ', 100, 0.8, 'UTF-8', 'SRCHHPGUSR=NRSLT=100', 'http://search.live.com/results.aspx?q=%s\\&mkt=ja-jp\\&FORM=LVSP\\&go.x=0\\&go.y=0\\&go=Search', '<div id="search_header"><h1>%s</h1><h5>', ' 件中 1 ページ目</h5>', '<div id="results"><h2 class="hide">結果</h2><ul><li class="first"><h3>', '</ul></li> <li><h3>','</ul></li> <li class="child"><h3>','<a href="','"');
insert into sy_search_engine values(4, 'goo', 50, 1.0, 'EUC-JP', '', 'http://search.goo.ne.jp/web.jsp?from=query\\&MT=%s\\&web.x=0\\&web.y=0\\&DC=50\\&FR=%d\\&ET=','（&nbsp;<strong>','</strong>件中','','<div class="title">   ','','href="http://rd.search.goo.ne.jp/click?DEST=','&no=');

drop table if exists sy_search_point;
create table sy_search_point(
	n_min integer,
	n_max integer,
	n_point integer
) TYPE = InnoDB;
insert into sy_search_point values(1,1,100);
insert into sy_search_point values(2,2,99);
insert into sy_search_point values(3,3,98);
insert into sy_search_point values(4,4,97);
insert into sy_search_point values(5,5,96);
insert into sy_search_point values(6,6,95);
insert into sy_search_point values(7,7,94);
insert into sy_search_point values(8,8,93);
insert into sy_search_point values(9,9,92);
insert into sy_search_point values(10,10,91);
insert into sy_search_point values(11,15,80);
insert into sy_search_point values(16,20,75);
insert into sy_search_point values(21,30,65);
insert into sy_search_point values(31,40,50);
insert into sy_search_point values(41,50,10);
insert into sy_search_point values(50,100,5);

drop table if exists sy_add_point;
create table sy_add_point(
	n_min integer,
	n_max integer,
	n_point integer
) TYPE = InnoDB;
insert into sy_add_point values(1000000,1999999,5);
insert into sy_add_point values(2000000,2999999,7);
insert into sy_add_point values(3000000,3999999,10);
insert into sy_add_point values(4000000,4999999,12);
insert into sy_add_point values(5000000,NULL,15);

drop table if exists sy_search_rank;
create table sy_search_rank(
	n_min integer,
	n_max integer,
	c_rank varchar(16)
) TYPE = InnoDB;
insert into sy_search_rank values(41,50,'C');
insert into sy_search_rank values(51,60,'CC');
insert into sy_search_rank values(61,70,'B');
insert into sy_search_rank values(71,80,'BB');
insert into sy_search_rank values(81,85,'A');
insert into sy_search_rank values(86,90,'AA');
insert into sy_search_rank values(91,95,'S');
insert into sy_search_rank values(96,100,'S');
