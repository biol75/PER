##Video editing before upload:

#Record the frame in which the first flash occurs, this is needed for later analysis.
#Using imagej/fiji remove frames where the flash obscures the image.
#Crop the video so that only the head remains/Find the region of interest (ROI) - leave room for the proboscis extension to be caught in the video
#Record x, y coordinates of the centre of the eye this will be needed to calcualte probocsis distance when entered into the program.
#Place file in same folder as running script 

#Based on code started by Amy Stewart. Copyright Amy Stewart, Chris Elliott, University of York
# GPL 2


#Import Modules
import cv2
import numpy as np
from math import sqrt
import matplotlib.pyplot as plt


import time
import imutils

import sys
import pdb

#####################################################################################
# def auto_canny(image, sigma=0.5):
# 	compute the median of the single channel pixel intensities
# 	v = np.median(image)
#  
# 	apply automatic Canny edge detection using the computed median
# 	lower = int(max(0, (1.0 - sigma) * v))
# 	upper = int(min(255, (1.0 + sigma) * v))
# 	edged = cv2.Canny(image, lower, upper)
#  
# 	return the edged image
# 	return edged
# 	
####################################################################################	


#pdb.set_trace()


if len(sys.argv) != 4 :
	print ('Usage: filename eyeX eyeY');
	sys.exit(1)

eye = [int(sys.argv[2]),int(sys.argv[3])] #enter coordinates of eye center here (x,y)
eye = tuple(eye)

fileName = sys.argv[1]
cap = cv2.VideoCapture(fileName) 


#global variables..
i = 1
m = [0]
d = [0]
p = [0]
a = [0]
seen_flash = False

# look at first image
ret, frame = cap.read()
img = cv2.cvtColor(frame,cv2.COLOR_RGB2GRAY)  # RGB file to greyscale
init_mean_val = cv2.mean(img) [0]

while (ret):
#Video manipulation/editing
#Converts video to greyscale, binary and blurs the image#
    ret, Binary = cv2.threshold(img, init_mean_val, 255, cv2.THRESH_BINARY ) #Greyscale to binary, can adjust threshold by changing init_mean_val.
#    ret, Binary = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU) #Greyscale to binary, can adjust threshold by changing MyThreshold.
    blurred = cv2.GaussianBlur(Binary, (7,7),  0)
    blurred = cv2.erode(blurred, None, iterations =2)
    blurred = cv2.dilate(blurred, None, iterations =2)
    
#calculate mean intensity of the greyscale image and test if we have a flash
    tmp_mean_val = cv2.mean(img) [0]
    if tmp_mean_val > 4 * init_mean_val :
        seen_flash = True
        
#save distance 
    if (seen_flash):
        #file.write(repr(i) +',' + repr(mean_val) + ',' + repr(distance) + ',' + repr(perimeter) + "\n")
        m.append (mean_val)
        d.append (distance)
        p.append (perimeter)
        a.append (area)
    
    mean_val = tmp_mean_val    
        
##Finding distance between centre of the eye and the tip of the proboscis
#Finds contours of object in the video
    # was im, contours, hierarchy = cv2.findContours(blurred, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#     auto = auto_canny(blurred)
    (cnts, _) = cv2.findContours(blurred, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cnts = sorted(cnts, key = cv2.contourArea, reverse = True)[:10]
    
#   cv2.drawContours(frame, contours, -1, (0, 255, 0), 2) #adds contour lines to video
    cv2.drawContours(frame, cnts, 0, (0, 255, 0), 2) #adds contour lines to video

# Determine the lowest point along the contour
    count = cnts[-1]
    bottommost = tuple(count[count[:, :, 1].argmax()][0])

###Determining distance proboscis has moved from eye center (pixels):
    list(zip(bottommost,eye))  
    distance = sqrt((bottommost[0] - eye[0])**2 + (bottommost[1]-eye[1])**2) #finds the distance between the coordinates    

# length of (smoothed) contour
    epsilon = 0.05*cv2.arcLength(cnts[0], True)
    approx = cv2.approxPolyDP(cnts[0], epsilon, True)    
    perimeter = cv2.arcLength(approx,True)
# and area    
    area = cv2.contourArea(approx)
    
# Applies a circle to the lowest point of the proboscis and the centre of the eye so that when displayed the point being tracked can be observed.
    cv2.circle(frame, bottommost, 5, (255, 255, 0), -1)
    cv2.circle(frame, eye, 5, (255, 255, 0), -1)

#Shows videos
    cv2.imshow('Binary', Binary)
    cv2.imshow('Original', frame)
    
    if (i == 198):
        # save last image with contour
		figName = fileName.replace ('.avi', '.png')
		figName = figName.rsplit('/', 1)[-1]
		figName = 'contour' + figName 
		cv2.imwrite  (figName, frame) 
    
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
            
    ret, frame = cap.read()
    img = cv2.cvtColor(frame,cv2.COLOR_RGB2GRAY)  # RGB file to greyscale
    
    if (seen_flash):
        i = i + 1 # count the frames
    
    if (i > 202):  # 200 frames is about 1s
        break  # finish data


#all done....       
cap.release()
cv2.destroyAllWindows()

# create a file (at least on unix) in the current directory to record the analysis of the video in some other directory
fWriteName = fileName.replace ('.avi', '.csv')
fWriteName = fWriteName.rsplit('/', 1)[-1]
file = open(fWriteName,"w")
file.write('index,' + 'mean_val,' + 'distance,' + 'perimeter,' + 'area' + "\n")
for i in range(1,200):
#file.write(repr(i) +',' + repr(mean_val) + ',' + repr(distance) + ',' + repr(perimeter) + "\n")
    file.write(repr(i) +',' + repr(m[i]) + ',' + repr(d[i]) + ',' + repr(p[i]) + ',' + repr(a[i]) + "\n")
file.close()

#draw a graph
#pdb.set_trace()
figName = fileName.replace ('.avi', '.png')
figName = figName.rsplit('/', 1)[-1]
fig = plt.figure()
plt.plot ( range(7,199), p[8:200], range(7,199), a[8:200])
plt.show()
fig.savefig(figName)




