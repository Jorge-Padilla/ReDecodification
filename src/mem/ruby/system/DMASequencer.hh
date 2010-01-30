
#ifndef DMASEQUENCER_H
#define DMASEQUENCER_H

#include <ostream>
#include "mem/ruby/common/DataBlock.hh"
#include "mem/ruby/system/RubyPort.hh"

#include "params/DMASequencer.hh"

struct DMARequest {
  uint64_t start_paddr;
  int len;
  bool write;
  int bytes_completed;
  int bytes_issued;
  uint8* data;
  int64_t id;
};

class DMASequencer :public RubyPort {
public:
    typedef DMASequencerParams Params;
  DMASequencer(const Params *);
  void init();
  /* external interface */
  int64_t makeRequest(const RubyRequest & request);
  bool isReady(const RubyRequest & request, bool dont_set = false) { assert(0); return false;};
  //  void issueRequest(uint64_t paddr, uint8* data, int len, bool rw);
  bool busy() { return m_is_busy;}

  /* SLICC callback */
  void dataCallback(const DataBlock & dblk);
  void ackCallback();

  void printConfig(std::ostream & out);

private:
  void issueNext();

private:
  bool m_is_busy;
  uint64_t m_data_block_mask;
  DMARequest active_request;
  int num_active_requests;
};

#endif // DMACONTROLLER_H
