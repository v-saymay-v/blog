#!/bin/sh
if /bin/test -e $1/data/as-blog/blogdb
then
	exit
fi
sourceuser=sekine
/bin/mkdir $1/data/as-blog
/bin/mkdir $1/data/as-blog/images
/bin/mkdir $1/data/as-blog/data
/bin/mkdir $1/data/as-blog/attach
/bin/mkdir $1/data/as-blog/skeleton
/bin/mkdir $1/data/as-blog/css
/bin/mkdir $1/data/as-blog/rss
/usr/local/bin/sqlite $1/data/as-blog/blogdb < /home/staff/${sourceuser}/work/blog/db/create_table.sql
/bin/cp /home/staff/${sourceuser}/work/blog/skeleton/user/blog_error.skl $1/data/as-blog/skeleton/
/bin/cp /home/staff/${sourceuser}/work/blog/skeleton/user/blog_usertop_right.skl $1/data/as-blog/skeleton/
/bin/cp /home/staff/${sourceuser}/work/blog/skeleton/user/blog_usertop_left.skl $1/data/as-blog/skeleton/
/bin/cp /home/staff/${sourceuser}/work/blog/skeleton/user/blog_article.skl $1/data/as-blog/skeleton/
/bin/cp /home/staff/${sourceuser}/work/blog/skeleton/user/blog_article_list.skl $1/data/as-blog/skeleton/
/bin/cp /home/staff/${sourceuser}/work/blog/images/transparent.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_01_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_12.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_02_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_03_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_04_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_05_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_06_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_07_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_08.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_08.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_08_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_09.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_09.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_09_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_10.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_10.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_10_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_11.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_11.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_11_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_12.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_12.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_12_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_13.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_13.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_13_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_14.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_14.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_14_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_15.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_15.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_15_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_16.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_16.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_16_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_17.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_17.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_17_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_17_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_17_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_17_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_17_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_17_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_18.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_18.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_18_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_18_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_18_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_18_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_18_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_18_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_19.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_19.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_19_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_19_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_19_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_19_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_19_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_19_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/bg_20.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/title_bg_20.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_01.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_02.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_03.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_04.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_05.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_06.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_07.gif $1/data/as-blog/images/
/bin/cp /home/staff/${sourceuser}/work/blog/images/theme/asb_20_08.gif $1/data/as-blog/images/

/bin/cp /home/staff/${sourceuser}/work/blog/css/theme001.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme002.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme003.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme004.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme005.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme006.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme007.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme008.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme009.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme010.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme011.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme012.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme013.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme014.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme015.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme016.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme017.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme018.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme019.css $1/data/as-blog/css/
/bin/cp /home/staff/${sourceuser}/work/blog/css/theme020.css $1/data/as-blog/css/

/bin/chmod 755 $1/data/as-blog
/bin/chmod 755 $1/data/as-blog/images
/bin/chmod 755 $1/data/as-blog/data
/bin/chmod 755 $1/data/as-blog/attach
/bin/chmod 755 $1/data/as-blog/skeleton
/bin/chmod 755 $1/data/as-blog/css
/bin/chmod 755 $1/data/as-blog/rss
/bin/chmod 644 $1/data/as-blog/blogdb
/bin/chmod 644 $1/data/as-blog/skeleton/blog_error.skl
/bin/chmod 644 $1/data/as-blog/skeleton/blog_article.skl
/bin/chmod 644 $1/data/as-blog/skeleton/blog_article_list.skl
/bin/chmod 644 $1/data/as-blog/skeleton/blog_usertop_right.skl
/bin/chmod 644 $1/data/as-blog/skeleton/blog_usertop_left.skl
/bin/chmod 644 $1/data/as-blog/images/transparent.gif

/bin/chmod 644 $1/data/as-blog/images/bg_01.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_01_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_02.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_02_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_03.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_03_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_04.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_04_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_05.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_05_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_06.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_06_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_07.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_07_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_08.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_08.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_08_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_09.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_09.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_09_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_10.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_10.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_10_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_11.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_11.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_11_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_12.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_12.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_12_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_13.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_13.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_13_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_14.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_14.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_14_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_15.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_15.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_15_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_16.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_16.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_16_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_17.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_17.gif
/bin/chmod 644 $1/data/as-blog/images/asb_17_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_17_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_17_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_17_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_17_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_17_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_18.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_18.gif
/bin/chmod 644 $1/data/as-blog/images/asb_18_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_18_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_18_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_18_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_18_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_18_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_19.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_19.gif
/bin/chmod 644 $1/data/as-blog/images/asb_19_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_19_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_19_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_19_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_19_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_19_08.gif

/bin/chmod 644 $1/data/as-blog/images/bg_20.gif
/bin/chmod 644 $1/data/as-blog/images/title_bg_20.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_01.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_02.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_03.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_04.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_05.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_06.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_07.gif
/bin/chmod 644 $1/data/as-blog/images/asb_20_08.gif

/bin/chmod 644 $1/data/as-blog/css/theme001.css
/bin/chmod 644 $1/data/as-blog/css/theme002.css
/bin/chmod 644 $1/data/as-blog/css/theme003.css
/bin/chmod 644 $1/data/as-blog/css/theme004.css
/bin/chmod 644 $1/data/as-blog/css/theme005.css
/bin/chmod 644 $1/data/as-blog/css/theme006.css
/bin/chmod 644 $1/data/as-blog/css/theme007.css
/bin/chmod 644 $1/data/as-blog/css/theme008.css
/bin/chmod 644 $1/data/as-blog/css/theme009.css
/bin/chmod 644 $1/data/as-blog/css/theme010.css
/bin/chmod 644 $1/data/as-blog/css/theme011.css
/bin/chmod 644 $1/data/as-blog/css/theme012.css
/bin/chmod 644 $1/data/as-blog/css/theme013.css
/bin/chmod 644 $1/data/as-blog/css/theme014.css
/bin/chmod 644 $1/data/as-blog/css/theme015.css
/bin/chmod 644 $1/data/as-blog/css/theme016.css
/bin/chmod 644 $1/data/as-blog/css/theme017.css
/bin/chmod 644 $1/data/as-blog/css/theme018.css
/bin/chmod 644 $1/data/as-blog/css/theme019.css
/bin/chmod 644 $1/data/as-blog/css/theme020.css

