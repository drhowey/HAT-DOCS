/*
  Richard Howey
  Institute of Human Genetics, Newcastle University
  HAT-DOCS: HTML and TeX documentation from one common source
*/

#include <iostream>
#include <ostream>
#include <cstdlib>

using namespace std; // initiates the "std" or "standard" namespace
 
#include "main.h"
#include "ProcessHat.h"

void header()
{
	cout << "\nHAT-DOCS: HTML and TeX documentation from one common source\n"
		   << "-----------------------------------------------------------\n";
};

void usage()
{
	header();
	cout << "Author: Richard Howey, Institute of Human Genetics, Newcastle University\n\n"   
		<< "Usage:\n\t ./hatdoc [options] file.hat [bibtexfile.bib]\n\n"		
		<< "Options:\n"
		<< "  -f footer.txt      - HTML footer text for the bottom of each page.\n"
	    << "  -t file.tex        - alternative tex file name.\n"
		<< "  -v                 - verbose output.\n";
};

int main(int argc, char * argv[])
{
	int argcount = 1;
	string fileName;
	string bibFileName = "";
	string footerFileName = "";
	string texFileName = "";
	string option = "";
	bool verbose = false;

	while(argcount < argc && argv[argcount][0] == '-')
    {
		option = argv[argcount];
		if(option == "-f")
		{
			argcount++;
			footerFileName = argv[argcount];	
		}
		else if(option == "-t")
		{
			argcount++;
			texFileName = argv[argcount];	
		}
		else if(option == "-v")
		{
			verbose = true;	
		}
		else
		{
    		cerr << "\nUnrecognised command line switch: " << option << "\n";
			usage();
    		exit(1);
		};

		argcount++;
	};

	if(argcount < argc)
	{
		header();

		fileName = argv[argcount++];
		
		if(argcount < argc) bibFileName = argv[argcount++];

		cout << "Input file: "<< fileName <<"\n";
		if(bibFileName != "") cout << "Bibtex file: " << bibFileName << "\n";
		cout << "\n";

		ProcessHtml pHtml(bibFileName, footerFileName, verbose);
		pHtml.process(fileName);

		ProcessTex pTex(bibFileName, texFileName, verbose);
		pTex.process(fileName);

		
		cout << "Output files:\n";
		pHtml.displayCreatedFiles();
		pTex.displayCreatedFiles();
		cout << "\n";
		pHtml.displayNoSections();
	}
	else
	{
		usage();
	};

};

