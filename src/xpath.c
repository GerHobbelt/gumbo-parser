
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "util.h"
#include "xpath.h"

#define CONFIRM_DOC_NODE(x, y) \
    do { \
        if (is_last_node_seg) { \
            gumbo_vector_add(parser, x, y); \
        } else { \
            gumbo_push_child_node(parser, x, y); \
        } \
    } while(0); 

#define MULTI_BOOL_OPERATE(x) \
    do { \
        if (x->type == BOOL) { \
            if (filter_type == AND) { \
                ret &= x->bool_value; \
            } else { \
                ret |= x->bool_value; \
            } \
        } else if (x->type == AND || x->type == OR) { \
            filter_type = x->type; \
        } \
    } while(0);

static XpathSeg *gumbo_new_xpath_seg(GumboParser* parser) {
  XpathSeg *seg = (XpathSeg *)gumbo_parser_allocate(parser, sizeof(XpathSeg));
    seg->type = UNKNOWN_SEG;
    seg->is_deep_search = false;
    gumbo_string_buffer_init(parser, &seg->node_or_attr);
    gumbo_vector_init(parser, DEFAULT_VECTOR_SIZE, &seg->filters); 
    return seg;
}

static XpathFilter *gumbo_new_xpath_filter(GumboParser *parser, XpathFilterType filter_type) {
  XpathFilter *filter = (XpathFilter *)gumbo_parser_allocate(parser, sizeof(XpathFilter));
    filter->type = filter_type;
    filter->op = NONE;
    filter->bool_value = false;
    if (filter_type == NODE_STRING) {
        gumbo_string_buffer_init(parser, &filter->name); 
        gumbo_string_buffer_init(parser, &filter->value); 
    }
    return filter;
}

static void gumbo_free_xpath_filter(GumboParser *parser, XpathFilter *filter) {
    if (filter->type < LEFT_BRACKETS) { 
        gumbo_string_buffer_destroy(parser, &filter->name);
        gumbo_string_buffer_destroy(parser, &filter->value);
    }
    gumbo_parser_deallocate(parser, filter);
}

static void gumbo_free_xpath_seg(GumboParser *parser, XpathSeg *xpath_seg) {
    gumbo_string_buffer_destroy(parser, &xpath_seg->node_or_attr);
    XpathFilter *filter;
    while ((filter = gumbo_vector_pop(parser, &xpath_seg->filters)) != NULL) {
        gumbo_free_xpath_filter(parser, filter);
    }
    gumbo_vector_destroy(parser, &xpath_seg->filters);
    gumbo_parser_deallocate(parser, xpath_seg);
}

static void gumbo_push_xpath_filter(GumboParser *parser, XpathFilter *filter, GumboVector *filters) {
    if (filter->type == NODE_STRING) {
        if (filter->op == NONE && gumbo_str_to_positive_integer(filter->name.data, filter->name.length, &filter->index)) {
            filter->type = NODE_INDEX;
        }
    } else if (filter->type == NODE_NUMERIC || filter->type == ATTR_NUMERIC) {
        gumbo_str_to_double(filter->value.data, filter->value.length, &filter->numeric_value);
    }
    gumbo_vector_add(parser, filter, filters);
}

static void gumbo_push_child_node(GumboParser *parser, GumboNode *parent_node, GumboVector *nodes) {
    GumboVector *children = NULL;
    if (parent_node->type == GUMBO_NODE_ELEMENT) {
        children = &parent_node->v.element.children;
    }
    if (children) {
        int i = 0, child_size = children->length;
        for (i = 0; i < child_size; i++) {
            GumboNode *child = (GumboNode *)children->data[i];
            if (child->type == GUMBO_NODE_ELEMENT) {
                gumbo_vector_add(parser, children->data[i], nodes);
            }
        }
    }
}

static bool gumbo_cmp(XpathFilterOp op, double left, double right) {
    bool is_matched = false;
    switch(op) {
    case LT:
        if (left < right) {
            is_matched = true;
        }
        break;
    case LE:
        if (left <= right) {
            is_matched = true;
        }
        break;
    case GT:
        if (left > right) {
            is_matched = true;
        }
        break;
    case GE:
       if (left >= right) {
            is_matched = true;
       }
       break;
    case EQ:
       if (left == right) {
            is_matched = true;
       }
       break;
    case NE:
       if (left != right) {
            is_matched = true;
       }
       break;
       default:
           break;
    }
    return is_matched;
}

