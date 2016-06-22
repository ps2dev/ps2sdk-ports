/* This file has been generated by the Perl script `afblue.pl', */
/* using data from file `afblue.dat'.                           */

/***************************************************************************/
/*                                                                         */
/*  afblue.h                                                               */
/*                                                                         */
/*    Auto-fitter data for blue strings (specification).                   */
/*                                                                         */
/*  Copyright 2013-2016 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef AFBLUE_H_
#define AFBLUE_H_


FT_BEGIN_HEADER


  /* an auxiliary macro to decode a UTF-8 character -- since we only use */
  /* hard-coded, self-converted data, no error checking is performed     */
#define GET_UTF8_CHAR( ch, p )                      \
          do                                        \
          {                                         \
            ch = (unsigned char)*p++;               \
            if ( ch >= 0x80 )                       \
            {                                       \
              FT_UInt  len_;                        \
                                                    \
                                                    \
              if ( ch < 0xE0 )                      \
              {                                     \
                len_ = 1;                           \
                ch  &= 0x1F;                        \
              }                                     \
              else if ( ch < 0xF0 )                 \
              {                                     \
                len_ = 2;                           \
                ch  &= 0x0F;                        \
              }                                     \
              else                                  \
              {                                     \
                len_ = 3;                           \
                ch  &= 0x07;                        \
              }                                     \
                                                    \
              for ( ; len_ > 0; len_-- )            \
                ch = ( ch << 6 ) | ( *p++ & 0x3F ); \
            }                                       \
          } while ( 0 )


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                    B L U E   S T R I N G S                    *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* At the bottommost level, we define strings for finding blue zones. */


