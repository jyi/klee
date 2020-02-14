#ifndef KLEE_PROPOSAL_HANDLER_H
#define KLEE_PROPOSAL_HANDLER_H

#include "cJSON.h"
#include <tr1/unordered_map>
#include <string>

using namespace std;
using namespace std::tr1;

typedef unordered_map<string, unordered_map<string, short*> > proposal_table;

namespace klee {

class ProposalHandler {
public:
  ProposalHandler();
  ~ProposalHandler();
  int getBranch(char* proposal_file,
                const char* file, unsigned assemblyLine);

private:
  cJSON* proposal;

  unordered_map<string, unordered_map<string, short*> > file_map;

  cJSON* loadProposal(char* proposal_file);
  void makeProposalTable(cJSON* proposal);
};

} // End klee namespace

#endif
