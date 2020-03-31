#!/usr/bin/python
import os
import sys

import thread
import time
import serial
import subprocess
import json
import select




def thread_seriallogger_reader(dispatch, arg2, arg3):

    with serial.Serial('/dev/ttyUSB0', 921600, timeout=2) as ser:
        while(True):
            line = ser.readline()   # read a '\n' terminated line
            if len(line) == 0:
                continue

            if line[0] in dispatch.keys():
                dispatch[line[0]](line[1:]) # call it


###################### MAIN #########################

# Serial Port Dispatch
messageDispatch = {
    'I':app.updateIdleTickCount,
    'L':app.updateEventLog,
    'M':app.updateMidiProcessedLog,
    'B':app.receivedNodeUpdate,
    'E':app.receivedEdgeListUpdate,
}

thread.start_new_thread(thread_seriallogger_reader, (messageDispatch, 'hurr', 'durr'))

