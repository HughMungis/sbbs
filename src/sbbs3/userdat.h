/* userdat.h */

/* Synchronet user data access routines (exported) */

/* $Id$ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2000 Rob Swindell - http://www.synchro.net/copyright.html		*
 *																			*
 * This program is free software; you can redistribute it and/or			*
 * modify it under the terms of the GNU General Public License				*
 * as published by the Free Software Foundation; either version 2			*
 * of the License, or (at your option) any later version.					*
 * See the GNU General Public License for more details: gpl.txt or			*
 * http://www.fsf.org/copyleft/gpl.html										*
 *																			*
 * Anonymous FTP access to the most recent released source is available at	*
 * ftp://vert.synchro.net, ftp://cvs.synchro.net and ftp://ftp.synchro.net	*
 *																			*
 * Anonymous CVS access to the development source and modification history	*
 * is available at cvs.synchro.net:/cvsroot/sbbs, example:					*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs login			*
 *     (just hit return, no password is necessary)							*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs checkout src		*
 *																			*
 * For Synchronet coding style and modification guidelines, see				*
 * http://www.synchro.net/source.html										*
 *																			*
 * You are encouraged to submit any modifications (preferably in Unix diff	*
 * format) via e-mail to mods@synchro.net									*
 *																			*
 * Note: If this box doesn't appear square, then you need to fix your tabs.	*
 ****************************************************************************/

#ifndef _USERDAT_H
#define _USERDAT_H

#include "scfgdefs.h"   /* scfg_t */

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif
#ifdef DLLCALL
#undef DLLCALL
#endif

#ifdef _WIN32
	#ifdef SBBS_EXPORTS
		#define DLLEXPORT __declspec(dllexport)
	#else
		#define DLLEXPORT __declspec(dllimport)
	#endif
	#ifdef __BORLANDC__
		#define DLLCALL __stdcall
	#else
		#define DLLCALL
	#endif
#else
	#define DLLEXPORT
	#define DLLCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern char* crlf;
extern char* nulstr;

DLLEXPORT int	DLLCALL getuserdat(scfg_t* cfg, user_t* user); 	/* Fill userdat struct with user data   */
DLLEXPORT int	DLLCALL putuserdat(scfg_t* cfg, user_t* user);	/* Put userdat struct into user file	*/
DLLEXPORT void	DLLCALL getrec(char *instr,int start,int length,char *outstr); /* Retrieve a record from a string */
DLLEXPORT void	DLLCALL putrec(char *outstr,int start,int length,char *instr); /* Place a record into a string */
DLLEXPORT uint	DLLCALL matchuser(scfg_t* cfg, char *str); /* Checks for a username match */
DLLEXPORT uint	DLLCALL lastuser(scfg_t* cfg);
DLLEXPORT char	DLLCALL getage(scfg_t* cfg, char *birthdate);
DLLEXPORT char*	DLLCALL username(scfg_t* cfg, int usernumber, char * str);
DLLEXPORT int	DLLCALL getnodedat(scfg_t* cfg, uint number, node_t *node, char lockit);
DLLEXPORT int	DLLCALL putnodedat(scfg_t* cfg, uint number, node_t *node);
DLLEXPORT uint	DLLCALL userdatdupe(scfg_t* cfg, uint usernumber, uint offset, uint datlen, char *dat
							,BOOL del);

DLLEXPORT BOOL	DLLCALL chk_ar(scfg_t* cfg, uchar* str, user_t* user); /* checks access requirements */

DLLEXPORT int	DLLCALL getuserrec(scfg_t*, int usernumber, int start, int length, char *str);
DLLEXPORT int	DLLCALL putuserrec(scfg_t*, int usernumber, int start, uint length, char *str);
DLLEXPORT ulong	DLLCALL adjustuserrec(scfg_t*, int usernumber, int start, int length, long adj);
DLLEXPORT void	DLLCALL subtract_cdt(scfg_t*, user_t*, long amt);

#ifdef __cplusplus
}
#endif

#endif