#define AF_BLUE_STRING_MAX_LEN  51

  /* The AF_Blue_String enumeration values are offsets into the */
  /* `af_blue_strings' array.                                   */

  typedef enum  AF_Blue_String_
  {
    AF_BLUE_STRING_ARABIC_TOP = 0,
    AF_BLUE_STRING_ARABIC_BOTTOM = 18,
    AF_BLUE_STRING_ARABIC_JOIN = 33,
    AF_BLUE_STRING_BENGALI_BASE = 36,
    AF_BLUE_STRING_BENGALI_TOP = 68,
    AF_BLUE_STRING_BENGALI_HEAD = 96,
    AF_BLUE_STRING_CYRILLIC_CAPITAL_TOP = 128,
    AF_BLUE_STRING_CYRILLIC_CAPITAL_BOTTOM = 152,
    AF_BLUE_STRING_CYRILLIC_SMALL = 176,
    AF_BLUE_STRING_CYRILLIC_SMALL_DESCENDER = 200,
    AF_BLUE_STRING_DEVANAGARI_BASE = 209,
    AF_BLUE_STRING_DEVANAGARI_TOP = 241,
    AF_BLUE_STRING_DEVANAGARI_HEAD = 273,
    AF_BLUE_STRING_DEVANAGARI_BOTTOM = 305,
    AF_BLUE_STRING_GREEK_CAPITAL_TOP = 313,
    AF_BLUE_STRING_GREEK_CAPITAL_BOTTOM = 334,
    AF_BLUE_STRING_GREEK_SMALL_BETA_TOP = 352,
    AF_BLUE_STRING_GREEK_SMALL = 370,
    AF_BLUE_STRING_GREEK_SMALL_DESCENDER = 394,
    AF_BLUE_STRING_HEBREW_TOP = 418,
    AF_BLUE_STRING_HEBREW_BOTTOM = 442,
    AF_BLUE_STRING_HEBREW_DESCENDER = 460,
    AF_BLUE_STRING_KANNADA_TOP = 475,
    AF_BLUE_STRING_KANNADA_BOTTOM = 519,
    AF_BLUE_STRING_KHMER_TOP = 551,
    AF_BLUE_STRING_KHMER_SUBSCRIPT_TOP = 575,
    AF_BLUE_STRING_KHMER_BOTTOM = 615,
    AF_BLUE_STRING_KHMER_DESCENDER = 647,
    AF_BLUE_STRING_KHMER_LARGE_DESCENDER = 681,
    AF_BLUE_STRING_KHMER_SYMBOLS_WAXING_TOP = 768,
    AF_BLUE_STRING_KHMER_SYMBOLS_WANING_BOTTOM = 776,
    AF_BLUE_STRING_LAO_TOP = 784,
    AF_BLUE_STRING_LAO_BOTTOM = 816,
    AF_BLUE_STRING_LAO_ASCENDER = 848,
    AF_BLUE_STRING_LAO_LARGE_ASCENDER = 864,
    AF_BLUE_STRING_LAO_DESCENDER = 876,
    AF_BLUE_STRING_LATIN_CAPITAL_TOP = 900,
    AF_BLUE_STRING_LATIN_CAPITAL_BOTTOM = 916,
    AF_BLUE_STRING_LATIN_SMALL_F_TOP = 932,
    AF_BLUE_STRING_LATIN_SMALL = 946,
    AF_BLUE_STRING_LATIN_SMALL_DESCENDER = 960,
    AF_BLUE_STRING_LATIN_SUBS_CAPITAL_TOP = 970,
    AF_BLUE_STRING_LATIN_SUBS_CAPITAL_BOTTOM = 990,
    AF_BLUE_STRING_LATIN_SUBS_SMALL_F_TOP = 1010,
    AF_BLUE_STRING_LATIN_SUBS_SMALL = 1030,
    AF_BLUE_STRING_LATIN_SUBS_SMALL_DESCENDER = 1066,
    AF_BLUE_STRING_LATIN_SUPS_CAPITAL_TOP = 1086,
    AF_BLUE_STRING_LATIN_SUPS_CAPITAL_BOTTOM = 1117,
    AF_BLUE_STRING_LATIN_SUPS_SMALL_F_TOP = 1146,
    AF_BLUE_STRING_LATIN_SUPS_SMALL = 1172,
    AF_BLUE_STRING_LATIN_SUPS_SMALL_DESCENDER = 1197,
    AF_BLUE_STRING_MALAYALAM_TOP = 1208,
    AF_BLUE_STRING_MALAYALAM_BOTTOM = 1252,
    AF_BLUE_STRING_MYANMAR_TOP = 1284,
    AF_BLUE_STRING_MYANMAR_BOTTOM = 1316,
    AF_BLUE_STRING_MYANMAR_ASCENDER = 1348,
    AF_BLUE_STRING_MYANMAR_DESCENDER = 1376,
    AF_BLUE_STRING_TAMIL_TOP = 1408,
    AF_BLUE_STRING_TAMIL_BOTTOM = 1440,
    AF_BLUE_STRING_TELUGU_TOP = 1472,
    AF_BLUE_STRING_TELUGU_BOTTOM = 1500,
    AF_BLUE_STRING_THAI_TOP = 1528,
    AF_BLUE_STRING_THAI_BOTTOM = 1552,
    AF_BLUE_STRING_THAI_ASCENDER = 1580,
    AF_BLUE_STRING_THAI_LARGE_ASCENDER = 1592,
    AF_BLUE_STRING_THAI_DESCENDER = 1604,
    AF_BLUE_STRING_THAI_LARGE_DESCENDER = 1620,
    AF_BLUE_STRING_THAI_DIGIT_TOP = 1628,
    af_blue_1_1 = 1639,
#ifdef AF_CONFIG_OPTION_CJK
    AF_BLUE_STRING_CJK_TOP = af_blue_1_1 + 1,
    AF_BLUE_STRING_CJK_BOTTOM = af_blue_1_1 + 203,
    af_blue_1_1_1 = af_blue_1_1 + 404,
#ifdef AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT
    AF_BLUE_STRING_CJK_LEFT = af_blue_1_1_1 + 1,
    AF_BLUE_STRING_CJK_RIGHT = af_blue_1_1_1 + 204,
    af_blue_1_1_2 = af_blue_1_1_1 + 405,
#else
    af_blue_1_1_2 = af_blue_1_1_1 + 0,
#endif /* AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT */
    af_blue_1_2 = af_blue_1_1_2 + 0,
#else
    af_blue_1_2 = af_blue_1_1 + 0,
#endif /* AF_CONFIG_OPTION_CJK                */


    AF_BLUE_STRING_MAX   /* do not remove */

  } AF_Blue_String;


  FT_LOCAL_ARRAY( char )
  af_blue_strings[];


  /*************************************************************************/
  /*************************************************************************/
  /*****                                                               *****/
  /*****                 B L U E   S T R I N G S E T S                 *****/
  /*****                                                               *****/
  /*************************************************************************/
  /*************************************************************************/

  /* The next level is to group blue strings into style-specific sets. */


  /* Properties are specific to a writing system.  We assume that a given  */
  /* blue string can't be used in more than a single writing system, which */
  /* is a safe bet.                                                        */
