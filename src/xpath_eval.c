
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"
#include "util.h"
#include "xpath_eval.h"

static XpathSeg *gumbo_new_xpath_seg(GumboParser* parser) {
    XpathSeg *seg = (XpathSeg *)malloc(sizeof(XpathSeg));
    seg->type = UNKNOWN_SEG;
    seg->is_deep_search = false;
    gumbo_string_buffer_init(parser, &seg->node_or_attr); 
    gumbo_vector_init(parser, DEFAULT_VECTOR_SIZE, &seg->filters); 
    return seg;
}

static void gumbo_free_xpath_seg(GumboParser *parser, XpathSeg *xpath_seg) {
    gumbo_string_buffer_destroy(parser, &xpath_seg->node_or_attr);
    gumbo_vector_destroy(parser, &xpath_seg->filters);
    free(xpath_seg);
}

static XpathFilter *gumbo_new_xpath_filter(GumboParser *parser, XpathFilterType filter_type) {
    XpathFilter *filter = (XpathFilter *)malloc(sizeof(XpathFilter));
    filter->type = filter_type;
    if (filter_type == NODE_STRING) {
        gumbo_string_buffer_init(parser, &filter->name); 
        gumbo_string_buffer_init(parser, &filter->value); 
    }
    return filter;
}

static void gumbo_free_xpath_filter(GumboParser *parser, XpathFilter *filter) {
    gumbo_string_buffer_destroy(parser, &filter->name);
    gumbo_string_buffer_destroy(parser, &filter->value);
    free(filter);
}

static void gumbo_push_xpath_filter(GumboParser *parser, XpathFilter *filter, GumboVector *filters) {
    if (filter->type == NODE_STRING) {
        if (filter->op == NONE && gumbo_str_to_positive_integer(filter->name.data, filter->name.length, &filter->index)) {
            filter->type = NODE_INDEX;
        }
    } else if (filter->type == NODE_NUMERIC || filter->type == ATTR_NUMERIC) {
        gumbo_str_to_double(filter->value.data, filter->value.length, &filter->double_value);
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
            gumbo_vector_add(parser, children->data[i], nodes);
        }
    }
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
    if (*xpath == ']') {
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

static XpathSegType gumbo_do_filter(GumboParser *parser, GumboVector *src_nodes, XpathSeg *seg, GumboVector *dsts) {
    GumboNode *src_node;
    int i = 0;
    while ((src_node = (GumboNode *)gumbo_vector_pop(parser, src_nodes)) != NULL) {
        if (seg->type == DOC_NODE) {
            if (src_node->v.element.tag == seg->node_tag) {
                GumboVector *filters = &seg->filters;
                XpathFilter *filter;
                for (i = 0; i < filters->length; i++) {
                    filter = filters->data[i];
                    
                } 
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
        if (seg->is_deep_search) {
            gumbo_push_child_node(parser, src_node, src_nodes);
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
        ret_type = gumbo_do_filter(parser, src_nodes, xpath_seg, dst_nodes);
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

void gumbo_dup_xpath_segs(GumboVector *xpath_segs) {
    int i = 0, j = 0;
    for (i = 0; i < xpath_segs->length; i++) {
        XpathSeg *xpath_seg = (XpathSeg *)xpath_segs->data[i];
        xpath_seg->node_or_attr.data[xpath_seg->node_or_attr.length] = '\0';
        printf("%d:%s", xpath_seg->is_deep_search, xpath_seg->node_or_attr.data);
        GumboVector *filters = &xpath_seg->filters;
        if (filters->length > 0) {
            printf("[");
            for (j = 0; j < filters->length; j++) {
                XpathFilter *filter = filters->data[j];
                switch(filter->type) {
                case NODE_INDEX:
                    printf("%d", filter->index);
                    break;
                case NODE_NUMERIC:
                    filter->name.data[filter->name.length] = '\0';
                    printf("%s", filter->name.data);
                    switch(filter->op) {
                    case LE:
                        printf("<=");
                        break;
                    case LT:
                        printf("<");
                        break;
                    case GE:
                        printf(">=");
                        break;
                    case GT:
                        printf(">");
                        break;
                    case NE:
                        printf("!=");
                        break;
                    case EQ:
                        printf("=");
                        break;
                        default:
                            break;
                    }
                    printf("%.02lf", filter->double_value);
                    break;
                case NODE_STRING:
                    filter->name.data[filter->name.length] = '\0';
                    printf("%s", filter->name.data);
                    if (filter->op != NONE) {
                        if (filter->op != EQ) {
                            printf("systax error\n");
                        } else {
                            filter->value.data[filter->value.length] = '\0';
                            printf("=%s", filter->value.data);
                        }
                    }
                    break;
                case ATTR_NUMERIC:
                    filter->name.data[filter->name.length] = '\0';
                    printf("@%s", filter->name.data);
                    switch(filter->op) {
                    case LE:
                        printf("<=");
                        break;
                    case LT:
                        printf("<");
                        break;
                    case GE:
                        printf(">=");
                        break;
                    case GT:
                        printf(">");
                        break;
                    case NE:
                        printf("!=");
                        break;
                    case EQ:
                        printf("=");
                        break;
                        default:
                            break;
                    }
                    printf("%.02lf", filter->double_value);
                    break;
                case ATTR_STRING:
                    filter->name.data[filter->name.length] = '\0';
                    printf("@%s", filter->name.data);
                    if (filter->op != NONE) {
                        if (filter->op != EQ) {
                            printf("systax error\n");
                        } else {
                            filter->value.data[filter->value.length] = '\0';
                            printf("=%s", filter->value.data);
                        }
                    }
                    break;
                case LEFT_BRACKETS:
                    printf("(");
                    break;
                case RIGHT_BRACKETS:
                    printf(")");
                    break;
                case AND:
                    printf(" and ");
                    break;
                case OR:
                    printf(" or ");
                    break;
                    default:
                        break;
                }
            }
            printf("]");
        }
        printf("\n");
    }
}
