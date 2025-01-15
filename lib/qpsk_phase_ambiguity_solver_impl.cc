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
              gr::io_signature::make(2, 2 , sizeof(input_type)),
              gr::io_signature::make(1, 1 , sizeof(output_type)), 2),
              d_tolerance(tolerance),
              d_status_phase(QPSK_PHASE_UNK)
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
    qpsk_phase_ambiguity_solver_impl::~qpsk_phase_ambiguity_solver_impl()
    {
    }

    void qpsk_phase_ambiguity_solver_impl::handle_msg_lock(pmt::pmt_t msg)
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
    qpsk_phase_ambiguity_solver_impl::set_sync_word(const std::string& sync_word)
    {
     	std::vector<float> i_taps;
    	std::vector<float> q_taps;    	
    	for (unsigned int i = 0; i < sync_word.length(); i++) {
        	std::string byteString = sync_word.substr(i, 1);
        	char bytes = (char)strtol(byteString.c_str(), NULL, 16);
        	for (int j = NIBBLE_SIZE-1; j >= 0;) {
        	        float i_tap = (float)((bytes >> j--) & 0x01) * 2 - 1;
        	        float q_tap = (float)((bytes >> j--) & 0x01) * 2 - 1;
            		i_taps.insert(i_taps.begin(), i_tap);
            		q_taps.insert(q_taps.begin(), q_tap);
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
    	d_threshold = 2*d_num_filter_taps - d_tolerance;
    	
    	set_history(d_num_filter_taps);

	    d_lock = false;
    }

    int
    qpsk_phase_ambiguity_solver_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
 
      auto in_i = static_cast<const input_type*>(input_items[0]);
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
      
        if(!d_lock){
      	    int i_i_result = (int) d_i_fir_filter->filter(in_i_f+i);
      	    int i_q_result = (int) d_q_fir_filter->filter(&in_i_f[i]);
      	    int q_i_result = (int) d_i_fir_filter->filter(&in_q_f[i]);
      	    int q_q_result = (int) (int) d_q_fir_filter->filter(in_q_f+i);
      	    
            if(i >= (int)history()){
      	        int ph_ii_qq_comparation = i_i_result + q_q_result;
      	        int ph_niq_qi_comparation = (-1 * i_q_result) + q_i_result;
      	        int ph_nii_nqq_comparation = (-1 * i_i_result) + (-1 * q_q_result);
      	        int ph_iq_nqi_comparation = i_q_result + (-1 * q_i_result);

      	        int ph_iq_qi_comparation = i_q_result + q_i_result;
      	        int ph_ii_nqq_comparation = i_i_result + (-1 * q_q_result);
      	        int ph_niq_nqi_comparation = (-1 * i_q_result) + (-1 * q_i_result);
      	        int ph_nii_qq_comparation = (-1 * i_i_result) + q_q_result;
      	              	        
      	        if(ph_ii_qq_comparation >= d_threshold && ph_ii_qq_comparation <= (2*d_num_filter_taps)){
      	    	    d_status_phase = QPSK_PHASE_0;
      	    	    message_port_pub(pmt::mp("msg"), 
          			    pmt::cons(d_key, pmt::from_long(0)));
          	    }else if (ph_niq_qi_comparation >= d_threshold && ph_niq_qi_comparation <= (2*d_num_filter_taps)){
          		    d_status_phase = QPSK_PHASE_90;
          		    message_port_pub(pmt::mp("msg"), 
          			pmt::cons(d_key, pmt::from_long(90)));
          	    }else if (ph_nii_nqq_comparation >= d_threshold && ph_nii_nqq_comparation <= (2*d_num_filter_taps)){
          		    d_status_phase = QPSK_PHASE_180;
          		    message_port_pub(pmt::mp("msg"), 
          			    pmt::cons(d_key, pmt::from_long(180)));
          	    }else if (ph_iq_nqi_comparation >= d_threshold && ph_iq_nqi_comparation <= (2*d_num_filter_taps)){
          		    d_status_phase = QPSK_PHASE_270;
          		    message_port_pub(pmt::mp("msg"), 
          			    pmt::cons(d_key, pmt::from_long(270)));
          	    }else if (ph_iq_qi_comparation >= d_threshold && ph_iq_qi_comparation <= (2*d_num_filter_taps)){
          		    d_status_phase = QPSK_PHASE_INV_0;
          		    message_port_pub(pmt::mp("msg"), 
          			    pmt::cons(d_key, pmt::from_long(-0)));
          	    }else if (ph_ii_nqq_comparation >= d_threshold && ph_ii_nqq_comparation <= (2*d_num_filter_taps)){
          		    d_status_phase = QPSK_PHASE_INV_90;
          		    message_port_pub(pmt::mp("msg"), 
          			    pmt::cons(d_key, pmt::from_long(-90)));
          	    }else if (ph_niq_nqi_comparation >= d_threshold && ph_niq_nqi_comparation <= (2*d_num_filter_taps)){
          		    d_status_phase = QPSK_PHASE_INV_180;
          		    message_port_pub(pmt::mp("msg"), 
          			    pmt::cons(d_key, pmt::from_long(-180)));
          	    }else if (ph_nii_qq_comparation >= d_threshold && ph_nii_qq_comparation <= (2*d_num_filter_taps)){
          		    d_status_phase = QPSK_PHASE_INV_270;
          		    message_port_pub(pmt::mp("msg"), 
          			    pmt::cons(d_key, pmt::from_long(-270)));
          	    }

           }
      	}
     	switch(d_status_phase){
      		case QPSK_PHASE_0:
      			out[i*2] = in_i[i+history()-1];
      			out[i*2+1] = in_q[i+history()-1];
      			break;
      		case QPSK_PHASE_90:
      			out[i*2] = in_q[i+history()-1];
      			out[i*2+1] = ~in_i[i+history()-1] & 0x01;
      			break;
      		case QPSK_PHASE_180:
      			out[i*2] = ~in_i[i+history()-1] & 0x01;
      			out[i*2+1] = ~in_q[i+history()-1] & 0x01;
      			break;
      		case QPSK_PHASE_270:
      			out[i*2] = ~in_q[i+history()-1] & 0x01;
      			out[i*2+1] = in_i[i+history()-1];
      			break;
      		case QPSK_PHASE_INV_0:
      			out[i*2] = in_q[i+history()-1];
      			out[i*2+1] = in_i[i+history()-1];
      			break;
      		case QPSK_PHASE_INV_90:
      			out[i*2] = in_i[i+history()-1];
      			out[i*2+1] = ~in_q[i+history()-1] & 0x01;
      			break;
      		case QPSK_PHASE_INV_180:
      			out[i*2] = ~in_q[i+history()-1] & 0x01;
      			out[i*2+1] = ~in_i[i+history()-1] & 0x01;
      			break;
      		case QPSK_PHASE_INV_270:
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
