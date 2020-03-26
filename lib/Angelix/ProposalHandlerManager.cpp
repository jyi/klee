#include "angelix/ProposalHandler.h"
#include "angelix/ProposalHandlerManager.h"

using namespace std;
using namespace klee;

unordered_map<string, ProposalHandler*> ProposalHandlerManager::phMap;

ProposalHandlerManager::ProposalHandlerManager() {}

ProposalHandlerManager::~ProposalHandlerManager() {}

ProposalHandler*
ProposalHandlerManager::getProposalHandler(string proposal_file) {
  ProposalHandler* handler = phMap[proposal_file];
  if (handler) {
    return handler;
  }

  handler = new ProposalHandler();
  phMap[proposal_file] = handler;
  return handler;
}
