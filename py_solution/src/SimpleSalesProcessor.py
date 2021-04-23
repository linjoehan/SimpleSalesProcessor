import psycopg2
import os

def get_all_files_names_within_folder(folder):
  arr = os.listdir(folder)
  return arr

conn = psycopg2.connect(database="simplesalesprocessor")
conn.close()

files = get_all_files_names_within_folder("./Data/Archive/")
print(files)