static void gumbo_parse_xpath_seg_filter(GumboParser *parser, const char **xpath_ptr, GumboVector *filters) {
    const char *xpath = *xpath_ptr, *start_quotation_pos;
    char quotation;
    GumboStringPiece value_piece = {0};
    XpathFilter *filter = gumbo_new_xpath_filter(parser, NODE_STRING), *simple_filter;
    while (*xpath != '\0' && *xpath != ']') {
        switch(*xpath) {
        case '@':
            filter->type = ATTR_STRING;
            break;
        case '>':
            if (*(xpath+1) == '=') {
                filter->op = GE;
                xpath++;
            } else {
                filter->op = GT;
            }
            break;
        case '<':
            if (*(xpath+1) == '=') {
                filter->op = LE;
                xpath++;
            } else {
                filter->op = LT;
            }
            break;
        case '=':
            if (*(xpath-1) == '!') {
                filter->op = NE;
            } else {
                filter->op = EQ;
            }
            break;
        case '\'':
        case '"':
            quotation = *xpath;
            start_quotation_pos = ++xpath;
            for (; *xpath != quotation && *xpath != ']' && *xpath != '\0'; xpath++);
            if (*xpath == quotation) {
                value_piece.data = start_quotation_pos;
                value_piece.length = xpath - start_quotation_pos;
                gumbo_string_buffer_append_string(parser, &value_piece, &filter->value);
            } else {
                //TODO:sytax error
            }
            break;
        case '(':
            simple_filter = gumbo_new_xpath_filter(parser, LEFT_BRACKETS);
            gumbo_vector_add(parser, simple_filter, filters);
            break;
        case ')':
            gumbo_push_xpath_filter(parser, filter, filters);
            filter = gumbo_new_xpath_filter(parser, RIGHT_BRACKETS);
            break;
        case ' ':
            for (; *xpath == ' '; xpath++);
            if (*xpath != ']') {
                if ((*xpath == 'a' || *xpath == 'A') && (*(xpath + 1) == 'n' || *(xpath + 1) == 'N') 
                    && (*(xpath + 2) == 'd' || *(xpath + 2) == 'D') && *(xpath + 3) == ' ') {
                    gumbo_push_xpath_filter(parser, filter, filters);
                    filter = gumbo_new_xpath_filter(parser, AND);
                    gumbo_vector_add(parser, filter, filters);
                    filter = gumbo_new_xpath_filter(parser, NODE_STRING);
                    xpath += 3;
                } else if ((*xpath == 'o' || *xpath == 'O') && (*(xpath + 1) == 'r' || *(xpath + 1) == 'R') && *(xpath + 2) == ' ') {
                    gumbo_push_xpath_filter(parser, filter, filters);
                    filter = gumbo_new_xpath_filter(parser, OR);
                    gumbo_vector_add(parser, filter, filters);
                    filter = gumbo_new_xpath_filter(parser, NODE_STRING);
                    xpath += 2;
                } else {
                    //TODO:sytax error
                }
            }
            break;
            default:
                if (filter->op == NONE) {
                    gumbo_string_buffer_append_codepoint(parser, *xpath, &filter->name);
                } else {
                    if (filter->type == NODE_STRING) {
                        filter->type = NODE_NUMERIC;
                    } else if (filter->type == ATTR_STRING) {
                        filter->type = ATTR_NUMERIC;
                    }
                    gumbo_string_buffer_append_codepoint(parser, *xpath, &filter->value);
                }
                break;
        }
        xpath++;
    }
    if (*xpath != ']') {
        //TODO:sytax error
    }
    gumbo_push_xpath_filter(parser, filter, filters);
    *xpath_ptr = xpath;
}

