/* -*- c++ -*- */
/*
 * Copyright 2023 Gustavo Gonzalez.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_COMM_TOOLS_QPSK_PHASE_AMBIGUITY_SOLVER_H
#define INCLUDED_COMM_TOOLS_QPSK_PHASE_AMBIGUITY_SOLVER_H

#include <gnuradio/comm_tools/api.h>
#include <gnuradio/sync_interpolator.h>

namespace gr {
  namespace comm_tools {

    /*!
     * \brief <+description of block+>
     * \ingroup comm_tools
     *
     */
    class COMM_TOOLS_API qpsk_phase_ambiguity_solver : virtual public gr::sync_interpolator
    {
     public:
      typedef std::shared_ptr<qpsk_phase_ambiguity_solver> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of comm_tools::qpsk_phase_ambiguity_solver.
       *
       * To avoid accidental use of raw pointers, comm_tools::qpsk_phase_ambiguity_solver's
       * constructor is in a private implementation
       * class. comm_tools::qpsk_phase_ambiguity_solver::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string& sync_word, int tolerance, const std::string& tag_lock_name);
    
      virtual void set_sync_word(const std::string& sync_word) = 0;
      virtual void set_tolerance(int tolerance) = 0;    
    };

  } // namespace comm_tools
} // namespace gr

#endif /* INCLUDED_COMM_TOOLS_QPSK_PHASE_AMBIGUITY_SOLVER_H */
