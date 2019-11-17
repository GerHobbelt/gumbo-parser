#include "xpath_eval.h"

void gumbo_init_xpath_seg(GumboParser* parser, XpathFilterType seg_type, XpathSeg *seg) {
    seg->seg_type = seg_type;
    if (seg_type == DOC_NODE) {
        gumbo_string_buffer_init(parser, &seg->node);
    } else {
        gumbo_string_buffer_init(parser, &seg->attr);
    }
    gumbo_string_buffer_init(parser, &seg->filter.name); 
    gumbo_string_buffer_init(parser, &seg->filter.value); 
}

void gumbo_reset_xpath_seg(GumboParser *parser, XpathSeg *seg) {
    if (seg->seg_type == DOC_NODE) {
        gumbo_string_buffer_clear(parser, &seg->node);
    } else {
        gumbo_string_buffer_clear(parser, &seg->attr);
    }
    gumbo_string_buffer_clear(parser, &seg->filter.name);
    gumbo_string_buffer_init(parser, &seg->filter.value); 
}

void gumbo_destroy_xpath_seg(GumboParser *parser, XpathSeg *seg) {
    if (seg->seg_type == DOC_NODE) {
        gumbo_string_buffer_destroy(parser, &seg->node);
    } else {
        gumbo_string_buffer_destroy(parser, &seg->attr);
    }
    gumbo_string_buffer_destroy(parser, &seg->filter.name);
    gumbo_string_buffer_destroy(parser, &seg->filter.value);
}

XpathFilterType gumbo_eval_xpath_from_root(GumboParser *parser, GumboNode *root, const char *xpath, GumboVector *output) {
    XpathFilterType ret_type;
    GumboVector nodes = {0};
    gumbo_vector_init(parser, );
    return ret_type;    
}

XpathFilterType gumbo_eval_xpath_from_nodes(GumboParser *parser, GumboVector *doc_nodes, const char *xpath, GumboVector *output) {

}