void gumbo_compile_xpath(GumboParser *parser, const char *xpath, GumboVector *xpath_segs) {
    XpathSeg *xpath_seg = gumbo_new_xpath_seg(parser);
    while (*xpath != '\0') {
        switch(*xpath) {
        case '/':
            if (*(xpath - 1) == '/') {
                xpath_seg->is_deep_search = true;
            } else {
                if (xpath_seg->type != UNKNOWN_SEG) {
                    if (xpath_seg->type == DOC_NODE) {
                        xpath_seg->node_tag = gumbo_tagn_enum(xpath_seg->node_or_attr.data, xpath_seg->node_or_attr.length);
                    }
                    gumbo_vector_add(parser, xpath_seg, xpath_segs);
                    xpath_seg = gumbo_new_xpath_seg(parser);
                }
            }
            break;
        case '@':
            xpath_seg->type = DOC_NODE_ATTR;
            break;
        case '[':
            xpath++;
            gumbo_parse_xpath_seg_filter(parser, &xpath, &xpath_seg->filters);
            break;
            default:
                if (xpath_seg->type == UNKNOWN_SEG) {
                    xpath_seg->type = DOC_NODE;
                }
                gumbo_string_buffer_append_codepoint(parser, *xpath, &xpath_seg->node_or_attr);
                break;
        }
        xpath++;
    }
    if (xpath_seg->type == DOC_NODE) {
        xpath_seg->node_tag = gumbo_tagn_enum(xpath_seg->node_or_attr.data, xpath_seg->node_or_attr.length);
    }
    gumbo_vector_add(parser, xpath_seg, xpath_segs);
}

XpathSegType gumbo_eval_xpath_from_root(GumboParser *parser, GumboNode *root, const char *xpath, GumboVector *output) {
    XpathFilterType ret_type;
    GumboVector nodes = {0};
    gumbo_vector_init(parser, DEFAULT_VECTOR_SIZE, &nodes);
    gumbo_vector_add(parser, root, &nodes);
    ret_type = gumbo_eval_xpath_from_nodes(parser, &nodes, xpath, output);
    gumbo_vector_destroy(parser, &nodes);
    return ret_type;
}

static bool gumbo_is_filtered_ok(GumboParser *parser, XpathFilter *filters, int filter_num) {
    bool ret = false;
    int i = 0, j = 0, k = 0;
    XpathFilterType filter_type = OR;
    XpathFilter *filter;
    for (i = 0; i < filter_num; i++) {
        if (filters[i].type == RIGHT_BRACKETS) {
            for (j = i; j >= 0; j--) {
                if (filters[j].type == LEFT_BRACKETS) {
                    filter_type = OR;
                    ret = false;
                    for (k = j + 1; k <= i; k++) {
                        filter = &filters[k];
                        MULTI_BOOL_OPERATE(filter);
                        filter->type = UNKNOWN_FILTER;
                    }
                    filters[j].type = BOOL;
                    filters[j].bool_value = ret;
                    break;
                }
            }
        }
    }
    filter_type = OR;
    ret = false;
    for (i = 0; i < filter_num; i++) {
        filter = &filters[i];
        MULTI_BOOL_OPERATE(filter);
    }
    return ret;
}

