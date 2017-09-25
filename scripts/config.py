__author__ = 'kisame'

import os

"""
Here is ULTRA param finder in config.txt it searches for path="some/path/to/out/dir"
and sets out_dir variable to it.
"""

def read_cfg_path(cfg_path):
    out_dir = ""

    with open(cfg_path, "r") as cfg_file:
        str_list = cfg_file.readlines()
        for str_index in range(len(str_list)):
            str_line = str_list[str_index].replace('\n', '').split("=")
            if len(str_line) == 2:
                str_param = str_line[0]
                str_value = str_line[1].replace('"', '')
                if str_param == "path":
                    out_dir = str_value
            else:
                print("Wrong config line:" + str_line)
    
    if len(out_dir) > 0:
        files = [f for f in os.listdir(out_dir) if not os.path.isfile(f)]
        files.insert(0, "current folder")
        for i in range(0, len(files)):
            print("[{}] - {}".format(i, files[i]))
        file_id = int(input())
        if file_id != 0:
            out_dir = out_dir + files[file_id] + "/"

    return out_dir
