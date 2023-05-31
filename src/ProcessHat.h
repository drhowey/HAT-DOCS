/*
  Richard Howey
  Research Software Engineering, Newcastle University
  HAT-DOCS: HTML and TeX documentation from one common source
*/

#ifndef __PROCESSHAT
#define __PROCESSHAT

#include <string>
#include <list>
#include <map>
#include <iostream>
#include <fstream>

//basic class for storing webpage info
struct Webpage
{
	string name;//name of section for labelling and html file names
	string title;
	
	Webpage(string na, string t) : name(na), title(t) {};

	~Webpage()
	{
		
	};
};

//basic class for storing info about a section
struct Citation
{
	string name;
	string title;
	string authors;
	string year;	
	string journal;
	string volume;
	string number;
	string pages;
	string url;
	string editor;
	string edition;
	string publisher;
	string refName;
	string orderName;
	string note;

	~Citation()
	{
		
	};

};

//basic class for storing info about a section
struct Section
{
	string number;//section number 1, 1.2 etc
	string name;//name of section for labelling and html file names
	string title;
	list<Section * > subsections;
	string nameUpperSection;
	bool newPageForSubsections;
	
	Section(string nu, string na, string t, list<Section * > ss, string nus, bool np) : number(nu), name(na), title(t), subsections(ss), nameUpperSection(nus), newPageForSubsections(np) {};

	~Section()
	{
		for(list<Section *>::iterator i = subsections.begin(); i != subsections.end(); ++i)
		{			
			delete *i;
		};
	};
};

//class used for processing the document, owns common methods for html and latex, e.g. sectioning
class ProcessHat
{
private:
	
protected:

	map<string, Section * > sections; //name, details
	list<Section * > orderedSections; //section in order
	map<string, string> figures; //figure ref name, figure name 
	list<string> filesCreated;
	list<Webpage * > orderedWebpages;
	string title;
	string subtitle;
	string date;
	string author;
	string address;
	string abstract;
	string styleFile;
	string logo;
	int logowidth;
	bool subSectionsOnNewPage;
	string bibFileName;
	bool processingWebpage;
	string texFileName;
	bool verbose;

public:

	ProcessHat(string & bfn, string tfn = "") : sections(), orderedSections(), figures(), filesCreated(), title(""), subtitle(""), author(""), address(""), styleFile("styles.css"), logo(""), logowidth(0), subSectionsOnNewPage(false), bibFileName(bfn), processingWebpage(false), texFileName(tfn) {};

	virtual ~ProcessHat()
	{
		for(list<Section *>::iterator i = orderedSections.begin(); i != orderedSections.end(); ++i)
		{			
			delete *i;
		};
	};


	virtual void process(string & filename);
	virtual void processWord(string & word, ifstream & fileIn, ostream & fileOut);
	void processFile(ifstream & fileIn, ofstream & fileOut);
	void processInputFile(ifstream & fileIn, ostream & fileOut);
	void processInput(ifstream & fileIn, ostream & fileOut);	
	virtual void getSectionNameAndTitle(ifstream & fileIn, ostream & fileOut, string & sectionName, string & sectionTitle) {};
	void getWebpageNameAndTitle(ifstream & fileIn, ostream & fileOut, string & webpageName, string & webpageTitle);
	void addSectionData(string & filename, ostream & fileOut, unsigned int & sectionCount, unsigned int & figureNo);
	void addWebpageData(string & filename, ostream & fileOut);
	void addTitleData(string & filename, ostream & fileOut);
	list<Section * > getSubsections(string & sectionUpperName, string & sectionNumberUpper, ifstream & fileIn, ostream & fileOut, bool & newPageForSubsections, unsigned int & figureNo, unsigned int subsectionCount = 1);
	list<Section * > getSubsubsections(string & sectionUpperUpperName, string & sectionNumberUpper, ifstream & fileIn, ostream & fileOut, unsigned int & figureNo, unsigned int subsubsectionCount = 1);
	string getSectionNumber(string & sectionNumber, unsigned int & sectionCount, unsigned int & sectionDepth);
	string getFigureNo(string & label);
	string getText(ifstream & fileIn, string endWord = "");
	void processComment(ifstream & fileIn);		
	void processBoldTypeCommand(const string & starting, const string & ending, ifstream & fileIn, ostream & fileOut);
	void processTheSection(string & sectionName, string & sectionTitle, ifstream & fileIn, ostream & fileOut, unsigned int depth);
	void displayCreatedFiles();
	void displayNoSections();
	void trimStartWord(string & word, ifstream & fileIn, ostream & fileOut);
	string trimEndWord(string & word, ifstream & fileIn, ostream & fileOut);
	pair<string, string> getLatexFormula(string & word, ifstream & fileIn, ostream & fileOut);
	string getCodeExample(ifstream & fileIn, ostream & fileOut);
	bool nextWordIsEndWord(ifstream & fileIn);

