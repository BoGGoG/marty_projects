print("hello")

import sys
import os
from icecream import ic 
# import csv
import numpy as np
# import more_itertools
# import matplotlib.pyplot as plt
import sympy as sp
# from tqdm.notebook import tqdm
from tqdm import tqdm
from datetime import datetime
import multiprocessing as mp
import multiprocessing.queues as mpq
import functools
import dill
from typing import Tuple, Callable, Dict, Optional, Iterable
  
# current = os.path.dirname(os.path.realpath(__file__))
# parent = os.path.dirname(current)

current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from source.read_amplitudes import read_amplitudes, read_amplitudes_and_raw_squares, fix_operator_num_args, get_tree, fix_tree, fix_subscript, fix_subscripts, read_amplitudes_and_squares
import sympy as sp
from source.SympyPrefix import sympy_to_hybrid_prefix, hybrid_prefix_to_sympy

def test_example1():
    test_sqampl = "8*g**4*(2*m**4 - m**2*s)"
    test_sqampl_sp = sp.factor(sp.sympify(test_sqampl))
    test_sqampl_prefix = sympy_to_hybrid_prefix(test_sqampl_sp)
    rec = hybrid_prefix_to_sympy(test_sqampl_prefix)
    assert rec == test_sqampl_sp
    return 0

def test_example2():
    test_sqampl = "8*g**4*(2*m**4 - m**2*(s+d))"
    test_sqampl_sp = sp.factor(sp.sympify(test_sqampl))
    test_sqampl_prefix = sympy_to_hybrid_prefix(test_sqampl_sp)
    rec = hybrid_prefix_to_sympy(test_sqampl_prefix)
    assert rec == test_sqampl_sp
    return 0
