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
import time
import imutils

import sys
import pdb

pdb.set_trace()

if len(sys.argv) != 5 :
	print ('Usage: filename eyeX eyeY threshold');
	sys.exit(1)

eye = [int(sys.argv[2]),int(sys.argv[3])] #enter coordinates of eye center here (x,y)
eye = tuple(eye)

MyThreshold = int(sys.argv[4])

fileName = sys.argv[1]
cap = cv2.VideoCapture(fileName) 


# create a file (at least on unix) in the current directory to record the analysis of the video in some other directory
fWriteName = fileName.replace ('.avi', '.csv')
fWriteName = fWriteName.rsplit('/', 1)[-1]
file = open(fWriteName,"w")

ret, frame = cap.read()
i = 1
# look at first image


while (ret):
    #find position/roi in first frame then follow in each frame


#Video manipulation/editing
#Converts video to greyscale, binary and blurs the image
    img = cv2.cvtColor(frame,cv2.COLOR_RGB2GRAY)  # RGB file to greyscale
    ret, Binary = cv2.threshold(img, MyThreshold, 255, cv2.THRESH_BINARY) #Greyscale to binary, can adjust threshold by changing first number in brackets.
    blurred = cv2.GaussianBlur(Binary, (7,7),  0)
    blurred = cv2.erode(blurred, None, iterations =2)
    blurred = cv2.dilate(blurred, None, iterations =2)
    
    #calculate mean intensity of the greyscale image
    mean_val = cv2.mean(img)

##Finding distance between centre of the eye and the tip of the proboscis
#Finds contours of object in the video
    # was im, contours, hierarchy = cv2.findContours(blurred, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours, hierarchy = cv2.findContours(blurred, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    count = contours[-1]
    c = max(count, key=cv2.contourArea)
    cv2.drawContours(frame, contours, -1, (0, 255, 0), 2) #adds contour lines to video

# Determine the lowest point along the contour
    bottommost = tuple(count[count[:, :, 1].argmax()][0])

###Determining distance proboscis has moved from eye center (pixels):
    list(zip(bottommost,eye))  
    distance = sqrt((bottommost[0] - eye[0])**2 + (bottommost[1]-eye[1])**2) #finds the distance between the coordinates    
    #print distance #Can also be altered to allow the data to be exported to an external file, instead of displaeyd in the running window.
    file.write(repr(i) +',' + repr(mean_val[0]) + ',' + repr(distance) + "\n")
    

# Applies a circle to the lowest point of the proboscis and the centre of the eye so that when displayed the point being tracked can be observed.
    cv2.circle(frame, bottommost, 5, (255, 255, 0), -1)
    cv2.circle(frame, eye, 5, (255, 255, 0), -1)

#Shows videos
    cv2.imshow('Original', frame)
    cv2.imshow('Binary', Binary)

    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
    ret, frame = cap.read()
    i = i + 1 # count the frames
    
    
cap.release()
cv2.destroyAllWindows()
file.close()



