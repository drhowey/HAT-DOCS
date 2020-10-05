/*
  Richard Howey
  Institute of Genetic Medicine, Newcastle University
  HAT-DOCS: HTML and TeX documentation from one common source
*/

#include <sstream>
#include <iostream>
#include <ostream>
#include <list>
#include <map>
#include <set>
#include <cstdlib>

using namespace std; // initiates the "std" or "standard" namespace
 
#include "ProcessHat.h"

void ProcessHat::processWord(string & word, ifstream & fileIn, ostream & fileOut)
{	
	if(word.length() >= 2 && word.substr(0, 2) == "**") return;  //ignore lines of *'s

	trimStartWord(word, fileIn, fileOut);
	string endTrim;
	bool formula = false;

	if(word.substr(0, 1) == "$")
	{
		pair<string, string> formulaEndBit = getLatexFormula(word, fileIn, fileOut);
		word = formulaEndBit.first;
		endTrim = formulaEndBit.second;
		
		formula = true;
	};

	if(!formula) endTrim = trimEndWord(word, fileIn, fileOut);

	//single quoted word
	if(word.length() >= 7 && word.substr(0, 3) == "*q*" && word.substr((word.length() - 4)) == "*/q*")
	{
		processQuoteOneWord(word, fileIn, fileOut);
		if(endTrim != "") fileOut << endTrim;
		fileOut << " ";
		return;
	};

	//if(verbose) cout << word << " ";	

	if(formula) processLatexFormula(word, fileIn, fileOut);
	else if(word == "*section*")
	{	
		unsigned int depth = 0;
		processSection(fileIn, fileOut, depth);
	}	
	else if(word == "*section2*")
	{
		subSectionsOnNewPage = true;
		unsigned int depth = 0;
		processSection(fileIn, fileOut, depth);
		subSectionsOnNewPage = false;
	}
	else if(word == "*webpage*") processWebpage(fileIn, fileOut);
	else if(word == "*comment*" || word == "*title*" || word == "*subtitle*" || word == "*author*" || word == "*address*" || word == "*date*" || word == "*abstract*" || word == "*stylefile*" || word == "*logo*" || word == "*logowidth*" ) processComment(fileIn);
	else if(word == "*html*") processHtml(fileIn, fileOut);
	else if(word == "*tex*") processTex(fileIn, fileOut);	
	else if(word == "*codeexample*") processCodeExample(fileIn, fileOut);
	else if(word == "*codeexample-small*") processCodeExampleSmall(fileIn, fileOut);
	else if(word == "*list*") processList(fileIn, fileOut, false);
	else if(word == "*numlist*") processList(fileIn, fileOut, true);
	else if(word == "*table*") processTable(fileIn, fileOut);
	else if(word == "*tabler*") processTable(fileIn, fileOut, 1);
	else if(word == "*tablel*") processTable(fileIn, fileOut, 2);
	else if(word == "*tablec*") processTable(fileIn, fileOut, 3);
	else if(word == "*tableropt*") processTable(fileIn, fileOut, 1, true);
	else if(word == "*tablelopt*") processTable(fileIn, fileOut, 2, true);
	else if(word == "*tablecopt*") processTable(fileIn, fileOut, 3, true);
	else if(word == "*ref*") processRef(fileIn, fileOut);
	else if(word == "*figref*") processFigRef(fileIn, fileOut);
	else if(word == "*figure*") processFigure(fileIn, fileOut);
	else if(word == "*star*") fileOut << "*";
	else if(word == "*dollar*") fileOut << "$";
	else if(word == "*code*") processCode(fileIn, fileOut, true);
	else if(word == "*/code*") processCode(fileIn, fileOut, false);	
	else if(word == "*b*") processBold(word, fileIn, fileOut, true);
	else if(word == "*/b*") processBold(word, fileIn, fileOut, false);
	else if(word == "*i*") processItalic(word, fileIn, fileOut, true);
	else if(word == "*/i*") processItalic(word, fileIn, fileOut, false);
	else if(word == "*u*") processUnderline(word, fileIn, fileOut, true);
	else if(word == "*/u*") processUnderline(word, fileIn, fileOut, false);	
	else if(word == "*q*") processQuote(fileIn, fileOut, true);
	else if(word == "*/q*") processQuote(fileIn, fileOut, false);
	else if(word == "*cite*") processCite(fileIn, fileOut, true);
	else if(word == "*/cite*") processCite(fileIn, fileOut, false);
	else if(word == "*percent*") processPercent(fileIn, fileOut);
	else if(word == "*input*") processInput(fileIn, fileOut);
	else
	{
		fileOut << word;
	};

	if(endTrim != "") fileOut << endTrim;
	
	//a space if not after a beginning or before the end of a command
	if(!nextWordIsEndWord(fileIn) &&
		((word.substr(0, 1) != "*" && word.substr((word.length() - 1), 1) != "*")
		||
		(word == "*percent*" || word == "*star*")
		||
		(word == "*q*" && endTrim != "")
		||
		(word.length() >= 2 && word.substr(0, 2) == "*/"))) fileOut << " ";
};

void ProcessHtml::processWord(string & word, ifstream & fileIn, ostream & fileOut, bool replaceChars)
{
	if(replaceChars && !processingWebpage) replaceSpecialChars(word);

	ProcessHat::processWord(word, fileIn, fileOut);
};

void ProcessHat::process(string & filename)
{
	string fileOutName;
	if(texFileName != "") fileOutName = texFileName;
	else fileOutName = getFileOutName(filename);

	ifstream fileIn(filename.c_str());
	ofstream fileOut(fileOutName.c_str());

	if(!fileIn.is_open())
	{
		cerr<<"Cannot read file: "<<filename<< "!\n";
		exit(1);
	};

	if(verbose) cout << "\n\nProcessing TEX: " << filename << "\n";
	if(verbose) cout << "Adding title data\n";
	addTitleData(filename, fileOut);
	unsigned int sectionCount = 1;
	unsigned int figureNumber = 1;
	if(verbose) cout << "Adding section data\n";
	addSectionData(filename, fileOut, sectionCount, figureNumber);

	filesCreated.push_back(fileOutName);
	
	processFile(fileIn, fileOut);
};

void ProcessHtml::process(string & filename)
{	
	ifstream fileIn(filename.c_str());
	ofstream fileOut;

	if(!fileIn.is_open())
	{
		cerr<<"Cannot read file: "<<filename<< "!\n";
		exit(1);
	};

	if(verbose) cout << "Processing HTML: " << filename << "\n\n";

	addTitleData(filename, fileOut);
	unsigned int sectionCount = 1;
	unsigned int figureNumber = 1;
	addSectionData(filename, fileOut, sectionCount, figureNumber);
	addWebpageData(filename, fileOut);
	addReferences(filename, fileOut);
	addReferencesWebpage(fileIn, fileOut);

	string word;
	fileIn >> word;
		
	do{
		if(verbose) cout << word << " ";
		processWord(word, fileIn, fileOut);
		if(!fileIn.eof()) fileIn >> word;

	}while(!fileIn.eof());

	
	fileIn.close();
	fileOut.close();

};

string ProcessHtml::getFileOutName(string & filename)
{
	return "index.html";
};

string ProcessTex::getFileOutName(string & filename)
{
	unsigned int length = filename.length();
	return filename.substr(0,length-4) + ".tex";
};

void ProcessHat::processFile(ifstream & fileIn, ofstream & fileOut)
{
	
	header(fileIn, fileOut);
	string word;
	fileIn >> word;

	do{
		if(verbose) cout << word << "\n";
		processWord(word, fileIn, fileOut);
		if(!fileIn.eof()) fileIn >> word;

	}while(!fileIn.eof());

	if(verbose) cout << "Adding footer\n";	
	footer(fileIn, fileOut);

	fileIn.close();
	fileOut.close();
};

void ProcessHat::processInputFile(ifstream & fileIn, ostream & fileOut)
{	
	string word;
	fileIn >> word;

	do{
			
		processWord(word, fileIn, fileOut);
		fileIn >> word;

	}while(!fileIn.eof());

	fileIn.close();
};

void ProcessHat::processInput(ifstream & fileIn, ostream & fileOut)
{
	string filename;
	fileIn >> filename;

	ifstream newInputFileIn(filename.c_str());

	if(!newInputFileIn.is_open())
	{
		cerr<<"Cannot read input file: "<<filename<< "!\n";
		exit(1);
	};

	processInputFile(newInputFileIn, fileOut);

	fileIn >> filename;
	if(filename != "*/input*")
	{
		cerr<<"Input command not ended properly for file: "<<filename<< "!\n";
		exit(1);
	};

	newInputFileIn.close();
};

void ProcessHtml::getSectionNameAndTitle(ifstream & fileIn, ostream & fileOut, string & sectionName, string & sectionTitle)
{
	string word;

	fileIn >> word;
	if(!(word == "*section-name*" || word == "*subsection-name*" || word == "*subsubsection-name*"))
	{
		cerr << "Incorrect word: "<<word<<"\n";
		cerr << "A section name must follow the beginning of a section!\n";
		//fileOut.close();
		exit(1);
	};

	sectionName = getText(fileIn);
	
	fileIn >> word;

	bool specialTex = false;

	//skip forward to special tex title
	if(word == "*section-title-tex*" || word == "*subsection-title-tex*" || word == "*subsubsection-title-tex*")
	{
		specialTex = true;
		while(word != "*/section-title-tex*" && word != "*/subsection-title-tex*" && word != "*/subsubsection-title-tex*") fileIn >> word;

		fileIn >> word;
	};

	bool specialHtml = false;
	if((word == "*section-title-html*" || word == "*subsection-title-html*" || word == "*subsubsection-title-html*"))
	{
		specialHtml = true;
	};

	if(!(word == "*section-title*" || word == "*subsection-title*" || word == "*subsubsection-title*"
		|| word == "*section-title-html*" || word == "*subsection-title-html*" || word == "*subsubsection-title-html*"))
	{
		cerr << "Incorrect word: "<<word<<"\n";
		cerr << "A section title must follow the section name!\n";
		//fileOut.close();
		exit(1);
	};

	sectionTitle = getText(fileIn);

	//skip any special tex title
	if(specialHtml && !specialTex)
	{
		while(word != "*/section-title-tex*" && word != "*/subsection-title-tex*" && word != "*/subsubsection-title-tex*") fileIn >> word;

		//fileIn >> word;
	};
};

