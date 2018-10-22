import os
import argparse

OUTDIR = 'results/reformed/batch2'
RESULTS_DIR = 'results/results_runtime_new'
# RESULTS_DIR = 'results/results_runtime_only'
# RESULTS_DIR = 'results/results_maximin_path'
PREFIX_LEN = 4

def collect_results(query_type):
    out = open("{}/{}.txt".format(OUTDIR, query_type), 'w')
    info = {}
    exact = {}
    tcp = {}
    path = {}
    boundary = {}
    equi = {}
    for subdir, dirs, files in os.walk(RESULTS_DIR):
        for f in files:
            if f.find(query_type) == -1:
                continue
            graph_name = f[PREFIX_LEN:f.find('_query')]
            fn = os.path.join(subdir, f)
            flag = ""
            prefix = ""
            posfix = ""
            with open(fn) as log:
                for line in log:
                    if line.find("Starting truss info") != -1:
                        info[graph_name] = []
                        flag = "info"
                        prefix = "elapsed time: "
                        posfix = "ms."
                        continue
                    if line.find("Starting truss exact") != -1:
                        exact[graph_name] = []
                        flag = "exact"
                        prefix = "elapsed time without cache "
                        posfix = "\n"
                        continue
                    if line.find("Starting tcp") != -1:
                        tcp[graph_name] = []
                        flag = "tcp"
                        prefix = "elapsed time: "
                        posfix = "ms."
                        continue
                    if line.find("Starting truss path") != -1:
                        path[graph_name] = []
                        flag = "path"
                        prefix = "elapsed time: "
                        posfix = "ms."
                        continue
                    if line.find("Starting truss boundary") != -1:
                        boundary[graph_name] = []
                        flag = "boundary"
                        prefix = "elapsed time without cache "
                        posfix = "\n"
                        continue
                    if line.find("Starting equi") != -1:
                        equi[graph_name] = []
                        flag = "equi"
                        prefix = "elapsed time: "
                        posfix = "ms."
                        continue
                    if flag == "" or line.find(prefix) == -1:
                        continue
                    remain = line.split(prefix)[1]
                    remain = remain.split(posfix)[0]
                    
                    if flag == "info":
                        info[graph_name].append(remain)
                    elif flag == "exact":
                        exact[graph_name].append(remain)
                    elif flag == "tcp":
                        tcp[graph_name].append(remain)
                    elif flag == "path":
                        path[graph_name].append(remain)
                    elif flag == "boundary":
                        boundary[graph_name].append(remain)
                    elif flag == "equi":
                        equi[graph_name].append(remain)

    graph_seq = ["facebook", "wiki", "skitter", "baidu", "Livejournal",
            "orkut", "sinaweibo", "hollywood", "bio"]

    for graph_name in graph_seq:
        if graph_name not in info:
            continue
        for item in info[graph_name]:
            out.write("{}, ".format(item))
        out.write("\n")
    out.write("\n")
    for graph_name in graph_seq:
        if graph_name not in exact:
            continue
        for item in exact[graph_name]:
            out.write("{}, ".format(item))
        out.write("\n")
    out.write("\n")
    for graph_name in graph_seq:
        if graph_name not in tcp:
            continue
        for item in tcp[graph_name]:
            out.write("{}, ".format(item))
        out.write("\n")
    out.write("\n")
    for graph_name in graph_seq:
        if graph_name not in path:
            continue
        for item in path[graph_name]:
            out.write("{}, ".format(item))
        out.write("\n")
    out.write("\n")
    for graph_name in graph_seq:
        if graph_name not in boundary:
            continue
        for item in boundary[graph_name]:
            out.write("{}, ".format(item))
        out.write("\n")
    out.write("\n")
    for graph_name in graph_seq:
        if graph_name not in equi:
            continue
        for item in equi[graph_name]:
            out.write("{}, ".format(item))
        out.write("\n")

    out.close()

if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('query_type', type=str)
    args = arg_parser.parse_args()
    collect_results(args.query_type)
