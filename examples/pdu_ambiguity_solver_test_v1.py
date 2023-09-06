#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: PDU Ambiguity Solver Test
# Author: Gustavo Gonzalez
# GNU Radio version: 3.10.3.0

from packaging.version import Version as StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from gnuradio import blocks
import pmt
from gnuradio import comm_tools
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import satellite_tools



from gnuradio import qtgui

class pdu_ambiguity_solver_test_v1(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "PDU Ambiguity Solver Test ", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("PDU Ambiguity Solver Test ")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
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

        self.settings = Qt.QSettings("GNU Radio", "pdu_ambiguity_solver_test_v1")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.satellite_tools_pdu_vector_source_0_0_1 = satellite_tools.pdu_vector_source((1,1,1,1,1,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,1,0,1,1,1,1,1))
        self.satellite_tools_pdu_vector_source_0_0_0_1_0 = satellite_tools.pdu_vector_source((3, 3, 1, 2, 2, 0, 3, 0, 0, 0, 0, 0, 3, 3, 1, 0, 1, 0, 0, 1))
        self.satellite_tools_pdu_vector_source_0_0 = satellite_tools.pdu_vector_source((1,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0))
        self.comm_tools_pdu_phase_ambiguity_solver_0_0 = comm_tools.pdu_phase_ambiguity_solver(1, '1ACFFC1D', 0, 'LOCK')
        self.blocks_message_strobe_0 = blocks.message_strobe(pmt.intern("trg"), 3000)
        self.blocks_message_debug_0_0_0 = blocks.message_debug(True)
        self.blocks_message_debug_0_0 = blocks.message_debug(True)


        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_message_strobe_0, 'strobe'), (self.satellite_tools_pdu_vector_source_0_0, 'trg'))
        self.msg_connect((self.blocks_message_strobe_0, 'strobe'), (self.satellite_tools_pdu_vector_source_0_0_0_1_0, 'trg'))
        self.msg_connect((self.blocks_message_strobe_0, 'strobe'), (self.satellite_tools_pdu_vector_source_0_0_1, 'trg'))
        self.msg_connect((self.comm_tools_pdu_phase_ambiguity_solver_0_0, 'msg'), (self.blocks_message_debug_0_0_0, 'print'))
        self.msg_connect((self.comm_tools_pdu_phase_ambiguity_solver_0_0, 'pdu_out'), (self.blocks_message_debug_0_0_0, 'print'))
        self.msg_connect((self.satellite_tools_pdu_vector_source_0_0_0_1_0, 'pdu_out'), (self.comm_tools_pdu_phase_ambiguity_solver_0_0, 'pdu_in'))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "pdu_ambiguity_solver_test_v1")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate




def main(top_block_cls=pdu_ambiguity_solver_test_v1, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
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
