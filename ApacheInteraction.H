/* 
 * Copyright (c) 2005-2011, Guillaume Gimenez <guillaume@blackmilk.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of G.Gimenez nor the names of its contributors may
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL G.GIMENEZ BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors:
 *     * Guillaume Gimenez <guillaume@blackmilk.fr>
 *
 */
#ifndef __APACHEINTERACTION_H__
#define __APACHEINTERACTION_H__

#include "Object.H"
#include <http_config.h>
#include <http_log.h>

#if ( AP_SERVER_MAJORVERSION_NUMBER == 2 ) && (AP_SERVER_MINORVERSION_NUMBER == 2)
# define RAII_APACHE22
#else
APLOG_USE_MODULE(raii);
#endif

namespace raii {

/*! The way to put things in appache error.log
 * \code
 * Logger log("my module");
 * log("A message");
 * \endcode
 * output is
 * \verbatim
[Thu Jan 21 16:34:59 2010] [notice] [client 127.0.0.1] my module: A message
 \endverbatim
 */
class Logger {
  int errorLevel;
  int errorNumber;
  String moduleName;
  String msg;
  String file;
  int line;

  public:
    /*! Create a new logger
     * \param str is the title to prepend to log messages
     */
    Logger(const String& str) : errorLevel(APLOG_NOTICE),
    errorNumber(0),
    moduleName(str),
    msg(""),
    file("n/a"),
    line(0)  {}

    /*! Reset logger to default log level
     */
    Logger& reset() {
      errorLevel=APLOG_NOTICE;
      errorNumber=0;
      file="n/a";
      line=0;
      return *this;
    }
    
    //levels

    /*! set Emergency log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     */
    Logger& emerg(const String& str="")
    { errorLevel=APLOG_EMERG;   if ( !str.empty() ) message(str); return *this; }

    /*! set Emergency log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     */
    Logger& alert(const String& str="")
    { errorLevel=APLOG_ALERT;   if ( !str.empty() ) message(str); return *this; }

    /*! set Critical log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     */
    Logger& critical(const String& str="")
    { errorLevel=APLOG_CRIT;    if ( !str.empty() ) message(str); return *this; }

    /*! set Error log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     */
    Logger& error(const String& str="")
    { errorLevel=APLOG_ERR;     if ( !str.empty() ) message(str); return *this; }

    /*! set Warning log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     */
    Logger& warning(const String& str="")
    { errorLevel=APLOG_WARNING; if ( !str.empty() ) message(str); return *this; }

    /*! set Notice log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     */
    Logger& notice(const String& str="")
    { errorLevel=APLOG_NOTICE;  if ( !str.empty() ) message(str); return *this; }

    /*! set INFO log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     * \note apache default log level is notice, so info messages are usualy not appended to logs
     */
    Logger& info(const String& str="")
    { errorLevel=APLOG_INFO;    if ( !str.empty() ) message(str); return *this; }

    /*! set Debug log level and append a message to log
     * \param str is the message to log
     * \note if message is empty, only set the log level
     * \note apache default log level is notice, so debug messages are usualy not appended to logs
     */
    Logger& debug(const String& str="")
    { errorLevel=APLOG_DEBUG;   if ( !str.empty() ) message(str); return *this; }
    
    //errorNumber
    Logger& number(int err) { errorNumber=err; return *this; }
    
    // source file & line
#define HERE __FILE__, __LINE__
    /*! Set source file and line field for debuging purposes
     * \code
     * Logger log("myapp");
     * log(HERE).debug("message");
     * \endcode
     */
    Logger& operator() (const String& f, int l) {
      file=f;
      line=l;
      return *this;
    }


    /*! append a message to apache's error.log
     * \param str is the message to send to logs
     */
    Logger& message(const String& str) {
      msg=str;
      //  errorLevel < APLOG_NOTICE car c'est plus chiant qu'autre chose d'avoir tout le temps le referrer dans error_log
      // donc le referer est ajouté uniquement pour les cas plus prioritaires que NOTICE
      if ( errorLevel < APLOG_NOTICE && apacheRequest ) {
          ap_log_rerror(file.c_str(),line,
#ifndef RAII_APACHE22
			APLOG_MODULE_INDEX,
#endif
			errorLevel,errorNumber,
			apacheRequest,
			"%s",(moduleName.empty()?msg:(moduleName+": "+msg)).c_str());
      }
      else {
          ap_log_perror(file.c_str(),line,
#ifndef RAII_APACHE22
			APLOG_MODULE_INDEX,
#endif
			errorLevel,errorNumber,
                        NULL,
			"%s",(moduleName.empty()?msg:(moduleName+": "+msg)).c_str());
      }
      //toujours le même
      //file="n/a";
      line=0;
      return *this;
    }

    /*! fast written log form
     * \param str is the message to log
     * \code
     * Logger log("my module");
     * log("A message");
     * \endcode
     */
    Logger& operator() (const String& str) {
      return message(str);
    }
    
   
};





Vector<String> raii_get_tables_keys(apr_pool_t *pool, apr_table_t *t);
}

#ifdef RAII_APACHE22
# undef RAII_APACHE22
#endif

#endif /* __APACHEINTERACTION_H__ */