void ProcessTex::getSectionNameAndTitle(ifstream & fileIn, ostream & fileOut, string & sectionName, string & sectionTitle)
{
	string word;

	fileIn >> word;
	if(!(word == "*section-name*" || word == "*subsection-name*" || word == "*subsubsection-name*"))
	{
		cerr << "Incorrect word: "<<word<<"\n";
		cerr << "A section name must follow the beginning of a section!\n";
		//fileOut.close();
		exit(1);
	};

	sectionName = getText(fileIn);
	
	fileIn >> word;

	bool specialHtml = false;
	//skip formward to special tex title
	if(word == "*section-title-html*" || word == "*subsection-title-html*" || word == "*subsubsection-title-html*")
	{
		specialHtml = true;
		while(word != "*/section-title-html*" && word != "*/subsection-title-html*" && word != "*/subsubsection-title-html*") fileIn >> word;

		fileIn >> word;
	};

	bool specialTex = false;
	if((word == "*section-title-tex*" || word == "*subsection-title-tex*" || word == "*subsubsection-title-tex*"))
	{
		specialTex = true;
	};

	if(!(word == "*section-title*" || word == "*subsection-title*" || word == "*subsubsection-title*"
		|| word == "*section-title-tex*" || word == "*subsection-title-tex*" || word == "*subsubsection-title-tex*"))
	{
		cerr << "Incorrect word: "<<word<<"\n";
		cerr << "A section title must follow the section name!\n";
		//fileOut.close();
		exit(1);
	};

	sectionTitle = getText(fileIn);

	//skip over any special tex title
	if(specialTex && !specialHtml)
	{
		while(word != "*/section-title-html*" && word != "*/subsection-title-html*" && word != "*/subsubsection-title-html*") fileIn >> word;

		//fileIn >> word;
	};
};

void ProcessHat::getWebpageNameAndTitle(ifstream & fileIn, ostream & fileOut, string & webpageName, string & webpageTitle)
{
	string word;

	fileIn >> word;
	if(!(word == "*webpage-name*"))
	{
		cerr << "Incorrect word: "<<word<<"\n";
		cerr << "A webpage name must follow the beginning of a webpage!\n";
		//fileOut.close();
		exit(1);
	};

	webpageName = getText(fileIn);
	
	fileIn >> word;
	if(!(word == "*webpage-title*"))
	{
		cerr << "Incorrect word: "<<word<<"\n";
		cerr << "A webpage title must follow the webpage name!\n";
		//fileOut.close();
		exit(1);
	};

	webpageTitle = getText(fileIn);

};

void ProcessHat::addWebpageData(string & filename, ostream & fileOut)
{
	ifstream fileWebpagesIn(filename.c_str());

	if(!fileWebpagesIn.is_open())
	{
		cerr<<"Cannot read file: "<<filename<< "!?\n";
		exit(1);
	};

	string word, webpageName, webpageTitle;
	Webpage * webpage;

	fileWebpagesIn >> word;

	do{

		if(word == "*webpage*")
		{
			getWebpageNameAndTitle(fileWebpagesIn, fileOut, webpageName, webpageTitle);
			
			webpage = new Webpage(webpageName, webpageTitle);

			orderedWebpages.push_back(webpage);
		};

		fileWebpagesIn >> word;

	}while(!fileWebpagesIn.eof());

	fileWebpagesIn.close();
};

string getField(const string & fieldLine)
{
	unsigned int startPos = 0, endPos = 0;
	unsigned int length = fieldLine.length();
	unsigned int pos = 0;
	bool pastEqualSign = false;

	//find the start pos
	do{
		if(fieldLine.substr(pos, 1) == "=") pastEqualSign = true;
		else if(pastEqualSign && fieldLine.substr(pos, 1) != " " && fieldLine.substr(pos, 1) != "{")
		{
//cout << pastEqualSign << " pastEqualSign--" << pos << " pos\n";
			startPos = pos;
			break;
		};

		pos++;
	}while(pos < length);

	pos = length - 2; //was length -1, the length was/is 1 too long? Will always be at least one unwanted char at end so - 2 is ok
//cout << startPos << " " << pos << " --"<<fieldLine<<"\n";
	//find the end pos
	do{

		if(fieldLine.substr(pos, 1) != " " && fieldLine.substr(pos, 1) != "}" && fieldLine.substr(pos, 1) != ",")
		{
			endPos = pos;
			break;
		};

		pos--;
	}while(pos >= 0);

	return fieldLine.substr(startPos, (endPos-startPos+1));
};

void changeJournalName(Citation * citation)
{

	if(citation->journal == "ajhg") citation->journal = "Am J Hum Genet";
	else if(citation->journal == "ajmg") citation->journal = "Am J  Med Genet";
	else if(citation->journal == "ajmga") citation->journal = "Am J Med Genet A";
	else if(citation->journal == "annals") citation->journal = "Ann Hum Genet";             
	else if(citation->journal == "bmcs") citation->journal = "Biometrics";
	else if(citation->journal == "bmka") citation->journal = "Biometrika";
	else if(citation->journal == "ejhg") citation->journal = "Eur J  Hum Genet";
	else if(citation->journal == "genepi") citation->journal = "Genet Epidemiol";
	else if(citation->journal == "genom") citation->journal = "Genomics";
	else if(citation->journal == "genet") citation->journal = "Genetics";
	else if(citation->journal == "humgen") citation->journal = "Hum Genet";
	else if(citation->journal == "humher") citation->journal = "Hum Hered";
	else if(citation->journal == "hummol") citation->journal = "Hum Molec Genet";
	else if(citation->journal == "jasa") citation->journal = "Journal of the American Statistical Association";
	else if(citation->journal == "jci") citation->journal = "Journal of Clinical Immunity";
	else if(citation->journal == "jrssb") citation->journal = "Journal of the Royal Statistical Society, Series B";
	else if(citation->journal == "lancet") citation->journal = "Lancet";
	else if(citation->journal == "natgenet") citation->journal = "Nat Genet";
	else if(citation->journal == "nature") citation->journal = "Nature";
	else if(citation->journal == "natrevgenet") citation->journal = "Nat Rev Genet";
	else if(citation->journal == "science") citation->journal = "Science";
	else if(citation->journal == "statmed") citation->journal = "Statistics in Medicine";
	else if(citation->journal == "statsci") citation->journal = "Statistical Science";
	else if(citation->journal == "TPB") citation->journal = "Theoretical Population Biology";
	
	if(citation->publisher == "Chapman \\& Hall/CRC") citation->publisher = "Chapman &amp; Hall/CRC";
	else if(citation->publisher == "Chapman \\& Hall") citation->publisher = "Chapman &amp; Hall";
	else if(citation->publisher == "Texts in Statistical Science, Chapman \\& Hall/CRC (US)") citation->publisher = "Texts in Statistical Science, Chapman &amp; Hall/CRC (US)";
	else if(citation->publisher == "Chapman \\& Hall/CRC, London") citation->publisher = "Chapman &amp; Hall/CRC, London";
	else if(citation->publisher == "John Wiley \\& Sons, New York") citation->publisher = "John Wiley &amp; Sons, New York";
	

};

void changeAuthorsAndRefName(Citation * citation)
{
	string ans;
	string refName = "";
	string authors = citation->authors;
	string orderName = "";

	map<unsigned int, string> listAuthors;
	unsigned int count = 1;
	char commaChar = ',';

	//cout << authors << "\n";
	unsigned int pos = 0;
	unsigned int lastPos = 0;

	//get all authors of citation
	do{		
		if(authors.substr(pos, 5) == " and ")
		{
			listAuthors[count] = authors.substr(lastPos, (pos-lastPos + 1));
			lastPos = pos + 5;
			count++;
		};
			
		pos++;		
	}while(pos < (authors.length() - 5));

	//add the last name
	listAuthors[count] = authors.substr(lastPos);

	string initials = "";
	string aInitial;
	string lastName = "";
	unsigned int commaPos;
	unsigned int lastSpacePos = 0;
	unsigned int prevSpacePos = 0;

	for(map<unsigned int, string>::const_iterator a = listAuthors.begin(); a != listAuthors.end(); ++a)
	{
		//cout << a->first << " " <<a->second <<"\n";
		lastName = "";
		initials = "";

		//get the last name and initials
		//check if there is a comma
		pos = 0;
		commaPos = 0;
		do{
			if(a->second.substr(pos, 1) == ",")
			{
				commaPos = pos;
				break;
			};			
	
			pos++;
		}while(pos < a->second.length());

		//if name has comma get last name and initials
		if(commaPos != 0)
		{
			lastName = a->second.substr(0, commaPos);

			pos = commaPos;
			do{
				if(a->second.substr(pos, 1) == " " && a->second.substr((pos + 1),1) != " " &&
					(pos == (a->second.length() - 2) || a->second.substr((pos + 2), 1) == " "))
				{
					initials.append(a->second.substr((pos + 1),1));
					initials.append(". ");
				}
				else if( //"J-B"					
					a->second.substr(pos, 1) == " " && a->second.substr((pos + 1),1) != " " &&
					a->second.length() - pos >= 3 && a->second.substr((pos + 2), 1) == "-" &&
					(pos == (a->second.length() - 4) || a->second.substr((pos + 4), 1) == " ")					
					)
				{
					initials.append(a->second.substr((pos + 1),3));
					initials.append(". ");
				};

				pos++;
			}while(pos <= (a->second.length() - 2));

		}
		else
		{

			stringstream getNameAndInitials(a->second);
			list<string> someStrings;
			char space = ' ';
			string aString;

			//get initials and last name
			do{
				getline(getNameAndInitials, aString, space);
				someStrings.push_back(aString);				
			}while(!getNameAndInitials.eof());

			lastName = *(someStrings.rbegin());

			for(list<string>::const_iterator ss = someStrings.begin(); ss != someStrings.end(); )
			{
				aString = *ss;
				ss++;
				if(ss != someStrings.end())
				{
					initials.append(aString);
					if((aString).size() == 1) initials.append(". ");
					else initials.append(" ");
				};

			};

		};

		if(a->first == 1)
		{
			orderName.append(lastName);
			orderName.append(citation->year);
			refName.append(lastName);
		};

		if(listAuthors.size() == 2 && a->first == 2)
		{
			string secondAuthor = " and " + lastName;
			refName.append(secondAuthor);
		};

		if(a->first >1 && a->first == listAuthors.size()) ans.append(" and ");
		else if(a->first > 1)
		{
			if(ans.length() >= 1 && ans.substr(ans.length()-1, 1)==" ") ans = ans.substr(0, ans.length()-1);
			ans.append(", ");
		};

		ans.append(initials);
		ans.append(lastName);
	};

	
	if(listAuthors.size() > 2) refName.append(" et al.");
	refName.append(" (");
	refName.append(citation->year);
	refName.append(")");
	citation->refName = refName;
	citation->orderName = orderName;
	citation->authors = ans;

	if(citation->note != "") citation->refName = citation->authors;
};

