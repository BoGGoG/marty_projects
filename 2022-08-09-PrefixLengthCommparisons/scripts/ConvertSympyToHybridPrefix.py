import sys
import os
from icecream import ic 
import csv
import numpy as np
import more_itertools
import matplotlib.pyplot as plt
import sympy as sp
# from tqdm.notebook import tqdm
from tqdm import tqdm
from datetime import datetime
import multiprocessing as mp
import multiprocessing.queues as mpq
import functools
import dill
from typing import Tuple, Callable, Dict, Optional, Iterable, List
from parallelbar import progress_imap, progress_map
  
# current = os.path.dirname(os.path.realpath(__file__))
# parent = os.path.dirname(current)

current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from source.read_amplitudes import read_amplitudes, read_amplitudes_and_raw_squares, fix_operator_num_args, get_tree, fix_tree, fix_subscript, fix_subscripts, read_amplitudes_and_squares
import sympy as sp
from source.SympyPrefix import prefix_to_sympy, sympy_to_prefix, simplify_and_prefix, simplify_sqampl, sympy_to_hybrid_prefix, sympify_and_hybrid_prefix

# amplitudes_file =  "../data.nosync/QED_amplitudes_TreeLevel_UpTo3to3.txt"
sqamplitudes_simplified_file =  "../data.nosync/QED_sqamplitudes_TreeLevel_UpTo3to3_simplified.txt"
sqamplitudes_simplified_prefix_file =  "../data.nosync/QED_sqamplitudes_TreeLevel_UpTo3to3_simplified_prefix.txt"
outfile_sqamplitudes_simplified_hybrid_prefix =  "../data.nosync/QED_sqamplitudes_TreeLevel_UpTo3to3_simplified_hybrid_prefix.txt"

sqamplitudes = []
with open(sqamplitudes_simplified_file) as f:
    csv_reader = csv.reader(f, delimiter=",")
    count = 0
    for row in csv_reader:
        sqamplitudes.append(row[0])
        # count = count + 1
        # if count == 100:
        #     break


if __name__ == '__main__':
    # freeze_support()
    cpus = 19
    with mp.Pool(processes=cpus) as p:
        # sqamplitudes_simplified_prefix_batch = p.map(simplify_and_prefix, sqamplitudes_batch)
        sqamplitudes_hybrid_prefix = progress_map(sympify_and_hybrid_prefix, sqamplitudes, n_cpu=cpus)  #, core_progress=True)
    out_sqamplitudes_hybrid_prefix_str = [",".join(x) for x in sqamplitudes_hybrid_prefix]
    out_sqamplitudes_hybrid_prefix_str = "\n".join(out_sqamplitudes_hybrid_prefix_str)+"\n"

    with open(outfile_sqamplitudes_simplified_hybrid_prefix, "w") as f:
        f.write(out_sqamplitudes_hybrid_prefix_str)
    
    # sqamplitudes_prefix = [sympy_to_prefix(sp.sympify(e)) for e in sqamplitudes]
    # sqamplitudes_hybrid_prefix = [sympy_to_hybrid_prefix(sp.sympify(e)) for e in sqamplitudes]

    # sqamplitudes_lengths = [len(e) for e in sqamplitudes_prefix]
    # sqamplitudes_hybrid_lengths = [len(e) for e in sqamplitudes_hybrid_prefix]

    # plt.hist(sqamplitudes_lengths)
    # plt.title("Squared amplitudes (prefix) token lengths")
    # plt.show()
    #
    # plt.hist(sqamplitudes_hybrid_lengths)
    # plt.title("Squared amplitudes (hybrid prefix) token lengths")
    # plt.show()
