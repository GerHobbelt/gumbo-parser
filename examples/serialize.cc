// Copyright 2015-2017 Kevin B. Hendricks, Stratford, Ontario,  All Rights Reserved.
// loosely based on a greatly simplified version of BeautifulSoup4 decode() routine
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
// Author: Kevin Hendricks
//
// Serialize back to html / xhtml making as few changes as possible (even in whitespace)

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unordered_set>

#include "gumbo.h"

static std::unordered_set<std::string> nonbreaking_inline  = { 
  "a","abbr","acronym","b","bdo","big","br","button","cite","code","del",
  "dfn","em","font","i","image","img","input","ins","kbd","label","map",
  "nobr","object","q","ruby","rt","s","samp","select","small","span",
  "strike","strong","sub","sup","textarea","tt","u","var","wbr"
};


static std::unordered_set<std::string> preserve_whitespace = {
  "pre","textarea","script","style"
};


static std::unordered_set<std::string> special_handling    = { 
  "html","body"
};


static std::unordered_set<std::string> no_entity_sub       = {
  "script","style"
};


static std::unordered_set<std::string> void_tags          = {
  "area","base","basefont","bgsound","br","command","col","embed",
  "event-source","frame","hr","img","input","keygen","link",
  "menuitem","meta","param","source","spacer","track","wbr"
};


static inline bool in_set(std::unordered_set<std::string> &s, std::string &key)
{
  return s.find(key) != s.end();
}


// These need to match the GumboAttributeNamespaceEnum sequence
static const char * attribute_nsprefixes[4] = { "", "xlink:", "xml:", "xmlns:" };

static inline void rtrim(std::string &s) 
{
  s.erase(s.find_last_not_of(" \n\r\t\v\f")+1);
}


static inline void ltrim(std::string &s)
{
  s.erase(0,s.find_first_not_of(" \n\r\t\v\f"));
}


static void replace_all(std::string &s, const char * s1, const char * s2)
{
  std::string t1(s1);
  size_t len = t1.length();
  size_t pos = s.find(t1);
  while (pos != std::string::npos) {
    s.replace(pos, len, s2);
    pos = s.find(t1, pos + len);
  }
}


static std::string substitute_xml_entities_into_text(const std::string &text)
{
  std::string result = text;
  // replacing & must come first 
  replace_all(result, "&", "&amp;");
  replace_all(result, "<", "&lt;");
  replace_all(result, ">", "&gt;");
  return result;
}


static std::string substitute_xml_entities_into_attributes(char quote, const std::string &text)
{
  std::string result = substitute_xml_entities_into_text(text);
  if (quote == '"') {
    replace_all(result,"\"","&quot;");
  }    
  else if (quote == '\'') {
    replace_all(result,"'","&apos;");
  }
 return result;
}

static std::string get_tag_name(GumboNode *node)
{
  std::string tagname;
  if (node->type == GUMBO_NODE_DOCUMENT) {
    tagname = "#document";
    return tagname;
  } else if ((node->type == GUMBO_NODE_TEXT) || (node->type == GUMBO_NODE_WHITESPACE)) {
    tagname = "#text";
    return tagname;
  } else if (node->type == GUMBO_NODE_CDATA) {
    tagname = "#cdata";
    return tagname;
  }
  tagname = gumbo_normalized_tagname(node->v.element.tag);
  if ((tagname.empty()) ||
      (node->v.element.tag_namespace == GUMBO_NAMESPACE_SVG)) {

    // set up to examine original text of tag.
    GumboStringPiece gsp = node->v.element.original_tag;
    gumbo_tag_from_original_text(&gsp);

    // special handling for some svg tag names.
    if (node->v.element.tag_namespace  == GUMBO_NAMESPACE_SVG) {
      const char * data = gumbo_normalize_svg_tagname(&gsp);
      // NOTE: data may not be null-terminated!
      // since case change only - length must be same as original.
      // if no replacement found returns null, not original tag!
      if (data != NULL) {
        return std::string(data, gsp.length);
      }
    }
    if (tagname.empty()) {
      return std::string(gsp.data, gsp.length);
    }
  }
  return tagname;
}


static std::string build_doctype(GumboNode *node)
{
  std::string results = "";
  if (node->v.document.has_doctype) {
    results.append("<!DOCTYPE ");
    results.append(node->v.document.name);
    std::string pi(node->v.document.public_identifier);
    if ((node->v.document.public_identifier != NULL) && !pi.empty() ) {
        results.append(" PUBLIC \"");
        results.append(node->v.document.public_identifier);
        results.append("\"\n    \"");
        results.append(node->v.document.system_identifier);
        results.append("\"");
    }
    results.append(">\n");
  }
  return results;
}

// handle the known foreign attribute namespaces
static std::string get_attribute_name(GumboAttribute * at)
{
  std::string attr_name = at->name;
  GumboAttributeNamespaceEnum attr_ns = at->attr_namespace;
  if ((attr_ns == GUMBO_ATTR_NAMESPACE_NONE) || (attr_name == "xmlns")) {
    return attr_name;
  } 
  attr_name = std::string(attribute_nsprefixes[attr_ns]) + attr_name;
  return attr_name;
}


