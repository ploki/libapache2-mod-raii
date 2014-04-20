#ifndef RAIIRESULTSETIMPLSQLITE_H
#define RAIIRESULTSETIMPLSQLITE_H
#include <unistd.h>

namespace raii {
namespace sql {

class ResultSetImplSQLITE : public ResultSetImpl {

	int cols,rows;
	//char **result;
	Vector<String> result;
	String errMessage;

        static Mutex& getMutex() {
                static Mutex m;
                return m;
        }

	Map<String,int> tupleIndex;
	//interdit
	ResultSetImplSQLITE(const ResultSetImpl& rsi);
	public:

	ResultSetImplSQLITE(SQLDriverSQLITE& driver, const String& q)
		:	ResultSetImpl(driver,q),
			cols(0), rows(0),
			result(/*NULL*/),
			errMessage(/*NULL*/),
			tupleIndex()
			{
                sendQuery(q);
	}

	virtual ~ResultSetImplSQLITE() {

                /*if ( result )
        		sqlite_free_table(result);*/
		/*if ( errMessage )
			free(errMessage);*/
	}

        int get_table(sqlite* db, const char *sql, Vector<String>& result, int& nrow, int& ncolumn, String& errmsg) {
        
                        result.clear();
                        ncolumn=0;
                        nrow=0;
                        errmsg="";

                        const char *tail=NULL;
                        char *zerrmsg=NULL;
                        sqlite_vm *vm=NULL;

                        int ret = sqlite_compile(db, sql, &tail, &vm, &zerrmsg);
                        if ( zerrmsg ) { errmsg = zerrmsg; free(zerrmsg); zerrmsg=NULL; }
                        if ( vm == NULL ) return ret;
                        if ( ret != SQLITE_OK ) {
                                ret = sqlite_finalize(vm,&zerrmsg);
                                if ( zerrmsg ) {
                                        if ( errmsg.empty() )
                                                errmsg = zerrmsg;
                                        else
                                                errmsg = errmsg + ". Additional message is : " + zerrmsg;
                                        free(zerrmsg);
                                        zerrmsg=NULL;
                                }
                                return ret;
                        }
                        if ( *tail != '\0' ) {
                                ret = sqlite_finalize(vm,&zerrmsg);
                                if ( zerrmsg ) { free( zerrmsg ); }
                                throw SQLiteException((String("Multiple statements in query: tail is [")+tail+"]").c_str());
                        }

                        const char **names = NULL;
                        const char **values = NULL;
                        bool namesRead = false;
                        //Logger log(sql);
                        do {
                                int count=5;
                                while ( (ret=sqlite_step(vm,&ncolumn,&values,&names)) == SQLITE_BUSY && --count > 0 ) {
                                        usleep(500000);
                                }
                                //log("cols="+itostring(*ncolumn));
                                if ( ret == SQLITE_ROW ) {
                                
                                        if ( ! namesRead ) {
                                                namesRead = true;
                                                for ( int i = 0 ; i < ncolumn ; ++i ) {
                                                        result.push_back(names[i]);
                                                }
                                        }

                                        for ( int i = 0 ; i < ncolumn ; ++i ) {
                                                result.push_back(values[i]);
                                        }
                                        ++nrow;
                                
                                }
                                else
                                        break;
                        
                        } while ( 1 );
                        //log("lines="+itostring(*nrow));
                        if ( nrow == 0 ) ncolumn=0;
                        ret = sqlite_finalize(vm,&zerrmsg);
                        if ( zerrmsg ) {
                               if ( errmsg.empty() )
                                      errmsg = zerrmsg;
                               else
                                      errmsg = errmsg + ". Additional message is : " + zerrmsg;
                               free(zerrmsg);
                               zerrmsg=NULL;
                        }
                        return ret;
        }

	virtual void sendQuery(const String& query) {

                //Lock l(getMutex());
		cursor=-1;

		int ret, count=5;
		tupleIndex.clear();

		while ( --count > 0 ) {

			if ( ! errMessage.empty() )
				errMessage="";

			ret = get_table((sqlite*)driver.conn,query.c_str(),result,rows,cols,errMessage);

			switch(ret) {
				case SQLITE_LOCKED:
				case SQLITE_BUSY: {
					Logger log("raii::sqlite");
					if ( !errMessage.empty() ) {
						log.warning(errMessage);
                                                errMessage="";
					}
					else
						log.warning("database is locked");
					log.warning(String("query was: ")+query);
					log.warning("retrying later...");
					usleep(500000);
				}
				break;
				case SQLITE_OK: {
					for ( int i = 0 ; i < cols ; ++i ) {
						tupleIndex[result[i]]=i;
					}
					return;
                                }
				break;
				default: {
 					String error("No error reported");
					if ( ! errMessage.empty() ) {
						error=errMessage;
						errMessage="";
					}
					Logger log("raii::sqlite");
					log.error(String("Attempted Query: ") + query);
					throw SQLiteException(( error ).c_str());
				}

			}
		}
		Logger log("raii::sqlite");
		log.error(String("Attempted Query: ") + query);
		throw SQLiteException("database is locked");

	}

	virtual int rowCount() {
		return rows;
	}

	virtual String operator [] (const String& attrName) {

		Map<String,int>::iterator pair = tupleIndex.find(attrName);
		if ( pair == tupleIndex.end() ) {
			throw SQLiteException("Field doesn't exist.");
                }

		if ( cursor >= 0 && cursor < rows )
			return result[ (cursor+1) * cols + pair->second ];
		throw SQLiteException("Cursor out of bound");

	}

};

}
}

#endif

