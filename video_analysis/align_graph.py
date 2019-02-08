import numpy as np
import pandas
import os
import pdb


#pdb.set_trace()
dircount = 0
rootdirs = os.walk('.').next()[1]
mean_mat = np.zeros((193,len(rootdirs)))
se_mat = np.zeros((193,len(rootdirs)))
N_mat = np.zeros((1,len(rootdirs)))

#for each directory found, iterate round...
for myrootdir in rootdirs:
		myfileList = []
		usedfilelist = []
		print(myrootdir)
		# make list of files
		for subdir, dirs, files in os.walk(myrootdir):
			for file in files:
				#print os.path.join(subdir, file)
				filepath = subdir + os.sep + file

				if filepath.endswith(".csv"):
					if file.startswith("fc"):
					    if 'pon' not in subdir:  # eliminate unrespons and Didn't respo
					        if 'cto' not in subdir: # eliminate obscured  
						    myfileList.append (filepath)
	

		mat2=np.zeros((193,len(myfileList)))
		filecount = 0
		for filepath in myfileList:
					print ('      ' + filepath)
					#pdb.set_trace()
					df = pandas.read_csv(filepath, header=0, 
						 names=['Summary','genotype', 'file', 'initial area', 'area spline Max', 'MaxAt', 'actual area max', 'actual area MaxAt', 'residual'])
					
					x = df[['area spline Max']].iloc[2:195,[0]].astype(np.float)
					
					#ignore traces where the max is less than 5.0
					ss = x['area spline Max'].iloc[0] # starting value
					mm = x.max()[0]
					ii = mm-ss
					if ii > 5.0 :
					    y = x['area spline Max'] - ss # subtract starting area, results in data in rows
					    mat2 [:,[filecount]] = y.values.reshape(193,1)
					    filecount = filecount + 1
					    usedfilelist.append(filepath)
					else:
					    print('            deemed unresponsive ' + filepath)    
	
		#pdb.set_trace()

		#mat2=np.delete(mat2, slice(filecount,2000), 1) # 1 means delete columns
		out_df = pandas.DataFrame(data = mat2[:,:filecount])
		out_df.columns=usedfilelist
		out_df.to_csv(myrootdir + 'out.csv')
		
		#Now calculate the mean
		mean_mat[:,dircount] = out_df.mean(axis=1)
		se_mat  [:,dircount] = out_df.sem(axis=1)
		N_mat [0,dircount] = filecount
		
		#zero the data
		#mean_mat[:,dircount] = mean_mat[:,dircount] - mean_mat[0,dircount]
		
		dircount = dircount + 1

		
mean_df = pandas.DataFrame(data = mean_mat)
mean_df.columns=rootdirs

sd_df = pandas.DataFrame(data = se_mat)
sd_df.columns=rootdirs

N_df = pandas.DataFrame(data = N_mat)
N_df.columns=rootdirs

# Create a Pandas Excel writer using XlsxWriter as the engine.
writer = pandas.ExcelWriter('mean_se.xlsx', engine='xlsxwriter')

# Position the dataframes in the worksheet.
mean_df.to_excel(writer, sheet_name='mean')  # Default position, cell A1.
sd_df.to_excel  (writer, sheet_name='se') 
N_df.to_excel   (writer, sheet_name='N')  

#df2.to_excel(writer, sheet_name='Sheet1', startcol=3)
#df3.to_excel(writer, sheet_name='Sheet1', startrow=6)

writer.save()