#define AF_BLUE_PROPERTY_LATIN_TOP       ( 1U << 0 )  /* must have value 1 */
#define AF_BLUE_PROPERTY_LATIN_SUB_TOP   ( 1U << 1 )
#define AF_BLUE_PROPERTY_LATIN_NEUTRAL   ( 1U << 2 )
#define AF_BLUE_PROPERTY_LATIN_X_HEIGHT  ( 1U << 3 )
#define AF_BLUE_PROPERTY_LATIN_LONG      ( 1U << 4 )

#define AF_BLUE_PROPERTY_CJK_TOP    ( 1U << 0 )       /* must have value 1 */
#define AF_BLUE_PROPERTY_CJK_HORIZ  ( 1U << 1 )       /* must have value 2 */
#define AF_BLUE_PROPERTY_CJK_RIGHT  AF_BLUE_PROPERTY_CJK_TOP


#define AF_BLUE_STRINGSET_MAX_LEN  8

  /* The AF_Blue_Stringset enumeration values are offsets into the */
  /* `af_blue_stringsets' array.                                   */

  typedef enum  AF_Blue_Stringset_
  {
    AF_BLUE_STRINGSET_ARAB = 0,
    AF_BLUE_STRINGSET_BENG = 4,
    AF_BLUE_STRINGSET_CYRL = 9,
    AF_BLUE_STRINGSET_DEVA = 15,
    AF_BLUE_STRINGSET_GREK = 21,
    AF_BLUE_STRINGSET_HEBR = 28,
    AF_BLUE_STRINGSET_KNDA = 32,
    AF_BLUE_STRINGSET_KHMR = 35,
    AF_BLUE_STRINGSET_KHMS = 41,
    AF_BLUE_STRINGSET_LAO = 44,
    AF_BLUE_STRINGSET_LATN = 50,
    AF_BLUE_STRINGSET_LATB = 57,
    AF_BLUE_STRINGSET_LATP = 64,
    AF_BLUE_STRINGSET_MLYM = 71,
    AF_BLUE_STRINGSET_MYMR = 74,
    AF_BLUE_STRINGSET_TAML = 79,
    AF_BLUE_STRINGSET_TELU = 82,
    AF_BLUE_STRINGSET_THAI = 85,
    af_blue_2_1 = 93,
#ifdef AF_CONFIG_OPTION_CJK
    AF_BLUE_STRINGSET_HANI = af_blue_2_1 + 0,
    af_blue_2_1_1 = af_blue_2_1 + 2,
#ifdef AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT
    af_blue_2_1_2 = af_blue_2_1_1 + 2,
#else
    af_blue_2_1_2 = af_blue_2_1_1 + 0,
#endif /* AF_CONFIG_OPTION_CJK_BLUE_HANI_VERT */
    af_blue_2_2 = af_blue_2_1_2 + 1,
#else
    af_blue_2_2 = af_blue_2_1 + 0,
#endif /* AF_CONFIG_OPTION_CJK                */


    AF_BLUE_STRINGSET_MAX   /* do not remove */

  } AF_Blue_Stringset;


  typedef struct  AF_Blue_StringRec_
  {
    AF_Blue_String  string;
    FT_UShort       properties;

  } AF_Blue_StringRec;


  FT_LOCAL_ARRAY( AF_Blue_StringRec )
  af_blue_stringsets[];

/* */

FT_END_HEADER


#endif /* AFBLUE_H_ */


/* END */
