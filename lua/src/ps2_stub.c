#include <stdio.h>
#include <stdlib.h>

struct lconv
{
  char *decimal_point;
  char *thousands_sep;
  char *grouping;
  char *int_curr_symbol;
  char *currency_symbol;
  char *mon_decimal_point;
  char *mon_thousands_sep;
  char *mon_grouping;
  char *positive_sign;
  char *negative_sign;
  char int_frac_digits;
  char frac_digits;
  char p_cs_precedes;
  char p_sep_by_space;
  char n_cs_precedes;
  char n_sep_by_space;
  char p_sign_posn;
  char n_sign_posn;
};

static struct lconv l =
  {".","","","","","", 		/* decimal_point - mon_decimal_point */
   "","","","",127,127,		/* mon_thousands_sep - frac_digits */
   127,127,127,127,127,127};	/* p_cs_precedes - n_sign_posn */
   //127,127,127,127,127,127 };	/* __int_p_cs_precedes - __int_n_sign_posn */


struct lconv* localeconv() {
	  return &l;
}

int setvbuf(FILE *f, char *buf, int type, size_t len) {
	return 1;
}

int signal() __attribute__((weak));
int system(const char * path) __attribute__((weak));
int clock() __attribute__((weak));
int localtime() __attribute__((weak));
int strftime() __attribute__((weak));
int gmtime() __attribute__((weak));
int time() __attribute__((weak));
int mktime() __attribute__((weak));
int difftime() __attribute__((weak));
int setlocale() __attribute__((weak));

int signal() { return 0; };
int system(const char * path) { return 0; };
int clock() { return 0; };
int localtime() { return 0; };
int strftime() { return 0; };
int gmtime() { return 0; };
int time() { return 0; };
int mktime() { return 0; };
int difftime() { return 0; };
int setlocale() { return 0; };
