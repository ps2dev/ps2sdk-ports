/*
 * madplay - MPEG audio decoder and player
 * Copyright (C) 2000-2004 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <stdlib.h>

# include "getopt.h"

/* banner output from mpg123 */
/*
High Performance MPEG 1.0/2.0/2.5 Audio Player for Layer 1, 2 and 3.
Version 0.59r (1999/Jun/15). Written and copyrights by Michael Hipp.
Uses code from various people. See 'README' for more!
THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! USE AT YOUR OWN RISK!

*/

/* short usage output from mpg123 */
/*
usage: mpg123 [option(s)] [file(s) | URL(s) | -]
supported options [defaults in brackets]:
   -v    increase verbosity level       -q    quiet (don't print title)
   -t    testmode (no output)           -s    write to stdout
   -w <filename> write Output as WAV file
   -k n  skip first n frames [0]        -n n  decode only n frames [all]
   -c    check range violations         -y    DISABLE resync on errors
   -b n  output buffer: n Kbytes [0]    -f n  change scalefactor [32768]
   -r n  set/force samplerate [auto]    -g n  set audio hardware output gain
   -os,-ol,-oh  output to built-in speaker,line-out connector,headphones
                                        -a d  set audio device
   -2    downsample 1:2 (22 kHz)        -4    downsample 1:4 (11 kHz)
   -d n  play every n'th frame only     -h n  play every frame n times
   -0    decode channel 0 (left) only   -1    decode channel 1 (right) only
   -m    mix both channels (mono)       -p p  use HTTP proxy p [$HTTP_PROXY]
   -@ f  read filenames/URLs from f
   -z    shuffle play (with wildcards)  -Z    random play
   -u a  HTTP authentication string     -E f  Equalizer, data from file
   -C    enable control keys
See the manpage mpg123(1) or call mpg123 with --longhelp for more information.
*/

/* long usage output from mpg123 */
/*
usage: mpg123 [option(s)] [file(s) | URL(s) | -]
supported options:

 -k <n> --skip <n>         
 -a <f> --audiodevice <f>  
 -2     --2to1             2:1 Downsampling
 -4     --4to1             4:1 Downsampling
 -t     --test             
 -s     --stdout           
 -S     --STDOUT           Play AND output stream (not implemented yet)
 -c     --check            
 -v[*]  --verbose          Increase verboselevel
 -q     --quiet            Enables quiet mode
 -y     --resync           DISABLES resync on error
 -0     --left --single0   Play only left channel
 -1     --right --single1  Play only right channel
 -m     --mono --mix       Mix stereo to mono
        --stereo           Duplicate mono channel
        --reopen           Force close/open on audiodevice
 -g     --gain             Set audio hardware output gain
 -r     --rate             Force a specific audio output rate
        --8bit             Force 8 bit output
 -o h   --headphones       Output on headphones
 -o s   --speaker          Output on speaker
 -o l   --lineout          Output to lineout
 -f <n> --scale <n>        Scale output samples (soft gain)
 -n     --frames <n>       Play only <n> frames of every stream
 -b <n> --buffer <n>       Set play buffer ("output cache")
 -d     --doublespeed      Play only every second frame
 -h     --halfspeed        Play every frame twice
 -p <f> --proxy <f>        Set WWW proxy
 -@ <f> --list <f>         Play songs in <f> file-list
 -z     --shuffle          Shuffle song-list before playing
 -Z     --random           full random play
        --equalizer        Exp.: scales freq. bands acrd. to 'equalizer.dat'
        --aggressive       Tries to get higher priority (nice)
 -u     --auth             Set auth values for HTTP access
 -w <f> --wav <f>          Writes samples as WAV file in <f> (- is stdout)
        --au <f>           Writes samples as Sun AU file in <f> (- is stdout)
        --cdr <f>          Writes samples as CDR file in <f> (- is stdout)
 -E <s> --esd <s>          Plays to  ESD server <s> 

See the manpage mpg123(1) for more information.
*/

static
struct option const options[] = {
  { "skip",        required_argument, 0,  'k' },
  { "audiodevice", required_argument, 0,  'a' },
  { "2to1",        no_argument,       0,  '2' },
  { "4to1",        no_argument,       0,  '4' },
  { "test",        no_argument,       0,  't' },
  { "stdout",      no_argument,       0,  's' },
  { "STDOUT",      no_argument,       0,  'S' },
  { "check",       no_argument,       0,  'c' },
  { "verbose",     no_argument,       0,  'v' },
  { "quiet",       no_argument,       0,  'q' },
  { "resync",      no_argument,       0,  'y' },
  { "left",        no_argument,       0,  '0' },
  { "single0",     no_argument,       0,  '0' },
  { "right",       no_argument,       0,  '1' },
  { "single1",     no_argument,       0,  '1' },
  { "mono",        no_argument,       0,  'm' },
  { "mix",         no_argument,       0,  'm' },
  { "stereo",      no_argument,       0, -'s' },
  { "reopen",      no_argument,       0, -'r' },
  { "gain",        required_argument, 0,  'g' },
  { "rate",        required_argument, 0,  'r' },
  { "8bit",        no_argument,       0, -'8' },
  { "headphones",  no_argument,       0,  'o' },
  { "speaker",     no_argument,       0,  'o' },
  { "lineout",     no_argument,       0,  'o' },
  { "scale",       required_argument, 0,  'f' },
  { "frames",      required_argument, 0,  'n' },
  { "buffer",      required_argument, 0,  'b' },
  { "doublespeed", required_argument, 0,  'd' },
  { "halfspeed",   required_argument, 0,  'h' },
  { "proxy",       required_argument, 0,  'p' },
  { "list",        required_argument, 0,  '@' },
  { "shuffle",     no_argument,       0,  'z' },
  { "random",      no_argument,       0,  'Z' },
  { "equalizer",   required_argument, 0,  'E' },
  { "aggressive",  no_argument,       0, -'a' },
  { "auth",        required_argument, 0,  'u' },
  { "wav",         required_argument, 0,  'w' },
  { "au",          required_argument, 0, -'m' },
  { "cdr",         required_argument, 0, -'c' },
  { "esd",         required_argument, 0, -'e' },
  { 0 }
};

static
struct {
  int verbosity;
} config = {
  0	/* verbosity */
};

int main(int argc, char *argv[])
{
  int opt, index;

  while ((opt = getopt_long(argc, argv,
			    "vqtsSw:k:n:cyb:f:r:g:o:a:24d:h:01mp:@:zZu:E:C",
			    options, &index)) != -1) {
    switch (opt) {
    case 'v':
      ++config.verbosity;
      break;

    case 'q':
      config.verbosity = -1;
      break;

    case '?':
      exit(1);
    }
  }

  if (config.verbosity >= 0) {
    fprintf(stderr,
	    "High Quality MPEG 1.0/2.0/2.5 Audio Player"
	    " for Layer I, II, and III.\n"
	    "Version 0.59r (2000/Oct/04)."
	    " Written and copyright by Robert Leslie.\n"
	    "Uses mpg123 command interface. See the documentation!\n"
	    "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY!"
	    " USE AT YOUR OWN RISK!\n");
  }

  /* ... */

  return 0;
}
