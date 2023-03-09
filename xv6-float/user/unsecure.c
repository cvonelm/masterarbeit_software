#include <stdio.h>
#include <stdint.h>
#include <sodium.h>
#include "sqlite3.h"

unsigned char public_key[] = { 0x73, 0xd7, 0x7e, 0x7f, 0x8a, 0x83, 0x72, 0x1a, 0x1c, 0x69, 0x6b, 0x83, 0xe8, 0x49, 0xc0, 0xfe, 0x68, 0x72, 0x1a, 0xf2, 0x47, 0xd8, 0x50, 0x79, 0x83, 0xb5, 0x40, 0x2, 0xec, 0x18, 0xe, 0x4e };

unsigned char secret_key[] = { 0x7, 0x5f, 0x32, 0xe5, 0x1c, 0xd9, 0x93, 0x15, 0xde, 0xe3, 0x72, 0x47, 0xd9, 0x80, 0x9, 0xc4, 0xc2, 0xe3, 0x27, 0x8a, 0x1c, 0x5f, 0xc8, 0x21, 0xfb, 0x73, 0x26, 0xed, 0xab, 0xb0, 0x93, 0x32
 };

extern sqlite3_vfs* sqlite3_demovfs();
int sqlite3_os_init()
{
sqlite3_vfs_register(sqlite3_demovfs(), 1);
  return 0;
}

int sqlite3_os_end()
{
  return 0;
}


sqlite3_stmt *in_stmt, *out_stmt;
  sqlite3 *db;
void *db_init()
{
    char *err_msg = 0;
    int rc = sqlite3_open("test.db", &db);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return (void*)1;
    }
    char *sql_create = "DROP TABLE IF EXISTS amount_owed;" 
                "CREATE TABLE amount_owed(name TEXT, ciphertext BLOB, nonce BLOB);";
    rc = sqlite3_exec(db, sql_create, 0, 0, &err_msg);
 
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return (void *)1;
    } 
    
    char *sql_pragma = "PRAGMA journal_mode = OFF;"; 
    rc = sqlite3_exec(db, sql_pragma, 0, 0, &err_msg);
 
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return (void *)1;
    } 
    char *sql = "SELECT * FROM taxes";
    rc = sqlite3_prepare_v2(db, sql, -1, &in_stmt, 0);

    if (rc != SQLITE_OK ) {

        fprintf(stderr, "SQL error: %d, %s\n",rc, err_msg);


        sqlite3_free(err_msg);
        sqlite3_close(db);

        return (void*)1;
    }

    char *sql_store = "INSERT INTO amount_owed VALUES(?, ?, ?)";
    rc = sqlite3_prepare_v2(db, sql_store, -1, &out_stmt, 0);

    return (void *)0;
}
void *db_close()
{
    sqlite3_finalize(in_stmt);
    sqlite3_finalize(out_stmt);
    sqlite3_close(db);
    return (void*)0;
}
struct data
{
  const char *name;
  const void *ciphertext;
  const void *nonce;
  uint64_t ciphertext_size;
};

struct out_data
{
  unsigned char *name;
  unsigned char nonce[crypto_box_NONCEBYTES];
  unsigned char ciphertext[crypto_box_MACBYTES + 8];
};
struct data d;
struct out_data out_d;
void *db_get()
{
    int step = sqlite3_step(in_stmt);

       if (step == SQLITE_ROW) {
        
      d.ciphertext_size = sqlite3_column_bytes(in_stmt, 1);
      d.ciphertext = sqlite3_column_blob(in_stmt, 1);
      d.nonce = sqlite3_column_blob(in_stmt, 2);
      d.name = sqlite3_column_text(in_stmt, 0);
    return (void*)1;    
  }
      return (void*)0;
}

void *db_store()
{
  sqlite3_bind_text(out_stmt, 0, out_d.name, -1, SQLITE_STATIC);
    sqlite3_bind_blob(out_stmt, 1, out_d.ciphertext, crypto_box_MACBYTES + 8, SQLITE_STATIC);
    sqlite3_bind_blob(out_stmt, 2, out_d.nonce, crypto_box_NONCEBYTES, SQLITE_STATIC);
    int rc = sqlite3_step(out_stmt);
    
    if (rc != SQLITE_DONE) {
        
        printf("execution failed: %s", sqlite3_errmsg(db));
    }

    sqlite3_reset(out_stmt);
}
void *crypto_init()
{
      if (sodium_init() < 0) {
      return (void*)-1;
  }
  return (void*)0;
}

void *crypto_decrypt()
{
 uint64_t decrypted;
 if (crypto_box_open_easy((unsigned char*)&decrypted, d.ciphertext, d.ciphertext_size, d.nonce,
                          public_key, secret_key) != 0) {
     /* message for Bob pretending to be from Alice has been forged! */
   return(void*)1;
 }
        printf("%s: %d", d.name, decrypted);

        decrypted *= 0.1;
  if(crypto_box_easy(d.ciphertext, &decrypted, 8, d.nonce, public_key, secret_key ) != 0)
  {
    return (void*)1;
  }
return (void*)0;
}

int main(void)
{
 

  crypto_init();
  db_init();
  while(1)
  {
            asm volatile (
          ".4byte  0xb400007b"
          );
    if(!db_get())
    {
      break;
    }
    crypto_decrypt();
    db_store();
   asm volatile (
                 ".4byte  0xb600007b"
            );
  }
  db_close();
   
    return 0;
}
