#ifndef KLEE_PROPOSAL_HANDLER_H
#define KLEE_PROPOSAL_HANDLER_H

namespace klee {

class ProposalHandler {
public:
  ProposalHandler();
  ~ProposalHandler();
  int getBranch(const char* file, unsigned assemblyLine);
};

} // End klee namespace

#endif
