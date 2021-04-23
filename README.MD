# Simple Sales Processor
This package contains both a C++ and a Python solution that consumes a regional reference table and Sales data into a Postgres database then generates a Report.

## PostgreSQL setup
#### Install
This package requires a PostgreSQL database. This Package was developed using PostgreSQL version 9.21 (32 bit). Although it may also work with a 64bit database, however the C++ solution will require the 32 bit PostgreSQL libraries. To keep things simple I've included the libraries of version 9.21 in this package, you may however need to replace them if you are using a different PostgreSQL version. For 64 bit versions you will have to replace the libraries and be sure to also use a 64 bit C++ compiler.
#### Install PostgreSQL
An installation guide can be found [here](https://www.tutorialspoint.com/postgresql/postgresql_environment.htm) and the download page [here](https://www.enterprisedb.com/downloads/postgres-postgresql-downloads)
I got errors when trying to unpack this though as it tries to install an older version of VC++ runtime redistributables. This can be remedied by installing it with
```
postgresql-9.6.21-2-windows-x64.exe --install_runtimes 0
```
#### Getting the PostgreSQL libraries
As mentioned before I've included the libraries for version 9.21 in this package. However if you are using something different you will need to copy the include folder from `..\PostgreSQL\{version}\` in your PostgreSQL into `cpp_solution\extern_libs\postgresql` and all the .dll files from 
`...\PostgreSQL\{version}\bin\ into cpp_solution\extern_libs\postgresql\lib`

#### Setting up environment and session variables
You will need to edit `db_session_startup.bat` to the correct values that you've used to setup your PostgreSQL.

## MinGW setup
#### Install
The C++ solution requires MinGW you can follow [this](https://www.ics.uci.edu/~pattis/common/handouts/mingweclipse/mingw.html) guide to set it up for 32 bit and [this](https://nuwen.net/mingw.html) if you are using a 64 bit solution.

#### Environment Variables
You can either edit you local environment PATH variable or edit `cpp_build.bat` and set the PATH variable to where you have installed your MinGW.

## Python setup
Python installation guide found [here](https://www.tutorialspoint.com/python/python_environment.htm)
I used [version 3.94](https://www.python.org/downloads/release/python-394/)
#### Environment Variables
You can either edit you local environment PATH variable or edit `py_setup.bat` to point to where your python installation is.

## Running the solution
#### Database setup
Running or double clicking`db_init.bat` will create the database and tables required for this project to run.

#### C++ Version
To compile the solution you will need to run `cpp_build.bat`. Once built you can run the solution using `cpp_run.bat`

#### Python Version
The python version can be built using `py_build.bat`, this will create the required directories for this project

Running the python solution can be done by running `py_run.bat`

#### Processing the data
To process data, the csv data files will need to be placed in the correct Data folders. There are some validation checks on each data file, Firstly that the header record has the correct column names. and that each record in the file has the correct number of columns. Files in the incorrect format will be rejected.
## Notes
#### Assumptions on the data
Firstly I have to assume that the sales data being dropped into the stream is correct, I didn't see anything that looks like it is a primary key so if the stream is duplicated there may not be any way of knowing. On the Region reference data I assume a primary key on Region and start_date. So conflicts will be updated, however the sales data is enhanced at the time it is inserted which could cause some discrepancies, however I preferred this solution as having the regions possibly changing over time may cause the older reports to conflict with newer ones.

#### Solution improvments
I tried to keep the solution as simple, given that I also had to translate it to python. The processing of both the region and sales data files follow a very similar pattern, and if I had more time I'd refactor those functions into one with variables determining which path to take.
#### Running in a live environment
Again for simplicity, I kept the solution of the file processing and reporting in the same program. In reality it makes more sense to separate them. With say the Processing either being called frequently with a cron job, or running in a loop watching the input folders for files to process. Where as the reporting program will be run only when a report needs to be generated.