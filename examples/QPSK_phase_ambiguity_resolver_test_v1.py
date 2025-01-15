#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: QPSK Phase Ambiguity Resolver Test v1
# Author: Gustavo Gonzalez
# Description: This flow graph will test QPSK Phase Ambiguity Solver block
# GNU Radio version: 3.10.9.2

from PyQt5 import Qt
from gnuradio import qtgui
from PyQt5.QtCore import QObject, pyqtSlot
from gnuradio import analog
from gnuradio import blocks
import pmt
from gnuradio import blocks, gr
from gnuradio import comm_tools
from gnuradio import digital
from gnuradio import filter
from gnuradio import fec
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import gr, pdu
from gnuradio import pdu_utils
import numpy as np



class QPSK_phase_ambiguity_resolver_test_v1(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "QPSK Phase Ambiguity Resolver Test v1", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("QPSK Phase Ambiguity Resolver Test v1")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except BaseException as exc:
            print(f"Qt GUI: Could not set Icon: {str(exc)}", file=sys.stderr)
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "QPSK_phase_ambiguity_resolver_test_v1")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)

        ##################################################
        # Variables
        ##################################################
        self.syncword_hex = syncword_hex = '1ACFFC1D'
        self.syncword = syncword = 2*np.unpackbits(np.frombuffer(bytes.fromhex(syncword_hex), dtype = 'uint8')).astype('int')-1
        self.sps = sps = 20
        self.nfilts = nfilts = 32
        self.variable_qtgui_chooser_1 = variable_qtgui_chooser_1 = 0
        self.variable_qtgui_chooser_0 = variable_qtgui_chooser_0 = 0
        self.variable_constellation_rect_qpsk = variable_constellation_rect_qpsk = digital.constellation_rect([1+1j, 1-1j,-1+1j, -1-1j], [0, 1, 2, 3],
        4, 2, 2, 1, 1).base()
        self.variable_cc_encoder_ccsds_c2noinv_def = variable_cc_encoder_ccsds_c2noinv_def = fec.cc_encoder_make(2048,7, 2, [79, 109], 0, fec.CC_STREAMING, False)
        self.variable_cc_decoder_ccsds_c2noinv_def = variable_cc_decoder_ccsds_c2noinv_def = fec.cc_decoder.make(2048,7, 2, [79,109], 0, (-1), fec.CC_STREAMING, False)
        self.samp_rate = samp_rate = 64000*sps
        self.rrc_taps = rrc_taps = firdes.root_raised_cosine(nfilts, nfilts, 1.0/float(sps), 0.35, 45*nfilts)
        self.access_code = access_code = ''.join([str(i) for i in (syncword+1)//2])[-64:]

        ##################################################
        # Blocks
        ##################################################

        # Create the options list
        self._variable_qtgui_chooser_1_options = [0, 1]
        # Create the labels list
        self._variable_qtgui_chooser_1_labels = ['Normal', 'Reverse']
        # Create the combo box
        self._variable_qtgui_chooser_1_tool_bar = Qt.QToolBar(self)
        self._variable_qtgui_chooser_1_tool_bar.addWidget(Qt.QLabel("Normal/Reverse" + ": "))
        self._variable_qtgui_chooser_1_combo_box = Qt.QComboBox()
        self._variable_qtgui_chooser_1_tool_bar.addWidget(self._variable_qtgui_chooser_1_combo_box)
        for _label in self._variable_qtgui_chooser_1_labels: self._variable_qtgui_chooser_1_combo_box.addItem(_label)
        self._variable_qtgui_chooser_1_callback = lambda i: Qt.QMetaObject.invokeMethod(self._variable_qtgui_chooser_1_combo_box, "setCurrentIndex", Qt.Q_ARG("int", self._variable_qtgui_chooser_1_options.index(i)))
        self._variable_qtgui_chooser_1_callback(self.variable_qtgui_chooser_1)
        self._variable_qtgui_chooser_1_combo_box.currentIndexChanged.connect(
            lambda i: self.set_variable_qtgui_chooser_1(self._variable_qtgui_chooser_1_options[i]))
        # Create the radio buttons
        self.top_grid_layout.addWidget(self._variable_qtgui_chooser_1_tool_bar, 1, 1, 1, 2)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        # Create the options list
        self._variable_qtgui_chooser_0_options = [0, 1, 2, 3]
        # Create the labels list
        self._variable_qtgui_chooser_0_labels = ['0॰', '90॰', '180॰', '270॰']
        # Create the combo box
        self._variable_qtgui_chooser_0_tool_bar = Qt.QToolBar(self)
        self._variable_qtgui_chooser_0_tool_bar.addWidget(Qt.QLabel("Rotation" + ": "))
        self._variable_qtgui_chooser_0_combo_box = Qt.QComboBox()
        self._variable_qtgui_chooser_0_tool_bar.addWidget(self._variable_qtgui_chooser_0_combo_box)
        for _label in self._variable_qtgui_chooser_0_labels: self._variable_qtgui_chooser_0_combo_box.addItem(_label)
        self._variable_qtgui_chooser_0_callback = lambda i: Qt.QMetaObject.invokeMethod(self._variable_qtgui_chooser_0_combo_box, "setCurrentIndex", Qt.Q_ARG("int", self._variable_qtgui_chooser_0_options.index(i)))
        self._variable_qtgui_chooser_0_callback(self.variable_qtgui_chooser_0)
        self._variable_qtgui_chooser_0_combo_box.currentIndexChanged.connect(
            lambda i: self.set_variable_qtgui_chooser_0(self._variable_qtgui_chooser_0_options[i]))
        # Create the radio buttons
        self.top_grid_layout.addWidget(self._variable_qtgui_chooser_0_tool_bar, 1, 3, 1, 2)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(3, 5):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.variable_qtgui_msg_push_button_0_1_0_0_1_0_0 = _variable_qtgui_msg_push_button_0_1_0_0_1_0_0_toggle_button = qtgui.MsgPushButton('Unlock', 'lock',0,"default","default")
        self.variable_qtgui_msg_push_button_0_1_0_0_1_0_0 = _variable_qtgui_msg_push_button_0_1_0_0_1_0_0_toggle_button

        self.top_layout.addWidget(_variable_qtgui_msg_push_button_0_1_0_0_1_0_0_toggle_button)
        self.variable_qtgui_msg_push_button_0_1_0_0_1_0 = _variable_qtgui_msg_push_button_0_1_0_0_1_0_toggle_button = qtgui.MsgPushButton('Lock', 'lock',1,"default","default")
        self.variable_qtgui_msg_push_button_0_1_0_0_1_0 = _variable_qtgui_msg_push_button_0_1_0_0_1_0_toggle_button

        self.top_layout.addWidget(_variable_qtgui_msg_push_button_0_1_0_0_1_0_toggle_button)
        self.pdu_utils_tags_to_pdu_X_0 = pdu_utils.tags_to_pdu_b(pmt.intern('syncword'), pmt.intern('EOB'), (1024*8), samp_rate, [], False, 0, 0.0)
        self.pdu_utils_tags_to_pdu_X_0.set_eob_parameters(1, 0)
        self.pdu_utils_tags_to_pdu_X_0.enable_time_debug(False)
        self.pdu_pdu_to_tagged_stream_0 = pdu.pdu_to_tagged_stream(gr.types.byte_t, 'packet_len')
        self.fec_generic_encoder_0_0 = fec.encoder(variable_cc_encoder_ccsds_c2noinv_def, gr.sizeof_char, gr.sizeof_char)
        self.fec_extended_decoder_0_0 = fec.extended_decoder(decoder_obj_list=variable_cc_decoder_ccsds_c2noinv_def, threading= None, ann=None, puncpat='11', integration_period=10000)
        self.digital_symbol_sync_xx_0 = digital.symbol_sync_cc(
            digital.TED_SIGNAL_TIMES_SLOPE_ML,
            sps,
            0.0628,
            1.0,
            1.0,
            1.5,
            1,
            digital.constellation_qpsk().base(),
            digital.IR_PFB_MF,
            nfilts,
            rrc_taps)
        self.digital_map_bb_0_0 = digital.map_bb([-1,1])
        self.digital_correlate_access_code_tag_xx_0 = digital.correlate_access_code_tag_bb(access_code, 0, 'syncword')
        self.digital_constellation_modulator_0 = digital.generic_mod(
            constellation=variable_constellation_rect_qpsk,
            differential=False,
            samples_per_symbol=sps,
            pre_diff_code=True,
            excess_bw=0.35,
            verbose=False,
            log=False,
            truncate=False)
        self.digital_constellation_decoder_cb_0_1 = digital.constellation_decoder_cb(variable_constellation_rect_qpsk)
        self.comm_tools_stream_to_iq_streams_0 = comm_tools.stream_to_iq_streams()
        self.comm_tools_qpsk_phase_ambiguity_solver_0 = comm_tools.qpsk_phase_ambiguity_solver('D49C24FF2686', 0, 'LOCK')
        self.blocks_unpack_k_bits_bb_0_0 = blocks.unpack_k_bits_bb(8)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_char*1, samp_rate,True)
        self.blocks_selector_0_0_0 = blocks.selector(gr.sizeof_gr_complex*1,variable_qtgui_chooser_1,0)
        self.blocks_selector_0_0_0.set_enabled(True)
        self.blocks_selector_0_0 = blocks.selector(gr.sizeof_gr_complex*1,variable_qtgui_chooser_0,0)
        self.blocks_selector_0_0.set_enabled(True)
        self.blocks_selector_0 = blocks.selector(gr.sizeof_gr_complex*1,variable_qtgui_chooser_0,0)
        self.blocks_selector_0.set_enabled(True)
        self.blocks_pack_k_bits_bb_0_1_0 = blocks.pack_k_bits_bb(8)
        self.blocks_pack_k_bits_bb_0_0_0_0_0_0 = blocks.pack_k_bits_bb(8)
        self.blocks_multiply_xx_0_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_message_debug_0 = blocks.message_debug(True, gr.log_levels.info)
        self.blocks_float_to_complex_0_0_0_0_0_0_0_0 = blocks.float_to_complex(1)
        self.blocks_float_to_complex_0_0_0_0_0_0_0 = blocks.float_to_complex(1)
        self.blocks_float_to_complex_0_0_0_0_0_0 = blocks.float_to_complex(1)
        self.blocks_float_to_complex_0_0_0_0_0 = blocks.float_to_complex(1)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_char*1, './random_AOS_frames.bin', True, 0, 0)
        self.blocks_file_source_0.set_begin_tag(pmt.PMT_NIL)
        self.blocks_file_sink_0_1 = blocks.file_sink(gr.sizeof_char*1, './qpsk_IQ_out.bin', False)
        self.blocks_file_sink_0_1.set_unbuffered(False)
        self.blocks_complex_to_float_0_0_0_0_0_0_0_0 = blocks.complex_to_float(1)
        self.blocks_complex_to_float_0_0_0_0_0_0_0 = blocks.complex_to_float(1)
        self.blocks_complex_to_float_0_0_0_0_0_0 = blocks.complex_to_float(1)
        self.blocks_complex_to_float_0_0_0_0_0 = blocks.complex_to_float(1)
        self.blocks_char_to_float_0_0 = blocks.char_to_float(1, 1)
        self.analog_const_source_x_0_0_0 = analog.sig_source_c(0, analog.GR_CONST_WAVE, 0, 0, -1j)
        self.analog_const_source_x_0_0 = analog.sig_source_c(0, analog.GR_CONST_WAVE, 0, 0, -1)
        self.analog_const_source_x_0 = analog.sig_source_c(0, analog.GR_CONST_WAVE, 0, 0, 1j)


        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.comm_tools_qpsk_phase_ambiguity_solver_0, 'msg'), (self.blocks_message_debug_0, 'print'))
        self.msg_connect((self.pdu_utils_tags_to_pdu_X_0, 'pdu_out'), (self.pdu_pdu_to_tagged_stream_0, 'pdus'))
        self.msg_connect((self.variable_qtgui_msg_push_button_0_1_0_0_1_0, 'pressed'), (self.comm_tools_qpsk_phase_ambiguity_solver_0, 'ctrl'))
        self.msg_connect((self.variable_qtgui_msg_push_button_0_1_0_0_1_0_0, 'pressed'), (self.comm_tools_qpsk_phase_ambiguity_solver_0, 'ctrl'))
        self.connect((self.analog_const_source_x_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_const_source_x_0_0, 0), (self.blocks_multiply_xx_0_0, 1))
        self.connect((self.analog_const_source_x_0_0_0, 0), (self.blocks_multiply_xx_0_0_0, 1))
        self.connect((self.blocks_char_to_float_0_0, 0), (self.fec_extended_decoder_0_0, 0))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0, 1), (self.blocks_float_to_complex_0_0_0_0_0, 0))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0, 0), (self.blocks_float_to_complex_0_0_0_0_0, 1))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0_0, 0), (self.blocks_float_to_complex_0_0_0_0_0_0, 1))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0_0, 1), (self.blocks_float_to_complex_0_0_0_0_0_0, 0))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0_0_0, 1), (self.blocks_float_to_complex_0_0_0_0_0_0_0, 0))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0_0_0, 0), (self.blocks_float_to_complex_0_0_0_0_0_0_0, 1))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0_0_0_0, 1), (self.blocks_float_to_complex_0_0_0_0_0_0_0_0, 0))
        self.connect((self.blocks_complex_to_float_0_0_0_0_0_0_0_0, 0), (self.blocks_float_to_complex_0_0_0_0_0_0_0_0, 1))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_float_to_complex_0_0_0_0_0, 0), (self.blocks_selector_0_0, 0))
        self.connect((self.blocks_float_to_complex_0_0_0_0_0_0, 0), (self.blocks_selector_0_0, 1))
        self.connect((self.blocks_float_to_complex_0_0_0_0_0_0_0, 0), (self.blocks_selector_0_0, 2))
        self.connect((self.blocks_float_to_complex_0_0_0_0_0_0_0_0, 0), (self.blocks_selector_0_0, 3))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_complex_to_float_0_0_0_0_0_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_selector_0, 1))
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.blocks_complex_to_float_0_0_0_0_0_0_0, 0))
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.blocks_selector_0, 2))
        self.connect((self.blocks_multiply_xx_0_0_0, 0), (self.blocks_complex_to_float_0_0_0_0_0_0_0_0, 0))
        self.connect((self.blocks_multiply_xx_0_0_0, 0), (self.blocks_selector_0, 3))
        self.connect((self.blocks_pack_k_bits_bb_0_0_0_0_0_0, 0), (self.digital_constellation_modulator_0, 0))
        self.connect((self.blocks_pack_k_bits_bb_0_1_0, 0), (self.blocks_file_sink_0_1, 0))
        self.connect((self.blocks_selector_0, 0), (self.blocks_selector_0_0_0, 0))
        self.connect((self.blocks_selector_0_0, 0), (self.blocks_selector_0_0_0, 1))
        self.connect((self.blocks_selector_0_0_0, 0), (self.digital_symbol_sync_xx_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.blocks_unpack_k_bits_bb_0_0, 0))
        self.connect((self.blocks_unpack_k_bits_bb_0_0, 0), (self.fec_generic_encoder_0_0, 0))
        self.connect((self.comm_tools_qpsk_phase_ambiguity_solver_0, 0), (self.digital_map_bb_0_0, 0))
        self.connect((self.comm_tools_stream_to_iq_streams_0, 1), (self.comm_tools_qpsk_phase_ambiguity_solver_0, 1))
        self.connect((self.comm_tools_stream_to_iq_streams_0, 0), (self.comm_tools_qpsk_phase_ambiguity_solver_0, 0))
        self.connect((self.digital_constellation_decoder_cb_0_1, 0), (self.comm_tools_stream_to_iq_streams_0, 0))
        self.connect((self.digital_constellation_modulator_0, 0), (self.blocks_complex_to_float_0_0_0_0_0, 0))
        self.connect((self.digital_constellation_modulator_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.digital_constellation_modulator_0, 0), (self.blocks_multiply_xx_0_0, 0))
        self.connect((self.digital_constellation_modulator_0, 0), (self.blocks_multiply_xx_0_0_0, 0))
        self.connect((self.digital_constellation_modulator_0, 0), (self.blocks_selector_0, 0))
        self.connect((self.digital_correlate_access_code_tag_xx_0, 0), (self.pdu_utils_tags_to_pdu_X_0, 0))
        self.connect((self.digital_map_bb_0_0, 0), (self.blocks_char_to_float_0_0, 0))
        self.connect((self.digital_symbol_sync_xx_0, 0), (self.digital_constellation_decoder_cb_0_1, 0))
        self.connect((self.fec_extended_decoder_0_0, 0), (self.digital_correlate_access_code_tag_xx_0, 0))
        self.connect((self.fec_generic_encoder_0_0, 0), (self.blocks_pack_k_bits_bb_0_0_0_0_0_0, 0))
        self.connect((self.pdu_pdu_to_tagged_stream_0, 0), (self.blocks_pack_k_bits_bb_0_1_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "QPSK_phase_ambiguity_resolver_test_v1")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_syncword_hex(self):
        return self.syncword_hex

    def set_syncword_hex(self, syncword_hex):
        self.syncword_hex = syncword_hex
        self.set_syncword(2*np.unpackbits(np.frombuffer(bytes.fromhex(self.syncword_hex), dtype = 'uint8')).astype('int')-1)

    def get_syncword(self):
        return self.syncword

    def set_syncword(self, syncword):
        self.syncword = syncword
        self.set_access_code(''.join([str(i) for i in (self.syncword+1)//2])[-64:])

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.set_rrc_taps(firdes.root_raised_cosine(self.nfilts, self.nfilts, 1.0/float(self.sps), 0.35, 45*self.nfilts))
        self.set_samp_rate(64000*self.sps)
        self.digital_symbol_sync_xx_0.set_sps(self.sps)

    def get_nfilts(self):
        return self.nfilts

    def set_nfilts(self, nfilts):
        self.nfilts = nfilts
        self.set_rrc_taps(firdes.root_raised_cosine(self.nfilts, self.nfilts, 1.0/float(self.sps), 0.35, 45*self.nfilts))

    def get_variable_qtgui_chooser_1(self):
        return self.variable_qtgui_chooser_1

    def set_variable_qtgui_chooser_1(self, variable_qtgui_chooser_1):
        self.variable_qtgui_chooser_1 = variable_qtgui_chooser_1
        self._variable_qtgui_chooser_1_callback(self.variable_qtgui_chooser_1)
        self.blocks_selector_0_0_0.set_input_index(self.variable_qtgui_chooser_1)

    def get_variable_qtgui_chooser_0(self):
        return self.variable_qtgui_chooser_0

    def set_variable_qtgui_chooser_0(self, variable_qtgui_chooser_0):
        self.variable_qtgui_chooser_0 = variable_qtgui_chooser_0
        self._variable_qtgui_chooser_0_callback(self.variable_qtgui_chooser_0)
        self.blocks_selector_0.set_input_index(self.variable_qtgui_chooser_0)
        self.blocks_selector_0_0.set_input_index(self.variable_qtgui_chooser_0)

    def get_variable_constellation_rect_qpsk(self):
        return self.variable_constellation_rect_qpsk

    def set_variable_constellation_rect_qpsk(self, variable_constellation_rect_qpsk):
        self.variable_constellation_rect_qpsk = variable_constellation_rect_qpsk
        self.digital_constellation_decoder_cb_0_1.set_constellation(self.variable_constellation_rect_qpsk)

    def get_variable_cc_encoder_ccsds_c2noinv_def(self):
        return self.variable_cc_encoder_ccsds_c2noinv_def

    def set_variable_cc_encoder_ccsds_c2noinv_def(self, variable_cc_encoder_ccsds_c2noinv_def):
        self.variable_cc_encoder_ccsds_c2noinv_def = variable_cc_encoder_ccsds_c2noinv_def

    def get_variable_cc_decoder_ccsds_c2noinv_def(self):
        return self.variable_cc_decoder_ccsds_c2noinv_def

    def set_variable_cc_decoder_ccsds_c2noinv_def(self, variable_cc_decoder_ccsds_c2noinv_def):
        self.variable_cc_decoder_ccsds_c2noinv_def = variable_cc_decoder_ccsds_c2noinv_def

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.pdu_utils_tags_to_pdu_X_0.set_rate(self.samp_rate)

    def get_rrc_taps(self):
        return self.rrc_taps

    def set_rrc_taps(self, rrc_taps):
        self.rrc_taps = rrc_taps

    def get_access_code(self):
        return self.access_code

    def set_access_code(self, access_code):
        self.access_code = access_code
        self.digital_correlate_access_code_tag_xx_0.set_access_code(self.access_code)




def main(top_block_cls=QPSK_phase_ambiguity_resolver_test_v1, options=None):

    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
