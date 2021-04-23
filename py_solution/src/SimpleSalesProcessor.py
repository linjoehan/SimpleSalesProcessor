import psycopg2
import os
import sys

def get_all_files_names_within_folder(folder):
  arr = os.listdir(folder)
  return arr

def move_file(filepath,filename,move_type):
  
  target_path = ""
  if move_type == "reject":
    target_path = "./Data/Rejected/"
  elif move_type == "archive":
    target_path = "./Data/Archive/"
  else:
    sys.stderr.write("Unknown move type:" + move_type + " Supported types are \"reject\" and \"archive\" ")
    return
  
  existing_file = filepath+filename
  target_file = target_path+filename
  os.replace(existing_file,target_file)
  return

def count_delimiters(line,delim):
  return line.count(delim)

def load_sql_from_file(file):
  infile = open(file, "r")
  filestr = infile.read()
  infile.close()
  return filestr

  
  
conn = psycopg2.connect(database="simplesalesprocessor")
conn.close()

sqlfile = "./sql/raw_region.sql"
sql = load_sql_from_file(sqlfile)

print(sql)