	virtual void processSection(ifstream & fileIn, ostream & fileOut, unsigned int depth) {};
	virtual void processWebpage(ifstream & fileIn, ostream & fileOut) {};
	virtual void startSection(ostream & fileOut, Section * section, unsigned int & depth) {};
	virtual void endSection(ostream & fileOut, Section * section, unsigned int & depth) {};
	virtual void startParagraph(ostream & fileOut) {};
	virtual void endParagraph(ostream & fileOut) {};
	virtual string getFileOutName(string & filename) {return "";};
	virtual void processHtml(ifstream & fileIn, ostream & fileOut) {};
	virtual void processTex(ifstream & fileIn, ostream & fileOut) {};
	virtual void processCode(ifstream & fileIn, ostream & fileOut, bool start) {};
	virtual void processBold(string & word, ifstream & fileIn, ostream & fileOut, bool start) {};
	virtual void processItalic(string & word, ifstream & fileIn, ostream & fileOut, bool start) {};
	virtual void processUnderline(string & word, ifstream & fileIn, ostream & fileOut, bool start) {};
	virtual void header(ifstream & fileIn, ostream & fileOut) {};
	virtual void footer(ifstream & fileIn, ostream & fileOut) {};
	virtual void menu(ifstream & fileIn, ostream & fileOut) {};
	virtual void contents(ifstream & fileIn, ostream & fileOut) {};	
	virtual void processCodeExample(ifstream & fileIn, ostream & fileOut) {};
	virtual void processCodeExampleSmall(ifstream & fileIn, ostream & fileOut) {};
	virtual void processTable(ifstream & fileIn, ostream & fileOut, const unsigned int & align = 1, const bool & scale = false) {};
	virtual void processFigure(ifstream & fileIn, ostream & fileOut) {};
	virtual void processList(ifstream & fileIn, ostream & fileOut, const bool & numList) {};
	virtual void processRef(ifstream & fileIn, ostream & fileOut) {};
	virtual void processFigRef(ifstream & fileIn, ostream & fileOut) {};
	virtual void processLatexFormula(string & word, ifstream & fileIn, ostream & fileOut) {};
	virtual void processQuote(ifstream & fileIn, ostream & fileOut, bool start) {};
	virtual void processQuoteOneWord(string & word, ifstream & fileIn, ostream & fileOut) {};
	virtual void processPercent(ifstream & fileIn, ostream & fileOut) {};
	virtual void processCite(ifstream & fileIn, ostream & fileOut, bool start) {};
	virtual void replaceSpecialChars(string & aString) {};
};

//a class for producing the html files
class ProcessHtml : public ProcessHat
{
private:
	
	map<string, Citation *> citations;
	string footerFileName;

public:

	ProcessHtml(string & bfn, string & ffn, const bool & ver) : ProcessHat(bfn), footerFileName(ffn) {verbose = ver;};

	virtual ~ProcessHtml()
	{
		for(map<string, Citation *>::iterator c = citations.begin(); c != citations.end(); ++c)
		{
			delete c->second;
		};
	};

