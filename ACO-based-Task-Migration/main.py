
import numpy as np
import acopy
import tsplib95

# data to test the code. 

taskSet = [0.2, 0.5, 0.7, 1.0, 0.8] # task set with 5 tasks. 
processorUti = [0, 0, 0, 0]  # 4 processors with no utilization. 
processorSpeed = [0.1, 0.2, 0.3, 0.4, 0.5] # available processor speed rates

# initiate ACO variables. 
solver = acopy.Solver( rho = .03, q = 1 )
colony = acopy.Colony( alpha = 1, beta = 3 )

problem = tsplib95.load( "/home/malsara/Research/repositories/MPSoC-Research/ACO-based-Task-Migration/bays29.tsp")

G = problem.get_graph()

tour = solver.solve( G, colony, limit = 100 )

print(tour.cost)

print (tour.nodes)

print (tour.path)