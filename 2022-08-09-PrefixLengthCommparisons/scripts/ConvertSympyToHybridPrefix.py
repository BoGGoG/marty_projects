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

sqamplitudes = []
with open(sqamplitudes_simplified_file) as f:
    csv_reader = csv.reader(f, delimiter=",")
    count = 0
    for row in csv_reader:
        sqamplitudes.append(row[0])
        count = count + 1 
        if count == 100:
            break

ic(len(sqamplitudes))

example = sp.sympify(sqamplitudes[60])

mul_example = sp.sympify("a * b * c")
add_example = sp.sympify("a + b + c")
mul_add_example = sp.sympify("a + b + c * d * e * (f + g)")

ic(example)
ic(mul_example)
print(sympy_to_prefix(example))
print(sympy_to_prefix(mul_example))
print(sympy_to_hybrid_prefix(mul_example))
print(sympy_to_hybrid_prefix(add_example))
print(sympy_to_hybrid_prefix(mul_add_example))

# sqamplitudes_prefix = []
# with open(sqamplitudes_simplified_prefix_file) as f:
#     csv_reader = csv.reader(f, delimiter=",")
#     for row in csv_reader:
#         sqamplitudes_prefix.append(row)
#
# ic(len(sqamplitudes_prefix))
# ic(np.array(sqamplitudes_prefix[0]))
#
# assert len(sqamplitudes) == len(sqamplitudes_prefix)