	void process(string & filename);
	void processWord(string & word, ifstream & fileIn, ostream & fileOut, bool replaceChars = true);
	void getSectionNameAndTitle(ifstream & fileIn, ostream & fileOut, string & sectionName, string & sectionTitle);
	void addReferencesWebpage(ifstream & fileIn, ostream & fileOut);
	void addCitation(const string & citeName);
	void addReferences(string & filename, ostream & fileOut);
	string getFileOutName(string & filename);
	void processSection(ifstream & fileIn, ostream & fileOut, unsigned int depth);
	void processWebpage(ifstream & fileIn, ostream & fileOut);
	void startSection(ostream & fileOut, Section * section, unsigned int & depth);
	void endSection(ostream & fileOut, Section * section, unsigned int & depth);
	void addNextAndPrev(ostream & fileOut, Section * section);
	void startParagraph(ostream & fileOut);
	void endParagraph(ostream & fileOut);
	void processHtml(ifstream & fileIn, ostream & fileOut);
	void processTex(ifstream & fileIn, ostream & fileOut);
	void header(ifstream & fileIn, ostream & fileOut);
	void footer(ifstream & fileIn, ostream & fileOut);
	void menu(ifstream & fileIn, ostream & fileOut);
	void contents(ifstream & fileIn, ostream & fileOut);
	void processCode(ifstream & fileIn, ostream & fileOut, bool start);
	void processCodeExample(ifstream & fileIn, ostream & fileOut);
	void processCodeExampleSmall(ifstream & fileIn, ostream & fileOut);
	void processTable(ifstream & fileIn, ostream & fileOut, const unsigned int & align = 1, const bool & scale = false);
	void processFigure(ifstream & fileIn, ostream & fileOut);
	void processList(ifstream & fileIn, ostream & fileOut, const bool & numList);
	void processRef(ifstream & fileIn, ostream & fileOut);
	void processFigRef(ifstream & fileIn, ostream & fileOut);
	void processBold(string & word, ifstream & fileIn, ostream & fileOut, bool start);
	void processItalic(string & word, ifstream & fileIn, ostream & fileOut, bool start);
	void processUnderline(string & word, ifstream & fileIn, ostream & fileOut, bool start);
	void processLatexFormula(string & word, ifstream & fileIn, ostream & fileOut);
	void processQuote(ifstream & fileIn, ostream & fileOut, bool start);
	void processQuoteOneWord(string & word, ifstream & fileIn, ostream & fileOut);
	void processPercent(ifstream & fileIn, ostream & fileOut);
	void processCite(ifstream & fileIn, ostream & fileOut, bool start);
	void replaceSpecialChars(string & aString);

	void addFooterText(ostream & fileOut);
};

class ProcessTex : public ProcessHat
{
private:
	
	

public:

	ProcessTex(string & bfn, string & tfn, const bool & ver) : ProcessHat(bfn, tfn) {verbose = ver;};

	virtual ~ProcessTex()
	{
		
	};

	string getFileOutName(string & filename);
	void processSection(ifstream & fileIn, ostream & fileOut, unsigned int depth);
	void startSection(ostream & fileOut, Section * section, unsigned int & depth);
	void getSectionNameAndTitle(ifstream & fileIn, ostream & fileOut, string & sectionName, string & sectionTitle);
	void processWebpage(ifstream & fileIn, ostream & fileOut);
	void endSection(ostream & fileOut, Section * section, unsigned int & depth);
	void startParagraph(ostream & fileOut);
	void endParagraph(ostream & fileOut);
	void processHtml(ifstream & fileIn, ostream & fileOut);
	void processTex(ifstream & fileIn, ostream & fileOut);
	void header(ifstream & fileIn, ostream & fileOut);
	void footer(ifstream & fileIn, ostream & fileOut);
	void contents(ifstream & fileIn, ostream & fileOut) {};
	void processCode(ifstream & fileIn, ostream & fileOut, bool start);
	void processCodeExample(ifstream & fileIn, ostream & fileOut);
	void processCodeExampleSmall(ifstream & fileIn, ostream & fileOut);
	void processTable(ifstream & fileIn, ostream & fileOut, const unsigned int & align = 1, const bool & scale = false);
	void processFigure(ifstream & fileIn, ostream & fileOut);
	void processList(ifstream & fileIn, ostream & fileOut, const bool & numList);
	void processRef(ifstream & fileIn, ostream & fileOut);
	void processFigRef(ifstream & fileIn, ostream & fileOut);
	void processBold(string & word, ifstream & fileIn, ostream & fileOut, bool start);
	void processItalic(string & word, ifstream & fileIn, ostream & fileOut, bool start);
	void processUnderline(string & word, ifstream & fileIn, ostream & fileOut, bool start);
	void processLatexFormula(string & word, ifstream & fileIn, ostream & fileOut);
	void processQuote(ifstream & fileIn, ostream & fileOut, bool start);
	void processQuoteOneWord(string & word, ifstream & fileIn, ostream & fileOut);
	void processPercent(ifstream & fileIn, ostream & fileOut);
	void processCite(ifstream & fileIn, ostream & fileOut, bool start);
};

#endif