void ProcessHtml::addCitation(const string & citeName)
{
	//check if citation already exists
	map<string, Citation *>::const_iterator c = citations.find(citeName);
	if(c != citations.end()) return;

	Citation * citation = new Citation();
	citation->name = citeName;
	citations[citeName] = citation;

	//open a new bib file
	ifstream fileBibIn(bibFileName.c_str());

	if(!fileBibIn.is_open())
	{
		cerr<<"Cannot read file: "<<bibFileName<< "!?\n";
		exit(1);
	};
	
	string wordBib;
	bool citeFound = false;
	//find ref in file
	do{
		fileBibIn >> wordBib;	

		if(((wordBib.length() >= (citeName.length() + 9)) && wordBib.substr(9, citeName.length()) == citeName)
			|| (wordBib.length() >= (citeName.length() + 6)) && wordBib.substr(6, citeName.length()) == citeName)
		{
			citeFound = true;
			break;
		};

	}while(!fileBibIn.eof());

	if(!citeFound)
	{
		cerr << "Warning: citation "<< citeName <<" not found in file "<<bibFileName<<"!\n";
		return;
	};

	string aLine;	
	char returnChar = '\n';
	set<string> citeLines;

	//get all lines of citation
	do{		
		getline(fileBibIn, aLine, returnChar);
		if(aLine.substr(0, 1) == "}") break;
		citeLines.insert(aLine);
	}while(!fileBibIn.eof());

	//add fields of the citation
	unsigned int length;
	for(set<string>::const_iterator c = citeLines.begin(); c != citeLines.end(); ++c)
	{
		length = (*c).length();
		if(length >= 6 && (c->substr(0, 6) == "author" || c->substr(0, 6) == "AUTHOR") ) citation->authors = getField(*c);
		else if(length >= 5 && (c->substr(0, 5) == "title" || c->substr(0, 5) == "TITLE") ) citation->title = getField(*c);
		else if(length >= 7 && (c->substr(0, 7) == "journal" || c->substr(0, 7) == "JOURNAL") ) citation->journal = getField(*c);
		else if(length >= 6 && (c->substr(0, 6) == "volume" || c->substr(0, 6) == "VOLUME") ) citation->volume = getField(*c);
		else if(length >= 6 && (c->substr(0, 6) == "number" || c->substr(0, 6) == "NUMBER") ) citation->number = getField(*c);
		else if(length >= 4 && (c->substr(0, 4) == "year" || c->substr(0, 4) == "YEAR") ) citation->year = getField(*c);
		else if(length >= 3 && (c->substr(0, 3) == "url" || c->substr(0, 3) == "URL") ) citation->url = getField(*c);
		else if(length >= 5 && (c->substr(0, 5) == "pages" || c->substr(0, 5) == "PAGES") ) citation->pages = getField(*c);
		else if(length >= 6 && (c->substr(0, 6) == "editor" || c->substr(0, 6) == "EDITOR") ) citation->editor = getField(*c);
		else if(length >= 7 && (c->substr(0, 7) == "edition" || c->substr(0, 7) == "EDITION")) citation->edition = getField(*c);
		else if(length >= 9 && (c->substr(0, 9) == "publisher" || c->substr(0, 9) == "PUBLISHER")) citation->publisher = getField(*c);
		else if(length >= 4 && (c->substr(0, 4) == "note" || c->substr(0, 4) == "NOTE") ) citation->note = getField(*c);
	};

	changeJournalName(citation);
	changeAuthorsAndRefName(citation);

	fileBibIn.close();
};

void ProcessHtml::addReferences(string & filename, ostream & fileOut)
{
	ifstream fileCiteIn(filename.c_str());

	if(!fileCiteIn.is_open())
	{
		cerr<<"Cannot read file: "<<filename<< "!?\n";
		exit(1);
	};

	
	string name, word;
	
	

	do{
		fileCiteIn >> word;

		if(word == "*input*")
		{
			string newFilename;
			fileCiteIn >> newFilename;
	
			addReferences(newFilename, fileOut);
			
			fileCiteIn >> newFilename;
			
			if(newFilename != "*/input*")
			{
				cerr<<"Input command not ended properly for file: "<<filename<< "!\n";
				exit(1);
			};
		}
		else if(word.length() >= 6 && word.substr((word.length()-6), 6) == "*cite*")
		{
			fileCiteIn >> name;				
			addCitation(name);
		
		};

	}while(!fileCiteIn.eof());

	fileCiteIn.close();
};

void ProcessHat::addTitleData(string & filename, ostream & fileOut)
{
	ifstream fileTitleIn(filename.c_str());

	if(!fileTitleIn.is_open())
	{
		cerr<<"Cannot read file: "<<filename<< "!?\n";
		exit(1);
	};

	string word;
	
	fileTitleIn >> word;

	do{

		if(word == "*title*")
		{
			title = getText(fileTitleIn);			
		}
		else if(word == "*author*")
		{
			author = getText(fileTitleIn);
		}
		else if(word == "*address*")
		{
			address = getText(fileTitleIn);
		}
		else if(word == "*abstract*")
		{
			abstract = getText(fileTitleIn);
		}
		else if(word == "*date*")
		{
			date = getText(fileTitleIn);
		}
		else if(word == "*subtitle*")
		{
			subtitle = getText(fileTitleIn);
		}
		else if(word == "*stylefile*")
		{
			styleFile = getText(fileTitleIn);
		}
		else if(word == "*logo*")
		{
			logo = getText(fileTitleIn);			
		}
		else if(word == "*logowidth*")
		{
			fileTitleIn >> logowidth;			
		};

		fileTitleIn >> word;

	}while(!fileTitleIn.eof());

	fileTitleIn.close();
};

void ProcessHat::addSectionData(string & filename, ostream & fileOut, unsigned int & sectionCount, unsigned int & figureNo)
{
	unsigned int sectionDepth = 0;
	ifstream fileSectionsIn(filename.c_str());
	
	if(!fileSectionsIn.is_open())
	{
		cerr<<"Cannot read file: "<<filename<< "!?\n";
		exit(1);
	};

	string word, sectionName, sectionNumber, sectionTitle;
	bool newPageForSubsections;	
	list<Section * > subsections;
	Section * section;
	string sectionUpperName = "";
	string figRefName, figName;

	fileSectionsIn >> word;

	do{
		if(word == "*input*")
		{
			string newFilename;
			fileSectionsIn >> newFilename;

			addSectionData(newFilename, fileOut, sectionCount, figureNo);

			fileSectionsIn >> newFilename;
			if(newFilename != "*/input*")
			{
				cerr<<"Input command not ended properly for file: "<<filename<< "!\n";
				exit(1);
			};
		}
		else if(word == "*section*" || word == "*section2*")
		{
			getSectionNameAndTitle(fileSectionsIn, fileOut, sectionName, sectionTitle);
			sectionNumber = getSectionNumber(sectionUpperName, sectionCount, sectionDepth);
			newPageForSubsections = (word == "*section2*");

			//sectionStarts = 1; sectionEnds = 0;
			subsections = getSubsections(sectionName, sectionNumber, fileSectionsIn, fileOut, newPageForSubsections, figureNo);

			section = new Section(sectionNumber, sectionName, sectionTitle, subsections, sectionUpperName, newPageForSubsections);

			sections[sectionName] = section;
			orderedSections.push_back(section);

			sectionCount++;
		}
		else if(word == "*label*")
		{
			ostringstream aStringStream;
			aStringStream << figureNo;

			figRefName = getText(fileSectionsIn);
			figName = aStringStream.str();

			figures[figRefName] = figName;
			figureNo++;
		};

		fileSectionsIn >> word;

	}while(!fileSectionsIn.eof());

	fileSectionsIn.close();
};

list<Section * > ProcessHat::getSubsections(string & sectionUpperName, string & sectionNumberUpper, ifstream & fileIn, ostream & fileOut, bool & newPageForSubsections, unsigned int & figureNo, unsigned int subsectionCount)
{
	list<Section * > subsections, subsubsections, inputSubsections;
	unsigned int sectionDepth = 1;//depth + 1;	
	
	
	string word, sectionName, sectionTitle, sectionNumber;
	string figRefName, figName;
	Section * section;

	fileIn >> word;
	
	do{
		if(word == "*input*")
		{
			string filename;
			fileIn >> filename;

			//get input subsections
			inputSubsections = getSubsections(sectionUpperName, sectionNumberUpper, fileIn, fileOut, newPageForSubsections, figureNo, subsectionCount);

			fileIn >> filename;
			if(filename != "*/input*")
			{
				cerr<<"Input command not ended properly for file: "<<filename<< "!\n";
				exit(1);
			};

			//add input subsections to the list
			for(list<Section * >::const_iterator is = inputSubsections.begin(); is != inputSubsections.end(); ++is)
			{
				subsections.push_back(*is);
			};
		}
		else if(word == "*subsection*")
		{	
			string sectionUpperName2 = "";
			if(!newPageForSubsections) sectionUpperName2 = sectionUpperName;

			getSectionNameAndTitle(fileIn, fileOut, sectionName, sectionTitle);
			sectionNumber = getSectionNumber(sectionNumberUpper, subsectionCount, sectionDepth);

			subsubsections = getSubsubsections(sectionUpperName, sectionNumber, fileIn, fileOut, figureNo, sectionDepth);
			
			section = new Section(sectionNumber, sectionName, sectionTitle, subsubsections, sectionUpperName2, false);

			sections[sectionName] = section;
			subsections.push_back(section);
			
			subsectionCount++;
		}
		else if(word == "*/section*")
		{			
			return subsections;	
		}
		else if(word == "*label*")
		{
			ostringstream aStringStream;
			aStringStream << figureNo;

			figRefName = getText(fileIn);
			figName = aStringStream.str();

			figures[figRefName] = figName;
			figureNo++;
		};

		fileIn >> word;
	}while(!fileIn.eof());

	return subsections;
};

list<Section * > ProcessHat::getSubsubsections(string & sectionUpperUpperName, string & sectionNumberUpper, ifstream & fileIn, ostream & fileOut, unsigned int & figureNo, unsigned int subsubsectionCount)
{
	list<Section * > subsubsections, nosubs, inputSubsubsections;
	unsigned int sectionDepth = 2;//depth + 1;	
	
	string word, sectionName, sectionTitle, sectionNumber;
	string figRefName, figName;
	Section * section;

	fileIn >> word;
	
	do{
		if(word == "*input*")
		{
			string filename;
			fileIn >> filename;

			//get input subsubsections
			inputSubsubsections = getSubsubsections(sectionUpperUpperName, sectionNumberUpper, fileIn, fileOut, figureNo, subsubsectionCount);

			fileIn >> filename;
			if(filename != "*/input*")
			{
				cerr<<"Input command not ended properly for file: "<<filename<< "!\n";
				exit(1);
			};

			//add input subsubsections to the list
			for(list<Section * >::const_iterator iss = inputSubsubsections.begin(); iss != inputSubsubsections.end(); ++iss)
			{
				subsubsections.push_back(*iss);
			};
		}
		if(word == "*subsubsection*")
		{			
			getSectionNameAndTitle(fileIn, fileOut, sectionName, sectionTitle);
			sectionNumber = getSectionNumber(sectionNumberUpper, subsubsectionCount, sectionDepth);

			section = new Section(sectionNumber, sectionName, sectionTitle, nosubs, sectionUpperUpperName, false);

			sections[sectionName] = section;
			subsubsections.push_back(section);
			
			subsubsectionCount++;
		}
		else if(word == "*/subsection*")
		{			
			return subsubsections;	
		}
		else if(word == "*label*")
		{
			ostringstream aStringStream;
			aStringStream << figureNo;

			figRefName = getText(fileIn);
			figName = aStringStream.str();

			figures[figRefName] = figName;
			figureNo++;
		};

		fileIn >> word;
	}while(!fileIn.eof());

	return subsubsections;
};

