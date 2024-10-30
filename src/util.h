// Copyright 2010 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: jdtang@google.com (Jonathan Tang)
//
// This contains some utility functions that didn't fit into any of the other
// headers.

#ifndef GUMBO_UTIL_H_
#define GUMBO_UTIL_H_

#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration since it's passed into some of the functions in this
// header.
struct GumboInternalParser;

// Utility function for allocating & copying a null-terminated string into a
// freshly-allocated buffer.  This is necessary for proper memory management; we
// have the convention that all const char* in parse tree structures are
// freshly-allocated, so if we didn't copy, we'd try to delete a literal string
// when the parse tree is destroyed.
char* gumbo_copy_stringz(struct GumboInternalParser* parser, const char* str);

// Allocate a chunk of memory, using the allocator specified in the Parser's
// config options.
void* gumbo_parser_allocate(struct GumboInternalParser* parser, size_t num_bytes);

// Deallocate a chunk of memory, using the deallocator specified in the Parser's
// config options.
void gumbo_parser_deallocate(struct GumboInternalParser* parser, void* ptr);

// Resize (reallocate) a chunk of memory, using the allocator specified in the Parser's
// config options.
void* gumbo_parser_reallocate(struct GumboInternalParser* parser, void* ptr, size_t new_num_bytes, size_t old_num_bytes);

// non-locale specific replacements for ctype.h functions needed for spec
static inline int gumbo_tolower(int c)
{
  return c | ((c >= 'A' && c <= 'Z') << 5);
}

static inline bool gumbo_isalpha(int c)
{
  return (c | 0x20) >= 'a' && (c | 0x20) <= 'z';
}

static inline bool gumbo_isspace(unsigned char ch) 
{
  switch(ch) {
  case ' ':
  case '\f':
  case '\r':
  case '\n':
  case '\t':
    return true;
  default:
    return false;
  }
}

static inline bool gumbo_isalnum(unsigned char ch) 
{
  if ('a' <= ch && ch <= 'z') return true;
  if ('A' <= ch && ch <= 'Z') return true;
  if ('0' <= ch && ch <= '9') return true;
  return false;
}

// Returns -1 if string1 is less than string2; 1 if string1 is greater than string2, and 0 if they are equal.
int gumbo_strcasecmp(const char* string1, const char* string2);

// translate string to positive integer
bool gumbo_str_to_positive_integer(const char *str, int len, int *out);

// translate string to double
bool gumbo_str_to_double(const char *str, int len, double *out);

#if defined(NO_GUMBO_DEBUG)
#undef GUMBO_DEBUG
#elif !defined(GUMBO_DEBUG)
#if !defined(NDEBUG) || defined(_DEBUG)
#define GUMBO_DEBUG    1
#else
#define GUMBO_DEBUG    1
#endif
#endif

// Print debugging info to stderr.
void gumbo_vdebug(const char* format, va_list args);

// Debug wrapper for printf, to make it easier to turn off debugging info when
// required.
static inline void gumbo_debug(const char* format, ...) {
#if defined(GUMBO_DEBUG)
  va_list args;
  va_start(args, format);
  gumbo_vdebug(format, args);
  va_end(args);
#else
  // nada
#endif
}

#ifdef __cplusplus
}
#endif

#endif  // GUMBO_UTIL_H_
