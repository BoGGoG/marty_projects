import os
from subprocess import call
from itertools import combinations_with_replacement, product, cycle, permutations
from tqdm import tqdm
from icecream import ic
from pathlib import Path
import logging
import shutil
from multiprocessing import Pool
from parallelbar import progress_imap, progress_map, progress_imapu
import numpy as np

particles_list = [
        "electron",
        "anti_electron",
        "muon",
        "anti_muon",
        "tau",
        "anti_tau",
        "up",
        "anti_up",
        "down",
        "anti_down",
        "strange",
        "anti_strange",
        "charm",
        "anti_charm",
        "top",
        "anti_top",
        "bottom",
        "anti_bottom",

        "photon"
]

def particles_format(particles_list):
    return ",".join(particles_list)


def get_possible_n_to_m_ordered(particles_list, n, m):
    """
    All thinkable n->m processes where in and out are ordered
    e.g. (in_electron, in_electron, out_electron, out_electron, out_photon) for a 2->3
    """
    in_list = ["in_"+p for p in particles_list]
    out_list = ["out_"+p for p in particles_list]

    possible_n_in = combinations_with_replacement(in_list, n)
    possible_m_out = combinations_with_replacement(out_list, m)
    possible_n_to_m = list(product(possible_n_in, possible_m_out))
    possible_n_to_m = [sum(p, ()) for p in possible_n_to_m]
    possible_n_to_m = [particles_format(p) for p in possible_n_to_m]

    return possible_n_to_m


def get_possible_n_to_m_unordered(particles_list, n, m):
    """
    All thinkable n->m processes where the whole ordering matters
    e.g. (in_electron, in_electron, out_electron, out_electron, out_photon)
         (in_electron, in_electron, out_electron, out_photon, out_electron)
         (in_electron, in_electron, out_photon, out_electron, out_electron)
         (in_electron, out_photon, in_electron, out_electron, out_electron)
         ...
    """
    in_list = ["in_"+p for p in particles_list]
    out_list = ["out_"+p for p in particles_list]

    possible_n_in = combinations_with_replacement(in_list, n)
    possible_m_out = combinations_with_replacement(out_list, m)
    possible_n_to_m = product(possible_n_in, possible_m_out)
    possible_n_to_m = [permutations(x) for x in possible_n_to_m]
    possible_n_to_m = [element for sublist in possible_n_to_m for element in sublist]   # flatten list
    possible_n_to_m = [sum(p, ()) for p in possible_n_to_m]
    possible_n_to_m = [particles_format(p) for p in possible_n_to_m]

    return possible_n_to_m


def get_possible_n_to_m(particles_list, n, m):
    """
    All thinkable n->m processes where in and out are not ordered
    e.g. (in_electron, in_electron, out_electron, out_electron, out_photon)
         (in_electron, in_electron, out_electron, out_photon, out_electron)
         (in_electron, in_electron, out_photon, out_electron, out_electron)
    """
    in_list = ["in_"+p for p in particles_list]
    out_list = ["out_"+p for p in particles_list]

    possible_n_in = product(in_list, repeat=n)
    possible_m_out = product(out_list, repeat=m)
    possible_n_to_m = list(product(possible_n_in, possible_m_out))
    possible_n_to_m = [sum(p, ()) for p in possible_n_to_m]
    possible_n_to_m = [particles_format(p) for p in possible_n_to_m]

    return possible_n_to_m

if __name__ == "__main__":
        lst = get_possible_n_to_m_unordered(particles_list, 3, 3)
        # lst = list(lst)
        ic(len(lst))
        ic(lst[0:10])
        lst2 = get_possible_n_to_m_ordered(particles_list, 3, 3)
        ic(len(lst2))
        ic(lst2[0:10])
