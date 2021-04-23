#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>

#include "libpq-fe.h"

//Function used to get all files 
//Reference: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
std::vector<std::string> get_all_files_names_within_folder(std::string folder)
{
    std::vector<std::string> names;
    std::string search_path = folder + "/*.*";
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

//Move rejected file to rejected folder or completed file to archive folder
//Note that this will replace files if they are named the same
int move_file(std::string filepath,std::string filename,std::string move_type)
{
    std::string target_path;
    if(move_type == "reject")
    {
        target_path = "./Data/Rejected/";
    }
    else if (move_type == "archive")
    {
        target_path = "./Data/Archive/";
    }
    else
    {
        std::cerr << "Unknown move type:" << move_type << " Supported types are \"reject\" and \"archive\" " << std::endl;
    }
    
    const std::string existing_file = filepath+filename;
    const std::string target_file = target_path+filename;
    
    int res = MoveFileEx(existing_file.c_str(),target_file.c_str(),MOVEFILE_REPLACE_EXISTING);
    if(res==0)
    {
        std::cerr << "Could not move file " << existing_file << " to " << target_file << std::endl;
        std::cerr << "Error Code:" << GetLastError();
    }
    return res;
}

int count_delimiters(std::string line,char delim)
{
    int res = 0;
    for(unsigned i = 0;i<line.size();i++)
    {
        if(line[i] == delim)
        {
            res++;
        }
    }
    return res;
}

//Reference https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
std::string load_sql_from_file(std::string file)
{
    std::ifstream infile;
    infile.open(file);
    if(infile.is_open())
    {
        std::stringstream ss;
        ss << infile.rdbuf();
        return ss.str();
    }
}

//Reference: https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
std::string replace(const std::string& str, const std::string& from, const std::string& to) 
{
    std::string res = str;
    size_t start_pos = res.find(from);
    while(start_pos != std::string::npos)
    {
        res.replace(start_pos, from.length(), to);
        start_pos = res.find(from);
    }
    return res;
}

std::string get_full_path()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL,buffer,sizeof(buffer));
    std::string res = std::string(buffer);
    
    //Change slash direction
    res = replace(res,"\\","/");
    return res;
}

void process_region_files(PGconn *conn)
{
    std::string file_path = "./Data/Region/";
    std::vector<std::string> files = get_all_files_names_within_folder(file_path);
    
    std::cout << "Region data files found:" << files.size() <<std::endl;
    
    for(int i = 0;i<files.size();i++)
    {
        std::cout << "Processing " << files[i] << "..." <<std::endl;
        
        std::ifstream filestream;
        filestream.open(file_path+files[i]);
        std::string line;
        
        bool verify_header = false;
        bool verify_records = true;
        bool verify_processed = true;
        
        PGresult *res = NULL;
        std::string sql;
            
        if(filestream.is_open())
        {
            //Validate file header (first line)
            std::getline(filestream,line);
            verify_header = line == "Region,RegionDescription,StartDate,EndDate";
            
            //Validate file delimiters
            if(verify_header)
            {
                while(std::getline(filestream,line) and verify_records)
                {
                    int delimiters = count_delimiters(line,',');
                    verify_records = delimiters == 3;
                }
            }
        }
        filestream.close();
        
        //Process file into database raw
        if(verify_header and verify_records and verify_processed) 
        {
            //Process file to raw database table
            sql = load_sql_from_file("./sql/raw_region.sql");
            
            //Workaround to use copy with absolute path as relative path is not supported
            std::string full_file_name = replace(get_full_path(),"/cpp_solution/bin/SimpleSalesProcessor.exe","") + replace(file_path,".","") + files[i];
            
            sql = replace(sql,"%filename%",full_file_name);
            
            res = PQexec(conn,sql.c_str());
            if(PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                std::cerr << "Could not Exceute sql query: " << PQresultErrorMessage(res);
                verify_processed = false;
            }
            
            //Other data verifications on the raw data
        }
        
        //Process file into database table
        if(verify_header and verify_records and verify_processed)
        {
            //Load the raw data into the main table and remove the raw data table
            sql = load_sql_from_file("./sql/raw_to_tbl_region.sql");
            res = PQexec(conn,sql.c_str());
            if(PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                std::cerr << "Could not Exceute sql query: " << PQresultErrorMessage(res);
                verify_processed = false;
            }
        }
        
        //archive or reject the processed file
        if(verify_header and verify_records and verify_processed)
        {
            move_file(file_path,files[i],"archive");
            std::cout << " DONE" << std::endl;
        }
        else
        {
            move_file(file_path,files[i],"reject");
            std::cout << " FAILED" << std::endl;
        }
    }
}

