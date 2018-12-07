import numpy as np
import pandas
import os
import pdb


#pdb.set_trace()
for rootsubdir, rootdirs, rootfiles in os.walk('.') :
		for myrootdir in rootdirs:
		#for each directory found, iterate round...

				myfileList = []
				# make list of files
				for subdir, dirs, files in os.walk(myrootdir):
					for file in files:
						#print os.path.join(subdir, file)
						filepath = subdir + os.sep + file
		
						if filepath.endswith(".csv"):
							if file.startswith("fc"):
								myfileList.append (filepath)
			

				mat2=np.zeros((193,len(myfileList)))
				filecount = 0
				for filepath in myfileList:
							print (filepath)
							#pdb.set_trace()
							df = pandas.read_csv(filepath, header=0, 
								 names=['Summary','genotype', 'file', 'initial area', 'area spline Max', 'MaxAt', 'actual area max', 'actual area MaxAt', 'residual'])
							df1 = df[['actual area max']]
			
							x = df[['actual area max']].iloc[2:195,[0]]
			
							mat2 [:,[filecount]] = x
								  
							filecount = filecount + 1
			
				#pdb.set_trace()

				#mat2=np.delete(mat2, slice(filecount,2000), 1) # 1 means delete columns
				out_df = pandas.DataFrame(data = mat2)
				out_df.columns=myfileList
				out_df.to_csv(myrootdir + 'out.csv')
