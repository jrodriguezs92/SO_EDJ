#############################################################
#	Instituto Tecnologico de Costa Rica						#
#	Computer Engineering									#
#															#
#	Programmer: Daniela Hernandez Alvarado (DaniHdez)		#	
#															#
#	Last update:25/5/2019									# 
#															#
#	Operating Systems Principles							#
#	Professor. Diego Vargas									#
#	Pygame version: 1.9.6									#
#															#
#############################################################

#Libraries needed
import pygame
import sys 
from pygame.locals import * #to import pygame constants

# This path NEEDS TO BE UPDATED
sys.path.insert(0, '/home/estape/Documents/SO_EDJ/Checkers-P2/libmearm/python')
import roboticPlayer

#to use text
pygame.font.init() 

#Definition of colors
WHITE 		= (255, 255, 255)
LIGHTBLUE 	= (0, 102, 102)
PINK		= (153, 0, 76)  
GOLD		= (255, 215, 0)
HIGH		= (160, 190, 255)
GRAY		= (192, 192, 192)
BLACK		= (  0,   0,   0)
DARKPURPLE	= ( 51,   0,  51)
PEACH		= (255, 204, 153)

#Definition of directions of the movements
NORTHWEST = "northwest"
NORTHEAST = "northeast"
SOUTHWEST = "southwest"
SOUTHEAST = "southeast"

#Window size
WINDOWSIZE = 600

#Main class of the game, controls game drawing, screen updates, players turns and game flow in general
class Game: 
	def __init__(self):
		self.drawGame = DrawGame()
		self.board = Board()

		self.turn = LIGHTBLUE
		self.clickedPiece = None
		self.hop = False
		self.selectedLegalMoves = []

	#Window setup
	def setup(self):
		self.drawGame.initializeScreen()

	#Trigger events: exit and mouse click, here the pieces move 
	#Here robotic arm moves have to be define
	def eventLoop(self):
		self.mousePos = self.drawGame.boardCoords(pygame.mouse.get_pos())
		if self.clickedPiece != None:
			self.selectedLegalMoves = self.board.legalMoves(self.clickedPiece, self.hop)

		for event in pygame.event.get():
			#If x button is pressed
			if event.type == QUIT:
				pygame.quit()
				sys.exit
			if event.type == MOUSEBUTTONDOWN:
				if self.hop == False:
					if self.board.location(self.mousePos).occupant != None and self.board.location(self.mousePos).occupant.color == self.turn:
						self.clickedPiece = self.mousePos
					elif self.clickedPiece != None and self.mousePos in self.board.legalMoves(self.clickedPiece):
						self.board.movePiece(self.clickedPiece, self.mousePos)

						if self.mousePos not in self.board.diagonalSquare(self.clickedPiece):
							self.board.removePiece((self.clickedPiece[0]+(self.mousePos[0]-self.clickedPiece[0])/2, self.clickedPiece[1]+(self.mousePos[1]-self.clickedPiece[1])/2))
							self.hop = True
							self.clickedPiece = self.mousePos
						else:
							self.endTurn()
			if self.hop == True:
				if self.clickedPiece != None and self.mousePos in self.board.legalMoves(self.clickedPiece, self.hop):
					self.board.movePiece(self.clickedPiece, self.mousePos)
					self.board.removePiece((self.clickedPiece[0]+(self.mousePos[0]-self.clickedPiece[0])/2, self.clickedPiece[1]+(self.mousePos[1]-self.clickedPiece[1])/2))
				if self.board.legalMoves(self.mousePos, self.hop) == []:
					self.endTurn()
				else:
					self.clickedPiece = self.mousePos

	#Update screen 
	def update(self):
		self.drawGame.updateScreen(self.board, self.selectedLegalMoves, self.clickedPiece)

	#Controls the flow of the game
	def main(self):
		self.setup()
		while True:
			self.eventLoop()
			self.update()

	#Switch player turn 
	def endTurn(self):
		if self.turn == LIGHTBLUE:
			self.turn = PINK
		else:
			self.turn = LIGHTBLUE
		self.clickedPiece = None
		self.selectedLegalMoves = []
		self.hop = False
		if self.checkendGame():
			if self.turn == LIGHTBLUE:
				self.drawGame.showMessage("PINK WINS!")
			else:
				self.drawGame.showMessage("LIGHTBLUE WINS!")

	#Check if game is over
	def checkendGame(self):
		for x in xrange(10):
			for y in xrange(10):
				if self.board.location((x,y)).color == GRAY and self.board.location((x,y)).occupant != None and self.board.location((x,y)).occupant.color == self.turn:
					if self.board.legalMoves((x,y)) != []:
						return False
		return True

