__author__ = 'dimaxx'

import os
import shutil

gas_num = 2
params = ["conc", "temp", "pressure"]
#params = ["conc"]

"""
Here is ULTRA param finder in config.txt it searches for path="some/path/to/out/dir"
and sets out_dir variable to it.
"""

out_dir = '../out/'

with open("config.txt", "r") as cfg_file:
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

"""
"""

for param in params:
  for gas in range(gas_num):
    data_folder = out_dir + 'gas' + '%i' % gas + '/' + param
    folder = data_folder + '/pic'

    for the_file in os.listdir(folder):
      file_path = os.path.join(folder, the_file)
      try:
          if os.path.isfile(file_path):
              os.unlink(file_path)
          elif os.path.isdir(file_path): shutil.rmtree(file_path)
      except Exception(e):
          print(e)