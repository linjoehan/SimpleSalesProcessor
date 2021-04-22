#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "libpq-fe.h"

int main()
{
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
    
    PQfinish(conn);
    
    return 0;
}