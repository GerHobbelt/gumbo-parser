#ifndef GUMBO_UTIL_H_
#define GUMBO_UTIL_H_
#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include <stdbool.h>
#include <stddef.h>
#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

// Utility function for allocating & copying a null-terminated string into a
// freshly-allocated buffer. This is necessary for proper memory management; we
// have the convention that all const char* in parse tree structures are
// freshly-allocated, so if we didn't copy, we'd try to delete a literal string
// when the parse tree is destroyed.
char* gumbo_strdup(const char* str) XMALLOC NONNULL_ARGS;

void* gumbo_alloc(size_t size) XMALLOC;
void* gumbo_realloc(void* ptr, size_t size) RETURNS_NONNULL;
void gumbo_free(void* ptr);

// Deallocate a chunk of memory, using the deallocator specified in the Parser's
// config options.
void gumbo_parser_deallocate(struct GumboInternalParser* parser, void* ptr);

// non-locale specific replacements for ctype.h functions needed for spec
static inline int gumbo_tolower(int c)
{
  return c | ((c >= 'A' && c <= 'Z') << 5);
}

static inline bool gumbo_isalpha(int c)
{
  return (c | 0x20) >= 'a' && (c | 0x20) <= 'z';
}

bool gumbo_isspace(unsigned char ch);

bool gumbo_isalnum(unsigned char ch);

// translate string to positive integer
bool gumbo_str_to_positive_integer(const char *str, int len, int *out);

// translate string to double
bool gumbo_str_to_double(const char *str, int len, double *out);

// Debug wrapper for printf, to make it easier to turn off debugging info when
// required.
void gumbo_debug(const char* format, ...) PRINTF(1);

#ifdef __cplusplus
}
#endif

#endif // GUMBO_UTIL_H_
