/* 
   Copyright (c) 1991-1999 Thomas T. Wetmore IV

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
/* modified 05 Jan 2000 by Paul B. McBride (pmcbride@tiac.net) */
/*=============================================================
 * edit.c -- Edit person or family record
 * Copyright(c) 1992-96 by T.T. Wetmore IV; all rights reserved
 *   2.3.4 - 24 Jun 93    2.3.5 - 01 Sep 93
 *   3.0.0 - 23 Sep 94    3.0.2 - 12 Dec 94
 *   3.0.3 - 15 Feb 96
 *===========================================================*/

#include "llstdlib.h"
#include "table.h"
#include "translat.h"
#include "gedcom.h"
#include "indiseq.h"
#include "liflines.h"
#include "feedback.h"

#include "llinesi.h"

extern STRING iredit, fredit, cfpupt, cffupt, idpedt, idspse, idfbys;
extern STRING ntprnt, gdpmod, gdfmod, ronlye;

/*=====================================
 * write_indi_to_editfile - write indi gedcom node to editfile
 *===================================*/
void
write_indi_to_editfile(NODE indi)
{
	FILE *fp;
	TRANTABLE tto = tran_tables[MINED];
	NODE name, refn, sex, body, famc, fams;
	
	ASSERT(fp = fopen(editfile, LLWRITETEXT));
	split_indi_old(indi, &name, &refn, &sex, &body, &famc, &fams);
	write_nodes(0, fp, tto, indi, TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, name, TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, refn, TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, sex,   TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, body , TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, famc,  TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, fams,  TRUE, TRUE, TRUE);
	fclose(fp);
	join_indi(indi, name, refn, sex, body, famc, fams);
}
/*=====================================
 * edit_indi -- Edit person in database
 *===================================*/
NODE
edit_indi (NODE indi1)  /* may be NULL */
{
	NODE indi2=0, name1, name2, refn1, refn2, sex, body, famc, fams;
	NODE node, namen, refnn, name1n, refn1n, indi0;
	BOOLEAN emp;
	STRING msg, key;
	TRANTABLE tti = tran_tables[MEDIN];
	TRANTABLE ttd = tran_tables[MINDS];

	if (!indi1 && !(indi1 = ask_for_indi_old(idpedt, NOCONFIRM, NOASK1)))
		return NULL;

/* Prepare file for user to edit */
	write_indi_to_editfile(indi1);

/* Have user edit file */

	do_edit();
	if (readonly) {
		indi2 = file_to_node(editfile, tti, &msg, &emp);
		if (!equal_tree(indi1, indi2))
			message(_(ronlye));
		free_nodes(indi2);
		return indi1;
	}
	while (TRUE) {
		indi2 = file_to_node(editfile, tti, &msg, &emp);
		if (!indi2) {
			if (ask_yes_or_no_msg(msg, iredit)) {
				do_edit();
				continue;
			} 
			break;
		}
		if (!valid_indi_old(indi2, &msg, indi1)) {
			if (ask_yes_or_no_msg(msg, iredit)) {
				do_edit();
				continue;
			}
			free_nodes(indi2);
			indi2 = NULL;
			break;
		}
		break;
	}

/* Editing done; see if database changes */

	if (!indi2) return indi1;
	if (equal_tree(indi1, indi2) || !ask_yes_or_no(_(cfpupt))) {
		free_nodes(indi2);
		return indi1;
	}

/* Prepare to change database */

	split_indi_old(indi1, &name1, &refn1, &sex, &body, &famc, &fams);
	indi0 = copy_node(indi1);
	join_indi(indi0, NULL, NULL, sex, body, famc, fams);
	free_nodes(indi0);
	split_indi_old(indi2, &name2, &refn2, &sex, &body, &famc, &fams);
	namen = copy_nodes(name2, TRUE, TRUE);
	refnn = copy_nodes(refn2, TRUE, TRUE);
	join_indi(indi1, name2, refn2, sex, body, famc, fams);
	free_node(indi2);

/* Write changed person to database */

	resolve_links(indi1);
	indi_to_dbase(indi1);
	key = rmvat(nxref(indi1));
	classify_nodes(&name1, &namen, &name1n);
	classify_nodes(&refn1, &refnn, &refn1n);
	for (node = name1; node; node = nsibling(node))
		remove_name(nval(node), key);
	for (node = namen; node; node = nsibling(node))
		add_name(nval(node), key);
	rename_from_browse_lists(key);
	for (node = refn1; node; node = nsibling(node))
		if (nval(node)) remove_refn(nval(node), key);
	for (node = refnn; node; node = nsibling(node))
		if (nval(node)) add_refn(nval(node), key);
	free_nodes(name1);
	free_nodes(namen);
	free_nodes(name1n);
	free_nodes(refn1);
	free_nodes(refnn);
	free_nodes(refn1n);
	msg_status(gdpmod, indi_to_name(indi1, ttd, 35));
	return indi1;
}
/*=====================================
 * write fam gedcom node to editfile
 *===================================*/
