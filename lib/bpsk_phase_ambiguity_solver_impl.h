/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_COMM_TOOLS_BPSK_PHASE_AMBIGUITY_SOLVER_IMPL_H
#define INCLUDED_COMM_TOOLS_BPSK_PHASE_AMBIGUITY_SOLVER_IMPL_H

#include <gnuradio/comm_tools/bpsk_phase_ambiguity_solver.h>
#include <gnuradio/filter/fir_filter.h>
#include <volk/volk.h>
#include <gnuradio/thread/thread.h>

using namespace gr::filter::kernel;

namespace gr {
  namespace comm_tools {
    enum bpsk_phase {BPSK_PHASE_0 = 0, BPSK_PHASE_180 = 2, BPSK_PHASE_UNK = 8};
    
    class bpsk_phase_ambiguity_solver_impl : public bpsk_phase_ambiguity_solver
    {
     private:

	    int d_tolerance;
	    int8_t d_status_phase; 
	    pmt::pmt_t d_key;
        bool d_lock;	
        	        
        int d_num_filter_taps;
	    fir_filter_fff *d_fir_filter = NULL;

	    static constexpr size_t s_map_size = 0x100;
	    unsigned char d_map[s_map_size];
	
    	void handle_msg_lock(pmt::pmt_t msg);

     public:
      bpsk_phase_ambiguity_solver_impl(const std::string& sync_word, int tolerance, const std::string& tag_lock_name);
      ~bpsk_phase_ambiguity_solver_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
      
      
      void set_sync_word(const std::string& sync_word) override;
      void set_tolerance(int tolerance) override { d_tolerance = tolerance; };
    };

  } // namespace comm_tools
} // namespace gr

#endif /* INCLUDED_COMM_TOOLS_BPSK_PHASE_AMBIGUITY_SOLVER_IMPL_H */
