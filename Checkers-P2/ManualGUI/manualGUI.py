#############################################################
#	Instituto Tecnologico de Costa Rica						#
#	Computer Engineering									#
#															#
#	Programmer: Daniela Hernandez Alvarado (DaniHdez)		#	
#															#
#	Last update:30/5/2019									# 
#															#
#	Operating Systems Principles							#
#	Professor. Diego Vargas									#
#	tkinter	python3											#
#															#
#############################################################

from tkinter import *
from tkinter.ttk import *
import sys

# This path NEEDS TO BE UPDATED
sys.path.insert(0, '/home/estape/Documents/SO_EDJ/Checkers-P2/libmearm/python3')
import roboticPlayer

#Warning Popup window
class PopUp:
	def __init__(self):
		self.popUp = Tk()
		self.popUp.title("Warning!")
		self.popUp.geometry("200x50")
		self.canvasPop = Canvas(self.popUp, width = 200, height = 50, background = "#0C528E")
		self.canvasPop.pack()
		self.msgLabel = Label(self.canvasPop, text = "Please choose a position", background = "#0C528E", foreground = "#68B0E0")
		self.msgLabel.place(x = 15, y = 1)
		self.okButton = Button(self.canvasPop, text = "OK", command =  self.popUp.destroy, width = 3)
		self.okButton.place(x = 90, y = 20)					

#Main window
class DrawScreen:
	def __init__(self, window):
		self.window = window
		window.title("Manual Interaction GUI - EDJ")
		window.geometry("400x200")

		#Canvas
		self.canvas = Canvas(window, width = 400, height = 200)
		self.canvas.pack()

		#Image
		self.bgImage = PhotoImage(file = "images/bg.gif")
		self.canvas.create_image(0, 0 , anchor = NW, image = self.bgImage)

		#Title label 
		self.labelTitle = Label(self.canvas, compound = CENTER, text = "Manual Interaction GUI", foreground = "#68B0E0", font = " Times 15 bold italic", background = "#106095")
		self.labelTitle.place (x = 90, y = 5)
		self.labelAction = Label(self.canvas, text = "Set position:", foreground = "#68B0E0", font = "Times 13 italic", background = "#054583")
		self.labelAction.place (x = 15, y = 50)

		#Combobox
		self.position = Combobox(self.canvas, values = ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0", "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "1.8", "1.9", "2.0", "2.1", "2.2", "2.3", "2.4", "2.5", "2.6", "2.7", "2.8", "2.9", "3.0", "3.1", "3.2", "3.3", "3.4", "3.5", "3.6", "3.7", "3.8", "3.9", "4.0", "4.1", "4.2", "4.3", "4.4", "4.5", "4.6", "4.7", "4.8", "4.9", "5.0", "5.1", "5.2", "5.3", "5.4", "5.5", "5.6", "5.7", "5.8", "5.9", "6.0", "6.1", "6.2", "6.3", "6.4", "6.5", "6.6", "6.7", "6.8", "6.9", "7.0", "7.1", "7.2", "7.3", "7.4", "7.5", "7.6", "7.7", "7.8", "7.9", "8.0", "8.1", "8.2", "8.3", "8.4", "8.5", "8.6", "8.7", "8.8", "8.9", "9.0", "9.1", "9.2", "9.3", "9.4", "9.5", "9.6", "9.7", "9.8", "9.9"])
		self.position.place (x = 130, y = 50)

		#Buttons
		self.buttonMove = Button(self.canvas, text = "Move to", width = 10, command = self.move)
		self.buttonMove.place(x = 160, y = 85)

		self.buttonPick = Button(self.canvas, text = "Pick", width = 6, command = self.pick)
		self.buttonPick.place(x = 130, y = 130)

		self.buttonDrop = Button(self.canvas, text = "Drop", width = 6, command = self.drop)
		self.buttonDrop.place(x = 200, y = 130)

	#Button actions
	def move(self):
		self.positionMove = self.position.get()
		if (self.positionMove == ""):
			self.alertPopUp = PopUp()
		else: 
			#Move the arm 
			print(self.positionMove)

	def pick(self):
		#Pick the piece
		print("> pick")
		roboticPlayer.paser("pick")

	def drop(self):
		#drop the piece
		print("> drop")
		roboticPlayer.paser("drop")


def main():
	# Starts the interpreter
	roboticPlayer.initDevice("/dev/ttyUSB0", 115200)
	window = Tk()
	drawScreen = DrawScreen(window)
	window.mainloop()
	roboticPlayer.closeDevice()


if __name__ == "__main__":
	main()