#!/bin/sh

/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/db/cartex/create_table.sql /usr/local/share/skel/as-blog/as-blog.sql

/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/skeleton/cartex/admin/blog_editentry_buffer.skl /usr/local/apache/share/as-blog/skeleton/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/skeleton/cartex/admin/blog_editentry.skl /usr/local/apache/share/as-blog/skeleton/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/skeleton/cartex/admin/blog_delete_file_ask.skl /usr/local/apache/share/as-blog/skeleton/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/skeleton/cartex/admin/blog_upload_movie.skl /usr/local/apache/share/as-blog/skeleton/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/skeleton/cartex/admin/blog_entry_list.skl /usr/local/apache/share/as-blog/skeleton/
/bin/chmod 644 /usr/local/apache/share/as-blog/skeleton/*.skl

/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/entry/blog_entry.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/commentlist/blog_comment_list.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/blog_ranking/blog_ranking.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/boofmarklet/blog_bookmarklet.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/category/blog_category.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/configure/blog_configure.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/entrylist/blog_entry_list.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/looks/blog_looks.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/menu/blog_menu.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/sidecontent/blog_sidecontent.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/manage/tblist/blog_tb_list.cgi /usr/local/apache/share/e-commex/admin/
/bin/cp /staff/work/dbb_blog/RELENG_4/dbb_blog/user/build_html/blog_build_html.cgi /usr/local/apache/share/e-commex/
