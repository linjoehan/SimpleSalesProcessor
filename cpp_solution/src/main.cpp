#include <iostream>
#include <vector>
#include <fstream>
#include <string>
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

int main()
{
    //Establish connection to database
    const char *conninfo;
    PGconn *conn;
    
    conninfo = "dbname = simplesalesprocessor";
    
    conn = PQconnectdb(conninfo);
    if(PQstatus(conn) != CONNECTION_OK)
    {
        std::cout << "Connection to database failed:" << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return -1;
    }
    
    std::cout << "Sucessfully connected to database" << std::endl;
    
    //Process all Region data files
    std::string file_path = "./Data/Region/";
    std::vector<std::string> files = get_all_files_names_within_folder(file_path);
    std::cout << "Region files:" << files.size() << std::endl;
    
    
    PQfinish(conn);
    
    
    return 0;
}



