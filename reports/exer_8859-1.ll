/*
 * @progname       exer_8859-1
 * @version        0.02 (2002/07/24)
 * @author         Perry Rapp
 
 * @category       test

 * @output         mixed

 * @description    8859-1 subset of exercise.ll tests

*/


char_encoding("ISO-8859-1") 

require("lifelines-reports.version:1.3")
option("explicitvars") /* Disallow use of undefined variables */

proc finnish_8859_1()
{
	if (not(set_and_check_locale("fi_FI", "Finnish"))) {
		return()
	}
	/* sanity check */
	call check_collate3("A", "L", "Z")
	/* Adia sorts between Z and Odia */
	call check_collate3("Z", "�:[Adia]", "�:[Odia]")
	/* ydia & udia sort as y */
	call check_collate3("x", "y", "z")
	call check_collate3("x", "�:[ydia]", "z")
	call check_collate3("x", "�:[udia]", "z")
	/* eth (lower=u00F0) sorts as d */
	call check_collate3("c", "d", "e")
	call check_collate3("c", "�:[eth]", "e")
}
proc spanish_8859_1()
{
	if (not(set_and_check_locale("es", "Spanish"))) {
		return()
	}
	call check_collate3("A", "N", "Z")
	call check_collate3("N", "�:[Ntilde]", "O")
}
proc testCollate_8859_1()
{
	call finnish_8859_1()
	call spanish_8859_1()
}


