#!/usr/bin/env python2.7
import sys
import os
import math

# Link parameters
link_latency = "20us"
link_bandwidth = 10
link_bandwidth_unit = "Gbps"


# Convenient math wrappers
def floor(x):
	return int(math.floor(x))
def ceil(x):
	return int(math.ceil(x))
def pow2(x):
	return int(math.pow(2,x))

# XML generation functions
def issueHead():
	return "<?xml version='1.0'?>\n<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid.dtd\">\n<platform version=\"3\">\n<AS id=\"AS0\" routing=\"Full\">\n"

def issueTail():
	return "</AS>\n</platform>\n"

def issueLink1(x):
	return "  <link id=\"link-"+str(x)+"\" latency=\""+str(link_latency)+"\" bandwidth=\""+str(link_bandwidth)+link_bandwidth_unit+"\"/>\n"

def issueLink2(x,y):
	return "  <link id=\"link-"+str(x)+"-"+str(y)+"\" latency=\""+str(link_latency)+"\" bandwidth=\""+str(link_bandwidth)+link_bandwidth_unit+"\"/>\n"

def issueLink3(x,y,bw):
	return "  <link id=\"link-"+str(x)+"-"+str(y)+"\" latency=\""+str(link_latency)+"\" bandwidth=\""+str(bw)+link_bandwidth_unit+"\"/>\n"

def issueHost(index):
	return "  <host id=\"host-"+str(index)+".hawaii.edu\" power=\"1\"/>\n"

def issueRouteHead(index1, index2):
	return "  <route src=\"host-"+str(index1)+".hawaii.edu\" dst=\"host-"+str(index2)+".hawaii.edu\">\n"
def issueRouteTail():
	return "  </route>\n"

def issueRouteLink1(x):
	return "\t<link_ctn id=\"link-"+str(x)+"\"/>\n"

def issueRouteLink2(x,y):
	return "\t<link_ctn id=\"link-"+str(x)+"-"+str(y)+"\"/>\n"

######################################################################
# Parse command-line arguments
if (len(sys.argv) != 2):
	print >> sys.stderr, "Usage:a"+sys.argv[0]+" <num hosts>\n"
	exit(1)

num_hosts = int(sys.argv[1])

###############################################################
# Generate RING XML file
filename = "./ring_"+str(num_hosts)+".xml"
fh = open(filename, 'w')
fh.write(issueHead())

# Create hosts
for i in range(0,num_hosts):
	fh.write(issueHost(i))

# Create links
for i in range(0,num_hosts):
	fh.write(issueLink1(i))

# Create routes
for i in range (0,num_hosts):
	for j in range(i+1,num_hosts):
		fh.write(issueRouteHead(i,j))
		for k in range(i,(j+num_hosts)%num_hosts):
  			fh.write(issueRouteLink1(k))
    		fh.write(issueRouteTail())

fh.write(issueTail())
fh.close()
print >> sys.stderr, "Ring XML platform description file created: "+filename


###############################################################
# Generate Cluster Crossbar XML file
filename = "./cluster_"+str(num_hosts)+".xml"
fh = open(filename, 'w')
fh.write(issueHead())

fh.write("<cluster id=\"cluster\" prefix=\"host-\" suffix=\".hawaii.edu\" radical=\"0-"+str(num_hosts-1)+"\" power=\"1\" bw=\""+str(link_bandwidth)+link_bandwidth_unit+"\" lat=\""+str(link_latency)+"\"/>\n")
fh.write(issueTail())
fh.close()
print >> sys.stderr, "Cluster XML platform description file created: "+filename

###############################################################
# Generate Binary Tree XML file

filename = "./bintree_"+str(num_hosts)+".xml"
fh = open(filename, 'w')
fh.write(issueHead())

# Create all hosts and links
for i in range(0,num_hosts):
	fh.write(issueHost(i))
	if (i*2+1 < num_hosts):
  		fh.write(issueLink2(i,i*2+1))
	if (i*2+2 < num_hosts):
  		fh.write(issueLink2(i,i*2+2))

