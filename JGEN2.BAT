@echo off
rem JacORB v0.5 IDL to Java translator compiler (12/2/96)
rem batch by Akhil Kumar <akhil@genius.tisl.soft.net>
rem Parses a Java .class file to produce Java
rem code for a JacORB stub and skeleton file.

IF "%1" == "" GOTO USAGE

java jacorb.Generator.Main %1 %2 %3
GOTO EXIT
rem Passing three args means that class will take care if the number of arguments is more than required.

:USAGE
    echo.
    echo Usage: %0 [^-f ^| ^-i] ^<classname.class^>
    echo        generates stub and skeleton for classname
    echo        ^-f     "flatten"  classname.class (signature information) to stdout
    echo        ^-i     derive the public interface for classname.class to classnameInterface.java 
    GOTO EXIT

