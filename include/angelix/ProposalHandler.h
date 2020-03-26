#ifndef KLEE_PROPOSAL_HANDLER_H
#define KLEE_PROPOSAL_HANDLER_H

#include "cJSON.h"
#include "klee/Internal/ADT/RNG.h"
#include <tr1/unordered_map>
#include <string>
#include <vector>
#include <stdint.h>

using namespace std;
using namespace std::tr1;

typedef unordered_map<string, unordered_map<string, vector<int> > > proposal_table;
typedef unordered_map<string, unordered_map<string, int> > instance_table;

namespace klee {

class ProposalHandler {
public:
  ProposalHandler();
  ~ProposalHandler();
  int getBranch(char* proposal_file,
                const char* src_file,
                unsigned assemblyLine, unsigned line,
                RNG& theRNG);
  int getPointerIndex(char* proposal_file,
                      const char* src_file,
                      unsigned assemblyLine, unsigned line,
                      RNG& theRNG, int min_val, int max_val);

private:
  cJSON* proposal;

  proposal_table* p_table;
  instance_table* i_table;

  cJSON* loadProposal(const char* proposal_file);
  void makeProposalTable(cJSON* proposal);
  void makeInstanceTable(proposal_table* p_table);
};

} // End klee namespace

#endif
