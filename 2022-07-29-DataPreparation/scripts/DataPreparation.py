# %%
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
  
# current = os.path.dirname(os.path.realpath(__file__))
# parent = os.path.dirname(current)

current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from source.read_amplitudes import read_amplitudes, read_amplitudes_and_raw_squares, fix_operator_num_args, get_tree, fix_tree, fix_subscript, fix_subscripts, read_amplitudes_and_squares
import sympy as sp
from source.SympyPrefix import prefix_to_sympy, sympy_to_prefix, simplify_and_prefix

  

# # %%
ampl_folders_prefix = "../QED_AllParticles_IO/out/ampl/"
# sqampl_folders_prefix = "../QED_AllParticles_IO/out/sq_ampl/"
sqampl_raw_folders_prefix = "../QED_AllParticles_IO/out/sq_ampl_raw/"
amplitudes_folders_names = ["1to2/", "2to1/", "2to2/",]# "2to3/", "3to2/", "3to3/",]
amplitudes_folders = [ampl_folders_prefix+a for a in amplitudes_folders_names]
sqamplitudes_raw_folders_names = ["1to2/", "2to1/", "2to2/",]# "2to3/", "3to2/", "3to3/",]
sqamplitudes_folders = [sqampl_raw_folders_prefix+a for a in sqamplitudes_raw_folders_names]
#
# # %%
# # # should really parallelize the simplify etc. Here is a link:
# # # https://www.delftstack.com/howto/python/parallel-for-loops-python/
# # # the code will look like this:
#
# # import multiprocessing
#
#
# # def sumall(value):
# #     return sum(range(1, value + 1))
#
# # pool_obj = multiprocessing.Pool()
#
# # answer = pool_obj.map(sumall,range(0,5))
# # print(answer)
#
# # %%
#
amplitudes = dict()
sqamplitudes = dict()
for amplitudes_folder, sqamplitudes_folder, name in zip(amplitudes_folders, sqamplitudes_folders, amplitudes_folders_names):
    ic(name)

    amplitudes_files = os.listdir(amplitudes_folder)
    sqamplitudes_files = os.listdir(sqamplitudes_folder)
    ampl, sqampl_raw = read_amplitudes_and_raw_squares(amplitudes_folder, sqamplitudes_folder)

    ampls_prefix = []
    print("Working on amplitudes")
    for exp in tqdm(ampl):
        tree = get_tree(exp)
        tree = fix_tree(tree)
        final_expr = fix_subscripts(tree)
        ampls_prefix.append(final_expr)

    sqampls_prefix = []
    print("Working on squared amplitudes")
    for exp in tqdm(sqampl_raw):
        # simplified = sp.factor(exp)   # worked best for simplification
        # prefix = sympy_to_prefix(simplified)
        # sqampls_prefix.append(prefix)
        sqampls_prefix.append(exp)
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
#
# # %%
ic(len(all_amplitudes))
ic(len(all_sqamplitudes))
#
# # %%
def get_unique_indices(l):
    seen = set([0])
    res = []
    for i, n in enumerate(l):
        if n not in seen:
            res.append(i)
            seen.add(n)
    return res
#
# # %%
all_amplitudes_str = [''.join(a) for a in all_amplitudes]
#
# # %%
unique_indices = np.unique(all_amplitudes_str, return_index=True, axis=0)[1]
unique_indices_sq = np.unique(all_sqamplitudes, return_index=True, axis=0)[1]
#
# # %%
unique_indices.sort()
unique_indices_sq.sort()
#
# # %%
ic(len(unique_indices));
ic(len(unique_indices_sq));
ic(len(unique_indices_sq) / len(unique_indices));
#
# # %%
all_amplitudes_unique = [all_amplitudes[i] for i in unique_indices]
all_sqamplitudes_unique = [all_sqamplitudes[i] for i in unique_indices]
#
# %% [markdown]
# All amplitudes are unique, but only 54% of squared amplitudes are unique.
# I will still keep all of them.

progress_file = "log/progress.log"
overwrite = True   # overwrite progress_file
batch_size = 20
batches = len(unique_indices) // batch_size + 1
ic(batches)

if overwrite:
    if os.path.exists(progress_file):
        os.remove(progress_file)

for batch in range(batches):
    batch_start_index = batch*batch_size
    batch_end_index = (batch+1)*batch_size

    start_time = datetime.now()
    # do simplificaion etc
    # write out to file etc
    end_time = datetime.now()

    with open(progress_file, 'a') as f:
        f.write("batch:" + str(batch) + "\n")
        f.write("started at:" + str(start_time) + "\n")
        f.write("finished at:" + str(end_time) + "\n")
        f.write("batch_start_index:" + str(batch_start_index) + "\n")
        f.write("batch_end_index:" + str(batch_end_index) + "\n")
        f.write("----------------------\n")

# # %%
# import multiprocessing as mp
# # pool = mp.Pool(mp.cpu_count())
# ic(mp.cpu_count())
# cpus = 16
# # all_sqamplitudes_simplified_prefix = run_imap_multiprocessing(simplify_and_prefix, all_sqamplitudes_unique, cpus)
#
# # %%
# # def run_imap_multiprocessing(func, argument_list, num_processes):
#
# #     pool = mp.Pool(processes=num_processes)
#
# #     result_list_tqdm = []
# #     for result in tqdm(pool.imap(func=func, iterable=argument_list), total=len(argument_list)):
# #         result_list_tqdm.append(result)
#
# #     return result_list_tqdm
#
# # %%
# from parallelbar import progress_imap, progress_map
#
# # %%
# from lib2to3.pgen2.literals import simple_escapes
#
#
# with mp.Pool(processes=cpus) as p:
#     all_sqamplitudes_simplified_prefix = progress_map(simplify_and_prefix, all_sqamplitudes_unique, n_cpu=cpus)  #, core_progress=True)
#
#
# # %%
# outfile_amplitudes = "../data.nosync/QED_amplitudes_TreeLevel_UpTo3to3.txt"
# outfile_sqamplitudes = "../data.nosync/QED_sqamplitudes_TreeLevel_UpTo3to3.txt"
#
# # %%
# with open(outfile_amplitudes, 'w') as f:
#     for line in all_amplitudes_unique:
#         line = ";".join(line)
#         f.write(line)
#         f.write("\n")
#
# with open(outfile_sqamplitudes, 'w') as f:
#     for line in all_sqamplitudes_simplified_prefix:
#         line = ";".join(line)
#         f.write(line)
#         f.write("\n")
#
# # %%
# X = []
# with open(outfile_amplitudes, 'r') as f:
#     for line in f.readlines() :
#         line = line.split(";")
#         # have to remove new line character for some reason
#         line[-1] = line[-1].replace("\n", "")
#         X.append(line)
#
# y = []
# with open(outfile_sqamplitudes, 'r') as f:
#     for line in f.readlines() :
#         line = line.split(";")
#         # have to remove new line character for some reason
#         line[-1] = line[-1].replace("\n", "")
#         y.append(line)
#
# # %%
# ic(X == all_amplitudes_unique);
# ic(y == all_sqamplitudes_simplified_prefix);
#
# # %%
# ampls_lens = [len(x) for x in X]
# sqampls_lens = [len(x) for x in y]
#
# # %%
# plt.hist(ampls_lens, label="Amplitudes lengths")
# plt.hist(sqampls_lens, label="Squared amplitudes lengths")
# plt.legend()
# ic(np.mean(ampls_lens))
# ic(np.mean(sqampls_lens))
#
# # %%
#
#
#