static XpathSegType gumbo_do_filter(GumboParser *parser, GumboVector *src_nodes, XpathSeg *seg, bool is_last_node_seg, GumboVector *dsts) {
    GumboNode *src_node;
    int i = 0, j = 0, filter_num = 0;
    while ((src_node = (GumboNode *)gumbo_vector_pop(parser, src_nodes)) != NULL) {
        if (seg->is_deep_search) {
            gumbo_push_child_node(parser, src_node, src_nodes);
        }
        if (seg->type == DOC_NODE) {
            if (src_node->v.element.tag == seg->node_tag) {
                GumboVector *filter_vector = &seg->filters;
                XpathFilter *filter;
                if (filter_vector->length == 0) {
                    CONFIRM_DOC_NODE(src_node, dsts);
                    continue;
                } else if (((XpathFilter *)filter_vector->data[0])->type == NODE_INDEX) {
                    filter = filter_vector->data[i];
                    if (src_node->v.element.children.length >= filter->index) {
                        int k = 0;
                        for (j = 0; j < src_node->v.element.children.length; j++) {
                            GumboNode *child_node = (GumboNode *)src_node->v.element.children.data[j];
                            if (child_node->type != GUMBO_NODE_WHITESPACE && child_node->type != GUMBO_NODE_COMMENT) {
                                if (++k == filter->index) {
                                    CONFIRM_DOC_NODE(child_node, dsts);
                                    break;
                                }
                            }
                        }
                    }
                    continue;
                }
                filter_num = filter_vector->length;
                //XpathFilter filters[filter_num];
                XpathFilter filters_100[100];
                XpathFilter *filters = filters_100;
                if (filter_num > 100)
									filters = gumbo_parser_allocate(parser, filter_num * sizeof(filters[0]));

                for (i = 0; i < filter_num; i++) {
                    filter = filter_vector->data[i];
                    if (filter->type == NODE_NUMERIC || filter->type == NODE_STRING) {
                        filters[i].type = BOOL;
                        filters[i].bool_value = false;
                        GumboTag seg_filter_node_tag = gumbo_tagn_enum(filter->name.data, filter->name.length);
                        for (j = 0; j < src_node->v.element.children.length; j++) {
                            GumboNode *child_node = (GumboNode *)src_node->v.element.children.data[j];
                            if (child_node->type == GUMBO_NODE_ELEMENT && child_node->v.element.tag == seg_filter_node_tag) {
                                if (filter->op != NONE) {
                                    if (child_node->v.element.children.length == 1
                                        && ((GumboNode *)child_node->v.element.children.data[0])->type == GUMBO_NODE_TEXT) {
                                        GumboText *text_node = &((GumboNode *)child_node->v.element.children.data[0])->v.text;
                                        if (filter->type == NODE_NUMERIC) {
                                            double numeric_node;
                                            bool is_numeric_node = gumbo_str_to_double(text_node->text, strlen(text_node->text), &numeric_node);
                                            if (is_numeric_node && gumbo_cmp(filter->op, numeric_node, filter->numeric_value)) {
                                                filters[i].bool_value = true;
                                                break;
                                            }
                                        } else {
                                            if (strlen(text_node->text) == filter->value.length
                                                && !strncmp(text_node->text, filter->value.data, filter->value.length)) {
                                                filters[i].bool_value = true;
                                                break;
                                            }
                                        }
                                    }
                                } else {
                                    filters[i].bool_value = true;
                                    break;
                                }
                            }
                        }
                    } else if (filter->type == ATTR_NUMERIC || filter->type == ATTR_STRING) {
                        filters[i].type = BOOL;
                        filters[i].bool_value = false;
                        GumboVector *attrs = &src_node->v.element.attributes;
                        for (j = 0; j < attrs->length; j++) {
                            GumboAttribute *attr = (GumboAttribute *)attrs->data[j];
                            if (attr->name_end.column - attr->name_start.column == filter->name.length
                                && !strncmp(attr->name, filter->name.data, filter->name.length)) {
                                if (filter->type == ATTR_NUMERIC) {
                                    double numeric_attr = 0;
                                    bool is_numeric_attr = gumbo_str_to_double(attr->value, strlen(attr->value), &numeric_attr);
                                    if (is_numeric_attr && gumbo_cmp(filter->op, numeric_attr, filter->numeric_value)) {
                                        filters[i].bool_value = true;
                                    }
                                } else {
                                    if (filter->op != NONE) {
                                        if (strlen(attr->value) == filter->value.length && !strncmp(attr->value, filter->value.data, filter->value.length)) {
                                            filters[i].bool_value = true;
                                        }
                                    } else {
                                        filters[i].bool_value = true;
                                    }
                                }
                                break;
                            }
                        } 
                    } else {
                        filters[i].type = filter->type;
                    }
                } 
                if (gumbo_is_filtered_ok(parser, filters, filter_num)) {
                    CONFIRM_DOC_NODE(src_node, dsts);
                }

								if (filters != filters_100)
									gumbo_parser_deallocate(parser, filters);
            } 
        } else {
            GumboVector *attrs = &src_node->v.element.attributes;
            for (i = 0; i < attrs->length; i++) {
                GumboAttribute *attr = (GumboAttribute *)attrs->data[i];
                if (attr->name_end.column - attr->name_start.column == seg->node_or_attr.length
                    && !strncmp(attr->name, seg->node_or_attr.data, seg->node_or_attr.length)) {
                    gumbo_vector_add(parser, attr, dsts);
                    break;
                }
            }
        }
    }
    return seg->type; 
}

