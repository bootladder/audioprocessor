import thread
import time
import serial
import subprocess
import queue
import select



# Open all /dev/midi* devices, pipe them all into the serial port output
def thread_read_midi_and_pipe_to_ttyUSB():
    output = subprocess.check_output("ls /dev/midi*", shell=True)
    midi_devices = output.strip().split('\n')
    midi_descriptors = []
    for device in midi_devices:
        print "Found MIDI Device : " + device
        d = open(device, 'r')
        midi_descriptors.append(d)

    with serial.Serial('/dev/ttyUSB1', 9600, timeout=2) as ser:
        while(True):
            readable, _, _ = select.select(midi_descriptors, [], [])
            for descriptor in readable:
                s = descriptor.read(3)
                print 'got it'
                ser.write(s)


###################### MAIN #########################


# MIDI Controller Forwarding
thread.start_new_thread(thread_read_midi_and_pipe_to_ttyUSB, ())

while True:
    time.sleep(1000)
    continue
    # wait for thread
