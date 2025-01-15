/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "bpsk_phase_ambiguity_solver_impl.h"

#define NIBBLE_SIZE 4


namespace gr {
  namespace comm_tools {
    using input_type = int8_t;
    using output_type = int8_t;
     
    bpsk_phase_ambiguity_solver::sptr
    bpsk_phase_ambiguity_solver::make(const std::string& sync_word, int tolerance, const std::string& tag_lock_name)
    {
      return gnuradio::make_block_sptr<bpsk_phase_ambiguity_solver_impl>(
        sync_word, tolerance, tag_lock_name);
    }


    /*
     * The private constructor
     */
    bpsk_phase_ambiguity_solver_impl::bpsk_phase_ambiguity_solver_impl(const std::string& sync_word, int tolerance, const std::string& tag_lock_name)
      : gr::sync_block("bpsk_phase_ambiguity_solver",
              gr::io_signature::make(1, 1, sizeof(input_type)),
              gr::io_signature::make(1, 1, sizeof(output_type))),
              d_tolerance(tolerance),
              d_status_phase(BPSK_PHASE_UNK)
    {
    	
	    set_sync_word(sync_word);
        
	    d_map[0] = -1;
	    d_map[1] = 1;
        for (unsigned int i = 2; i < s_map_size; i++)
        	d_map[i] = 0;

    	message_port_register_in(pmt::mp("ctrl")); 
        set_msg_handler(pmt::mp("ctrl"),
                        	[this](pmt::pmt_t msg) { this->handle_msg_lock(msg); });    
        	
	    d_key = pmt::string_to_symbol(tag_lock_name);
    	message_port_register_out(pmt::mp("msg"));
 
    }

    /*
     * Our virtual destructor.
     */
    bpsk_phase_ambiguity_solver_impl::~bpsk_phase_ambiguity_solver_impl()
    {

    }

    void bpsk_phase_ambiguity_solver_impl::handle_msg_lock(pmt::pmt_t msg)
    {
        // make sure PDU data is formed properly
        if (!(pmt::is_pair(msg)) || pmt::is_dict(msg) || pmt::is_pdu(msg)) {
            GR_LOG_NOTICE(d_logger, "received unexpected PMT (expected a simple pair)");
            return;
        }
        pmt::pmt_t name = pmt::car(msg);
        pmt::pmt_t value = pmt::cdr(msg);


        if (pmt::is_symbol(name)) {
            std::string key = pmt::symbol_to_string(name);
            if (key == "lock") {
                if (pmt::is_bool(value)) {
                    d_lock = pmt::to_bool(value);
                } else if (pmt::is_integer(value)) {
                    d_lock = (bool)pmt::to_long(value);
                } else {
                    GR_LOG_NOTICE(d_logger, "received unexpected PMT (value is not valid, expected boolean or long");
                }
            } 
        } else {
            GR_LOG_NOTICE(d_logger, "received unexpected PMT (key is not a string, expected 'lock'");
        }
    }
    
    void 
    bpsk_phase_ambiguity_solver_impl::set_sync_word(const std::string& sync_word)
    {
    	std::vector<float> taps;
    	for (unsigned int i = 0; i < sync_word.length(); i++) {
        	std::string byteString = sync_word.substr(i, 1);
        	char bytes = (char)strtol(byteString.c_str(), NULL, 16);
        	for (int j = NIBBLE_SIZE-1; j >= 0; j--) {
            		taps.insert(taps.begin(),(float)((bytes >> j) & 0x01) * 2 - 1);
        	}
    	}

    	if (d_fir_filter == NULL)
        	d_fir_filter = new fir_filter_fff(taps);
    	else
    		d_fir_filter->set_taps(taps);
    		
    	d_num_filter_taps = taps.size();
    	set_history(d_num_filter_taps);

	    d_lock = false;
    }

    int
    bpsk_phase_ambiguity_solver_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      auto in = static_cast<const input_type*>(input_items[0]);
      auto out = static_cast<output_type*>(output_items[0]);
      
      int8_t * in_b  = new int8_t[noutput_items];      
      float * in_f  = new float[noutput_items];
      
	// Map the input to 0=>-1, 1=>+1, any other value to 0
      for (int i = 0; i < noutput_items; i++)
          in_b[i] = d_map[in[i]];

  	// Conversion to float required for the FIR filter    
      volk_8i_s32f_convert_32f_u(in_f, in_b, 1, noutput_items);
      
      for (int i = 0; i < noutput_items; i++)
      {

      	if(!d_lock){
      	    int result = (int) d_fir_filter->filter(in_f+i);

            if(i >= (int)history()){
                if (result >= (d_num_filter_taps - d_tolerance) && result <= d_num_filter_taps){
      		        d_status_phase = BPSK_PHASE_0;
      		        message_port_pub(pmt::mp("msg"), 
      		    	    pmt::cons(d_key, pmt::from_long(0)));
      	        }else if ((result * -1) >= (d_num_filter_taps - d_tolerance) && (result * -1) <= d_num_filter_taps){
      		        d_status_phase = BPSK_PHASE_180;
      		        message_port_pub(pmt::mp("msg"), 
      		    	    pmt::cons(d_key, pmt::from_long(180)));
      	        }
      	    }
      	}
      	switch(d_status_phase){
      		case BPSK_PHASE_0:
      		    out[i] = ~in[i+history()-1] & 0x01;
      		    break;
      		case BPSK_PHASE_180:
      		    out[i] = in[i+history()-1];
      		    break;
      		default:
      		    out[i] = 0;  
        }    		
      }
      // Tell runtime system how many output items we produced.
      delete[] in_b;
      delete[] in_f;
      return noutput_items;
    }

  } /* namespace comm_tools */
} /* namespace gr */
