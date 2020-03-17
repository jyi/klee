#include "angelix/ProposalHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;
using namespace klee;

ProposalHandler::ProposalHandler() : proposal(NULL), p_table(NULL), i_table(NULL) {}

ProposalHandler::~ProposalHandler() {
  delete p_table;
  delete i_table;
}

int ProposalHandler::getBranch(char* proposal_file,
                               const char* src_file,
                               unsigned assemblyLine, unsigned line,
                               RNG& theRNG) {
  if (proposal == NULL) {
    proposal = loadProposal(proposal_file);
    makeProposalTable(proposal);
    makeInstanceTable(p_table);
  }

  ostringstream convert;
  convert << assemblyLine;
  string loc = convert.str();
  vector<int> bv = (*p_table)[src_file][loc];

  fprintf(stderr, "[ProposalHandler] src_file: %s\n", src_file);
  fprintf(stderr, "[ProposalHandler] loc: %s\n", loc.c_str());
  fprintf(stderr, "[ProposalHandler] assemblyLine: %d\n", assemblyLine);
  fprintf(stderr, "[ProposalHandler] line: %d\n", line);

  int res;
  if (bv.empty()) {
    fprintf(stderr, "[ProposalHandler] bv is empty\n");
    if (theRNG.getBool()) {
      res = 1;
    } else {
      res = 0;
    }
  } else {
    int ins = (*i_table)[src_file][loc];
    res = bv[ins];
    (*i_table)[src_file][loc] = ins + 1;
    fprintf(stderr, "[ProposalHandler] fetch %d\n", res);
    fprintf(stderr, "[ProposalHandler] instance %d\n", ins);
  }

  return res;
}

cJSON* ProposalHandler::loadProposal(char *proposal_file) {
  FILE *f = NULL;
  long len = 0;
  char *data = NULL;
  char *out = NULL;
  const cJSON *values_cjson = NULL;
  int idx = 0;

  /* delete the existing one */
  if (proposal != NULL) {
    fprintf(stderr, "[ProposalHandler] proposal is already loaded\n");
    exit(1);
  }

  /* open in read binary mode */
  f = fopen(proposal_file, "rb");
  /* get the length */
  fseek(f, 0, SEEK_END);
  len = ftell(f);
  fseek(f, 0, SEEK_SET);

  data = (char*)malloc(len + 1);

  fread(data, 1, len, f);
  data[len] = '\0';
  fclose(f);

  /* parase the json text */
  proposal = cJSON_Parse(data);
  if (!proposal) {
    fprintf(stderr, "[ProposalHandler] Error before: [%s]\n", cJSON_GetErrorPtr());
    exit(1);
  }

  return proposal;
}

void ProposalHandler::makeProposalTable(cJSON* proposal) {
  p_table = new proposal_table();
  cJSON* file = NULL;
  cJSON_ArrayForEach(file, proposal) {
    unordered_map<string, vector<int> > *loc_map = new unordered_map<string, vector<int> >();
    cJSON* loc = NULL;
    cJSON_ArrayForEach(loc, file) {
      const cJSON* bit_vector = cJSON_GetObjectItemCaseSensitive(file, loc->string);
      vector<int> *bv = new vector<int>();
      const cJSON* bit = NULL;
      cJSON_ArrayForEach(bit, bit_vector) {
        bv->push_back(bit->valueint);
      }
      (*loc_map)[loc->string] = *bv;
    }
    (*p_table)[file->string] = *loc_map;
  }
}

void ProposalHandler::makeInstanceTable(proposal_table* p_table) {
  i_table = new instance_table();
  for (proposal_table::iterator it = p_table->begin(); it != p_table->end(); it++) {
    string file = it->first;
    unordered_map<string, vector<int> > loc_map = it->second;
    unordered_map<string, int> *loc_map2 = new unordered_map<string, int>();
    for (unordered_map<string, vector<int> >::iterator it2 = loc_map.begin(); it2 != loc_map.end(); it2++) {
      (*loc_map2)[it2->first] = 0;
    }
    (*i_table)[file] = *loc_map2;
  }
}
