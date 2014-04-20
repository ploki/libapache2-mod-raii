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
 * ARE DISCLAIMED. IN NO EVENT SHALL G.Gimenez SA BE LIABLE FOR ANY DIRECT,
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
#include "raii.H"
#include <regex.h>
namespace raii {	

	void Regex::compile(Compiled compileFor) const {

		int copts=0;

		if ( compiledFor == compileFor ) return;
		if ( compiledFor != NO ) regfree(&re);
		if ( compileFor == MATCH ) copts |= REG_NOSUB;

		if (m_caseInsensitive) copts |= REG_ICASE;
		if (m_extended) copts |=REG_EXTENDED;
		if (m_newLine) copts |=REG_NEWLINE;

		int err = regcomp(&re,pattern.c_str(),copts);
		if (err) {
			char errBuf[1024];
			regerror(err,&re,errBuf,1024);
			regfree(&re);
			compiledFor=NO;
			throw RegularExpressionException(errBuf);
		}

		compiledFor=compileFor;


	}


	Regex::Regex(const String& pat,bool ext, bool icase, bool newline)
		: pattern(pat), re(), m_extended(ext), m_caseInsensitive(icase), m_newLine(newline), compiledFor(NO) {}
	Regex::Regex(const char* pat,bool ext, bool icase, bool newline)
		: pattern(pat), re(), m_extended(ext), m_caseInsensitive(icase), m_newLine(newline), compiledFor(NO) {}

        Regex::Regex(const Regex& r)
                : pattern(r.pattern), re(), m_extended(r.m_extended), m_caseInsensitive(r.m_caseInsensitive), m_newLine(r.m_newLine), compiledFor(NO) {}

        Regex& Regex::operator=(const Regex& r) {
                pattern=r.pattern;
                m_extended=r.m_extended;
                m_caseInsensitive=r.m_caseInsensitive;
                m_newLine=r.m_newLine;
                compiledFor=NO;
                return *this;
        }

	Regex& Regex::extended(bool e) {
		m_extended=e;
		return *this;
	}
	Regex& Regex::caseInsensitive(bool i) {
		m_caseInsensitive=i;
		return *this;
	}
	Regex& Regex::newLine(bool nl) {
		m_newLine=nl;
		return *this;
	}



	// ex: Regex("plop").matches(str);

	bool Regex::matches(const String& str, bool notbol, bool noteol) const {

		compile(MATCH);
		int err = regexec(&re,str.c_str(),0,NULL,
			              (notbol?REG_NOTBOL:0) | (noteol?REG_NOTEOL:0) );

		switch (err) {
			case REG_NOERROR: return true;
			case REG_NOMATCH: return false;
			default:
				char errBuf[1024];
				regerror(err,&re,errBuf,1024);
				throw RegularExpressionException(errBuf);
		}
		
	}

	
	// ex: Regex("plop").replace("gr8k",str);


