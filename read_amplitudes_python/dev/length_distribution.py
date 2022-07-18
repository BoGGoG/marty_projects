import sys
import os
from icecream import ic 
import csv
import numpy as np
import more_itertools
  
current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from source.read_amplitudes import read_amplitudes, fix_operator_num_args, get_tree, fix_tree, fix_subscript, fix_subscripts, read_amplitudes_and_squares
  
amplitudes_folder = "../QED_AllParticles_IO/out/ampl/2to3/"
sqamplitudes_folder = "../QED_AllParticles_IO/out/sq_ampl/2to3/"

amplitudes_files = os.listdir(amplitudes_folder)
sqamplitudes_files = os.listdir(sqamplitudes_folder)
# ic(len(amplitudes_files))
# ic(len(sqamplitudes_files))
ampl, sqampl = read_amplitudes_and_squares(amplitudes_folder, sqamplitudes_folder)


ampls_prefix = []
for exp in ampl:
    tree = get_tree(exp)
    tree = fix_tree(tree)
    final_expr = fix_subscripts(tree)
    ampls_prefix.append(final_expr)

sqampls_prefix = []
for exp in sqampl:
    tree = get_tree(exp)
    tree = fix_tree(tree)
    final_expr = fix_subscripts(tree)
    sqampls_prefix.append(final_expr)

ampls_lengths = [len(a) for a in ampls_prefix]
sqampls_lengths = [len(a) for a in sqampls_prefix]

ic(np.mean(ampls_lengths))
ic(np.mean(sqampls_lengths))