#Draw game on screen
class DrawGame:
	def __init__(self):
		self.caption = "EDJ Checkers"

		#Create clock
		self.fps = 60
		self.clock = pygame.time.Clock()

		#Add music 
		pygame.mixer.init()
		pygame.mixer.music.load('music/game.mp3')
		pygame.mixer.music.play(-1,0.0)

		#Create window
		self.screen = pygame.display.set_mode((WINDOWSIZE, WINDOWSIZE))
		self.background = pygame.image.load('images/BOARD.png')

		self.squareSize = WINDOWSIZE / 10 
		self.pieceSize = self.squareSize / 2

		self.message = False 

	#Initialize the window 
	def initializeScreen(self):
		pygame.init()
		pygame.display.set_caption(self.caption)

	#Updates screen, pieces on board, possible moves and check if a player wins to display winning message
	def updateScreen(self, board, legalMoves, clickedPiece):
		self.screen.blit(self.background, (0,0))

		self.possibleMoves(legalMoves, clickedPiece)
		self.drawPieces(board)

		if self.message:
			self.screen.blit(self.textSurface, self.textRect)

		pygame.display.update()
		self.clock.tick(self.fps)

	#Drawing pieces
	def drawPieces(self, board):
		for x in xrange(10):
			for y in xrange(10):
				if board.matrix[x][y].occupant != None:
					pygame.draw.circle(self.screen, board.matrix[x][y].occupant.color, self.pixelCoords((x,y)), self.pieceSize)
					if board.location((x,y)).occupant.king == True:
						pygame.draw.circle(self.screen, PEACH, self.pixelCoords((x,y)), int (self.pieceSize/1.7), self.pieceSize/4)
    
    #Returns the pixel coordinates of the center of the square
	def pixelCoords(self, coords):
		return (coords[0] * self.squareSize + self.pieceSize, coords[1] * self.squareSize + self.pieceSize)
	
    #Takes coordinates of a pixel and returns the square 
	def boardCoords(self, (pixelX, pixelY)):
		return (pixelX/self.squareSize, pixelY/self.squareSize)

    #Change color to the squares where the player can move
	def possibleMoves(self, squares, origin):
		for square in squares:
			pygame.draw.rect(self.screen, HIGH, (square[0]*self.squareSize, square[1]*self.squareSize, self.squareSize, self.squareSize))
		if origin != None:
			pygame.draw.rect(self.screen, HIGH, (origin[0]*self.squareSize, origin[1]*self.squareSize, self.squareSize, self.squareSize))

    #Shows message in the middle of the screen
	def showMessage(self, message):
		self.message = True
		self.fontObj = pygame.font.Font('freesansbold.ttf',44)
		self.textSurface = self.fontObj.render(message, True, HIGH, DARKPURPLE)
		self.textRect = self.textSurface.get_rect()
		self.textRect.center = (WINDOWSIZE/2, WINDOWSIZE/2)

