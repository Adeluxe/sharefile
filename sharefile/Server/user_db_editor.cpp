#include "user_db_editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <cstring>
#include <queue>
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
/*
static int callbackAns(void * NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    //std::string NoUse=azColName[i];
    printf("%s\n",  argv[i] ? argv[i] : "NULL");
    //return argv[i] ? argv[i] : "NULL";
  }
  printf("\n");
  return 0;
}
*/
bool user_db_editor::DbEditor(std::string s, sqlite3 * db)
{
  //sqlite3 *db_user;
  char *zErrMsg = 0;
  int rc;
   /* Execute SQL statement */
  rc = sqlite3_exec(db, s.c_str(), callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
     fprintf(stderr, "%s, SQL error: %s\n", s.c_str(),zErrMsg);
     sqlite3_free(zErrMsg);
   }else{
     fprintf(stdout, "%s successfully\n", s.c_str());
   }
   //sqlite3_close(db_user);
   return 1;
}

std::string user_db_editor::DbGetAnswer(std::string s,int c, sqlite3 * db)
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

        char **pResult=NULL;

        char* errmsg;
        int nRow;
        int nCol;
        int nResult = sqlite3_get_table(db,s.c_str(),&pResult,&nRow,&nCol,&errmsg);
        //std::cout<<"getTable finished\n"<<s.c_str();
          if (nResult != SQLITE_OK)
        {
            std::cout<<errmsg<<std::endl;
            sqlite3_free(errmsg);
            //std::cout<<"NotOK\n";
            return "-1";
        }
       //std::cout<<"R"<<nRow<<"C"<<nCol<<"NotOK\n";
/*        int nIndex = nCol;
        std::string strOut;
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
          if (nRow==0&&nCol==0) return "-1";
       //std::cout<<"iffinished\n";
       std::string Answer=pResult[c];
       //std::cout<<"Answerfinished\n";
        sqlite3_free_table(pResult);

        //std::cout<<strOut<<std::endl;
        return Answer;

        //return "10";
}

std::string user_db_editor::DbGetAnswerThenCloseDb(std::string s,int c, sqlite3 * db)
{
    std::string answer=DbGetAnswer(s,c, db);
    sqlite3_close(db);
    return answer;
}

bool user_db_editor::DbContain(std::string tablename, std::string col, std::string target, sqlite3 * db ){
    char **pResult=NULL;

    char* errmsg;
    int nRow;
    int nCol;
    std::stringstream sqlss;
    sqlss<<"SELECT * from "<<tablename<<" WHERE "<<col<<" Like '"<<target<<"';";
    std::string sql=sqlss.str();

    int nResult = sqlite3_get_table(db,sql.c_str(),&pResult,&nRow,&nCol,&errmsg);
    //std::cout<<"getTable finished\n";
      if (nResult != SQLITE_OK)
    {
        std::cout<<errmsg<<std::endl;
        sqlite3_free(errmsg);
        //std::cout<<"NotOK\n";
        return "-1";
    }

      if (nRow==0&&nCol==0) return 0;
    return 1;

}

int user_db_editor::DbUGetSize(sqlite3 * db_user){

     std::string sql="SELECT max(id) From UsersTable;";
     std::string::size_type sz;   // alias of size_t
     std::string Ans=DbGetAnswer(sql,1, db_user);

     int size = atoi (Ans.c_str());
     return size;
}

int user_db_editor::DbFGetSize(sqlite3 * db_file, std::string DbName){
     std::string sql="SELECT max(id) From ";
     sql.append( DbName);
     sql.append(";");
     std::string::size_type sz;   // alias of size_t
     std::string Ans=DbGetAnswer(sql,1, db_file);
     int size = atoi (Ans.c_str());
     return size;
}

int user_db_editor::DbUGetID(std::string username, sqlite3 * db_user)
{
    std::stringstream idss;
    std::string::size_type sz;   // alias of size_t
    // Create SQL statement
    std::string sql = "SELECT id from UsersTable WHERE username Like '";
    idss<<username<<"';";
    sql.append(idss.str());
    // sql.append(",'");
    std::string ids=DbGetAnswer(sql,1, db_user);
    //DbGetAnswer(sql,1,db_user);
    //int id = std::stoi (ids,&sz);
    int id = atoi(ids.c_str());
    return id;
}

std::string user_db_editor::UserDbGetAt(std::string username, sqlite3 * db_user)
{
    std::stringstream idss;
    //std::string::size_type sz=0;   // alias of size_t
    // Create SQL statement
    std::string sql = "SELECT auth_token from UsersTable WHERE username Like '";
    idss<<username<<"';";
    sql.append(idss.str());
    // sql.append(",'");
    return DbGetAnswer(sql,1, db_user);
}

std::string user_db_editor::DbGetPath( std::string username,  std::string FileName, sqlite3 * db_user)
{
  int user_id=DbUGetID(username, db_user);
  std::string path;
  if(user_id==-1) return 0;
  sqlite3 * db_file;
  std::stringstream DbNamess;
  DbNamess<<"user"<<user_id<<"FilesTable";
  std::string DbNamedb = DbNamess.str();
  DbNamedb.append(".db");
  sqlite3_open(DbNamedb.c_str(), &db_file);
  if(!DbContain(DbNamess.str(), "name", FileName, db_file )){return "not exist";}//not exist
  std::stringstream idss;
  std::string sql;
  idss<< "SELECT path from "<<DbNamess.str()<<" WHERE name Like '"<<FileName<<"';";
  sql.append(idss.str());
  // sql.append(",'");
  return DbGetAnswerThenCloseDb(sql,1, db_file);

}

std::string user_db_editor::DbGetPathS( std::string username,  std::string FileName, sqlite3 * db_user)
{
  int user_id=DbUGetID(username, db_user);
  std::string path;
  if(user_id==-1) return 0;
  sqlite3 * db_file;
  std::stringstream DbNamess;
  DbNamess<<"user"<<user_id<<"FilesTableS";
  std::string DbNamedb = DbNamess.str();
  DbNamedb.append(".db");
  sqlite3_open(DbNamedb.c_str(), &db_file);
  if(!DbContain(DbNamess.str(), "name", FileName, db_file )){return "not exist";}//not exist
  std::stringstream idss;
  std::string sql;
  idss<< "SELECT path from "<<DbNamess.str()<<" WHERE name Like '"<<FileName<<"';";
  sql.append(idss.str());
  // sql.append(",'");
  return DbGetAnswerThenCloseDb(sql,1, db_file);

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

  DbEditor(sql, db_user);
  return 0;
}
  */

