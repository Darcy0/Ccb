/****************************************************************************
*																			*
*						cryptlib RC4 Encryption Routines					*
*						Copyright Peter Gutmann 1992-1996					*
*																			*
****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "rc4.h"


/* The size of the expanded IDEA keys */

#define RC4_EXPANDED_KEYSIZE	sizeof( RC4KEY )

/****************************************************************************
*																			*
*							RC4 En/Decryption Routines						*
*																			*
****************************************************************************/

/* Encrypt/decrypt data.  Since RC4 is a stream cipher, encryption and
   decryption are one and the same */

int rc4Encrypt( RC4KEY *key, BYTE *buffer, int noBytes )
	{
	rc4Crypt( key, buffer, noBytes );

	return( CRYPT_OK );
	}

int rc4Decrypt( RC4KEY *key, BYTE *buffer, int noBytes )
	{
	rc4Crypt(  key, buffer, noBytes );

	return( CRYPT_OK );
	}

/****************************************************************************
*																			*
*							RC4 Key Management Routines						*
*																			*
****************************************************************************/

/* Create an expanded RC4 key */

int rc4InitKey( RC4KEY *key,unsigned char *userKey,unsigned userKeyLength )
{
	rc4ExpandKey( ( RC4KEY * ) key, userKey, userKeyLength );

	return( CRYPT_OK );
}
