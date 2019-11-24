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

#include "util.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <stdio.h>

#include "gumbo.h"
#include "parser.h"

// TODO(jdtang): This should be elsewhere, but there's no .c file for
// SourcePositions and yet the constant needs some linkage, so this is as good
// as any.
const GumboSourcePosition kGumboEmptySourcePosition = {0, 0, 0};

void* gumbo_parser_allocate(GumboParser* parser, size_t num_bytes) {
  return parser->_options->allocator(parser->_options->userdata, num_bytes);
}

void gumbo_parser_deallocate(GumboParser* parser, void* ptr) {
  parser->_options->deallocator(parser->_options->userdata, ptr);
}

bool gumbo_is_positive_integer(const char *str, int len) {
    int i = 0;
    if (!str || len <= 0) {
        return false;
    } else {
        for (i = 0; i < len; i++) {
            if (str[i] > '9' || str[i] < '0') {
                return false;
            }
        }
        return true;
    }
}

bool gumbo_str_to_positive_integer(const char *str, int len, int *out) {
    int i = 0, ret = 0;
    if (!str || len <= 0) {
        return false;
    }
    for (i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
        ret = ret * 10 + (str[i] - '0');
    }
    *out = ret;
    return true;
}

bool gumbo_str_to_double(const char *str, int len, double *out) {
    int i = 0, j = 0, int_part = 0; 
    double float_part = 0;
    bool is_positive = true;
    if (!str || len <= 0) {
        return false;
    }
    if (str[i] == '-') {
        is_positive = false;
        i++;
    } else if (str[i] == '+') {
        i++;
    }
    for (; i < len; i++) {
        if (str[i] == '.') {
            i++;
            break;
        } else {
            if (str[i] < '0' || str[i] > '9') {
                return false;
            }
            int_part = int_part * 10 + (str[i] - '0');
        }
    }
    for (j = len - 1; j >= i; j--) {
        if (str[j] < '0' || str[j] > '9') {
            return false;
        }
        float_part = float_part / 10 + (str[j] - '0');
    }
    if (is_positive) {
        *out = int_part + float_part / 10;
    } else {
        *out = 0 - int_part - float_part / 10;
    }
    return true;
}

char* gumbo_copy_stringz(GumboParser* parser, const char* str) {
  char* buffer = gumbo_parser_allocate(parser, strlen(str) + 1);
  strcpy(buffer, str);
  return buffer;
}

// Debug function to trace operation of the parser.  Pass --copts=-DGUMBO_DEBUG
// to use.
void gumbo_debug(const char* format, ...) {
#ifdef GUMBO_DEBUG
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  fflush(stdout);
#endif
}
