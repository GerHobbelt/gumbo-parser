// Author: herengao@huawei.com

#ifndef GUMBO_XPATH_EVAL_H_
#define GUMBO_XPATH_EVAL_H_A

#include <stdbool.h>
#include "parser.h"
#include "string_buffer.h"
#include "vector.h"
#include "util.h"

#define DEFAULT_VECTOR_SIZE 10

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UNKNOWN_SEG, DOC_NODE, DOC_NODE_ATTR
} XpathSegType;

typedef enum {
    NONE, LE, LT, GE, GT, NE, EQ
} XpathFilterOp;

typedef enum {
    UNKNOWN_FILTER, NODE_INDEX, NODE_NUMERIC, NODE_STRING, ATTR_NUMERIC, ATTR_STRING, LEFT_BRACKETS, RIGHT_BRACKETS, AND, OR, BOOL
} XpathFilterType;

typedef struct {
    XpathFilterType type;
    GumboStringBuffer name;
    int index;
    XpathFilterOp op;
    GumboStringBuffer value;
    double numeric_value;
    bool bool_value;
} XpathFilter;

typedef struct {
    XpathSegType type;
    GumboTag node_tag;
    GumboStringBuffer node_or_attr;
    GumboVector filters;
    bool is_deep_search;
} XpathSeg;

XpathSegType gumbo_eval_xpath_from_root(GumboParser* parser, GumboNode* doc, const char *xpath, GumboVector *output);

XpathSegType gumbo_eval_xpath_from_nodes(GumboParser* parser, GumboVector *doc_nodes, const char *xpath, GumboVector *output);

void gumbo_compile_xpath(GumboParser *parser, const char *xpath, GumboVector *xpath_segs);

#if defined(GUMBO_DEBUG)
void gumbo_dump_xpath_segs(GumboParser *parser, GumboVector *xpath_segs);
#endif

#ifdef __cplusplus
}
#endif

#endif  // GUMBO_XPATH_EVAL_H_