bool user_db_editor::DbAddUser( std::string username,std::string auth_token, sqlite3 * db_user)
{
  if(DbUGetID(username, db_user)!=-1) return 0;
  int id = user_db_editor::DbUGetSize(db_user)+1;
  std::stringstream idss;
  idss<<id;
  // Create SQL statement
  std::string sql = "INSERT INTO UsersTable (id,username,auth_token, friend_id) " \
    "VALUES (";
  idss<<",'"<<username<<"','"<<auth_token<<"',',');";
  sql.append(idss.str());
  // sql.append(",'");

  DbEditor(sql, db_user);
  std::stringstream DbName;
  DbName<<"user"<<id<<"F";
  DbFrInitialize(DbName.str());
  DbName<<"ilesTable";
  DbFInitialize(DbName.str());
  DbName<<"S";
  DbFInitialize(DbName.str());
  return 1;
}

int user_db_editor::UserDbLogin(std::string username, std::string password, sqlite3 * db_user)
{
    int id=DbUGetID(username, db_user);
    if(id==-1){
        std::cout<<id<<"user doss not exist\n";
        return -1;//user doss not exist
    }
    std::string auth_token=UserDbGetAt(username, db_user);
    if(strcmp(auth_token.c_str(),password.c_str())!=0){
     std::cout<<id<<"wrong password\n";
     return 0;//wrong password
    }
    std::cout<<id<<"login\n";
    return id;
}

std::string user_db_editor::UserDbGetF(int id, sqlite3 * db_user)
{
    std::stringstream idss;
    //std::string::size_type sz;   // alias of size_t
    // Create SQL statement
    std::string sql = "SELECT friend_id from UsersTable WHERE id Like ";
    idss<<id<<";";
    sql.append(idss.str());
    return DbGetAnswer(sql,1, db_user);
}

bool UserDbContF(int id,std::string friend_id )
{
   std::stringstream idss;
   idss<<","<<id<<",";
   std::size_t found = friend_id.find(idss.str());
   if (found==std::string::npos){return 0;}
   return 1;

}

/*bool user_db_editor::DbContF(int id2, sqlite3 * db_frd ){
 return 0;
}
*/

