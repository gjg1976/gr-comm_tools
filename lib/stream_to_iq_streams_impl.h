/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_COMM_TOOLS_STREAM_TO_IQ_STREAMS_IMPL_H
#define INCLUDED_COMM_TOOLS_STREAM_TO_IQ_STREAMS_IMPL_H

#include <gnuradio/comm_tools/stream_to_iq_streams.h>

namespace gr {
  namespace comm_tools {

    class stream_to_iq_streams_impl : public stream_to_iq_streams
    {
     private:
      // Nothing to declare in this block.

     public:
      stream_to_iq_streams_impl();
      ~stream_to_iq_streams_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace comm_tools
} // namespace gr

#endif /* INCLUDED_COMM_TOOLS_STREAM_TO_IQ_STREAMS_IMPL_H */
