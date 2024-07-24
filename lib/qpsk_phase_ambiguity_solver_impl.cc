/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "qpsk_phase_ambiguity_solver_impl.h"

#define NIBBLE_SIZE 4

namespace gr {
  namespace comm_tools {

    using input_type = int8_t;
    using output_type = int8_t;
    
    qpsk_phase_ambiguity_solver::sptr
    qpsk_phase_ambiguity_solver::make(const std::string& sync_word, int tolerance, const std::string& tag_lock_name)
    {
      return gnuradio::make_block_sptr<qpsk_phase_ambiguity_solver_impl>(
        sync_word, tolerance, tag_lock_name);
    }


    /*
     * The private constructor
     */
    qpsk_phase_ambiguity_solver_impl::qpsk_phase_ambiguity_solver_impl(const std::string& sync_word, int tolerance, const std::string& tag_lock_name)
      : gr::sync_interpolator("qpsk_phase_ambiguity_solver",
              gr::io_signature::make(2 /* min inputs */, 2 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)), 2),
              d_status_phase(0)
    {
    	
	set_sync_word(sync_word);
        
	d_map[0] = -1;
	d_map[1] = 1;
    	for (unsigned int i = 2; i < s_map_size; i++)
        	d_map[i] = 0;
        	
	d_key = pmt::string_to_symbol(tag_lock_name);
    	message_port_register_out(pmt::mp("msg"));
    }

    /*
     * Our virtual destructor.
     */
    qpsk_phase_ambiguity_solver_impl::~qpsk_phase_ambiguity_solver_impl()
    {
    }

    void 
    qpsk_phase_ambiguity_solver_impl::set_sync_word(const std::string& sync_word)
    {
        gr::thread::scoped_lock guard(d_mutex);
        
    	std::vector<float> i_taps;
    	std::vector<float> q_taps;    	
    	for (unsigned int i = 0; i < sync_word.length(); i++) {
        	std::string byteString = sync_word.substr(i, 1);
        	char bytes = (char)strtol(byteString.c_str(), NULL, 16);
        	for (int j = NIBBLE_SIZE-1; j >= 0;) {
            		i_taps.insert(i_taps.begin(),(float)((bytes >> j--) & 0x01) * 2 - 1);
            		q_taps.insert(q_taps.begin(),(float)((bytes >> j--) & 0x01) * 2 - 1);            		
        	}
        	
    	}
    	if (d_i_fir_filter == NULL)
        	d_i_fir_filter = new fir_filter_fff(i_taps);
    	else
    		d_i_fir_filter->set_taps(i_taps);
    	
    	if (d_q_fir_filter == NULL)
        	d_q_fir_filter = new fir_filter_fff(q_taps);
    	else
    		d_q_fir_filter->set_taps(q_taps);    		
    	
    	d_num_filter_taps = i_taps.size();
    	d_threshold = d_num_filter_taps - d_tolerance;
    	set_history(d_num_filter_taps);
    }

