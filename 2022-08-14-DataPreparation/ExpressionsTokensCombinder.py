import re

def combine_m_match(matchobj):
    """
    `m_tt ^ x --> m_tt2 ^ x/2` makes sequences shorter
    """
    match = matchobj.group(0).split("^")
    if int(match[1]) % 2 != 0:
        print("asdf")
    exponent = int(match[1]) // 2
    m, m_name = match[0].split("_")
    if exponent == 1:
        return m+"2"+m_name
    else:
        return m+"2"+m_name + "**" + str(exponent)

def combine_m_s_match(matchobj):
    """
    `m_tt2**y*s_12 --> m2_ttyxs_12` makes sequences shorter
    """
    x = matchobj.group(0).split("**")
    if len(x) == 2:
        m = x[0]
        exp_and_s = x[1]
        exp, s = exp_and_s.split("*")
        return m+exp+"x"+s
    else:
        m, s = x[0].split("*")
    return m+"x"+s

def combine_m(expr_str):
    reg_ex = "(m\_[a-z]{1,})\^(\d{1,})"
    replaced = re.sub(reg_ex, combine_m_match, expr_str)
    return replaced

def combine_m_s(expr_str):
    reg_ex = "m\d[a-z]{1,}(\*\*\d{1,})?\*s\_\d\d"
    replaced = re.sub(reg_ex, combine_m_s_match, expr_str)
    return replaced