string ProcessHat::getSectionNumber(string & sectionNumber, unsigned int & sectionCount, unsigned int & sectionDepth)
{
	string number;
	ostringstream aStringStream;
	aStringStream << sectionCount;
	

	if(sectionDepth != 0) number = sectionNumber + "." + aStringStream.str();
	else number = aStringStream.str();

	return number;
};

string ProcessHat::getFigureNo(string & label)
{
	map<string, string>::const_iterator i = figures.find(label);

	if(i != figures.end()) return i->second;
	else return "?";
};

//get all word up until the an end command, starts with */ 
string ProcessHat::getText(ifstream & fileIn, string endWord)
{
	string word;
	string sentence = "";
	bool atStart = true;
	unsigned int length = endWord.length();
	
	do{

		fileIn >> word;

		if((endWord == "" && word.length() >= 2 && word.substr(0, 2) == "*/") ||
			(endWord != "" && word.length() >= length && word.substr(0, length) == endWord)) return sentence;
		else if(!atStart)
		{
			sentence += " ";			
		};

		sentence += word;
		atStart = false;
	}while(!fileIn.eof());

	return sentence;
};

//ignore all words up until the an end command, starts with */ 
void ProcessHat::processComment(ifstream & fileIn)
{
	string word;
	
	do{

		fileIn >> word;

		if(word == "*/comment*"	|| word == "*/title*" 
			|| word == "*/subtitle*" || word == "*/author*" || word == "*/address*" || word == "*/date*" || word == "*/abstract*" || word == "*/stylefile*"
			|| word == "*/logo*" || word == "*/logowidth*") return;		

	}while(!fileIn.eof());
};

void ProcessHat::processBoldTypeCommand(const string & starting, const string & ending, ifstream & fileIn, ostream & fileOut)
{	
	
	string word;
	string sentence = "";
	bool atStart = true;

	do{

		fileIn >> word;

		if(word.length() >=2 && word.substr(0, 2) == "*/") break;
		else if(!atStart)
		{
			sentence += " ";			
		};

		sentence += word;
		atStart = false;
	}while(!fileIn.eof());

	replaceSpecialChars(sentence);

	fileOut << starting << sentence << ending;

	//process the last word and catch any commas etc and put in space after
	processWord(word, fileIn, fileOut);
};

void ProcessHat::displayCreatedFiles()
{

	for(list<string>::const_iterator f = filesCreated.begin(); f != filesCreated.end(); ++f)
	{
		cout << "\t" << *f << "\n";
	};

};

void ProcessHat::displayNoSections()
{
	unsigned int noSections = orderedSections.size();
	unsigned int noSubsections = 0;
	for(list<Section * >::const_iterator os = orderedSections.begin(); os != orderedSections.end(); ++os)
	{
		noSubsections += (*os)->subsections.size();
	};

	cout << "Number of sections: "<< noSections <<"\n";
	cout << "Number of subsections: "<< noSubsections <<"\n";
};

void ProcessHat::trimStartWord(string & word, ifstream & fileIn, ostream & fileOut)
{
	unsigned int length = word.length();
	if(length < 3) return;

	string startChars = "";
	bool keyWordFoundAndTrim = false;

	for(unsigned int i = 1; i <= length; ++i)
	{
		if(word.substr(i-1, 1) == "*" || word.substr(i-1, 1) == "$")
		{
			if(i > 1)
			{
				keyWordFoundAndTrim = true;
				startChars = word.substr(0, (i-1));
				word = word.substr((i-1));
			};
			break;
		};
	};

	if(keyWordFoundAndTrim) fileOut << startChars;
};

string ProcessHat::trimEndWord(string & word, ifstream & fileIn, ostream & fileOut)
{
	unsigned int length = word.length();
	if(length < 3) return "";

	string endChars = "";	
	
	for(unsigned int i = length; i >= 1; --i)
	{
		if(word.substr(i-1, 1) == "*" || word.substr(i-1, 1) == "$")
		{			
			if(i < length)
			{
				endChars = word.substr(i);	
				word = word.substr(0, i);													
			};
			break;
		};
	};

	return endChars;
};

//return ith char that is a $, 0 denote no $ found
unsigned int getdollarPosition(string & word)
{
	unsigned int length = word.length();

	for(unsigned int i = 1; i <= length; i++)
	{
		if(word.substr((i-1), 1) == "$") return i;
	};

	return 0;
};

pair<string, string> ProcessHat::getLatexFormula(string & word, ifstream & fileIn, ostream & fileOut)
{
	string formula = "";
	string endChars = "";
	

	if(word == "$$") return make_pair(formula, endChars);

	//remove dollar $ from the start of the string
	if(word.length() > 1) word = word.substr(1);

	unsigned int dollarPos = getdollarPosition(word);

	if(dollarPos > 1)
	{
		formula = word.substr(0, (dollarPos - 1));
		if(word.length() > dollarPos) endChars = word.substr(dollarPos);
		replaceSpecialChars(formula);
		replaceSpecialChars(endChars);
		return make_pair(formula, endChars);
	}
	else
	{
		formula = word;
		string restOfFormula;
		char dollar = '$';
		getline(fileIn, restOfFormula, dollar);
		char space = ' ';
		getline(fileIn, endChars, space);

		formula.append(restOfFormula);
		//char oneChar[1];		
		//string aString;

		//formula = word; //start of expression, now get the rest
		////check word has a $ in it		

		//do{

		//	fileIn.read(oneChar, 1);
		//	//number = oneChar[1];
		//	if( fileIn.bad() ) {
  //     cerr << "Error reading data" << endl;
  //     exit( 0 );
  //   };
		//	aString = (string)(oneChar);
		//	cout << oneChar << "\n";
		//	cout << aString << "\n";
		//	exit(1);

		//	if(aString == "$") break;
		//	formula.append(aString);

		//}while(!fileIn.eof());
	};

	//get trailing bit
	
	/*do{

			fileIn.read(oneChar, 1);			
			aString = (string)(oneChar);
			if(aString == " " || aString == "\n") break;
			endChars += aString;

	}while(!fileIn.eof());*/

	//cout << formula << " "<<endChars<<"\n";
	
	//exit(1);

	replaceSpecialChars(formula);
	replaceSpecialChars(endChars);
	return make_pair(formula, endChars);

	/*if(word.substr((word.length() - 1)) == "$" check for dollar anywhere trim end)
	{
		formula = word.substr(0, (word.length() - 1));
	}
	else
	{
		do{

			fileIn >> word;
			if(word == "$") break;
			else if(word.substr((word.length() - 1)) == "$" check for dollar anywhere trim end)
			{
				formula.append(" ");
				formula.append(word.substr(0, (word.length() - 1)));
				break;
			}
			else
			{
				formula.append(" ");
				formula.append(word);
			};
			
		}while(!fileIn.eof());
	};
	
	cout << formula <<"\n";
	return formula;*/
};

bool ProcessHat::nextWordIsEndWord(ifstream & fileIn)
{
	string nextWord;

	//get position of next read
	int pos = fileIn.tellg();

	fileIn >> nextWord;

	//put read position back to where it was
	fileIn.seekg(pos);

	return (nextWord.length() >= 2 && nextWord.substr(0,2) == "*/");
};


string ProcessHat::getCodeExample(ifstream & fileIn, ostream & fileOut)
{
	string codeExample = "";
	string codeExamplePiece;
	string word;

	string restOfFormula;
	char star = '*';

	do{

		getline(fileIn, codeExamplePiece, star);
	
		fileIn >> word;		

		if(word == "/codeexample*")
		{
			codeExample.append(codeExamplePiece);
			break;
		}
		else
		{
			//found a star but there is more to get
			codeExample.append(codeExamplePiece);
			codeExample.append("*");
			codeExample.append(word);
		};

	}while(!fileIn.eof());

	//remove return from the start
	return codeExample.substr(1);
};

void ProcessHat::processTheSection(string & sectionName, string & sectionTitle, ifstream & fileIn, ostream & fileOut, unsigned int depth)
{
	depth++;
	//unsigned int sectionDepth = 0;

	string word;
	
	Section * section;
	map<string, Section * >::const_iterator s = sections.find(sectionName);
	if(s == sections.end())
	{
		cerr << "Cannot find section: "<<sectionName<<"!\n";

		cerr << "\nHave sections:\n\n";
		for(map<string, Section * >::const_iterator s = sections.begin(); s != sections.end(); ++s)
		{
			cerr << s->first << "\n";
		};
		//fileOut.close();
		exit(1);
	}
	else
	{
		section = s->second;
	};

	startSection(fileOut, section, depth);

	bool atStart = true;
	bool process = true;
	bool checkNextWordForPara = false;
	bool mayEndPara = false;

	do{
		fileIn >> word;
		process = true;

		//if word is a regular word (or bold, italic, underline, etc.) then start a paragraph
		if(checkNextWordForPara && 
			(!(word.length() >= 1 && word.substr(0, 1) == "*") ||
			  (word.length() >= 3 && 
			    (word.substr(0, 3) == "*b*" || 
				 word.substr(0, 3) == "*i*" ||
				 word.substr(0, 3) == "*u*" ||
				 word.substr(0, 3) == "*q*")) ||
			  (word.length() >= 5 && 
			    (word.substr(0, 5) == "*ref*" || 
				 word.substr(0, 5) == "*tex*")) ||
			  (word.length() >= 6 && 
			    (word.substr(0, 6) == "*html*" || 
				 word.substr(0, 6) == "*star*" ||
				 word.substr(0, 6) == "*code*" ||
				 word.substr(0, 6) == "*cite*" ||
				 word.substr(0, 6) == "*perce"))
				 ))
		{
			//start of paragraph
			startParagraph(fileOut);
			mayEndPara = true; //can only end a paragraph if one is started
		};

		checkNextWordForPara = false;

		while(word == "*subsection*" || word == "*subsubsection*")
		{
			processSection(fileIn, fileOut, depth);

			fileIn >> word; if(fileIn.eof()) return;
			atStart = true;
		};

		//check for end of section
		if(word == "*/section*" || word == "*/subsection*" 
			|| word == "*/subsubsection*"
			)
		{
			endSection(fileOut, section, depth);
			return;
		}
		else if(word == "*")
		{
			process = false;
			if(atStart)
			{
				//start of paragraph
				startParagraph(fileOut);

				atStart = false;
				mayEndPara = true;
				//fileIn >> word; if(fileIn.eof()) return;
			}
			else
			{
				//end of paragraph
				if(mayEndPara) endParagraph(fileOut);
				checkNextWordForPara = true;
				mayEndPara = false;
			};

		};		
	
		
		if(process) processWord(word, fileIn, fileOut);

		//a paragraph was not started here so do not end one after a block of something
		//if(word == "/codeexample*" || word == "*/table*" ||
		//	word == "*/tabler*" || word == "*/tablec*" || word == "*/tablel*")
		//{
		//	mayEndPara = false;
		//};
		
	}while(!fileIn.eof());

};

