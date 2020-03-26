#include "angelix/ProposalHandler.h"
#include "../Core/Common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

using namespace std;
using namespace klee;

ProposalHandler::ProposalHandler() : proposal(NULL), p_table(NULL), i_table(NULL) {}

ProposalHandler::~ProposalHandler() {
  if (!p_table) delete p_table;
  if (!i_table) delete i_table;
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

  klee_message("[ProposalHandler] src_file: %s", src_file);
  klee_message("[ProposalHandler] loc: %s", loc.c_str());
  klee_message("[ProposalHandler] assemblyLine: %d", assemblyLine);
  klee_message("[ProposalHandler] line: %d", line);

  int res;
  if (bv.empty()) {
    klee_message("[ProposalHandler] bit vector is empty");
    theRNG.seed(time(0));
    if (theRNG.getBool()) {
      res = 1;
    } else {
      res = 0;
    }
  } else {
    int ins = (*i_table)[src_file][loc];
    if (ins < bv.size()) {
      res = bv[ins];
      (*i_table)[src_file][loc] = ins + 1;
      klee_message("[ProposalHandler] fetch %d", res);
      klee_message("[ProposalHandler] instance %d", ins);
    } else {
      klee_message("[ProposalHandler] bit vector is already used up");
      theRNG.seed(time(0));
      if (theRNG.getBool()) {
        res = 1;
      } else {
        res = 0;
      }
    }
  }

  return res;
}

int ProposalHandler::getPointerIndex(char* proposal_file,
                                     const char* src_file,
                                     unsigned assemblyLine, unsigned line,
                                     RNG& theRNG, int min_val, int max_val) {
  if (proposal == NULL) {
    proposal = loadProposal(proposal_file);
    makeProposalTable(proposal);
    makeInstanceTable(p_table);
  }

  ostringstream convert;
  convert << assemblyLine;
  string loc = convert.str();
  vector<int> bv = (*p_table)[src_file][loc];
  for (vector<int>::iterator it = bv.begin(); it != bv.end(); it++) {
    klee_message("[ProposalHandler] bit vector element: %d", *it);
  }

  klee_message("[ProposalHandler] src_file: %s", src_file);
  klee_message("[ProposalHandler] loc: %s", loc.c_str());
  klee_message("[ProposalHandler] assemblyLine: %d", assemblyLine);
  klee_message("[ProposalHandler] line: %d", line);
  klee_message("[ProposalHandler] min_val: %d", min_val);
  klee_message("[ProposalHandler] max_val: %d", max_val);

  int res;
  if (bv.empty()) {
    klee_message("[ProposalHandler] bit vector is empty");
    theRNG.seed(time(0));
    res = ((theRNG.getInt32() % max_val) + min_val) % max_val;
  } else {
    int ins = (*i_table)[src_file][loc];
    if (ins < bv.size()) {
      klee_message("[ProposalHandler] ins: %d", ins);
      res = bv[ins];
      (*i_table)[src_file][loc] = ins + 1;
      klee_message("[ProposalHandler] fetch %d", res);
      klee_message("[ProposalHandler] instance %d", ins);
    } else {
      klee_message("[ProposalHandler] bit vector is already used up");
      theRNG.seed(time(0));
      res = ((theRNG.getInt32() % max_val) + min_val) % max_val;
    }
  }

  return res;
}

cJSON* ProposalHandler::loadProposal(const char *proposal_file) {
  FILE *f = NULL;
  long len = 0;
  char *data = NULL;
  char *out = NULL;
  const cJSON *values_cjson = NULL;
  int idx = 0;

  /* delete the existing one */
  if (proposal != NULL) {
    klee_message("[ProposalHandler] proposal is already loaded");
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
    klee_message("[ProposalHandler] Error before: [%s]", cJSON_GetErrorPtr());
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
