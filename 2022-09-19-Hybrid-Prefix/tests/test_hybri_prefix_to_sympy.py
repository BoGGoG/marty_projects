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

def test_example3():
    test_sqampl = "8*g**4*(2*m**4 - m**2*(s+d)**2)"
    test_sqampl_sp = sp.factor(sp.sympify(test_sqampl))
    test_sqampl_prefix = sympy_to_hybrid_prefix(test_sqampl_sp)
    rec = hybrid_prefix_to_sympy(test_sqampl_prefix)
    assert rec == test_sqampl_sp
    return 0

def test_qcd():
    ampl_folders_prefix = "../2022-09-12-QCD_AllParticles_Loop/out/ampl/"
    sqampl_raw_folders_prefix = "../2022-09-12-QCD_AllParticles_Loop/out/sq_ampl_raw/"
    # amplitudes_folders_names = ["1to2/", "2to1/", "2to2/", "2to3/", ]# "3to2/", "3to3/",]
    amplitudes_folders_names = ["2to2/"]
    amplitudes_folders = [ampl_folders_prefix+a for a in amplitudes_folders_names]
    # sqamplitudes_raw_folders_names = ["1to2/", "2to1/", "2to2/", "2to3/", ]# "3to2/", "3to3/",]
    sqamplitudes_raw_folders_names = ["2to2/"]
    sqamplitudes_folders = [sqampl_raw_folders_prefix+a for a in sqamplitudes_raw_folders_names]

    amplitudes = dict()
    sqamplitudes = dict()
    read_ampls = 10
    for amplitudes_folder, sqamplitudes_folder, name in zip(amplitudes_folders, sqamplitudes_folders, amplitudes_folders_names):
        amplitudes_files = os.listdir(amplitudes_folder)
        sqamplitudes_files = os.listdir(sqamplitudes_folder)
        ampl, sqampl_raw = read_amplitudes_and_raw_squares(amplitudes_folder, sqamplitudes_folder)

        ampls_prefix = []
        print("Loading amplitudes")
        ctr = 0
        for exp in tqdm(ampl):
            tree = get_tree(exp)
            tree = fix_tree(tree)
            final_expr = fix_subscripts(tree)
            ampls_prefix.append(final_expr)
            ctr = ctr + 1
            if ctr >= read_ampls:
                break

        sqampls_prefix = []
        print("Loading squared amplitudes")
        ctr = 0
        for exp in tqdm(sqampl_raw):
            sqampls_prefix.append(exp)
            ctr = ctr + 1
            if ctr >= read_ampls:
                break
        amplitudes[name] = ampls_prefix
        sqamplitudes[name] = sqampls_prefix


    # # %%
    all_amplitudes = []
    for key in amplitudes.keys():
        for x in amplitudes[key]:
            all_amplitudes.append(x)

    all_sqamplitudes = []
    for key in sqamplitudes.keys():
        for x in sqamplitudes[key]:
            all_sqamplitudes.append(x)

    for test_sqampl in all_sqamplitudes:
        test_sqampl_sp = sp.factor(sp.sympify(test_sqampl))
        test_sqampl_prefix = sympy_to_hybrid_prefix(test_sqampl_sp)
        rec = hybrid_prefix_to_sympy(test_sqampl_prefix)
        assert rec == test_sqampl_sp
