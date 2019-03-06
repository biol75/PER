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
from scipy.interpolate import UnivariateSpline
import matplotlib.pyplot as plt


import time
import imutils

import sys
import os.path
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


#if len(sys.argv) != 4 :
#	print ('Usage: filename eyeX eyeY');
#	sys.exit(1)

#eye = [int(sys.argv[2]),int(sys.argv[3])] #enter coordinates of eye center here (x,y)
#eye = tuple(eye)

fullFileName = sys.argv[1]
cap = cv2.VideoCapture(fullFileName)

dirName, fileName = os.path.split(fullFileName)
dirName = dirName.replace("/myRaid/videos/","")
if not os.path.exists (dirName):
    os.makedirs(dirName)

figName = fileName.replace ('.avi', '.pdf')
figName = dirName + '/' + figName

# if we already have a figure, just exit
if (os.path.isfile (figName)):
    exit()

fig = plt.figure()
grid = plt.GridSpec(2, 3, wspace=0.4, hspace=0.3)

#global variables..
i = 1
m = [0]
d = [0]
p = [0]
a = [0]
cX = 0
cY = 0
seen_flash = False
isFirstFrame = True

# look at first image
ret, frame = cap.read()
img = cv2.cvtColor(frame,cv2.COLOR_RGB2GRAY)  # RGB file to greyscale
histr = cv2.calcHist([frame],[1],None,[256],[0,256])
modeintensity = np.argmax(histr)
plt.subplot(grid[0,0])
plt.plot(histr,color = 'g')
plt.xlim([0,256])
#plt.show()

init_mean_val = (7 * modeintensity)/3  #cv2.mean(img) [0]

height, width, _ = frame.shape
cv2.namedWindow('Binary', cv2.WINDOW_NORMAL)
cv2.resizeWindow('Binary', height, width)
cv2.moveWindow('Binary',100,100)

#

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
        m.append (mean_val)
        d.append (distance) 
        p.append (perimeter)
        a.append (area)
        
    mean_val = tmp_mean_val    
        
##Finding distance between centre of the eye and the tip of the proboscis
#Finds contours of object in the video
    # was im, contours, hierarchy = cv2.findContours(blurred, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#     auto = auto_canny(blurred)
    (im, cnts, _) = cv2.findContours(blurred, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cnts = sorted(cnts, key = cv2.contourArea, reverse = True)[:10]
    
#   cv2.drawContours(frame, contours, -1, (0, 255, 0), 2) #adds contour lines to video
    cv2.drawContours(frame, cnts, 0, (0, 255, 0), 2) #adds contour lines to video
    #pdb.set_trace()
# Determine the lowest point along the contour (x,y)
    count = cnts[0] # was -1
    bottommost = tuple(count[count[:, :, 1].argmax()][0])

#calculate centroid as approximation of eye...    
    if cX == 0:
	M = cv2.moments(count)
	cX = int(M["m10"] / M["m00"])
	cY = int(M["m01"] / M["m00"])
    
###Determining distance proboscis has moved from eye center (pixels):
    distance = sqrt((bottommost[0] - cX)**2 + (bottommost[1]-cY)**2) #finds the distance between the coordinates    

# length of (smoothed) contour 0.005 is too much 0.001 too little
    epsilon = 0.002*cv2.arcLength(cnts[0], True)
    approx = cv2.approxPolyDP(cnts[0], epsilon, True) # returns a numpy array   
    perimeter = cv2.arcLength(approx,True)
# and area    
    area = cv2.contourArea(approx) / 1000
    cv2.drawContours(frame, [approx], -1, (0, 0, 255), 3)
    
# Applies a circle to the lowest point of the proboscis and the centre of the eye so that when displayed the point being tracked can be observed.
    cv2.circle(frame, bottommost, 5, (255, 255, 0), -1)
    cv2.circle(frame, (cX, cY), 5, (255, 255, 0), -1)

#Shows videos
    cv2.imshow('Binary', Binary)
    cv2.imshow(fileName, frame)
    
    if (isFirstFrame):
        m [0] = (mean_val)
        d [0] = (distance) 
        p [0] = (perimeter)
        a [0] = (area)
        
        # save first image with contour
        plt.subplot(grid[1,0])
	plt.imshow(frame) 
        isFirstFrame = False

                
    k = cv2.waitKey(30) & 0xff
    if k == 27:
        break
            
    ret, frame = cap.read()
    if not ret:
        print ("No flash found in " + fullFileName )
        # create a dummy figure
        text_file= open (figName, "w")
        text_file.write("No flash")
        text_file.close()
        sys.exit(1)
        
    img = cv2.cvtColor(frame,cv2.COLOR_RGB2GRAY)  # RGB file to greyscale
    
    if (seen_flash):
        i = i + 1 # count the frames
    
    if (i > 202):  # 200 frames is about 1s
        break  # finish data


#all done....       
cap.release()
cv2.destroyAllWindows()

#pdb.set_trace()

#create an univariate spline approximation
x = np.arange(7,i) 
spl = UnivariateSpline(x, a[7:i])
spl.set_smoothing_factor(50) # default of i (203) seems enough

pp = np.array(p)/p[0]
aa = np.array(a)/a[0]
dd = np.array(d)/d[0]

# create a file (at least on unix) in the current directory to record the analysis of the video in some other directory
fWriteName = fileName.replace ('.avi', '.csv')
fWriteName = dirName + '/' + fWriteName
file = open(fWriteName,"w")
file.write(',genotype, file, initial area, area spline Max, MaxAt, actual area max, actual area MaxAt, residual \n')
file.write('Summary' + ',' + dirName + ',' + fileName + ',' + repr(a[0]) + ',' + repr(max(spl(x))) + ',' + repr (7 + np.argmax(spl(x)))  + ',' )
file.write( repr(max(a[7:200])) + ',' + repr(np.argmax(aa[7:200])) + ',' + repr(spl.get_residual()) + '\n\n') 


file.write('\n')
file.write('frame,' + 'mean_val,' + 'distance,' + 'perimeter,' + 'area,' + 'area spline,' + 'area difference' + "\n")

for i in range(7,200):
    #spl returns an ndarray
    aTmp = spl(i-7).item()
    file.write(repr(i) +',' + repr(m[i]) + ',' + repr(d[i]) + ',' + repr(p[i]) + ',' + repr(a[i]) + ',' + repr(aTmp) + ',' + repr(abs(a[i] - aTmp)) + "\n")
file.close()

#write a header file for grep to find
fWriteName = 'summary.csv'
if not os.path.isfile (fWriteName):
    file = open(fWriteName,"w")
    file.write('Summary,genotype, file, initial area, area spline Max, MaxAt, actual area max, actual area MaxAt, residual \n') 
    file.close()


#draw a graph
ax = plt.subplot(grid[0:, 1:])
# exclude the first 7 bits of data to avoid the flash
x = x / 200.0 # divide by frame rate
plt.plot ( x, pp[7:203], label='perimeter')
plt.plot ( x, aa[7:203], label='area')
plt.plot ( x, dd[7:203], label='distance')
plt.plot ( x, spl(x * 200.0)/a[0], label='area(Spline)')
plt.title (fileName) 

ax.set_xlabel("time (s)")
ax.set_ylabel("change in parameter")
plt.legend()
#plt.tight_layout()
#plt.show()

#pdb.set_trace()
fig.savefig(figName)




