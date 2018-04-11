#include <limits.h>

/* If the system can handle byte ops, we use those so we don't have to do a
   lot of masking.  Otherwise, we use machine-word-size ops which will be
   faster on RISC machines */

#if UINT_MAX > 0xFFFFL		/* System has 32-bit ints */
  #define USE_LONG_RC4

  typedef unsigned int rc4word;
#else
  typedef unsigned char rc4word;
#endif /* UINT_MAX > 0xFFFFL */

/* The scheduled RC4 key */

typedef struct {
	rc4word state[ 256 ];
	rc4word x, y;
	} RC4KEY ;

void rc4ExpandKey( RC4KEY *rc4, unsigned char const *key, int keylen );
int rc4InitKey( RC4KEY *key,unsigned char *userKey,unsigned userKeyLength );
void rc4Crypt( RC4KEY *rc4, unsigned char *data, int len );


#ifndef BYTE
typedef unsigned char BYTE;
#endif

#if ULONG_MAX > 0xFFFFFFFFUL
  #define _BIG_WORDS
#endif /* LONG > 32 bits */

#if !defined( LITTLE_ENDIAN ) && !defined( BIG_ENDIAN )
#if ( ( 0x4142>> 8 ) == 'B' )
#define LITTLE_ENDIAN
#elif ( ( 0x4142 >> 8 ) == 'A' )
#define BIG_ENDIAN
#else
#error Cannot determine processor endianness - edit crypt.h and recompile
#endif /* Endianness test */
#endif /* !( LITTLE_ENDIAN || BIG_ENDIAN ) */

#ifdef BIG_ENDIAN
	#define mgetLong(memPtr)		\
		( ( ( LONG ) memPtr[ 0 ] << 24 ) | ( ( LONG ) memPtr[ 1 ] << 16 ) | \
		  ( ( LONG ) memPtr[ 2 ] << 8 ) | ( ( LONG ) memPtr[ 3 ] ) ); \
		memPtr += 4

	#define mputLong(memPtr,data)	\
		memPtr[ 0 ] = ( BYTE ) ( ( ( data ) >> 24 ) & 0xFF ); \
		memPtr[ 1 ] = ( BYTE ) ( ( ( data ) >> 16 ) & 0xFF ); \
		memPtr[ 2 ] = ( BYTE ) ( ( ( data ) >> 8 ) & 0xFF ); \
		memPtr[ 3 ] = ( BYTE ) ( ( data ) & 0xFF ); \
		memPtr += 4
#else
	#define mgetLong(memPtr)		\
		( ( ( LONG ) memPtr[ 0 ] ) | ( ( LONG ) memPtr[ 1 ] << 8 ) | \
		  ( ( LONG ) memPtr[ 2 ] << 16 ) | ( ( LONG ) memPtr[ 3 ] << 24 ) ); \
		memPtr += 4

	#define mputLong(memPtr,data)	\
		memPtr[ 0 ] = ( BYTE ) ( ( data ) & 0xFF ); \
		memPtr[ 1 ] = ( BYTE ) ( ( ( data ) >> 8 ) & 0xFF ); \
		memPtr[ 2 ] = ( BYTE ) ( ( ( data ) >> 16 ) & 0xFF ); \
		memPtr[ 3 ] = ( BYTE ) ( ( ( data ) >> 24 ) & 0xFF ); \
		memPtr += 4
#endif /* BIG_ENDIAN */


#if UINT_MAX > 0xFFFFUL
  #define MASK16( x )	( ( x ) & 0xFFFFUL )
#else
  #define MASK16( x )	x
#endif /* > 16-bit ints */

#ifdef _BIG_WORDS
  #define MASK32( x )	( ( x ) & 0xFFFFFFFFUL )
#else
  #define MASK32( x )	x
#endif /* _BIG_WORDS */


/* Versions of the above which are guaranteed to always be big or
   little-endian (these are needed for some algorithms where the external
   data format is always little-endian, eg anything designed by Ron
   Rivest) */

#define mgetBWord(memPtr)		\
		( ( WORD ) memPtr[ 0 ] << 0 ) | ( ( WORD ) memPtr[ 1 ] ); \
		memPtr += 2

#define mputBWord(memPtr,data)	\
		memPtr[ 0 ] = ( BYTE ) ( ( ( data ) >> 8 ) & 0xFF ); \
		memPtr[ 1 ] = ( BYTE ) ( ( data ) & 0xFF ); \
		memPtr += 2

#define mgetBLong(memPtr)		\
		( ( ( LONG ) memPtr[ 0 ] << 24 ) | ( ( LONG ) memPtr[ 1 ] << 16 ) | \
		  ( ( LONG ) memPtr[ 2 ] << 8 ) | ( LONG ) memPtr[ 3 ] ); \
		memPtr += 4

#define mputBLong(memPtr,data)	\
		memPtr[ 0 ] = ( BYTE ) ( ( ( data ) >> 24 ) & 0xFF ); \
		memPtr[ 1 ] = ( BYTE ) ( ( ( data ) >> 16 ) & 0xFF ); \
		memPtr[ 2 ] = ( BYTE ) ( ( ( data ) >> 8 ) & 0xFF ); \
		memPtr[ 3 ] = ( BYTE ) ( ( data ) & 0xFF ); \
		memPtr += 4

#define mgetLWord(memPtr)		\
		( ( WORD ) memPtr[ 0 ] ) | ( ( WORD ) memPtr[ 1 ] << 8 ); \
		memPtr += 2

#define mputLWord(memPtr,data)	\
		memPtr[ 0 ] = ( BYTE ) ( ( data ) & 0xFF ); \
		memPtr[ 1 ] = ( BYTE ) ( ( ( data ) >> 8 ) & 0xFF ); \
		memPtr += 2

#define mgetLLong(memPtr)		\
		( ( ( LONG ) memPtr[ 0 ] ) | ( ( LONG ) memPtr[ 1 ] << 8 ) | \
		  ( ( LONG ) memPtr[ 2 ] << 16 ) | ( ( LONG ) memPtr[ 3 ] << 24 ) ); \
		memPtr += 4

#define mputLLong(memPtr,data)	\
		memPtr[ 0 ] = ( BYTE ) ( ( data ) & 0xFF ); \
		memPtr[ 1 ] = ( BYTE ) ( ( ( data ) >> 8 ) & 0xFF ); \
		memPtr[ 2 ] = ( BYTE ) ( ( ( data ) >> 16 ) & 0xFF ); \
		memPtr[ 3 ] = ( BYTE ) ( ( ( data ) >> 24 ) & 0xFF ); \
		memPtr += 4

#define zeroise(a,b) memset( (a),0,(b))

#define CRYPT_OK			0		/* No error */