XpathSegType gumbo_eval_xpath_from_nodes(GumboParser* parser, GumboVector *doc_nodes, const char *xpath, GumboVector *output) {
    XpathFilterType ret_type = DOC_NODE;
    GumboVector xpath_segs;
    GumboVector *src_nodes = output, *dst_nodes = doc_nodes, *temp;
    XpathSeg *xpath_seg;
    gumbo_vector_init(parser, DEFAULT_VECTOR_SIZE, &xpath_segs);
    gumbo_compile_xpath(parser, xpath, &xpath_segs);
    int i = 0;
    for (i = 0; i < xpath_segs.length; i++) {
        xpath_seg = xpath_segs.data[i];
        temp = src_nodes;
        src_nodes = dst_nodes;
        dst_nodes = temp;
        if (i + 1 < xpath_segs.length && ((XpathSeg *)xpath_segs.data[i + 1])->type == DOC_NODE) {
            ret_type = gumbo_do_filter(parser, src_nodes, xpath_seg, false, dst_nodes);
        } else {
            ret_type = gumbo_do_filter(parser, src_nodes, xpath_seg, true, dst_nodes);
        }
        gumbo_free_xpath_seg(parser, xpath_seg);        
    }
    if (output != dst_nodes) {
        void *node;
        while ((node = gumbo_vector_pop(parser, dst_nodes)) != NULL) {
            gumbo_vector_add(parser, node, output);
        }
    }
    gumbo_vector_destroy(parser, &xpath_segs);
    return ret_type;
}

#if defined(GUMBO_DEBUG)

void gumbo_dump_xpath_segs(GumboParser *parser, GumboVector *xpath_segs) {
    int i = 0, j = 0;
    for (i = 0; i < xpath_segs->length; i++) {
        XpathSeg *xpath_seg = (XpathSeg *)xpath_segs->data[i];
        gumbo_debug("%s(%d,%d)", gumbo_string_buffer_cstr(parser, &xpath_seg->node_or_attr), xpath_seg->is_deep_search, xpath_seg->type);
        GumboVector *filters = &xpath_seg->filters;
        if (filters->length > 0) {
          gumbo_debug("[");
            for (j = 0; j < filters->length; j++) {
                XpathFilter *filter = filters->data[j];
              gumbo_debug("%d:", filter->type);
                switch(filter->type) {
                case NODE_INDEX:
                    gumbo_debug("%d", filter->index);
                    break;
                case NODE_NUMERIC:
                  gumbo_debug("%s", gumbo_string_buffer_cstr(parser, &filter->name));
                    switch(filter->op) {
                    case LE:
                        gumbo_debug("<=");
                        break;
                    case LT:
                      gumbo_debug("<");
                        break;
                    case GE:
                      gumbo_debug(">=");
                        break;
                    case GT:
                      gumbo_debug(">");
                        break;
                    case NE:
                      gumbo_debug("!=");
                        break;
                    case EQ:
                      gumbo_debug("=");
                        break;
                        default:
                            break;
                    }
                    gumbo_debug("%.02lf", filter->numeric_value);
                    break;
                case NODE_STRING:
                  gumbo_debug("%s", gumbo_string_buffer_cstr(parser, &filter->name));
                    if (filter->op != NONE) {
                        if (filter->op != EQ) {
                        gumbo_debug(" :: syntax error\n");
                        } else {
                          gumbo_debug("=%s", gumbo_string_buffer_cstr(parser, &filter->value));
                        }
                    }
                    break;
                case ATTR_NUMERIC:
                  gumbo_debug("@%s", gumbo_string_buffer_cstr(parser, &filter->name));
                    switch(filter->op) {
                    case LE:
                        gumbo_debug("<=");
                        break;
                    case LT:
                      gumbo_debug("<");
                        break;
                    case GE:
                      gumbo_debug(">=");
                        break;
                    case GT:
                      gumbo_debug(">");
                        break;
                    case NE:
                      gumbo_debug("!=");
                        break;
                    case EQ:
                      gumbo_debug("=");
                        break;
                        default:
                            break;
                    }
                    gumbo_debug("%.02lf", filter->numeric_value);
                    break;
                case ATTR_STRING:
                  gumbo_debug("@%s", gumbo_string_buffer_cstr(parser, &filter->name));
                    if (filter->op != NONE) {
                        if (filter->op != EQ) {
                        gumbo_debug(" :: syntax error\n");
                        } else {
                          gumbo_debug("=%s", gumbo_string_buffer_cstr(parser, &filter->value));
                        }
                    }
                    break;
                case LEFT_BRACKETS:
                  gumbo_debug("(");
                    break;
                case RIGHT_BRACKETS:
                  gumbo_debug(")");
                    break;
                case AND:
                  gumbo_debug(" and ");
                    break;
                case OR:
                  gumbo_debug(" or ");
                    break;
                    default:
                        break;
                }
            }
            gumbo_debug("]");
        }
        gumbo_debug("\n");
    }
}

#endif