int user_db_editor::DbAddFriend(std::string username1, std::string username2, sqlite3 * db_user)
{
    int id1=DbUGetID(username1,db_user);
    int id2=DbUGetID(username2,db_user);
    /*std::stringstream friend_id;
    friend_id<<UserDbGetF(id1, db_user);
    if(UserDbContF(id2,friend_id.str() )){return 0;}//exist
    friend_id<<id2<<",' where id ="<<id1<<";";
    std::string sql = " UPDATE UsersTable set friend_id = '";
    sql.append(friend_id.str());
    DbEditor(sql, db_user);
    */
    sqlite3 * db_frd;
    std::stringstream DbNamess;
    DbNamess<<"user"<<id1<<"F";
    std::string DbName=DbNamess.str();
    std::string DbNamedb = DbNamess.str();
    DbNamedb.append(".db");
    sqlite3_open(DbNamedb.c_str(), &db_frd);
    if(DbContain(DbName, "name", username2, db_frd )){return 0;}//exist
    std::string sql = "INSERT INTO ";
    sql.append(DbName);
    sql.append("  (id,name) " \
        "VALUES (");
    std::stringstream id2ss;
    id2ss<<id2;
    sql.append(id2ss.str());
    sql.append(",'");
    sql.append(username2);
    sql.append("');");
    user_db_editor::DbEditor(sql, db_frd);

    return 1;
}

bool user_db_editor::DbAddFileSToF( int user_id, int tofriend_id, std::string FileName, std::string path)
{
  sqlite3 * db_fileS;
  std::stringstream DbNamess;
  DbNamess<<"user"<<tofriend_id<<"FilesTableS";
  std::string DbName=DbNamess.str();
  std::string DbNamedb = DbNamess.str();
  DbNamedb.append(".db");
  sqlite3_open(DbNamedb.c_str(), &db_fileS);
  int id = user_db_editor::DbFGetSize(db_fileS, DbName)+1;
  std::stringstream sqlss;
  // Create SQL statement
  std::string sql;
  sqlss<<"INSERT INTO "<<DbNamess.str()<<" (id,path,name, toFriend_id) " \
    "VALUES ("<< id<<",'"<<path<<"','"<<FileName<<"','"<<user_id<<"');";
  sql.append(sqlss.str());
  // sql.append(",'");
  DbEditor(sql, db_fileS);
  sqlite3_close(db_fileS);
  return 1;
}

bool user_db_editor::DbAddFileS( std::string username, std::string path, std::string FileName, std::string toFriend_name, sqlite3 * db_user)
{
  int user_id=DbUGetID(username, db_user);
  int toFriend_id=DbUGetID(toFriend_name, db_user);
  //if(user_id==-1) return 0;
  sqlite3 * db_file;
  std::stringstream DbNamess;
  DbNamess<<"user"<<user_id<<"FilesTable";
  std::string DbNamedb = DbNamess.str();
  DbNamedb.append(".db");
  sqlite3_open(DbNamedb.c_str(), &db_file);
  if(DbContain(DbNamess.str(), "name", FileName, db_file )){return 0;}//exist
  int id = user_db_editor::DbFGetSize(db_file, DbNamess.str())+1;
  std::stringstream sqlss;
  // Create SQL statement
  std::string sql;
  sqlss<<"INSERT INTO "<<DbNamess.str()<<" (id,path,name, toFriend_id) " \
    "VALUES (";
  sqlss<<id<<",'"<<path<<"','"<<FileName<<"','"<<toFriend_id<<"');";
  sql.append(sqlss.str());
  // sql.append(",'");
  DbEditor(sql, db_file);
  sqlite3_close(db_file);
  DbAddFileSToF( user_id, toFriend_id,FileName,  path);
  return 1;
}