	String Regex::replace(const String& repl, const String& str) const {

		compile(REPLACE);
		regmatch_t *subs;

		char	*buf,  /* buf is where we build the replaced string */
			*nbuf, /* nbuf is used when we grow the buffer */
			*walkbuf; /* used to walk buf when replacing backrefs */
		const char *walk; /* used to walk replacement string for backrefs */
		const char *string=str.c_str();
		const char *replace=repl.c_str();
		int buf_len;
		int pos, tmp, string_len, new_l;
		int err;

		string_len = str.size();
	
		/* allocate storage for (sub-)expression-matches */
		subs = (regmatch_t *)calloc(sizeof(regmatch_t),re.re_nsub+1);

		/* start with a buffer that is twice the size of the stringo
		   we're doing replacements in */
		buf_len = 2 * string_len + 1;
		buf = (char*)calloc(buf_len ,sizeof(char) );

		err = pos = 0;
		buf[0] = '\0';
		while (!err) {
			//etrange... l'appel suivant segfault depuis un sighandler...
			err = regexec(&re, &string[pos], re.re_nsub+1, subs, (pos ? REG_NOTBOL : 0));

			if (err && err != REG_NOMATCH) {

				free(subs);
				free(buf);
				char errBuf[1024];
				regerror(err,&re,errBuf,1024);
				throw RegularExpressionException(errBuf);
			}

			if (!err) {
				/* backref replacement is done in two passes:
				   1) find out how long the string will be, and allocate buf
				   2) copy the part before match, replacement and backrefs to buf
				   
				   Jaakko Hyv?tti <Jaakko.Hyvatti@iki.fi>
				   */

			        new_l = strlen(buf) + subs[0].rm_so; /* part before the match */
				walk = replace;
				while (*walk) {
					if (	'\\' == *walk
						&& isdigit((unsigned char)walk[1])
						&& ((unsigned char)walk[1]) - '0' <= int(re.re_nsub)) {

						if (subs[walk[1] - '0'].rm_so > -1 && subs[walk[1] - '0'].rm_eo > -1) {
							new_l += subs[walk[1] - '0'].rm_eo - subs[walk[1] - '0'].rm_so;
						}
						walk += 2;
					} else {
						new_l++;
						walk++;
					}
				}

				if (new_l + 1 > buf_len) {
					buf_len = 1 + buf_len + 2 * new_l;
					nbuf = (char*)malloc(buf_len);
					strcpy(nbuf, buf);
					free(buf);
					buf = nbuf;
				}
				tmp = strlen(buf);

				/* copy the part of the string before the match */
				strncat(buf, &string[pos], subs[0].rm_so);

				/* copy replacement and backrefs */
				walkbuf = &buf[tmp + subs[0].rm_so];
				walk = replace;
				while (*walk) {
					if (	'\\' == *walk
						&& isdigit(walk[1])
						&& walk[1] - '0' <= int(re.re_nsub)) {

						if (subs[walk[1] - '0'].rm_so > -1 && subs[walk[1] - '0'].rm_eo > -1
							/* this next case shouldn't happen. it does. */
							&& subs[walk[1] - '0'].rm_so <= subs[walk[1] - '0'].rm_eo) {

							tmp = subs[walk[1] - '0'].rm_eo - subs[walk[1] - '0'].rm_so;
							memcpy (walkbuf, &string[pos + subs[walk[1] - '0'].rm_so], tmp);
							walkbuf += tmp;
						}
						walk += 2;
					} else {
						*walkbuf++ = *walk++;
					}
				}
				*walkbuf = '\0';

				/* and get ready to keep looking for replacements */
				if (subs[0].rm_so == subs[0].rm_eo) {
					if (subs[0].rm_so + pos >= string_len) {
						break;
					}
					new_l = strlen (buf) + 1;
					if (new_l + 1 > buf_len) {
						buf_len = 1 + buf_len + 2 * new_l;
						nbuf = (char*)calloc(buf_len, sizeof(char));
						strcpy(nbuf, buf);
						free(buf);
						buf = nbuf;
					}
					pos += subs[0].rm_eo + 1;
					buf [new_l-1] = string [pos-1];
					buf [new_l] = '\0';
				} else {
					pos += subs[0].rm_eo;
				}
			} else { /* REG_NOMATCH */
				new_l = strlen(buf) + strlen(&string[pos]);
				if (new_l + 1 > buf_len) {
					buf_len = new_l + 1; /* now we know exactly how long it is */
					nbuf = (char*)calloc(buf_len, sizeof(char));
					strcpy(nbuf, buf);
					free(buf);
					buf = nbuf;
				}
				/* stick that last bit of string on our output */
				strcat(buf, &string[pos]);
			}
		}

	    	/* don't want to leak memory .. */
		free(subs);

		/* whew. */
		String retString(buf);
		free(buf);
		return retString;
	}


	Vector<String> Regex::split(const String& str, bool notbol, bool noteol) const {

		compile(REPLACE);

		Vector<String> vec;

		const char * string = str.c_str();

		int nmatch = 1;
		regmatch_t pmatch[1];
		regoff_t& so=pmatch[0].rm_so;
		regoff_t& eo=pmatch[0].rm_eo;

		int err;
		while (  ( err=regexec(&re,string,nmatch, pmatch, (notbol?REG_NOTBOL:0) | (noteol?REG_NOTEOL:0)) ) == REG_NOERROR ) {

			vec.push_back(String(string,so));
			string+=eo;
			
		}
		if ( err == REG_NOMATCH ) {
			vec.push_back(string);
		}
		else if (err) {
			char errBuf[1024];
			regerror(err,&re,errBuf,1024);
			throw RegularExpressionException(errBuf);
		}
		return vec;		

	}

	Vector<String> Regex::substrs(const String& str, bool notbol, bool noteol) const {

		compile(REPLACE);
		
		Vector<String> vec;

		const char * string = str.c_str();

		int nmatch = 1;
		regmatch_t pmatch[1];
		regoff_t& so=pmatch[0].rm_so;
		regoff_t& eo=pmatch[0].rm_eo;

		int err;
		while (  ( err=regexec(&re,string,nmatch, pmatch, (notbol?REG_NOTBOL:0) | (noteol?REG_NOTEOL:0)) ) == REG_NOERROR ) {

			string+=so;
			vec.push_back(String(string,eo-so));
			string+=eo;
			
		}
		if (err && err != REG_NOMATCH) {
			char errBuf[1024];
			regerror(err,&re,errBuf,1024);
			throw RegularExpressionException(errBuf);
		}
		return vec;		
	}


	Regex::~Regex() {
		regfree(&re);
	}
	
}
