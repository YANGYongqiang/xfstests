/*
 * Copyright (c) 2000-2001 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <lib/hsm.h>

#include <getopt.h>
#include <string.h>


/*---------------------------------------------------------------------------

Test program used to test the DMAPI function dm_probe_hole().  The
command line is:

	probe_hole [-o offset] [-l length] [-s sid] pathname

where pathname is the name of a file, offset is the offset of the start of
the proposed punch, and length is the length of the punch.  sid is the
session ID whose events you you are interested in.

----------------------------------------------------------------------------*/

#ifndef linux
extern	char	*sys_errlist[];
#endif
extern  int     optind;
extern  char    *optarg;


char	*Progname;


static void
usage(void)
{
	fprintf(stderr, "usage:\t%s [-o offset] [-l length] "
		"[-s sid] pathname\n", Progname);
	exit(1);
}


int
main(
	int	argc, 
	char	**argv)
{
	dm_sessid_t	sid = DM_NO_SESSION;
	char		*pathname = NULL;
	dm_off_t	offset = 0;
	dm_size_t	length = 0;
	dm_off_t	roffp;
	dm_size_t	rlenp;
	void		*hanp;
	size_t	 	hlen;
	char		*name;
	int		opt;

	if (Progname = strrchr(argv[0], '/')) {
		Progname++;
	} else {
		Progname = argv[0];
	}

	/* Crack and validate the command line options. */

	while ((opt = getopt(argc, argv, "o:l:s:")) != EOF) {
		switch (opt) {
		case 'o':
			offset = atol(optarg);
			break;
		case 'l':
			length = atol(optarg);
			break;
		case 's':
			sid = atol(optarg);
			break;
		case '?':
			usage();
		}
	}
	if (optind + 1 != argc)
		usage();
	pathname = argv[optind];

	if (dm_init_service(&name) == -1)  {
		fprintf(stderr, "Can't initialize the DMAPI\n");
		exit(1);
	}
	if (sid == DM_NO_SESSION)
		find_test_session(&sid);

	/* Get the file's handle. */

	if (dm_path_to_handle(pathname, &hanp, &hlen)) {
		fprintf(stderr, "can't get handle for file %s\n", pathname);
		exit(1);
	}

	if (dm_probe_hole(sid, hanp, hlen, DM_NO_TOKEN, offset, length,
	    &roffp, &rlenp)) {
		fprintf(stderr, "dm_probe_hole failed, %s\n",
			strerror(errno));
		exit(1);
	}
	fprintf(stdout, "roffp is %lld, rlenp is %lld\n", roffp, rlenp);
	dm_handle_free(hanp, hlen);
	exit(0);
}
