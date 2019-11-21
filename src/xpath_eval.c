
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "util.h"
#include "xpath_eval.h"

static void gumbo_init_xpath_seg(GumboParser* parser, XpathSeg *seg) {
    seg->type = UNKNOWN;
    seg->is_deep_search = false;
    seg->filter.type = UNKNOWN;
    seg->filter.op = NONE;
    gumbo_string_buffer_init(parser, &seg->filter.name); 
    gumbo_string_buffer_init(parser, &seg->filter.value); 
}

static void gumbo_reset_xpath_seg(GumboParser *parser, XpathSeg *seg) {
    if (seg->type == DOC_NODE) {
        gumbo_string_buffer_clear(parser, &seg->node);
    } else {
        gumbo_string_buffer_clear(parser, &seg->attr);
    }
    seg->type = UNKNOWN;
    seg->is_deep_search = false;
    seg->filter.type = UNKNOWN;
    seg->filter.op = NONE;
    gumbo_string_buffer_clear(parser, &seg->filter.name);
    gumbo_string_buffer_init(parser, &seg->filter.value); 
}

static void gumbo_destroy_xpath_seg(GumboParser *parser, XpathSeg *seg) {
    if (seg->type == DOC_NODE) {
        gumbo_string_buffer_destroy(parser, &seg->node);
    } else {
        gumbo_string_buffer_destroy(parser, &seg->attr);
    }
    gumbo_string_buffer_destroy(parser, &seg->filter.name);
    gumbo_string_buffer_destroy(parser, &seg->filter.value);
}

static void gumbo_push_child_node(GumboParser *parser, GumboNode *parent_node, GumboVector *nodes) {
    GumboVector children = parent_node->v.element.children;
    int i = 0, child_size = children.length;
    for (i = 0; i < child_size; i++) {
        gumbo_vector_add(parser, children.data[i], nodes);
    }
}

static void gumbo_eval_xpath_seg(GumboParser *parser, XpathSeg *seg, GumboVector *src_nodes, GumboVector *dst_nodes, bool is_last_seg) {
    GumboNode *src_node;
    GumboTag seg_node_tag = GUMBO_TAG_UNKNOWN;
    int i = 0;
    while ((src_node = (GumboNode *)gumbo_vector_pop(parser, src_nodes)) != NULL) {
        if (src_node->type == GUMBO_NODE_ELEMENT) {
            if (seg->type == DOC_NODE) {
                if (seg_node_tag == GUMBO_TAG_UNKNOWN) {
                    seg_node_tag = gumbo_tagn_enum(seg->node.data, seg->node.length);
                }
                if (seg_node_tag == src_node->v.element.tag) {
                    GumboNode *node_filtered = NULL;
                    if (seg->filter.type != UNKNOWN) {
                        if(seg->filter.op == NONE) {
                            if (seg->filter.type == DOC_NODE) {
                                if (gumbo_is_positive_integer(seg->filter.name.data, seg->filter.name.length)) {
                                    seg->filter.name.data[seg->filter.name.length] = '\0';
                                    int index = atoi(seg->filter.name.data);
                                    if (src_node->v.element.children.length >= index) {
                                        node_filtered = (GumboNode *)src_node->v.element.children.data[index-1];
                                    }                                    
                                } else {
                                    GumboTag filter_node_tag = gumbo_tagn_enum(seg->filter.name.data, seg->filter.name.length);
                                    if (filter_node_tag != GUMBO_TAG_UNKNOWN) {
                                        for (i = 0; i < src_node->v.element.children.length;i++) {
                                            GumboNode *child_node = (GumboNode *)src_node->v.element.children.data[i];
                                            if (child_node->type == GUMBO_NODE_ELEMENT && child_node->v.element.tag == filter_node_tag) {
                                                node_filtered = src_node;
                                            }
                                        }
                                    }
                                }
                            } else {
                                GumboVector attrs = src_node->v.element.attributes;
                                for (i = 0; i < attrs.length; i++) {
                                    GumboAttribute *attr = (GumboAttribute *)attrs.data[i];
                                    if (!strncmp(attr->name, seg->filter.name.data, seg->filter.name.length)) {
                                         node_filtered = src_node;
                                         break;
                                    }
                                } 
                            }
                        } else {
                            switch(seg->filter.op) {
                            case LT:
                                break;
                            case LE:
                                break;
                            case GT:
                                break;
                            case GE:
                                break;
                            case EQ:
                                break;
                            case NE:
                                break;
                                default:
                                    break;
                            }
                        }
                    } else {
                        node_filtered = src_node;
                    }
                    if (node_filtered) {
                        if (is_last_seg) {
                            gumbo_vector_add(parser, node_filtered, dst_nodes);
                        } else {
                            gumbo_push_child_node(parser, node_filtered, dst_nodes);
                        }
                    }
                }
            } else {
                
            }
            if (seg->is_deep_search) {
                gumbo_push_child_node(parser, src_node, src_nodes);
            }
        }
    }
}

