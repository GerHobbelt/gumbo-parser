// Author: herengao@huawei.com

#ifndef GUMBO_XPATH_EVAL_H_
#define GUMBO_XPATH_EVAL_H_A

#include <stdbool.h>
#include "parser.h"
#include "string_buffer.h"
#include "vector.h"

#define DEFAULT_VECTOR_SIZE 10

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UNKNOWN, DOC_NODE, DOC_NODE_ATTR
} XpathFilterType;

typedef enum {
    NONE, LE, LT, GE, GT, NE, EQ
} XpathFilterOp;

typedef enum {
    IN_DOC_NODE_KEY,
    IN_DOC_NODE_VALUE,
    IN_DOC_NODE_ATTR_KEY,
    IN_DOC_NODE_ATTR_VALUE
} XpathFilterState;

typedef struct {
    XpathFilterType type;
    bool is_index;
    GumboStringBuffer name;
    int index;
    XpathFilterOp op;
    bool is_numeric_value;
    GumboStringBuffer value;
    double double_value;
} XpathFilterNode;

typedef struct {
    XpathFilterType type;
    union {
        GumboStringBuffer node;
        GumboStringBuffer attr;
    };
    XpathFilterNode filter;
    bool is_deep_search;
} XpathSeg;

XpathFilterType gumbo_eval_xpath_from_root(GumboParser* parser, GumboNode* root, const char *xpath, GumboVector *output);

XpathFilterType gumbo_eval_xpath_from_nodes(GumboParser* parser, GumboVector *doc_nodes, const char *xpath, GumboVector *output);

#ifdef __cplusplus
}
#endif

#endif  // GUMBO_XPATH_EVAL_H_
