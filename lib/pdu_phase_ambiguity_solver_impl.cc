/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "pdu_phase_ambiguity_solver_impl.h"

#define NIBBLE_SIZE 4

namespace gr {
  namespace comm_tools {

    pdu_phase_ambiguity_solver::sptr
    pdu_phase_ambiguity_solver::make(uint8_t modulation, const std::string& sync_word, int tolerance, const std::string& tag_lock_name)
    {
      return gnuradio::make_block_sptr<pdu_phase_ambiguity_solver_impl>(
        modulation, sync_word, tolerance, tag_lock_name);
    }


    /*
     * The private constructor
     */
    pdu_phase_ambiguity_solver_impl::pdu_phase_ambiguity_solver_impl(uint8_t modulation, const std::string& sync_word, int tolerance, const std::string& tag_lock_name)
      : gr::block("pdu_phase_ambiguity_solver",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
	d_modulation(modulation)              
    {

	set_sync_word(sync_word);

	d_map[0] = -1;
	d_map[1] = 1;
    	for (unsigned int i = 2; i < s_map_size; i++)
        	d_map[i] = 0;

    	message_port_register_out(pmt::mp("pdu_out"));
    	message_port_register_in(pmt::mp("pdu_in"));

    	switch (d_modulation) {
    		case MOD_BPSK: {
        		GR_LOG_DEBUG(d_logger, "pdu phase ambiguity solver for BPSK");
        		set_msg_handler(pmt::mp("pdu_in"),
                        	[this](pmt::pmt_t msg) { this->handle_msg_bpsk(msg); });
       		break;
    		}
    		case MOD_QPSK: {
        		GR_LOG_DEBUG(d_logger, "pdu phase ambiguity solver for QPSK");
        		set_msg_handler(pmt::mp("pdu_in"),
                        	[this](pmt::pmt_t msg) { this->handle_msg_qpsk(msg); });
        		break;
    		}
    		default: {
        		GR_LOG_DEBUG(d_logger, "pdu phase ambiguity solver in PASSTHROUGH mode");
        		set_msg_handler(pmt::mp("pdu_in"),
                        	[this](pmt::pmt_t msg) { this->handle_msg_passthrough(msg); });
    		}
    	}

                        	   
	    d_key = pmt::string_to_symbol(tag_lock_name);
    	message_port_register_out(pmt::mp("msg"));
    }

    /*
     * Our virtual destructor.
     */
    pdu_phase_ambiguity_solver_impl::~pdu_phase_ambiguity_solver_impl()
    {
    }
    
    void 
    pdu_phase_ambiguity_solver_impl::set_sync_word(const std::string& sync_word)
    {
        if(d_modulation == MOD_BPSK){        
    		std::vector<float> taps;
    		for (unsigned int i = 0; i < sync_word.length(); i++) {
        		std::string byteString = sync_word.substr(i, 1);
        		char bytes = (char)strtol(byteString.c_str(), NULL, 16);
        		for (int j = NIBBLE_SIZE-1; j >= 0; j--) {
            			taps.insert(taps.begin(),(float)((bytes >> j) & 0x01) * 2 - 1);
        		}
    		}
    		if (d_fir_1_filter == NULL)
        		d_fir_1_filter = new fir_filter_fff(taps);
    		else
    			d_fir_1_filter->set_taps(taps);
    		
    		d_num_filter_taps = taps.size();
    		d_threshold = d_num_filter_taps - d_tolerance;   	    		
    		set_history(0);
    	}else{
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
    		if (d_fir_1_filter == NULL)
        		d_fir_1_filter = new fir_filter_fff(i_taps);
    		else
    			d_fir_1_filter->set_taps(i_taps);
    	
    		if (d_fir_2_filter == NULL)
        		d_fir_2_filter = new fir_filter_fff(q_taps);
    		else
    			d_fir_2_filter->set_taps(q_taps);    		
    	
    		d_num_filter_taps = i_taps.size();
    		d_threshold = d_num_filter_taps - d_tolerance;   	
    		set_history(0);
    	}
    }
  
    void 
    pdu_phase_ambiguity_solver_impl::handle_msg_passthrough(pmt::pmt_t pdu)
    {
        message_port_pub(pmt::mp("pdu_out"), pdu);
    }
        
    void pdu_phase_ambiguity_solver_impl::handle_msg_qpsk(pmt::pmt_t pdu)
    {
        // make sure PDU data is formed properly
        if (!(pmt::is_pair(pdu))) {
            GR_LOG_NOTICE(d_logger, "received unexpected PMT (non-pair)");
            return;
        }

        pmt::pmt_t meta = pmt::car(pdu);
        pmt::pmt_t v_data = pmt::cdr(pdu);
	
        if (pmt::is_u8vector(v_data)) {
            std::vector<uint8_t> in_data = pmt::u8vector_elements(v_data);
            std::vector<int8_t> in_i_b(in_data.size());
            std::vector<int8_t> in_q_b(in_data.size());
            std::vector<float> in_i_f(in_data.size());
            std::vector<float> in_q_f(in_data.size());
            std::vector<uint8_t> out_data(in_data.size() * 2);
            d_status_phase = PHASE_INVALID;
	     
           for (size_t i = 0; i < in_data.size(); i++){
           	in_i_b[i] = d_map[(in_data[i] >> 1) & 0x01];
            	in_q_b[i] = d_map[in_data[i] & 0x01];
           }   
            		
            
           	 // Conversion to float required for the FIR filter    
           volk_8i_s32f_convert_32f_u(in_i_f.data(), in_i_b.data(), 1, in_data.size());
           volk_8i_s32f_convert_32f_u(in_q_f.data(), in_q_b.data(), 1, in_data.size());

           for(size_t i = 0; i < in_data.size(); i++)
           {
      		int i_i_result = (int) d_fir_1_filter->filter(in_i_f.data()+i);
      		int i_q_result = (int) d_fir_2_filter->filter(in_i_f.data()+i);
      		int q_i_result = (int) d_fir_1_filter->filter(in_q_f.data()+i);
      		int q_q_result = (int) d_fir_2_filter->filter(in_q_f.data()+i);

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
      		case PHASE_90:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{
	      			out_data[i*2] = in_data[i] & 0x01;
	      			out_data[i*2+1] = ~(in_data[i] >> 1) & 0x01;
			}
      			break;
      		case PHASE_180:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{   		
      				out_data[i*2] = ~(in_data[i] >> 1) & 0x01;
      				out_data[i*2+1] = ~in_data[i] & 0x01;
      			}
      			break;
      		case PHASE_270:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{
      				out_data[i*2] = ~in_data[i]  & 0x01;
      				out_data[i*2+1] = (in_data[i] >> 1) & 0x01;
			}
      			break;
      		case PHASE_INV_0:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{
      				out_data[i*2] = in_data[i] & 0x01;
      				out_data[i*2+1] = (in_data[i] >> 1) & 0x01;
      			}
      			break;
      		case PHASE_INV_90:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{
	      			out_data[i*2] = (in_data[i] >> 1) & 0x01;
	      			out_data[i*2+1] = ~in_data[i] & 0x01;
			}
			break;
      		case PHASE_INV_180:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{
      				out_data[i*2] = ~in_data[i] & 0x01;
      				out_data[i*2+1] = ~(in_data[i] >> 1) & 0x01;
			}
      			break;
      		case PHASE_INV_270:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{
      				out_data[i*2] = ~(in_data[i] >> 1) & 0x01;
      				out_data[i*2+1] = in_data[i] & 0x01;
      			}	
      			break;
      		case PHASE_0:
      		default:
      			for(size_t i = 0; i < in_data.size(); i++)
            		{
      				out_data[i*2] = (in_data[i] >> 1) & 0x01;
      				out_data[i*2+1] = in_data[i] & 0x01;
      			}
      			break;     			
           }

           message_port_pub(
                	pmt::mp("pdu_out"),
                	(pmt::cons(meta, pmt::init_u8vector(out_data.size(), out_data))));                
        } else {
            GR_LOG_WARN(d_logger,
                        "Failed to 'Phase Ambiguity Resolve' the data because it is not a u8vector");
            message_port_pub(pmt::mp("pdu_out"), pdu);
        }
    }

    void pdu_phase_ambiguity_solver_impl::handle_msg_bpsk(pmt::pmt_t pdu)
    {
        // make sure PDU data is formed properly
        if (!(pmt::is_pair(pdu))) {
            GR_LOG_NOTICE(d_logger, "received unexpected PMT (non-pair)");
            return;
        }

        pmt::pmt_t meta = pmt::car(pdu);
        pmt::pmt_t v_data = pmt::cdr(pdu);
	
        if (pmt::is_u8vector(v_data)) {
            std::vector<uint8_t> in_data = pmt::u8vector_elements(v_data);
            std::vector<int8_t> in_b(in_data.size());
            std::vector<float> in_f(in_data.size());
            std::vector<uint8_t> out_data(in_data.size());
	     d_status_phase = PHASE_INVALID;
     
            // Map the input to 0=>-1, 1=>+1, any other value to 0
            for (size_t i = 0; i < in_data.size(); i++)
            	in_b[i] = d_map[in_data[i]];
      	
            // Conversion to float required for the FIR filter    
            volk_8i_s32f_convert_32f_u(in_f.data(), in_b.data(), 1, in_data.size());

            for(size_t i=0; i<in_data.size(); i++)
            {
            	int result = (int) d_fir_1_filter->filter(&in_f[i]);

                if (result >= (d_num_filter_taps - d_tolerance) && result <= d_num_filter_taps){
      		       d_status_phase = false;
      		       message_port_pub(pmt::mp("msg"), 
      		        	pmt::cons(d_key, pmt::from_long(0)));
      	        }else if ((result * -1) >= (d_num_filter_taps - d_tolerance) && (result * -1) <= d_num_filter_taps){
      		        d_status_phase = true;
      		        message_port_pub(pmt::mp("msg"), 
      		    	    pmt::cons(d_key, pmt::from_long(180)));
      	        }
            }

            if(d_status_phase == PHASE_180){
            	for(size_t i=0; i<in_data.size(); i++)
            	{
            		out_data[i] = ~in_data[i] & 0x01;
            	}
            	message_port_pub(
                	pmt::mp("pdu_out"),
                	(pmt::cons(meta, pmt::init_u8vector(out_data.size(), out_data))));            	
            }else{
            	message_port_pub(
                	pmt::mp("pdu_out"),
                 	(pmt::cons(meta, pmt::init_u8vector(in_data.size(), in_data))));
            }            	
            
        } else {
            GR_LOG_WARN(d_logger,
                        "Failed to 'Phase Ambiguity Resolve' the data because it is not a u8vector");
            message_port_pub(pmt::mp("pdu_out"), pdu);
        }

    }
    
  } /* namespace comm_tools */
} /* namespace gr */
