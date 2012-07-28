#ifndef TRIPCODE_H
#define TRIPCODE_H

#include <iconv.h> /* iconv_t */

/**
 * transform_html
 *
 * Translates characters from the input string into their
 * corresponding HTML entities and writes the result to the
 * output buffer.
 *
 * At most will 8 characters be read from the input string.
 * The output buffer must be able to hold at least 15 chars.
 *
 * This function returns the number of characters that were
 * put in the output buffer.
 *
 * @param  (const char *) string to use for translation
 * @param  (char *) buffer to put results
 * @return (size_t) number of characters put in buffer
 */
extern size_t transform_html (const char * s, char * dest);

/**
 * transform_sjis
 *
 * Translates the input string from UTF-8 to Shift-JIS and
 * writes the result to the output buffer.
 *
 * The input iconv descriptor argument must be a valid and
 * open iconv descriptor from UTF-8 to SJIS.
 *
 * This function returns the number of characters written to
 * the output buffer, upon error this function returns
 * (size_t) -1 and sets errno to indicate the error.
 *
 * @param  (iconv_t) open UTF-8 to SJIS iconv descriptor
 * @param  (char *) string to translate
 * @param  (size_t) length of input string
 * @param  (char *) buffer to store result
 * @param  (size_t) size of buffer
 * @return (size_t) numbers of characters written to buffer
 */
extern size_t transform_sjis (iconv_t cd,
			      char * s, size_t len,
			      char * dest, size_t size);

/**
 * trip
 *
 * Creates a tripcode from a string. It is assumed that the
 * string has already undergone SJIS and HTML transformations
 * so no conversion is performed here.
 *
 * The output buffer must be able to hold at least 14 chars.
 *
 * @param  (const char *) string to trip
 * @param  (size_t) length of string
 * @param  (char *) buffer where the resulting tripcode is put
 */
extern void trip (const char * s, size_t len, char * dest);

#endif /* TRIPCODE_H */
