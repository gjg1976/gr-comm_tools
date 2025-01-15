/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_COMM_TOOLS_qpsk_phase_ambiguity_solver_IMPL_H
#define INCLUDED_COMM_TOOLS_qpsk_phase_ambiguity_solver_IMPL_H

#include <gnuradio/comm_tools/qpsk_phase_ambiguity_solver.h>
#include <gnuradio/filter/fir_filter.h>
#include <volk/volk.h>
#include <gnuradio/thread/thread.h>

using namespace gr::filter::kernel;

namespace gr {
  namespace comm_tools {
    enum qpsk_phase {QPSK_PHASE_0 = 0, QPSK_PHASE_90, QPSK_PHASE_180, QPSK_PHASE_270, QPSK_PHASE_INV_0, QPSK_PHASE_INV_90, QPSK_PHASE_INV_180, QPSK_PHASE_INV_270, QPSK_PHASE_UNK};
    
    class qpsk_phase_ambiguity_solver_impl : public qpsk_phase_ambiguity_solver
    {
     private:
      // Nothing to declare in this block.
	    int d_tolerance;
	    int d_threshold;
	    int8_t d_status_phase; 
	    pmt::pmt_t d_key;
        bool d_lock;	
        	        
        int d_num_filter_taps;
	    fir_filter_fff *d_i_fir_filter = NULL;
	    fir_filter_fff *d_q_fir_filter = NULL;

	    static constexpr size_t s_map_size = 0x100;
	    unsigned char d_map[s_map_size];
	

    	void handle_msg_lock(pmt::pmt_t msg);

     public:
      qpsk_phase_ambiguity_solver_impl(const std::string& sync_word, int tolerance, const std::string& tag_lock_name);
      ~qpsk_phase_ambiguity_solver_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
      
      void set_sync_word(const std::string& sync_word) override;
      void set_tolerance(int tolerance) override { d_tolerance = tolerance; d_threshold = d_num_filter_taps - d_tolerance;};
    };

  } // namespace comm_tools
} // namespace gr

#endif /* INCLUDED_COMM_TOOLS_qpsk_phase_ambiguity_solver_IMPL_H */