# Create all routes
for i in range(0,num_hosts):
	level_i = floor(math.log(1+i,2))
	for j in range(i+1,num_hosts):
		fh.write(issueRouteHead(j,i))
		# Host j is at the same of lower level than host i
		level_j = floor(math.log(1+j,2))
		current_host_path_j = j
		# Go up to the same level of that of host i
		for l in range(level_j,level_i,-1):
			parent_host = floor(float(current_host_path_j-1)/2)
			fh.write(issueRouteLink2(min(current_host_path_j,parent_host),max(current_host_path_j,parent_host)))
			current_host_path_j = parent_host
		# Find the common ancestor
		current_host_path_i = i
		while (current_host_path_j != current_host_path_i):
			fh.write(issueRouteLink2(min(current_host_path_j,floor(float(current_host_path_j-1)/2)), max(current_host_path_j,floor(float(current_host_path_j-1)/2))))
			current_host_path_i = floor(float(current_host_path_i-1)/2)
			current_host_path_j = floor(float(current_host_path_j-1)/2)
		common_ancestor = current_host_path_j
		# Go back from i to the common ancestor
		current_host_path_i = i
		sequence = []
		sequence.append(current_host_path_i)
		while (current_host_path_i != common_ancestor):
			parent_host = floor(float(current_host_path_i-1)/2)
			sequence.append(parent_host)
			current_host_path_i = parent_host
		# Issue links in the common ancestor -> i order
		sequence = sequence[::-1]
		for k in range(0,len(sequence)-1):
			fh.write(issueRouteLink2(min(sequence[k],sequence[k+1]),max(sequence[k],sequence[k+1])))
		fh.write(issueRouteTail())

fh.write(issueTail())
fh.close()
print >> sys.stderr, "BinTree XML platform description file created: "+filename

################################################################
## Generate Fat Tree XML file

filename = "./fattree_"+str(num_hosts)+".xml"
fh = open(filename, 'w')
fh.write(issueHead())

# Create clusters
num_hosts_per_cluster = 8
num_clusters = ceil(float(num_hosts)/float(num_hosts_per_cluster))
for i in range(0,num_clusters):
	fh.write("<cluster id=\"cluster_"+str(i)+"\" prefix=\"host-\" suffix=\".hawaii.edu\" radical=\""+str(i*num_hosts_per_cluster)+"-"+str(min(num_hosts-1,i*num_hosts_per_cluster+num_hosts_per_cluster-1))+"\" power=\"1\" bw=\""+str(link_bandwidth)+link_bandwidth_unit+"\" lat=\""+link_latency+"\"/>\n")

# Create all links
num_levels = ceil(math.log(num_clusters,2))
for l in range(0,num_levels):
	for i in range(0,pow2(num_levels-l)):
		fh.write(issueLink3(l,i,pow2(l+1)*link_bandwidth))

for i in range(0,num_clusters):
	for j in range(i+1, num_clusters):
		# Create route from cluster i to cluster j
		fh.write("  <ASroute src=\"cluster_"+str(i)+"\" dst=\"cluster_"+str(j)+"\" gw_src=\"host-cluster_"+str(i)+"_router.hawaii.edu\" gw_dst=\"host-cluster_"+str(j)+"_router.hawaii.edu\" symmetrical=\"YES\">\n")
		# Compute number of levels to go up
		num_levels = 1
		while (True):
			if (floor(float(i)/pow2(num_levels)) != floor(float(j)/pow2(num_levels))):
				num_levels = num_levels + 1
			else:
				break
		# Go up
		for l in range(0,num_levels):
			fh.write(issueRouteLink2(l,floor(float(i)/pow2(l))))
		# Go back down
		for l in range(num_levels-1,-1,-1):
			fh.write(issueRouteLink2(l,floor(float(j)/pow2(l))))
		fh.write("  </ASroute>\n")

fh.write(issueTail())
fh.close()
print >> sys.stderr, "FatTree XML platform description file created: "+filename

###############################################################
## Generate host file
filename = "./hostfile_"+str(num_hosts)
fh = open(filename, 'w')

for i in range(0,num_hosts):
	fh.write("host-"+str(i)+".hawaii.edu\n")

fh.close()
print >> sys.stderr, "Hostfile created: "+filename

