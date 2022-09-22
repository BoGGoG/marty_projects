import os
from subprocess import call, run, DEVNULL
from itertools import combinations_with_replacement, product, permutations, combinations
from tqdm import tqdm
from icecream import ic
from pathlib import Path
import logging
import shutil
from multiprocessing import Pool
from itertools import cycle
from parallelbar import progress_imap, progress_map, progress_imapu
from shlex import split
import copy
import os
from os.path import exists
os.chdir("/home/boggog/Documents/PhD-ML/marty_projects/2022-09-12-QCD_AllParticles_Loop")

INCLUDE_OFF_SHELL = True

particles_list = [
        # "electron",
        # "anti_electron",
        # "muon",
        # "anti_muon",
        # "tau",
        # "anti_tau",
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
        # "photon"
]

def calc_amplitude(particles, 
        ampl_file="out_test/ampl.txt",
        ampl_raw_file="out_test/ampl_raw.txt",
        sqampl_file="out_test/sq_ampl.txt",
        sqampl_raw_file="out_test/sq_ampl_raw.txt",
        insertions_file="out_test/insertions.txt",
        diagrams_file="out_test/diagrams.txt",
        log_file=False
        ):

    options = "--particles=" + particles + " -e" + " -a " + ampl_file + " -s " + sqampl_file + " -i " + insertions_file + " -r " + sqampl_raw_file + " -t " + ampl_raw_file + " -b " + diagrams_file
    if log_file:
        options = options + " > " + log_file

    call_arg = "./QCD_AllParticles_IO.x " + options 
    call_arg = split(call_arg)
    _ = run(call_arg, shell=False, stdout=DEVNULL) #, stderr=DEVNULL)
    

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


def get_possible_n_to_m_all_orderings(particles_list, n, m):
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
    # possible_n_to_m = [particles_format(p) for p in possible_n_to_m]

    return possible_n_to_m


def get_possible_n_to_m(particles_list, n, m):
    """
    All thinkable n->m processes where in and out are not ordered (in and out separately)
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


def all_offshell_combinations(process):
    """
    For a process [a, b, c] return all where any number of particles is OffShell
    Here: 
         [['a', 'b', 'c'],
          ['OffShell_a', 'b', 'c'],
          ['a', 'OffShell_b', 'c'],
          ['a', 'b', 'OffShell_c'],
          ['OffShell_a', 'OffShell_b', 'c'],
          ['OffShell_a', 'b', 'OffShell_c'],
          ['a', 'OffShell_b', 'OffShell_c'],
          ['OffShell_a', 'OffShell_b', 'OffShell_c']]
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

def run_all_n_to_m(particles_list, n, m, folders=["out_test/ampl/", "out_test/sq_ampl/", "out_test/sq_ampl_raw", "out_test/insertions/",
        "out_test/log/"], file_names=["ampl.txt", "sq_ampl.txt", "sq_ampl_raw.txt", "insertions.txt", "log.log"]):

    possible_processes = get_possible_n_to_m_all_orderings(particles_list, n, m)
    print("Calculating all", n, "to", m, "processes.")
    print("number of potential processes:", len(possible_processes))

    ampl_folder, sqampl_folder, sqampl_raw_folder, insertions_folder, log_folder = folders
    ampl_folder = ampl_folder + "/" + str(n) + "to" + str(m) + "/"
    sqampl_folder = sqampl_folder + "/" + str(n) + "to" + str(m) + "/"
    insertions_folder = insertions_folder + "/" + str(n) + "to" + str(m) + "/"
    log_folder = log_folder + "/" + str(n) + "to" + str(m) + "/"
    folders = [ampl_folder, sqampl_folder, sqampl_raw_folder, insertions_folder, log_folder]

    # delete_folder(ampl_folder[:-1])
    # delete_folder(sqampl_folder)
    # delete_folder(insertions_folder)
    # delete_folder(log_folder)
    for process in tqdm(possible_processes):
        run_process(process, folders, file_names)


def run_all_n_to_m_parallel(particles_list, n, m, folders=["out_test/ampl/",
        "out_test/ampl_raw", "out_test/sq_ampl/", "out_test/sq_ampl_raw/" "out_test/insertions/",
        "out_test/diagrams/", "out_test/log/"], file_names=["ampl.txt", "ampl_raw.txt",
                "sq_ampl.txt", "sq_ampl_raw.txt", "insertions.txt",
                "diagrams.txt", "log.log"],
        cpu_cores=2, delete_previous=False):

    possible_processes = get_possible_n_to_m_all_orderings(particles_list, n, m)
    print("Calculating all", n, "to", m, "processes.")
    print("number of potential processes:", len(possible_processes))

    ampl_folder, ampl_raw_folder, sqampl_folder, sqampl_raw_folder, insertions_folder, diagrams_folder, log_folder = folders
    ampl_folder = ampl_folder +  str(n) + "to" + str(m) + "/"
    ampl_raw_folder = ampl_raw_folder + str(n) + "to" + str(m) + "/"
    sqampl_folder = sqampl_folder +  str(n) + "to" + str(m) + "/"
    sqampl_raw_folder = sqampl_raw_folder + str(n) + "to" + str(m) + "/"
    insertions_folder = insertions_folder + str(n) + "to" + str(m) + "/"
    diagrams_folder = diagrams_folder + str(n) + "to" + str(m) + "/"
    log_folder = log_folder + str(n) + "to" + str(m) + "/"
    folders = [ampl_folder, ampl_raw_folder, sqampl_folder, sqampl_raw_folder, insertions_folder, diagrams_folder, log_folder]


    if delete_previous:
        delete_folder(ampl_folder[:-1])
        delete_folder(ampl_raw_folder[:-1])
        delete_folder(sqampl_folder)
        delete_folder(sqampl_raw_folder)
        delete_folder(insertions_folder)
        delete_folder(diagrams_folder)
        delete_folder(log_folder)

    tasks = list(zip(possible_processes, cycle([folders]), cycle([file_names])))

    # with Pool(processes=cpu_cores) as pool:
    #     p = pool.imap(run_process_phelper, tasks)
    #     result = []
    #     for i in tqdm(p, total=len(tasks)):
    #         result.append(i)

    with Pool(processes=cpu_cores) as p:
        _ = progress_imapu(run_process_phelper, tasks, n_cpu=cpu_cores)  #, core_progress=True)

