__author__ = 'kisame'

"""
Here is ULTRA param finder in config.txt it searches for path="some/path/to/out/dir"
and sets out_dir variable to it.
"""

def read_cfg_path(cfg_path):
    out_dir = []

    with open(cfg_path, "r") as cfg_file:
        str_list = cfg_file.readlines()
        for str_index in range(len(str_list)):
            str_line = str_list[str_index].replace('\n', '').split("=")
            if len(str_line) == 2:
                str_param = str_line[0]
                str_value = str_line[1].replace('"', '')
                if str_param == "path":
                    out_dir.append(str_value)
                    # out_dir = str_value
            else:
                print("Wrong config line:" + str_line)
    return out_dir
