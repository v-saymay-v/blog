insert into sy_sidecontent values(15, 13, 15, '���i���', 1, 1);
insert into sy_sidecontent values(16, 14, 16, '�����ߋL��', 1, 1);
insert into sy_sidecontent values(17, 15, 17, '�J�[�g���O�C��', 1, 1);
insert into sy_sidecontent values(18, 16, 18, '����o�^', 1, 1);
insert into at_sidecontent select n_blog_id, 15, 13, 15, '���i���', 1, 1 from at_blog;
insert into at_sidecontent select n_blog_id, 16, 14, 16, '�����ߋL��', 1, 1 from at_blog;
insert into at_sidecontent select n_blog_id, 17, 15, 17, '�J�[�g���O�C��', 1, 1 from at_blog;
insert into at_sidecontent select n_blog_id, 18, 16, 18, '����o�^', 1, 1 from at_blog;

alter table sy_authinfo add c_passfile varchar(255);
alter table sy_baseinfo add b_cartmode bool;
alter table sy_baseinfo add b_hbmode bool;
alter table sy_baseinfo add b_hostingmode bool;
update sy_baseinfo set c_systemname = "�X���u���O";
alter table at_blog add c_blog_id varchar(20);
alter table at_blog add n_hbuser_id integer default 0;
alter table at_blog add n_iteminfo_id integer default 1;
alter table at_blog add n_iteminfo_num integer default 3;
alter table at_blog add c_mail_subject varchar(255) default '%b�@�X�V�ʒm';
alter table at_blog add c_mail_body text;
update at_blog set c_mail_body = '%c�@�l\r\n�u���O���X�V���܂����̂ł��m�点���܂��B���LURL���A�N�Z�X���Ă��������B\r\n%u\r\n';
alter table at_entry add c_item_id varchar(64);

drop table if exists sy_cartinfo;
create table sy_cartinfo(
	c_host varchar(255),						-- �J�[�gDB������z�X�g
	c_database varchar(255),					-- �J�[�gDB�̃f�[�^�x�[�X��
	c_passfile varchar(255),					-- �J�[�gDB�ւ̃��O�C���p�X���[�h��������Ă���t�@�C��
	c_imageloc varchar(255),					-- ���i�摜��\�����邽�߂̃x�[�X���P�[�V����
	c_purchase_cgi varchar(255)					-- �����ɓ���邽�߂�CGI
) TYPE = InnoDB;
insert into sy_cartinfo values (
	'localhost',
	'pib_#USERID#',
	'#USERDIR#/data/e-commex/pass.dat',
	'/e-commex/images',
	'/e-commex/cgi-bin/ex_purchase_item.cgi'
);

drop table if exists sy_iteminfo;
create table sy_iteminfo(
	n_iteminfo_id integer not null,				-- ���i���\�����@ID
	c_iteminfo_name varchar(32),				-- ���i���\�����@
	constraint ping_pkey primary key (n_iteminfo_id)
) TYPE = InnoDB;
insert into sy_iteminfo values (1, '�������i');
insert into sy_iteminfo values (2, '�V�����i');
insert into sy_iteminfo values (3, '�l�C���i');

drop table if exists sy_theme_category;
create table sy_theme_category(
	n_category_id integer not null,				-- �J�e�S��ID
	c_category_title varchar(32),				-- �J�e�S���^�C�g��
	c_theme_thumbnail varchar(255)				-- �J�e�S���T���l�C��
) TYPE = InnoDB;
insert into sy_theme_category values( 1, '�x�r�[�p�i', 'baby.gif');
insert into sy_theme_category values( 2, '���e�E���N', 'beauty.gif');
insert into sy_theme_category values( 3, '���y�E�f���E�{', 'books.gif');
insert into sy_theme_category values( 4, '�Ɠd�E�d�C', 'electro.gif');
insert into sy_theme_category values( 5, '�t�@�b�V����', 'fashion.gif');
insert into sy_theme_category values( 6, '�ԁE�M�t�g', 'flower.gif');
insert into sy_theme_category values( 7, '�H�i�E����', 'food.gif');
insert into sy_theme_category values( 8, '��������E�Q�[��', 'games.gif');
insert into sy_theme_category values( 9, '��E��y', 'hobby.gif');
insert into sy_theme_category values(10, '�ƁE�}���V����', 'housing.gif');
insert into sy_theme_category values(11, '�C���e���A�E�G��', 'interior.gif');
insert into sy_theme_category values(12, '��ÁE����', 'medical.gif');
insert into sy_theme_category values(13, '�I�t�B�X�p�i', 'office.gif');
insert into sy_theme_category values(14, '�y�b�g�p�i', 'pet.gif');
insert into sy_theme_category values(15, '�X�|�[�c', 'sports.gif');
insert into sy_theme_category values(16, '�X�C�[�c�E�a�َq', 'sweets.gif');

alter table sy_theme add n_category_id integer;
alter table sy_theme drop c_theme_image;
alter table sy_theme drop primary key;
update sy_theme set n_category_id = (n_theme_id + 4) / 5;
update sy_theme set n_theme_id = n_theme_id % 5;
alter table sy_theme add primary key(n_category_id, n_theme_id);

alter table at_looks add n_category_id integer;
update at_looks set n_category_id = (n_theme_id + 4) / 5;
update at_looks set n_theme_id = n_theme_id % 5;
