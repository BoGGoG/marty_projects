import os
import csv
import more_itertools
import re
from icecream import ic

indices_roman = [
        "i",
        "j",
        "k",
        "l",
        ]

indices_greek = [
        "gam",
        "del",
        "eta",
        "sigma",
        ]

def read_amplitudes(folder):
    files = os.listdir(folder)
    ret = []
    for file in files:
        with open(folder+file) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=';')
            for row in csv_reader:
                ret.append(row[:-1])
    return ret


def fix_operator_num_args(tree_expression, op="Prod"):
    """Prod 1 2 3 --> Prod 1 Prod 2 3"""
    if (tree_expression[0] == op) and (len(tree_expression) > 3):
        return [op, fix_operator_num_args(tree_expression[1], op=op)] + fix_operator_num_args([[op] + fix_operator_num_args(tree_expression[2:], op=op)])
    elif type(tree_expression) == type([1234]):
        return [fix_operator_num_args(e, op=op) for e in tree_expression]
    else:
        return tree_expression


def get_tree(expression):
    last_open_bracket_idx = get_last_open_bracket(expression)
    while last_open_bracket_idx != -1:
        next_closing_bracket_idx = get_next_closing_bracket(expression, last_open_bracket_idx)
        sub_expr = expression[last_open_bracket_idx+1:next_closing_bracket_idx]
        sub_expr = [expression[last_open_bracket_idx-1]] + sub_expr  # add operator before ()
        expression[last_open_bracket_idx-1] = sub_expr
        del expression[last_open_bracket_idx:next_closing_bracket_idx+1]
        last_open_bracket_idx = get_last_open_bracket(expression)

    return expression[0]

def fix_tree(tree_expression, operators=["Sum", "Prod"]):
    for op in operators:
        tree_expression = fix_operator_num_args(tree_expression, op=op)
    # return tree_expression
    ret = list(more_itertools.collapse(tree_expression))
    # ret = [value for value in ret if value not in ["(", ")"]]
    return ret


def get_last_open_bracket(expression):
    for i in range(len(expression)):
        if expression[-i] == '(':
            return len(expression) - i
        else:
            pass
    return -1


def get_next_closing_bracket(expression, last_open_bracket_idx):
    for i in range(last_open_bracket_idx+1, len(expression)):
        if expression[i] == ")":
            return i
    return -1

def fix_subscripts(expression):
    """expression: flat array of strings"""
    indices = set([])
    for i, str in enumerate(expression):
        str_new, idxs = fix_subscript(str)
        expression[i] = str_new
        for idx in idxs:
            indices.add(idx)

    greek = set([])
    for index in indices:
        ind_name = index.split("_")[0]
        if ind_name in indices_greek:
            greek.add(index)
    roman = set([])
    for index in indices:
        ind_name = index.split("_")[0]
        if ind_name in indices_roman:
            roman.add(index)
    # print("greek indices:", greek)
    # print("roman indices:", roman)
    ret = list(more_itertools.collapse(expression))
    return ret
        

def fix_subscript(str):
    if not has_subscript(str):
        return str, []
    var, subscript = str.split("_", maxsplit=1)
    if var == "gamma":
        subscripts = format_gamma(subscript)
        return [list(more_itertools.collapse(["gamma", subscripts])), subscripts]
    elif var == "p":
        num, index = format_p(subscript)
        return [["p_"+num, index], [index]]
    else:
        new_str, subscripts = format_other_subscripts(var, subscript)
        new_str = list(more_itertools.collapse(new_str))
        return [new_str, subscripts]

def format_p(subs):
    subs = subs.replace("\\", "")
    subs = subs.replace("%", "")
    num, index = subs.split("_", maxsplit=1)
    ic(num, index)
    return [num, index]

def format_gamma(subscript):
    """
    input looks like: '{%\\sigma_49,%gam_44,%eta_12}'
    gamma alsways has 3 indices
    """
    ind1, ind2, ind3 = subscript[1:-1].split(",")
    ind1 = format_index(ind1)
    ind2 = format_index(ind2)
    ind3 = format_index(ind3)

    return [ind1, ind2, ind3]


def format_index(ind):
    ind = ind.replace("\\","")
    ind = ind.replace("%", "")
    ind = ind.replace("+", "")
    # # not really consistent in data, so I'll just leave the + out
    # if ind[0] == "+":
    #     position = "up"
    #     ind = ind[1:]
    # else:
    #     position = "down"
    return ind

def format_other_subscripts(var, subscript):
    subscript = subscript[1:]  # remove first "{"
    subscript = subscript.replace("p", "|p|")
    subscript, other = subscript.split("}")
    ind1, ind2 = subscript.split(",")
    ind1 = format_index(ind1)
    ind2 = format_index(ind2)

    other = other.split("^")

    if len(other) == 2:
        var = var+"^"+other[1]
    other = other[0]


    return [[var, ind1, ind2, other], [ind1, ind2]]

def has_subscript(str):
    ret = ("_" in str) and ("{" in str)
    ret = (ret or ("p_" in str))
    return ret

