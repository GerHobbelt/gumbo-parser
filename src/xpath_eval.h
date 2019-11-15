// Author: herengao@huawei.com

#ifndef GUMBO_XPATH_EVAL_H_
#define GUMBO_XPATH_EVAL_H_A

#include <stdbool.h>

#include "string_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DOC_NODE, DOC_NODE_ATTR
} XpathFilterType;

typedef enum {
    LE, LT, GE, GT, NE, EQ
} XpathFilterOP;

typedef enum {
    IN_DOC_NODE_KEY,
    IN_DOC_NODE_VALUE,
    IN_ATTR_KEY,
    IN_ATTR_VALUE
} XpathFilterState;

typedef struct {
    XpathFilterType filter_type;
    GumboStringBuffer *name;
    XpathFilterOp op;
    GumboStringBuffer *node;
} XpathFilterNode;

typedef struct {
    XpathFilterType seg_type;
    union {
        GumboStringBuffer* node;
        GumboStringBuffer* attr;
    }
    XpathFilterNode filter;
    bool is_deep_search;
} XpathSeg;

void gumbo_create_xpath_seg();

XpathFilterType gumbo_eval_xpath_from_root(GumboNode* root, const char *xpath);

XpathFilterType gumbo_eval_xpath_from_nodes(GumboVector *doc_nodes, const char *xpath);

#ifdef __cplusplus
}
#endif

#endif  // GUMBO_XPATH_EVAL_H_
