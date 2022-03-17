
mkdir textParser

robocopy headers textParser/headers
robocopy release textParser *.pdb
robocopy release textParser *.lib

pause