    int
    qpsk_phase_ambiguity_solver_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);
        
      auto in_i = static_cast<const input_type*>(input_items[0])+history()-1;
      auto in_q = static_cast<const input_type*>(input_items[1]);
      auto out = static_cast<output_type*>(output_items[0]);

      int ninput_items = noutput_items/2;
      	
      int8_t * in_i_b  = new int8_t[ninput_items];      
      int8_t * in_q_b  = new int8_t[ninput_items];      

      float * in_i_f  = new float[ninput_items]; 
      float * in_q_f  = new float[ninput_items];          
      
	// Map the input to 0=>-1, 1=>+1, any other value to 0
      for (int i = 0; i < ninput_items; i++){
      	in_i_b[i] = d_map[in_i[i]];
      	in_q_b[i] = d_map[in_q[i]];
       }
  	// Conversion to float required for the FIR filter    
      volk_8i_s32f_convert_32f_u(in_i_f, in_i_b, 1, ninput_items);
      volk_8i_s32f_convert_32f_u(in_q_f, in_q_b, 1, ninput_items);
            
      for(int i=0; i<ninput_items; i++)
      {
      	int i_i_result = (int) d_i_fir_filter->filter(in_i_f+i);
      	int i_q_result = (int) d_q_fir_filter->filter(in_i_f+i);
      	int q_i_result = (int) d_i_fir_filter->filter(in_q_f+i);
      	int q_q_result = (int) d_q_fir_filter->filter(in_q_f+i);
        if(i >= history()){
      	    if (i_i_result >= d_threshold && i_i_result <= d_num_filter_taps && 
      	        q_q_result >= d_threshold && q_q_result <= d_num_filter_taps){
      	    	d_status_phase = PHASE_0;
      	    	message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(0)));
          	}else if ((i_q_result * -1) >= d_threshold && (i_q_result * -1) <= d_num_filter_taps && 
          	        q_i_result >= d_threshold && q_i_result <= d_num_filter_taps){
          		d_status_phase = PHASE_90;
          		message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(90)));
          	}else if ((i_i_result * -1) >= d_threshold && (i_i_result * -1) <= d_num_filter_taps && 
          	        (q_q_result * -1)  >= d_threshold && (q_q_result * -1) <= d_num_filter_taps){
          		d_status_phase = PHASE_180;
          		message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(180)));
          	}else if (i_q_result >= d_threshold && i_q_result <= d_num_filter_taps && 
          	        (q_i_result * -1)  >= d_threshold && (q_i_result * -1) <= d_num_filter_taps){
          		d_status_phase = PHASE_270;
          		message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(270)));
          	}else if (i_q_result >= d_threshold && i_q_result <= d_num_filter_taps && 
          	        q_i_result >= d_threshold && q_i_result){
          		d_status_phase = PHASE_INV_0;
          		message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(-0)));
          	}else if (i_i_result >= d_threshold && i_i_result <= d_num_filter_taps && 
          	        (q_q_result * -1) >= d_threshold && (q_q_result * -1) <= d_num_filter_taps){
          		d_status_phase = PHASE_INV_90;
          		message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(-90)));
          	}else if ((i_q_result * -1) >= d_threshold && (i_q_result * -1) <= d_num_filter_taps && 
          	        (q_i_result * -1)  >= d_threshold && (q_i_result * -1) <= d_num_filter_taps){
          		d_status_phase = PHASE_INV_180;
          		message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(-180)));
          	}else if ((i_i_result * -1)>= d_threshold && (i_i_result * -1) <= d_num_filter_taps && 
          	        q_q_result >= d_threshold && q_q_result <= d_num_filter_taps){
          		d_status_phase = PHASE_INV_270;
          		message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(-270)));
          	}
       	}
      	switch(d_status_phase){
      		case PHASE_0:
      			out[i*2] = in_i[i+history()-1];
      			out[i*2+1] = in_q[i+history()-1];
      			break;
      		case PHASE_90:
      			out[i*2] = in_q[i+history()-1];
      			out[i*2+1] = ~in_i[i+history()-1] & 0x01;
      			break;
      		case PHASE_180:
      			out[i*2] = ~in_i[i+history()-1] & 0x01;
      			out[i*2+1] = ~in_q[i+history()-1] & 0x01;
      			break;
      		case PHASE_270:
      			out[i*2] = ~in_q[i+history()-1] & 0x01;
      			out[i*2+1] = in_i[i+history()-1];
      			break;
      		case PHASE_INV_0:
      			out[i*2] = in_q[i+history()-1];
      			out[i*2+1] = in_i[i+history()-1];
      			break;
      		case PHASE_INV_90:
      			out[i*2] = in_i[i+history()-1];
      			out[i*2+1] = ~in_q[i+history()-1] & 0x01;
      			break;
      		case PHASE_INV_180:
      			out[i*2] = ~in_q[i+history()-1] & 0x01;
      			out[i*2+1] = ~in_i[i+history()-1] & 0x01;
      			break;
      		case PHASE_INV_270:
      			out[i*2] = ~in_i[i+history()-1] & 0x01;
      			out[i*2+1] = in_q[i+history()-1];
      			break;
      		default:
      			out[i*2] = 0;
      			out[i*2+1] = 0;
      			break;     			
	}
      }
      // Tell runtime system how many output items we produced.
      delete[] in_i_b;
      delete[] in_q_b;
      delete[] in_i_f; 
      delete[] in_q_f;
      return noutput_items;
    }

  } /* namespace comm_tools */
} /* namespace gr */
