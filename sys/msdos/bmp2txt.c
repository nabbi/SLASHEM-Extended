/*   SCCS Id: @(#)bmp2txt.c   3.3     95/01/26                     */
/*   Copyright (c) NetHack PC Development Team 1993, 1994, 1995     */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * This creates a bmp2txt.exe
 *
 * This takes a big tile file (generated by txt2bmp -bXX or idx2bmp)
 * and converts it back into the nethack .txt format
 * Uses 'index' to generate the associated names
 *
 * Edit History:
 *
 *      Initial Creation                        W.Cheung	00/06/23
 */

#define alleg_mouse_unused
#define alleg_timer_unused
#define alleg_keyboard_unused
#define alleg_joystick_unused
#define alleg_sound_unused
#define alleg_gui_unused

#include <allegro.h>

#include "hack.h"
#include "pcvideo.h"
#include "tile.h"
#include "pctiles.h"

#include <ctype.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>

extern char *tilename(int, int);

static const char
    *output_file = "tiles.txt",
    *index_file = "index",
    *input_file = "tiles.bmp";

/* These next two functions were stolen from hacklib.c */

char *
eos(s)			/* return the end of a string (pointing at '\0') */
    register char *s;
{
    while (*s) s++;	/* s += strlen(s); */
    return s;
}

/* remove excess whitespace from a string buffer (in place) */
char *
mungspaces(bp)
char *bp;
{
    register char c, *p, *p2;
    boolean was_space = TRUE;

    for (p = p2 = bp; (c = *p) != '\0'; p++) {
	if (c == '\t') c = ' ';
	if (c != ' ' || !was_space) *p2++ = c;
	was_space = (c == ' ');
    }
    if (was_space && p2 > bp) p2--;
    *p2 = '\0';
    return bp;
}



int
main(argc, argv)
int argc;
char *argv[];
{
        int i;
        FILE *fp;
        char            buf[BUFSZ];
        char            *bufp;
        BITMAP *bigtile_bmp;
        BITMAP *tilebmp;
        RGB *bmp_pal[256];
        struct tm *newtime;
        time_t aclock;
        char filename[60];
        char tilename[BUFSZ];
        int tile_x = 32, tile_y = 32;
        int col, row;
        boolean has_index = 0, has_output = 0;

        if (argc > 3) {        	
	    	fprintf(stderr, "Bad arg count (%d).\n", argc-1);
	    	(void) fflush(stderr);
                exit(EXIT_FAILURE);
        }
        has_index = (argc > 1);
        has_output = (argc > 2);

        time(&aclock);
        newtime = localtime(&aclock);
             
        /* Open the index file */
        
	if (has_index) sprintf(filename, argv[1]);
	else sprintf(filename, index_file);
	
        if ((fp = fopen(filename, "r")) == (FILE *)0)
        {
                fprintf(stderr, "Could not open index file '%s'!\n", filename);
	        exit(EXIT_FAILURE);
        }

        
        set_color_depth(24);

        sprintf(filename, input_file);
	bigtile_bmp = load_bitmap(filename, bmp_pal);	
        if (!bigtile_bmp)
        {
                fprintf(stderr, "Could not open bitmap file '%s'!\n", filename);
	        exit(EXIT_FAILURE);
        }
              
        /* Deal with the palette - bmp is truecolor, 
         * This code assumes that txt files need to have < 256 colors         
         * HOW DO WE HANDLE TRANSPARENT TILES?
         */
	
        fprintf(stderr, "Generating rgb lookup table
        create_rgb_table(rgb_map, bmp_pal, NULL);
        
        i = 0;

        while(fgets(buf,120,fp))
        {
                if (*buf == '#')
                        continue;

                bufp = eos(buf);
                while (--bufp > buf && isspace(*bufp))
                        continue;

                if (bufp <= buf)
                        continue;               /* skip all-blank lines */
                else
                        *(bufp + 1) = '\0';     /* terminate line */

                /* find the '=' or ':' */
                bufp = index(buf, ':');
                if (!bufp)
                	continue;

                *bufp = '\0'; /* we only want everything before the : */

		/* #### (tile name) */
                sscanf (buf, "%*4c (%[^)])", tilename);
                fprintf(stderr, "# tile %d (%s)\n", i, tilename);


	    	col = (int)(i % TILES_PER_ROW);
		row = (int)(i / TILES_PER_ROW);
	                
                i++;
        }

        exit(EXIT_SUCCESS);
        /*NOTREACHED*/
        return 0;
}