bool user_db_editor::DbAddFile( std::string username, std::string path, std::string FileName, sqlite3 * db_user)
{
  int user_id=DbUGetID(username, db_user);

  if(user_id==-1) return 0;
  sqlite3 * db_file;
  std::stringstream DbNamess;
  DbNamess<<"user"<<user_id<<"FilesTable";
  std::string DbNamedb = DbNamess.str();
  DbNamedb.append(".db");
  sqlite3_open(DbNamedb.c_str(), &db_file);
  if(DbContain(DbNamess.str(), "name", FileName, db_file )){return 0;}//exist
  int id = user_db_editor::DbFGetSize(db_file, DbNamess.str())+1;
  std::stringstream sqlss;
  // Create SQL statement
  std::string sql;
  sqlss<<"INSERT INTO "<<DbNamess.str()<<" (id,path,name) " \
    "VALUES ("<<id<<",'"<<path<<"','"<<FileName<<"');";
  sql.append(sqlss.str());
  // sql.append(",'");
  DbEditor(sql, db_file);
  sqlite3_close(db_file);

  return 1;
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
/*
   const char * sql_file = "CREATE TABLE FilesTable(" \
         "id INT PRIMARY KEY     NOT NULL," \
         "user_id          INT   NOT NULL," \
         "path             TEXT  NOT NULL,"\
         "accessibility    INT     NOT NULL," \
         "created_at       DATETIME," \
         "updated_at       DATETIME );";

*/
   /* Execute SQL statement */
   rc_user = sqlite3_exec(db_user, sql_user, callback, 0, &zErrMsg);
   //rc_file = sqlite3_exec(db_user, sql_file, callback, 0, &zErrMsg);
   if( rc_user != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
   }else{
     fprintf(stdout, "Tables created successfully\n");
   }
   std::string sql = "INSERT INTO UsersTable (id,username,auth_token) " \
       "VALUES (0, 'TEST', 'TEST' ); ";
   user_db_editor::DbEditor(sql, db_user);
   sqlite3_close(db_user);
   sqlite3_close(db_file);
   return 1;
}

int user_db_editor::DbFInitialize(std::string DbName)
{
  sqlite3 * db;

  //char *zErrMsg = 0;
  int  rc;
  /* Open database */
  std::string DbNamedb=DbName;
  DbNamedb.append(".db");
  rc = sqlite3_open(DbNamedb.c_str(), &db);

if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(0);
  }
  //Create SQL statement
  std::string sql = "CREATE TABLE ";
  sql.append(DbName);
  sql.append("("  \
         "id INT PRIMARY KEY     NOT NULL," \
         "name             TEXT  NOT NULL,"\
         "path             TEXT  NOT NULL,"\
         "toFriend_id      INT " \
         "created_at       DATETIME," \
         "updated_at       DATETIME );");


   /* Execute SQL statement */
  DbEditor(sql, db);
   sql = "INSERT INTO ";
   sql.append(DbName);
   sql.append("  (id,name, path) " \
       "VALUES (0, 'TEST' ,'TEST' ); ");
   user_db_editor::DbEditor(sql, db);
   sqlite3_close(db);
   return 1;
}

int user_db_editor::DbFrInitialize(std::string DbName)
{
  sqlite3 * db;

  //char *zErrMsg = 0;
  int  rc;
  /* Open database */
  std::string DbNamedb=DbName;
  DbNamedb.append(".db");
  rc = sqlite3_open(DbNamedb.c_str(), &db);

if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(0);
  }
  //Create SQL statement
  std::string sql = "CREATE TABLE ";
  sql.append(DbName);
  sql.append("("  \
         "id INT  NOT NULL," \
         "name             TEXT  NOT NULL );");


   /* Execute SQL statement */
  DbEditor(sql, db);

   sqlite3_close(db);
   return 1;
}

int user_db_editor::DbPrint(std::string DbName)
{
  sqlite3 *db_user;
  //sqlite3 *db_file;
  char *zErrMsg = 0;
  int rc;
  const char* data = "Callback function called";
  std::string DbNamedb=DbName;
  DbNamedb.append(".db");
  /* Open database */
  rc = sqlite3_open(DbNamedb.c_str(), &db_user);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_user));
    exit(0);
  }
  else{
    fprintf(stderr, "Opened database successfully\n");
  }
  /* Create merged SQL statement */

  std::string sql = "SELECT * from ";
  sql.append(DbName);
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


/*
  rc = sqlite3_open("FilesTable.db", &db_file);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_file));
    exit(0);
  }
  else{
    fprintf(stderr, "Opened database successfully\n");
  }
  //Create merged SQL statement
  sql = "SELECT * from FilesTable";
   //Execute SQL statement
  rc = sqlite3_exec(db_file, sql.c_str(), callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
     fprintf(stderr, "SQL error: %s\n", zErrMsg);
     sqlite3_free(zErrMsg);
   }
   else{
     fprintf(stdout, "Operation done successfully\n");
   }
   sqlite3_close(db_file);
   */
 return 0;
}
