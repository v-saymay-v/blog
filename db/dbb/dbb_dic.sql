drop table if exists sy_point_kind;
create table sy_point_kind (
	n_kind integer not null,
	c_name varchar(20),
	n_max int,
	primary key (n_kind)
);
insert into sy_point_kind values(1, 'ÍÑ¸ì', 0);
insert into sy_point_kind values(2, '£Ä£Â£Â', 0);
insert into sy_point_kind values(3, '£Î£Ð£Â', 20);
insert into sy_point_kind values(4, '£Í£Ì£Â', 0);
insert into sy_point_kind values(5, '£Î£Ç', 0);
insert into sy_point_kind values(6, '£Â£Ì', 20);
insert into sy_point_kind values(7, '£Ã£Á', 20);
insert into sy_point_kind values(8, '£Ó£Ð£Ì', 0);

drop table if exists at_access;
create table at_access (
	n_blog_id integer not null,
	d_date date,
	n_count integer,
	constraint access_pkey primary key (n_blog_id,d_date)
) TYPE = InnoDB;

alter table at_entry add n_point integer;

drop table if exists at_point_detail;
create table at_point_detail (
	n_blog_id integer not null,
	n_entry_id integer not null,
	n_kind integer,
	n_point integer,
	constraint point_detail_pkey primary key (n_blog_id,n_entry_id,n_kind)
) TYPE = InnoDB;

drop table if exists at_keyword_appear;
create table at_keyword_appear (
	c_keyword varchar(64) not null,
	d_appear date,
	n_count integer,
	primary key (c_keyword, d_appear)
) TYPE = InnoDB;

--drop table if exists dt_keywords;
--create table dt_keywords (
--	n_seq integer auto_increment unique key,
--	c_keyword varchar(64) not null,
--	c_kana varchar(128),
--	c_type varchar(64),
--	n_point integer,
--	primary key (c_keyword)
--);
-- alter table dt_keywords add n_kind integer;
-- update dt_keywords T1,sy_point_kind T2 set T1.n_kind=T2.n_kind where T1.c_type=T2.c_name;
