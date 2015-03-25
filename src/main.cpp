#include <fstream>
#include <string>

#include <prefixer.h>

using namespace boolean_parser;

//Example use of string parser, for 
//more information see prefixer.h
int main(int argc, char* argv[]) { 
    
    if(argc != 2)
    {
		std::cout << argv[0] << " <file>\n";
		return 1;
    }
	//Open a file and read a line
    ifstream ifs(argv[1],ios::in); 	
	if(!ifs)
	{
		ifs.close();
		cerr<<"Failed to open file" << endl;
		return 1; //Error
	}
    string query;
	std::getline(ifs,query);	
	ifs.close();
	try {
    	abstract_syntax_tree ast(query);
		ast.simplify();
		ast.repr(std::cout);
		std::cout << endl;
	} 
	catch (parsing_exception* e) {	
		cerr << "Parse error" << endl;
		return 1;	
	}
   return 0; 
}

