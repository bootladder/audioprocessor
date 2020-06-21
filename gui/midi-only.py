#!/usr/bin/python

import thread
import time
import serial
import subprocess
import select
import sys

midi_descriptors = []
midi_device_names = []

# Open all /dev/midi* devices, pipe them all into the serial port output
def thread_read_midi_and_pipe_to_ttyUSB():
    global midi_device_names
    global midi_descriptors

    with serial.Serial('/dev/ttyUSB1', 9600, timeout=2) as ser:
        while(True):

            try:
                print "selecting"
                readable, writeable, fail = select.select(midi_descriptors, [], [], 1)
                print 'readables  ' + str(readable)
                print 'fails  ' + str(fail)

                for descriptor in readable:
                    print ' READING'
                    s = descriptor.read(3)
                    print 'got it %d %d %d' % (ord(s[0]),ord(s[1]),ord(s[2]))
                    ser.write(s)

            except:
                print 'READING THREAD FAILED.  CLOSING FILES'
                reset_files()


def thread_check_for_connected_midi_devices():
    global midi_device_names
    global midi_descriptors

    while(True):
        try: 
            #print 'checking'
            print 'names ' + str(midi_device_names)
            print 'descriptors ' + str(midi_descriptors)

            try:
                output = subprocess.check_output("ls /dev/snd/midi*", shell=True).strip().split('\n')
            except:
                print 'no midi devices'
                time.sleep(1)
                continue

            print 'output ' + str(output)
            if len(output) < len(midi_device_names):
                print "Lost a device, clearing the list"
                midi_device_names = []
                midi_descriptors = []

            for device_name in output:
                if device_name in midi_device_names:
                    continue

                print "Found MIDI Device : " + device_name

                d = open(device_name, 'r')

                midi_device_names.append(device_name)
                print 'opened ok'
                midi_descriptors.append(d)

        except:
            print 'READING THREAD FAILED.  CLOSING FILES'
            reset_files()

        time.sleep(1)
        sys.stdout.flush()

def reset_files():
    global midi_device_names
    global midi_descriptors

    for descriptor in midi_descriptors:
        descriptor.close()
    midi_descriptors = []
    midi_device_names = []

###################### MAIN #########################


# MIDI Controller Forwarding
thread.start_new_thread(thread_check_for_connected_midi_devices, ())
thread.start_new_thread(thread_read_midi_and_pipe_to_ttyUSB, ())

while True:
    print 'looping'
    sys.stdout.flush()
    time.sleep(1)
    continue
