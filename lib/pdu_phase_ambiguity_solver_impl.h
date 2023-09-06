/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_COMM_TOOLS_PDU_PHASE_AMBIGUITY_SOLVER_IMPL_H
#define INCLUDED_COMM_TOOLS_PDU_PHASE_AMBIGUITY_SOLVER_IMPL_H

#include <gnuradio/comm_tools/pdu_phase_ambiguity_solver.h>
#include <gnuradio/filter/fir_filter.h>
#include <volk/volk.h>
#include <gnuradio/thread/thread.h>

using namespace gr::filter::kernel;

namespace gr {
  namespace comm_tools {
        
    class pdu_phase_ambiguity_solver_impl : public pdu_phase_ambiguity_solver
    {
     private:
      // Nothing to declare in this block.
	int d_tolerance;
	int d_threshold;
	int8_t d_status_phase; 
	int8_t d_modulation;
			        
        int d_num_filter_taps;
	fir_filter_fff *d_fir_1_filter = NULL;
	fir_filter_fff *d_fir_2_filter = NULL;

	static constexpr size_t s_map_size = 0x100;
	unsigned char d_map[s_map_size];
	
	pmt::pmt_t d_key;

    	void handle_msg_qpsk(pmt::pmt_t pdu);
    	void handle_msg_bpsk(pmt::pmt_t pdu);
    	void handle_msg_passthrough(pmt::pmt_t pdu);
    
     public:
      pdu_phase_ambiguity_solver_impl(uint8_t modulation, const std::string& sync_word, int tolerance, const std::string& tag_lock_name);
      ~pdu_phase_ambiguity_solver_impl();

      void set_sync_word(const std::string& sync_word) override;
      void set_tolerance(int tolerance) override { d_tolerance = tolerance; d_threshold = d_num_filter_taps - d_tolerance;};

    };

    enum phase {PHASE_0 = 0, PHASE_90, PHASE_180, PHASE_270, PHASE_INV_0, PHASE_INV_90, PHASE_INV_180, PHASE_INV_270, PHASE_INVALID = 255};
    enum modulation { MOD_BPSK = 0, MOD_QPSK = 1 };
  } // namespace comm_tools
} // namespace gr

#endif /* INCLUDED_COMM_TOOLS_PDU_PHASE_AMBIGUITY_SOLVER_IMPL_H */
