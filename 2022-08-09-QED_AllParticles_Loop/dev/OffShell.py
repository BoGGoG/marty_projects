# One can also set any of the particles OffShell
# Goal here:
# Take a list [a, b, c, ...] of any length and return all lists
# where any possible number of elements are OffShell 
from itertools import combinations
from icecream import ic
import copy

def all_offshell_combinations(process):
    """
    For a process [a, b, c] return all where any number of particles is OffShell
    Here: 
        []
    """
    cbs = [combinations(range(len(process)), x) for x in range(len(process)+1)]
    off_shell_positions = [element for sublist in cbs for element in sublist]  # flatten
    ret = [change_to_offshell(process, pos) for pos in off_shell_positions]
    return ret

def change_to_offshell(process, positions):
    ret = copy.deepcopy(process)
    for p in positions:
        ret[p] = "OffShell_" + ret[p]
    return ret

lst = list(range(3))
cbs1 = combinations(lst, 1)
cbs2 = combinations(lst, 2)
cbs3 = combinations(lst, 3)

test_process = ["e_in", "e_in", "gamma_in", "e_out", "e_out"]
ret = all_offshell_combinations(test_process)
ic(ret)
ret = all_offshell_combinations(["a", "b", "c"])
ic(ret)

