/*
 Copyright 2003, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "Logger.h"

int Logger::InitLog(char *filename)
{
	FileName=filename;

	LoggingIsOn=true;

	return OK;
}

int Logger::WriteToLog(const char* text, ...)
{
	if(!LoggingIsOn)
		return OK;

	char buff[1024];
	
	va_list ap;

	if(text==NULL)
		return ERROR;

	va_start(ap, text);
	vsprintf(buff, text, ap);
	va_end(ap);

	time_t plaintime;
	tm *ptm;

	time(&plaintime);
	ptm = localtime(&plaintime);

	ofstream file;

	if(!FileName)
		file.open("LOG.txt",ios::out | ios::app);
	else
		file.open(FileName,ios::out | ios::app);

	if(!file)
		return ERROR;
	else
	{
		file << ptm->tm_mon+1 << '/' << ptm->tm_mday << '/' << ptm->tm_year+1900 <<
			    ' ' << ptm->tm_hour << ':' << ptm->tm_min << ':' << ptm->tm_sec;
		file << "  ";

		file << buff;
		file << endl;
	}

	file.close();

	return OK;
}