def run_process_phelper(task):
    processes = task[0]
    global INCLUDE_OFF_SHELL
    if INCLUDE_OFF_SHELL:
        processes = all_offshell_combinations(list(processes))
    else:
        processes = [processes]
    processes = [particles_format(p) for p in processes]
    folders = task[1]
    for process in processes:
        run_process(process, folders)
    return 0

def run_process(process_string, folders=["out_test/ampl/", "out_test/ampl_raw/",
        "out_test/sq_ampl/", "out_test/sq_ampl_raw", "out_test/insertions/", "out_test/diagrams/",
        "out_test/log/"], file_names=["ampl.txt", "ampl_raw.txt", "sq_ampl.txt",
                "sq_ampl_raw.txt", "insertions.txt", "diagrams.txt",
                "log.log"]):
    basenames_without_ext = [os.path.splitext(os.path.basename(f))[0] for f in file_names]
    extensions = [os.path.splitext(os.path.basename(f))[1] for f in file_names]
    _ = [Path(folder).mkdir(parents=True, exist_ok=True) for folder in folders]

    logging.info("Running process " + process_string)

    ampl_file = folders[0]+basenames_without_ext[0]+"-"+process_string.replace(",", "-")+extensions[0]
    ampl_raw_file = folders[1]+basenames_without_ext[1]+"-"+process_string.replace(",", "-")+extensions[1]
    sq_ampl_file = folders[2]+basenames_without_ext[2]+"-"+process_string.replace(",", "-")+extensions[2]
    sq_ampl_raw_file = folders[3]+basenames_without_ext[3]+"-"+process_string.replace(",", "-")+extensions[3]
    insertions_file =  folders[4]+basenames_without_ext[4]+"-"+process_string.replace(",", "-")+extensions[4]
    diagrams_file = folders[5]+basenames_without_ext[5]+"-"+process_string.replace(",", "-")+extensions[5]
    log_file =  folders[6]+basenames_without_ext[6]+"-"+process_string.replace(",", "-")+extensions[6]

    if (exists(ampl_file) and exists(sq_ampl_file) and exists(sq_ampl_raw_file)):
        print("---------------------")
        print("files already exist:")
        print(ampl_file)
        print(sq_ampl_file)
        print(sq_ampl_raw_file)
        print("Skipping process")
        print("---------------------")
        print("")
    else:
        calc_amplitude(process_string, 
                        ampl_file=ampl_file,
                        ampl_raw_file=ampl_raw_file,
                        sqampl_file=sq_ampl_file,
                        sqampl_raw_file=sq_ampl_raw_file,
                        insertions_file=insertions_file,
                        log_file=log_file,
                        diagrams_file=diagrams_file
                        )

def delete_folder(folder):
    try:
        shutil.rmtree(folder, ignore_errors=True)
        print("Out folder", folder, "existed before. Deleted")
    except:
        pass

def delete_file(file):
    try:
        os.remove(file)
        print("Out file", file, "existed before. Deleted")
    except:
        pass


if __name__== "__main__":
    cpu_cores = 19
    ampl_folder = "out_test/ampl/"
    ampl_raw_folder = "out_test/ampl_raw/"
    sqampl_folder = "out_test/sq_ampl/"
    sqampl_raw_folder = "out_test/sq_ampl_raw/"
    insertions_folder = "out_test/insertions/"
    diagrams_folder = "out_test/diagrams/"
    log_folder = "out_test/log/"
    progress_file = "out_test/progress.txt"
    folders = [ampl_folder, ampl_raw_folder, sqampl_folder, sqampl_raw_folder, insertions_folder, diagrams_folder, log_folder]
    delete_previous = False   # delete previous out folder

    _ = Path(log_folder).mkdir(parents=True, exist_ok=True)
    logging.basicConfig(filename = 'out_test/log/general_log.log',
                    level = logging.DEBUG,
                    format = '%(asctime)s:%(levelname)s:%(name)s:%(message)s')


    # run_all_n_to_m_parallel(particles_list, 1, 2, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    # run_all_n_to_m_parallel(particles_list, 2, 1, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    run_all_n_to_m_parallel(particles_list, 2, 2, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    # run_all_n_to_m_parallel(particles_list, 3, 1, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    # run_all_n_to_m_parallel(particles_list, 1, 3, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    # run_all_n_to_m_parallel(particles_list, 3, 2, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    # run_all_n_to_m_parallel(particles_list, 2, 3, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    # run_all_n_to_m_parallel(particles_list, 3, 3, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
    # run_all_n_to_m_parallel(particles_list, 2, 4, folders, cpu_cores=cpu_cores, delete_previous=delete_previous)
