# KTruss

Two layer index for truss community queries on large-scale graphs.

See more details in our paper: [Two-level Index for Truss Community Query in Large-Scale Graphs]()

## Overview:
We propose a two-level index structure to solve local community query problem based on K-truss commuity model proposed by Huang Xin in their 2014 Sigmod Paper. The index contains 2 level, the top-level index preserves the community-level information, the bottom-level index conatains edge-level information. Here is an overview of our index strucutre:

<img src="/example/example.png" width="500px"/>  

## Triangle-derived MST (bottom-level index):
The bottom-level index is a maximum spanning forest of a triangle-derived graph that preserves the edge level trussness and triangle connectivity
inside the k-truss communities.

<img src="/example/system_overview.png" width="700px"/> 

## Community Graph (top-level index):
The top-level index is a super-graph whose vertices represent unique k-truss communities and edges represent containment
relations between k-truss communities.

<img src="/example/distance_lb.png" width="500px"/> 

## Speed estimation error
We develop a two-level index structure that can efficiently process both the community-level and the edge-level ktruss community query for a single query vertex or a set of query vertices with any given cohesiveness criteria, using an efficient two step process. Here we show the effectiveness of our method on both single-vertex edge-level community query (community search) and multiple-vertex community-level community query.

<img src="/example/error_cdf.png" width="450px"/>

## How to run the code
The configuration script and run script locate in KTruss/utils/
