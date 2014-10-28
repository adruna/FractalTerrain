Fractal Terrain!

**** PRESS 'm', THEN YOU CAN USE THE MOUSE TO LOOK AROUND, PRESS AGAIN TO GET CONTROL OF YOUR MOUSE AGAIN.
ESC - exits

Camera Controls :
	m - Toggles capture of mouse to orient camera.
	
	Basic FPS style
	W/w - Forward
	S/s - Backward
	A/a - Left
	D/d - Right
	Space - Up
	b - Speed up movement
	B - Slow down movement
	
Terrain Controls
	Starts out with only the 4 outermost points calculated.
	
	The coloured in vertices indicate the next step to be calculated(red = 4 pints to be averaged, green = point that will be calculated).
	
	p - Continuously steps through while held down.
	P - Does a single calculation on the vertices indicated. (must be pressed again for second calculation).
	O/o - Finishes the current iteration (if on square, finishes all the squares until we go back to diamond) 
					(if on diamond finishes all the diamonds until we go back to do the next squares at half size)
	I/i - Finishes all calculations.
	
	INCODE:
		TerrainGenerator.h
		GRID_SIZE = number of points in 1 dimension of the grid (power of 2 + 1 works best)
		CELL_SIZE = size of each individual cell (larger numbers might not look larger but you'll perceive that the camera doesn't seem as fast)
		RANDOMNESS = how much random can be added/subtracted each step (gets smaller as the blocks get smaller, so this is just the starting point).
		
	