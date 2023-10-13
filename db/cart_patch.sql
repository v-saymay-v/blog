insert into sy_sidecontent values(15, 13, 15, '商品情報', 1, 1);
insert into sy_sidecontent values(16, 14, 16, 'お勧め記事', 1, 1);
insert into sy_sidecontent values(17, 15, 17, 'カートログイン', 1, 1);
insert into sy_sidecontent values(18, 16, 18, '会員登録', 1, 1);
insert into at_sidecontent select n_blog_id, 15, 13, 15, '商品情報', 1, 1 from at_blog;
insert into at_sidecontent select n_blog_id, 16, 14, 16, 'お勧め記事', 1, 1 from at_blog;
insert into at_sidecontent select n_blog_id, 17, 15, 17, 'カートログイン', 1, 1 from at_blog;
insert into at_sidecontent select n_blog_id, 18, 16, 18, '会員登録', 1, 1 from at_blog;

alter table sy_authinfo add c_passfile varchar(255);
alter table sy_baseinfo add b_cartmode bool;
alter table sy_baseinfo add b_hbmode bool;
alter table sy_baseinfo add b_hostingmode bool;
update sy_baseinfo set c_systemname = "店長ブログ";
alter table at_blog add c_blog_id varchar(20);
alter table at_blog add n_hbuser_id integer default 0;
alter table at_blog add n_iteminfo_id integer default 1;
alter table at_blog add n_iteminfo_num integer default 3;
alter table at_blog add c_mail_subject varchar(255) default '%b　更新通知';
alter table at_blog add c_mail_body text;
update at_blog set c_mail_body = '%c　様\r\nブログを更新しましたのでお知らせします。下記URLよりアクセスしてください。\r\n%u\r\n';
alter table at_entry add c_item_id varchar(64);

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

drop table if exists sy_iteminfo;
create table sy_iteminfo(
	n_iteminfo_id integer not null,				-- 商品情報表示方法ID
	c_iteminfo_name varchar(32),				-- 商品情報表示方法
	constraint ping_pkey primary key (n_iteminfo_id)
) TYPE = InnoDB;
insert into sy_iteminfo values (1, '特売商品');
insert into sy_iteminfo values (2, '新着商品');
insert into sy_iteminfo values (3, '人気商品');

drop table if exists sy_theme_category;
create table sy_theme_category(
	n_category_id integer not null,				-- カテゴリID
	c_category_title varchar(32),				-- カテゴリタイトル
	c_theme_thumbnail varchar(255)				-- カテゴリサムネイル
) TYPE = InnoDB;
insert into sy_theme_category values( 1, 'ベビー用品', 'baby.gif');
insert into sy_theme_category values( 2, '美容・健康', 'beauty.gif');
insert into sy_theme_category values( 3, '音楽・映像・本', 'books.gif');
insert into sy_theme_category values( 4, '家電・電気', 'electro.gif');
insert into sy_theme_category values( 5, 'ファッション', 'fashion.gif');
insert into sy_theme_category values( 6, '花・ギフト', 'flower.gif');
insert into sy_theme_category values( 7, '食品・飲料', 'food.gif');
insert into sy_theme_category values( 8, 'おもちゃ・ゲーム', 'games.gif');
insert into sy_theme_category values( 9, '趣味・娯楽', 'hobby.gif');
insert into sy_theme_category values(10, '家・マンション', 'housing.gif');
insert into sy_theme_category values(11, 'インテリア・雑貨', 'interior.gif');
insert into sy_theme_category values(12, '医療・福祉', 'medical.gif');
insert into sy_theme_category values(13, 'オフィス用品', 'office.gif');
insert into sy_theme_category values(14, 'ペット用品', 'pet.gif');
insert into sy_theme_category values(15, 'スポーツ', 'sports.gif');
insert into sy_theme_category values(16, 'スイーツ・和菓子', 'sweets.gif');

alter table sy_theme add n_category_id integer;
alter table sy_theme drop c_theme_image;
alter table sy_theme drop primary key;
update sy_theme set n_category_id = (n_theme_id + 4) / 5;
update sy_theme set n_theme_id = n_theme_id % 5;
alter table sy_theme add primary key(n_category_id, n_theme_id);

alter table at_looks add n_category_id integer;
update at_looks set n_category_id = (n_theme_id + 4) / 5;
update at_looks set n_theme_id = n_theme_id % 5;
