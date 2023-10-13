alter table sy_authinfo add c_table_login varchar(20);

drop table if exists at_sendlink;
create table at_sendlink(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- �����N���L��
	n_link_id integer not null,					-- �����NID
	c_link_title varchar(255),					-- �����N�^�C�g��
	c_link_url varchar(255) not null,			-- �����N��URL
	b_link_tb bool,								-- �g���b�N�o�b�N�ɂ�郊���N��
	constraint reratelink_pkey primary key (n_blog_id,n_entry_id,n_link_id)
) TYPE = InnoDB;

drop table if exists at_recvlink;
create table at_recvlink(
	n_blog_id integer not null,
	n_entry_id integer not null,				-- �����N��L��
	n_link_id integer not null,					-- �����NID
	n_from_blog integer not null,				-- �����N���u���O
	n_from_entry integer not null,				-- �����N���L��
	constraint reratelink_pkey primary key (n_blog_id,n_entry_id,n_link_id)
) TYPE = InnoDB;

alter table at_entry add c_item_kind varchar(64);
alter table at_entry add c_item_url varchar(255);
alter table at_entry add c_item_owner integer;
alter table at_entry add d_entry_real_time datetime;
update at_entry set d_entry_real_time = d_entry_modify_time where d_entry_real_time is null;

alter table at_profile add n_page_view_all bigint default 0;
alter table at_profile add n_unique_user_all bigint default 0;
