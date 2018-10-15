import os
import argparse

OUTDIR = 'results'
RESULTS_DIR = 'results/results_runtime_only'
PREFIX_LEN = 4

def collect_results(query_type):
    out = open("{}/{}.txt".format(OUTDIR, query_type), 'w')
    info = {}
    exact = {}
    tcp = {}
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

    graph_seq = ["facebook", "wiki", "skitter", "baidu", "livejournal",
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

    out.close()

if __name__ == '__main__':
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('query_type', type=str)
    args = arg_parser.parse_args()
    collect_results(args.query_type)