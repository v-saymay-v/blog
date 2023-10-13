alter table sy_authinfo add c_table_login varchar(20);

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

alter table at_entry add c_item_kind varchar(64);
alter table at_entry add c_item_url varchar(255);
alter table at_entry add c_item_owner integer;
alter table at_entry add d_entry_real_time datetime;
update at_entry set d_entry_real_time = d_entry_modify_time where d_entry_real_time is null;

alter table at_profile add n_page_view_all bigint default 0;
alter table at_profile add n_unique_user_all bigint default 0;
