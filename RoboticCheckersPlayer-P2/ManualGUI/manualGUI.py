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

#Button actions
def move():
	positionMove = position.get()
	if (positionMove == ""):
		alertPopUp("", "Please choose a position")
	else: 
		#Move the arm 
		print(positionMove)
def pick():
	#Pick the piece
	pass
def drop():
	#drop the piece
	pass

#Popup
def alertPopUp(title, message):
	popUp = Tk()
	popUp.title(title)
	popUp.geometry("200x50")
	msgLabel = Label(popUp, text = message)
	msgLabel.pack()
	okButton = Button(popUp, text = "OK", command =  popUp.destroy, width = 6)
	okButton.pack()

#Window
window = Tk()
window.title("Manual Interaction GUI - EDJ")
window.geometry("400x200")

#Canvas
canvas = Canvas(window, width = 400, height = 200, background = "blue")
canvas.pack()

#Title label 
labelTitle = Label(canvas, text = "Manual Interaction GUI", font = 40, background = "blue")
labelTitle.place (x = 100, y = 1)
labelAction = Label(canvas, text = "Set position:", font = 10, background = "blue")
labelAction.place (x = 10, y = 50)

#Combobox
position = Combobox(canvas, values = ["0.0", "0.1", "0.2", "0.3", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1.0", "1.1", "1.2", "1.3", "1.4", "1.5", "1.6", "1.7", "1.8", "1.9", "2.0", "2.1", "2.2", "2.3", "2.4", "2.5", "2.6", "2.7", "2.8", "2.9", "3.0", "3.1", "3.2", "3.3", "3.4", "3.5", "3.6", "3.7", "3.8", "3.9", "4.0", "4.1", "4.2", "4.3", "4.4", "4.5", "4.6", "4.7", "4.8", "4.9", "5.0", "5.1", "5.2", "5.3", "5.4", "5.5", "5.6", "5.7", "5.8", "5.9", "6.0", "6.1", "6.2", "6.3", "6.4", "6.5", "6.6", "6.7", "6.8", "6.9", "7.0", "7.1", "7.2", "7.3", "7.4", "7.5", "7.6", "7.7", "7.8", "7.9", "8.0", "8.1", "8.2", "8.3", "8.4", "8.5", "8.6", "8.7", "8.8", "8.9", "9.0", "9.1", "9.2", "9.3", "9.4", "9.5", "9.6", "9.7", "9.8", "9.9"])
position.place (x = 130, y = 50)

#Buttons
buttonMove = Button(canvas, text = "Move to", width = 10, command = move)
buttonMove.place(x = 160, y = 85)

buttonPick = Button(canvas, text = "Pick", width = 6, command = pick)
buttonPick.place(x = 130, y = 130)

buttonDrop = Button(canvas, text = "Drop", width = 6, command = drop)
buttonDrop.place(x = 200, y = 130)

window.mainloop()