void process_sales_files(PGconn *conn)
{
    std::string file_path = "./Data/Sales/";
    std::vector<std::string> files = get_all_files_names_within_folder(file_path);
    
    std::cout << "Sales data files found:" << files.size() <<std::endl;
    
    for(int i = 0;i<files.size();i++)
    {
        std::cout << "Processing " << files[i] << "..." <<std::endl;
        
        std::ifstream filestream;
        filestream.open(file_path+files[i]);
        std::string line;
        
        bool verify_header = false;
        bool verify_records = true;
        bool verify_processed = true;
        
        PGresult *res = NULL;
        std::string sql;
            
        if(filestream.is_open())
        {
            //Validate file header (first line)
            std::getline(filestream,line);
            verify_header = line == "identifier,Network,Region,Date,Product,Amount";
            
            //Validate file delimiters
            if(verify_header)
            {
                while(std::getline(filestream,line) and verify_records)
                {
                    int delimiters = count_delimiters(line,',');
                    verify_records = delimiters == 5;
                }
            }
        }
        filestream.close();
        
        //Process file into database raw
        if(verify_header and verify_records and verify_processed) 
        {
            //Process file to raw database table
            sql = load_sql_from_file("./sql/raw_sales.sql");
            
            //Workaround to use copy with absolute path as relative path is not supported
            std::string full_file_name = replace(get_full_path(),"/cpp_solution/bin/SimpleSalesProcessor.exe","") + replace(file_path,".","") + files[i];
            
            sql = replace(sql,"%filename%",full_file_name);
            
            res = PQexec(conn,sql.c_str());
            if(PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                std::cerr << "Could not Exceute sql query: " << PQresultErrorMessage(res);
                verify_processed = false;
            }
            
            //Other data verifications on the raw data
        }
        
        //Process file into database raw
        if(verify_header and verify_records and verify_processed)
        {
            //Load the raw data into the main table and remove the raw data table
            sql = load_sql_from_file("./sql/raw_to_tbl_sales.sql");
            res = PQexec(conn,sql.c_str());
            if(PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                std::cerr << "Could not Exceute sql query: " << PQresultErrorMessage(res);
                verify_processed = false;
            }
        }
        
        //archive or reject the processed file
        if(verify_header and verify_records and verify_processed)
        {
            move_file(file_path,files[i],"archive");
            std::cout << " DONE" << std::endl;
        }
        else
        {
            move_file(file_path,files[i],"reject");
            std::cout << " FAILED" << std::endl;
        }
    }
}

void run_sales_summary_report(PGconn *conn)
{
    std::cout << "Running Sales Summary report..." <<std::endl;
    
    PGresult *res = NULL;
    std::string sql;
    
    sql = load_sql_from_file("./sql/sales_summary_report.sql");
    res = PQexec(conn,sql.c_str());
    
    if(PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        std::cerr << "Could not Exceute sql query: " << PQresultErrorMessage(res);
        std::cout << " FAILED" << std::endl;
    }
    else
    {
        std::string filename = "./Data/Output/Sales_Summary_Report.txt";
        std::ofstream reportfile;
        reportfile.open(filename , std::ofstream::trunc);
        
        if(reportfile.is_open())
        {
            int result_rows = PQntuples(res);
            int result_cols = PQnfields(res);
            
            //Write column headers
            for(int col = 0;col < result_cols;col++)
            {
                reportfile << (col==0 ? "" : ",") << PQfname(res,col);
            }
            reportfile << std::endl;
            
            for(int row = 0;row<result_rows;row++)
            {
                for(int col = 0;col<result_cols;col++)
                {
                    reportfile << (col==0 ? "" : ",") << PQgetvalue(res,row,col);
                }
                reportfile << std::endl;
            }
            
            reportfile.close();
            std::cout << " DONE" <<std::endl;
        }
        else
        {
            std::cerr << "Could not write to file " << filename << std::endl;
            std::cout << " FAILED" << std::endl;
        }
    }
}

int main()
{
    //Establish connection to database
    const char *conninfo;
    PGconn *conn;
    
    conninfo = "dbname = simplesalesprocessor";
    
    conn = PQconnectdb(conninfo);
    if(PQstatus(conn) != CONNECTION_OK)
    {
        std::cerr << "Connection to database failed:" << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return -1;
    }
    
    //Process all Region data files
    process_region_files(conn);
    process_sales_files(conn);
    
    //Output latest report
    run_sales_summary_report(conn);
    
    PQfinish(conn);
    
    return 0;
}


