import sys
import os
from icecream import ic 
import csv
import numpy as np
import more_itertools
import pytest
  
current = os.path.dirname(os.path.realpath(__file__))
parent = os.path.dirname(current)
sys.path.append(parent)

from source.read_amplitudes import read_amplitudes, fix_operator_num_args, get_tree, fix_tree
  
amplitudes_folder = "../QED_IO/out/ampl/2to2/"
sqamplitudes_folder = "../QED_IO/out/sq_ampl/2to2/"

amplitudes_files = os.listdir(amplitudes_folder)
sqamplitudes_files = os.listdir(sqamplitudes_folder)

ampl = read_amplitudes(amplitudes_folder)
sqampl = read_amplitudes(sqamplitudes_folder)

def test_get_tree():
    expression_1 = ["Prod", "(", "2", "a", ")"]
    expression_2 = ["Prod", "(", "2", "a", "b", ")"]
    expression_3 = ["Prod", "(", "2", "a", "b", "Prod", "(", "c", "d", ")", ")"]
    assert get_tree(expression_1) == ["Prod", "2", "a"]
    assert get_tree(expression_2) == ["Prod", "2", "a", "b"]
    assert get_tree(expression_3) == ['Prod', '2', 'a', 'b', ['Prod', 'c', 'd']]
    expression_4 = ["Prod", "(", "2", "a", "b", "Prod", "(", "c", "d", ")", ")"]

def test_get_tree_2():
    expression_1 = ['Prod', '(', '-1', 'i', 'e', 'gamma_{%\\sigma_49,%gam_44,%eta_12}', 'A_{l_3,+%\\sigma_49}(p_3)^(*)', 'e_{i_3,%eta_12}(p_1)_u', 'e_{k_3,%gam_44}(p_2)_u^(*)', ')']
    tree = get_tree(expression_1)
    tree = fix_tree(expression_1)

def test_fix_operator_num_args():
    expression_1 = get_tree(["Prod", "(", "2", "a", ")"])
    expression_2 = get_tree(["Prod", "(", "2", "a", "b", ")"])
    assert fix_operator_num_args(expression_1, op="Prod") == expression_1
    assert fix_operator_num_args(expression_2, op="Prod") == ["Prod", "2", ["Prod", "a", "b"]]
    expression_3 = get_tree(["Sum", "(", "2", "a", "b", ")"])
    assert fix_operator_num_args(expression_3, op="Sum") == ["Sum", "2", ["Sum", "a", "b"]]
    expression_4 = get_tree(["Prod", "(", "2", "a", "b", "Prod", "(", "c", "d", ")", ")"])

def test_fix_tree():
    expression_3 = get_tree(["Prod", "(", "2", "a", "b", "Sum", "(", "c", "d", "1", ")", ")"])
    expression_4 = get_tree(["Sum", "(", "2", "a", "b", "Sum", "(", "c", "d", "1", ")", ")"])
    assert fix_tree(expression_3) == list(more_itertools.collapse(['Prod', '2', ['Prod', 'a', ['Prod', 'b', ['Sum', 'c', ['Sum', 'd', '1']]]]]))
    assert fix_tree(expression_4) == list(more_itertools.collapse(['Sum', '2', ['Sum', 'a', 'b', ['Sum', 'c', ['Sum', 'd', '1']]]]))
