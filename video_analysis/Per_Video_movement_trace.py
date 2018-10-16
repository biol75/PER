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

import pdb

#Enter file name here.
cap = cv2.VideoCapture('Sample video 2.avi') 
pdb.set_trace()

#fgbg = cv2.createBackgroundSubtractorMOG2() #extracts the foreground from the background

ret, frame = cap.read()
ret, first_frame = cap.read()
while (ret):
    #find position/roi in first frame then follow in each frame


#Video manipulation/editing
#Converts video to greyscale, binary and blurs the image
    img = cv2.cvtColor(frame,cv2.COLOR_RGB2GRAY)  # RGB file to greyscale
    ret, Binary = cv2.threshold(img, 22, 255, cv2.THRESH_BINARY) #Greyscale to binary, can adjust threshold by changing first number in brackets.
    blurred = cv2.GaussianBlur(Binary, (7,7),  0)
    blurred = cv2.erode(blurred, None, iterations =2)
    blurred = cv2.dilate(blurred, None, iterations =2)

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
    eye = [57,156] #enter coordinates of eye center here (x,y)
    eye = tuple(eye)
    list(zip(bottommost,eye))  
    distance = sqrt((bottommost[0] - eye[0])**2 + (bottommost[1]-eye[1])**2) #finds the distance between the coordinates    
    print distance #Can also be altered to allow the data to be exported to an external file, instead of displaeyd in the running window.

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
    
    
cap.release()
cv2.destroyAllWindows()



