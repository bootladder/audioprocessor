from tkinter import *
import tkinter.scrolledtext
import thread
import time
import serial
import networkx as nx
import networkx.drawing.nx_pydot as nx_pydot
import subprocess

class App:
    def __init__(self, master):
        self.frame = Frame(master, height=300, width=300)

        self.quitButton = Button(self.frame, text="QUIT", fg="red", command=self.frame.quit)
        self.helloButton = Button(self.frame, text="Hello", command=self.say_hi)

        self.idleTickCount = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=25)
        self.midiControllerInput = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=45)
        self.midiProcessedLog = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=45)
        self.eventLog = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=50)
        self.blockGraphUpdateLog = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=50)

        self.graphImage = PhotoImage(file="graph.png")
        self.graphLabel = Label(self.frame, image=self.graphImage)



        self.frame.grid(row=0, column=0)
        self.quitButton.grid(row=0, column=0)
        self.helloButton.grid(row=0, column=2)

        self.idleTickCount.grid(row=5, column=0,sticky=W)
        self.midiControllerInput.grid(row=5, column=1,sticky=W)
        self.midiProcessedLog.grid(row=5, column=2,sticky=W)
        self.eventLog.grid(row=5, column=3,sticky=W)
        self.blockGraphUpdateLog.grid(row=6, column=0, sticky=W)
        self.graphLabel.grid(row=7, column=0, columnspan=5)

        self.static_i = 0

    def say_hi(self):
        print "hi there!"

    def updateIdleTickCount(self, theText):
        self.idleTickCount.insert(END, theText)
        self.idleTickCount.see("end")

    def updateMidiControllerInput(self, theText):
        self.midiControllerInput.insert(END, theText)
        self.midiControllerInput.see("end")

    def updateMidiProcessedLog(self, theText):
        self.midiProcessedLog.insert(END, theText)
        self.midiProcessedLog.see("end")

    def updateEventLog(self, theText):
        self.eventLog.insert(END, theText)
        self.eventLog.see("end")

    def updateBlockGraphUpdateLog(self, theText):
        # modify the graph data structure
        # export the graph to a .graph dot file
        # convert the dot file to a png
        # load the png image
        blockGraph.nodes['yay']['label'] = '%d' % self.static_i
        self.static_i = self.static_i + 1
        write_graph_to_file(blockGraph)

        self.graphImage = PhotoImage(file="/tmp/output.png")
        self.graphLabel.configure(image=self.graphImage)

        self.blockGraphUpdateLog.insert(END, theText)
        self.blockGraphUpdateLog.see("end")


def seriallogger_reader(dispatch, arg2, arg3):

    with serial.Serial('/dev/ttyUSB0', 921600, timeout=2) as ser:
        while(True):
            line = ser.readline()   # read a '\n' terminated line
            if len(line) == 0:
                continue

            if(line[0] in dispatch.keys()):
                dispatch[line[0]](line[1:]) # call it

def read_midi_and_pipe_to_ttyUSB(callback, ttyUSB_file, midi_file):
    midi_device = subprocess.check_output("ls /dev/midi*", shell=True).strip()
    print "Found MIDI Device : " + midi_device

    with serial.Serial('/dev/ttyUSB1', 9600, timeout=2) as ser:
        with open(midi_device, 'r') as midi:
            while(True):
                s = midi.read(3)
                print 'got it'
                callback("MIDI IN:  %d   %d   %d\n" % (ord(s[0]),ord(s[1]),ord(s[2])))
                ser.write(s)


def write_graph_to_file(graph):
    pydot_graph = nx_pydot.to_pydot(blockGraph)
    png_str = pydot_graph.create_png(prog='dot')

    #write the png to disk
    with open("/tmp/output.png", "w") as text_file:
        text_file.write(png_str)

    #write the dot to disk
    nx_pydot.write_dot(blockGraph,'/tmp/output.graph')

root = Tk()
app = App(root)

messageDispatch = {
    'I':app.updateIdleTickCount,
    'L':app.updateEventLog,
    'M':app.updateMidiProcessedLog,
    'B':app.updateBlockGraphUpdateLog,
}

thread.start_new_thread(seriallogger_reader, (messageDispatch, 'hurr', 'durr'))
thread.start_new_thread(read_midi_and_pipe_to_ttyUSB, (app.updateMidiControllerInput, 'hurr', 'durr'))

# Read the block graph
blockGraph = nx_pydot.read_dot('test.graph')
blockGraph.add_node('yay')
blockGraph.nodes['yay']['label'] = 'hurr durr'
write_graph_to_file(blockGraph)


root.mainloop()
root.destroy()