#Create game board matrix
class Board:
	def __init__(self):
		self.matrix = self.newBoard()

	#Place squares in matrix
	def newBoard(self):
		matrix = [[None] * 10 for i in xrange(10)]

		for x in xrange(10):
			for y in xrange(10):
				if (x % 2 != 0) and (y % 2 == 0):
					matrix[y][x] = Square(WHITE)
				elif (x % 2 != 0) and (y % 2 != 0):
					matrix[y][x] = Square(GRAY)
				elif (x % 2 == 0) and (y % 2 != 0):
					matrix[y][x] = Square(WHITE)
				elif (x % 2 == 0) and (y % 2 == 0): 
					matrix[y][x] = Square(GRAY)
		
		#Pieces initial positions
		for x in xrange(10):
			for y in xrange(4):
				if matrix[x][y].color == GRAY:
					matrix[x][y].occupant = Piece(PINK)
			for y in xrange(6, 10):
				if matrix[x][y].color == GRAY:
					matrix[x][y].occupant = Piece(LIGHTBLUE)

		return matrix

	#Directions
	def directionMove(self, dir, (x,y)):
		if dir == NORTHEAST:
			return (x+1, y-1)
		elif dir == NORTHWEST:
			return (x-1, y-1)
		elif dir == SOUTHWEST:
			return (x-1, y+1)
		elif dir == SOUTHEAST:
			return (x+1, y+1)
		else: 
			return 0


	#Returns a list of squares that are diagonal from the (x, y) square given 
	def diagonalSquare(self,(x,y)):
		return [self.directionMove(NORTHWEST,(x,y)), self.directionMove(NORTHEAST,(x,y)), self.directionMove(SOUTHWEST,(x,y)), self.directionMove(SOUTHEAST,(x,y))]

	#Returns matrix[x][y] value 
	def location(self, (x,y)):
		return self.matrix[x][y]

	#Returns a list of legal moves when the player can take other player piece
	def blindLegalMoves(self, (x,y)):
		if self.matrix[x][y].occupant != None:
			
			if self.matrix[x][y].occupant.king == False and self.matrix[x][y].occupant.color == LIGHTBLUE:
				blindLegalMoves = [self.directionMove(NORTHWEST, (x,y)), self.directionMove(NORTHEAST, (x,y))]
				
			elif self.matrix[x][y].occupant.king == False and self.matrix[x][y].occupant.color == PINK:
				blindLegalMoves = [self.directionMove(SOUTHWEST, (x,y)), self.directionMove(SOUTHEAST, (x,y))]

			else:
				blindLegalMoves = [self.directionMove(NORTHWEST, (x,y)), self.directionMove(NORTHEAST, (x,y)), self.directionMove(SOUTHWEST, (x,y)), self.directionMove(SOUTHEAST, (x,y))]

		else:
			blindLegalMoves = []

		return blindLegalMoves

	#Returns a list of the legal moves of the x, y positions that recieves
	def legalMoves(self, (x,y), hop = False):
		blindLegalMoves = self.blindLegalMoves((x,y))
		legalMoves = []

		if hop == False:
			for move in blindLegalMoves:
				if hop == False:
					if self.pieceExist(move): 
						if self.location(move).occupant == None:
							legalMoves.append(move)
						elif self.location(move).occupant.color != self.location((x,y)).occupant.color and self.pieceExist((move[0] + (move[0] - x), move[1] + (move[1] - y))) and self.location((move[0] + (move[0] - x), move[1] + (move[1] - y))).occupant == None:
							legalMoves.append((move[0] + (move[0] - x), move[1] + (move[1] - y)))

		if hop == True:
			for move in blindLegalMoves:
				if self.pieceExist(move) and self.location(move).occupant != None:
					if self.location(move).occupant.color != self.location((x,y)).occupant.color and self.pieceExist((move[0] + (move[0] - x), move[1] + (move[1] - y))) and self.location((move[0] + (move[0] - x), move[1] + (move[1] - y))).occupant == None: 
						legalMoves.append((move[0] + (move[0] - x), move[1] + (move[1] - y)))

		return legalMoves

	#Removes the piece from the board, changing position occupant state
	def removePiece(self,(x,y)):
		self.matrix[x][y].occupant = None

	#Move the piece to the new position 
	def movePiece(self, (startX, starY), (endX, endY)):
		self.matrix[endX][endY].occupant = self.matrix[startX][starY].occupant
		self.removePiece((startX, starY))

		self.king((endX, endY))

	#If the piece is in the square of the final line
	def finalSquare(self, coords):
		if coords[1] == 0 or coords[1] == 7:
			return True
		else:
			return False 

	#Check if a piece exist in the coords
	def pieceExist(self, (x,y)):
		if x < 0 or y < 0 or x > 9 or y > 9:
			return False
		else:
			return True

	#Asign an identification to the piece that get the end line
	def king(self, (x,y)):
		if self.location((x,y)).occupant != None:
			if (self.location((x,y)).occupant.color == LIGHTBLUE and y == 0) or (self.location((x,y)).occupant.color == PINK and y == 9):
				self.location((x,y)).occupant.king = True 

class  Piece:
	def __init__(self, color, king=False):
		self.color = color
		self.king = king

class Square:
	def __init__(self, color, occupant = None):
		self.color = color
		self.occupant = occupant

#Calls the Game class
def main():
	# Starts the interpreter
	roboticPlayer.initDevice("/dev/ttyUSB0", 115200)
	game = Game()
	game.main()
	roboticPlayer.closeDevice()

if __name__ == "__main__":
	main()


