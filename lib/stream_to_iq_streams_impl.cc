/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "stream_to_iq_streams_impl.h"

namespace gr {
  namespace comm_tools {

    using input_type = int8_t;
    using output_type = int8_t;
    
    stream_to_iq_streams::sptr
    stream_to_iq_streams::make()
    {
      return gnuradio::make_block_sptr<stream_to_iq_streams_impl>(
        );
    }


    /*
     * The private constructor
     */
    stream_to_iq_streams_impl::stream_to_iq_streams_impl()
      : gr::sync_block("stream_to_iq_streams",
              gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(2 /* min outputs */, 2 /*max outputs */, sizeof(output_type)))
    {}

    /*
     * Our virtual destructor.
     */
    stream_to_iq_streams_impl::~stream_to_iq_streams_impl()
    {
    }

    int
    stream_to_iq_streams_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      	auto in = static_cast<const input_type*>(input_items[0]);
      	auto out_i = static_cast<output_type*>(output_items[0]);
      	auto out_q = static_cast<output_type*>(output_items[1]);
      
    	for (int i = 0; i < noutput_items; i++) {
         	out_i[i] = (in[i] >> 1) & 0x01;
         	out_q[i] = in[i] & 0x01;
    	}

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace comm_tools */
} /* namespace gr */