static std::string build_attributes(GumboAttribute * at, bool no_entities)
{
  std::string atts = " ";
  std::string name = get_attribute_name(at);
  atts.append(name);
  std::string attvalue = at->value;

  // how do we want to handle attributes with empty values
  // <input type="checkbox" checked />  or <input type="checkbox" checked="" /> 
  // for now we handle empty attribute values like so: alt=""

  char quote = '"';
  std::string qs="\"";

  if ( (!attvalue.empty())   || 
       (at->original_value.data[0] == '"') || 
       (at->original_value.data[0] == '\'') ) {

    // determine original quote character used if it exists
    quote = at->original_value.data[0];
    if (quote == '\'') qs = std::string("'");
    if (quote == '"') qs = std::string("\"");
  }

  atts.append("=");
  atts.append(qs);
  if (no_entities) {
    atts.append(attvalue);
  } else {
    atts.append(substitute_xml_entities_into_attributes(quote, attvalue));
  }
  atts.append(qs);
  return atts;
}


// forward declaration
static std::string serialize(GumboNode*);


// serialize children of a node
// may be invoked recursively

static std::string serialize_contents(GumboNode* node) {
  std::string contents        = "";
  std::string tagname         = get_tag_name(node);
  bool no_entity_substitution = in_set(no_entity_sub, tagname);
  bool keep_whitespace        = in_set(preserve_whitespace, tagname);
  bool is_inline              = in_set(nonbreaking_inline, tagname);

  // build up result for each child, recursively if need be
  GumboVector* children = &node->v.element.children;

  for (unsigned int i = 0; i < children->length; ++i) {
    GumboNode* child = static_cast<GumboNode*> (children->data[i]);

    if (child->type == GUMBO_NODE_TEXT) {
      if (no_entity_substitution) {
        contents.append(std::string(child->v.text.text));
      } else {
        contents.append(substitute_xml_entities_into_text(std::string(child->v.text.text)));
      }

    } else if (child->type == GUMBO_NODE_ELEMENT || child->type == GUMBO_NODE_TEMPLATE) {
      contents.append(serialize(child));

    } else if (child->type == GUMBO_NODE_WHITESPACE) {
      // keep all whitespace to keep as close to original as possible
      contents.append(std::string(child->v.text.text));

    } else if (child->type == GUMBO_NODE_CDATA) {
      contents.append("<![CDATA[" + std::string(child->v.text.text) + "]]>");

    } else if (child->type == GUMBO_NODE_COMMENT) {
      contents.append("<!--" + std::string(child->v.text.text) + "-->");

    } else {
      fprintf(stderr, "unknown element of type: %d\n", child->type); 
    }
  }
  return contents;
}


// serialize a GumboNode back to html/xhtml
// may be invoked recursively

static std::string serialize(GumboNode* node) {
  // special case the document node
  if (node->type == GUMBO_NODE_DOCUMENT) {
    std::string results = build_doctype(node);
    results.append(serialize_contents(node));
    return results;
  }

  std::string close = "";
  std::string closeTag = "";
  std::string atts = "";
  std::string tagname            = get_tag_name(node);
  bool need_special_handling     = in_set(special_handling, tagname);
  bool is_void_tag              = in_set(void_tags, tagname);
  bool no_entity_substitution    = in_set(no_entity_sub, tagname);

  // build attr string  
  const GumboVector * attribs = &node->v.element.attributes;
  for (int i=0; i< attribs->length; ++i) {
    GumboAttribute* at = static_cast<GumboAttribute*>(attribs->data[i]);
    atts.append(build_attributes(at, no_entity_substitution));
  }

  // determine closing tag type
  if (is_void_tag) {
      close = "/";
  } else {
      closeTag = "</" + tagname + ">";
  }

  // serialize your contents
  std::string contents = serialize_contents(node);

  if (need_special_handling) {
    ltrim(contents);
    rtrim(contents);
    contents.append("\n");
  }

  // build results
  std::string results;
  results.append("<"+tagname+atts+close+">");
  if (need_special_handling) results.append("\n");
  results.append(contents);
  results.append(closeTag);
  if (need_special_handling) results.append("\n");
  return results;
}


#if defined(BUILD_MONOLITHIC)
#define main		gumbo_serialize_main
#endif

int main(int argc, const char** argv) {
  if (argc != 2) {
      std::cout << "serialize <html filename>\n";
      return EXIT_FAILURE;
  }
  const char* filename = argv[1];

  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (!in) {
    std::cout << "File " << filename << " not found!\n";
    return EXIT_FAILURE;
  }

  std::string contents;
  in.seekg(0, std::ios::end);
  contents.resize(in.tellg());
  in.seekg(0, std::ios::beg);
  in.read(&contents[0], contents.size());
  in.close();
 
  GumboOptions options = kGumboDefaultOptions;

  GumboOutput* output = gumbo_parse_with_options(&options, contents.data(), contents.length());
  std::cout << serialize(output->document) << std::endl;
  gumbo_destroy_output(&options, output);

  return EXIT_SUCCESS;
}
