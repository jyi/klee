#ifndef KLEE_PROPOSAL_HANDLER_MANGER_H
#define KLEE_PROPOSAL_HANDLER_MANAGER_H

#include "angelix/ProposalHandler.h"
#include <tr1/unordered_map>
#include <string>

using namespace std;
using namespace std::tr1;

namespace klee {

  class ProposalHandlerManager {
  public:
    ProposalHandlerManager();
    ~ProposalHandlerManager();

    static ProposalHandler* getProposalHandler(string proposal_file);

  private:
    static unordered_map<string, ProposalHandler*> phMap;
  };

} // End klee namespace

#endif
