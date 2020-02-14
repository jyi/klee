#include "angelix/ProposalHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include<iostream>

using namespace std;
using namespace klee;

ProposalHandler::ProposalHandler() : proposal(NULL) {}

ProposalHandler::~ProposalHandler() {
  // TODO
}

int ProposalHandler::getBranch(char* proposal_file,
                               const char* src_file, unsigned assemblyLine) {
  if (proposal == NULL) {
    proposal = loadProposal(proposal_file);
    makeProposalTable(proposal);
  }
  unordered_map<string, short*> m = file_map[src_file];
  // short *bv = file_map[src_file][assemblyLine];
  return 0;
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
  cJSON* file = NULL;
  cJSON_ArrayForEach(file, proposal) {
    unordered_map<string, short*> *loc_map = new unordered_map<string, short*>();
    cJSON* loc = NULL;
    cJSON_ArrayForEach(loc, file) {
      const cJSON* bit_vector = cJSON_GetObjectItemCaseSensitive(file, loc->string);
      short* bv = (short*) malloc(sizeof(short) * cJSON_GetArraySize(bit_vector));
      const cJSON* bit = NULL;
      int idx = 0;
      cJSON_ArrayForEach(bit, bit_vector) {
        bv[idx++] = bit->valueint;
      }
      (*loc_map)[loc->string] = bv;
    }
    file_map[file->string] = *loc_map;
  }
}
