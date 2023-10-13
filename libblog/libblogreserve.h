#ifndef __LIBBLOGRESERVE_H__
#define __LIBBLOGRESERVE_H__

#include "libcgi2.h"
#include "libmysql.h"

DBase* Open_Reserve_Db(DBase *db, NLIST *nlp_out);
int Build_Reserve_Combo(DBase *db, NLIST *nlp_out, char *chp_target, char *chp_name, char *chp_select, int in_blog, int in_owner);
int Build_Reserve_Item(DBase *db, NLIST *nlp_in, NLIST *nlp_out, char *chp_item_id, char *chp_target);
int Get_Blog_Priv(DBase *db, NLIST *nlp_out, int in_owner);
int Get_Sample_Mode(DBase *db, NLIST *nlp_out);

#endif
