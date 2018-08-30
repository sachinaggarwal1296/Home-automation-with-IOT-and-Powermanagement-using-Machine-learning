# -*- coding: utf-8 -*-
"""
Created on Mon Oct  9 09:18:40 2017

@author: SACHIN
"""

from statistics import mean
import numpy as np
import matplotlib.pyplot as plt
import math
import pandas as pd



df = pd.read_csv('Book1.data.txt')
df.drop(['d_id'],1,inplace=True)
df.convert_objects(convert_numeric=True)
df.fillna(0,inplace=True)



x1=np.array(df['date'], dtype = np.float64)
y1=np.array(df['ENERGY'], dtype = np.float64)
z1=np.array(df['POWER'], dtype = np.float64)
m=np.array(df['date1'],dtype = np.datetime64)
def bestFitLine(x,y):
    m = ((mean(x)*mean(y)) - mean(x*y))/((mean(x)*mean(x))-mean(x*x))

    b = mean(y) - m*mean(x) 
    return m,b





def sqrd_err(y_orgin,y_line):
    return sum((y_line-y_orgin)**2)



def cof_of_det(y_orgin,y_line):

    sqrd_err_best_line = sqrd_err(y_orgin,y_line)
    #print(sqrd_err_best_line)
    
    y_mean_line = [mean(y_orgin) for i in y_orgin]
    sqrd_err_y_mean = sqrd_err(y_orgin,y_mean_line)    

    return 1 - (sqrd_err_best_line/sqrd_err_y_mean)





def predict_energy(x1,y1):
    plt.figure(1)
    p =  20171012
    q =  20171027
    r =  1
    list1=[]
    list2=[]
    daate=2017-10-12
    m,b = bestFitLine(x1,y1)
    best_line = [(m*e)+b for e in x1]

    c1=cof_of_det(y1,best_line)
    #plt.plot(x1,best_line)
    date=np.array([],dtype = np.datetime64)
    for fx in range(p,q,r):
        fy=(m*fx+b)
        list1.append(daate)
        
        list2.append(fy)
        plt.scatter(fx,fy)
        print(fx,fy)
    print()    
    print(100-(c1*100))
    plt.show()

def predict_power(x1,z1):
    plt.figure(2)
    p =  20171012
    q =  20171027
    r =  1

    m,b = bestFitLine(x1,z1)
    best_line = [(m*e)+b for e in x1]

    c1=cof_of_det(z1,best_line)
    #plt.plot(x1,best_line)

    for fx in range(p,q,r):
        fz=(m*fx+b)
        plt.scatter(fx,fz)
        print(fx,fz)

    print(100-(c1*100))
    plt.show()

plt.show()
predict_energy(x1,y1)
predict_power(x1,z1)
#plt.scatter(x1,y1)

plt.figure(3)
plt.scatter(m,y1)


#plt.plot(x1,best_line)






#plt.plot(x1,y1)





