import sys
import os
from icecream import ic 
import csv
import numpy as np
import more_itertools
  
current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from source.read_amplitudes import read_amplitudes, fix_operator_num_args, get_tree, fix_tree, fix_subscript, fix_subscripts
  
amplitudes_folder = "../QED_IO/out/ampl/1to2/"
sqamplitudes_folder = "../QED_IO/out/sq_ampl/1to2/"

amplitudes_files = os.listdir(amplitudes_folder)
sqamplitudes_files = os.listdir(sqamplitudes_folder)

# ampl = []
# with open(amplitudes_folder+amplitudes_files[0]) as csv_file:
#     csv_reader = csv.reader(csv_file, delimiter=',')
#     for row in csv_reader:
#         ampl.append(row)

ampl = read_amplitudes(amplitudes_folder)
sqampl = read_amplitudes(sqamplitudes_folder)

# i = 0
# sqampl_str = ",".join(sqampl[i])
# ic(sqampl_str)
# tree = get_tree(sqampl[i])
# ic(tree)
# tree = fix_tree(tree)
# ic(tree)
#

# i = 0
# ampl_str = ",".join(ampl[i])
# ic(ampl_str)
# tree = get_tree(ampl[i])
# ic(tree)
# tree = fix_tree(tree)
# ic(tree)
#
# ic(fix_subscript(tree[0]))
# ic(fix_subscript(tree[7]))
# ic(fix_subscript(tree[9]))
# ic(fix_subscript(tree[-2]))
# ic(fix_subscript(tree[-1]))

#
for exp in ampl[0:1]:
    # print("new expression:")
    tree = get_tree(exp)
    ic(tree)
    tree = fix_tree(tree)
    ic(tree)
    final_expr = fix_subscripts(tree)
    ic(final_expr)


tree = get_tree(["Prod", "(", "a", "b", "c", ")"])
# tree = fix_tree([["Prod", "a", "b", "c", "d"]])
ic(tree)