void ProcessTex::processSection(ifstream & fileIn, ostream & fileOut, unsigned int depth)
{
	string sectionName, sectionTitle;

	getSectionNameAndTitle(fileIn, fileOut, sectionName, sectionTitle);

	if(verbose) cout << "\nStart TEX section: " << sectionName << " -- " << sectionTitle << " depth = " << depth << "\n";
	processTheSection(sectionName, sectionTitle, fileIn, fileOut, depth);
	if(verbose) cout << "\nEnd TEX section: " << sectionName << " -- " << sectionTitle << " depth = " << depth << "\n";
};

void ProcessHtml::processSection(ifstream & fileIn, ostream & fileOut, unsigned int depth)
{
	string sectionName, sectionTitle;
	getSectionNameAndTitle(fileIn, fileOut, sectionName, sectionTitle);

	if(verbose) cout << "\nStart HTML section: " << sectionName << " -- " << sectionTitle << " depth = " << depth << "\n";

	if(depth == 0 || (subSectionsOnNewPage && depth == 1) )
	{
		string newSectionNameFile = sectionName + ".html";
		ofstream fileOutNewSection(newSectionNameFile.c_str());

		filesCreated.push_back(newSectionNameFile);
		header(fileIn, fileOutNewSection); 
		processTheSection(sectionName, sectionTitle, fileIn, fileOutNewSection, depth);
		footer(fileIn, fileOutNewSection); 

		fileOutNewSection.close();
	}
	else
	{
		processTheSection(sectionName, sectionTitle, fileIn, fileOut, depth);
	};
	if(verbose) cout << "\nEnd HTML section: " << sectionName << " -- " << sectionTitle << " depth = " << depth << "\n";
};

void ProcessHtml::processWebpage(ifstream & fileIn, ostream & fileOut)
{
	processingWebpage = true;
	string webpageName, webpageTitle;
	getWebpageNameAndTitle(fileIn, fileOut, webpageName, webpageTitle);

	string newWebpageNameFile = webpageName + ".html";
	ofstream fileOutNewWebpage(newWebpageNameFile.c_str());
	
	filesCreated.push_back(newWebpageNameFile);
	header(fileIn, fileOutNewWebpage); 

	fileOutNewWebpage << "<td valign=\"top\">\n";
	fileOutNewWebpage << "<!-- Begin Right Column -->\n";
	fileOutNewWebpage << "<div id=\"rightcolumn\">\n"; 

	string word;
	
	bool atStart = true;
	bool process = true;
	bool checkNextWordForPara = false;

	do{
		fileIn >> word;
		process = true;

		//if word is a regular word then start a paragraph
		if(checkNextWordForPara && !(word.length() >= 2 && word.substr(0, 1) == "*" && word.substr(0, 2) != "**"))
		{
			//start of paragraph
			startParagraph(fileOutNewWebpage);
		};

		checkNextWordForPara = false;

		//check for end of section
		if(word == "*/webpage*")
		{
			break;			
		}
		else if(word == "*")
		{
			process = false;
			if(atStart)
			{
				//start of paragraph
				startParagraph(fileOutNewWebpage);
				atStart = false;				
			}
			else
			{
				//end of paragraph
				endParagraph(fileOutNewWebpage);
				checkNextWordForPara = true;		
			};
		};		
	
		if(process) processWord(word, fileIn, fileOutNewWebpage);
		
	}while(!fileIn.eof());

	
	fileOutNewWebpage << "</div>\n"; 
	fileOutNewWebpage << "<!-- End Right Column -->\n";
	fileOutNewWebpage << "</td>\n";
	footer(fileIn, fileOutNewWebpage); 

	fileOutNewWebpage.close();
	processingWebpage = false;
};

void ProcessHtml::addReferencesWebpage(ifstream & fileIn, ostream & fileOut)
{
	if(bibFileName == "") return;

	string references = "references.html";
	ofstream fileOutNewWebpage(references.c_str());
	filesCreated.push_back(references);

	header(fileIn, fileOutNewWebpage); 

	fileOutNewWebpage << "<td valign=\"top\">\n";
	fileOutNewWebpage << "<!-- Begin Right Column -->\n";
	fileOutNewWebpage << "<div id=\"rightcolumn\">\n"; 

	fileOutNewWebpage << "<h1>References</h1>";

	//reorder the references
	map<string, Citation *> orderedCitations; //orderName, citation
	string aName;

	for(map<string, Citation *>::const_iterator c = citations.begin(); c != citations.end(); ++c)
	{
		aName = c->second->orderName + c->first; //ensure is unique
		orderedCitations[aName] = c->second;		
	};

	

	for(map<string, Citation *>::const_iterator oc = orderedCitations.begin(); oc != orderedCitations.end(); ++oc)
	{
		
		fileOutNewWebpage << "<br />\n";	

		fileOutNewWebpage << "<p>\n";
		fileOutNewWebpage << "<a id=\""<<oc->second->name<<"\">\n";
		if(oc->second->authors != "") fileOutNewWebpage << oc->second->authors <<".<br />\n";
		else if(oc->second->editor != "") fileOutNewWebpage << oc->second->editor <<".\n";
		fileOutNewWebpage << "</a>\n";
		fileOutNewWebpage << "<b>&ldquo;" << oc->second->title <<".&rdquo;</b><br />\n";
		if(oc->second->authors != "" && oc->second->editor != "") fileOutNewWebpage << "Edited by " << oc->second->editor <<".\n";
		if(oc->second->note != "")
		{
			if((oc->second->note).length() >= 4 && oc->second->note.substr(0, 4) == "http") fileOutNewWebpage << "<a target=\"_blank\" href=\"" << oc->second->note << "\">"<< oc->second->note <<"</a> ";
			else fileOutNewWebpage << oc->second->note << " ";
		};
		if(oc->second->journal != "") fileOutNewWebpage << "<i>" << oc->second->journal <<",</i> ";
		if(oc->second->publisher != "") fileOutNewWebpage << "<i>" << oc->second->publisher <<",</i> ";
		if(oc->second->volume != "") fileOutNewWebpage << oc->second->volume;
		if(oc->second->number != "") fileOutNewWebpage << "(" << oc->second->number << ")";
		if(oc->second->pages != "") fileOutNewWebpage << ", pp. " << oc->second->pages;
		if(oc->second->volume != "" || oc->second->number != "" || oc->second->pages != "") fileOutNewWebpage << ", ";
		fileOutNewWebpage << oc->second->year << ". ";

		fileOutNewWebpage << "<small><a target=\"_blank\" href=\"http://www.google.com/search?as_epq=" << oc->second->title;
		//if(oc->second->authors != "") fileOutNewWebpage << "&as_oq="<<oc->second->authors;

		if(oc->second->journal != "") fileOutNewWebpage << "&as_q=" << oc->second->journal;

		if(oc->second->refName != "") fileOutNewWebpage << "&as_oq="<<oc->second->refName;
		else if(oc->second->editor != "") fileOutNewWebpage << "&as_oq=" << oc->second->editor;

		//fileOutNewWebpage << "&as_q=:site -www.staff.ncl.ac.uk";
		fileOutNewWebpage << "\">Search</a></small>";

		if(oc->second->url != "") fileOutNewWebpage << "</br>\n <a href=\"" << oc->second->url << "\">" << oc->second->url << "</a>";

		fileOutNewWebpage << "</p>\n";
	};











	fileOutNewWebpage << "</div>\n"; 
	fileOutNewWebpage << "<!-- End Right Column -->\n";
	fileOutNewWebpage << "</td>\n";
	footer(fileIn, fileOutNewWebpage); 

	fileOutNewWebpage.close();

};

void ProcessTex::processHtml(ifstream & fileIn, ostream & fileOut)
{
	//do nothing with text
	string word;
	fileIn >> word;
	
	string endTrim;
	
	do{
		endTrim = trimEndWord(word, fileIn, fileOut);
		
		if(word == "*/html*")
		{
			if(endTrim != "") fileOut << endTrim;
			return; 
		};
		
		fileIn >> word;
	}while(!fileIn.eof());
};

void ProcessTex::processWebpage(ifstream & fileIn, ostream & fileOut)
{
	string word;
	fileIn >> word;
	
	do{
		if(word == "*/webpage*") return; 		
		fileIn >> word;
	}while(!fileIn.eof());

};

void ProcessTex::processTex(ifstream & fileIn, ostream & fileOut)
{
	string word;
	fileIn >> word;
	
	string endTrim;
	
	do{
		endTrim = trimEndWord(word, fileIn, fileOut);
		
		if(word == "*/tex*")
		{
			if(endTrim != "") fileOut << endTrim;
			return; 
		};
		
		processWord(word, fileIn, fileOut);
		if(endTrim != "") fileOut << endTrim;

		fileIn >> word;
	}while(!fileIn.eof());

};

void ProcessHtml::processHtml(ifstream & fileIn, ostream & fileOut)
{
	string word;
	fileIn >> word;
	
	string endTrim;
	
	do{
		endTrim = trimEndWord(word, fileIn, fileOut);
		
		if(word == "*/html*")
		{
			if(endTrim != "") fileOut << endTrim;
			return; 
		};
		
		processWord(word, fileIn, fileOut, false);
		if(endTrim != "") fileOut << endTrim;

		fileIn >> word;
	}while(!fileIn.eof());
};

void ProcessHtml::processTex(ifstream & fileIn, ostream & fileOut)
{
	
	//do nothing with text
	string word;
	fileIn >> word;
	
	string endTrim;
	
	do{
		endTrim = trimEndWord(word, fileIn, fileOut);
		
		if(word == "*/tex*")
		{
			if(endTrim != "") fileOut << endTrim;
			return; 
		};
		
		fileIn >> word;
	}while(!fileIn.eof());
};

