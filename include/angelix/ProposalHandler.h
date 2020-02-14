#ifndef KLEE_PROPOSAL_HANDLER_H
#define KLEE_PROPOSAL_HANDLER_H

#include "cJSON.h"
#include "klee/Internal/ADT/RNG.h"
#include <tr1/unordered_map>
#include <string>

using namespace std;
using namespace std::tr1;

typedef unordered_map<string, unordered_map<string, short*> > proposal_table;
typedef unordered_map<string, unordered_map<string, int> > instance_table;

namespace klee {

class ProposalHandler {
public:
  ProposalHandler();
  ~ProposalHandler();
  int getBranch(char* proposal_file,
                const char* file, unsigned assemblyLine,
                RNG& theRNG);

private:
  cJSON* proposal;

  proposal_table* p_table;
  instance_table* i_table;

  cJSON* loadProposal(char* proposal_file);
  void makeProposalTable(cJSON* proposal);
  void makeInstanceTable(proposal_table* p_table);
};

} // End klee namespace

#endif
