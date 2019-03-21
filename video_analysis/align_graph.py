import numpy as np
import pandas
import os
import pdb


#pdb.set_trace()
dircount = 0
rootdirs = os.walk('.').next()[1]
mean_mat = np.zeros((193,len(rootdirs)))
se_mat = np.zeros((193,len(rootdirs)))
N_mat = np.zeros((3,len(rootdirs)))

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

                if filepath.endswith("-0000.csv"):
                     myfileList.append (filepath)
    

        mat2=np.zeros((193,len(myfileList)))
        file_success_count = 0
        file_obscured_count = 0
        file_unresponsive_count = 0
        for filepath in myfileList:
                    print ('      ' + filepath)
                    #pdb.set_trace()
                    if ('Unr' in filepath) or ('unr' in filepath):  # eliminate unrespons and Didn't respo
                        print('            deemed unresponsive ' + filepath)  
                        file_unresponsive_count = file_unresponsive_count + 1
                        
                    
                    elif 'scu' in filepath:  # eliminate Obscured
                        print('            found to be obscured ' + filepath)  
                        file_obscured_count = file_obscured_count + 1 
                        
                        
                    else:
                        df = pandas.read_csv(filepath, header=0, 
                             names=['Summary','genotype', 'file', 'initial area', 'area spline Max', 'MaxAt', 'actual area max', 'actual area MaxAt', 'residual'])
                    
                        x = df[['area spline Max']].iloc[2:195,[0]].astype(np.float)
                    
                        #ignore traces where the max is less than 5.0
                        ss = x['area spline Max'].iloc[0] # starting value
                        mm = x.max()[0]
                        ii = mm-ss
                        if ii > 5.0 :
                            y = x['area spline Max'] - ss # subtract starting area, results in data in rows
                            mat2 [:,[file_success_count]] = y.values.reshape(193,1)
                            file_success_count = file_success_count + 1
                            usedfilelist.append(filepath)
                        else:
                            print('            found to be unresponsive ' + filepath)  
                            file_unresponsive_count = file_unresponsive_count + 1  
    
        #pdb.set_trace()

        #mat2=np.delete(mat2, slice(file_success_count,2000), 1) # 1 means delete columns
        # Create a Pandas Excel writer using XlsxWriter as the engine.
        writer = pandas.ExcelWriter(myrootdir + 'out.xlsx', engine='xlsxwriter')
        out_df = pandas.DataFrame(data = mat2[:,:file_success_count])
        out_df.columns=usedfilelist
        out_df.to_excel   (writer, sheet_name=myrootdir)
        
        if file_success_count > 0 :
            # now get the workbook etc
            workbook  = writer.book
            workbook.set_size(2000, 1400)
            worksheet = writer.sheets[myrootdir]

            chart = workbook.add_chart({'type': 'scatter'})
            # add the data...
            max_row = 193
            for i in range(file_success_count):
                col = i + 1
                chart.add_series({
                'name':       [myrootdir, 0, col],
                'categories': [myrootdir, 1, 0, max_row, 0],
                'values':     [myrootdir, 1, col, max_row, col],
                'marker':     {'type': 'none'},
                'line':       {'width': 1.25},
                })
            chart.set_x_axis({ 'name': 'frames'}) 
            chart.set_y_axis({ 'name': 'extension'})  
            chart.set_y_axis({'min': 0, 'max': 15})  
            chart.set_x_axis({'min': 0, 'max': 190})
            chart.set_size({'x_scale': 3, 'y_scale': 3})
            worksheet.insert_chart('K2', chart)

            #now write a sheet to calculate (in Excel the 75% latency)
            worksheet2 = workbook.add_worksheet('seventyFive')
            worksheet2.write_string('B1', myrootdir)
            worksheet2.write_string('A6', 'max')
            worksheet2.write_string('A7', 'max pos')
            worksheet2.write_string('A8', 'frames to 75%')
            worksheet2.write_string('A9', 'time to 75%')

            for i in range (1,file_success_count+1):
                worksheet2.write_formula(4,i, '=INDIRECT("\'"&$B$1&"\'!"&ADDRESS(1,COLUMN()))')
                worksheet2.write_formula(5,i, '=MAX(INDIRECT("\'"&$B$1&"\'!"&ADDRESS(2,COLUMN())&":"&ADDRESS(200,COLUMN())))')
                worksheet2.write_formula(6,i, '=IF(INDIRECT(ADDRESS(ROW()-1,COLUMN()))>0,MATCH(INDIRECT(ADDRESS(ROW()-1,COLUMN())),INDIRECT("\'"&$B$1&"\'!"&ADDRESS(2,COLUMN())&":"&ADDRESS(200,COLUMN())),0),"")')
                worksheet2.write_formula(7,i, '=IF(INDIRECT(ADDRESS(ROW()-2,COLUMN()))>0,MATCH(INDIRECT(ADDRESS(ROW()-2,COLUMN()))*0.75,INDIRECT("\'"&$B$1&"\'!"&ADDRESS(2,COLUMN())&":"&ADDRESS(INDIRECT(ADDRESS(ROW()-1,COLUMN())),COLUMN())),1),"")')
                worksheet2.write_formula(8,i, '=IF(INDIRECT(ADDRESS(ROW()-3,COLUMN()))>0,(INDIRECT(ADDRESS(ROW()-1,COLUMN()))+7)/200,"")')
            
        writer.save()
        
        #Now calculate the mean
        mean_mat[:,dircount] = out_df.mean(axis=1)
        se_mat  [:,dircount] = out_df.sem(axis=1)
        N_mat [0,dircount] = file_success_count        
        N_mat [1,dircount] = file_unresponsive_count
        N_mat [2,dircount] = file_obscured_count
        
        #zero the data
        #mean_mat[:,dircount] = mean_mat[:,dircount] - mean_mat[0,dircount]
        
        dircount = dircount + 1

#pdb.set_trace()
mean_df = pandas.DataFrame(data = mean_mat)
mean_df.columns=rootdirs

sd_df = pandas.DataFrame(data = se_mat)
sd_df.columns=rootdirs

N_df = pandas.DataFrame(data = N_mat)
N_df.columns=rootdirs
N_df.rename(index={0:'responded',1:'NR', 2:'Obscured'}, inplace=True)

# Create a Pandas Excel writer using XlsxWriter as the engine.
writer = pandas.ExcelWriter('mean_se.xlsx', engine='xlsxwriter')
# Position the dataframes in the worksheet.
mean_df.to_excel(writer, sheet_name='mean')  # Default position, cell A1.
sd_df.to_excel  (writer, sheet_name='se') 
N_df.to_excel   (writer, sheet_name='N')  

workbook  = writer.book
workbook.set_size(2000, 1400)
worksheet = writer.sheets['N']
nCol = len(rootdirs)
for i in range (1, nCol+1):
    worksheet.write_formula(4,i, '=SUM(INDIRECT(ADDRESS(ROW()-3,COLUMN())&":"&(ADDRESS(ROW()-1,COLUMN()))))')
    
chart = workbook.add_chart({'type': 'column', 'subtype': 'percent_stacked'})

for i in range (1, 4):
	chart.add_series({
		'name':       ['N', i, 0],
		'categories': ['N', 0, 1, 0, nCol],
		'values':     ['N', i, 1, i, nCol],
		})
    
chart.set_size({'x_scale': 2, 'y_scale': 2})    
worksheet.insert_chart('K2', chart)

#df2.to_excel(writer, sheet_name='Sheet1', startcol=3)
#df3.to_excel(writer, sheet_name='Sheet1', startrow=6)

writer.save()

