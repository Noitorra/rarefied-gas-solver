__author__ = 'dimaxx'

import os
import shutil
import config

gas_num = 1
params = ["conc", "temp", "pressure", "flow"]
#params = ["conc"]

out_dir = config.read_cfg_path("config.txt")

out_num = len(out_dir)

for out_i in range(0, out_num):
	for param in params:
	  for gas in range(gas_num):
	    data_folder = out_dir[out_i] + 'gas' + '%i' % gas + '/' + param
	    folder = data_folder + '/pic'
	
	    for the_file in os.listdir(folder):
	      file_path = os.path.join(folder, the_file)
	      try:
	          if os.path.isfile(file_path):
	              os.unlink(file_path)
	          elif os.path.isdir(file_path): shutil.rmtree(file_path)
	      except Exception(e):
	          print(e)