void ProcessTex::startSection(ostream & fileOut, Section * section, unsigned int & depth)
{
	if(depth == 1)	fileOut << "\\section{"<<section->title<<"}\n";
	else if(depth == 2) fileOut << "\\subsection{"<<section->title<<"}\n";
	else if(depth >= 3) fileOut << "\\subsubsection{"<<section->title<<"}\n";

	fileOut << "\\label{"<<section->name<<"}\n";

};

void ProcessTex::endSection(ostream & fileOut, Section * section, unsigned int & depth)
{
	
	if(depth == 1)	fileOut << "\n%================== End of section \""<<section->name<<"\"==================\n\n";
	else if(depth == 2) fileOut << "\n%============ End of subsection \""<<section->name<<"\"============\n\n";
	else if(depth >= 3) fileOut << "\n%====== End of subsubsection \""<<section->name<<"\"======\n\n";
};

void ProcessTex::startParagraph(ostream & fileOut)
{
	fileOut << "\n";
};

void ProcessTex::endParagraph(ostream & fileOut)
{
	fileOut << "\n";
};

void ProcessHtml::startSection(ostream & fileOut, Section * section, unsigned int & depth)
{
	if(depth == 1 || (subSectionsOnNewPage && depth == 2))
	{
		fileOut << "<td valign=\"top\">\n";
		fileOut << "<!-- Begin Right Column -->\n";
		fileOut << "<div id=\"rightcolumn\">\n"; 
	}
	else
	{
		//fileOut << "<div id=\"subsection\">\n"; 
	};

	if(depth == 1) fileOut << "<h1>"<<section->number<<" "<<section->title<<"</h1>\n";
	else if(depth == 2)
	{		
		fileOut << "<h2><a name=\""<<section->name<<"\">"<<section->number<<" "<<section->title<<"</a></h2>\n";	
	}
	else if(depth >= 3)
	{		
		fileOut << "<h3><a name=\""<<section->name<<"\">"<<section->title<<"</a></h3>\n";
	};
};

void ProcessHtml::addNextAndPrev(ostream & fileOut, Section * section)
{
	Section * prev = 0;

	list<Section *> orderedSectionsAndSubsections;

	for(list<Section * >::const_iterator os = orderedSections.begin(); os != orderedSections.end(); ++os)
	{
		orderedSectionsAndSubsections.push_back(*os);

		if((*os)->newPageForSubsections)
		{
			for(list<Section * >::const_iterator ss = (*os)->subsections.begin(); ss != (*os)->subsections.end(); ++ss)
			{
				orderedSectionsAndSubsections.push_back(*ss);
			};
		};
	};

	for(list<Section *>::const_iterator i = orderedSectionsAndSubsections.begin(); i != orderedSectionsAndSubsections.end(); )
	{
		if((*i)->name == section->name)
		{
			fileOut << "<br />\n<div class=\"prevnext\">";
			
			if(prev != 0)
			{		
				fileOut << "<span class=\"left\">";
				if(prev->nameUpperSection != "") fileOut << "<a href=\""<<prev->nameUpperSection<<".html#"<<prev->name<<"\"><-prev</a>\n";
				else fileOut << "<a href=\""<<prev->name<<".html\">&lt;-prev</a>\n";		
				fileOut << "</span>";
			};

			prev = *i;
			i++;

			if(i != orderedSectionsAndSubsections.end())
			{	
				fileOut << "<span class=\"right\">";
				if((*i)->nameUpperSection != "") fileOut << "<a href=\""<<(*i)->nameUpperSection<<".html#"<<(*i)->name<<"\">next-></a>\n";
				else fileOut << "<a href=\""<<(*i)->name<<".html\">next-&gt;</a>\n";		
				fileOut << "</span>";
			};

			fileOut << "</div>";
			return;
		}
		else
		{
			prev = *i;
			i++;
		};
	
	};


};

void ProcessHtml::endSection(ostream & fileOut, Section * section, unsigned int & depth)
{
	if(depth == 1 || (subSectionsOnNewPage && depth == 2))
	{
		addNextAndPrev(fileOut, section);

		fileOut << "<!-- End Right Column -->\n";
		fileOut << "</div>\n";

		fileOut << "</td>\n";
	}
	else
	{
		//fileOut << "</div>\n";
	};
	
};

void ProcessHtml::startParagraph(ostream & fileOut)
{
	fileOut << "<p>\n";
};

void ProcessHtml::endParagraph(ostream & fileOut)
{
	fileOut << "</p>\n";
};

void ProcessHtml::header(ifstream & fileIn, ostream & fileOut)
{
	fileOut << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
			<< "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
			<< "<head>\n"
			<< "<title>"<<title<<"</title>\n"
			<< "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" />\n"
			<< "<link rel=\"stylesheet\" type=\"text/css\" href=\""<<styleFile<<"\" />\n"
			<< "<link rel=\"shortcut icon\" href=\"favicon.ico\" />\n" 
			<< "</head>\n"
			<< "<body>\n"
			<< "<!-- Begin Wrapper -->\n"
			<< "<div id=\"wrapper\">\n"
			<< "<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr><td>\n"			
			<< "<!-- Begin Header -->\n"
			<< "<div id=\"header\">\n";

	if(logo != "")
	{
		fileOut	<< "<table><tr>\n"
				<< "<td>\n"
				<< "<img width=\""<<(logowidth)<<"\" src=\""<<logo<<"\" alt=\""<<title<<"\"/>\n" 
				<< "</td>\n"
				<< "<td>\n";
	};

	fileOut	<< "<p style=\"margin-left: 1em;\">"<<title<<"</p>\n";

	if(subtitle != "") fileOut << "<div id=\"subtitle\"><p>" << subtitle<<"</p></div>\n";
	
	if(logo != "")
	{
		fileOut	<< "</td></tr>\n"
				<< "</table>\n";
	};

	fileOut	<< "</div>\n"
			<< "<!-- End Header -->\n";

	menu(fileIn, fileOut);

	fileOut << "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr>\n"
			<< "<td valign=\"top\">\n";
	
	contents(fileIn, fileOut);

	fileOut << "</td>\n";
};

void ProcessHtml::menu(ifstream & fileIn, ostream & fileOut)
{

	fileOut << "<!-- Begin Menu Navigation -->\n"
			//<< "<div id=\"navigation\">\n"
			<< "<ul id=\"menunav\">\n";

	for(list<Webpage * >::const_iterator ow = orderedWebpages.begin(); ow != orderedWebpages.end(); ++ow)
	{
		
		fileOut << "<li><a href=\""<<(*ow)->name<<".html\">"<<(*ow)->title<<"</a></li>\n";

	};

	fileOut << "</ul>\n"
			//<< "</div>\n"
			<< "<!-- End Menu Navigation -->\n"
			<< "\n</td></tr>\n<tr><td>\n";

};

void ProcessHtml::contents(ifstream & fileIn, ostream & fileOut)
{
	
    fileOut << "<!-- Begin Left Column -->\n"
		   << "<div id=\"leftcolumn\">\n";

    fileOut << "<!-- Begin Contents Navigation -->\n"
			<< "<div id=\"navcontainer\">\n"
			<< "<ul>\n";

	for(list<Section * >::iterator os = orderedSections.begin(); os != orderedSections.end(); ++os)
	{
		fileOut << "<li><a href=\""<<(*os)->name<<".html\">" << (*os)->number <<" "<<(*os)->title << "</a>\n";
		//do subsections
		if((*os)->subsections.size() > 0)
		{
			fileOut << "<ul>\n";
			for(list<Section * >::const_iterator s = (*os)->subsections.begin(); s != (*os)->subsections.end(); ++s)
			{
				if((*os)->newPageForSubsections) fileOut << "<li><a href=\""<<(*s)->name<<".html\">" << (*s)->number << " " <<(*s)->title << "</a></li>\n";
				else fileOut << "<li><a href=\""<<(*os)->name<<".html#"<<(*s)->name<<"\">" << (*s)->number << " " <<(*s)->title << "</a></li>\n";
			};
			fileOut << "</ul>\n";
		};
		fileOut << "</li>\n";
	};

	if(bibFileName != "") fileOut << "<li><a href=\"references.html\">References</a></li>\n";

	fileOut << "</ul>\n";

	fileOut << "</div>\n"
			<< "<!-- End Contents Navigation -->\n";

	fileOut << "</div>\n"
			<< "<!-- End Left Column -->\n";
		
};

void ProcessHtml::addFooterText(ostream & fileOut)
{
	ifstream fileFooterIn(footerFileName.c_str());
	string word;

	if(!fileFooterIn.is_open())
	{
		cerr<<"Cannot read file: "<<footerFileName<< "!\n";
		exit(1);
	};

	do{

		fileFooterIn >> word;
		if(fileFooterIn.eof()) break;
		fileOut << " " << word;

	}while(!fileFooterIn.eof());

};

void ProcessHtml::footer(ifstream & fileIn, ostream & fileOut)
{
	fileOut << "</tr></table>\n";
	fileOut << "<!-- End Wrapper -->\n"
			<< "</td></tr></table>\n"
			<< "</div>\n";

	if(footerFileName != "") addFooterText(fileOut);

	fileOut << "</body>\n"
			<< "</html>\n";
};

void ProcessTex::header(ifstream & fileIn, ostream & fileOut)
{
	fileOut << "\\documentclass[a4paper,12pt]{article}\n"
			<< "\\setcounter{secnumdepth}{2}\n"
			<< "\\newcommand{\\code}[1]{{\\footnotesize{{\\tt #1}}}}\n";
	
	fileOut << "\\usepackage{natbib}\n"
			<< "\\usepackage{color}\n"
			<< "\\usepackage{graphicx}\n";

	fileOut << "\\usepackage{listings}\n"
			<< "\\lstset{\n"
			<< "basicstyle=\\small\\ttfamily,\n"
			<< "columns=flexible,\n"
			<< "breaklines=true\n"
			<< "}\n";

	fileOut << "\\addtolength{\\textwidth}{2cm} % a = -2b, where this is a and below is b\n"
			<< "\\addtolength{\\hoffset}{-1cm}\n"
			<< "\\addtolength{\\textheight}{2cm} % c = -d, where this is c and d is below\n"
			<< "\\addtolength{\\voffset}{-2cm}\n";

	fileOut	<< "\\begin{document}\n";

	if(subtitle != "")	fileOut << "\\title{"<<title<<" {\\small "<<subtitle<<"}}\n";
	else fileOut << "\\title{"<<title<<"}\n";

	if(date != "") fileOut << "\\date{" <<date<< "}\n";
	else fileOut << "\\date{}\n";

	if(author != "" || address != "")
	{
		fileOut << "\\author{";
		if(author != "")
		{
			fileOut << author;
			if(address!="") fileOut << "\\\\";
		};

		if(address!="") fileOut << address;

		fileOut << "}\n";

	}
	else fileOut << "\\author{}\n";

	fileOut	<< "\\maketitle\n"
			<< "\\newpage\n";

	if(abstract != "")
	{
		fileOut	<< "\\abstract\n";

		fileOut	<< abstract << "\n";

		fileOut	<< "\\newpage\n";
	};

	fileOut << "\\tableofcontents\n"
			<< "\\newpage\n";


};