void
write_fam_to_editfile(NODE fam)
{
	FILE *fp;
	TRANTABLE tto = tran_tables[MINED];
	NODE refn, husb, wife, chil, body;

	ASSERT(fp = fopen(editfile, LLWRITETEXT));
	split_fam(fam, &refn, &husb, &wife, &chil, &body);
	write_nodes(0, fp, tto, fam,  TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, refn, TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, husb,  TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, wife,  TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, body,  TRUE, TRUE, TRUE);
	write_nodes(1, fp, tto, chil,  TRUE, TRUE, TRUE);
	join_fam(fam, refn, husb, wife, chil, body);
	fclose(fp);
}
/*====================================
 * edit_fam -- Edit family in database
 *==================================*/
NODE
edit_family (NODE fam1) /* may be NULL */
{
	NODE fam2=0, husb, wife, chil, body, refn1, refn2, refnn, refn1n;
	NODE indi, node, fam0;
	TRANTABLE tti = tran_tables[MEDIN];
	STRING msg, key;
	BOOLEAN emp;
/* Identify family if need be */
	if (!fam1) {
		indi = ask_for_indi_old(idspse, NOCONFIRM, NOASK1);
		if (!indi) return NULL;
		if (!FAMS(indi)) {
			message(ntprnt);
			return NULL;
		} 
		fam1 = choose_family(indi, "e", idfbys, TRUE);
		if (!fam1) return FALSE; 
	}

/* Prepare file for user to edit */
	write_fam_to_editfile(fam1);

/* Have user edit record */
	do_edit();
	if (readonly) {
		fam2 = file_to_node(editfile, tti, &msg, &emp);
		if (!equal_tree(fam1, fam2))
			message(_(ronlye));
		free_nodes(fam2); 
		return fam1;
	}
	while (TRUE) {
		fam2 = file_to_node(editfile, tti, &msg, &emp);
		if (!fam2) {
			if (ask_yes_or_no_msg(msg, fredit)) {
				do_edit();
				continue;
			}
			break;
		}
		if (!valid_fam_old(fam2, &msg, fam1)) {
			if (ask_yes_or_no_msg(msg, fredit)) {
				do_edit();
				continue;
			}
			free_nodes(fam2);
			fam2 = NULL;
			break;
		}
		break;
	}

/* If error or user backs out return */

	if (!fam2) return fam1;
	if (equal_tree(fam1, fam2) || !ask_yes_or_no(_(cffupt))) {
		free_nodes(fam2);
		return fam1;
	}

/* Prepare to change database */

	split_fam(fam1, &refn1, &husb, &wife, &chil, &body);
	fam0 = copy_node(fam1);
	join_fam(fam0, NULL, husb, wife, chil, body);
	free_nodes(fam0);
	split_fam(fam2, &refn2, &husb, &wife, &chil, &body);
	refnn = copy_nodes(refn2, TRUE, TRUE);
	join_fam(fam1, refn2, husb, wife, chil, body);
	free_node(fam2);

/* Change the database */

	resolve_links(fam1);
	fam_to_dbase(fam1);
	key = rmvat(nxref(fam1));
	classify_nodes(&refn1, &refnn, &refn1n);
	for (node = refn1; node; node = nsibling(node))
		if (nval(node)) remove_refn(nval(node), key);
	for (node = refnn; node; node = nsibling(node))
		if (nval(node)) add_refn(nval(node), key);
	free_nodes(refn1);
	free_nodes(refnn);
	free_nodes(refn1n);
	msg_status(gdfmod);
	return fam1;
}
