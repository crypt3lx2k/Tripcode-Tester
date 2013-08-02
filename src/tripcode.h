/*
 * Tripcode Tester - A program for testing tripcodes.
 * Copyright (C) 2010-2013 Truls Edvard Stokke
 *
 * Tripcode Tester is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tripcode Tester is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tripcode Tester.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRIPCODE_H
#define TRIPCODE_H

#include <iconv.h> /* iconv_t */
#include <stddef.h> /* size_t */

/**
 * tripcode
 *
 * Performs all necessary conversions on the input string
 * and outputs a pointer to inside of the buffer where
 * the tripcode resides.
 *
 * The input iconv conversion descriptor argument must be
 * a valid and open iconv descriptor that translates to SJIS.
 *
 * The output buffer must be able to hold at least 14 chars.
 *
 * This function returns a pointer to the start of the
 * tripcode, 3 characters into the buffer, upon error this
 * function returns NULL and sets errno to indicate the
 * error.
 *
 * If don't need any other functionality than to just create
 * a tripcode from some input then this is the only function
 * in this file you need to concern yourself with.
 *
 * @param  (iconv_t) open SJIS iconv conversion descriptor
 * @param  (const char *) tripcode key
 * @param  (size_t) tripcode key length
 * @param  (char *) output buffer
 * @return (char *) pointer to tripcode
 */
extern char * tripcode (iconv_t cd,
			char * input, size_t len,
			char * output);

/**
 * tripcode_html
 *
 * Translates characters from the input string into their
 * corresponding HTML entities and writes the result to the
 * output buffer.
 *
 * At most 8 characters will be read from the input string.
 * The output buffer must be able to hold at least 15 chars.
 *
 * This function returns the number of characters that were
 * put in the output buffer.
 *
 * @param  (const char *) string to use for translation
 * @param  (char *) buffer to put results
 * @return (size_t) number of characters put in buffer
 */
extern size_t tripcode_html (const char * s, char * dest);

/**
 * tripcode_sjis
 *
 * Translates the input string with an iconv conversion
 * descriptor and writes the result to the output buffer.
 *
 * The input iconv descriptor argument must be a valid and
 * open iconv descriptor that translates to SJIS.
 *
 * This function returns the number of characters written to
 * the output buffer, upon error this function returns
 * (size_t) -1 and sets errno to indicate the error.
 *
 * @param  (iconv_t) open SJIS iconv descriptor
 * @param  (char *) string to translate
 * @param  (size_t) length of input string
 * @param  (char *) buffer to store result
 * @param  (size_t) size of buffer
 * @return (size_t) numbers of characters written to buffer
 */
extern size_t tripcode_sjis (iconv_t cd,
			     char * s,    size_t len,
			     char * dest, size_t size);

/**
 * tripcode_crypt
 *
 * Creates a tripcode from a string. It is assumed that the
 * string has already undergone SJIS and HTML transformations
 * so no conversion is performed within this function.
 *
 * The output buffer must be able to hold at least 14 chars.
 *
 * The return value is a pointer to inside of the buffer where
 * the tripcode resides, the first 3 characters in the buffer
 * are not part of the tripcode but are part of the DES based
 * crypt(3) output.
 *
 * @param  (const char *) string to crypt(3)
 * @param  (size_t) length of string
 * @param  (char *) buffer where the resulting tripcode is put
 * @return (char *) pointer to tripcode
 */
extern char * tripcode_crypt (const char * s, size_t len, char * dest);

#endif /* TRIPCODE_H */