void ProcessTex::footer(ifstream & fileIn, ostream & fileOut)
{
	if(bibFileName != "")
	{
		//fileOut << "\\bibliographystyle{plain}\n";
		fileOut << "\\bibliographystyle{genepi}\n";
		fileOut << "\\bibliography{"<<bibFileName.substr(0, (bibFileName.length() - 4)) <<"}\n";
	};

	fileOut << "\\end{document}";
};

void ProcessTex::processCode(ifstream & fileIn, ostream & fileOut, bool start)
{
	if(start) fileOut << "\\code{";
	else fileOut << "}";

	//string word;
	//fileIn >> word;

	//fileOut << "\\code{";
	//do{
	//	if(word == "*/code*") break; 
	//	processWord(word, fileIn, fileOut);
	//	fileIn >> word;
	//}while(!fileIn.eof());

	//fileOut << "}";
};

void ProcessHtml::processCode(ifstream & fileIn, ostream & fileOut, bool start)
{
	if(start) processBoldTypeCommand("<tt>", "</tt>", fileIn, fileOut);

	//if(start) fileOut << "<tt>";
	//else fileOut << "</tt>";

	//string word = getText(fileIn);
	//fileOut << "<tt>"<<word<<"</tt>";

	//string word;
	//fileIn >> word;

	//fileOut << "<tt>";
	//do{
	//	if(word == "*/code*") break; 
	//	processWord(word, fileIn, fileOut);
	//	fileIn >> word;
	//}while(!fileIn.eof());

	//fileOut << "</tt>";
};

void ProcessTex::processCodeExample(ifstream & fileIn, ostream & fileOut)
{
	//string word;
	//fileIn >> word;

	//fileOut << "\\begin{verbatim}\n";
	//do{
	//	if(word == "*/codeexample*") break; 
	//	else if(word == "*") fileOut << "\n";
	//	else if(word == "_") fileOut << " ";
	//	else fileOut << word << " ";

	//	fileIn >> word;
	//}while(!fileIn.eof());

	//fileOut << "\\end{verbatim}\n";

    string codeExample = getCodeExample(fileIn, fileOut);

	//fileOut << "\\begin{verbatim}"<<codeExample<<"\\end{verbatim}\n";

	//use new package to ensure wrapping
	fileOut << "\\vspace{0.35cm} \\begin{lstlisting}\n";
	fileOut << codeExample << "\n";
	fileOut << "\\end{lstlisting} \\vspace{0.35cm}";
};

void ProcessHtml::processCodeExample(ifstream & fileIn, ostream & fileOut)
{
	string codeExample = getCodeExample(fileIn, fileOut);
	replaceSpecialChars(codeExample);

	fileOut << "<pre>"<<codeExample<<"</pre>\n";
	//fileOut << "<pre>\n";
	//do{
	//	if(word == "*/codeexample*") break; 
	//	else if(word == "*") fileOut << "\n";
	//	else if(word == "_") fileOut << " ";
	//	else fileOut << word << " ";
//
	//	fileIn >> word;
	//}while(!fileIn.eof());

	//fileOut << "</pre>\n";
};

void ProcessTex::processCodeExampleSmall(ifstream & fileIn, ostream & fileOut)
{
	//string word;
	//fileIn >> word;

	//fileOut << "\\begin{verbatim}\n";
	//do{
	//	if(word == "*/codeexample*") break; 
	//	else if(word == "*") fileOut << "\n";
	//	else if(word == "_") fileOut << " ";
	//	else fileOut << word << " ";

	//	fileIn >> word;
	//}while(!fileIn.eof());

	//fileOut << "\\end{verbatim}\n";

    string codeExample = getCodeExample(fileIn, fileOut);

	//fileOut << "{\\scriptsize \\begin{verbatim}"<<codeExample<<"\\end{verbatim}}\n";

	fileOut << "{\\scriptsize \\begin{lstlisting} ";
	fileOut << codeExample << " ";
	fileOut << "\\end{lstlisting}}\n";
};

void ProcessHtml::processCodeExampleSmall(ifstream & fileIn, ostream & fileOut)
{
	string codeExample = getCodeExample(fileIn, fileOut);
	replaceSpecialChars(codeExample);

	fileOut << "<pre>"<<codeExample<<"</pre>\n";
	//fileOut << "<pre>\n";
	//do{
	//	if(word == "*/codeexample*") break; 
	//	else if(word == "*") fileOut << "\n";
	//	else if(word == "_") fileOut << " ";
	//	else fileOut << word << " ";
//
	//	fileIn >> word;
	//}while(!fileIn.eof());

	//fileOut << "</pre>\n";
};

void ProcessHtml::replaceSpecialChars(string & aString)
{
	string newString = "";

	string::iterator it;
	for(string::iterator it = aString.begin(); it < aString.end(); it++ )
	{
		if(*it == '<') newString.append("&lt;");
		else if(*it == '>') newString.append("&gt;");
		else newString.push_back(*it);
	};

	aString = newString;
};

//align 1 = right, 2 = left, 3 = center
void ProcessHtml::processTable(ifstream & fileIn, ostream & fileOut, const unsigned int & align, const bool & scale)
{
	bool firstRow = true;
	string word;
	fileIn >> word;
	bool alt = false;

	if(align == 1) fileOut << "<table id=\"tablestyle\" class=\"center\">\n";
	else if(align == 2) fileOut << "<table id=\"tablestylel\" class=\"center\">\n";
	else if(align == 3) fileOut << "<table id=\"tablestylec\" class=\"center\">\n";

	do{
		if(word == "*/table*" || word == "*/tabler*" || word == "*/tablel*" || word == "*/tablec*"  || word == "*/tableropt*" || word == "*/tablelopt*" || word == "*/tablecopt*") break; 
		else if(word == "*tr*")
		{
			if(firstRow) fileOut << "<tr><th>";
			else if(alt) fileOut << "<tr class=\"alt\"><td valign=\"top\">";
			else fileOut << "<tr><td valign=\"top\">";

			alt = !alt;
		}
		else if(word == "*/tr*")
		{
			if(firstRow)
			{
				fileOut << "</th></tr>\n";
				firstRow = false;
			}
			else
				fileOut << "</td></tr>\n";
		}
		else if(word == "&")
		{
			if(firstRow) fileOut << "</th><th>";
			else fileOut << "</td><td valign=\"top\">";
		}		
		else processWord(word, fileIn, fileOut);

		fileIn >> word;
		
	}while(!fileIn.eof());

	fileOut << "</table>\n";
};

void ProcessHtml::processFigure(ifstream & fileIn, ostream & fileOut)
{
	string fig, word;
	string caption = "";
	string label = "";
	string figName = "";
	string width ="";
	string widthtex ="";

	//first word should be the file name
	fileIn >> fig;	

	
	//next word should be start of caption 
	fileIn >> word;	

	if(!(word.length() >= 9 && word.substr(0, 9) == "*caption*"))
	{
			cerr << "Found "<< word <<" instead of *caption* for figure "<<fig<<"!\n"; 
	}
	else
	{
			caption = getText(fileIn);
			//next word should be start of label 
			fileIn >> word;	

			if(!(word.length() >= 7 && word.substr(0, 7) == "*label*"))
			{
				cerr << "Found "<< word <<" instead of *label* for figure "<<fig<<"!\n";
				figName = "Figure ?. "; 
			}
			else
			{
				label = getText(fileIn);
				
				figName =  "Figure " + getFigureNo(label) +". "; 
			};

			
	};

	fileIn >> word;	

	if(word.length() >= 7 && word.substr(0, 7) == "*width*")
	{
		width = getText(fileIn);
		fileIn >> word;	
	};

	fileOut << "<div id=\"fig\"><a href=\""<<fig<<"\"><img src=\""<<fig<<"\" border=\"0\" class=\"figimg\" ";
	if(width != "") fileOut << "width=\""<<width<<"\"";
	fileOut << "></a><br />\n<br />\n"
			<< figName << caption <<"\n"
			<< "</div>\n";
	
	if(word.length() >= 10 && word.substr(0, 10) == "*widthtex*")
	{
		widthtex = getText(fileIn); 
		fileIn >> word;	
	};
	

	if(!(word.length() >= 9 && word.substr(0, 9) == "*/figure*")) cerr << "Warning */figure* not found at end of figure: "<<fig<<"!\n"; 
	
};

//align 1 = right, 2 = left, 3 = center
void ProcessTex::processTable(ifstream & fileIn, ostream & fileOut, const unsigned int & align, const bool & scale)
{
	bool firstRow = true;
	string word;
	//string ans;
	unsigned int noOfCols = 0;
	ostringstream oss;

	fileIn >> word;

	do{
		if(word == "*/table*" || word == "*/tabler*" || word == "*/tablel*" || word == "*/tablec*" || word == "*/tableropt*" || word == "*/tablelopt*" || word == "*/tablecopt*") break;
		else if(word == "*tr*") {;}
		else if(word == "*/tr*")
		{
			//ans.append("\\\\\n");
			//if(firstRow) ans.append("\\hline\n");
			oss << "\\\\\n";
			if(firstRow) oss << "\\hline\n";

			firstRow = false;
		}
		else if(word == "&")
		{
			oss << " & ";
			//ans.append(" & ");		
		}
		else
		{
			//ans.append(word);
			//ans.append(" ");
			processWord(word, fileIn, oss);

			if(firstRow) noOfCols++;
		};

		fileIn >> word;

		
		
	}while(!fileIn.eof());

	string alignStr = "{";
	for(unsigned int i = 1; i <= noOfCols; ++i)
	{
		if(scale && i == 2) alignStr.append("p{9cm}");
		else if(align == 1) alignStr.append("r");
		else if(align == 2) alignStr.append("l");
		else if(align == 3) alignStr.append("c");
	};
	alignStr.append("}");

	string all;
	if(scale) all = "\n{\\begin{center}\\resizebox{16cm}{!}{\\begin{tabular}";
	else all = "\n{\\begin{center}\\begin{tabular}";

	all.append(alignStr);
	all.append("\n");
	all.append(oss.str());

	if(scale) all.append("\\end{tabular}}\\end{center}}\n");
	else all.append("\\end{tabular}\\end{center}}\n");
	

	fileOut << all;
};

