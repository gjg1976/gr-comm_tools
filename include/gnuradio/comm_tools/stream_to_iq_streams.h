/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_COMM_TOOLS_STREAM_TO_IQ_STREAMS_H
#define INCLUDED_COMM_TOOLS_STREAM_TO_IQ_STREAMS_H

#include <gnuradio/comm_tools/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace comm_tools {

    /*!
     * \brief <+description of block+>
     * \ingroup comm_tools
     *
     */
    class COMM_TOOLS_API stream_to_iq_streams : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<stream_to_iq_streams> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of comm_tools::stream_to_iq_streams.
       *
       * To avoid accidental use of raw pointers, comm_tools::stream_to_iq_streams's
       * constructor is in a private implementation
       * class. comm_tools::stream_to_iq_streams::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace comm_tools
} // namespace gr

#endif /* INCLUDED_COMM_TOOLS_STREAM_TO_IQ_STREAMS_H */