XpathFilterType gumbo_eval_xpath_from_root(GumboParser *parser, GumboNode *root, const char *xpath, GumboVector *output) {
    XpathFilterType ret_type;
    GumboVector nodes = {0};
    gumbo_vector_init(parser, DEFAULT_VECTOR_SIZE, &nodes);
    gumbo_vector_add(parser, root, &nodes);
    ret_type = gumbo_eval_xpath_from_nodes(parser, &nodes, xpath, output);
    gumbo_vector_destroy(parser, &nodes); 
    return ret_type;    
}

XpathFilterType gumbo_eval_xpath_from_nodes(GumboParser *parser, GumboVector *doc_nodes, const char *xpath, GumboVector *output) {
    int i = 0;
    bool is_in_filter = false;
    XpathFilterType ret;
    XpathFilterState parse_state = IN_DOC_NODE_KEY;
    XpathSeg seg = {0};
    GumboVector *src_nodes = output, *dst_nodes = doc_nodes, *temp = {0};
    gumbo_init_xpath_seg(parser, &seg);
    while (xpath[i] != '\0') {
        switch(xpath[i]) {
        case '/':
            if (i > 0 && xpath[i - 1] == '/') {
                seg.is_deep_search = true;
            } else {
                if (seg.type != UNKNOWN) {
                    temp = src_nodes;
                    src_nodes = dst_nodes;
                    dst_nodes = temp;
                    gumbo_eval_xpath_seg(parser, &seg, src_nodes, dst_nodes, false);
                }
                gumbo_reset_xpath_seg(parser, &seg);
            }
            break;
        case '@':
            if (is_in_filter) {
                seg.filter.type = DOC_NODE_ATTR;
                parse_state = IN_DOC_NODE_ATTR_KEY;
            } else {
                seg.type = DOC_NODE_ATTR;
                gumbo_string_buffer_init(parser, &seg.attr);
            }
            break;
        case '[':
            is_in_filter = true;
            parse_state = IN_DOC_NODE_KEY;
            seg.filter.type = DOC_NODE;
            break;
        case '>':
        case '<':
        case '=':
        case '!':
            if (is_in_filter && (parse_state == IN_DOC_NODE_KEY || parse_state == IN_DOC_NODE_ATTR_KEY)) {
                switch (xpath[i]) {
                case '>':
                    if (xpath[i+1] != '\0' && xpath[i+1] == '=') {
                        seg.filter.op = GE;
                        i++;
                    } else {
                        seg.filter.op = GT;
                    }
                    break;
                case '<':
                    if (xpath[i+1] != '\0' && xpath[i+1] == '=') {
                        seg.filter.op = LE;
                        i++;
                    } else {
                        seg.filter.op = LT;
                    }
                    break;
                case '=':
                    seg.filter.op = EQ;
                    break;
                case '!':
                    if (xpath[i+1] != '\0' && xpath[i+1] == '=') { 
                        seg.filter.op = NE;
                        i++;
                    }
                    break;
                }
                if (parse_state == IN_DOC_NODE_KEY) {
                    parse_state = IN_DOC_NODE_VALUE;
                } else {
                    parse_state = IN_DOC_NODE_ATTR_VALUE;
                }
            }
            break;
        case ']':
            is_in_filter = false;
            break;
            default:
                if (is_in_filter) {
                    switch(parse_state) {
                    case IN_DOC_NODE_KEY:
                    case IN_DOC_NODE_ATTR_KEY:
                        gumbo_string_buffer_append_codepoint(parser, xpath[i], &seg.filter.name);
                        break;
                    case IN_DOC_NODE_VALUE:
                    case IN_DOC_NODE_ATTR_VALUE:
                        gumbo_string_buffer_append_codepoint(parser, xpath[i], &seg.filter.value);
                        break;
                    }
                } else {
                    if (seg.type == UNKNOWN) {
                        seg.type = DOC_NODE;
                        gumbo_string_buffer_init(parser, &seg.node);
                    }
                    if (seg.type == DOC_NODE) {
                        gumbo_string_buffer_append_codepoint(parser, xpath[i], &seg.node);
                    } else {
                        gumbo_string_buffer_append_codepoint(parser, xpath[i], &seg.attr);
                    }
                }
                break;
        }
        i++;
    }
    temp = src_nodes;
    src_nodes = dst_nodes;
    dst_nodes = temp;
    gumbo_eval_xpath_seg(parser, &seg, src_nodes, dst_nodes, true);
    if (output != dst_nodes) {
        void *node = gumbo_vector_pop(parser, dst_nodes);
        gumbo_vector_add(parser, node, output);
    }    
    ret = seg.type;
    gumbo_destroy_xpath_seg(parser, &seg);
    return ret;
}

