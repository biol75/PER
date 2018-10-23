

import pandas
import sys
# use splines to fit and interpolate data
from scipy.interpolate import interp1d
from scipy.interpolate import UnivariateSpline
from scipy.optimize import fmin
import numpy as np
import matplotlib.pyplot as plt
import pdb


if len(sys.argv) != 2 :
	print ('Usage: filename ');
	sys.exit(1)
	
	
fileName = sys.argv[1]
df = pandas.read_csv(fileName)
print(df)

#pdb.set_trace()

df.set_index("index");

#remove rows when flash is on...
df = df.drop([0,1,2,3,4,5,6], axis=0)
x=np.array(df.index)
y=np.array(df.perimeter)

# create the interpolating function
#f = interp1d(x, y, kind='cubic', bounds_error=False)
sp = UnivariateSpline(x, y, s=50000)
d1s = sp.derivative()

# pdb.set_trace()	
# print('Roots = {}'.format(sp.derivative().roots()))
# minmax = sp.derivative().roots()

# to find the maximum, we minimize the negative of the function. We
# cannot just multiply f by -1, so we create a new function here.
# f2 = interp1d(x, -y, kind='cubic')
# xmax = fmin(f2, 150)
sp_invert = UnivariateSpline(x, -y, s=50000)
xmax = fmin(sp_invert, 150)

xfit = np.linspace(7,198)

plt.plot(x,y,'bo')
plt.plot(xfit, sp(xfit),'r-')
# plt.plot(xmax, f(xmax),'g*')
# plt.legend(['data','fit','max'], loc='best', numpoints=1)
plt.plot(xmax, sp(xmax), 'ro ')
plt.xlabel('x data')
plt.ylabel('y data')
plt.title('Max point = ({0:1.2f}, {1:1.2f})'.format(float(xmax), float(sp(xmax))))
plt.show()                                                    
                                                    
#pdb.set_trace()                                                                                                      
figName = fileName.replace ('.csv', '.png')
figName = figName.rsplit('/', 1)[-1]
plt.savefig('splinefit_' + figName)