void ProcessTex::processFigure(ifstream & fileIn, ostream & fileOut)
{
	string fig, word;
	string caption = "";
	string label = "";
	string figName = "";
	string width ="";

	//first word should be the file name
	fileIn >> fig;	

	

	//next word should be start of caption 
	fileIn >> word;	

	if(!(word.length() >= 9 && word.substr(0, 9) == "*caption*"))
	{
			cerr << "Warning caption not found for figure "<<fig<<"!\n"; 
	}
	else
	{
			caption = getText(fileIn);
			//next word should be start of label 
			fileIn >> word;	

			if(!(word.length() >= 7 && word.substr(0, 7) == "*label*"))
			{
				cerr << "Warning label not found for figure "<<fig<<"!\n";
				label = ""; 
			}
			else
			{
				label = getText(fileIn);				
			};

			
	};

	fileIn >> word;	

	if(word.length() >= 7 && word.substr(0, 7) == "*width*")
	{
		width = getText(fileIn);
		fileIn >> word;	
	};

	if(word.length() >= 10 && word.substr(0, 10) == "*widthtex*")
	{
		width = getText(fileIn); 
		fileIn >> word;	
	};

	fileOut << "{\\begin{figure}[ht]\n"
			<< "{\\begin{center}\n"
			<< "{\\includegraphics";
	if(width != "") fileOut << "[width="<<width<<"pt]";
	fileOut << "{"<<fig<<"}}\n";


	fileOut << "\\caption{"<<caption<<"}\n"
				    << "\\label{"<<label<<"}\n";

	fileOut << "\\end{center}}\n"
			<< "\\end{figure}\n"
			<< "}\n";


	if(!(word.length() >= 9 && word.substr(0, 9) == "*/figure*")) cerr << "Warning */figure* not found at end of figure: "<<fig<<"!\n"; 
	
};

void ProcessTex::processList(ifstream & fileIn, ostream & fileOut, const bool & numList)
{	
	string word;

	if(numList) fileOut << "\\begin{enumerate}\n";
	else fileOut << "\\begin{itemize}\n";

	do{
		fileIn >> word;

		if(word == "*item*") fileOut << "\n\\item ";
		else if(word == "*/list*" || word == "*/numlist*") break;
		else processWord(word, fileIn, fileOut);

	}while(!fileIn.eof());

	if(numList) fileOut << "\\end{enumerate}\n";
	else fileOut << "\\end{itemize}\n";

};

void ProcessHtml::processList(ifstream & fileIn, ostream & fileOut, const bool & numList)
{
	bool atStart = true;
	string word;
	
	if(numList) fileOut << "<ol>\n";
	else fileOut << "<ul>\n";
	
	do{
		fileIn >> word;

		if(word == "*item*")
		{
			if(atStart) fileOut << "<li>\n";
			else fileOut << "</li><li>\n";			
		}
		else if(word == "*/list*" || word == "*/numlist*") break;
		else processWord(word, fileIn, fileOut);

		atStart = false;
	}while(!fileIn.eof());

	
	if(numList) fileOut << "</li>\n</ol>\n";
	else fileOut << "</li>\n</ul>\n";
};

void ProcessTex::processRef(ifstream & fileIn, ostream & fileOut)
{
	string word;
	
	fileIn >> word;

	fileOut << " section \\ref{"<<word<<"}";

	fileIn >> word;
	if(word.length() >= 7 && word.substr(0, 6) == "*/ref*") fileOut << word.substr(6)<<" "; 
	else if(!(word.length() >= 6 && word.substr(0, 6) == "*/ref*")) cerr << "Warning */ref* not found at end of reference!\n"; 
	else fileOut << " ";
};

void ProcessTex::processFigRef(ifstream & fileIn, ostream & fileOut)
{
	string word;
	
	fileIn >> word;

	fileOut << " \\ref{"<<word<<"}";

	fileIn >> word;
	if(word.length() >= 10 && word.substr(0, 9) == "*/figref*") fileOut << word.substr(9)<<" "; 
	else if(!(word.length() >= 9 && word.substr(0, 9) == "*/figref*")) cerr << "Warning */figref* not found at end of figure reference!\n"; 
	else fileOut << " ";
};

void ProcessHtml::processRef(ifstream & fileIn, ostream & fileOut)
{
	string word;
	fileIn >> word;

	map<string, Section * >::const_iterator s = sections.find(word);
	
	if(s != sections.end())
	{
		if(s->second->nameUpperSection != "") fileOut << "<a href=\""<<s->second->nameUpperSection<<".html#"<<s->second->name<<"\">section "<<s->second->number<<"</a>";
		else fileOut << "<a href=\""<<s->second->name<<".html\">section "<<s->second->number<<"</a>";

		fileIn >> word;
		
	}
	else
	{
		cerr << "Warning reference: "<<word<<" not found!\n"; 
		fileIn >> word;
		
		fileOut <<"section ?";

				
	};

	if(word.length() >= 7 && word.substr(0, 6) == "*/ref*") fileOut << word.substr(6) <<" "; 
	else if(!(word.length() >= 6 && word.substr(0, 6) == "*/ref*")) cerr << "Warning */ref* not found at end of reference: "<<s->second->name<<"!\n"; 
	else fileOut << " ";
};

void ProcessHtml::processFigRef(ifstream & fileIn, ostream & fileOut)
{
	string word, ref;
	fileIn >> word;

	map<string, string>::const_iterator f = figures.find(word);
	
	if(f != figures.end())
	{
		ref = f->second;	
		
	}
	else
	{
		cerr << "Warning figure reference: "<<word<<" not found!\n"; 		
		
		ref = "?";			
	};

	fileOut << ref;

	fileIn >> word;

	if(word.length() >= 10 && word.substr(0, 9) == "*/figref*") fileOut << word.substr(9) <<" "; 
	else if(!(word.length() >= 9 && word.substr(0, 9) == "*/figref*")) cerr << "Warning */figref* not found at end of figure reference: "<<ref<<"!\n"; 
	else fileOut << " ";
};

void ProcessHtml::processBold(string & word, ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("<b>", "</b>", fileIn, fileOut);
	if(start) fileOut << "<b>";
	else fileOut << "</b>";
	/*else
	{
		if(word.length() >= 5) fileOut << "</b> ";
		else fileOut << "</b>" << word.substr(4, 1) << " ";
	};*/
};

void ProcessHtml::processItalic(string & word, ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("<i>", "</i>", fileIn, fileOut);
	if(start) fileOut << "<i>";
	else fileOut << "</i>";
	/*else
	{
		if(word.length() >= 5) fileOut << "</i> ";
		else fileOut << "</i>" << word.substr(4, 1) << " ";
	};*/
};

void ProcessHtml::processUnderline(string & word, ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("<u>", "</u>", fileIn, fileOut);
	if(start) fileOut << "<u>";
	else fileOut << "</u>";

	/*else
	{
		if(word.length() >= 5) fileOut << "</u> ";
		else fileOut << "</u>" << word.substr(4, 1) << " ";
	};*/
};

void ProcessTex::processBold(string & word, ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("{\\bf ", "}", fileIn, fileOut);
	if(start) fileOut << "{\\bf ";
	else fileOut << "}";
	/*else
	{
		if(word.length() >= 5) fileOut << "} ";
		else fileOut << "}" << word.substr(4, 1) << " ";
	};*/
};

void ProcessTex::processItalic(string & word, ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("{\\it ", "}", fileIn, fileOut);
	if(start) fileOut << "{\\it ";
	else fileOut << "}";
	/*else
	{
		if(word.length() >= 5) fileOut << "} ";
		else fileOut << "}" << word.substr(4, 1) << " ";
	};*/
};

void ProcessTex::processUnderline(string & word, ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("\\underline{", "}", fileIn, fileOut);
	if(start) fileOut << "\\underline{";
	else fileOut << "}";

	/*else
	{
		if(word.length() >= 5) fileOut << "} ";
		else fileOut << "}" << word.substr(4, 1) << " ";
	};*/
};

void ProcessTex::processQuote(ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("``", "''", fileIn, fileOut);
	if(start) fileOut << "``";
	else fileOut << "''";
};

void ProcessHtml::processQuote(ifstream & fileIn, ostream & fileOut, bool start)
{
	//if(start) processBoldTypeCommand("&ldquo;", "&rdquo;", fileIn, fileOut);
	if(start) fileOut << "&ldquo;";
	else fileOut << "&rdquo;";
};

void ProcessHtml::processQuoteOneWord(string & word, ifstream & fileIn, ostream & fileOut)
{
	fileOut << "&ldquo;"<<word.substr(3, (word.length()-7))<<"&rdquo;";		
};

void ProcessTex::processQuoteOneWord(string & word, ifstream & fileIn, ostream & fileOut)
{
	fileOut << "``"<<word.substr(3, (word.length()-7))<<"''";		
};

void ProcessTex::processLatexFormula(string & word, ifstream & fileIn, ostream & fileOut)
{
	//string formula = getLatexFormula(word, fileIn, fileOut);

	fileOut << "$" << word << "$";
};

void ProcessHtml::processLatexFormula(string & word, ifstream & fileIn, ostream & fileOut)
{
	//string formula = getLatexFormula(word, fileIn, fileOut);

	//cout << word << "\n";
	//fileOut << "<img src=\"http://latex.codecogs.com/png.latex?\\inline "<<word<<"\">";
	fileOut << "<img src=\"http://latex.codecogs.com/png.latex?\\inline "<<word<<" \\small \" alt=\""<<word<<"\"/>";
	//fileOut << "<img src=\"http://latex.codecogs.com/png.latex?"<<word<<"\">";		
};

void ProcessHtml::processPercent(ifstream & fileIn, ostream & fileOut)
{
	fileOut << "%";
};

void ProcessTex::processPercent(ifstream & fileIn, ostream & fileOut)
{
	fileOut << "\\%";
};

void ProcessTex::processCite(ifstream & fileIn, ostream & fileOut, bool start)
{
	
	if(start)
	{
		string word;
		fileIn >> word;
		fileOut << "\\citet{"<<word;

	}
	else fileOut << "}";

};

void ProcessHtml::processCite(ifstream & fileIn, ostream & fileOut, bool start)
{

	if(start)
	{
		string word, ref;
		fileIn >> word;
		map<string, Citation *>::const_iterator c = citations.find(word);
		if(c != citations.end())
		{
			ref = c->second->refName;
		}
		else
		{
			ref = word;
			cerr << "Warning: citation "<<word<<" not found!\n";
		};

		fileOut << "<a href=\"references.html#"<<word<<"\">"<<ref<<"</a>";
		//fileOut << "<a href=\"references.html\">"<<ref<<"</a>";
	};
	
};