/usr/sbin/chown $2 $1/data/as-blog
/usr/sbin/chown $2 $1/data/as-blog/images
/usr/sbin/chown $2 $1/data/as-blog/data
/usr/sbin/chown $2 $1/data/as-blog/attach
/usr/sbin/chown $2 $1/data/as-blog/skeleton
/usr/sbin/chown $2 $1/data/as-blog/css
/usr/sbin/chown $2 $1/data/as-blog/rss
/usr/sbin/chown $2 $1/data/as-blog/blogdb
/usr/sbin/chown $2 $1/data/as-blog/skeleton/blog_error.skl
/usr/sbin/chown $2 $1/data/as-blog/skeleton/blog_article.skl
/usr/sbin/chown $2 $1/data/as-blog/skeleton/blog_article_list.skl
/usr/sbin/chown $2 $1/data/as-blog/skeleton/blog_usertop_right.skl
/usr/sbin/chown $2 $1/data/as-blog/skeleton/blog_usertop_left.skl
/usr/sbin/chown $2 $1/data/as-blog/images/transparent.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_01_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_03_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_04_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_05_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_06_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_07_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_08.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_08.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_08_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_09.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_09.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_09_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_10.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_10.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_10_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_11.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_11.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_11_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_12.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_12.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_12_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_13.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_13.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_13_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_14.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_14.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_14_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_15.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_15.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_15_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_16.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_16.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_16_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_17.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_17.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_17_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_17_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_17_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_17_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_17_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_17_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_18.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_18.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_18_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_18_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_18_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_18_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_18_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_18_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_19.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_19.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_19_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_19_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_19_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_19_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_19_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_19_08.gif

/usr/sbin/chown $2 $1/data/as-blog/images/bg_20.gif
/usr/sbin/chown $2 $1/data/as-blog/images/title_bg_20.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_20_01.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_20_02.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_20_03.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_20_04.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_20_05.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_20_06.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_02_07.gif
/usr/sbin/chown $2 $1/data/as-blog/images/asb_20_08.gif

/usr/sbin/chown $2 $1/data/as-blog/css/theme001.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme002.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme003.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme004.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme005.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme006.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme007.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme008.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme009.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme010.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme011.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme012.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme013.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme014.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme015.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme016.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme017.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme018.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme019.css
/usr/sbin/chown $2 $1/data/as-blog/css/theme020.css

/usr/bin/chflags sunlnk $1/data/as-blog
/usr/bin/chflags sunlnk $1/data/as-blog/images
/usr/bin/chflags sunlnk $1/data/as-blog/data
/usr/bin/chflags sunlnk $1/data/as-blog/attach
/usr/bin/chflags sunlnk $1/data/as-blog/skeleton
/usr/bin/chflags sunlnk $1/data/as-blog/css
/usr/bin/chflags sunlnk $1/data/as-blog/rss
/usr/bin/chflags sunlnk $1/data/as-blog/blogdb
/usr/bin/chflags sunlnk $1/data/as-blog/skeleton/blog_error.skl
/usr/bin/chflags sunlnk $1/data/as-blog/skeleton/blog_article.skl
/usr/bin/chflags sunlnk $1/data/as-blog/skeleton/blog_article_list.skl
/usr/bin/chflags sunlnk $1/data/as-blog/skeleton/blog_usertop_right.skl
/usr/bin/chflags sunlnk $1/data/as-blog/skeleton/blog_usertop_left.skl
/usr/bin/chflags sunlnk $1/data/as-blog/images/transparent.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_01_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_02_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_03_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_04_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_05_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_06_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_07_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_08.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_08.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_08_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_09.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_09.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_09_08.gif

/usr/bin/chflags sunlink $1/data/as-blog/images/bg_10.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/title_bg_10.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_01.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_02.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_03.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_04.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_05.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_06.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_07.gif
/usr/bin/chflags sunlink $1/data/as-blog/images/asb_10_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_11.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_11.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_04.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_05.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_11_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_12.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_12.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_04.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_05.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_12_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_13.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_13.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_04.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_05.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_13_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_14.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_14.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_04.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_05.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_14_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_15.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_15.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_04.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_05.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_15_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_16.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_16.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_04.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_05.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_16_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_17.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_17.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_17_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_17_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_17_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_17_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_17_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_17_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_18.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_18.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_18_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_18_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_18_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_18_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_18_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_18_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_19.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_19.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_19_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_19_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_19_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_19_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_19_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_19_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/images/bg_20.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/title_bg_20.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_01.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_02.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_03.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_04.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_05.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_06.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_07.gif
/usr/bin/chflags sunlnk $1/data/as-blog/images/asb_20_08.gif

/usr/bin/chflags sunlnk $1/data/as-blog/css/theme001.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme002.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme003.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme004.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme005.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme006.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme007.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme008.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme009.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme010.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme011.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme012.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme013.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme014.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme015.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme016.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme017.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme018.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme019.css
/usr/bin/chflags sunlnk $1/data/as-blog/css/theme020.css

exit
