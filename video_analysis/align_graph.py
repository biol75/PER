import numpy as np
import pandas
import os
import pdb


mat2=np.zeros((193,2000))
filecount = 0
myfileList = []

rootdir = '../../../3Dec/DicerGr5aOPRR10tripTHG'


for subdir, dirs, files in os.walk(rootdir):
    for file in files:
        #print os.path.join(subdir, file)
        filepath = subdir + os.sep + file

        if filepath.endswith(".csv"):
            print (filepath)
            #pdb.set_trace()
            df = pandas.read_csv(filepath, header=0, 
                 names=['Summary','genotype', 'file', 'initial area', 'area spline Max', 'MaxAt', 'actual area max', 'actual area MaxAt', 'residual'])
            df1 = df[['actual area max']]
            
            x = df[['actual area max']].iloc[2:195,[0]]
            mat2 [:,[filecount]] = x
            
            
            myfileList.append (filepath)
            filecount = filecount + 1
            


out_df = pandas.DataFrame(data = mat2)
out_df.to_csv('out.csv')
