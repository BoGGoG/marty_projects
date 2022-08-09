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
  
# current = os.path.dirname(os.path.realpath(__file__))
# parent = os.path.dirname(current)

current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from source.read_amplitudes import read_amplitudes, read_amplitudes_and_raw_squares, fix_operator_num_args, get_tree, fix_tree, fix_subscript, fix_subscripts, read_amplitudes_and_squares
import sympy as sp
from source.SympyPrefix import prefix_to_sympy, sympy_to_prefix, simplify_and_prefix, simplify_sqampl, sympy_to_hybrid_prefix

# amplitudes_file =  "../data.nosync/QED_amplitudes_TreeLevel_UpTo3to3.txt"
sqamplitudes_simplified_file =  "../data.nosync/QED_sqamplitudes_TreeLevel_UpTo3to3_simplified.txt"
sqamplitudes_simplified_prefix_file =  "../data.nosync/QED_sqamplitudes_TreeLevel_UpTo3to3_simplified_prefix.txt"
sqamplitudes_simplified_hybrid_prefix_file =  "../data.nosync/QED_sqamplitudes_TreeLevel_UpTo3to3_simplified_hybrid_prefix.txt"

sqamplitudes_prefix = []
sqamplitudes_hybrid_prefix = []
with open(sqamplitudes_simplified_prefix_file) as f:
    csv_reader = csv.reader(f, delimiter=",")
    count = 0
    for row in csv_reader:
        sqamplitudes_prefix.append(row)
        # count = count + 1
        # if count == 10000:
        #     break

with open(sqamplitudes_simplified_hybrid_prefix_file) as f:
    csv_reader = csv.reader(f, delimiter=",")
    count = 0
    for row in csv_reader:
        sqamplitudes_hybrid_prefix.append(row)
        # count = count + 1
        # if count == 10000:
        #     break

ic(len(sqamplitudes_prefix))
ic(len(sqamplitudes_hybrid_prefix))
prefix_lengths = [len(x) for x in sqamplitudes_prefix]
hybrid_prefix_lengths = [len(x) for x in sqamplitudes_hybrid_prefix]
prefix_mean = np.mean(prefix_lengths)
hybrid_prefix_mean = np.mean(hybrid_prefix_lengths)

fig, ax = plt.subplots()
plt.hist([prefix_lengths, hybrid_prefix_lengths], bins=70, label=["prefix", "hybrid prefix"])
plt.title("Number of tokens, QED up to 3->3")
textstr = '\n'.join((
    r'$\mu_\mathrm{prefix}=%.2f$' % (prefix_mean, ),
    r'$\mu_\mathrm{hybrid}=%.2f$' % (hybrid_prefix_mean, ),
    ))
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
ax.text(3000, 8000, textstr, fontsize=14,
        verticalalignment='top', bbox=props)
# plt.hist(hybrid_prefix_lengths, bins=100, label="hybrid prefix")
plt.legend()
plt.xlim(0,4000)
plt.show()

# ic(len(sqamplitudes))
#
# example = sp.sympify(sqamplitudes[60])
#
# mul_example = sp.sympify("a * b * c")
# add_example = sp.sympify("a + b + c")
# mul_add_example = sp.sympify("a + b + c * d * e * (f + g)")
#
# ic(example)
# ic(mul_example)
# print(sympy_to_prefix(example))
# print(sympy_to_prefix(mul_example))
# print(sympy_to_hybrid_prefix(mul_example))
# print(sympy_to_hybrid_prefix(add_example))
# print(sympy_to_hybrid_prefix(mul_add_example))
# print(sympy_to_hybrid_prefix(example))
#
# ic(len(sympy_to_prefix(example)))
# ic(len(sympy_to_hybrid_prefix(example)))
#
# ic(len(sympy_to_prefix(sp.sympify(sqamplitudes[99]))))
# ic(len(sympy_to_hybrid_prefix(sp.sympify(sqamplitudes[99]))))
#
# ic(len(sympy_to_prefix(sp.sympify(sqamplitudes[999]))))
# ic(len(sympy_to_hybrid_prefix(sp.sympify(sqamplitudes[999]))))
#
# ic(len(sympy_to_prefix(sp.sympify(sqamplitudes[9999]))))
# ic(len(sympy_to_hybrid_prefix(sp.sympify(sqamplitudes[9999]))))
#
# # sqamplitudes_prefix = []
# # with open(sqamplitudes_simplified_prefix_file) as f:
# #     csv_reader = csv.reader(f, delimiter=",")
# #     for row in csv_reader:
# #         sqamplitudes_prefix.append(row)
# #
# # ic(len(sqamplitudes_prefix))
# # ic(np.array(sqamplitudes_prefix[0]))
# #
# # assert len(sqamplitudes) == len(sqamplitudes_prefix)
#
#
