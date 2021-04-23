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
  #os.replace(existing_file,target_file)
  return

def count_delimiters(line,delim):
  return line.count(delim)

def load_sql_from_file(file):
  infile = open(file, "r")
  filestr = infile.read()
  infile.close()
  return filestr

def process_region_files(conn):
  file_path = "./Data/Region/"
  files = get_all_files_names_within_folder(file_path)
  print("Region data files found:" , len(files))
  
  for file_i in files:
    print("Processing ",file_i,"...")
    filestream = open(file_path+file_i,"r")
    line = ""
    
    verify_header = False
    verify_records = True
    verify_processed = True
    
    if filestream.closed == False:
      #Validate file header (first line)
      line = filestream.readline()
      line = line.rstrip()
      verify_header = line == "Region,RegionDescription,StartDate,EndDate"
      line = filestream.readline()
      if verify_header:
        while True and verify_records:
          if line == "" :
            break
          delimiters = line.count(",")
          verify_records = delimiters == 3
          line = filestream.readline()
    filestream.close()
    
    #Process file into database raw
    if verify_header and verify_records and verify_processed:
      sql = load_sql_from_file("./sql/raw_region.sql");
      full_file_name = os.path.abspath(file_path+file_i)
      sql = sql.replace("%filename%",full_file_name)
      cur = conn.cursor()
      try:
        cur.execute(sql)
      except:
        print("Could not Exceute sql query:",sql)
        verify_processed = False
      cur.close()
    
    #Process file into database table
    if verify_header and verify_records and verify_processed:
      sql = load_sql_from_file("./sql/raw_to_tbl_region.sql")
      cur = conn.cursor()
      try:
        cur.execute(sql)
      except:
        print("Could not Exceute sql query:",sql)
        verify_processed = False
      cur.close()
    
    #archive or reject the processed file
    if verify_header and verify_records and verify_processed:
      move_file(file_path,file_i,"archive")
      print(" DONE")
    else:
      move_file(file_path,file_i,"reject")
      print(" FAILED")
      
    conn.commit()
    
  return

def process_sales_files(conn):
  conn = psycopg2.connect(database="simplesalesprocessor")
  file_path = "./Data/Sales/"
  files = get_all_files_names_within_folder(file_path)
  print("Sales data files found:" , len(files))
  
  for file_i in files:
    print("Processing ",file_i,"...")
    filestream = open(file_path+file_i,"r")
    line = ""
    
    verify_header = False
    verify_records = True
    verify_processed = True
    
    if filestream.closed == False:
      #Validate file header (first line)
      line = filestream.readline()
      line = line.rstrip()
      verify_header = line == "identifier,Network,Region,Date,Product,Amount"
      line = filestream.readline()
      if verify_header:
        while True and verify_records:
          if line == "" :
            break
          delimiters = line.count(",")
          verify_records = delimiters == 5
          line = filestream.readline()
    filestream.close()
    
    #Process file into database raw
    if verify_header and verify_records and verify_processed:
      sql = load_sql_from_file("./sql/raw_sales.sql");
      full_file_name = os.path.abspath(file_path+file_i)
      sql = sql.replace("%filename%",full_file_name)
      cur = conn.cursor()
      try:
        cur.execute(sql)
      except:
        print("Could not Exceute sql query:",sql)
        verify_processed = False
      cur.close()
    
    #Process file into database table
    if verify_header and verify_records and verify_processed:
      sql = load_sql_from_file("./sql/raw_to_tbl_sales.sql")
      cur = conn.cursor()
      try:
        cur.execute(sql)
      except:
        print("Could not Exceute sql query:",sql)
        verify_processed = False
      cur.close()
    
    #archive or reject the processed file
    if verify_header and verify_records and verify_processed:
      move_file(file_path,file_i,"archive")
      print(" DONE")
    else:
      move_file(file_path,file_i,"reject")
      print(" FAILED")
    
    conn.commit()
    
  return

def run_sales_summary_report(conn):
  report_sucess = True
  sql = load_sql_from_file("./sql/sales_summary_report.sql")
  cur = conn.cursor()
  try:
    cur.exceute(sql)
  except:
    print("Could not Exceute sql query:",sql)
    report_sucess = False
  
  if report_sucess:
    #reportfile = open("./Data/Output/Sales_Summary_Report.txt","w")
    colnames = [desc[0] for desc in cur.description]
    data = cur.fetchall()
    print(colnames)
    print(data)
    #reportfile.close()
  
  cur.close()
  return
    
  
conn = psycopg2.connect(database="simplesalesprocessor")
process_region_files(conn)
process_sales_files(conn)
run_sales_summary_report(conn)
conn.close()