#include "user_db_editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <E:/551/sqlite-amalgamation-3080702/sqlite3.h>
//#include <sqlite3.h>
#include <string>
#include <sstream>
user_db_editor::user_db_editor()
{

}
static int callback(void * NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

static int callbackAns(void * NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s\n",  argv[i] ? argv[i] : "NULL");
    //return argv[i] ? argv[i] : "NULL";
  }
  printf("\n");
  return 0;
}

bool user_db_editor::UserDbEditor(std::string s, sqlite3 * db_user)
{
  //sqlite3 *db_user;
  char *zErrMsg = 0;
  int rc;
   /* Execute SQL statement */
  rc = sqlite3_exec(db_user, s.c_str(), callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
   }else{
     fprintf(stdout, "%s successfully\n", s.c_str());
   }
   //sqlite3_close(db_user);
   return 1;
}

std::string user_db_editor::UserDbEditorAnswer(std::string s,int c, sqlite3 * db_user)
{
/*
  char *zErrMsg = 0;
  int rc;
  std::string Ans;
  rc = sqlite3_exec(db_user, s.c_str(), callbackAns, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
   }else{
     fprintf(stdout, "%s successfully\n", s.c_str());
   }
   return "Ans";
   */

        char** pResult;
        char* errmsg;
        int nRow;
        int nCol;
        int nResult = sqlite3_get_table(db_user,s.c_str(),&pResult,&nRow,&nCol,&errmsg);
          if (nResult != SQLITE_OK)
        {
            std::cout<<errmsg<<std::endl;
            sqlite3_free(errmsg);
            return 0;
        }

        /*int nIndex = nCol;
        for(int i=0;i<nRow;i++)
        {
            for(int j=0;j<nCol;j++)
            {
                strOut+=pResult[j];
                strOut+=":";
                strOut+=pResult[nIndex];
                strOut+="\n";
                ++nIndex;
            }
        }
        */
       std::string Answer=pResult[c];
        sqlite3_free_table(pResult);
        //std::cout<<strOut<<std::endl;
        return Answer;
}

int user_db_editor::DbGetSize(sqlite3 * db_user){

     std::string sql="SELECT max(id) From UsersTable;";
     std::string::size_type sz;   // alias of size_t


     std::string Ans=UserDbEditorAnswer(sql,1, db_user);
     int size = std::stoi (Ans,&sz);

     return size;
}

bool user_db_editor::UserDbSelectName(std::string username, sqlite3 * db_user)
{
    std::stringstream idss;
    // Create SQL statement
    std::string sql = "SELECT * from UsersTable WHERE username Like '";
    idss<<username<<"';";
    sql.append(idss.str());
    // sql.append(",'");
    UserDbEditor(sql, db_user);
    return 1;
}

/*bool user_db_editor::DbAddUser(int id, std::string username,std::string auth_token,std::string email, sqlite3 * db_user)
{
  std::stringstream idss;
  idss<<id;
  // Create SQL statement
  std::string sql = "INSERT INTO UsersTable (id,username,auth_token,email) " \
    "VALUES (";
  idss<<",'"<<username<<"','"<<auth_token<<"','"<<email<<"');";
  sql.append(idss.str());
  // sql.append(",'");

  UserDbEditor(sql, db_user);
  return 0;
}
  */

bool user_db_editor::DbAddUser(int id, std::string username,std::string auth_token, sqlite3 * db_user)
{
  std::stringstream idss;
  idss<<id;
  // Create SQL statement
  std::string sql = "INSERT INTO UsersTable (id,username,auth_token) " \
    "VALUES (";
  idss<<",'"<<username<<"','"<<auth_token<<"');";
  sql.append(idss.str());
  // sql.append(",'");

  UserDbEditor(sql, db_user);
  return 0;
}

int user_db_editor::DbInitialize(void)
{
  sqlite3 * db_user;
  sqlite3 * db_file;
  char *zErrMsg = 0;
  int  rc_user, rc_file;
  /* Open database */
  rc_user = sqlite3_open("UsersTable.db", &db_user);
  rc_file = sqlite3_open("FilesTable.db", &db_file);

if( rc_user ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_user));
    exit(0);
  }else{
    fprintf(stdout, "Opened database successfully\n");
  }

if( rc_file ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_file));
    exit(0);
  }else{
    fprintf(stdout, "Opened database successfully\n");
  }

  //Create SQL statement
  /* const char * sql_user = "CREATE TABLE UsersTable("  \
         "id INT PRIMARY KEY     NOT NULL," \
         "username           TEXT    NOT NULL," \
         "auth_token         TEXT     NOT NULL," \
         "email              TEXT    NOT NULL,"\
         "friend_id          TEXT," \
         "created_at         DATETIME ,"\
         "updated_at         DATETIME  );";*/
const char * sql_user = "CREATE TABLE UsersTable("  \
      "id INT PRIMARY KEY     NOT NULL," \
      "username           TEXT    NOT NULL," \
      "auth_token         TEXT     NOT NULL," \

      "friend_id          TEXT," \
      "created_at         DATETIME ,"\
      "updated_at         DATETIME  );";

   const char * sql_file = "CREATE TABLE FilesTable(" \
         "id INT PRIMARY KEY     NOT NULL," \
         "user_id          INT   NOT NULL," \
         "path             TEXT  NOT NULL,"\
         "accessibility    INT     NOT NULL," \
         "created_at       DATETIME," \
         "updated_at       DATETIME );";


   /* Execute SQL statement */
   rc_user = sqlite3_exec(db_user, sql_user, callback, 0, &zErrMsg);
   rc_file = sqlite3_exec(db_user, sql_file, callback, 0, &zErrMsg);
   if( rc_user != SQLITE_OK || rc_file != SQLITE_OK){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
   }else{
     fprintf(stdout, "Tables created successfully\n");
   }
   sqlite3_close(db_user);
   sqlite3_close(db_file);
   return 1;
}

int user_db_editor::DbPrint(void)
{
  sqlite3 *db_user;
  sqlite3 *db_file;
  char *zErrMsg = 0;
  int rc;

  const char* data = "Callback function called";
  /* Open database */
  rc = sqlite3_open("UsersTable.db", &db_user);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_user));
    exit(0);
  }
  else{
    fprintf(stderr, "Opened database successfully\n");
  }


  /* Create merged SQL statement */
  /*  sql = "DELETE from COMPANY where ID=2; "	*/
  std::string sql = "SELECT * from UsersTable";

   /* Execute SQL statement */
  rc = sqlite3_exec(db_user, sql.c_str(), callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     /*sqlite3_free(zErrMsg);*/
   }
   else{
     fprintf(stdout, "Operation done successfully\n");
   }
   sqlite3_close(db_user);


  /* Open database */
  rc = sqlite3_open("FilesTable.db", &db_file);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_file));
    exit(0);
  }
  else{
    fprintf(stderr, "Opened database successfully\n");
  }


  /* Create merged SQL statement */
  /*  sql = "DELETE from COMPANY where ID=2; "	*/
  sql = "SELECT * from FilesTable";

   /* Execute SQL statement */
  rc = sqlite3_exec(db_file, sql.c_str(), callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
   }
   else{
     fprintf(stdout, "Operation done successfully\n");
   }
   sqlite3_close(db_file);


 return 0;
}
