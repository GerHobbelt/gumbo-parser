#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "xpath.h"

static void read_file(FILE* fp, char** output, int* length) {
  struct stat filestats;
  int fd = fileno(fp);
  fstat(fd, &filestats);
  *length = filestats.st_size;
  *output = (char *)malloc(*length + 1);
  int start = 0;
  int bytes_read;
  while ((bytes_read = fread(*output + start, 1, *length - start, fp))) {
    start += bytes_read;
  }
}

#if defined(BUILD_MONOLITHIC)
#define main gumbo_eval_xpath_main
#endif

int main(int argc, const char** argv) {
  if (argc != 3) {
      printf("evalxpath <html filename> <xpathexpression>\n");
      exit(EXIT_FAILURE);
  }
  const char* filename = argv[1];
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    printf("File %s not found!\n", filename);
    exit(EXIT_FAILURE);
  }
  const char* xpathexpression = argv[2];

  char* input;
  int input_length;
  GumboParser parser;
  parser._options = &kGumboDefaultOptions;
  read_file(fp, &input, &input_length);
  GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, input, input_length);
  GumboVector nodes = {0};
  gumbo_vector_init(&parser, DEFAULT_VECTOR_SIZE, &nodes);
  XpathSegType ret_type = gumbo_eval_xpath_from_root(&parser, output->root, xpathexpression, &nodes);
  GumboAttribute *attr;
  if (ret_type == DOC_NODE) {
      GumboNode *node;
      while ((node = (GumboNode *)gumbo_vector_pop(&parser, &nodes)) != NULL) {
          printf("%s\n", gumbo_normalized_tagname(node->v.element.tag));
          GumboVector *attrs = &node->v.element.attributes;
          printf("\t");
          while ((attr = (GumboAttribute *)gumbo_vector_pop(&parser, attrs)) != NULL) {
              printf(" %s=%s ", attr->name, attr->value);
          }
          printf("\n");
      }
  } else {
      while ((attr = (GumboAttribute *)gumbo_vector_pop(&parser, &nodes)) != NULL) {
          printf("%s=%s\n", attr->name, attr->value);
      } 
  }
  gumbo_vector_destroy(&parser, &nodes);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  free(input);
  return 0;
}
