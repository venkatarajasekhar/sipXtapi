@ECHO OFF
REM Copyright 2008 AOL LLC.
REM Licensed to SIPfoundry under a Contributor Agreement.
REM
REM This library is free software; you can redistribute it and/or
REM modify it under the terms of the GNU Lesser General Public
REM License as published by the Free Software Foundation; either
REM version 2.1 of the License, or (at your option) any later version.
REM
REM This library is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
REM Lesser General Public License for more details.
REM
REM You should have received a copy of the GNU Lesser General Public
REM License along with this library; if not, write to the Free Software
REM Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
REM USA. 
REM  
REM Copyright (C) 2004-2006 SIPfoundry Inc.
REM Licensed by SIPfoundry under the LGPL license.
REM
REM Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
REM Licensed to SIPfoundry under a Contributor Agreement.

ECHO.
ECHO ****************************************
ECHO Staging sipXtapi binaries
ECHO ****************************************

SET SOURCE_BASE=..\..\sipXcallLib
SET STAGE_BASE=..\build.Win32

:POPULATE_STAGING
  copy %SOURCE_BASE%\Release\sipXtapi.dll %STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy %SOURCE_BASE%\Debug\sipXtapid.dll .\%STAGE_BASE%\bin\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy %SOURCE_BASE%\Release\sipXtapi.lib .\%STAGE_BASE%\lib\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy %SOURCE_BASE%\Debug\sipXtapid.lib .\%STAGE_BASE%\lib\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  
  copy %SOURCE_BASE%\examples\PlaceCall\Release\PlaceCall.exe .\%STAGE_BASE%\bin\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy %SOURCE_BASE%\\examples\ReceiveCall\Release\ReceiveCall.exe .\%STAGE_BASE%\bin\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT

  copy %SOURCE_BASE%\Release\sipXtapiTest.exe .\%STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT

  copy %SOURCE_BASE%\doc\sipXtapi\html\* .\%STAGE_BASE%\doc\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  
  copy %SOURCE_BASE%\include\tapi\sipXtapi.h .\%STAGE_BASE%\include\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy %SOURCE_BASE%\include\tapi\sipXtapiEvents.h .\%STAGE_BASE%\include\
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  
  copy c:\windows\system32\msvcr71d.dll .\%STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy c:\windows\system32\msvcr71.dll .\%STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT

  copy %SOURCE_BASE%\Debug\sipXtapid.pdb .\%STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy %SOURCE_BASE%\Release\sipXtapi.pdb .\%STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT

  copy %SOURCE_BASE%\Debug\*.map .\%STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT
  copy %SOURCE_BASE%\Release\*.map .\%STAGE_BASE%\bin
  IF NOT "%ERRORLEVEL%"=="0" GOTO ERROR_EXIT

  GOTO DONE

:ERROR_EXIT
ECHO.
ECHO ****************************************
ECHO *** %0 Error detected, aborting ... ***
ECHO ****************************************
ECHO .
exit /b 1

:DONE
exit /b 0