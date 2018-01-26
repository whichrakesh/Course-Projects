#! /usr/bin/python3 
import subprocess
subprocess.call(['mkdir','-p','./plots'])
import matplotlib.pyplot as pyplot
import numpy 
import random
import pylab
import re

itr_num=1200
rerun=150

infile=open("data/g26_lab05data_02.csv",'r')

readdata=[[],[],[],[],[],[],[]]

for line in infile:
	inp=line.split(',')
	#print(float(inp[6]))
	#if int(inp[0]) == 501:
	#	break;
	readdata[0].append(int(inp[0]))
	readdata[1].append(int(inp[1]))
	readdata[2].append(float(inp[2]))
	readdata[3].append(float(inp[3]))
	readdata[4].append(float(inp[4]))
	readdata[5].append(float(inp[5]))
	readdata[6].append(float(inp[6]))

#print(readdata)

itrlist=[]
avgstep=[]
avgcoll=[]
avgvel=[]
avgpos=[]
avgloop=[]
avgsum=[]
stdev=[]
#~ plotdata[0]  itrlist=[]
#~ plotdata[1]  avgstep=[]
#~ plotdata[2]  avgcoll=[]
#~ plotdata[3]  avgvel=[]
#~ plotdata[4]  avgpos=[]
#~ plotdata[5]  avgloop=[]
#~ plotdata[6]  avgsum=[]


for i in range(0,itr_num):
	itrlist.append(i+1)
	
	templist=readdata[2][i*rerun:(i+1)*rerun]
	avgstep.append(numpy.mean(templist))
	
	templist=readdata[3][i*rerun:(i+1)*rerun]
	avgcoll.append(numpy.mean(templist))
	
	templist=readdata[4][i*rerun:(i+1)*rerun]
	avgvel.append(numpy.mean(templist))
	
	templist=readdata[5][i*rerun:(i+1)*rerun]
	avgpos.append(numpy.mean(templist))
	
	templist=readdata[6][i*rerun:(i+1)*rerun]
	avgloop.append(numpy.mean(templist))
		
	avgsum.append(avgcoll[i]+avgvel[i]+avgpos[i])
	templist=readdata[2][i*rerun:(i+1)*rerun]
	stdev.append(numpy.std(templist))


#print(itrlist)
#print(avgstep)

fig = pyplot.figure()
p1 = fig.add_subplot(111)
p1.set_title('Step time and Loop time vs Number of iterations')    
p1.set_xlabel('Number of Iterations')
p1.set_ylabel('Averaged time over iterations (ms)')
p1.bar(itrlist, avgstep,facecolor='r',edgecolor='black',label='Avg Step Time')
p1.plot(itrlist,avgloop,label='Avg Loop Time')
p1.legend()
fig.savefig('./plots/g26_plot00.png')


fig = pyplot.figure()
p2 = fig.add_subplot(111)
p2.set_title('various times vs Number of iterations')    
p2.set_xlabel('Number of Iterations')
p2.set_ylabel('Averaged time over iterations (ms)')
p2.plot(itrlist, avgstep,color='m',label='Average Step ')
p2.plot(itrlist, avgcoll,color='c',label='Average Collision ')
p2.plot(itrlist, avgvel,color='g',label='Average Vel time ')
p2.plot(itrlist, avgpos,color='b',label='Average Pos time ')
p2.plot(itrlist, avgsum,color='r',label='Sum of all times')
p2.legend()
fig.savefig('./plots/g26_plot01.png')

fig = pyplot.figure()
p3 = fig.add_subplot(111)
p3.set_title('Step Time vs Number of iterations')    
p3.set_xlabel('Number of Iterations')
p3.set_ylabel('Averaged time over iterations (ms)')
p3.errorbar(itrlist, avgstep,stdev,color='r',ecolor='c',label='error lines')
p3.plot(itrlist, avgstep,color='r',label='Average Step Time')
p3.legend()
fig.savefig('./plots/g26_plot02.png')


numbins=100
plotdata_76=readdata[2][75*rerun:76*rerun]
freq1,freq2=numpy.histogram(numpy.array(plotdata_76),numbins)
cum=numpy.cumsum(freq1) 
fig = pyplot.figure()
p4 = fig.add_subplot(111)
p4.set_title('Step Time Frequency Plot')    
p4.set_xlabel('Step Time (ms)')
p4.set_ylabel('Frequency')
p4.hist(plotdata_76,numbins,color='g',label='Step Time Frequency')
p4.plot(freq2[1:],cum,color='r',label='Cumulative Step Time Frequency')
p4.legend(loc=2)
pyplot.show()
fig.savefig('./plots/g26_plot03.png')		
 
 
randlist=[]

for i in range(0,itr_num):
	templist=random.sample(readdata[2][i*rerun:(i+1)*rerun],15)
	randlist.append(numpy.mean(templist))

mrand,crand=numpy.polyfit(range(1,itr_num+1),randlist,1)
m,c=numpy.polyfit(range(1,itr_num+1),avgstep,1)
randl=[mrand+crand,mrand*itr_num+crand]
norml=[m+c,m*itr_num+c]
#print(m,c,mrand,crand)

x=range(10)
y=range(10)
fig = pyplot.figure()
p5 = fig.add_subplot(111)
p5.set_title('Random Step time vs Number of iterations')    
p5.set_xlabel('Number of Iterations')
p5.set_ylabel('Averaged time (ms)')
p5.scatter(itrlist,avgstep,color='y',label='Avg step for all reruns',marker="^")
p5.scatter(itrlist,randlist,color='c',label='Avg Step for random reruns')
p5.plot([1,itr_num],norml,color='r',label='best fit for all reruns')
p5.plot([1,itr_num],randl,color='g',label='best fit for random reruns')
#p5.plot(x,y,"yo",x,m*x+c,"--k")
p5.legend(loc=2)
pyplot.show()
fig.savefig('./plots/g26_plot04.png')			

infile.close()
