from tkinter import *
import tkinter.scrolledtext
import thread
import time
import serial
import networkx as nx
import networkx.drawing.nx_pydot as nx_pydot
import subprocess
import queue
import json
import select

class App:
    def __init__(self, master, blockGraphQueue):
        self.frame = Frame(master, height=300, width=300)

        self.quitButton = Button(self.frame, text="QUIT", fg="red", command=self.frame.quit)
        self.helloButton = Button(self.frame, text="Hello", command=self.say_hi)

        self.idleTickCount = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=25)
        self.midiControllerInput = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=45)
        self.midiProcessedLog = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=45)
        self.eventLog = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=50)
        self.nodeUpdateLog = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=50)
        self.edgeListUpdateLog = tkinter.scrolledtext.ScrolledText(self.frame, height=12, width=50)


        self.graphImage = PhotoImage(file="graph.png")
        self.graphLabel = Label(self.frame, image=self.graphImage)



        self.frame.grid(row=0, column=0)
        self.quitButton.grid(row=0, column=0)
        self.helloButton.grid(row=0, column=2)

        self.idleTickCount.grid(row=5, column=0,sticky=W)
        self.midiControllerInput.grid(row=5, column=1,sticky=W)
        self.midiProcessedLog.grid(row=5, column=2,sticky=W)
        self.eventLog.grid(row=5, column=3,sticky=W)
        self.nodeUpdateLog.grid(row=6, column=0, sticky=W)
        self.edgeListUpdateLog.grid(row=6, column=2, sticky=W)
        self.graphLabel.grid(row=7, column=0, columnspan=5)

        self.blockGraphQueue = blockGraphQueue

    def say_hi(self):
        print "hi there!"

    def updateTextWidgetWithMaxLineCount(self, widget, maxlines, theText):
        endlinecol = widget.index(END)
        linenum = int(endlinecol.split('.')[0])
        if linenum > 200:
            widget.delete(1.0,END)

        widget.insert(END, theText)
        widget.see("end")

    def updateIdleTickCount(self, theText):
        self.updateTextWidgetWithMaxLineCount(self.idleTickCount, 200, theText)

    def updateMidiControllerInput(self, theText):
        self.updateTextWidgetWithMaxLineCount(self.midiControllerInput, 200, theText)

    def updateMidiProcessedLog(self, theText):
        self.updateTextWidgetWithMaxLineCount(self.midiProcessedLog, 200, theText)

    def updateEventLog(self, theText):
        self.updateTextWidgetWithMaxLineCount(self.eventLog, 200, theText)

    def updateNodeUpdateLog(self, theText):
        self.updateTextWidgetWithMaxLineCount(self.nodeUpdateLog, 200, theText)

    def updateEdgeListUpdateLog(self, theText):
        self.updateTextWidgetWithMaxLineCount(self.edgeListUpdateLog, 200, theText)


    def updateBlockGraphDisplay(self, pathToGraphImage):
        self.graphImage = PhotoImage(file=pathToGraphImage)
        self.graphLabel.configure(image=self.graphImage)

    def receivedNodeUpdate(self, theText):
        self.updateNodeUpdateLog(theText)

        # modify the graph data structure
        items = theText.strip().split(',')
        node_name = items[0]
        if node_name not in blockGraph.nodes:
            print 'name : ' + node_name + 'not in nodes: '
            print blockGraph.nodes.items()
            return

        def make_graphviz_label(name, param, value):
            label = "%s|{%s|%s}" %(name, param, value)
            return label

        blockGraph.nodes[node_name]['label'] = '%s' % make_graphviz_label(items[0], items[1], items[2])
        blockGraph.nodes[node_name]['shape'] = 'record'

        # put the updated graph on the queue, to be picked up by the reader thread
        if self.blockGraphQueue.full():
            self.blockGraphQueue.get_nowait()
        self.blockGraphQueue.put_nowait(blockGraph)

    def receivedEdgeListUpdate(self, theText):
        self.updateEdgeListUpdateLog(theText)

        # remove all edges
        for edge in blockGraph.edges():
            blockGraph.remove_edge(edge[0], edge[1]) #terrible API

        # load them back in
        edges = json.loads(theText)
        for edge in edges:
            blockGraph.add_edge(edge['block'], edge['next'])

        # put the updated graph on the queue, to be picked up by the reader thread
        if self.blockGraphQueue.full():
            self.blockGraphQueue.get_nowait()
        self.blockGraphQueue.put_nowait(blockGraph)





def thread_seriallogger_reader(dispatch, arg2, arg3):

    with serial.Serial('/dev/ttyUSB0', 921600, timeout=2) as ser:
        while(True):
            line = ser.readline()   # read a '\n' terminated line
            if len(line) == 0:
                continue

            if(line[0] in dispatch.keys()):
                dispatch[line[0]](line[1:]) # call it


# Open all /dev/midi* devices, pipe them all into the serial port output
def thread_read_midi_and_pipe_to_ttyUSB(callback):
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
                callback("MIDI IN:  %d   %d   %d\n" % (ord(s[0]),ord(s[1]),ord(s[2])))
                ser.write(s)


def thread_render_block_graph_and_callback(queue, callback):

    def write_graph_to_file(graph):
        pydot_graph = nx_pydot.to_pydot(graph)
        png_str = pydot_graph.create_png(prog='dot')

        #write the png to disk
        with open("/tmp/output.png", "w") as text_file:
            text_file.write(png_str)

        #write the dot to disk
        nx_pydot.write_dot(graph,'/tmp/output.graph')

    while(True):
        # blocking read the queue
        graph = queue.get()

        write_graph_to_file(graph)

        # update the display
        callback('/tmp/output.png')


###################### MAIN #########################

root = Tk()
blockGraphQueue = queue.Queue(maxsize=4)
app = App(root, blockGraphQueue)

# Serial Port Dispatch
messageDispatch = {
    'I':app.updateIdleTickCount,
    'L':app.updateEventLog,
    'M':app.updateMidiProcessedLog,
    'B':app.receivedNodeUpdate,
    'E':app.receivedEdgeListUpdate,
}

thread.start_new_thread(thread_seriallogger_reader, (messageDispatch, 'hurr', 'durr'))

# MIDI Controller Forwarding
thread.start_new_thread(thread_read_midi_and_pipe_to_ttyUSB, (app.updateMidiControllerInput,))

# Block Graph Display Update Thread
thread.start_new_thread(thread_render_block_graph_and_callback, (blockGraphQueue, app.updateBlockGraphDisplay))

# Read the initial block graph (currently blank)
# Don't know why I can't just create a new graph here...
blockGraph = nx_pydot.read_dot('blank.graph')

root.mainloop()
root.destroy()
