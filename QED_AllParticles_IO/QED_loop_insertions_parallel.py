import os
from subprocess import call
from itertools import combinations_with_replacement, product
from tqdm import tqdm
from icecream import ic
from pathlib import Path
import logging
import shutil
from multiprocessing import Pool
from itertools import cycle
from parallelbar import progress_imap, progress_map, progress_imapu

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

def calc_amplitude(particles, ampl_file="out/ampl.txt",
        sqampl_file="out/sq_ampl.txt",
        insertions_file="out/insertions.txt",
        log_file=False
        ):

    options = "--particles=" + particles + " -e" + " -a " + ampl_file + " -s " + sqampl_file + " -i " + insertions_file
    if log_file:
        options = options + " > " + log_file

    _ = call("./QED_AllParticles_IO.x " + options, shell=True)
    

def particles_format(particles_list):
    return ",".join(particles_list)


def get_possible_n_to_m(particles_list, n, m):
    in_list = ["in_"+p for p in particles_list]
    out_list = ["out_"+p for p in particles_list]

    possible_two_in = combinations_with_replacement(in_list, n)
    possible_two_out = combinations_with_replacement(out_list, m)
    possible_two_to_two = list(product(possible_two_in, possible_two_out))
    possible_two_to_two = [sum(p, ()) for p in possible_two_to_two]
    possible_two_to_two = [particles_format(p) for p in possible_two_to_two]

    return possible_two_to_two

def run_all_n_to_m(particles_list, n, m, folders=["out/ampl/", "out/sq_ampl/", "out/insertions/",
        "out/log/"], file_names=["ampl.txt", "sq_ampl.txt", "insertions.txt", "log.log"]):

    possible_processes = get_possible_n_to_m(particles_list, n, m)
    print("Calculating all", n, "to", m, "processes.")
    print("number of potential processes:", len(possible_processes))

    ampl_folder, sqampl_folder, insertions_folder, log_folder = folders
    ampl_folder = ampl_folder + "/" + str(n) + "to" + str(m) + "/"
    sqampl_folder = sqampl_folder + "/" + str(n) + "to" + str(m) + "/"
    insertions_folder = insertions_folder + "/" + str(n) + "to" + str(m) + "/"
    log_folder = log_folder + "/" + str(n) + "to" + str(m) + "/"
    folders = [ampl_folder, sqampl_folder, insertions_folder, log_folder]

    delete_folder(ampl_folder[:-1])
    delete_folder(sqampl_folder)
    delete_folder(insertions_folder)
    delete_folder(log_folder)
    for process in tqdm(possible_processes):
        run_process(process, folders, file_names)


def run_all_n_to_m_parallel(particles_list, n, m, folders=["out/ampl/", "out/sq_ampl/", "out/insertions/",
        "out/log/"], file_names=["ampl.txt", "sq_ampl.txt", "insertions.txt", "log.log"],
        cpu_cores=2):

    possible_processes = get_possible_n_to_m(particles_list, n, m)
    print("Calculating all", n, "to", m, "processes.")
    print("number of potential processes:", len(possible_processes))

    ampl_folder, sqampl_folder, insertions_folder, log_folder = folders
    ampl_folder = ampl_folder + "/" + str(n) + "to" + str(m) + "/"
    sqampl_folder = sqampl_folder + "/" + str(n) + "to" + str(m) + "/"
    insertions_folder = insertions_folder + "/" + str(n) + "to" + str(m) + "/"
    log_folder = log_folder + "/" + str(n) + "to" + str(m) + "/"
    folders = [ampl_folder, sqampl_folder, insertions_folder, log_folder]

    delete_folder(ampl_folder[:-1])
    delete_folder(sqampl_folder)
    delete_folder(insertions_folder)
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
    process_string = task[0]
    folders = task[1]
    return run_process(process_string, folders)

def run_process(process_string, folders=["out/ampl/", "out/sq_ampl/", "out/insertions/",
        "out/log/"], file_names=["ampl.txt", "sq_ampl.txt", "insertions.txt", "log.log"]):
    basenames_without_ext = [os.path.splitext(os.path.basename(f))[0] for f in file_names]
    extensions = [os.path.splitext(os.path.basename(f))[1] for f in file_names]
    _ = [Path(folder).mkdir(parents=True, exist_ok=True) for folder in folders]

    logging.info("Running process " + process_string)

    ampl_file = folders[0]+basenames_without_ext[0]+"-"+process_string.replace(",", "-")+extensions[0]
    sq_ampl_file = folders[1]+basenames_without_ext[1]+"-"+process_string.replace(",", "-")+extensions[1]
    insertions_file =  folders[2]+basenames_without_ext[2]+"-"+process_string.replace(",", "-")+extensions[2]
    log_file =  folders[3]+basenames_without_ext[3]+"-"+process_string.replace(",", "-")+extensions[3]
    calc_amplitude(process_string, ampl_file=ampl_file,
            sqampl_file=sq_ampl_file,
            insertions_file=insertions_file,
            log_file=log_file
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
    cpu_cores = 12
    ampl_folder = "out/ampl/"
    sqampl_folder = "out/sq_ampl/"
    insertions_folder = "out/insertions/"
    log_folder = "out/log/"
    folders = [ampl_folder, sqampl_folder, insertions_folder, log_folder]

    _ = Path(log_folder).mkdir(parents=True, exist_ok=True)
    logging.basicConfig(filename = 'out/log/general_log.log',
                    level = logging.DEBUG,
                    format = '%(asctime)s:%(levelname)s:%(name)s:%(message)s')


    run_all_n_to_m_parallel(particles_list, 1, 2, folders, cpu_cores=cpu_cores)
    run_all_n_to_m_parallel(particles_list, 2, 1, folders, cpu_cores=cpu_cores)
    run_all_n_to_m_parallel(particles_list, 2, 2, folders, cpu_cores=cpu_cores)
    run_all_n_to_m_parallel(particles_list, 3, 1, folders, cpu_cores=cpu_cores)
    run_all_n_to_m_parallel(particles_list, 3, 2, folders, cpu_cores=cpu_cores)
    run_all_n_to_m_parallel(particles_list, 2, 3, folders, cpu_cores=cpu_cores)
    run_all_n_to_m_parallel(particles_list, 1, 3, folders, cpu_cores=cpu_cores)
    # run_all_n_to_m_parallel(particles_list, 3, 3, folders, cpu_cores=cpu_cores)
