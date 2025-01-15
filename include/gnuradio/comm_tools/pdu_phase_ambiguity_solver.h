/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_COMM_TOOLS_PDU_PHASE_AMBIGUITY_SOLVER_H
#define INCLUDED_COMM_TOOLS_PDU_PHASE_AMBIGUITY_SOLVER_H

#include <gnuradio/comm_tools/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace comm_tools {

    class COMM_TOOLS_API pdu_phase_ambiguity_solver : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<pdu_phase_ambiguity_solver> sptr;

      static sptr make(uint8_t modulation, const std::string& sync_word, int tolerance, const std::string& tag_lock_name);

      virtual void set_sync_word(const std::string& sync_word) = 0;
      virtual void set_tolerance(int tolerance) = 0;   
    };

  } // namespace comm_tools
} // namespace gr

#endif /* INCLUDED_COMM_TOOLS_PDU_PHASE_AMBIGUITY_SOLVER_H */
